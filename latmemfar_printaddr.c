#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#ifndef RUN_IN_USERSPACE
#include <sifive/devices/ccache.h>
#include <sifive/platform.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#endif

// Pointer chasing macros to force the loop to be unwound
#define CHASE1(x) ((uintptr_t *)*x)
#define CHASE2(x) CHASE1(CHASE1(x))
#define CHASE4(x) CHASE2(CHASE2(x))
#define CHASE8(x) CHASE4(CHASE4(x))
#define CHASE16(x) CHASE8(CHASE8(x))
#define CHASE32(x) CHASE16(CHASE16(x))
#define CHASE64(x) CHASE32(CHASE32(x))
#define CHASE128(x) CHASE64(CHASE64(x))
#define CHASE256(x) CHASE128(CHASE128(x))
#define CHASE512(x) CHASE256(CHASE256(x))
#define CHASE1024(x) CHASE512(CHASE512(x))
#define CHASE_STEPS 1024

static uintptr_t rdcycle()
{
  uintptr_t out;
  __asm__ __volatile__ ("rdcycle %0" : "=r"(out));
  return out;
}

#if 0
uintptr_t *chase(uintptr_t *x, long* cycles) {
  uintptr_t start = rdcycle();
  asm volatile("" ::: "memory");
  uintptr_t* out = CHASE1024(x);
  asm volatile("" ::: "memory");
  uintptr_t end = rdcycle();
  *cycles = end - start;
  return out;
}
#else
uintptr_t *chase(uintptr_t *x, long* cycles) {
  int i;
  uintptr_t * val = x;

  uintptr_t start = rdcycle();
  asm volatile("" ::: "memory");

  for(i=0; i<CHASE_STEPS; i++) {
	printf("\r%p", val);
	val = (uintptr_t*)*val;
  }
  uintptr_t* out = val;
//  uintptr_t* out = CHASE1024(x);
  asm volatile("" ::: "memory");
  uintptr_t end = rdcycle();
  *cycles = end - start;
  return out;
}
#endif

#ifndef TEST_SIZE
#define TEST_SIZE (16*1024*1024)
#endif

// To prevent homonyms on a RISC-V for a VIPT system with virtual memory,
// there are never any set collisions under the 4kB MMU page-size.
// Therefore, you if accesss memory in 4kB stride, it's the same as touching
// the memory with unit stride.
#define STRIDE 4096

uintptr_t *bigarray;

int main(int argc, char ** argv) {
  long test_size;
  long test_range = TEST_SIZE;
  long unsigned offset = 0x100000000UL;
  int stride = STRIDE;

  if ( argc < 2 ) {
	  printf("Usage: %s [offset, default=0x100000000] [MAX Test Size in kB: ex. 16384 (=16MB)] [stride (default=4096)]\n", argv[0]);
  } 
  if ( argc >= 2 ) {
	offset = strtoll(argv[1], NULL, 0);
  }
  if ( argc >= 3 ) {
	  test_range = atol(argv[2]) * 1024;
  }
  if (argc >= 4 ) {
	  stride = atoi(argv[3]);
  }

#ifndef RUN_IN_USERSPACE
  long ways = ccache_ways(CCACHE_CTRL_ADDR);
  // Enable all available ways in the L2
  ccache_enable_ways(CCACHE_CTRL_ADDR, ways-1);
#else
  /* FIXME: There's probably a correct way to do this. */
//  long ways = 16;
  long ways = 4;
#endif

  int fd = open("/dev/mem", O_RDWR);
  bigarray = mmap(0, test_range, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);

  for (test_size = stride; test_size <= test_range; test_size <<= 1) {
    long i, j, n, delta;
    long sum, sum2;
    uintptr_t *x = &bigarray[0];

    // Create a pointer loop
    i = 0;
    do {
#ifndef RANDOM
      j = (i + stride) & (test_size-1);
#endif
      bigarray[i/sizeof(uintptr_t)] = (uintptr_t)&bigarray[j/sizeof(uintptr_t)];
      i = j;
    } while (i != 0);

    // We need to chase the point test_size/STRIDE steps to exercise the loop.
    // Each invocation of chase performs CHASE_STEPS, so round-up the calls.
    // To warm a cache with random replacement, you need to walk it 'ways' times.
    n = (((test_size / stride) * ways + (CHASE_STEPS-1)) / CHASE_STEPS);
    if (n < 5) n = 5; // enough to compute variance to within 50% = 1/sqrt(n-1)

    // Warm the cache test
    for (i = 0; i < n; ++i) x = chase(x, &delta);

    // Perform iterations for real
    sum = 0;
    sum2 = 0;
    for (i = 0; i < n; ++i) {
      x = chase(x, &delta);
      sum += delta;
      sum2 += delta*delta;
    }

    double mean = sum / (1.0*n*CHASE_STEPS);
    // This is tricky. The sum2 and sum are actually measuring the random variable
    // which is a CHASE_STEPS* sum of the real random variable. To find the variance
    // of the underlying distribution, we need to multiply by sqrt(CHASE_STEPS).
    // We also need to divide by CHASE_STEPS to scale the result.
    double varDelta = (1.0*sum2 - 1.0*sum*sum/n) / (n-1);
    double var = varDelta / sqrt(CHASE_STEPS);
    printf("%ld %.3lf %.3lf %ld\n", test_size, mean, sqrt(var), n);
  }

  //unmap
  munmap(bigarray, test_range);

  // Test successful
  return 0;
}

// Provide missing symbol
int __errno;

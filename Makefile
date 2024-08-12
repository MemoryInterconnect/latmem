#CC=riscv64-buildroot-linux-gnu-gcc
CC=gcc
CFLAGS=-Wall -O2 -DRUN_IN_USERSPACE
LDFLAGS=-lm -static
TARGET=latmem latmemfar

all: $(TARGET)

latmem.o: latmem.c
	$(CC) $(CFLAGS) -c -o latmem.o $<

latmemfar.o: latmem.c
	$(CC) $(CFLAGS) -DDEVMEM -c -o latmemfar.o $<

latmem: latmem.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

latmemfar: latmemfar.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	@echo $(PATH)
	rm -rf *.o $(TARGET)

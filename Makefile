CC=riscv64-buildroot-linux-gnu-gcc
#CC=gcc
CFLAGS=-Wall -O2 -DRUN_IN_USERSPACE
LDFLAGS=-lm -static
TARGET=latmem latmemfar latmemfar_stride latmemfar_printaddr

all: $(TARGET)

latmem.o: latmem.c
	$(CC) $(CFLAGS) -c -o $@ $<

latmemfar.o: latmemfar.c
	$(CC) $(CFLAGS) -c -o $@ $< 

latmemfar_stride.o: latmemfar_stride.c
	$(CC) $(CFLAGS) -c -o $@ $< 

latmemfar_printaddr.o: latmemfar_printaddr.c
	$(CC) $(CFLAGS) -c -o $@ $< 

latmem: latmem.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

latmemfar: latmemfar.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

latmemfar_stride: latmemfar_stride.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

latmemfar_printaddr: latmemfar_printaddr.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	@echo $(PATH)
	rm -rf *.o $(TARGET)

CC=riscv64-buildroot-linux-gnu-gcc
#CC=gcc
CFLAGS=-Wall -O2 -DRUN_IN_USERSPACE
LDFLAGS=-lm -static
TARGET=latmem latmem_write latmemfar latmemfar_write #latmemfar_stride latmemfar_printaddr

all: $(TARGET)

latmem.o: latmem.c
	$(CC) $(CFLAGS) -c -o $@ $<

latmem_write.o: latmem_write.c
	$(CC) $(CFLAGS) -c -o $@ $<

latmemfar.o: latmemfar.c
	$(CC) $(CFLAGS) -c -o $@ $< 

latmemfar_write.o: latmemfar_write.c
	$(CC) $(CFLAGS) -c -o $@ $< 

#latmemfar_stride.o: latmemfar_stride.c
#	$(CC) $(CFLAGS) -c -o $@ $< 

#latmemfar_printaddr.o: latmemfar_printaddr.c
#	$(CC) $(CFLAGS) -c -o $@ $< 

latmem: latmem.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

latmem_write: latmem_write.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

latmemfar: latmemfar.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

latmemfair_write: latmemfar_write.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

#latmemfar_stride: latmemfar_stride.o
#	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

#latmemfar_printaddr: latmemfar_printaddr.o
#	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	@echo $(PATH)
	rm -rf *.o $(TARGET)

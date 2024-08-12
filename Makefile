#CC=riscv64-buildroot-linux-gnu-gcc
CC=gcc
CFLAGS=-Wall -O2 -DRUN_IN_USERSPACE
LDFLAGS=-lm -static
TARGET=latmem

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

latmem: latmem.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	@echo $(PATH)
	rm -rf *.o $(TARGET)

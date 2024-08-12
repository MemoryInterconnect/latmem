CC=riscv64-buildroot-linux-gnu-gcc
CFLAGS=-Wall -O2 -DRUN_IN_USERSPACE
LDFLAGS=-lm
TARGET=latmem

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

latmem: latmem.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	@echo $(PATH)
	rm -rf *.o $(TARGET)

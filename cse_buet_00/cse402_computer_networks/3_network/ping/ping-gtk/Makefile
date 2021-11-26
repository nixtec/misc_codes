CC	:= gcc
CFLAGS	:= -g -Wall
GCFLAGS	:= $(CFLAGS)

TARGET	:= test sticky-test

.PHONY: all clean

all: $(TARGET)

sticky-test: sticky-test.c Makefile
	$(CC) -o $@ sticky-test.c $(CFLAGS); \
	sudo chown root:root $@; \
	sudo chmod 4755 $@
clean:
	rm -f $(TARGET)

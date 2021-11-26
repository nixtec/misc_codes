#
# makefile for protocol6
# selective repeat protocol
#

CC              := gcc
#CFLAGS          := -D_PROTOCOL6_DEBUG_ -g -Wall -D_REENTRANT
CFLAGS          := -D_PROTOCOL6_DEBUG_ -O3 -Wall -D_REENTRANT
LDFLAGS         := -lpthread

TARGET          := protocol6

all: $(TARGET)

$(TARGET): protocol6.o crc16.o support.o
	$(CC) -o $@ $^ $(LDFLAGS)

protocol6.o: protocol6.c protocol6.h
	$(CC) -c protocol6.c $(CFLAGS)

crc16.o: crc16.c crc16.h
	$(CC) -c crc16.c $(CFLAGS)

support.o: support.c support.h
	$(CC) -c support.c $(CFLAGS)

clean:
	-rm -f $(TARGET) *.o

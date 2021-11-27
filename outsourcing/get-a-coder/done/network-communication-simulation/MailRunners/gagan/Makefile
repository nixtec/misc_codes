# Makefile
CC		:= gcc
CFLAGS		:= -ansi -g -Wall
LDFLAGS		:=
FILES		:= client.c server.c comm_man.c comm_man.h Makefile
TARGETS		:= server client

all: $(TARGETS)

comm_man.o: comm_man.c comm_man.h
	$(CC) $(CFLAGS) -c -o $@ comm_man.c

sec_man.o: sec_man.c sec_man.h
	$(CC) $(CFLAGS) -c -o $@ sec_man.c

server: server.c comm_man.o sec_man.o
	$(CC) $(CFLAGS) -o $@ $^

client: client.c comm_man.o sec_man.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS) *.o

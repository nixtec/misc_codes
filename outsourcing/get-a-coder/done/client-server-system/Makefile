CC		:= gcc
CFLAGS		:= -g -Wall
LDFLAGS		:=
TARGETS		:= server client

all: $(TARGETS)

server: server.c server.h

client: client.c client.h

.PHONY: clean

clean:
	rm -f $(TARGETS)

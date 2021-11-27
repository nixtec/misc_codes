# Makefile
CC		:= gcc
CFLAGS		:= -ansi -g -Wall
LDFLAGS		:=
FILES		:= monitorsim.c monitorsim.h Makefile input.txt
TARGETS		:= monitorsim

all: $(TARGETS)

monitorsim: monitorsim.c monitorsim.h
	$(CC) $(CFLAGS) -o $@ monitorsim.c $(LDFLAGS)

clean:
	rm -f $(TARGETS) *.o

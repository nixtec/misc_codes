# Makefile for simple network controller simulation
CC		:= gcc
CFLAGS		:= -g -Wall -ansi -finline-functions -O2
#CFLAGS		:= -g -Wall -O2
LDFLAGS		:=
SRCS		:= snc.c
OBJS		:= $(patsubst %.c,%.o,$(SRCS))
TARGET		:= snc
DIST		:= snc-1.0
EXT		:= tar.gz
FILES		:= Makefile snc.c snc.h \
			snc.in.txt snc.out.txt input.txt output.txt \
			README AUTHORS INSTALL design-implementation.pdf \
			snc.cproto.txt snc.jpg

all: $(TARGET)

.PHONY: clean dist


snc: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

snc.o: snc.c snc.h
	$(CC) $(CFLAGS) -c -o $@ snc.c

snc.out.txt: snc snc.in.txt
	./snc snc.in.txt snc.out.txt

output.txt: snc input.txt
	./snc input.txt output.txt

snc.cproto.txt: snc.c
	cproto snc.c > snc.cproto.txt

dist: $(FILES)
	mkdir -p $(DIST)
	cp $(FILES) $(DIST)
	tar -czf $(DIST).$(EXT) $(DIST)
	rm -rf $(DIST)

distclean:
	rm -f $(DIST).$(EXT)

clean:
	rm -f $(OBJS) $(TARGET) snc.cproto.txt snc.out.txt output.txt

# Makefile for distributed system simulator
CC		:= gcc
CFLAGS		:= -g -Wall -ansi -finline-functions -O2
#CFLAGS		:= -g -Wall -O2
LDFLAGS		:=
SRCS		:= dsfuncs.c
OBJS		:= $(patsubst %.c,%.o,$(SRCS))
TARGET		:= dsfuncs
DIST		:= dsfuncs-1.1
EXT		:= tar.gz
FILES		:= Makefile dsfuncs.c dsfuncs.h README AUTHORS INSTALL

all: $(TARGET)

.PHONY: clean dist distclean


dsfuncs: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

dsfuncs.o: dsfuncs.c dsfuncs.h
	$(CC) $(CFLAGS) -c -o $@ dsfuncs.c

dist: $(FILES)
	mkdir -p $(DIST)
	cp $(FILES) $(DIST)
	tar -czf $(DIST).$(EXT) $(DIST)
	rm -rf $(DIST)

zip: $(FILES)
	mkdir -p $(DIST)
	cp $(FILES) $(DIST)
	zip -r $(DIST).zip $(DIST)
	rm -rf $(DIST)

distclean:
	rm -f $(DIST).$(EXT)

clean:
	rm -f $(OBJS) $(TARGET)

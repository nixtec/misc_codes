# Makefile for project
CC		:= gcc
CFLAGS		:= -g -Wall -ansi -finline-functions -O2
#CFLAGS		:= -g -Wall -O2
LDFLAGS		:=
SRCS		:= $(wildcard *.c)
HEADERS 	:= $(wildcard *.h)
OBJS		:= $(patsubst %.c,%.o,$(SRCS))
TARGET		:= project
VERSION 	:= 1.0
DIST		:= project-$(VERSION)
EXT		:= tar.gz
ZIPEXT		:= zip
FILES		:= Makefile $(SRCS) $(HEADERS) README AUTHORS INSTALL

all: $(TARGET)

.PHONY: clean dist distclean


project: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

project.o: project.c project.h
	$(CC) $(CFLAGS) -c -o $@ project.c

dist: $(FILES)
	mkdir -p $(DIST)
	cp $(FILES) $(DIST)
	tar -czf $(DIST).$(EXT) $(DIST)
	rm -rf $(DIST)

zip: $(FILES)
	mkdir -p $(DIST)
	cp $(FILES) $(DIST)
	zip -r $(DIST).$(ZIPEXT) $(DIST)
	rm -rf $(DIST)

distclean:
	rm -f $(DIST).$(EXT) $(DIST).zip

clean:
	rm -f $(OBJS) $(TARGET)

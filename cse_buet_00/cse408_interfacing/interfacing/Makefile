#
# Makefile for compiling the project
# by Ayub <mrayub@gmail.com>
#
CC		:= gcc
CFLAGS		:= -O2 -Wall -I.
DEBUG_FLAGS	:= -D_DEBUG_ -D_NO_AUDIO_
AUDIO_LDFLAGS	:= -lao -lm
LDFLAGS		:= -L. -lparapin

#TARGETS		:= project project.static
TARGETS		:= project project.debug project.noaudio

all: $(TARGETS)

project: project.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(AUDIO_LDFLAGS)

project.static: project.c
	$(CC) $(CFLAGS) -static -o $@ $^ $(LDFLAGS)

project.noaudio: project.c
	$(CC) $(CFLAGS) -D_NO_AUDIO_ -o $@ $^ $(LDFLAGS)

project.debug: project.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGETS)

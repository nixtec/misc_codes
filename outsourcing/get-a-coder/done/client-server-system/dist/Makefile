#
# Makefile for project [linux environment]
# Use: make
#
# if you want optimizations, you can enable -O2 flag
# for enable debugging, enable -g flag
#
INSTDIR		:= /usr/local/bin
#CFLAGS		:= -D_REENTRANT -g -Wall -finline-functions -pthread -O2
#CFLAGS		:= -D_REENTRANT -Wall -finline-functions -pthread -O2
CFLAGS		:= -Wall -finline-functions
SCFLAGS		:= -D_REENTRANT -pthread $(CFLAGS)
CCFLAGS		:= $(CFLAGS)
SLDFLAGS	:= -lpthread -lcrypt
CLDFLAGS	:=
CXXFLAGS	:= ${CFLAGS}
CC		:= gcc
CXX		:= g++
SERVER_SRCS	:= server.c proc-download.c proc-upload.c proc-move.c \
			proc-none.c proc-exec.c support.c exec_cmd.c \
			cmdmap.c authenticate.c pthread.c
SERVER_OBJS	:= $(patsubst %.c,%.o,$(SERVER_SRCS))

SHELL_SRCS	:= shell.c functions.c shell-support.c parser.c getpass.c \
			oshacks.c cwdent.c
SHELL_OBJS	:= $(patsubst %.c,%.o,$(SHELL_SRCS))

TARGETS		:= server shell
WTARGETS	:= $(patsubst %,%.exe,$(TARGETS))

DISTFILES	:= $(wildcard *.c) $(wildcard *.h)
DISTFILES	+= AUTHORS INSTALL README \
		   build.win32.bat_ build.win32.sh cmdenv.bat_ \
		   cmdmap.conf commands.txt error-codes.txt protocol.txt \
		   Makefile Makefile.win32
DISTNAME	:= cs
DISTVER		:= 1.0
DISTDIR		:= $(DISTNAME)-$(DISTVER)
DISTARC		:= $(DISTDIR).tar.gz


.PHONY: clean install static

all: $(TARGETS)

server: $(SERVER_OBJS)
	$(CC) $(SCFLAGS) -o server $(SERVER_OBJS) $(SLDFLAGS)
#	strip server

static: $(SERVER_OBJS) $(SHELL_OBJS)
	$(CC) -static -o server.static $(SERVER_OBJS) $(SLDFLAGS)
	$(CC) -static -o shell.static $(SHELL_OBJS) $(CLDFLAGS)

proc-move.o: proc-move.c server-funcs.h
	$(CC) $(SCFLAGS) -c proc-move.c

proc-none.o: proc-none.c server-funcs.h
	$(CC) $(SCFLAGS) -c proc-none.c

proc-exec.o: proc-exec.c server-funcs.h
	$(CC) $(SCFLAGS) -c proc-exec.c

proc-download.o: proc-download.c server-funcs.h
	$(CC) $(SCFLAGS) -c proc-download.c

proc-upload.o: proc-upload.c server-funcs.h
	$(CC) $(SCFLAGS) -c proc-upload.c

server.o: server.c server.h server-funcs.h
	$(CC) $(SCFLAGS) -c server.c

support.o: support.h support.c
	$(CC) $(SCFLAGS) -c support.c

exec_cmd.o: exec_cmd.c exec_cmd.h exec_cmd.unix.c
	$(CC) $(SCFLAGS) -c exec_cmd.c

authenticate.o: authenticate.c authenticate.h
	$(CC) $(SCFLAGS) -c authenticate.c

cmdmap.o: cmdmap.c cmdmap.h
	$(CC) $(SCFLAGS) -c cmdmap.c

pthread.o: pthread.c pthread.h
	$(CC) $(SCFLAGS) -c pthread.c

shell: $(SHELL_OBJS)
	$(CC) $(CCFLAGS) -o shell $(SHELL_OBJS) $(CLDFLAGS)
#	strip shell

shell.o: shell.c functions.h
	$(CC) $(CCFLAGS) -c shell.c

functions.o: functions.c functions.h
	$(CC) $(CCFLAGS) -c functions.c

parser.o: parser.c parser.h
	$(CC) $(CCFLAGS) -c parser.c

shell-support.o: shell-support.c
	$(CC) $(CCFLAGS) -c shell-support.c

getpass.o: getpass.c
	$(CC) $(CCFLAGS) -c getpass.c

oshacks.o: oshacks.c oshacks.h
	$(CC) $(CCFLAGS) -c oshacks.c

cwdent.o: cwdent.c cwdent.h
	$(CC) $(CCFLAGS) -c cwdent.c


install: server shell
	cp -f server shell $(INSTDIR)

dist: $(DISTFILES)
	mkdir -p $(DISTDIR)
	cp $(DISTFILES) $(DISTDIR)
	tar -czf $(DISTARC) $(DISTDIR) && rm -rf $(DISTDIR)

distclean:
	rm -f $(DISTARC)

uninstall:
	rm -f $(INSTDIR)/server $(INSTDIR)/shell

clean:
	-rm -f *.o $(TARGETS) $(SERVER_OBJS)

# Makefile
CC 		:= gcc
CFLAGS 		:= -Wall
LDFLAGS 	:=
OBJECTS		:= fileManager.o mailrunner6.o SecMngr.o
TARGETS 	:= mailrunner6

#all: $(TARGETS)

mailrunner6 : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

mailrunner6.o : mailrunner6.c mailrunner6.h fileManager.o
	$(CC) $(CFLAGS) -c -o $@ mailrunner6.c

fileManager.o : fileManager.c fileManager.h SecMngr.o
	$(CC) $(CFLAGS) -c -o $@ fileManager.c

SecMngr.o : SecMngr.c SecMngr.h
	$(CC) $(CFLAGS) -c -o $@ SecMngr.c	

clean :
	rm -f $(TARGETS) *.o


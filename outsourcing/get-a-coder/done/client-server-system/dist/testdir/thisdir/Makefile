CC		:= gcc
CFLAGS		:= -g -Wall
LDFLAGS		:=

all: lockdemo-mt

lockdemo-mt: lockdemo-mt.c
	$(CC) -D_REENTRANT -o lockdemo-mt lockdemo-mt.c -lpthread

test: test.c getpasswin.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f test getpasswin.o

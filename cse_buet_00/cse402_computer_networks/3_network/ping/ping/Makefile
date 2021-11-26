# Makefile
# Written by Ayub <mrayub@gmail.com>
CC	:= gcc
#CFLAGS	:= -g -Wall -O2 -DINTERVAL=2 -DTIMEOUT=5
CFLAGS	:= -Wall -O2
LDFLAGS	:=

TARGET	:= myping4
EXTRA	:= README AUTHORS blockping.sh unblockping.sh

.PHONY: all clean dist

all: $(TARGET)

$(TARGET): myping4.c myping4.h Makefile
	$(CC) $(CFLAGS) -o $@ myping4.c $(LDFLAGS)
	sudo chown root:wheel $@
	sudo chmod 4755 $@

clean:
	rm -f $(TARGET) myping4.tgz

dist:
	touch $(EXTRA); \
	tar -czf myping4.tgz myping4.c myping4.h Makefile $(EXTRA)

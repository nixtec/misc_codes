# Makefile for the Mini Shell
AS		:= as
LD		:= ld
#ASFLAGS		:= -gstabs
ASFLAGS		:=
LDFLAGS		:= -s
SOURCES		:= $(wildcard *.s)
OBJECTS		:= $(patsubst %.s,%.o,$(SOURCES))
TARGETS		:= mshell
#LDFLAGS		:=

all: $(TARGETS)

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

$(TARGETS): $(OBJECTS)
	$(LD) ${LDFLAGS} -o $@ $^

.PHONY: clean distclean compile

compile: $(OBJECTS)

clean:
	-rm -f *.o

distclean: clean
	-rm -f $(TARGETS)

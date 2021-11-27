## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-Wall \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = licensing

licensing_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	myfuncs.c myfuncs.h

licensing_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -lcrypto


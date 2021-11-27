## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-Wall -D_USE_BIOS_DATE \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@ @SQLITE3_CFLAGS@

bin_PROGRAMS = attendence

attendence_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	myfuncs.c myfuncs.h \
	mysdb.c mysdb.h \
	mybdb.c mybdb.h

attendence_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) @SQLITE3_LIBS@ -ldb -lcrypto


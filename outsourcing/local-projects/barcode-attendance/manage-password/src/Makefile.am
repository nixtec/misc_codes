## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-Wall \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@ @SQLITE3_CFLAGS@

bin_PROGRAMS = manage-password

manage_password_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	mysdb.c mysdb.h

manage_password_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) @SQLITE3_LIBS@


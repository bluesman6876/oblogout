## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(OBLOGOUT_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g3

bin_PROGRAMS = oblogout

oblogout_SOURCES = \
	action.c    \
	action.h    \
	logout.c    \
	setting.c    \
	setting.h

#oblogout_CFLAGS = \
#	-Wall\
#	-g3

oblogout_LDFLAGS = 

oblogout_LDADD = \
	$(OBLOGOUT_LIBS)


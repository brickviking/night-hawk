# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.3 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

############################################################################
# nighthawk sound engine
############################################################################


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DISTDIR =

pkgdatadir = $(datadir)/nighthawk
pkglibdir = $(libdir)/nighthawk
pkgincludedir = $(includedir)/nighthawk

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = i686-pc-linux-gnu
host_triplet = i686-pc-linux-gnu
ADD_DEFS =  -DINSTALL_DIR="\"$(INSTALL_DIR)\"" -DSCORES_FILE="\"$(SCORES_FILE)\""
CC = gcc
CXX = c++
CXXINCLUDES = 
INCLUDES = -I$(top_builddir) -I$(top_srcdir)
INSTALL_DIR = $(pkgdatadir)
MAKEINFO = makeinfo
NIGHTHAWK_SOUND = nighthawk_sound
NIGHTHAWK_TOOLS = 
PACKAGE = nighthawk
SCORES_FILE = .nighthawk.scores
VERSION = 2.2
X_ALL_LIBS = -lXext  -lSM -lICE -lX11  
X_EXT_LIBS = -lXext  -lSM -lICE -lX11  
X_PLAIN_LIBS =  -lSM -lICE -lX11 

DEFS = -DHAVE_CONFIG_H $(ADD_DEFS) $(HOSTDEF)

libexec_PROGRAMS = nighthawk_sound
EXTRA_PROGRAMS = nighthawk_sound
EXTRA_DIST = COPYING.FunktrackerGOLD Make.common README README.FunktrackerGOLD
CLEANFILES = host_safe

nighthawk_sound_SOURCES = \
dsp_mixxer.c \
dsp_mixxer.h \
funkload.c \
funkload.h \
funktracker.c \
funktracker.h \
funktracker_defs.h \
nighthawk_sound.c \
nighthawk_sound.h
nighthawk_sound_DEPENDENCIES = host_safe
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
PROGRAMS =  $(libexec_PROGRAMS)

CPPFLAGS = 
LDFLAGS = 
LIBS = -lm 
X_CFLAGS =  -I/usr/X11R6/include
X_LIBS =  -L/usr/X11R6/lib
X_EXTRA_LIBS = 
X_PRE_LIBS =  -lSM -lICE
nighthawk_sound_OBJECTS =  dsp_mixxer.o funkload.o funktracker.o \
nighthawk_sound.o
nighthawk_sound_LDADD = $(LDADD)
nighthawk_sound_LDFLAGS = 
CFLAGS = -g -O2
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC) $(CFLAGS) $(LDFLAGS) -o $@
DIST_COMMON =  README Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
SOURCES = $(nighthawk_sound_SOURCES)
OBJECTS = $(nighthawk_sound_OBJECTS)

all: Makefile $(PROGRAMS)

.SUFFIXES:
.SUFFIXES: .S .c .o .s
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps src_sound/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libexecPROGRAMS:

clean-libexecPROGRAMS:
	-test -z "$(libexec_PROGRAMS)" || rm -f $(libexec_PROGRAMS)

distclean-libexecPROGRAMS:

maintainer-clean-libexecPROGRAMS:

install-libexecPROGRAMS: $(libexec_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libexecdir)
	@list='$(libexec_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "  $(INSTALL_PROGRAM) $$p $(DESTDIR)$(libexecdir)/`echo $$p|sed '$(transform)'`"; \
	     $(INSTALL_PROGRAM) $$p $(DESTDIR)$(libexecdir)/`echo $$p|sed '$(transform)'`; \
	  else :; fi; \
	done

uninstall-libexecPROGRAMS:
	@$(NORMAL_UNINSTALL)
	list='$(libexec_PROGRAMS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(libexecdir)/`echo $$p|sed '$(transform)'`; \
	done

.c.o:
	$(COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

nighthawk_sound: $(nighthawk_sound_OBJECTS) $(nighthawk_sound_DEPENDENCIES)
	@rm -f nighthawk_sound
	$(LINK) $(nighthawk_sound_LDFLAGS) $(nighthawk_sound_OBJECTS) $(nighthawk_sound_LDADD) $(LIBS)

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $(SOURCES) $(HEADERS) $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = src_sound

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
dsp_mixxer.o: dsp_mixxer.c funktracker_defs.h ../options.h ../config.h \
	funktracker.h funkload.h
funkload.o: funkload.c funktracker_defs.h ../options.h ../config.h \
	dsp_mixxer.h funktracker.h
funktracker.o: funktracker.c funktracker_defs.h ../options.h ../config.h \
	dsp_mixxer.h
nighthawk_sound.o: nighthawk_sound.c funktracker_defs.h ../options.h \
	../config.h dsp_mixxer.h funktracker.h funkload.h \
	nighthawk_sound.h

info:
dvi:
check: all
	$(MAKE)
installcheck:
install-exec: install-libexecPROGRAMS
	@$(NORMAL_INSTALL)

install-data: 
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-libexecPROGRAMS

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(DATADIR)$(libexecdir)


mostlyclean-generic:
	-test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(DISTCLEANFILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
	-test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
mostlyclean:  mostlyclean-libexecPROGRAMS mostlyclean-compile \
		mostlyclean-tags mostlyclean-generic

clean:  clean-libexecPROGRAMS clean-compile clean-tags clean-generic \
		mostlyclean

distclean:  distclean-libexecPROGRAMS distclean-compile distclean-tags \
		distclean-generic clean
	-rm -f config.status

maintainer-clean:  maintainer-clean-libexecPROGRAMS \
		maintainer-clean-compile maintainer-clean-tags \
		maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: mostlyclean-libexecPROGRAMS distclean-libexecPROGRAMS \
clean-libexecPROGRAMS maintainer-clean-libexecPROGRAMS \
uninstall-libexecPROGRAMS install-libexecPROGRAMS mostlyclean-compile \
distclean-compile clean-compile maintainer-clean-compile tags \
mostlyclean-tags distclean-tags clean-tags maintainer-clean-tags \
distdir info dvi installcheck install-exec install-data install \
uninstall all installdirs mostlyclean-generic distclean-generic \
clean-generic maintainer-clean-generic clean mostlyclean distclean \
maintainer-clean


-include $(srcdir)/Make.common

host_safe:
	@if [ $(HOST_SAFE) = '1' ]; then \
	  echo "host is safe" >host_safe; \
	else \
	  echo "***********************************************"; \
	  echo "This is not an INTEL based machine. As the Funktracker"; \
	  echo "format is in Intel byte sex (and i haven't yet made"; \
	  echo "provisions for other processors), trying to play"; \
	  echo "funktracker songs on this machine will be dangerous."; \
	  echo ""; \
	  echo "Refer to INSTALL file."; \
	  echo "***********************************************"; \
	  false; \
	fi

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I$(srcdir)/sdk

pkglib_LTLIBRARIES = libaudioplugin.la

libaudioplugin_la_LDFLAGS = -module -avoid-version
libaudioplugin_la_SOURCES = plugins/np_entry.c plugins/npn_gate.c audio.cpp \
	sox_media_player.cpp exec_media_player.cpp dshow_player.cpp \
	file_player.cpp media_player.cpp syslog.c

noinst_HEADERS = plugins/npplat.h common.h \
	exec_media_player.h dshow_player.h media_player.h

pkglib_DATA = xpi/install.js

CLEANFILES = xpi/install.js

EXTRA_DIST = xpi/install.js.in jiveaudio.bdsgroup npaudio.bdsproj npaudio.bpf npaudio.def plugins/npaudio.rc.in

SUBDIRS = sdk doc

xpi/install.js: $(srcdir)/xpi/install.js.in libaudioplugin.la
	$(INSTALL) -d xpi
	. ./libaudioplugin.la && sed -e 's,@dlname@,$$dlname,g' < $(srcdir)/xpi/install.js.in > $@

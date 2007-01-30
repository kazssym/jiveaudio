## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I./sdk -I$(srcdir)/sdk

pkglib_LTLIBRARIES = libaudioplugin.la

libaudioplugin_la_LDFLAGS = -module -avoid-version
libaudioplugin_la_SOURCES = plugins/np_entry.cpp plugins/npn_gate.cpp \
	audio.cpp \
	sox_media_player.cpp exec_media_player.cpp plugins/player_win32.cpp \
	file_player.cpp media_player.cpp plugins/debug.c

noinst_HEADERS = plugins/npplat.h common.h \
	exec_media_player.h plugins/player_win32.h media_player.h

pkglib_DATA = xpi/install.js

CLEANFILES = xpi/install.js

EXTRA_DIST = xpi/install.js.in jiveaudio.bdsgroup plugins/npaudio.bdsproj plugins/npaudio.bpf plugins/npaudio.rc

SUBDIRS = sdk plugins doc

xpi/install.js: $(srcdir)/xpi/install.js.in libaudioplugin.la
	$(INSTALL) -d xpi
	. ./libaudioplugin.la && sed -e 's,@dlname@,$$dlname,g' < $(srcdir)/xpi/install.js.in > $@

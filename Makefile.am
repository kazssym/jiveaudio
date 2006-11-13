## Process this file with automake to produce a Makefile.in.

INCLUDES = -I$(srcdir)/plugin -I$(srcdir)/java -I$(srcdir)/xpcom \
	-I./nspr -I$(srcdir)/nspr

pkglib_LTLIBRARIES = libaudioplugin.la

libaudioplugin_la_LDFLAGS = -module -avoid-version
libaudioplugin_la_SOURCES = np_entry.cpp npn_gate.cpp audio.cpp \
	sox_media_player.cpp exec_media_player.cpp dshow_media_player.cpp \
	file_media_player.cpp media_player.cpp syslog.cpp new.cpp

noinst_HEADERS = npplat.h common.h \
	exec_media_player.h dshow_media_player.h media_player.h

noinst_DATA = install.js

info_TEXINFOS = jiveaudio.texi

CLEANFILES = install.js

EXTRA_DIST = install.js.in npaudio.bpr npaudio.bpf npaudio.res npaudio.def

SUBDIRS = nspr xpcom java plugin

install.js: $(srcdir)/install.js.in libaudioplugin.la
	. ./libaudioplugin.la && sed -e 's,@dlname@,$$dlname,g' < $(srcdir)/install.js.in > $@

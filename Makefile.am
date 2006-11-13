## Process this file with automake to produce a Makefile.in.

INCLUDES = -I$(srcdir)/plugin -I$(srcdir)/java -I$(srcdir)/xpcom \
	-I./nspr -I$(srcdir)/nspr

pkglib_LTLIBRARIES = libaudioplugin.la

libaudioplugin_la_LDFLAGS = -module -avoid-version
libaudioplugin_la_SOURCES = np_entry.cpp npn_gate.cpp audio.cpp new.cpp \
	sox_media_player.cpp exec_media_player.cpp \
	file_media_player.cpp media_player.cpp

noinst_HEADERS = npplat.h common.h exec_media_player.h media_player.h

info_TEXINFOS = jiveaudio.texi

EXTRA_DIST = npaudio.bpr npaudio.bpf npaudio.res npaudio.def

SUBDIRS = nspr xpcom java plugin

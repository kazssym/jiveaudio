## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I$(srcdir)/../sdk

pkglib_LTLIBRARIES = libjiveplugin.la

libjiveplugin_la_LDFLAGS = -module -avoid-version
libjiveplugin_la_SOURCES = audio.cpp player.cpp file_player.cpp \
	player_posix.cpp sox_media_player.cpp exec_media_player.cpp \
	buffer.cpp debug.c win32.cpp \
	np_entry.cpp npn_gate.cpp

noinst_HEADERS = audio.h player.h \
	exec_media_player.h player_posix.h buffer.h debug.h win32.h

EXTRA_DIST = npjive.cbproj npjive.def resource.rc

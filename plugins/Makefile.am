## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I$(srcdir)/../sdk

pkglib_LTLIBRARIES = libjiveplugin.la

libjiveplugin_la_LDFLAGS = -module -avoid-version
libjiveplugin_la_SOURCES = plugin.cpp player.cpp \
	player_posix.cpp player_win32.cpp \
	sox_media_player.cpp exec_media_player.cpp \
	file_player.cpp buffer.cpp debug.c np_entry.cpp npn_gate.cpp

noinst_HEADERS = plugin.h player.h \
	exec_media_player.h player_posix.h player_win32.h buffer.h debug.h

EXTRA_DIST = npjive.cbproj npjive.rc

## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I../sdk -I$(srcdir)/../sdk

pkglib_LTLIBRARIES = libaudioplugin.la

libaudioplugin_la_LDFLAGS = -module
libaudioplugin_la_SOURCES = npaudio.cpp player.cpp \
	player_posix.cpp player_win32.cpp \
        ../sox_media_player.cpp ../exec_media_player.cpp \
        ../file_player.cpp debug.c np_entry.cpp npn_gate.cpp

noinst_HEADERS = npaudio.h player.h \
        ../exec_media_player.h player_posix.h player_win32.h debug.h npplat.h

EXTRA_DIST = npaudio.bdsproj npaudio.bpf npaudio.rc

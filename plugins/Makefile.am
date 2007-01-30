## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I../sdk -I$(srcdir)/../sdk

pkglib_LTLIBRARIES = libaudioplugin.la

libaudioplugin_la_LDFLAGS = -module
libaudioplugin_la_SOURCES = np_entry.cpp npn_gate.cpp npaudio.cpp \
        ../audio.cpp \
        ../sox_media_player.cpp ../exec_media_player.cpp player_win32.cpp \
        ../file_player.cpp ../media_player.cpp debug.c

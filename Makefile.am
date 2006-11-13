## Process this file with automake to produce a Makefile.in.

mozilla_libdir = $(libdir)/mozilla
pluginsdir = $(mozilla_libdir)/plugins

INCLUDES = -DXP_UNIX -I$(srcdir)/mozilla

plugins_LTLIBRARIES = libaudioplugin.la libmidiplugin.la

libaudioplugin_la_LDFLAGS = -module -avoid-version
libaudioplugin_la_SOURCES = np_entry.c npn_gate.c audio.c audio_mime.c

libmidiplugin_la_LDFLAGS = -module -avoid-version
libmidiplugin_la_SOURCES = np_entry.c npn_gate.c midi.c midi_mime.c

noinst_HEADERS = npplat.h common.h

SUBDIRS = mozilla

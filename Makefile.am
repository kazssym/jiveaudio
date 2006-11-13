## Process this file with automake to produce a Makefile.in.

mozilla_libdir = $(libdir)/mozilla
pluginsdir = $(mozilla_libdir)/plugins

INCLUDES = -I$(srcdir)/mozilla

plugins_LTLIBRARIES = libaudioplugin.la libmidiplugin.la

libaudioplugin_la_LDFLAGS = -module -avoid-version
libaudioplugin_la_SOURCES = np_entry.cpp npn_gate.cpp audio.cpp

libmidiplugin_la_LDFLAGS = -module -avoid-version
libmidiplugin_la_SOURCES = np_entry.cpp npn_gate.cpp midi.cpp

noinst_HEADERS = npplat.h common.h

SUBDIRS = mozilla

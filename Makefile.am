## Process this file with automake to produce a Makefile.in.

pkglib_DATA = xpi/install.js

CLEANFILES = xpi/install.js

EXTRA_DIST = xpi/install.js.in jiveaudio.bdsgroup

SUBDIRS = sdk plugins doc

xpi/install.js: $(srcdir)/xpi/install.js.in plugins/libaudioplugin.la
	$(INSTALL) -d xpi
	. plugins/libaudioplugin.la && sed -e 's,@dlname@,$$dlname,g' < $(srcdir)/xpi/install.js.in > $@

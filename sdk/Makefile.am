## Process this file with automake to produce a Makefile.in.

noinst_HEADERS = npapi.h npupp.h npruntime.h nptypes.h \
	jri.h jritypes.h jri_md.h jni.h jni_md.h prtypes.h obsolete/protypes.h

dist-hook:
	mkdir $(distdir)/md
	cp -p $(srcdir)/md/*.cfg $(distdir)/md

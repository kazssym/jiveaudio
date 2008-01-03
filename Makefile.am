## Process this file with automake to produce a Makefile.in.

noinst_HEADERS = win32/config.h win32/prcpucfg.h

EXTRA_DIST = jiveaudio.groupproj jiveaudio.bdsgroup

SUBDIRS = sdk plugins doc

dist-hook:
	cd $(distdir); \
find . ! -name \*SUMS -type f -print | sed 's,^\./,,' | xargs sha1sum > SHA1SUMS \
|| rm -f SHA1SUMS
	if test -f $(srcdir)/sign-distdir.sh; then \
  $(SHELL) $(srcdir)/sign-distdir.sh $(distdir); \
fi

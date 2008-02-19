## Process this file with automake to produce a Makefile.in.

AUTOMAKE_OPTIONS = check-news dist-zip

noinst_HEADERS = win32/config.h win32/prcpucfg.h

EXTRA_DIST = jiveaudio.groupproj

SUBDIRS = sdk plugins doc

dist-hook:
	cd $(distdir); \
find . ! -name \*SUMS -type f -print | sed 's,^\./,,' | sort | xargs sha1sum > SHA1SUMS \
|| rm -f SHA1SUMS
	if test -f $(srcdir)/sign-dist.sh; then \
  $(SHELL) $(srcdir)/sign-dist.sh $(distdir); \
fi
	if test -f $(srcdir)/export-dist.sh; then \
  $(SHELL) $(srcdir)/export-dist.sh $(distdir); \
fi

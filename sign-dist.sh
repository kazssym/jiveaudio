#!/bin/sh

distdir="$1"

test -f "$distdir"/SHA1SUMS || exit 1

gpg -sba "$distdir"/SHA1SUMS || exit 1

exit 0

#!/bin/sh

distdir="$1" && shift

test -f "$distdir"/SHA1SUMS || exit 0

gpg -sba "$distdir"/SHA1SUMS

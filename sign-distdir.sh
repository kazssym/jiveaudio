#!/bin/sh

distdir="$1" && shift

test -f "$distdir"/MD5SUMS || exit 0

gpg -sb "$distdir"/MD5SUMS

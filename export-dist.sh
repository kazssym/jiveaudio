#!/bin/sh

distdir="$1"
exportdir="$HOME"/sites/www.linuxfront.com/labs/jiveaudio/3/src/jiveaudio

test -d "$distdir" || exit 1

rm -fr "$exportdir"  || exit 1
cp -a "$distdir" "$exportdir" || exit 1

exit 0

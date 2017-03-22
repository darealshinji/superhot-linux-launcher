#!/bin/sh

set -e
set -x

cd "${0%/*}"

LANG=C
cflags="-Wall -O3 -fstack-protector -ffunction-sections -fdata-sections -DENABLE_BINRELOC -D_FORTIFY_SOURCE=2"

test -d fltk/build/usr/include || ./build-fltk.sh

cd images
xxd -i launcher.png > ../resources.h
xxd -i icon.png >> ../resources.h
cd ..

./genlists.sh > menu_item_lists.h

fltk_version="$(echo `cat fltk/VERSION`)"

gcc $cflags -c -o binreloc.o binreloc.c

# uncomment to enable decorations by the window manager
#cflags="$cflags -DWINDOW_DECORATION=1"

g++ -std=c++98 $cflags \
  -DPRINT_VERSION=\"$fltk_version\" \
  -I./fltk \
  -I./fltk/build/usr/include \
  -I./fltk/build/usr/include/FL/images \
  `./fltk/build/fltk-config --cflags --use-images | tr ' ' '\n' | grep '^-D'` \
  -c -o launcher.o launcher.cpp

g++ -o launcher launcher.o binreloc.o \
  -Wl,--as-needed -Wl,--gc-sections -Wl,-z,defs -Wl,-z,relro \
  `./fltk/build/fltk-config --ldflags --use-images | sed 's/-lpng//g; s/-lz//g; s/-lfltk_images/-lfltk_images -lfltk_png -lfltk_z/'`

strip --strip-all launcher


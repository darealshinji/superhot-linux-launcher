#!/bin/sh

LANG=C

cflags="-Wall -O3 -fstack-protector -ffunction-sections -fdata-sections -DENABLE_BINRELOC -D_FORTIFY_SOURCE=2"

set -v
set -e

test -x libcxx/libc++-wrapper || ./build-libcxx.sh
test -d fltk/build/usr/include || ./build-fltk.sh

cd images
xxd -i launcher.png > ../resources.h
xxd -i icon.png >> ../resources.h
cd ..

fltk_version="$(echo `cat fltk/VERSION`)"

gcc $cflags -c -o binreloc.o binreloc.c

# uncomment to enable decorations by the window manager
#cflags="$cflags -DWINDOW_DECORATION=1"

libcxx/libc++-wrapper -std=c++11 $cflags \
  -DPRINT_VERSION=\"$fltk_version\" \
  -I./fltk \
  -I./fltk/build/usr/include \
  -I./fltk/build/usr/include/FL/images \
  -D_THREAD_SAFE -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE \
  -c -o launcher.o launcher.cpp

libcxx/libc++-wrapper -o launcher launcher.o binreloc.o \
  -L./fltk/build/lib/ -lfltk_images -lfltk_png -lfltk_z -lfltk \
  -Wl,--as-needed -Wl,--gc-sections -Wl,-z,defs -Wl,-z,relro \
  -lm -lX11 -lXext -lpthread -lXinerama -lXfixes -lXcursor -lXft -lXrender -lfontconfig -ldl 

strip --strip-all launcher


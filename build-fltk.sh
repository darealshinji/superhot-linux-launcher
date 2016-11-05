#!/bin/sh

# sudo apt-get install libx11-dev libxft-dev libxcursor-dev libxext-dev libxfixes-dev libxinerama-dev cmake

cd "${0%/*}"
curdir="$PWD"

CXX="$curdir/libcxx/libc++-wrapper"
buildflags="-O2 -fstack-protector -ffunction-sections -fdata-sections -D_FORTIFY_SOURCE=2"

set -v
set -e

test -d fltk || svn co --username '' --password '' "http://seriss.com/public/fltk/fltk/branches/branch-1.3/" fltk

rm -rf fltk/build
mkdir -p fltk/build
cd fltk/build

cmake .. -DCMAKE_BUILD_TYPE="None" \
  -DCMAKE_INSTALL_PREFIX="$curdir/fltk/build/usr" \
  -DCMAKE_CXX_COMPILER="$CXX" \
  -DCMAKE_CXX_FLAGS="$buildflags" \
  -DCMAKE_C_FLAGS="$buildflags" \
  -DOPTION_USE_GL="OFF" \
  -DOPTION_OPTIM="" \
  -DOPTION_USE_SYSTEM_LIBJPEG="OFF" \
  -DOPTION_USE_SYSTEM_LIBPNG="OFF" \
  -DOPTION_USE_SYSTEM_ZLIB="OFF"
make -j4
make install

cd "$curdir"


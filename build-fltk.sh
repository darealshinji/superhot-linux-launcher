#!/bin/sh

# sudo apt-get install libx11-dev libxft-dev libxext-dev libxinerama-dev cmake

set -e
set -x

cd "${0%/*}"

curdir="$PWD"
buildflags="-O3 -fstack-protector -ffunction-sections -fdata-sections -D_FORTIFY_SOURCE=2"

if [ ! -f fltk/CMakeLists.txt ]; then
  git submodule init
  git submodule update
fi

rm -rf fltk/build
mkdir -p fltk/build
cd fltk/build

cmake .. -DCMAKE_BUILD_TYPE="Release" \
  -DCMAKE_INSTALL_PREFIX="$curdir/fltk/build/usr" \
  -DCMAKE_CXX_FLAGS="-std=c++11 $buildflags" \
  -DCMAKE_C_FLAGS="$buildflags" \
  -DOPTION_USE_GL="OFF" \
  -DOPTION_OPTIM="-O3" \
  -DOPTION_USE_SYSTEM_LIBJPEG="OFF" \
  -DOPTION_USE_SYSTEM_LIBPNG="OFF" \
  -DOPTION_USE_SYSTEM_ZLIB="OFF"
make -j4
make install

cd "$curdir"


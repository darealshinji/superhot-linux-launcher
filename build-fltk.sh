#!/bin/sh

# sudo apt-get install libx11-dev libxft-dev libxext-dev libxinerama-dev libpango1.0-dev cmake

set -e
set -x

cd "${0%/*}"

curdir="$PWD"
buildflags="-O3 -fstack-protector-strong -ffunction-sections -fdata-sections -D_FORTIFY_SOURCE=2"

rm -rf fltk/build
mkdir -p fltk/build
cd fltk/build

# w/o Pango
cmake .. -DCMAKE_BUILD_TYPE="Release" \
  -DCMAKE_INSTALL_PREFIX="$curdir/fltk/build/usr" \
  -DCMAKE_CXX_FLAGS="-std=c++11 $buildflags" \
  -DCMAKE_C_FLAGS="$buildflags" \
  -DOPTION_OPTIM="-O3" \
  -OPTION_PRINT_SUPPORT="OFF" \
  -DOPTION_CAIRO="OFF" \
  -DOPTION_USE_GL="OFF" \
  -DOPTION_USE_NANOSVG="OFF" \
  -DOPTION_USE_PANGO="OFF" \
  -DOPTION_USE_SYSTEM_LIBJPEG="OFF" \
  -DOPTION_USE_SYSTEM_LIBPNG="OFF" \
  -DOPTION_USE_SYSTEM_ZLIB="OFF"
make -j4
make install

cd "$curdir"
rm -rf fltk/build-pango
mkdir -p fltk/build-pango
cd fltk/build-pango

# Pango
cmake .. -DCMAKE_BUILD_TYPE="Release" \
  -DCMAKE_INSTALL_PREFIX="$curdir/fltk/build-pango/usr" \
  -DCMAKE_CXX_FLAGS="-std=c++11 $buildflags" \
  -DCMAKE_C_FLAGS="$buildflags" \
  -DOPTION_OPTIM="-O3" \
  -OPTION_PRINT_SUPPORT="OFF" \
  -DOPTION_CAIRO="OFF" \
  -DOPTION_USE_GL="OFF" \
  -DOPTION_USE_NANOSVG="OFF" \
  -DOPTION_USE_PANGO="ON" \
  -DOPTION_USE_SYSTEM_LIBJPEG="OFF" \
  -DOPTION_USE_SYSTEM_LIBPNG="OFF" \
  -DOPTION_USE_SYSTEM_ZLIB="OFF"
make -j4
make install

cd ..

ver=$(cat VERSION)
echo "$ver git$(git rev-parse --short HEAD)" > ../VERSION

cd "$curdir"


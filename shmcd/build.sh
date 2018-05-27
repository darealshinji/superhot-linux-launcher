#!/bin/sh
set -e
set -x

cd "${0%/*}"

xxd -i shmcd_background.jpg > background.h

#VER="1.0.0"
#VER="2.0.1m"
#VER="3.0.1m"
#VER="4.0.0m"
#VER="4.0.12m"
#VER="5.0.0m"
VER="5.0.1m"
echo "#define SHMCD_VERSION \"ALPHA $VER\"" > version.h

. ../build.inc


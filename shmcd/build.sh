#!/bin/sh
set -e
set -x

cd "${0%/*}"

xxd -i shmcd_background.jpg > background.h

. ../build.inc


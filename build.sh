#!/bin/bash
set -e
set -x

cd "${0%/*}"

if [ ! -d fltk ]; then
  git clone "https://github.com/fltk/fltk"
fi

./sh/build.sh
./shmcd/build.sh


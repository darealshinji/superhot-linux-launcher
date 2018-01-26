#!/bin/bash
set -e
set -x

cd "${0%/*}"
./sh/build.sh
./shmcd/build.sh


#!/bin/sh
cd "${0%/*}"
rm -f launcher ressources.h
rm -rf fltk/build libcxx libcxx_source/libcxx/build libcxx_source/libcxxabi/build

#!/bin/sh
cd "${0%/*}"
rm -f *.o launcher launcher.prefs menu_item_lists.h resources.h
rm -rf fltk/build libcxx libcxx_source/libcxx/build libcxx_source/libcxxabi/build

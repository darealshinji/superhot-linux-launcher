#!/bin/sh
set -e
set -x

cd "${0%/*}"

xxd -i shmcd_background.jpg > background.h

# no Pango
build="build"

. ../build.inc

str="BETA            "
strlen=$(printf "$str" | wc -c)

for ver in 1.0.0 2.0.1m 3.0.1m 4.0.0m 4.0.12m 5.0.0m 5.0.1m ; do
  str2="ALPHA $ver"
  strlen2=$(printf "$str2" | wc -c)
  append=$(($strlen - $strlen2))
  if [ $append -gt 0 ]; then
    str2="$str2$(perl -E "say ' ' x$append")"
    sed -e "s|$str|$str2|" launcher > launcher-a${ver}
    chmod a+x launcher-a${ver}
  fi
done

for ver in 1.0.0 ; do
  str2="BETA $ver"
  strlen2=$(printf "$str2" | wc -c)
  append=$(($strlen - $strlen2))
  if [ $append -gt 0 ]; then
    str2="$str2$(perl -E "say ' ' x$append")"
    sed -e "s|$str|$str2|" launcher > launcher-b${ver}
    chmod a+x launcher-b${ver}
  fi
done


LANG=C
cflags="-Wall -O3 -fstack-protector -ffunction-sections -fdata-sections -D_FORTIFY_SOURCE=2"

test -d ../fltk/build/usr/include || ../build-fltk.sh

rm -f images.h
for f in *.png; do
  xxd -i $f >> images.h
  echo "Fl_PNG_Image image_${f%.png}(NULL, ${f%.png}_png, ${f%.png}_png_len);" >> images.h
done

fltk_version="$(echo `cat ../fltk/VERSION`)"

g++ -std=c++11 $cflags \
  -DPRINT_VERSION=\"$fltk_version\" \
  -I.. \
  -I../fltk \
  -I../fltk/build/usr/include \
  -I../fltk/build/usr/include/FL/images \
  `../fltk/build/fltk-config --cflags --use-images | tr ' ' '\n' | grep '^-D'` \
  -c -o launcher.o launcher.cpp

g++ -o launcher launcher.o \
  -Wl,--as-needed -Wl,--gc-sections -Wl,-z,defs -Wl,-z,relro \
  `../fltk/build/fltk-config --ldflags --use-images | sed 's/-ljpeg//g; s/-lpng//g; s/-lz//g; s/-lfltk_images/-lfltk_images -lfltk_jpeg -lfltk_png -lfltk_z/'`

strip --strip-all launcher

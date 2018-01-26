
all:
	./build.sh

clean:
	rm -f sh/images.h sh/launcher sh/launcher.o sh/launcher.prefs
	rm -f shmcd/images.h shmcd/launcher shmcd/launcher.o shmcd/launcher.prefs

distclean: clean
	rm -rf fltk/build/


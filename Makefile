
all:
	./build.sh

clean:
	rm -f */background.h */images.h */launcher */launcher.o */launcher.prefs shmcd/version.h

distclean: clean
	rm -rf fltk/build/


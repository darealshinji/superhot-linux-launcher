This is a launcher for the Linux version of the game [SUPERHOT](http://superhotgame.com/) that I have written for fun.
It's intended to resemble the launcher from the Windows release.
FLTK and BinReloc are used because of their closed-source friendly licenses and for portability reasons.

The launcher builds and runs successfully on Ubuntu 12.04 or later versions.
I couldn't properly test the monitor selection as I don't have multiple screens connected to my PC.

Build dependencies on Ubuntu are: `cmake libx11-dev libxft-dev libxext-dev`

Don't forget to checkout the FLTK sources with `git submodule init && git submodule update`


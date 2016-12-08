This is a launcher for the Linux version of the game [SUPERHOT](http://superhotgame.com/) that I have written for fun.
It's intended to resemble the launcher from the Windows release.
FLTK, libc++ and BinReloc are used because of their closed-source friendly licenses and for portability reasons.

The launcher builds successfully on Ubuntu 16.04.
It is able to run in an Ubuntu 12.04 chroot environment.
I couldn't properly test the monitor selection as I don't have multiple screens connected to my PC.

Build dependencies on Ubuntu 16.04 are:
`git subversion cmake clang-3.6 llvm llvm-3.8-dev libx11-dev libxft-dev libxcursor-dev libxext-dev libxfixes-dev libxinerama-dev`

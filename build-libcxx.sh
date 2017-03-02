#!/bin/sh

# sudo apt-get install clang-3.6 llvm llvm-3.8-dev

cd "${0%/*}"

CC="clang-3.6"
CXX="clang++-3.6"
llvm_cmake_dir="/usr/share/llvm-3.8/cmake"
prefix="$PWD/libcxx"

buildflags="-O2 -fstack-protector -ffunction-sections -fdata-sections -D_FORTIFY_SOURCE=2"

set -v
set -e

checkout="$PWD/libcxx_source"
mkdir -p "$checkout"
cd "$checkout"
revision=$(cat last_tested_revision)
test -d libcxx || svn co -r $revision http://llvm.org/svn/llvm-project/libcxx/trunk libcxx
test -d libcxxabi || svn co -r $revision http://llvm.org/svn/llvm-project/libcxxabi/trunk libcxxabi


### libc++abi ###

rm -rf libcxxabi/build
mkdir -p libcxxabi/build
cd libcxxabi/build

if [ ! -f ../CMakeLists.txt.orig ]; then
	cp ../CMakeLists.txt ../CMakeLists.txt.orig
	sed -i "s|\${LLVM_BINARY_DIR}\/lib\${LLVM_LIBDIR_SUFFIX}\/cmake\/llvm|$llvm_cmake_dir|" ../CMakeLists.txt
fi

cmake .. -DCMAKE_BUILD_TYPE=None \
	-DCMAKE_INSTALL_PREFIX="$prefix" \
	-DCMAKE_C_COMPILER="$CC" \
	-DCMAKE_CXX_COMPILER="$CXX" \
	-DCMAKE_CXX_FLAGS="$buildflags" \
	-DCMAKE_C_FLAGS="$buildflags" \
	-DLLVM_CMAKE_PATH="$llvm_cmake_dir" \
	-DLIBCXXABI_LIBCXX_PATH="$checkout/libcxx" \
	-DLIBCXXABI_ENABLE_SHARED=OFF \
	-DLIBCXXABI_ENABLE_STATIC=ON \
	-DLIBCXXABI_ENABLE_THREADS=OFF
make -j4

mkdir -p "$prefix"
cp -rf lib "$prefix"

cd "$checkout"


### libc++ ###

rm -rf libcxx/build
mkdir -p libcxx/build
cd libcxx/build

cmake .. -DCMAKE_BUILD_TYPE=None \
	-DCMAKE_INSTALL_PREFIX="$prefix" \
	-DCMAKE_C_COMPILER="$CC" \
	-DCMAKE_CXX_COMPILER="$CXX" \
	-DCMAKE_CXX_FLAGS="$buildflags" \
	-DCMAKE_C_FLAGS="$buildflags" \
	-DLIBCXX_CXX_ABI="libcxxabi" \
	-DLIBCXX_CXX_ABI_INCLUDE_PATHS="$checkout/libcxxabi/include" \
	-DLIBCXX_CXX_ABI_LIBRARY_PATH="$checkout/libcxxabi/lib" \
	-DLIBCXX_ENABLE_SHARED=OFF \
	-DLIBCXX_ENABLE_THREADS=OFF \
	-DLIBCXX_INCLUDE_DOCS=OFF
make -j4
make -C include install
cp -f "$checkout/libcxxabi/include"/* "$prefix/include"
cd lib
if [ -f "CMakeFiles/cxx_static.dir/link.txt" ]; then
	cxx_dir="cxx_static.dir"
elif [ -f "CMakeFiles/cxx.dir/link.txt" ]; then
	cxx_dir="cxx.dir"
else
	exit 1
fi
sh CMakeFiles/$cxx_dir/link.txt
cp -f libc++.a "$prefix/lib"

### libc++-wrapper ###
set +v
cat <<EOF> "$prefix/libc++-wrapper"
#!/bin/sh

prefix="\$(cd "\${0%/*}" && echo \$PWD)"

CXX="g++"
cxxv1="\$prefix/include/c++/v1"
libcxxdir="\$prefix/lib"

if [ "x\$1" = "x" ]; then
	"\$CXX"
	exit \$?
fi

if (echo "\$*" | tr '\t' ' ' | grep -q -e ' -c '); then
	"\$CXX" \$* -nostdinc++ -I"\$cxxv1"
else
	"\$CXX" \$* -nostdinc++ -I"\$cxxv1" -nodefaultlibs \\
		-Wl,--no-as-needed "\$libcxxdir/libc++.a" "\$libcxxdir/libc++abi.a" -lc \\
		-Wl,--as-needed -lm -lgcc_s -lgcc -lpthread -ldl -lrt
fi

EOF
set -v

chmod a+x "$prefix/libc++-wrapper"

rm -rf "$checkout/libcxx/build"
rm -rf "$checkout/libcxxabi/build"


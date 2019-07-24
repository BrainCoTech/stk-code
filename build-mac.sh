#!/bin/bash
set -e
cd ${0%/*}
SCRIPT_DIR=$(pwd)

# https://github.com/supertuxkart/stk-code/blob/master/INSTALL.md#building-supertuxkart-on-macos

# create build folder
# rm -rf cmake_build    # remove cmake_build if you want to clean all
mkdir -p cmake_build
cd cmake_build

# cmake generate project
CMAKE_PREFIX_PATH=/usr/local/opt/freetype/:/usr/local/opt/curl/:/usr/local/opt/libogg/:/usr/local/opt/libogg/:/usr/local/opt/libvorbis/:/usr/local/opt/glew/:/usr/local/opt/fribidi/ \
cmake .. \
    -DFREETYPE_INCLUDE_DIRS=/usr/local/opt/freetype/include/freetype2/ \
    -DUSE_SYSTEM_GLEW=1 \
    -DOPENAL_INCLUDE_DIR=/usr/local/opt/openal-soft/include/ \
    -DOPENAL_LIBRARY=/usr/local/opt/openal-soft/lib/libopenal.dylib \
    -DFREETYPE_LIBRARY=/usr/local/opt/freetype/lib/libfreetype.dylib

# cmake build
make

# execute supertuxkart
cd ${SCRIPT_DIR}/cmake_build/bin/supertuxkart.app/Contents/MacOS
./supertuxkart --unlock-all

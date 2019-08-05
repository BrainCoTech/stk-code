#!/bin/bash
set -e          # exit script if occur any error
cd ${0%/*}/..   # goto source root directory
ROOT=$(pwd)

# https://github.com/supertuxkart/stk-code/blob/master/INSTALL.md#building-supertuxkart-on-macos

# check brew dependies listed in `Brewfile`
brew bundle

# create build folder
# rm -rf cmake_build    # remove cmake_build if you want to clean all
mkdir -p cmake_build
cd cmake_build

# remove supertuxkart.app
rm -rf bin

# cmake generate project
CMAKE_PREFIX_PATH=/usr/local/opt/freetype/:/usr/local/opt/curl/:/usr/local/opt/libogg/:/usr/local/opt/libogg/:/usr/local/opt/libvorbis/:/usr/local/opt/glew/:/usr/local/opt/fribidi/ \
cmake .. \
    -DFREETYPE_INCLUDE_DIRS=/usr/local/opt/freetype/include/freetype2/  \
    -DUSE_SYSTEM_GLEW=1                                                 \
    -DOPENAL_INCLUDE_DIR=/usr/local/opt/openal-soft/include/            \
    -DOPENAL_LIBRARY=/usr/local/opt/openal-soft/lib/libopenal.dylib     \
    -DFREETYPE_LIBRARY=/usr/local/opt/freetype/lib/libfreetype.dylib    \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="10.9"

# cmake build
make
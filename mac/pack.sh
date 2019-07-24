#!/bin/bash
set -e          # exit script if occur any error
cd ${0%/*}/..   # goto source root directory
ROOT=$(pwd)

# echo yellow
function echo_y () {
    local YELLOW="\033[1;33m"
    local RESET="\033[0m"
    echo -e "${YELLOW}$@${RESET}"
}

# build
./mac/build.sh

# 1. generate 'libs'
# remove libs
cd cmake_build
rm -rf ./bin/SuperTuxKart.app/Contents/libs/

# copy all depended dynamic libraries to `libs` folder
echo_y "\n[lib] copy all depended dynamic libraries"
dylibbundler --overwrite-dir --bundle-deps                              \
    --fix-file      ./bin/SuperTuxKart.app/Contents/MacOS/supertuxkart  \
    --dest-dir      ./bin/SuperTuxKart.app/Contents/libs/               \
    --install-path  @executable_path/../libs/

# fix libvorbisenc.2.dylib
echo_y "\n[lib] fix link path of libvorbisenc.2.dylib"
cd ./bin/SuperTuxKart.app/Contents/libs/
install_name_tool -change                                   \
    /usr/local/Cellar/libvorbis/1.3.6/lib/libvorbis.0.dylib \
    @executable_path/../libs/libvorbis.0.dylib              \
    libvorbisenc.2.dylib

# fix libvorbisfile.3.dylib
echo_y "[lib] fix link path of libvorbisfile.3.dylib"
install_name_tool -change                                   \
    /usr/local/Cellar/libvorbis/1.3.6/lib/libvorbis.0.dylib \
    @executable_path/../libs/libvorbis.0.dylib              \
    libvorbisfile.3.dylib

# show all link path of dynamic libraries
echo_y "\n[lib] show all link path of dynamic libraries"
ls -1 | xargs otool -L

# 2. generate 'data'
# remove data link
cd ${ROOT}/cmake_build
DATA_DIR=./bin/SuperTuxKart.app/Contents/Resources/data
rm -rf ${DATA_DIR}

# generate data folder
echo_y "\n[data] copy data to Resources/data"
mkdir -p ${DATA_DIR}
cp -r ${ROOT}/data/* ${DATA_DIR}

# copy the content from 'stk-assets' to data folder
echo_y "[data] copy stk-assets to Resources/data"
cp -r ${ROOT}/../stk-assets/* ${DATA_DIR}

# 3. open executable folder
open ./bin

#!/bin/bash
set -e          # exit script if occur any error
cd ${0%/*}/..   # goto source root directory
ROOT=$(pwd)

# create dependencies
rm -rf dependencies*
mkdir dependencies-vs-64bit
cp -r ../dependencies/windows_64bit/dependencies/* dependencies-vs-64bit

# create build folder
# rm -rf cmake_build    # remove cmake_build if you want to clean all
mkdir -p cmake_build
cd cmake_build

# remove supertuxkart.app
rm -rf bin

# cmake build
cmake .. -A"x64"      # build x64
cmake --build .

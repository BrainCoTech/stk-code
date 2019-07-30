#!/bin/bash
set -e          # exit script if occur any error
cd ${0%/*}/..   # goto source root directory
ROOT=$(pwd)

# remove SuperTuxKart installer-64bit.exe
rm -rf "${ROOT}/tools/windows_installer/SuperTuxKart 0.10.0-git installer-64bit.exe"

# create dependencies
rm -rf dependencies*
mkdir dependencies-vs-64bit
cp -r ../dependencies/windows_64bit/dependencies/* dependencies-vs-64bit

# create build folder
rm -rf cmake_build-64
mkdir -p cmake_build-64
cd cmake_build-64

# cmake build
cmake .. -A"x64"
cmake --build . --config Release

# NSIS build installer
cd ${ROOT}/tools/windows_installer
makensis supertuxkart-64bit.nsi
start .

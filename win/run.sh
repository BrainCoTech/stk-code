#!/bin/bash
set -e          # exit script if occur any error
cd ${0%/*}/..   # goto source root directory
ROOT=$(pwd)

# build
./win/build.sh

# execute supertuxkart
cd ${ROOT}/cmake_build/bin/Debug
./supertuxkart --unlock-all

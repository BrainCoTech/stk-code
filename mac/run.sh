#!/bin/bash
set -e          # exit script if occur any error
cd ${0%/*}/..   # goto source root directory
ROOT=$(pwd)

# build
./mac/build.sh

# execute supertuxkart
cd cmake_build/bin/supertuxkart.app/Contents/MacOS
./supertuxkart --unlock-all

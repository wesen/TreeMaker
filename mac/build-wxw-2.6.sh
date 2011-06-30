#!/bin/bash
#
# This script configures and builds debug and release libraries of wxWidgets 
# from a fresh download, using the options required for my projects, and using
# configure/make. (Note that the XCode build of wxWidgets builds its own libs.)

if [ ! -d build/wxMac-2.6 ]; then
  echo "Error: directory build/wxMac-2.6 does not exist; exiting."
  exit 1
fi

# Turn on Core Graphics rendering if it's off (which is the default)
CHKCONF_FILE="build/wxMac-2.6/include/wx/mac/carbon/chkconf.h"
sed -e '/#define wxMAC_USE_CORE_GRAPHICS/c\
\ \ \ \ #define wxMAC_USE_CORE_GRAPHICS 1' $CHKCONF_FILE > build/temp_chkconf.h
mv build/temp_chkconf.h $CHKCONF_FILE

cd build/wxMac-2.6

echo "Determining configuration options for this system..."
autoconf

# echo "Creating source code & library subdirectory and configuring (debug)..."
# if [ ! -d build-debug ]; then
#   mkdir build-debug
# fi
# cd build-debug
# ../configure --enable-debug --with-mac --enable-monolithic --disable-shared --enable-std_iostreams --enable-std_string
# 
# echo "Compiling wxWidgets (debug)..."
# make
# cd ..

echo "Creating source code & library subdirectory and configuring (release)..."
if [ ! -d build-release ]; then
  mkdir build-release
fi
cd build-release
../configure --with-mac --enable-monolithic --disable-shared --enable-std_iostreams --enable-std_string

echo "Compiling wxMac (release)..."
make
cd ..

cd ../..
exit 0

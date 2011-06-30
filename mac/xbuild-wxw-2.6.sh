#!/bin/bash

# This script (re)builds the wxWidgets libraries from the supplied XCode
# project after a fresh update from CVS.

WX_VERSION=wxMac-2.6

if [ ! -d build/$WX_VERSION ]; then
  echo "Error: directory build/$WX_VERSION doesn't exist."
  exit 1
fi
cd build/$WX_VERSION

# Construct a setup.h file from setup0.h with our required settings. We always
# create a fresh copy from setup0.h.
SETUP_FILE=include/wx/mac/setup.h
if [ -f $SETUP_FILE ]; then
  rm $SETUP_FILE
fi
sed -e '/#define wxMAC_USE_CORE_GRAPHICS/c\
#define wxMAC_USE_CORE_GRAPHICS 1' -e '/#define wxUSE_STD_IOSTREAM/c\
#define wxUSE_STD_IOSTREAM 1' -e '/#define wxUSE_STD_IOSTREAMH/c\
#define wxUSE_STD_IOSTREAMH 0' -e '/#define wxUSE_STD_STRING/c\
#define wxUSE_STD_STRING 1' -e '/#define wxUSE_GLCANVAS/c\
#define wxUSE_GLCANVAS 1' include/wx/mac/setup0.h > $SETUP_FILE

# Turn on Core Graphics rendering if it's off (which is the default in wxW)
CHKCONF_FILE="include/wx/mac/carbon/chkconf.h"
sed -e '/#define wxMAC_USE_CORE_GRAPHICS/c\
\ \ \ \ #define wxMAC_USE_CORE_GRAPHICS 1' $CHKCONF_FILE > temp_chkconf.h
mv temp_chkconf.h $CHKCONF_FILE

# Build the wxWidgets static libraries
cd src
if ! xcodebuild -project wxWindows.xcodeproj -target static -configuration Development
then
  exit 1
fi
if ! xcodebuild -project wxWindows.xcodeproj -target static -configuration Deployment
then
  exit 1
fi

cd ../../..

exit 0

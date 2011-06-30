#!/bin/bash
# This script builds all versions of TreeMaker and associated test code and the
# disk image for the binary distribution.
# usage:
# ./build-tm-all.sh
# ./build-tm-all.sh BUILD_CODE
#
# If a BUILD_CODE is supplied, the version file (tmVersion.h) is updated with
# the given code before building.

if [ $1 ]; then
  echo "setting build number to $1 in tmVersion.h"
  VERSION_FILE="../Source/tmVersion.h"
  sed -e "/#define TM_VERSION_BUILD/c\\
  #define TM_VERSION_BUILD $1" $VERSION_FILE > build/temp_tmVersion.h
  mv build/temp_tmVersion.h $VERSION_FILE
else
  echo "No build code supplied: will use existing value in tmVersion.h"
fi

./build-tm-help.sh

if ! xcodebuild -project TreeMaker.xcodeproj -target TreeMaker -configuration Debug
then
  exit 1
fi
if ! xcodebuild -project TreeMaker.xcodeproj -alltargets -configuration Development
then
  exit 1
fi
if ! xcodebuild -project TreeMaker.xcodeproj -alltargets -configuration Profile
then
  exit 1
fi
if ! xcodebuild -project TreeMaker.xcodeproj -target TreeMaker -configuration Release
then
  exit 1
fi
./build-tm-pkg.sh
exit 0

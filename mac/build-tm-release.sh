#!/bin/bash
# This script builds the Release version of TreeMaker and the disk image for
# the binary distribution.

# build the help.zip file and help cache
./build-tm-help.sh

#compile the Release configuration of the TreeMaker target
if ! xcodebuild -project TreeMaker.xcodeproj -target TreeMaker -configuration Release
then
  exit 1
fi

# build the distribution disk image
./build-tm-pkg.sh

exit 0

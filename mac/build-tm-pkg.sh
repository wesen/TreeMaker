#!/bin/bash
# This script builds the Mac distribution (disk image) for TreeMaker from the 
# latest build.

if [ ! -d build ]; then
  echo "Error: no build directory."
  exit 1
fi
if [ ! -d build/TreeMaker ]; then
  mkdir build/TreeMaker
fi
if [ ! -d "build/Release/TreeMaker 5.app" ]; then
  echo "Error: no Release build present"
  exit 1;
fi

if [ -d "build/TreeMaker/TreeMaker 5.app" ]; then
  rm -R "build/TreeMaker/TreeMaker 5.app"
fi
cp -R -f -p "build/Release/TreeMaker 5.app" "build/TreeMaker/TreeMaker 5.app"
cp -p resources/README.txt build/TreeMaker/README.txt
cp -p ../LICENSE.txt build/TreeMaker/LICENSE.txt

hdiutil create -srcfolder build/TreeMaker -ov build/TreeMaker.dmg

exit 0

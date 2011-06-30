#!/bin/bash
# Completely rebuilds the help.zip file in build/TreeMaker.build/help/help.zip
# from the source code and puts the zip file and its cache in the build
# directory, where it will be copied into the app the next time the XCode
# project TreeMaker.xcodeproj is built.

if [ ! -d build ]; then
  mkdir build
fi
if [ ! -d build/TreeMaker.build ]; then
  mkdir build/TreeMaker.build
fi
if [ ! -d build/TreeMaker.build/help ]; then
  mkdir build/TreeMaker.build/help
fi

cd ../Source/help
find . -type f \( -name '*.htm' -o -name '*.png' -o -name '*.hhp' -o -name '*.hhk' -o -name '*.hhc' \) -print | zip -D ../../mac/build/TreeMaker\.build/help/help -@
cd ../../mac

cd build
./hhp2cached ../../Source/help/treemaker.hhp
mv ../../Source/help/treemaker.hhp.cached TreeMaker.build/help/help.zip_zip_treemaker.hhp.cached
cd ..
exit 0

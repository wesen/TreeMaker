#!/bin/bash
# This script builds the hhp2cached wxWidgets utility. You must have previously
# built wxWidgets.

if [ ! -d build/wxMac-2.6/build-release/utils/hhp2cached ]; then
  echo "Error: directory build/wxMac-2.6/build-release/utils/hhp2cached cannot be found; exiting."
  exit 1;
fi

cd build/wxMac-2.6/build-release/utils/hhp2cached
make
cd ../../../../..
cp build/wxMac-2.6/build-release/utils/hhp2cached/hhp2cached build/hhp2cached
exit 0

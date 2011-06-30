#!/bin/bash
#
# This script checks out a fresh copy of wxMac and wxEtc (samples) using branch
# WX_2_6_BRANCH and creates a folder named wxWidgets to contain it, then
# moves and renames the folder to build/wxMac-2.6.

if [ ! -d build ]; then
  mkdir build
fi

if [ -d build/wxMac-2.6 ]; then
  echo "Error: directory build/wxMac-2.6 already exists; exiting."
  exit 1;
fi

cvs -d :pserver:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets co -r WX_2_6_BRANCH wxMac
cvs -d :pserver:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets co -r WX_2_6_BRANCH wxEtc
mv wxWidgets build/wxMac-2.6
exit 0

#!/bin/bash
# This script builds wxWidgets, then builds all versions of TreeMaker and
# associated test code. You must have the Apple Developer Tools installed and
# an internet connection in order to checkout wxWidgets from the CVS repository
# over the internet.

# Checkout a fresh copy of wxWidgets from CVS
./checkout-wxw-2.6.sh

# Autoconf, configure, and make wxWidgets with Mac-specific options. This is
# only needed to build the hhp2cached utility.
./build-wxw-2.6.sh

# Build the hhp2cached utility from wxWidgets, which is used to precompute the
# help cache file
./build-wxw-hhp2cached.sh

# Build the XCode wxWidgets libraries.
./xbuild-wxw-2.6.sh

# Build all TreeMaker files, including all test programs and all configurations
./build-tm-all.sh

# Done
exit 0

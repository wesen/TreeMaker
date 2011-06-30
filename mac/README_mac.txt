BUILDING and INSTALLING TreeMaker 5 for Macintosh OS X
  by Robert J. Lang

1. INTRODUCTION

This file describes basic procedures for building from source,
installing and running TreeMaker 5.0 for Macintosh (TM5). It is
not needed for installing and using the TreeMaker application.

This and all other files in this directory are subject to the same
license conditions mentioned in the Source/README.txt file, here
referred to as the main README.

Macintosh is a registered trademark of Apple Computer, Inc.

2. BEFORE YOU START

Requisites for building TM5:
- TreeMaker 5.0 requisites (see main README)
- Macintosh OS X Version 10.4.2 or higher
- Apple Developer Tools, including XCode 2.2 or higher
- wxWidgets (wxMac version), as described below

TM5 was programmed and tested on a 500 MHz PowerBook G4 with 512 MB RAM. You
should have 1.2 GB of disk space available to build all targets.

3. TreeMaker CONFIGURATIONS

TM5 may be built in four configurations, which are set up as project build
configurations within the TreeMaker.xcodeproj file. You'll find more detail
about each configuration in the main README:

- configuration Release: production version
- configuration Profile: adds performance measurements
- configuration Development: partial logging and error reporting
- configuration Debug: full logging and error reporting

4. BUILDING AND INSTALLING TM5 FROM SOURCE

4.1. Overview

TreeMaker/mac comes with several scripts to automate the build of TreeMaker.
Any of the scripts supplied in TreeMaker/mac should be run from the
TreeMaker/mac directory. A complete build can be performed by executing the
following script from Terminal:

  build-all.sh -- download the wxWidgets libraries, configure and build
  wxWidgets libraries and the hhp2cached utility. Then build all TreeMaker
  executables and test programs in all configurations.
  
To run this script, open Terminal; cd to the TreeMaker/mac directory; then type
./build-all.sh and hit return. Then sit back and wait. A full build takes about
6 hours on a 500 MHz Powerbook G4 and requires 1.2 GB of disk space.

You can also build TreeMaker incrementally with scripts as described below.

4.2. Compiling wxWidgets

TreeMaker 5.0's user interface uses the multiplatform open-source library
wxWidgets. For maximum portability, TM5 is statically linked. Mac OS X 10.4.2
(Tiger) comes with version 2.5 of wxWidgets preinstalled. TM5 requires
wxWidgets 2.6 or higher, so we will build our own version and not use the
preinstalled version.

TreeMaker comes with several Terminal scripts for setting up and building
wxWidgets.

  checkout-wxw-2.6.sh -- downloads the wxMac and wxEtc modules of wxWidgets
  from the CVS repository to the directory build/wxMac-2.6. You can also obtain
  wxWidgets on CD-ROM from the wxWidgets web site, www.wxwidgets.org or by
  downloading it directly from the wxWidgets web site. If you obtain wxWidgets
  by some other means, you should rename and move the wxWidgets folder to
  build/wxMac-2.6, because all the scripts assume this name and location.

  build-wxw-2.6.sh -- configures and builds the release static libraries
  for wxWidgets using make/configure. This is needed to build the hhp2cached
  utility.

  build-hhp2cached.sh -- builds the wxWidgets hhp2cached utility, which is
  needed to construct TreeMaker's help system.
  
  xbuild-wxw-2.6.sh -- configures and builds the wxWidgets libraries using the
  XCode project (wxMac-2.6/src/wxWindows.xcodeproj), both Development and
  Deployment. These are the libraries used by the TreeMaker XCode project.

4.3. Building TreeMaker

Once you have built the wxWidgets libraries and the hhp2cached utility, you are
ready to build TreeMaker. You can build TreeMaker 5 using Apple's XCode IDE, or
build it from Terminal using scripts (which call xcodebuild, the command-line
version of XCode).

The file TreeMaker.xcodeproj is an XCode project that you can open with Apple's
XCode IDE to build any configuration of any target and/or to interactively
debug TreeMaker. You can also perform the entire build process, or portions
thereof, by running the following scripts in Terminal.

  build-tm-help.sh -- builds the TreeMaker help archive. This should be run
  after any changes to any of the help files. You MUST run this script before
  building TreeMaker whether you are using XCode or xcodebuild. It is
  automatically run as part of the next two scripts.

  build-tm-all.sh -- builds all configurations of all executables and test
  programs.

  build-tm-release.sh -- builds just the release configuration of TreeMaker.
  
  build-tm-pkg.sh -- build the disk image for the binary distribution. This is
  automatically run as part of the previous two scripts.

A complete build of TreeMaker will consume about 1.2 GB of disk space, of which
about 600MB is taken up by the wxWidgets libraries and about 600 MB is taken up
by TreeMaker's object files.

The executables are all built within individual directories within the build
directory, namely build/Debug, build/Development, build/Profile, and
build/Release. When a build is complete, you can drag the application out of
its build directory to your preferred final location. The build/TreeMaker
directory contains the files of the binary distribution. The file TreeMaker.dmg
is a compressed disk image of the binary distribution.

4.4 Installing TreeMaker

After building, you can drag the build/Release/TreeMaker application to your
Applications folder and, if you wish, delete the entire build directory
(thereby reclaiming that 1.2 GB of disk space). There are no other files in the
TreeMaker installation; all documentation is stored within the TreeMaker
application. The first time TreeMaker is run, it will create a preferences file
at ~/Library/Preferences/TreeMaker 5 Prefs.

4.5. Uninstalling TreeMaker

To uninstall TreeMaker, drag the TreeMaker application to the Trash and empty
the Trash. You can also delete the TreeMaker 5 Prefs file, which is found at
~/Library/Preferences/TreeMaker 5 Prefs.

BUILDING and INSTALLING TreeMaker 5.0 for Linux
  by Carlos A. Furuti

1. INTRODUCTION

This file describes basic procedures about building from source, installing and
running TreeMaker 5.0 for Linux (TM5L for short). It is also valid for most
Unix-like environments with GNU tools. It is not needed for installing and
using a TM5L binary bundle.

This and all other files in this directory and its subdirectories (except when
explicitly stated otherwise, as it may be the case with wxWidgets patches) are
subject to the same license conditions mentioned in the Source/README.txt file,
here referred to as the main README.

Linux is a registered trademark of Linus Torvalds.

2. BEFORE YOU START

Requisites for building TM5L:
- TreeMaker 5.0 requisites (see main README)
- a reasonably recent Linux platform with an installed X11 subsystem
  and its development libraries
- GCC 4.0.0 or better, with C++ libraries
- the GTK2 library
- GNU Make or compatible
- wxWidgets (TM5L was tested with the wxGTK version), as described below

TM5L was mainly ported and tested on a Fedora Core 4 distribution,
running on a i386-compatible system. Figures for disk space requisites
below are valid for this platform only.

3. TreeMaker CONFIGURATIONS

TM5L may be built in four configurations, decided by a command-line
parameter to "make". You'll find more detail about each configuration
in the main README:

- with "BUILD=release" (default): production version
- with "BUILD=profile": adds performance measurements
- with "BUILD=devel": partial logging and error reporting
- with "BUILD=debug": full logging and error reporting

4. BUILDING AND INSTALLING TM5L FROM SOURCE

4.1. Compiling wxWidgets

TreeMaker 5.0's user interface uses the multiplatform open-source
library wxWidgets. See the "wxwidgets" subdirectory about properly
building the library, _even_ if you already have wxWidgets installed.

4.2. Configuring your Build

After wxWidgets is ready, edit the Makefile in this directory and, if
necessary, change the few initial variables:

- WXHOME is the wxWidgets source directory (it should contain the
  build-debug and build-release subdirectories)
- PROGRAM is the name the user will type for invoking TM5L
- INSTALL_PREFIX is the directory prefix for installation. Usually it
  is /usr/local, meaning TM5L's binary program will be installed in
  /usr/local/bin, and its auxiliary files somewhere in /usr/local/share.
  Auxiliary files may be shared by all configuration types.

4.3. Building TreeMaker

After configuring the Makefile, just type "make", optionally followed
by the desired configuration (e.g., type "make BUILD=profile" for
building a profiled configuration). With no configuration mentioned,
"make" will always build the normal "release" configuration,
appropriate for end users.

Only while building the TM5L program and auxiliary files, a "build"
subdirectory will be created, requiring additional disk space: about
12MB, 55MB, 66MB and 96MB for the release, profile, development and
debug configurations, respectively. All configurations may be built on
the same system.

4.4 Installing TreeMaker

After building, repeat the "make" command with an "install" target
(e.g., type "make BUILD=profile install") for installing TM5L in the
proper directories. You'll need write permission for those directories
and additional (permanent this time) disk space (roughly 7MB, 22MB,
30MB and 60MB for the release, profile, development and debug
configurations, respectively). Unless you specified different
INSTALL_PREFIXes, only one configuration may be installed at a time.

4.5. Building a Distribution Bundle

You may optionally build a binary distribution bundle, i.e., a
single-file, self-contained, self-extracting shell script which when
executed will install a prebuilt TM5L configuration. This step is not
needed if you installed TM5L in your own system as described above.

Build a bundle typing "make", plus the optional configuration, and a
target "dist" (e.g., "make dist").

Only while building a distribution bundle, additional disk space is
required beyond that needed when building the program: roughly 8MB,
20MB, 22MB and 40MB for the release, profile, development and debug
configurations, respectively. All four bundles may coexist.

4.6. TM5L includes some stand-alone, command-line tests which do not
involve the GUI. They be may used during development and testing to
check TM5L's mathematical and input/output features. To create them,

4.6.1. edit Source/test/tmModel/Tester/tmModelTester.cpp. Look for the
       variable "testdir" and change it so that it'll point to that
       directory when you execute the test (I think it's easier
       setting it to "./" and changing to that directory later)

4.6.2. run "make" with the "tests" target, like in "make BUILD=debug
       tests"

The Makefile does _not_ automatically run tests.

4.7. Cleaning the Build Directories

After installing TM5L (and/or backing up its distribution bundle) you
may run "make" (with the appropriate configuration) and the "clean"
target to remove temporary files while building TM5L, or more simply
just remove the whole "build" subdirectory.

After testing the installed application, you may of course remove the
whole source directory too.

5. RUNNING TM5L

5.1. If You Chose the Default Settings

Just type the application name configured in your Makefile (like
"TreeMaker"), adding a pathname if needed. Add "-h" to see the
available command-line options.

5.2. A Note on Installation Prefixes

Apparently, wxWidgets's machinery for standard paths on Linux assumes
all auxiliary files are in a "shared" directory. TM5L defines a
subdirectory of "shared" where its auxiliary files are stored. If for
some reason you couldn't install TM5L on a standard prefix like /usr,
/opt or /usr/local, you can set it at runtime with the "-d" option or
the environment variable TREEMAKER_PREFIX. Note that you can't specify
the exact subdirectory, only the prefix.

6. UNINSTALLING TM5L

There is yet no smart procedure for uninstalling TM5L, but a small
"uninstall" script is installed together with TM5L's auxiliary files.
Just run it.


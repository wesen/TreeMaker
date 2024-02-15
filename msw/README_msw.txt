Building and Installing TreeMaker 5.0 on Modern Windows Systems (Including Windows 11)
  by Wlodzimierz 'ABX' Skiba and Arsen Aldea

  

1. INTRODUCTION


This file describes the basic procedures for building from source, installing
and running TreeMaker 5.0 for Windows (TM5W for short). It is not needed for
installing and using a TM5W binary bundle.

The update for this file includes instructions for modern development environments and Windows versions up to Windows 11. 
It addresses common issues encountered when compiling with recent versions of MinGW-w64 and updates to the wxWidgets library.

This and all other files in this directory are subject to the same license
conditions mentioned in the Source/README.txt file, here referred to as the
main README.

Windows is a registered trademark of Microsoft Corp.

2. BEFORE YOU START

Requisites for building TM5W:
- TreeMaker 5.0 requisites (see main README)
- MinGW64, updated instructions for installation are provided below.
- wxWidgets library, ensuring compatibility with the latest compilers and Windows 11as described below

TM5W was mainly developed and tested on Windows 2000 and XP SP2. TM5W should
work on other MS Windows flavours thanks to using wxWidgets toolkit.

2.1 UPDATED INSTALLATION PROCESS FOR MODERN SYSTEMS
Installing MinGW64:
For Windows 11 and other modern systems, MinGW64 installation has been updated. 
Visit https://winlibs.com/ and download the latest release version under UCRT runtime, recommended: GCC 13.2.0 + LLVM/Clang/LLD/LLDB 17.0.6 with MinGW-w64 11.0.1 (UCRT) - release 5. 
Choose the Win64 version for 64-bit systems.

Environment Variables:
Ensure your PATH environment variable includes the bin directory of MinGW64 to use GCC globally. 
Update WXWIN to point to your wxWidgets installation directory.

3. TreeMaker CONFIGURATIONS

TM5W build was tested in two major configurations, debug and release, but there
is the possibility of mixing build settings into more combinations. 

with "WX_DEBUG=0 TMBUILD=release" : production version
with "WX_DEBUG=1 TMBUILD=debug"   : development version

There are more options and some are related to the wxWidgets build. They are
all listed in the beginning of msw/makefile.gcc file together with a short
description.

4. BUILDING AND INSTALLING TM5W FROM SOURCE

4.1. Compiling wxWidgets

First prepare for a clean build.clean environment for building TreeMaker 5.0, especially after previous build attempts, 
it's crucial to perform a clean command using MinGW.

Clean Command for wxWidgets that should be ran in \build\msw
mingw32-make.exe -f makefile.gcc clean



TreeMaker 5.0's user interface uses the multiplatform open-source
library wxWidgets. For maximum portability, TM5W is statically linked.
The complete command used to build wxWidgets for release of TM5W was:

mingw32-make.exe -f makefile.gcc SHARED=0 RUNTIME_LIBS=static UNICODE=1 MONOLITHIC=0 BUILD=release

Note that not all features of wxWidgets are used in TM5W. Unused features can
be turned off before a wxWidgets rebuild. This is done by providing a modified
setup0.h. After obtaining a fresh wxWidgets copy, this file should be copied
from msw/setup0.h to wxWidgets/include/wx/msw/setup0.h path.

4.1.1 Preparing Treemaker for Modern Compilers
If these changes to Treemaker src code are not made you will be encountering errors when running the make file.
 Follow these steps to address common errors encountered during the build process:

Error Fixes:

'erase' Not Declared: Qualify erase with this->erase to specify the vector's erase function in tmArray.h.

'strcpy' Not Declared: Include <cstring> in files where strcpy is used to ensure proper declaration.

'isnan' Not Declared: Ensure <cmath> is included and use std::isnan to check for 'Not a Number' values.

String Literal to char Assignments:* Use const char* for string literals to adhere to modern C++ standards.

'insert' Not Declared: Use this->insert in tmArray.h to clarify the call to the vector's insert method.

make sure you skip the warnings for now since fixing them will make this Read.me 5 more pages
Simply focus on fixing any errors encountered

4.1.2 Updating the Makefile for Modern Compilers
Next the makefile.gcc needs to be updated to connect your WXwidgets to the Treemaker project
Modify the CPPFLAGS in your makefile to include paths to wxWidgets headers and libraries, 
ensuring the correct setup.h file is used. Replace placeholders with your actual directory paths:

CPPFLAGS = -I"path\to\your\wxWidgets\include" \
-I"path\to\your\wxWidgets\include\wx\msw" \
-I"path\to\your\wxWidgets\lib\gcc_lib\mswu"


4.2. Configuring your Build

Building TM5W requires the environment variable WXWIN pointing to wxWidgets path.

4.3. Building TreeMaker

Again do not forget to do a clean command before building Treemaker.
this command should be ran inside the msw folder with the makefile.gcc

mingw32-make.exe -f makefile.gcc clean


The complete command used to build wxWidgets for release of TM5W was:

mingw32-make.exe -f makefile.gcc WX_SHARED=0 WX_UNICODE=1 WX_DEBUG=0 PROFILE=0 TMBUILD=release

This command will create a subdirectory in the msw directory of TM5W and will
create there 5 command line test tools and the main executable.

4.4 Installing TreeMaker

You can run TM5W from any path (including the subdirectory where it was
created) but you need to put in the same directory the splash screen, zip
archive with help files, and the about page. They are all located under Source
directory of TM5W and need to be copied into the executable path.

4.5. Building a Distribution Bundle

The Windows installer is made with Inno Setup, which is available at
http://www.jrsoftware.org/isinfo.php . The required script is located in
msw/treemaker.iss. Also, the mingwm10.dll runtime library from MinGW is
required for running the executable.

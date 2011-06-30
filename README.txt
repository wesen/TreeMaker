/*******************************************************************************
TreeMaker 5.0

A Program for Origami Design

Contact:
  web: http://www.langorigami.com/treemaker/treemaker.php4
  email: treemaker@langorigami.com
  
Developers:
  Robert J. Lang (lead)
  Carlos Furuti (Linux port)
  Wlodzimierz 'ABX' Skiba (MSW port)
  
Special thanks:
  The wxWidgets Team
  Stefan Csomer (wxMac guru)
  Will Naylor and Bill Chapman (authors of wnlib)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program (in a file called LICENSE.txt); if not, go to
http://www.gnu.org/copyleft/gpl.html or write to 

  Free Software Foundation, Inc.
  59 Temple Place - Suite 330
  Boston, MA 02111-1307 USA

Source code to this program is always available; for more information visit my
website at:

  http://www.langorigami.com

/*******************************************************************************
Information for Programmers

TreeMaker consists of several projects that can be built individually or
together. Source code and headers for all projects is contained within the
Source directory. TreeMaker can be built as a model-only library or as the
full application with GUI. All the code to build the model is in the included
files. However, you will need to download and build the wxWidgets libraries
(www.wxwidgets.org) in order to build the TreeMaker application with GUI.

Source contains the following subdirectories:

Source/tmModel contains the complete TreeMaker mathematical model, no GUI code. 
It does not require wxWidgets.

Source/tmwxGUI contains the TreeMaker GUI code, which is built on top of
wxWidgets.

Source/test contains some test files that allow you to compile and test 
portions of the TreeMaker model. Most of them do not require wxWidgets.

Source/images contains images (like the splash screen).

The file tmHeader.h is a required prefix headers for all code and can be
precompiled using tmPrec.cpp. File tmHeader.cpp contains the implementation of
assertion and log code common to all files.

********************************************************************************
To build TreeMaker:

Detailed build instructions for Macintosh, Linux, and Windows are given within
the platform-specific directories (mac, linux, and msw, respectively). Your
best (and easiest) bet is to use one of the build systems we've already set up.
But if you're using a different build system or building TreeMaker for a
different platform, the overall process is the following.

Download and build wxWidgets from www.wxWidgets.org, version 2.6.2 or higher.

Configure the libraries with the following options:

--enable-std_iostreams --enable-std_string

If you are building wxMac, you will also need 

--with-mac --disable-shared --enable-monolithic 

If you are building wxGTK and have GNOME installed, you should use

--with-gnomeprint

which will give improved printing and improved ability to convert printouts to
PDF.

and if you are building debug versions of the app (on any platform), you will
need

--enable-debug

See the wxWidgets home page (www.wxwidgets.org) for more details on building
wxWidgets or see the platform-specific README files for more information.

Once you've built the wxWidgets libraries, create a new wxWidgets project and
add the TreeMaker source code to it. How you do this depends on what
development environment you are using (e.g., makefiles vs. IDE).

Whatever you use, you will need to pass some preprocessor symbols to the
compiler to specify the type of build and whether wxWidgets is available. The
relevant symbols are:

__WXDEBUG__ (which is a wxWidgets-defined symbol)
TMDEBUG
TMPROFILE
TMWX

Read further for description of what these symbols do and what combinations are
allowed. For a release build of the full app, #define only TMWX.

Note that if __WXDEBUG__ is defined, you will need to link against the wxWidgets
debug libraries, rather than the release libraries.

You will also need to build the help file (help.zip). Essentially, this
consists of putting the contents of the Source/help folder into a ZIP archive
and creating a cache file using the wxWidgets utility hhp2cached. The location
of these two files is, in general, platform-dependent.

You will also need to copy some contents of the images, resources, and about
folder to locations that are also platform-dependent. Examine the code
for function twmxApp::OnInit() and the wxWidgets documentation to figure out
the appropriate locations for your system.

********************************************************************************
Build styles and preprocessor symbols

The code supports several different build configurations for various stages of
development. The configurations are implemented via compile symbols. The
following is a description of the configuration, how it is implemented, and its
purpose.

Note that you can build the model without wxWidgets in any of the different
styles, but if you do so, you won't link against any wxWidgets libraries
(obviously) and won't define compiler symbol TMWX.

Debug Configuration

  Compiler settings: Define  symbols __WXDEBUG__ and TMWX, no optimizations,
  generate symbols.
  Link against: wxWidgets debug libraries
  Purpose: For debugging either TreeMaker model or GUI using a debugger like
  GDB. Includes full debug symbols for the wxWidgets library, allowing one to
  step into wxW with the debugger. Assertions are tested and break into the
  debugger on failure.

Development Configuration

  Compiler settings: Define symbols TMDEBUG and TMWX, no optimizations,
  generate symbols.
  Link against: wxWidgets release libraries
  Purpose: For early testing by clients. No optimizations, so won't run at max
  speed. Includes debug symbols in the TreeMaker code, but not wxWidgets.
  Assertions are tested; they put up a dialog indicating where failure
  occurred, then program terminates.

Profile Configuration

  Compiler settings: Define symbols TMPROFILE and TMWX, full optimizations,
  generate symbols.
  Link against: wxWidgets release libraries
  Purpose: For profiling and speed optimization. Contains full speed
  optimizations, but still generates symbols so that you can use profiling
  tools. Also writes timing information to the log window (or console, for
  non-TMWX builds). Assertions are compiled out.

Release Configuration

  Compiler settings: Define symbol TMWX, full optimizations, no symbols.
  Link against: wxWidgets release libraries
  Purpose: For final deployment. Assertions are compiled out, no timing
  information is generated, no log messages go anywhere.

********************************************************************************
Test Code

The test files are built as command-line tools and are used to try out and
debug various pieces of the TreeMaker model (no GUI). The test files don't
require all of the source code; the required .cpp files are listed below (if a
folder is listed, compile every .c or .cpp file in the folder). Header search
paths should always include all folders in Source (recursively). Do NOT
#define TMWX for any of the test code.

tmArrayTester.cpp
  Tests the general-purpose container class used throughout TreeMaker.
  Requires:
    tmHeader.cpp
    tmModel/tmPtrClasses
    
tmDpptrTester.cpp
  Tests the dangle-proof pointer and array classes.
  Requires:
    tmHeader.cpp
    tmModel/tmPtrClasses

tmNewtonRaphsonTester.cpp
  Tests the Newton-Raphson root-finding algorithm.
  Requires:
    tmHeader.cpp
    tmModel/tmPtrClasses

tmNLCOTester.cpp
  Tests the nonlinear constrained optimization algorithms.
  Requires:
    tmHeader.cpp
    tmModel/tmPtrClasses
    tmModel/tmNLCO
    tmModel/wnlib

tmModelTester.cpp
  Tests the full TreeMaker model, including optimizers, from the command line.
  The program reads in several test files that are included in the tmModelTester
  folder; you will probably have to adjust the path variable or move the test
  files to the same folder where the app is located.
  Requires:
    tmHeader.cpp
    tmModel (all subdirectorys)

********************************************************************************
Supported Platforms and environments

TreeMaker has been successfully built in the following environments:

Mac OS X -- XCode 2.2 (Apple developer tools). Use TreeMaker.xcodeproj as your
project file. Note that the wxWidget libraries must be built with Mac-specific
options; for simplicity, use the script files in the mac subdirectory. Check
mac/README_mac.txt for further details about building.

Linux -- i386, GCC 4.0.0, GTK+-2.0. Instructions for compiling wxWidget are 
included.

Windows -- MinGW run from windows command box. Use msw/makefile.gcc. Note that
the wxWidget libraries must be built first and that input options passed to
msw/makefile.gcc should match wxWidgets build settings. Check
msw/README_msw.txt for further details about building.


********************************************************************************
Headers, implementations, and comments

As is typical in C++, most class functions are declared in a header (.h) file
and defined in an implementation (.c or .cpp) file. Generally, the purpose of
each function is described in its implementation, which is usually the .cpp
file.

The exception is functions that are defined inline in their header file; they
have a short description within the body of the inline definition.

Class member variables usually have an associated getter function (e.g.,
tmVertex::mDepth has the getter tmVertex::GetDepth()) and the descriptions of
the member variables are usually given with the getter function, rather than at
the point of declaration of the class member variable.

********************************************************************************
TreeMaker names and conventions

Classes are implemented in .h and .cpp files. Thus, class MyClass will have its 
declaration in "MyClass.h" and its implementation in "MyClass.cpp".

Usually "MyClass.cpp" will contain the full implementation for class MyClass.
Most modern filesystems can handle long names, so this convention is used even 
with classes like "tmwxConditionEdgesSameStrainPanel". I tend to favor long 
names that convey what they're used for, rather than cryptic combinations of 
letters whose only positive feature is that they survive in DOS.

In a few cases, I have broken up implementations into several files; these will
have their names extended with "_Usage", e.g., "MyClass_IO.cpp",
"MyClass_Cleanup.cpp", and so forth. Conversely, in a few cases I have grouped
closely-related classes together.

The TreeMaker code can be divided up into several groups:

(1) the TreeMaker mathematical model (tmModel);
(2) The wxWidgets class library (wxW);
(3) the TreeMaker GUI (tmwxGUI).

Which group a particular class or file belongs in can (usually) be inferred 
from its name. The groups are discussed below.

(1) The TreeMaker mathematical model implements the underlying data structure
and provides operations on that data structure through the TreeMaker API. It is
intended to be portable and I make minimal assumptions about the user
interface. Interaction with a UI can be provided by calls to the API, by
subclassing, and use of exceptions (to handle run-time errors). tmModel uses
exceptions, RTTI, and template specialization and so requires a fairly
ANSI-compliant compiler. (I have built it with Metrowerks CodeWarrior and gcc
4.0.)

Every exposed class within tmModel has a class name with a prefix of "tm", 
e.g., tmPoint, tmFloat, tmNode, etc. So if you see tmDoohickey, you know 
that it's part of the model, not the GUI.

(2) The wxWidgets class library is a portable multiplatform GUI tool set. 
Most of the classes and constants in wxWidgets have a prefix of "wx", 
e.g., classes wxWindow, wxTextCtrl; constants wxYES, wxNO, and so forth. 
So wxDoohickey is something from wxWidgets.

(3) The TreeMaker GUI (tmwxGUI) is built on top of the wxWidgets tool set and 
interacts with the TreeMaker mathematical model. I use a prefix scheme for 
GUI classes that is a hybrid of the TreeMaker "tm" and wxWidgets "wx" 
prefices: Namely, all such classes have a prefix "tmwx", e.g., classes 
tmwxApp, tmwxDesignCanvas. If a TreeMaker GUI class has  a name similar 
to a wxWidgets class, it's a pretty safe bet that the former is directly 
descended from the latter; e.g.,

class tmwxApp : public wxApp {
// ...
};

Within both the tmModel and tmwxGUI, I have adopted a few naming conventions 
that are, for the most part, pretty standard.

The two- or four-letter prefix "tm" or "tmwx" is used for the names of classes,
structs, and namespaces in the global namespace, but (usually) not functions,
constants, or variables. Classes that are local to another class, struct, or
namespace usually do not have a prefix.

All classes begin with an uppercase letter, optionally prefixed by "tm" or
"tmwx" as described earlier. All functions begin with an upper-case letter.

All variables begin with a lower-case letter.

Multi-word class, function, and variable names use interCaps to delineate words
(as opposed to, for example, underscores); thus,
"tmwxConditionEdgesSameStrainPanel", not
"tmwx_condition_edges_same_strain_panel".

Most class member variables begin with lower-case "m" followed by a further
descriptive name that begins with upper case: e.g., tmNode::mLoc,
tmNode::mPath. If a member variable is a reference to another class, the member
variable will be called "m" + (classname - prefix); for example, tmNode::mPath
is a tmPath*. Additional words are used in member names to distinguish multiple
variables of the same type, e.g., mLeafNode, mFirstNode, mLastNode, etc. I do
not name object and pointer-to-object member variables differently (e.g., obj
vs pObj) as it's usually clear which a member variable is.

Static member variables begin with lower-case s, e.g., tmNode::sTag.

There are a few exceptions to the mSomething scheme; some lightweight 
mathematical objects have member variables that are meant to be directly 
accessed and have short lower-case names, like tmPoint::x and tmPoint::y.

Function arguments that need no special distinction will usually be named 
"a"+classname, e.g., "aMyClass".

Temporary variables within a function (and frequently loop variables) are often
named "the"+classname, unless, of course, there's more than one, in which case
more descriptive names are used.

Within the tmModel, most function arguments and member variables are pointers 
for historical reasons, even though in some cases reference variables 
might be more appropriate. wxWidgets uses a mixture of pointers and 
references, and the tmwxGUI similarly uses both.

The following things usually have names in ALL_CAPS:
	macros
	constants and enum values
	classes with no members (e.g., exception and template resolution classes)
	
There are exceptions to all these rules. TreeMaker contains code written over
more than a decade, and if old code works, I don't go back and clean it up,
even if it's not perfectly consistent with the naming rules.

*******************************************************************************/

/*******************************************************************************
File:         tmHeader.h
Project:      TreeMaker 5.x
Purpose:      Common header file for all TreeMaker source code
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMHEADER_H_
#define _TMHEADER_H_

/*
Every TreeMaker source file includes this in its header tree. This file can be
precompiled. (Since it can be included from both C and C++ files, it is
standard C.) Note the symbols below that may be predefined in compilation:
TMWX
__WXDEBUG__
TMDEBUG
TMPROFILE

You may also use the following macros:
TMASSERT(cond) -- to perform assertion tests.
TMLOG(text) -- to send information to a log window or standard output.

See discussion below for more details.
*/

/**********
wxWidgets

#define TMWX if the project is being built with the wxWidgets libraries,
whether it is a Debug, Development, or Profile build. 
Command-line test code should not #define TMWX. 

Note that the wxWidgets headers are (apparently) NOT compatible with standard C
(rather than C++), so if this is getting #included from a C file (from some
external library), we're going to turn off TMWX and ignore the wxW headers.
**********/
#ifndef __cplusplus
  #ifdef TMWX
    #undef TMWX
  #endif /* TMWX */
#endif /* __cplusplus */
#ifdef TMWX
  /* For compilers that support precompilation, this includes "wx/wx.h". */
  #include "wx/wxprec.h"
  
  #ifdef __BORLANDC__
    #pragma hdrstop
  #endif
  
  #ifndef WX_PRECOMP
    #include "wx/wx.h"
  #endif
  
  /* Version -- make sure we're using an up-to-date version of wxW. */
  #if !wxCHECK_VERSION(2, 6, 1)
    #error "Sorry, you need to use at least version 2.6.1 of wxWidgets."
  #endif
  
  /* Build options -- make sure we turned on the right options when building 
    wxW */
  #if !wxUSE_STD_IOSTREAM
    #error "Please configure with --enable-std_iostreams or set \
      wxUSE_STD_IOSTREAM to 1 in wx/setup.h."
  #endif

  /* This avoids a collision between native windows headers and
    wnlib wrt symbol WN_SUCCESS */
  #ifdef __WXMSW__
      #include <winnetwk.h>
      #ifdef WN_SUCCESS
          #undef WN_SUCCESS
      #endif
  #endif
#endif


/**********
Assertions, debugging, and profiling

We define several macros that are used in debugging and/or profiling that
behave differently under various compile conditions. The macros are:

TMASSERT(cond) where cond is a condition that must always be true
TMLOG(text) where text is a string containing information to go to the
  debugging window or stdout.

The behavior of these macros is different depending on the compiler symbols
TMWX, __WXDEBUG__, TMDEBUG, and TMPROFILE.

You should #define TMWX if wxWidgets is being used; in addition, you should
#define no more than one of __WXDEBUG__, TMDEBUG, or TMPROFILE.

__WXDEBUG__ -- define this for debug builds using the wxWidgets library. This
              build enables all assertions and uses the wxWidgets assertion
              tools that break into the debugger on assertion failure.
              
TMDEBUG     -- define this for debug builds that don't include wxWidgets and
              for Development builds given to clients for user testing. This
              enables all assertions within TreeMaker code. Assertion failure
              gets reported to the log window (if it exists) or to stdout (if
              it doesn't).
              
TMPROFILE   -- define this for profiling builds, which will typically have all
              compiler optimizations, but which may still throw timing info
              to the log window or stdout.
              
            -- Don't define any of the three of these for release builds. No
              assertions will be enabled and all log messages will be ignored.
**********/

/*****
If __WXDEBUG__ is defined, then TMDEBUG will automatically be defined.
Either of these symbols specifies that it's a debugging build.
*****/
#if defined(__WXDEBUG__) && ! defined(TMDEBUG)
  #define TMDEBUG
#endif

/*****
TM_USE_LOGFRAME tells us to send log messages to the log frame; if it doesn't
exist, they'll go to stdout.
*****/
#ifdef TMDEBUG
  #ifdef TMWX
    #define TM_USE_LOGFRAME
  #endif
#endif
#ifdef TMPROFILE
  #ifdef TMWX
    #define TM_USE_LOGFRAME
  #endif
#endif

/*****
TM_LOG_ENABLED is defined if we accept log messages. If it doesn't exist, then
TMLOG() macro evaluates to nothing. Everything but release builds accept log
messages.
*****/
#ifdef TMDEBUG
  #define TM_LOG_ENABLED
#endif
#ifdef TMPROFILE
  #define TM_LOG_ENABLED
#endif

/*****
TM_UI_CHAR is the character type used for the user interface. Use wxChar with
wxWidgets, ordinary C char otherwise.
*****/
#ifdef TMWX
  #define TM_UI_CHAR wxChar
  #define TM_UI_T(cstr) wxT(cstr)
  #define __TM_FILE__ __TFILE__
#else
  #define TM_UI_CHAR char
  #define TM_UI_T(cstr) cstr
  #define __TM_FILE__ __FILE__
#endif


/*****
TMASSERT() macro - used to test conditions that should be true (and about which
subsequent code will make assumptions).
TMFAIL() macro - used when program execution reaches a place that it shouldn't:
For example, a switch() statement that reaches the default state when it
shouldn't. The argument is a string giving the reason for failure. TMFAIL(msg)
is equivalent to TMASSERT(false) but lets us give a reason for the failure.
*****/
#ifdef __cplusplus
  #if defined (TMDEBUG)
    /* Debug and Development builds test & respond to assertions */
    void tmOnAssert(int cond, const TM_UI_CHAR* szFile, int nLine, 
      const TM_UI_CHAR* szCond, const TM_UI_CHAR* szMsg = 0);
    #define TMASSERT(cond) \
      tmOnAssert(!!(cond), __TM_FILE__, __LINE__, TM_UI_T(#cond))
    #define TMFAIL(msg) \
      tmOnAssert(0, __TM_FILE__, __LINE__, TM_UI_T("fail"), TM_UI_T(msg))
  #else
    /* Profile and Release builds compile out assertions */
    #define TMASSERT(cond)
    #define TMFAIL(msg)
  #endif /* defined (TMDEBUG) */
#endif /* __cplusplus */


/*****
TMLOG(str) macro - used to dump textual information somewhere: to the log frame,
if it exists, or to the console if it doesn't. Only in Debug, Development,
and profile builds. Note that if wxWidgets is present, you can pass wxStrings
to the log macro; otherwise, only C and C++ strings.
*****/
#ifdef TM_LOG_ENABLED
  #define TMLOG(text) tmOnLog(text)
  void tmOnLog(const char* c);
  #ifdef __cplusplus
    #include <string>
    void tmOnLog(const std::string& s);
  #endif /* __cplusplus */
  #ifdef TMWX
    void tmOnLog(const wxString& ws);
  #endif /* TMWX */
#else
  #define TMLOG(text)
#endif /* TM_LOG_ENABLED */


/**********
typedef tmFloat
Set the floating-point precision used throughout the TreeMaker model.
**********/
#ifdef __DMC__
  /* STL of DMC has math functions for 'float' or 'long double', so we need one
  of them to avoid "ambiguous reference to symbol" errors in math functions */
  typedef float tmFloat;
#else
  typedef double tmFloat;
#endif


/**********
TM_CHECK_NAN(x)
Checks for NaN in debugging builds in numbers and (in C++) std::vector<T>
**********/
#ifdef TMDEBUG
  void tmCheckNaN(double x);
  #ifdef __cplusplus
    #include <vector>
    template <class T>
      void tmCheckNaN(const std::vector<T>& v) {
        for (size_t i = 0; i < v.size(); ++i)
          tmCheckNaN(v[i]);
      };
  #endif /* __cplusplus */
  #define TM_CHECK_NAN(x) tmCheckNaN(x)
#else
  #define TM_CHECK_NAN(x)
#endif /* TMDEBUG */


#endif /* _TMHEADER_H_ */

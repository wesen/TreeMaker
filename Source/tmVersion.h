/*******************************************************************************
File:         tmVersion.h
Project:      TreeMaker 5.x
Purpose:      Version macros used in About box and Info.plist on Mac
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-09
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMVERSION_H_
#define _TMVERSION_H_

/*****
Define version macros as symbols and strings. Nothing below these four lines
needs to be changed.
*****/

#define TM_VERSION_MAJOR 5
#define TM_VERSION_MINOR 0
#define TM_VERSION_RELEASE 1
#define TM_VERSION_BUILD 20060413

/******************************************************************************/
 
/* Symbols needed for treemaker.plist; ugly multistage construction because
  they need to be symbols, not strings, no quotes, no whitespace */
#define TM_PASTE_5(a, b, c, d, e) a##b##c##d##e
#define TM_PASTE_5A(a, b, c, d, e) TM_PASTE_5(a, b, c, d, e)
#define TM_MAC_CFBUNDLE_SHORT_VERSION_STRING \
  TM_PASTE_5A(TM_VERSION_MAJOR, ., TM_VERSION_MINOR, ., TM_VERSION_RELEASE)
#define TM_PASTE_7(a, b, c, d, e, f, g) a##b##c##d##e##f##g
#define TM_PASTE_7A(a, b, c, d, e, f, g) TM_PASTE_7(a, b, c, d, e, f, g)
#define TM_MAC_CFBUNDLE_VERSION \
  TM_PASTE_7A(TM_VERSION_MAJOR, ., TM_VERSION_MINOR, ., TM_VERSION_RELEASE, \
  ., TM_VERSION_BUILD)

/* Name of the application family */
#define TM_NAME_STR "TreeMaker"

/* Couple of useful macros. 1st version returns argument name as string. 2nd
  version returns argument value as string. */
#define TM_STRINGIZE(x) #x
#define TM_STRINGIZEA(x) TM_STRINGIZE(x)

/* Build code as string */
#define BUILD_STR TM_STRINGIZEA(TM_VERSION_BUILD)

/* Name of the executable */
#define TM_MAKE_APP_NAME_STR(x) \
  TM_NAME_STR " " TM_STRINGIZE(x)
#define TM_APP_NAME_STR \
  TM_MAKE_APP_NAME_STR(TM_VERSION_MAJOR)

/* Name with long version */
#define TM_MAKE_APP_LONG_VERSION_NAME(y, z) \
  TM_APP_NAME_STR "." TM_STRINGIZE(y) "." TM_STRINGIZE(z)
#define TM_APP_LONG_VERSION_NAME_STR \
  TM_MAKE_APP_LONG_VERSION_NAME(TM_VERSION_MINOR, TM_VERSION_RELEASE)

/* Name with long version and build date */
#define TM_MAKE_APP_BUILD_NAME_STR(b) \
  TM_APP_LONG_VERSION_NAME_STR "." TM_STRINGIZE(b)
#define TM_APP_BUILD_NAME_STR \
  TM_MAKE_APP_BUILD_NAME_STR(TM_VERSION_BUILD)

/* Long name used in about box */
#if defined(__WXDEBUG__)
  #define TM_APP_ABOUT_NAME_STR TM_APP_BUILD_NAME_STR " (Debug)"
#elif defined(TMDEBUG)
  #define TM_APP_ABOUT_NAME_STR TM_APP_BUILD_NAME_STR " (Development)"
#elif defined(TMPROFILE)
  #define TM_APP_ABOUT_NAME_STR TM_APP_BUILD_NAME_STR " (Profile)"
#else
  #define TM_APP_ABOUT_NAME_STR TM_APP_LONG_VERSION_NAME_STR
#endif

#endif // _TMVERSION_H_

/*******************************************************************************
File:         tmHeader.cpp
Project:      TreeMaker 5.x
Purpose:      Defines failed assertion behavior for Development builds
Author:       Robert J. Lang
Modified by:  
Created:      2005-08-24
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/
#include "tmHeader.h"
#ifdef TMWX
  #include "tmwxStr.h"
  #include "tmwxLogFrame.h"
#endif // TMWX

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>

using namespace std;

/**********
ASSERTIONS
**********/

#ifdef TMDEBUG
/*****
Global stream that records state for dumping
*****/
stringstream& DbgPreBuildCreasesStringStream()
{
  static stringstream sDbgPreBuildCreasesStringStream;
  return sDbgPreBuildCreasesStringStream;
}


/*****
Global stream that records state for dumping
*****/
stringstream& DbgPreCleanupStringStream()
{
  static stringstream sDbgPreCleanupStringStream;
  return sDbgPreCleanupStringStream;
}


/*****
Dump two tree state stringstreams to crash files.
*****/
void DumpTreeState()
{
  ofstream fout1("pre_build_creases.tmd5");
  fout1 << DbgPreBuildCreasesStringStream().str();
  fout1.close();
  ofstream fout2("pre_cleanup.tmd5");
  fout2 << DbgPreCleanupStringStream().str();
  fout2.close();
}


/*****
Test the assertion condition; if false, report the reason for failure and exit
the program.
*****/
void tmOnAssert(int cond, const TM_UI_CHAR* szFile, int nLine, 
  const TM_UI_CHAR* szCond, const TM_UI_CHAR* szMsg)
{
  if (cond) return;
  DumpTreeState();

  // Exact failure mechanism depends on build options
#ifdef __WXDEBUG__
  wxOnAssert(szFile, nLine, szCond, szMsg);
#else
  #ifdef TMWX
    wxString msg;
    if (szMsg)
      msg.Printf(wxT("Failed assertion in file %s ")
        wxT("at line %d. Failed condition was:\n")
        wxT("  %s: %s"), szFile, nLine, szCond, szMsg);
    else 
      msg.Printf(wxT("Failed assertion in file %s ")
        wxT("at line %d. Failed condition was:\n")
        wxT("  %s"), szFile, nLine, szCond);
    wxMessageBox(msg, wxT("Failed Assertion"));
    wxExit();
  #else
    char msg[1024];
    if (szMsg)
      sprintf(msg, "Failed assertion in file %s "
        "at line %d. Failed condition was:\n"
        "  %s: %s", szFile, nLine, szCond, szMsg);
    else
      sprintf(msg, "Failed assertion in file %s "
        "at line %d. Failed condition was:\n"
        "  %s", szFile, nLine, szCond);
    cout << msg << endl;
    abort();
  #endif // TMWX
#endif // __WXDEBUG__
}

/*****
For unicode builds push model asserts to unicode asserts.
*****/
#if defined(wxUSE_UNICODE) && wxUSE_UNICODE
void tmOnAssert(int cond, const char* szFile, int nLine, 
                const char* szCond, const char* szMsg)
{
  wxString strFile;  strFile.FromAscii(szFile);
  wxString strCond;  strCond.FromAscii(szCond);
  wxString strMsg;   strMsg.FromAscii(szMsg);
  tmOnAssert(cond, strFile.c_str(), nLine, strCond.c_str(), strMsg.c_str());
}
#endif // wxUSE_UNICODE

#endif // TMDEBUG


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
LOGGING FUNCTIONS
**********/

/*****
Report a C string to the log frame or stdout
*****/
#ifdef TM_LOG_ENABLED
  void tmOnLog(const char* c) {
  #ifdef TM_USE_LOGFRAME
    #ifdef __WXDEBUG__
      wxString msg(c, *wxConvCurrent);
      wxLogDebug(msg);
    #else
      tmwxLogFrame* logFrame = gLogFrame;
      if (logFrame) logFrame->AppendText(c);
    #endif // __WXDEBUG__
  #else
      cout << c << endl;
  #endif // TM_USE_LOGFRAME
  };
#endif // TM_LOG_ENABLED


/*****
Report a C++ string to the log frame or stdout
*****/
#ifdef TM_LOG_ENABLED
  void tmOnLog(const string& s) {
  #ifdef TM_USE_LOGFRAME
    #ifdef __WXDEBUG__
      wxString msg(s.c_str(), *wxConvCurrent);
      wxLogDebug(msg);
    #else
      tmwxLogFrame* logFrame = gLogFrame;
      if (logFrame) logFrame->AppendText(s);
    #endif // __WXDEBUG__
  #else
      cout << s << endl;
  #endif // TM_USE_LOGFRAME
  };
#endif // TM_LOG_ENABLED


/*****
Report a wxString to the log frame. We don't include a stdout option because
if you're not building with wxWidgets, you won't be logging wxStrings.
*****/
#ifdef TM_LOG_ENABLED
  #ifdef TMWX
    void tmOnLog(const wxString& ws) {
      #ifdef __WXDEBUG__
        wxLogDebug(ws);
      #else
        tmwxLogFrame* logFrame = gLogFrame;
      if (logFrame) logFrame->AppendText(ws);
      #endif // __WXDEBUG__
    };
  #endif // TMWX
#endif // TM_LOG_ENABLED


/**********
NAN DETECTION FUNCTIONS
**********/

/**********
Check for NaN in debug builds
**********/
#ifdef TMDEBUG
  void tmCheckNaN(double x) {
    if (isnan(x)) {
//     if (!(x == x)) { // use this if your compiler doesn't have isnan()
      // TBD, generate exception, or break here
      TMLOG("NaN encountered!");
    }
  }
#endif // TMDEBUG

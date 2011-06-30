/*******************************************************************************
File:         tmwxLogFrame.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmwxLogFrame
Author:       Robert J. Lang
Modified by:  
Created:      2005-02-19
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXLOGFRAME_H_
#define _TMWXLOGFRAME_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Forward declarations
#include "tmwxGUI_fwd.h"

// Addition headers
#include "tmwxPersistentFrame.h"

/**********
class tmwxLogFrame
A floating window that shows log info -- used for debugging
**********/
class tmwxLogFrame: public tmwxPersistentFrame<wxFrame> {
public:
  tmwxLogFrame();
  void AppendText(const wxString& ws);
  void AppendText(const std::string& s);
  void AppendText(const char* c);

  // Event handling
  void OnClose(wxCloseEvent& event);
  DECLARE_EVENT_TABLE()
  
private:
  wxTextCtrl* mTextCtrl;
  
  void GetPositionSizeInfo(Key key, wxString& keystr, int& val);
};

#endif //_TMWXLOGFRAME_H_

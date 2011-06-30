/*******************************************************************************
File:         tmwxLogFrame.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmwxLogFrame
Author:       Robert J. Lang
Modified by:  
Created:      2005-02-19
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmModel.h"
#include "tmwxLogFrame.h"
#include "tmwxDoc.h"
#include "tmwxStr.h"
#include "tmwxDocManager.h"

#include "wx/config.h"

/**********
class tmwxLogFrame
A window used in debugging to receive debug messages.
**********/

/*****
Constructor
*****/
tmwxLogFrame::tmwxLogFrame()
  : tmwxPersistentFrame<wxFrame>(wxT("Log"),
    wxFRAME_TOOL_WINDOW | wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX)
{
  mTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize,  wxTE_MULTILINE | wxTE_READONLY);
  mTextCtrl->SetFont(DEFAULT_FONT_10);
  wxLogTextCtrl* logTextCtrl = new wxLogTextCtrl(mTextCtrl);
  wxLog* oldTarget = wxLog::SetActiveTarget(logTextCtrl);
  if (oldTarget) delete oldTarget;

  // This comes last so that we already have a log target to receive debug
  // messages.
  InitPositionAndSize();
}


/*****
Append the passed text to the text in the log frame.
*****/
void tmwxLogFrame::AppendText(const wxString& ws)
{
  mTextCtrl->AppendText(ws + wxT("\n"));
}


/*****
Append the passed text to the text in the log frame.
*****/
void tmwxLogFrame::AppendText(const std::string& s)
{
  AppendText(s.c_str());
}


/*****
Append the passed text to the text in the log frame.
*****/
void tmwxLogFrame::AppendText(const char* c)
{
  AppendText(wxString::FromAscii(c));
}


/*****
Return the key string and default value for the given position/size component.
*****/
void tmwxLogFrame::GetPositionSizeInfo(Key key, wxString& keystr, int& val)
{
  switch(key) {
    case X_KEY:
      keystr = wxT("LogFrameX");
      val = 30;
      break;
    case Y_KEY:
      keystr = wxT("LogFrameY");
      val = 70;
      break;
    case W_KEY:
      keystr = wxT("LogFrameW");
      val = 280;
      break;
    case H_KEY:
      keystr = wxT("LogFrameH");
      val = 280;
      break;
    default:
      TMFAIL("unknown key encountered in "\
       "tmwxLogFrame::GetPositionSizeInfo(..)");
      break;
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Attempt to close should only hide the window except at shutdown.
*****/
void tmwxLogFrame::OnClose(wxCloseEvent& event)
{
  if (gApp->IsQuitting())
    event.Skip();
  else
    if (event.CanVeto()) {
      event.Veto();
      Hide();
    }
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxLogFrame, tmwxPersistentFrame<wxFrame>)
  EVT_CLOSE(tmwxLogFrame::OnClose)
END_EVENT_TABLE()

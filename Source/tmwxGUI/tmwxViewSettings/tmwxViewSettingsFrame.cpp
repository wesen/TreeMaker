/*******************************************************************************
File:         tmwxViewSettingsFrame.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for TreeMaker view settings frame
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-16
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxViewSettingsFrame.h"
#include "tmModel.h"
#include "tmwxViewSettingsPanel.h"
#include "tmwxDocManager.h"

#include "wx/config.h"

/**********
class tmwxViewSettingsFrame
Displays settings that affect what parts are shown and how they are displayed
**********/

/*****
tmwxViewSettingsFrame constants
*****/

/*****
Constructor creates the frame, which is a tool palette.
*****/
tmwxViewSettingsFrame::tmwxViewSettingsFrame()
  : tmwxPaletteFrame<wxFrame>(wxT("View Settings"), 554)
{
  // Get position and size from wxConfig
  InitPositionAndSize();
  // We have only one panel
  mViewSettingsPanel = new tmwxViewSettingsPanel(this);
}


/*****
Remove the currently selected panel.
Explicitly redeclared since otherwise overloaded versions would hide it
*****/
void tmwxViewSettingsFrame::SetSelection()
{
  tmwxPaletteFrame<wxFrame>::SetSelection();
}


/*****
Change selection panel to the view settings panel.
*****/
void tmwxViewSettingsFrame::SetSelection(tmwxViewSettings* aViewSettings)
{
  TMASSERT(aViewSettings);
  mObj = aViewSettings;
  mViewSettingsPanel->Fill(aViewSettings);
  if (!mPanel) InstallPanel(mViewSettingsPanel);
}


/*****
Return the key string and default value for the given position/size component.
*****/
void tmwxViewSettingsFrame::GetPositionSizeInfo(Key key, wxString& keystr, 
  int& val)
{
  switch(key) {
    case X_KEY:
      keystr = wxT("ViewSettingsFrameX");
      val = 30;
      break;
    case Y_KEY:
      keystr = wxT("ViewSettingsFrameY");
      val = 70;
      break;
    case W_KEY:
      keystr = wxT("ViewSettingsFrameW");
      val = 280;
      break;
    case H_KEY:
      keystr = wxT("ViewSettingsFrameH");
      val = 280;
      break;
    default:
      TMFAIL("unknown key encountered in "\
       "tmwxViewSettingsFrame::GetPositionSizeInfo(..)");
      break;
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Attempt to close should only hide the window except at shutdown.
*****/
void tmwxViewSettingsFrame::OnClose(wxCloseEvent& event) 
{
  if (gApp->IsQuitting())
    event.Skip();
  else
    if (event.CanVeto()) {
      event.Veto();
      Hide();
    }
}


BEGIN_EVENT_TABLE(tmwxViewSettingsFrame, tmwxPaletteFrame<wxFrame>)
  EVT_CLOSE(tmwxViewSettingsFrame::OnClose)
END_EVENT_TABLE()

/*******************************************************************************
File:         tmwxViewSettingsFrame.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker view settings frame
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-16
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXVIEWSETTINGSFRAME_H_
#define _TMWXVIEWSETTINGSFRAME_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Addition TreeMaker GUI classes
#include "tmwxPaletteFrame.h"

// Forward declarations
#include "tmwxGUI_fwd.h"

/**********
class tmwxViewSettingsFrame
Displays settings that affect what parts are shown and how they are displayed
**********/
class tmwxViewSettingsFrame: public tmwxPaletteFrame<wxFrame> {
public:
  tmwxViewSettingsFrame();
  void SetSelection();
  void SetSelection(tmwxViewSettings* aViewSettings);

  void OnClose(wxCloseEvent& event);
  DECLARE_EVENT_TABLE()

private:
  tmwxViewSettingsPanel* mViewSettingsPanel;
  void GetPositionSizeInfo(Key key, wxString& keystr, int& val);
};

#endif //_TMWXVIEWSETTINGSFRAME_H_

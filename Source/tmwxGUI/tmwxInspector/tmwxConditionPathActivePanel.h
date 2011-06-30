/*******************************************************************************
File:         tmwxConditionPathActivePanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionPathActive inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONPATHACTIVEPANEL_H_
#define _TMWXCONDITIONPATHACTIVEPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionPathActive;
class tmwxTextCtrl;
class tmwxStaticText;

/**********
class tmwxConditionPathActivePanel
Displays information about a tmConditionPathActive
**********/

class tmwxConditionPathActivePanel : public tmwxInspectorPanel {
private:
  tmConditionPathActive* mConditionPathActive;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode1;
  tmwxTextCtrl* mNode2;
  tmwxStaticText* mIsFeasible;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mPath;
#endif
public:
  tmwxConditionPathActivePanel(wxWindow* parent);
  void Fill(tmConditionPathActive* aConditionPathActive);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONPATHACTIVEPANEL_H_

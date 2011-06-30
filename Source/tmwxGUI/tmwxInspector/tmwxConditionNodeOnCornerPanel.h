/*******************************************************************************
File:         tmwxConditionNodeOnCornerPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodeOnCorner inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONNODEONCORNERPANEL_H_
#define _TMWXCONDITIONNODEONCORNERPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionNodeOnCorner;
class tmwxTextCtrl;

/**********
class tmwxConditionNodeOnCornerPanel
Displays information about a tmConditionNodeOnCorner
**********/

class tmwxConditionNodeOnCornerPanel : public tmwxInspectorPanel {
private:
  tmConditionNodeOnCorner* mConditionNodeOnCorner;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode;
  tmwxStaticText* mIsFeasible;
public:
  tmwxConditionNodeOnCornerPanel(wxWindow* parent);
  void Fill(tmConditionNodeOnCorner* aConditionNodeOnCorner);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONNODEONCORNERPANEL_H_

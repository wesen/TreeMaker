/*******************************************************************************
File:         tmwxConditionNodesPairedPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodesPaired inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONNODESPAIREDPANEL_H_
#define _TMWXCONDITIONNODESPAIREDPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionNodesPaired;
class tmwxTextCtrl;

/**********
class tmwxConditionNodesPairedPanel
Displays information about a tmConditionNodesPaired
**********/

class tmwxConditionNodesPairedPanel : public tmwxInspectorPanel {
private:
  tmConditionNodesPaired* mConditionNodesPaired;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode1;
  tmwxTextCtrl* mNode2;
  tmwxStaticText* mIsFeasible;
public:
  tmwxConditionNodesPairedPanel(wxWindow* parent);
  void Fill(tmConditionNodesPaired* aConditionNodesPaired);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONNODESPAIREDPANEL_H_

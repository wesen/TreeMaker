/*******************************************************************************
File:         tmwxConditionNodesCollinearPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodesCollinear inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONNODESCOLLINEARPANEL_H_
#define _TMWXCONDITIONNODESCOLLINEARPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionNodesCollinear;
class tmwxTextCtrl;

/**********
class tmwxConditionNodesCollinearPanel
Displays information about a tmConditionNodesCollinear
**********/

class tmwxConditionNodesCollinearPanel : public tmwxInspectorPanel {
private:
  tmConditionNodesCollinear* mConditionNodesCollinear;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode1;
  tmwxTextCtrl* mNode2;
  tmwxTextCtrl* mNode3;
  tmwxStaticText* mIsFeasible;
public:
  tmwxConditionNodesCollinearPanel(wxWindow* parent);
  void Fill(tmConditionNodesCollinear* aConditionNodesCollinear);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONNODESCOLLINEARPANEL_H_

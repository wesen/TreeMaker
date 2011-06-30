/*******************************************************************************
File:         tmwxConditionNodeOnEdgePanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodeOnEdge inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONNODEONEDGEPANEL_H_
#define _TMWXCONDITIONNODEONEDGEPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionNodeOnEdge;
class tmwxTextCtrl;

/**********
class tmwxConditionNodeOnEdgePanel
Displays information about a tmConditionNodeOnEdge
**********/

class tmwxConditionNodeOnEdgePanel : public tmwxInspectorPanel {
private:
  tmConditionNodeOnEdge* mConditionNodeOnEdge;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode;
  tmwxStaticText* mIsFeasible;
public:
  tmwxConditionNodeOnEdgePanel(wxWindow* parent);
  void Fill(tmConditionNodeOnEdge* aConditionNodeOnEdge);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONNODEONEDGEPANEL_H_

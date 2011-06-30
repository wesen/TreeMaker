/*******************************************************************************
File:         tmwxConditionNodeSymmetricPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodeSymmetric inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONNODESYMMETRICPANEL_H_
#define _TMWXCONDITIONNODESYMMETRICPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionNodeSymmetric;
class tmwxTextCtrl;

/**********
class tmwxConditionNodeSymmetricPanel
Displays information about a tmConditionNodeSymmetric
**********/

class tmwxConditionNodeSymmetricPanel : public tmwxInspectorPanel {
private:
  tmConditionNodeSymmetric* mConditionNodeSymmetric;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode;
  tmwxStaticText* mIsFeasible;
public:
  tmwxConditionNodeSymmetricPanel(wxWindow* parent);
  void Fill(tmConditionNodeSymmetric* aConditionNodeSymmetric);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONNODESYMMETRICPANEL_H_

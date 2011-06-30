/*******************************************************************************
File:         tmwxConditionNodeFixedPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodeFixed inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONNODEFIXEDPANEL_H_
#define _TMWXCONDITIONNODEFIXEDPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionNodeFixed;
class tmwxTextCtrl;
class wxCheckBox;

/**********
class tmwxConditionNodeFixedPanel
Displays information about a tmConditionNodeFixed
**********/

class tmwxConditionNodeFixedPanel : public tmwxInspectorPanel {
private:
  tmConditionNodeFixed* mConditionNodeFixed;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode;
  wxCheckBox* mXFixed;
  tmwxTextCtrl* mXFixValue;
  wxCheckBox* mYFixed;
  tmwxTextCtrl* mYFixValue;
  tmwxStaticText* mIsFeasible;
public:
  tmwxConditionNodeFixedPanel(wxWindow* parent);
  void Fill(tmConditionNodeFixed* aConditionNodeFixed);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
  void OnCheckBox(wxCommandEvent& event);
};

#endif //_TMWXCONDITIONNODEFIXEDPANEL_H_

/*******************************************************************************
File:         tmwxConditionNodeComboPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodeCombo inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-22
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONNODECOMBOPANEL_H_
#define _TMWXCONDITIONNODECOMBOPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionNodeCombo;
class tmwxTextCtrl;
class wxCheckBox;

/**********
class tmwxConditionNodeComboPanel
Displays information about a tmConditionNodeCombo
**********/

class tmwxConditionNodeComboPanel : public tmwxInspectorPanel {
private:
  tmConditionNodeCombo* mConditionNodeCombo;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode;
  wxCheckBox* mToSymmetryLine;
  wxCheckBox* mToPaperEdge;
  wxCheckBox* mToPaperCorner;
  wxCheckBox* mXFixed;
  tmwxTextCtrl* mXFixValue;
  wxCheckBox* mYFixed;
  tmwxTextCtrl* mYFixValue;
  tmwxStaticText* mIsFeasible;
public:
  tmwxConditionNodeComboPanel(wxWindow* parent);
  void Fill(tmConditionNodeCombo* aConditionNodeCombo);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
  void OnCheckBox(wxCommandEvent& event);
};

#endif //_TMWXCONDITIONNODECOMBOPANEL_H_

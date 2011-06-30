/*******************************************************************************
File:         tmwxConditionPathComboPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionPathCombo inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-10
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONPATHCOMBOPANEL_H_
#define _TMWXCONDITIONPATHCOMBOPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionPathCombo;
class tmwxTextCtrl;
class tmwxStaticText;

/**********
class tmwxConditionPathComboPanel
Displays information about a tmConditionPathCombo
**********/

class tmwxConditionPathComboPanel : public tmwxInspectorPanel {
private:
  tmConditionPathCombo* mConditionPathCombo;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode1;
  tmwxTextCtrl* mNode2;
  wxRadioBox* mRadioBox;
  tmwxTextCtrl* mAngle;
  tmwxTextCtrl* mQuant;
  tmwxTextCtrl* mQuantOffset;
  tmwxStaticText* mIsFeasible;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mPath;
  tmwxStaticText* mCurAngle;
#endif
public:
  tmwxConditionPathComboPanel(wxWindow* parent);
  void Fill(tmConditionPathCombo* aConditionPathCombo);
  void Fill();
  
  // Event handling
  void OnApply(wxCommandEvent& event);
  void OnRadioBox(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};

#endif // _TMWXCONDITIONPATHCOMBOPANEL_H_

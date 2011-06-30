/*******************************************************************************
File:         tmwxConditionPathAngleQuantPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionPathAngleQuant inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONPATHANGLEQUANTPANEL_H_
#define _TMWXCONDITIONPATHANGLEQUANTPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionPathAngleQuant;
class tmwxTextCtrl;
class tmwxStaticText;

/**********
class tmwxConditionPathAngleQuantPanel
Displays information about a tmConditionPathAngleQuant
**********/

class tmwxConditionPathAngleQuantPanel : public tmwxInspectorPanel {
private:
  tmConditionPathAngleQuant* mConditionPathAngleQuant;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode1;
  tmwxTextCtrl* mNode2;
  tmwxTextCtrl* mQuant;
  tmwxTextCtrl* mQuantOffset;
  tmwxStaticText* mIsFeasible;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mPath;
  tmwxStaticText* mCurAngle;
#endif
public:
  tmwxConditionPathAngleQuantPanel(wxWindow* parent);
  void Fill(tmConditionPathAngleQuant* aConditionPathAngleQuant);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONPATHANGLEQUANTPANEL_H_

/*******************************************************************************
File:         tmwxConditionPathAngleFixedPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionPathAngleFixed inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONPATHANGLEFIXEDPANEL_H_
#define _TMWXCONDITIONPATHANGLEFIXEDPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionPathAngleFixed;
class tmwxTextCtrl;
class tmwxStaticText;

/**********
class tmwxConditionPathAngleFixedPanel
Displays information about a tmConditionPathAngleFixed
**********/

class tmwxConditionPathAngleFixedPanel : public tmwxInspectorPanel {
private:
  tmConditionPathAngleFixed* mConditionPathAngleFixed;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mNode1;
  tmwxTextCtrl* mNode2;
  tmwxTextCtrl* mAngle;
  tmwxStaticText* mIsFeasible;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mPath;
  tmwxStaticText* mCurAngle;
#endif
public:
  tmwxConditionPathAngleFixedPanel(wxWindow* parent);
  void Fill(tmConditionPathAngleFixed* aConditionPathAngleFixed);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONPATHANGLEFIXEDPANEL_H_

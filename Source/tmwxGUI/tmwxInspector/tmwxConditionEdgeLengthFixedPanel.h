/*******************************************************************************
File:         tmwxConditionEdgeLengthFixedPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionEdgeLengthFixed inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONEDGELENGTHFIXEDPANEL_H_
#define _TMWXCONDITIONEDGELENGTHFIXEDPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionEdgeLengthFixed;
class tmwxTextCtrl;

/**********
class tmwxConditionEdgeLengthFixedPanel
Displays information about a tmConditionEdgeLengthFixed
**********/

class tmwxConditionEdgeLengthFixedPanel : public tmwxInspectorPanel {
private:
  tmConditionEdgeLengthFixed* mConditionEdgeLengthFixed;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mEdge;
  tmwxStaticText* mIsFeasible;
public:
  tmwxConditionEdgeLengthFixedPanel(wxWindow* parent);
  void Fill(tmConditionEdgeLengthFixed* aConditionEdgeLengthFixed);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONEDGELENGTHFIXEDPANEL_H_

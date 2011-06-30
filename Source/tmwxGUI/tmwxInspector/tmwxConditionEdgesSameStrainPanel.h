/*******************************************************************************
File:         tmwxConditionEdgesSameStrainPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionEdgesSameStrain inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONEDGESSAMESTRAINPANEL_H_
#define _TMWXCONDITIONEDGESSAMESTRAINPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmConditionEdgesSameStrain;
class tmwxTextCtrl;

/**********
class tmwxConditionEdgesSameStrainPanel
Displays information about a tmConditionEdgesSameStrain
**********/

class tmwxConditionEdgesSameStrainPanel : public tmwxInspectorPanel {
private:
  tmConditionEdgesSameStrain* mConditionEdgesSameStrain;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mEdge1;
  tmwxTextCtrl* mEdge2;
  tmwxStaticText* mIsFeasible;
public:
  tmwxConditionEdgesSameStrainPanel(wxWindow* parent);
  void Fill(tmConditionEdgesSameStrain* aConditionEdgesSameStrain);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXCONDITIONEDGESSAMESTRAINPANEL_H_

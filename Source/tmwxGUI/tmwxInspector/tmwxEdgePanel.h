/*******************************************************************************
File:         tmwxEdgePanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for node inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXEDGEPANEL_H_
#define _TMWXEDGEPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmEdge;
class tmwxTextCtrl;
class tmwxStaticText;
class tmwxConditionListBox;

/**********
class tmwxEdgePanel
Displays information about the entire tree
**********/

class tmwxEdgePanel : public tmwxInspectorPanel {
private:
  tmEdge* mEdge;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mLength;
  tmwxTextCtrl* mLabel;
  tmwxTextCtrl* mStrain;
  tmwxTextCtrl* mStiffness;
  tmwxConditionListBox* mConditions;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mEdgeOwner;
  tmwxStaticText* mStrainedLength;
  tmwxStaticText* mIsPinnedEdge;
  tmwxStaticText* mIsConditionedEdge;
#endif
public:
  tmwxEdgePanel(wxWindow* parent);
  void Fill(tmEdge* aEdge);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif //_TMWXEDGEPANEL_H_

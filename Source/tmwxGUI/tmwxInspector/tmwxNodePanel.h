/*******************************************************************************
File:         tmwxNodePanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for node inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXNODEPANEL_H_
#define _TMWXNODEPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmNode;
class tmwxConditionListBox;
class tmwxTextCtrl;
class tmwxStaticText;

/**********
class tmwxNodePanel
Displays information about the entire tree
**********/

class tmwxNodePanel : public tmwxInspectorPanel {
private:
  tmNode* mNode;
  tmwxTextCtrl* mIndex;
  tmwxTextCtrl* mLocX;
  tmwxTextCtrl* mLocY;
  tmwxTextCtrl* mLabel;
  tmwxConditionListBox* mConditions;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mNodeOwner;
  tmwxStaticText* mIsLeafNode;
  tmwxStaticText* mIsSubNode;
  tmwxStaticText* mIsBorderNode;
  tmwxStaticText* mIsPinnedNode;
  tmwxStaticText* mIsPolygonNode;
  tmwxStaticText* mIsJunctionNode;
  tmwxStaticText* mIsConditionedNode;
  tmwxStaticText* mEdges;
  tmwxStaticText* mLeafPaths;
  tmwxStaticText* mOwnedVertices;
  tmwxStaticText* mElevation;
  tmwxStaticText* mDepth;
  tmwxStaticText* mDiscreteDepth;
#endif
public:
  tmwxNodePanel(wxWindow* parent);
  void Fill(tmNode* aNode);
  void Fill();

  // Event handling
  DECLARE_EVENT_TABLE()
  void OnApply(wxCommandEvent& event);
};

#endif // _TMWXNODEPANEL_H_

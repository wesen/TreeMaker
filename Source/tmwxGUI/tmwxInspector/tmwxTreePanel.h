/*******************************************************************************
File:         tmwxTreePanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for tree inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXTREEPANEL_H_
#define _TMWXTREEPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmTree;
class tmwxStaticText;
class tmwxTextCtrl;
class tmwxConditionListBox;

/**********
class tmwxTreePanel
Displays information about the entire tree
**********/

class tmwxTreePanel : public tmwxInspectorPanel {
private:
  tmTree* mTree;
  tmwxTextCtrl* mPaperWidth;
  tmwxTextCtrl* mPaperHeight;
  tmwxTextCtrl* mScale;
  wxCheckBox* mHasSymmetry;
  wxButton* mDiag;
  wxButton* mBook;
  tmwxTextCtrl* mSymLocX;
  tmwxTextCtrl* mSymLocY;
  tmwxTextCtrl* mSymAngle;
  tmwxConditionListBox* mConditions;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mNumNodes;
  tmwxStaticText* mNumEdges;
  tmwxStaticText* mNumPaths;
  tmwxStaticText* mNumPolys;
  tmwxStaticText* mNumVertices;
  tmwxStaticText* mNumCreases;
  tmwxStaticText* mNumFacets;
  tmwxStaticText* mNumConditions;
  tmwxStaticText* mNumOwnedNodes;
  tmwxStaticText* mNumOwnedEdges;
  tmwxStaticText* mNumOwnedPaths;
  tmwxStaticText* mNumOwnedPolys;
  tmwxStaticText* mNumOwnedConditions;
  tmwxStaticText* mIsFeasible;
  tmwxStaticText* mIsPolygonValid;
  tmwxStaticText* mIsPolygonFilled;
  tmwxStaticText* mIsVertexDepthValid;
  tmwxStaticText* mIsFacetDataValid;
  tmwxStaticText* mIsLocalRootConnectable;
#endif // tmwxINSPECTOR_EXTRA
public:
  tmwxTreePanel(wxWindow* parent);
  void Fill(tmTree* aTree);
  void Fill();
  
  // Event handling
  DECLARE_EVENT_TABLE()
  void OnButton(wxCommandEvent& event);
  void OnCheckBox(wxCommandEvent& event);
  void OnApply(wxCommandEvent& event);
private:
  void SetSymEnable(bool enable);
};

#endif //_TMWXTREEPANEL_H_

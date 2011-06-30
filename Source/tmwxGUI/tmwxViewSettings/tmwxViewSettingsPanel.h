/*******************************************************************************
File:         tmwxViewSettingsPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for view settings inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXVIEWPANEL_H_
#define _TMWXVIEWPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional TreeMaker headers
#include "tmwxPalettePanel.h"

// Additional headers
#include "tmwxViewSettings.h"

/**********
class tmwxViewSettingsPanel
Displays information about the view settings
**********/

class tmwxViewSettingsPanel : public tmwxPalettePanel {
private:
  tmwxViewSettings* mViewSettings;

  wxButton* mTreeBtn;
  wxButton* mDesignBtn;
  wxButton* mCreasesBtn;
  wxButton* mPlanBtn;
  wxButton* mNoneBtn;
  wxButton* mAllBtn;
  
  wxCheckBox* mShowSymmetryLines;

  wxButton* mNodeAllBtn;
  wxButton* mNodeNoneBtn;
  wxCheckBox* mShowLeafNodes;
  wxCheckBox* mShowBranchNodes;
  wxCheckBox* mShowSubNodes;
  wxCheckBox* mShowNodeDots;
  wxCheckBox* mShowNodeCircles;
  wxCheckBox* mShowNodeIndices;
  wxCheckBox* mShowNodeCoords;
  wxCheckBox* mShowNodeElevation;
  wxCheckBox* mShowNodeDepth;
  wxCheckBox* mShowNodeLabels;
  wxCheckBox* mShowNodeFlags;
    
  wxButton* mEdgeAllBtn;
  wxButton* mEdgeNoneBtn;
  wxCheckBox* mShowAllEdges;
  wxCheckBox* mShowEdgeDots;
  wxCheckBox* mShowEdgeLines;
  wxCheckBox* mShowEdgeIndices;
  wxCheckBox* mShowEdgeLengths;
  wxCheckBox* mShowEdgeStrain;
  wxCheckBox* mShowEdgeLabels;
  wxCheckBox* mShowEdgeFlags;
  
  wxButton* mPathAllBtn;
  wxButton* mPathNoneBtn;
  wxCheckBox* mShowLeafPaths;
  wxCheckBox* mShowBranchPaths;
  wxCheckBox* mShowSubPaths;
  wxCheckBox* mShowActivePaths;
  wxCheckBox* mShowBorderPaths;
  wxCheckBox* mShowPolygonPaths;
  wxCheckBox* mShowInfeasiblePaths;
  wxCheckBox* mShowPathDots;
  wxCheckBox* mShowPathLines;
  wxCheckBox* mShowPathIndices;
  wxCheckBox* mShowPathLengths;
  wxCheckBox* mShowPathFlags;
  
  wxButton* mPolyAllBtn;
  wxButton* mPolyNoneBtn;
  wxCheckBox* mShowMajorPolys;
  wxCheckBox* mShowSubPolys;
  wxCheckBox* mShowPolyDots;
  wxCheckBox* mShowPolyLines;
  wxCheckBox* mShowPolyFills;
  wxCheckBox* mShowPolyIndices;
  wxCheckBox* mShowPolyFlags;
  
  wxButton* mVertexAllBtn;
  wxButton* mVertexNoneBtn;
  wxCheckBox* mShowMajorVertices;
  wxCheckBox* mShowMinorVertices;
  wxCheckBox* mShowVertexDots;
  wxCheckBox* mShowVertexIndices;
  wxCheckBox* mShowVertexCoords;
  wxCheckBox* mShowVertexElevation;
  wxCheckBox* mShowVertexDepth;
  wxCheckBox* mShowVertexDiscreteDepth;
  wxCheckBox* mShowVertexTreeNode;
  wxCheckBox* mShowVertexFlags;
  
  wxButton* mCreaseAllBtn;
  wxButton* mCreaseNoneBtn;
  wxCheckBox* mShowMajorCreases;
  wxCheckBox* mShowMinorCreases;
  wxCheckBox* mShowCreaseDots;
  wxCheckBox* mShowCreaseLines;
  wxRadioBox* mShowCreaseFolds;
  wxCheckBox* mShowCreaseIndices;
  wxCheckBox* mShowCreaseFlags;
  
  wxButton* mFacetAllBtn;
  wxButton* mFacetNoneBtn;
  wxCheckBox* mShowAllFacets;
  wxCheckBox* mShowFacetDots;
  wxCheckBox* mShowFacetArrows;
  wxCheckBox* mShowFacetFills;
  wxCheckBox* mShowFacetIndices;
  wxCheckBox* mShowFacetOrder;
  wxCheckBox* mShowFacetCorridorEdge;
  wxCheckBox* mShowFacetFlags;
  
  wxButton* mConditionAllBtn;
  wxButton* mConditionNoneBtn;
  wxCheckBox* mShowNodeConditions;
  wxCheckBox* mShowEdgeConditions;
  wxCheckBox* mShowPathConditions;
  wxCheckBox* mShowConditionDots;
  wxCheckBox* mShowConditionLines;
  wxCheckBox* mShowConditionIndices;
  wxCheckBox* mShowConditionTypes;  
public:
  tmwxViewSettingsPanel(wxWindow* parent);
  void Fill(tmwxViewSettings* aViewSettings);
  void Fill();
private:
  wxSizer* MakePaperSettings();
  wxSizer* MakeNodeSettings();
  wxSizer* MakeEdgeSettings();
  wxSizer* MakePathSettings();
  wxSizer* MakePolySettings();
  wxSizer* MakeVertexSettings();
  wxSizer* MakeCreaseSettings();
  wxSizer* MakeFacetSettings();
  wxSizer* MakeConditionSettings();
  void AddStatLine(wxBoxSizer* colSizer);
  void AddAllNoneBtns(wxBoxSizer* colSizer, wxButton*& allBtn, wxButton*& noneBtn);
  void ReadPanel();

  // Event handling
  void OnCheckBox(wxCommandEvent& event);
  void OnButton(wxCommandEvent& event);
  void OnRadioBox(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};

#endif //_TMWXVIEWPANEL_H_

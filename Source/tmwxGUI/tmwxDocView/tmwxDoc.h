/*******************************************************************************
File:         tmwxDoc.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker document class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXDOC_H_
#define _TMWXDOC_H_

#include "tmHeader.h"

#include "tmCluster.h"

#include "wx/docview.h"

#include <sstream>

#include "tmwxView.h"

class tmTree;

/**********
class tmwxDoc
TreeMaker document class
**********/
class tmwxDoc: public wxDocument {
  DECLARE_DYNAMIC_CLASS(tmwxDoc)
public:
  tmCluster mSelection;
  tmTree* mTree;
  std::stringstream mCleanState;  // unmodified tree state, serialized
  tmwxView* mView;
  
  // Constructor/destructor
  tmwxDoc();
  virtual ~tmwxDoc();
  
  // Getters
  tmwxView* GetView() const { return mView; };
  tmwxDesignFrame* GetDesignFrame() const { return mView->GetDesignFrame(); };
  tmwxDesignCanvas* GetDesignCanvas() const { return mView->GetDesignCanvas(); };
  
  // Selection routines
  template <class P>
    void SetSelection(P* p);
  template <class P>
    void ExtendSelection(P* p, bool wasShiftClick);
  tmNode* ExactlyOneNodeSelected();
  void ClearSelection();
  void GetLeafPathsFromSelection(tmArray<tmPath*>& leafPaths);
  
  // Command submission
  void SubmitCommand(const wxString& name);

  // Save/Load (required by framework)
  virtual bool OnNewDocument();
  bool ReadTreeFromFile(const wxString& file);
  virtual bool OnOpenDocument(const wxString& file);
  virtual bool OnCloseDocument();
  virtual bool Revert();
  
  // Must use std iostreams
  typedef wxSTD ostream tmwxOutputStream;
  typedef wxSTD istream tmwxInputStream;
  tmwxOutputStream& SaveObject(tmwxOutputStream& text_stream);
  tmwxInputStream& LoadObject(tmwxInputStream& text_stream);
  
  // view updating
  void UpdatePalettes();
  void UpdateAllViews(wxView* sender = NULL ,wxObject* hint = NULL);
  void UpdateDocViews();

  // File menu
  void OnExportv4UpdateUI(wxUpdateUIEvent& event);
  void OnExportv4(wxCommandEvent& event);
  
  // Edit menu
  void OnCutUpdateUI(wxUpdateUIEvent& event);
  void OnCut(wxCommandEvent& event);
  void OnCopyUpdateUI(wxUpdateUIEvent& event);
  void OnCopy(wxCommandEvent& event);
  void OnPasteUpdateUI(wxUpdateUIEvent& event);
  void OnPaste(wxCommandEvent& event);
  void OnClearUpdateUI(wxUpdateUIEvent& event);
  void OnClear(wxCommandEvent& event);
  // Edit->Select menu
  void OnSelectAllUpdateUI(wxUpdateUIEvent& event);
  void OnSelectAll(wxCommandEvent& event);
  void OnSelectNoneUpdateUI(wxUpdateUIEvent& event);
  void OnSelectNone(wxCommandEvent& event);
  void OnSelectPartByIndexUpdateUI(wxUpdateUIEvent& event);
  void OnSelectPartByIndex(wxCommandEvent& event);
  void OnSelectMovablePartsUpdateUI(wxUpdateUIEvent& event);
  void OnSelectMovableParts(wxCommandEvent& event);
  void OnSelectPathFromNodesUpdateUI(wxUpdateUIEvent& event);
  void OnSelectPathFromNodes(wxCommandEvent& event);
  void OnSelectCorridorFacetsUpdateUI(wxUpdateUIEvent& event);
  void OnSelectCorridorFacets(wxCommandEvent& event);
  // Edit->Node
  void OnMakeNodeRootUpdateUI(wxUpdateUIEvent& event);
  void OnMakeNodeRoot(wxCommandEvent& event);
  void OnPerturbSelectedNodesUpdateUI(wxUpdateUIEvent& event);
  void OnPerturbSelectedNodes(wxCommandEvent& event);
  void OnPerturbAllNodesUpdateUI(wxUpdateUIEvent& event);
  void OnPerturbAllNodes(wxCommandEvent& event);
  // Edit->Edge
  void OnSetEdgeLengthUpdateUI(wxUpdateUIEvent& event);
  void OnSetEdgeLength(wxCommandEvent& event);
  void OnScaleEdgeLengthUpdateUI(wxUpdateUIEvent& event);
  void OnScaleEdgeLength(wxCommandEvent& event);
  void OnRenormalizeToEdgeUpdateUI(wxUpdateUIEvent& event);
  void OnRenormalizeToEdge(wxCommandEvent& event);
  void OnRenormalizeToUnitScaleUpdateUI(wxUpdateUIEvent& event);
  void OnRenormalizeToUnitScale(wxCommandEvent& event);
  // Edit->Absorb
  void OnAbsorbSelectedNodesUpdateUI(wxUpdateUIEvent& event);
  void OnAbsorbSelectedNodes(wxCommandEvent& event);
  void OnAbsorbRedundantNodesUpdateUI(wxUpdateUIEvent& event);
  void OnAbsorbRedundantNodes(wxCommandEvent& event);
  void OnAbsorbSelectedEdgesUpdateUI(wxUpdateUIEvent& event);
  void OnAbsorbSelectedEdges(wxCommandEvent& event);
  // Edit->Split
  void OnSplitSelectedEdgeUpdateUI(wxUpdateUIEvent& event);
  void OnSplitSelectedEdge(wxCommandEvent& event);
  // Edit->Strain
  void OnRemoveSelectionStrainUpdateUI(wxUpdateUIEvent& event);
  void OnRemoveSelectionStrain(wxCommandEvent& event);
  void OnRemoveAllStrainUpdateUI(wxUpdateUIEvent& event);
  void OnRemoveAllStrain(wxCommandEvent& event);
  void OnRelieveSelectionStrainUpdateUI(wxUpdateUIEvent& event);
  void OnRelieveSelectionStrain(wxCommandEvent& event);
  void OnRelieveAllStrainUpdateUI(wxUpdateUIEvent& event);
  void OnRelieveAllStrain(wxCommandEvent& event);
  // Edit->Stub
  void OnPickStubNodesUpdateUI(wxUpdateUIEvent& event);
  void OnPickStubNodes(wxCommandEvent& event);
  void OnPickStubPolyUpdateUI(wxUpdateUIEvent& event);
  void OnPickStubPoly(wxCommandEvent& event);
  void OnAddLargestStubNodesUpdateUI(wxUpdateUIEvent& event);
  void OnAddLargestStubNodes(wxCommandEvent& event);
  void OnAddLargestStubPolyUpdateUI(wxUpdateUIEvent& event);
  void OnAddLargestStubPoly(wxCommandEvent& event);
  void OnTriangulateTreeUpdateUI(wxUpdateUIEvent& event);
  void OnTriangulateTree(wxCommandEvent& event);
  // Edit menu support
  void DoKillSelection();
  void DoPickStub(const tmArray<tmNode*> nlist);

  // View menu
  void OnDefaultViewUpdateUI(wxUpdateUIEvent& event);
  void OnDefaultView(wxCommandEvent& event);
  void OnTreeViewUpdateUI(wxUpdateUIEvent& event);
  void OnTreeView(wxCommandEvent& event);
  void OnCreasesViewUpdateUI(wxUpdateUIEvent& event);
  void OnCreasesView(wxCommandEvent& event);
  void OnPlanViewUpdateUI(wxUpdateUIEvent& event);
  void OnPlanView(wxCommandEvent& event);
  void OnFitToScreenUpdateUI(wxUpdateUIEvent& event);
  void OnFitToScreen(wxCommandEvent& event);
  void OnFitToWidthUpdateUI(wxUpdateUIEvent& event);
  void OnFitToWidth(wxCommandEvent& event);
  void OnFitToHeightUpdateUI(wxUpdateUIEvent& event);
  void OnFitToHeight(wxCommandEvent& event);
  void OnSetPaperSizeUpdateUI(wxUpdateUIEvent& event);
  void OnSetPaperSize(wxCommandEvent& event);

  // Action menu
  void OnScaleEverythingUpdateUI(wxUpdateUIEvent& event);
  void OnScaleEverything(wxCommandEvent& event);
  void OnScaleSelectionUpdateUI(wxUpdateUIEvent& event);
  void OnScaleSelection(wxCommandEvent& event);
  void OnMinimizeStrainUpdateUI(wxUpdateUIEvent& event);
  void OnMinimizeStrain(wxCommandEvent& event);
  void OnBuildCreasePatternUpdateUI(wxUpdateUIEvent& event);
  void OnBuildCreasePattern(wxCommandEvent& event);
  void OnKillCreasePatternUpdateUI(wxUpdateUIEvent& event);
  void OnKillCreasePattern(wxCommandEvent& event);
  // Action menu support
  void MsgEdgesTooShort();
  void MsgNotPolygonValid();
  void MsgNotPolygonFilled();
  void MsgMultipleInactiveBorderPaths();
  void MsgNotVertexDepthValid();
  void MsgNotFacetDataValid();
  void MsgNotLocalRootConnectable();
  int QueryEqnsVblsContinue(std::size_t ne, std::size_t nv);
  int QueryUserCancelledRevert();
  int QueryBadConvergenceRevert(int nlco, int reason);
  
  // Condition menu
  void OnNodeFixedToSymmetryLineUpdateUI(wxUpdateUIEvent& event);
  void OnNodeFixedToSymmetryLine(wxCommandEvent& event);
  void OnNodeFixedToPaperEdgeUpdateUI(wxUpdateUIEvent& event);
  void OnNodeFixedToPaperEdge(wxCommandEvent& event);
  void OnNodeFixedToCornerUpdateUI(wxUpdateUIEvent& event);
  void OnNodeFixedToCorner(wxCommandEvent& event);
  void OnNodeFixedToPositionUpdateUI(wxUpdateUIEvent& event);
  void OnNodeFixedToPosition(wxCommandEvent& event);
#ifdef TMDEBUG
  void OnNodeFixedToSymmetryLinev4UpdateUI(wxUpdateUIEvent& event);
  void OnNodeFixedToSymmetryLinev4(wxCommandEvent& event);
  void OnNodeFixedToPaperEdgev4UpdateUI(wxUpdateUIEvent& event);
  void OnNodeFixedToPaperEdgev4(wxCommandEvent& event);
  void OnNodeFixedToCornerv4UpdateUI(wxUpdateUIEvent& event);
  void OnNodeFixedToCornerv4(wxCommandEvent& event);
  void OnNodeFixedToPositionv4UpdateUI(wxUpdateUIEvent& event);
  void OnNodeFixedToPositionv4(wxCommandEvent& event);
#endif // TMDEBUG
  void OnNodesPairedAboutSymmetryLineUpdateUI(wxUpdateUIEvent& event);
  void OnNodesPairedAboutSymmetryLine(wxCommandEvent& event);
  void OnNodesCollinearUpdateUI(wxUpdateUIEvent& event);
  void OnNodesCollinear(wxCommandEvent& event);
  void OnRemoveNodeConditionsUpdateUI(wxUpdateUIEvent& event);
  void OnRemoveNodeConditions(wxCommandEvent& event);
  //
  void OnEdgeLengthFixedUpdateUI(wxUpdateUIEvent& event);
  void OnEdgeLengthFixed(wxCommandEvent& event);
  void OnEdgesSameStrainUpdateUI(wxUpdateUIEvent& event);
  void OnEdgesSameStrain(wxCommandEvent& event);
  void OnRemoveEdgeConditionsUpdateUI(wxUpdateUIEvent& event);
  void OnRemoveEdgeConditions(wxCommandEvent& event);
  //
  void OnPathActiveUpdateUI(wxUpdateUIEvent& event);
  void OnPathActive(wxCommandEvent& event);
  void OnPathAngleFixedUpdateUI(wxUpdateUIEvent& event);
  void OnPathAngleFixed(wxCommandEvent& event);
  void OnPathAngleQuantizedUpdateUI(wxUpdateUIEvent& event);
  void OnPathAngleQuantized(wxCommandEvent& event);
#ifdef TMDEBUG
  void OnPathActivev4UpdateUI(wxUpdateUIEvent& event);
  void OnPathActivev4(wxCommandEvent& event);
  void OnPathAngleFixedv4UpdateUI(wxUpdateUIEvent& event);
  void OnPathAngleFixedv4(wxCommandEvent& event);
  void OnPathAngleQuantizedv4UpdateUI(wxUpdateUIEvent& event);
  void OnPathAngleQuantizedv4(wxCommandEvent& event);
#endif // TMDEBUG
  void OnRemovePathConditionsUpdateUI(wxUpdateUIEvent& event);
  void OnRemovePathConditions(wxCommandEvent& event);
  //
  void OnRemoveAllConditionsUpdateUI(wxUpdateUIEvent& event);
  void OnRemoveAllConditions(wxCommandEvent& event);
  // Condition menu support
  void DoKillConditions(tmArray<tmCondition*>& markedConditions);

  // Debug menu
#ifdef TMDEBUG
  void OnToggleQuitCleanupEarlyUpdateUI(wxUpdateUIEvent& event);
  void OnToggleQuitCleanupEarly(wxCommandEvent& event);
  void OnMakeTreeBlankUpdateUI(wxUpdateUIEvent& event);
  void OnMakeTreeBlank(wxCommandEvent& event);
  void OnMakeTreeUnoptimizedUpdateUI(wxUpdateUIEvent& event);
  void OnMakeTreeUnoptimized(wxCommandEvent& event);
  void OnMakeTreeOptimizedUpdateUI(wxUpdateUIEvent& event);
  void OnMakeTreeOptimized(wxCommandEvent& event);
  void OnMakeTreeGussetUpdateUI(wxUpdateUIEvent& event);
  void OnMakeTreeGusset(wxCommandEvent& event);
  void OnMakeTreeConditionedUpdateUI(wxUpdateUIEvent& event);
  void OnMakeTreeConditioned(wxCommandEvent& event);
  // Debug menu support
  void DoReplaceTree(tmTree* newTree);
#endif // TMDEBUG

  DECLARE_EVENT_TABLE()
};


/**********
Template definitions
**********/

/*****
Set the selection to the part P.
*****/
template <class P>
void tmwxDoc::SetSelection(P* p)
{
  mSelection.ChangeToPart(p);
  UpdateAllViews();
}


/*****
Extend the selection by adding/removing the object p, depending on whether it
was regular-clicked or shift-clicked.
*****/
template <class P>
void tmwxDoc::ExtendSelection(P* p, bool wasShiftClick)
{
  if (wasShiftClick) {
    if (!mSelection.Contains(p)) {
      // shift-click, selection doesn't contain object, add it to selection
      mSelection.AddPart(p);
    }
    else {
      // shift-click, selection contains object, remove it from selection
      mSelection.RemovePart(p);
    }
  }
  else {
    if (!mSelection.Contains(p)) {
      // normal click, selection doesn't contain object, make it the selection
      mSelection.ClearAllParts();
      mSelection.AddPart(p);
    }
    else {
      // normal click, selection contains object, no change to selection
    }
  }
  UpdateAllViews();
}

#endif // _TMWXDOC_H_

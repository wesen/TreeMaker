/*******************************************************************************
File:         tmwxDoc_Edit.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker document class, Edit menu commands
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-10
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDoc.h"
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxView.h"
#include "tmwxInspectorFrame.h"
#include "tmwxGetUserInputDialog.h"
#include "tmwxDesignCanvas.h"
#include "tmwxTextCtrl.h"
#include "tmwxApp.h"

// Additional wxWidgets classes
#include "wx/metafile.h"

using namespace std;

/**********
class tmwxSetEdgeLengthDialog
Utility dialog for obtaining edge lengths from the user
**********/
class tmwxSetEdgeLengthDialog : public tmwxGetUserInputDialog {
public:
  tmwxDoc* mDoc;
  tmwxTextCtrl* mTextCtrl;
  double mValue;

  tmwxSetEdgeLengthDialog(tmwxDoc* doc);
  bool Validate();
  bool TransferDataFromWindow();
};


/*****
Constructor
*****/
tmwxSetEdgeLengthDialog::tmwxSetEdgeLengthDialog(tmwxDoc* doc)
  : tmwxGetUserInputDialog(wxT("Set Edge Lengths")),
  mDoc(doc)
{
  AddTextPair(wxT("Edge length: "), mTextCtrl, wxT("%.4f"));
  FinishDialog();
  if (mDoc->mSelection.GetEdges().size() == 1) {
    mTextCtrl->SetValue(mDoc->mSelection.GetEdges().front()->
      GetStrainedLength());
    mTextCtrl->SetSelection(-1, -1);
  }
  mTextCtrl->SetFocus();
}


/*****
Make sure the user entry is a valid edge length.
*****/
bool tmwxSetEdgeLengthDialog::Validate()
{
  return mTextCtrl->ValidateFloat(wxT("edge length"), 0.001, true, mValue);
}


/*****
Set the selected edge lengths from the dialog value
*****/
bool tmwxSetEdgeLengthDialog::TransferDataFromWindow()
{
  mDoc->mTree->SetEdgeLengths(mDoc->mSelection.GetEdges(), mValue);
  return true;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxScaleEdgeLengthDialog
Utility dialog for scaling edge lengths by a user-specified amount
**********/
class tmwxScaleEdgeLengthDialog : public tmwxGetUserInputDialog {
public:
  tmwxDoc* mDoc;
  tmwxTextCtrl* mTextCtrl;
  double mValue;

  tmwxScaleEdgeLengthDialog(tmwxDoc* doc);
  bool Validate();
  bool TransferDataFromWindow();
};


/*****
Constructor
*****/
tmwxScaleEdgeLengthDialog::tmwxScaleEdgeLengthDialog(tmwxDoc* doc)
  : tmwxGetUserInputDialog(wxT("Scale Edge Length")),
  mDoc(doc)
{
  AddTextPair(wxT("Scaling factor: "), mTextCtrl, wxT("%.4f"));
  FinishDialog(); 
  mTextCtrl->SetValue(1.);
  mTextCtrl->SetSelection(-1,-1);
  mTextCtrl->SetFocus();
}


/*****
Make sure the user entry is a valid edge length.
*****/
bool tmwxScaleEdgeLengthDialog::Validate()
{
  return mTextCtrl->ValidateFloat(wxT("scaling factor"), 0.0, false, mValue);
}


/*****
Scale the selected edges from the dialog value
*****/
bool tmwxScaleEdgeLengthDialog::TransferDataFromWindow()
{
  mDoc->mTree->ScaleEdgeLengths(mDoc->mSelection.GetEdges(), mValue);
  return true;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxSplitEdgeDialog
Utility dialog for splitting an edge
**********/
class tmwxSplitEdgeDialog : public tmwxGetUserInputDialog {
public:
  tmwxDoc* mDoc;
  wxRadioButton* mRadioAbs;     // enables absolute length
  wxRadioButton* mRadioRatio;   // enables length ratio
  tmwxTextCtrl* mTextCtrlAbs;   // absolute split point
  tmwxTextCtrl* mTextCtrlRatio; // ratio first subsegment/total length
  double mValue;

  tmwxSplitEdgeDialog(tmwxDoc* doc);
  bool Validate();
  bool TransferDataFromWindow();
  void OnRadioBtn(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};


/*****
Constructor
*****/
tmwxSplitEdgeDialog::tmwxSplitEdgeDialog(tmwxDoc* doc)
  : tmwxGetUserInputDialog(wxT("Split Edge")),
  mDoc(doc)
{
  TMASSERT(mDoc->mSelection.GetEdges().size() == 1);
  tmEdge* theEdge = mDoc->mSelection.GetEdges().front();
  wxString msg;
  msg.Printf(wxT("Edge %s from node %s to node %s has length %.4f."),
    tmwxStr(theEdge).c_str(), 
    tmwxStr(theEdge->GetNodes().front()).c_str(),
    tmwxStr(theEdge->GetNodes().back()).c_str(),
    theEdge->GetLength());
  mDialogSizer->Add(CreateTextSizer(msg), 
    wxSizerFlags().Left().Border(wxALL, 5));
  mDialogSizer->Add(CreateTextSizer(
    wxString::Format(wxT("Split distance from node %s:"), 
      tmwxStr(theEdge).c_str())), 
    wxSizerFlags().Left().Border(wxALL, 5));
  StartRadioPair(mRadioAbs, wxString::Format(wxT("Absolute (0 - %.4f)"),
    theEdge->GetLength()), mTextCtrlAbs, wxT("%.4f"));
  mTextCtrlAbs->Disable();
  AddRadioPair(mRadioRatio, wxT("Ratio (0 - 1.0)"), mTextCtrlRatio, 
    wxT("%.4f"));
  mTextCtrlRatio->SetValue(0.5);
  mTextCtrlRatio->SetSelection(-1, -1);
  mRadioRatio->SetValue(true);
  FinishDialog();
  mTextCtrlRatio->SetFocus();
}


/*****
Make sure the user entry is a valid edge length.
*****/
bool tmwxSplitEdgeDialog::Validate()
{
  TMASSERT(mDoc->mSelection.GetEdges().size() == 1);
  tmEdge* theEdge = mDoc->mSelection.GetEdges().front();
  if (mRadioAbs->GetValue())
    return mTextCtrlAbs->ValidateFloat(wxT("split distance"), 0.0, false,
      theEdge->GetLength(), false, mValue);
  if (mRadioRatio->GetValue()) {
    double ratio;
    if (!mTextCtrlRatio->ValidateFloat(wxT("split ratio"), 0.0, false,
      1.0, true, ratio)) 
      return false;
    mValue = ratio * theEdge->GetLength();
    return true;
  }
  TMFAIL("bad radio buttons");
  return false;
}


/*****
Set the selected edge lengths from the dialog value
*****/
bool tmwxSplitEdgeDialog::TransferDataFromWindow()
{
  TMASSERT(mDoc->mSelection.GetEdges().size() == 1);
  tmEdge* theEdge = mDoc->mSelection.GetEdges().front();
  tmNode* newNode;
  mDoc->mTree->SplitEdge(theEdge, mValue, newNode);
  return true;
}


/*****
Respond to radio button selection
*****/
void tmwxSplitEdgeDialog::OnRadioBtn(wxCommandEvent& event)
{
  event.Skip(true);
  if (mRadioAbs->GetValue()) {
    mTextCtrlRatio->Disable();
    mTextCtrlAbs->Enable();
    mTextCtrlAbs->SetFocus();
  }
  if (mRadioRatio->GetValue()) {
    mTextCtrlAbs->Disable();
    mTextCtrlRatio->Enable();
    mTextCtrlRatio->SetFocus();
  }
}


BEGIN_EVENT_TABLE(tmwxSplitEdgeDialog, tmwxGetUserInputDialog)
  EVT_RADIOBUTTON(wxID_ANY, tmwxSplitEdgeDialog::OnRadioBtn)
END_EVENT_TABLE()


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxSelectPartByIndexDialog
Utility dialog for changing the selection to a part, specified by index
**********/
class tmwxSelectPartByIndexDialog : public tmwxGetUserInputDialog {
public:
  tmwxDoc* mDoc;
  tmwxTextCtrl* mTextCtrl;
  wxRadioBox* mRadioBox;
  int mPartType;
  std::size_t mPartOffset;

  tmwxSelectPartByIndexDialog(tmwxDoc* doc);
  bool Validate();
  bool TransferDataFromWindow();
};


/*****
Constructor
*****/
tmwxSelectPartByIndexDialog::tmwxSelectPartByIndexDialog(tmwxDoc* doc)
  : tmwxGetUserInputDialog(wxT("Select Part")),
  mDoc(doc)
{
  const int NUM_PART_TYPES = 7;
  const wxString choices[NUM_PART_TYPES] = {
    wxT("Node"),
    wxT("Edge"),
    wxT("Path"),
    wxT("Poly"),
    wxT("Vertex"),
    wxT("Crease"),
    wxT("Facet") };
  mRadioBox = new wxRadioBox(this, wxID_ANY, wxT("Select Part Type"), 
    wxDefaultPosition, wxDefaultSize, NUM_PART_TYPES, choices, 4, 
    wxRA_SPECIFY_COLS);
  mDialogSizer->Add(mRadioBox,
    wxSizerFlags().Expand().Border(wxALL, 5));
  mDialogSizer->AddSpacer(5);
  
  AddTextPair(wxT("Enter Part Index:"), mTextCtrl);
  FinishDialog();
  mTextCtrl->SetFocus();
}


/*****
Make sure the user entry is a valid index for the selected type of part
*****/
bool tmwxSelectPartByIndexDialog::Validate()
{
  size_t maxIndex;
  wxString partTypeStr;
  mPartType = mRadioBox->GetSelection();
  switch(mPartType) {
    case 0:
      partTypeStr = wxT("node");
      maxIndex = mDoc->mTree->GetNodes().size();
      break;
    case 1:
      partTypeStr = wxT("edge");
      maxIndex = mDoc->mTree->GetEdges().size();
      break;
    case 2:
      partTypeStr = wxT("path");
      maxIndex = mDoc->mTree->GetPaths().size();
      break;
    case 3:
      partTypeStr = wxT("poly");
      maxIndex = mDoc->mTree->GetPolys().size();
      break;
    case 4:
      partTypeStr = wxT("vertex");
      maxIndex = mDoc->mTree->GetVertices().size();
      break;
    case 5:
      partTypeStr = wxT("crease");
      maxIndex = mDoc->mTree->GetCreases().size();
      break;
    case 6:
      partTypeStr = wxT("facet");
      maxIndex = mDoc->mTree->GetFacets().size();
      break;
    default:
      maxIndex = 0;
      TMFAIL("unknown case");
  }
  if (maxIndex == 0) {
    tmwxAlertError(wxString::Format(
      wxT("You can't select %s %s by index because there aren't any."),
      mPartType != 1 ? wxT("a") : wxT("an"),
      partTypeStr.c_str()));
    return false;
  }
  size_t index;
  if (!mTextCtrl->ValidateSizeT(wxString::Format(wxT("%s index"), 
    partTypeStr.c_str()), 1, maxIndex, index, 
    wxString::Format(wxT("%s %s index must be between 1 and %d."),
    mPartType != 1 ? wxT("A") : wxT("An"),
    partTypeStr.c_str(),
    int(maxIndex))))
    return false;
  mPartOffset = index - 1;
  return true;
}


/*****
Change the selection to the part specified by the dialog values
*****/
bool tmwxSelectPartByIndexDialog::TransferDataFromWindow()
{
  switch(mPartType) {
    case 0:
      mDoc->mSelection.ChangeToPart(mDoc->mTree->GetNodes()[mPartOffset]);
      break;
    case 1:
      mDoc->mSelection.ChangeToPart(mDoc->mTree->GetEdges()[mPartOffset]);
      break;
    case 2:
      mDoc->mSelection.ChangeToPart(mDoc->mTree->GetPaths()[mPartOffset]);
      break;
    case 3:
      mDoc->mSelection.ChangeToPart(mDoc->mTree->GetPolys()[mPartOffset]);
      break;
    case 4:
      mDoc->mSelection.ChangeToPart(mDoc->mTree->GetVertices()[mPartOffset]);
      break;
    case 5:
      mDoc->mSelection.ChangeToPart(mDoc->mTree->GetCreases()[mPartOffset]);
      break;
    case 6:
      mDoc->mSelection.ChangeToPart(mDoc->mTree->GetFacets()[mPartOffset]);
      break;
    default:
      TMFAIL("unknown case");
  }
  return true;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxDoc
TreeMaker document class
**********/

/*****
Enable Edit->Cut
*****/
void tmwxDoc::OnCutUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetNumEditableParts() > 0);
}


/*****
Perform Edit->Cut
*****/
void tmwxDoc::OnCut(wxCommandEvent&)
{
  DoKillSelection();
}


/*****
Enable Edit->Copy
*****/
void tmwxDoc::OnCopyUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
}


/*****
Perform Edit->Copy
*****/
void tmwxDoc::OnCopy(wxCommandEvent&)
{
#if wxUSE_METAFILE  // wxGTK doesn't support metafile
  wxMetafileDC dc;
  if (dc.Ok()) {
    GetDesignCanvas()->OnDraw(dc);
    wxMetafile *mf = dc.Close();
    if (mf) {
      mf->SetClipboard((int)(dc.MaxX() + 10), (int)(dc.MaxY() + 10));
      delete mf;
    }
  }
#endif // wxUSE_METAFILE
}


/*****
Disable Edit->Paste (although edit controls might grab and enable)
*****/
void tmwxDoc::OnPasteUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(false);
}


/*****
Perform Edit->Paste
*****/
void tmwxDoc::OnPaste(wxCommandEvent&)
{
  // no-op
}


/*****
Enable Edit->Clear
*****/
void tmwxDoc::OnClearUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetNumEditableParts() > 0);
}


/*****
Perform Edit->Clear
*****/
void tmwxDoc::OnClear(wxCommandEvent&)
{
  DoKillSelection();
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Enable Edit->Select->Part by Index
*****/
void tmwxDoc::OnSelectPartByIndexUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->GetNodes().not_empty());
}


/*****
Perform Edit->Select->Part by Index
*****/
void tmwxDoc::OnSelectPartByIndex(wxCommandEvent&)
{
  tmwxSelectPartByIndexDialog dialog(this);
  if (dialog.ShowModal() == wxID_OK) {
    // No changes to document, just update views
    UpdateAllViews();
  }
}


/*****
Enable Edit->Select->All
*****/
void tmwxDoc::OnSelectAllUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->GetNumEditableParts() > 0);
}


/*****
Perform Edit->Select->All
*****/
void tmwxDoc::OnSelectAll(wxCommandEvent&)
{
  mSelection.ClearAllParts();
  mSelection.AddParts(mTree->GetNodes());
  mSelection.AddParts(mTree->GetEdges());
  gInspectorFrame->DispatchSetSelection(mTree, mSelection);
  // no command, just changed selection
  UpdateAllViews();
}


/*****
Enable Edit->Select->None
*****/
void tmwxDoc::OnSelectNoneUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetNumAllParts() != 0);
}


/*****
Perform Edit->Select->None
*****/
void tmwxDoc::OnSelectNone(wxCommandEvent&)
{
  mSelection.ClearAllParts();
  gInspectorFrame->DispatchSetSelection(mTree, mSelection);
  // no command, just changed selection
  UpdateAllViews();
}


/*****
Enable Edit->Select->Movable Parts
*****/
void tmwxDoc::OnSelectMovablePartsUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->GetNumMovableParts() != 0);
}


/*****
Perform Edit->Select->Movable Parts
*****/
void tmwxDoc::OnSelectMovableParts(wxCommandEvent&)
{
  mSelection.ClearAllParts();
  mSelection.AddParts(mTree->GetNodes());
  mSelection.AddParts(mTree->GetEdges());
  mTree->FilterMovableParts(mSelection.mNodes, mSelection.mEdges);
  // no command, just changed selection
  UpdateAllViews();
}


/*****
Enable Edit->Select->Path From Nodes
*****/
void tmwxDoc::OnSelectPathFromNodesUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetNumNodes() == 2);
}


/*****
Perform Edit->Select->Path From Nodes
*****/
void tmwxDoc::OnSelectPathFromNodes(wxCommandEvent&)
{
  mSelection.ChangeToPart(mTree->GetPath(mSelection.GetNodes().front(),
    mSelection.GetNodes().back()));
  // no command, just changed selection
  UpdateAllViews();
}


/*****
Enable Edit->Select->Corridor Facets
*****/
void tmwxDoc::OnSelectCorridorFacetsUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->CanGetCorridorFacets());
}


/*****
Perform Edit->Select->Corridor Facets
*****/
void tmwxDoc::OnSelectCorridorFacets(wxCommandEvent&)
{
  tmArray<tmFacet*> facetList;
  mTree->GetCorridorFacets(mSelection.GetEdges(), facetList);
  mSelection.ClearAllParts();
  mSelection.AddParts(facetList);
  // no command, just changed selection
  UpdateAllViews();
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Enable Edit->Node->Make Root
*****/
void tmwxDoc::OnMakeNodeRootUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.ContainsExactly<tmNode>(1) &&
    mTree->CanMakeNodeRoot(mSelection.GetNodes().front()));
}


/*****
Perform Edit->Node->Make Root
*****/
void tmwxDoc::OnMakeNodeRoot(wxCommandEvent&)
{
  tmNode* theNode = mSelection.GetNodes().front();
  mTree->MakeNodeRoot(theNode);
  SubmitCommand(wxT("Make Node Root"));
}


/*****
Enable Edit->Node->Perturb Selected
*****/
void tmwxDoc::OnPerturbSelectedNodesUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetNodes().not_empty());
}


/*****
Perform Edit->Node->Perturb Selected
*****/
void tmwxDoc::OnPerturbSelectedNodes(wxCommandEvent&)
{
  mTree->PerturbNodes(mSelection.GetNodes());
  SubmitCommand(wxT("Perturb Nodes"));
}


/*****
Enable Edit->Node->Perturb All
*****/
void tmwxDoc::OnPerturbAllNodesUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->CanPerturbAllNodes());
}


/*****
Perform Edit->Node->Perturb All
*****/
void tmwxDoc::OnPerturbAllNodes(wxCommandEvent&)
{
  mTree->PerturbAllNodes();
  SubmitCommand(wxT("Perturb All Nodes"));
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Enable Edit->Edge->Set Length
*****/
void tmwxDoc::OnSetEdgeLengthUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetEdges().not_empty());
}


/*****
Perform Edit->Edge->Set Length
*****/
void tmwxDoc::OnSetEdgeLength(wxCommandEvent&)
{
  tmwxSetEdgeLengthDialog dialog(this);
  if (dialog.ShowModal() == wxID_OK) {
    SubmitCommand(wxT("Set Edge Length"));
  }
}


/*****
Enable Edit->Edge->Scale Length
*****/
void tmwxDoc::OnScaleEdgeLengthUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetEdges().not_empty());
}


/*****
Perform Edit->Edge->Scale Length
*****/
void tmwxDoc::OnScaleEdgeLength(wxCommandEvent&)
{
  tmwxScaleEdgeLengthDialog dialog(this);
  if (dialog.ShowModal() == wxID_OK) {
    SubmitCommand(wxT("Scale Edge Length"));
  }
}


/*****
Enable Edit->Edge->Renormalize to Selected
*****/
void tmwxDoc::OnRenormalizeToEdgeUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.ContainsExclusively<tmEdge>(1));
}


/*****
Perform Edit->Edge->Renormalize to Selected
*****/
void tmwxDoc::OnRenormalizeToEdge(wxCommandEvent&)
{
  mTree->RenormalizeToUnitEdge(mSelection.GetEdges().front());
  SubmitCommand(wxT("Renormalize to Selected Edge"));
}


/*****
Enable Edit->Edge->Renormalize to Unit Scale
*****/
void tmwxDoc::OnRenormalizeToUnitScaleUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->GetEdges().not_empty());
}


/*****
Perform Edit->Edge->Renormalize to Unit Scale
*****/
void tmwxDoc::OnRenormalizeToUnitScale(wxCommandEvent&)
{
  mTree->RenormalizeToUnitScale();
  SubmitCommand(wxT("Renormalize to Unit Scale"));
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Enable Edit->Absorb->Selected Nodes
*****/
void tmwxDoc::OnAbsorbSelectedNodesUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetNodes().not_empty() &&
    mTree->CanAbsorbNodes(mSelection.GetNodes()));
}


/*****
Perform Edit->Absorb->Selected Nodes
*****/
void tmwxDoc::OnAbsorbSelectedNodes(wxCommandEvent&)
{
  mTree->AbsorbNodes(mSelection.GetNodes());
  SubmitCommand(wxT("Absorb Nodes"));
}


/*****
Enable Edit->Absorb->Redundant Nodes
*****/
void tmwxDoc::OnAbsorbRedundantNodesUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->CanAbsorbRedundantNodes());
}


/*****
Perform Edit->Absorb->Redundant Nodes
*****/
void tmwxDoc::OnAbsorbRedundantNodes(wxCommandEvent&)
{
  mTree->AbsorbRedundantNodes();
  SubmitCommand(wxT("Absorb Redundant Nodes"));
}


/*****
Enable Edit->Absorb->Selected Edges
*****/
void tmwxDoc::OnAbsorbSelectedEdgesUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetEdges().not_empty() &&
    mTree->CanAbsorbEdges(mSelection.GetEdges()));
}


/*****
Perform Edit->Absorb->Selected Edges
*****/
void tmwxDoc::OnAbsorbSelectedEdges(wxCommandEvent&)
{
  mTree->AbsorbEdges(mSelection.GetEdges());
  SubmitCommand(wxT("Absorb Edges"));
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Enable Edit->Split->Selected Edge
*****/
void tmwxDoc::OnSplitSelectedEdgeUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.GetEdges().size() == 1);
}


/*****
Perform Edit->Split->Selected Edge
*****/
void tmwxDoc::OnSplitSelectedEdge(wxCommandEvent&)
{
  tmwxSplitEdgeDialog dialog(this);
  if (dialog.ShowModal() == wxID_OK) {
    SubmitCommand(wxT("Split Edge"));
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Enable Edit->Strain->Remove Selection
*****/
void tmwxDoc::OnRemoveSelectionStrainUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(tmEdge::ContainsStrainedEdges(mSelection.GetEdges()));
}


/*****
Perform Edit->Strain->Remove Selection
*****/
void tmwxDoc::OnRemoveSelectionStrain(wxCommandEvent&)
{
  mTree->RemoveStrain(mSelection.GetEdges());
  SubmitCommand(wxT("Remove Strain"));
}


/*****
Enable Edit->Strain->Remove All
*****/
void tmwxDoc::OnRemoveAllStrainUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->HasStrainedEdges());
}


/*****
Perform Edit->Strain->Remove All
*****/
void tmwxDoc::OnRemoveAllStrain(wxCommandEvent&)
{
  mTree->RemoveAllStrain();
  SubmitCommand(wxT("Remove All Strain"));
}


/*****
Enable Edit->Strain->Relieve Selection
*****/
void tmwxDoc::OnRelieveSelectionStrainUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(tmEdge::ContainsStrainedEdges(mSelection.GetEdges()));
}


/*****
Perform Edit->Strain->Relieve Selection
*****/
void tmwxDoc::OnRelieveSelectionStrain(wxCommandEvent&)
{
  mTree->RelieveStrain(mSelection.GetEdges());
  SubmitCommand(wxT("Relieve Strain"));
}


/*****
Enable Edit->Strain->Relieve All
*****/
void tmwxDoc::OnRelieveAllStrainUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->HasStrainedEdges());
}


/*****
Perform Edit->Strain->Relieve All
*****/
void tmwxDoc::OnRelieveAllStrain(wxCommandEvent&)
{
  mTree->RelieveAllStrain();
  SubmitCommand(wxT("Relieve All Strain"));
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Enable Edit->Stub->Pick Stub Nodes
*****/
void tmwxDoc::OnPickStubNodesUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.ContainsAtLeast<tmNode>(4));
}


/*****
Perform Edit->Stub->Pick Stub Nodes
*****/
void tmwxDoc::OnPickStubNodes(wxCommandEvent&)
{
  DoPickStub((const tmArray<tmNode*>&)mSelection.GetNodes());
}


/*****
Enable Edit->Stub->Pick Stub Poly
*****/
void tmwxDoc::OnPickStubPolyUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.ContainsExactly<tmPoly>(1) &&
    mSelection.GetPolys().front()->GetRingNodes().size() >= 4);
}


/*****
Perform Edit->Stub->Pick Stub Poly
*****/
void tmwxDoc::OnPickStubPoly(wxCommandEvent&)
{
  DoPickStub(mSelection.GetPolys().front()->GetRingNodes());
}


/*****
Enable Edit->Stub->Add Largest Stub Nodes
*****/
void tmwxDoc::OnAddLargestStubNodesUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.ContainsAtLeast<tmNode>(4));
}


/*****
Perform Edit->Stub->Add Largest Stub Nodes
*****/
void tmwxDoc::OnAddLargestStubNodes(wxCommandEvent&)
{
  tmStubFinder stubFinder(mTree);
  tmStubInfo stubInfo = stubFinder.FindLargestStub(mSelection.GetNodes());
  if (stubInfo.IsBlank()) {
    tmwxAlertInfo(wxT("No stub was found for these nodes."));
    return;
  }
  stubFinder.AddStubToTree(stubInfo);
  SubmitCommand(wxT("Add Largest Stub"));
}


/*****
Enable Edit->Stub->Add largest Stub Poly
*****/
void tmwxDoc::OnAddLargestStubPolyUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mSelection.ContainsExactly<tmPoly>(1) &&
    mSelection.GetPolys()[0]->GetSize() >= 4);
}


/*****
Perform Edit->Stub->Add largest Stub Poly
*****/
void tmwxDoc::OnAddLargestStubPoly(wxCommandEvent&)
{
  tmStubFinder stubFinder(mTree);
  tmStubInfo stubInfo = stubFinder.FindLargestStub(
    mSelection.GetPolys().front()->GetRingNodes());
  if (stubInfo.IsBlank()) {
    tmwxAlertInfo(wxT("No stub was found for this poly."));
    return;
  }
  stubFinder.AddStubToTree(stubInfo);
  SubmitCommand(wxT("Add Largest Stub"));
}


/*****
Enable Edit->Stub->Triangulate Tree
*****/
void tmwxDoc::OnTriangulateTreeUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->HasNonTrianglePoly());
}


/*****
Perform Edit->Stub->Triangulate Tree
*****/
void tmwxDoc::OnTriangulateTree(wxCommandEvent&)
{
  tmStubFinder stubFinder(mTree);
  stubFinder.TriangulateTree();
  SubmitCommand(wxT("Triangulate Tree"));
}


#ifdef __MWERKS__
#pragma mark -
#endif




/*****
Kill the currently-selected nodes, edges, and/or conditions.
*****/
void tmwxDoc::DoKillSelection()
{
  try {
    mTree->KillSomeParts(mSelection.GetNodes(), mSelection.GetEdges(), 
      mSelection.GetConditions());
    SubmitCommand(wxT("Delete Parts"));
  }
  catch (tmTree::EX_BAD_KILL_PARTS) {
    tmwxAlertError(
      wxT("You can't delete parts that would break up the tree."));
  }
}


/*****
Find all stubs that make 4 or more active paths with the nodes in nlist,
and let the user pick which one gets applied.
*****/
void tmwxDoc::DoPickStub(const tmArray<tmNode*> nlist)
{
  // First find the stubs
  tmArray<tmStubInfo> slist;
  tmStubFinder sfinder(mTree);
  sfinder.FindAllStubs(nlist, slist);
  if (slist.empty()) {
    tmwxAlertError(wxT("Sorry, I can't find any stubs for this set."));
    return;
  }
  
  // Now we have a list of stubs. Construct a wxMultipleChoices dialog to
  // let the user pick his/her favorite.
  wxArrayString choices;
  for (size_t i = 0; i < slist.size(); ++i) {
    tmStubInfo& sinfo = slist[i];
    wxString text;
    text.Printf(wxT("Edge %s (%.4f/%.4f), nodes "), 
      tmwxStr(sinfo.mEdge).c_str(),
      sinfo.mEdgeloc, sinfo.mEdge->GetStrainedLength() - sinfo.mEdgeloc);
      wxString itext;
    for (size_t j = 0; j < sinfo.mActiveNodes.size(); ++j) {
      text += tmwxStr(sinfo.mActiveNodes[j]) + wxT(", ");
    }
    wxString sltext;
    sltext.Printf(wxT("stub len = %.4f"), sinfo.mLength);
    text += sltext;
    choices.Add(text);
  }
  int ret = ::wxGetSingleChoiceIndex(wxT("Pick the desired stub:"),
    wxT("Stub Finder"), choices);
  if (ret == -1) return;  // user cancelled
  
  sfinder.AddStubToTree(slist[size_t(ret)]);
  SubmitCommand(wxT("Add Stub"));
}

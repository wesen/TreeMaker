/*******************************************************************************
File:         tmwxTreePanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tree inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      20o3-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxTreePanel.h"

// Additional wxWidgets classes
#include "wx/statline.h"

// Additional TreeMaker classes
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxTextCtrl.h"
#include "tmwxStaticText.h"
#include "tmwxDoc.h"
#include "tmwxApp.h"
#include "tmwxDocManager.h"
#include "tmwxConditionListBox.h"
#include "tmwxCheckBoxSmall.h"
#include "tmwxButtonSmall.h"

/*****
tmwxTreePanel constants
*****/

const wxSize CONDITION_BOX_SIZE(10, 250);


/*****
Constructor
*****/
tmwxTreePanel::tmwxTreePanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mTree(0)
{
  AddTextPair(wxT("Paper Width = "), mPaperWidth, wxT("%.4f"));
  AddTextPair(wxT("Paper Height = "), mPaperHeight, wxT("%.4f"));
  AddTextPair(wxT("Scale = "), mScale, wxT("%.4f"));
  
  const int BUTTON_WIDTH = 60;
  wxBoxSizer* symsizer = new wxBoxSizer(wxHORIZONTAL);
  symsizer->Add(1, 0, 1);
  symsizer->Add(mHasSymmetry = new tmwxCheckBoxSmall(this, wxT("Symmetry")),
    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL | wxTOP|wxBOTTOM, 2);
  symsizer->Add(10, 0, 0);
  symsizer->Add(mDiag = new tmwxButtonSmall(this, wxT("Diag"), BUTTON_WIDTH),
    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL | wxTOP|wxBOTTOM, 2);
  symsizer->Add(10, 0, 0);
  symsizer->Add(mBook = new tmwxButtonSmall(this, wxT("Book"), BUTTON_WIDTH),
    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL | wxTOP|wxBOTTOM, 2);
  symsizer->Add(1, 0, 1);
  mPanelSizer->Add(symsizer, 0, wxGROW);
  
  AddTextPair(wxT("X = "), mSymLocX, wxT("%.4f"));
  AddTextPair(wxT("Y = "), mSymLocY, wxT("%.4f"));
  AddTextPair(wxT("Angle = "), mSymAngle, wxT("%.2f"));
  AddApplyButton();
  AddStatLine();
  AddConditionListBox(mConditions, wxSize(10, 250));    
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mNumNodes);
  AddStatTextGridExtra(mNumEdges);
  AddStatTextGridExtra(mNumPaths);
  AddStatTextGridExtra(mNumPolys);
  AddStatTextGridExtra(mNumVertices);
  AddStatTextGridExtra(mNumCreases);
  AddStatTextGridExtra(mNumFacets);
  AddStatTextGridExtra(mNumConditions);
  AddStatTextGridExtra(mNumOwnedNodes);
  AddStatTextGridExtra(mNumOwnedEdges);
  AddStatTextGridExtra(mNumOwnedPaths);
  AddStatTextGridExtra(mNumOwnedPolys);
  AddStatTextGridExtra(mNumOwnedConditions);
  AddStatTextGridExtra(mIsFeasible);
  AddStatTextGridExtra(mIsPolygonValid);
  AddStatTextGridExtra(mIsPolygonFilled);
  AddStatTextGridExtra(mIsVertexDepthValid);
  AddStatTextGridExtra(mIsFacetDataValid);
  AddStatTextGridExtra(mIsLocalRootConnectable);
#endif // tmwxINSPECTOR_EXTRA
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the condition list with all conditions affecting the given part
template specialization for a tmTree
*****/
template <>
void tmwxConditionListBox::FillListBox(tmTree* aTree)
{
  mConditions = aTree->GetConditions();
  FillListBoxFromConditionList();
}


/*****
Fill the panel from aTree, which can be NULL.
*****/
void tmwxTreePanel::Fill(tmTree* aTree)
{
  mTree = aTree;
  Fill();
}


/*****
Fill the panel from its object, which may be NULL.
*****/
void tmwxTreePanel::Fill()
{
  if (!mTree) return;
  mPanelBox->SetLabel(wxT("Tree"));
  mPaperWidth->SetValue(mTree->GetPaperWidth());
  mPaperHeight->SetValue(mTree->GetPaperHeight());
  mScale->SetValue(mTree->GetScale());
  mHasSymmetry->SetValue(mTree->HasSymmetry());
  mSymLocX->SetValue(mTree->GetSymLocX());
  mSymLocY->SetValue(mTree->GetSymLocY());
  mSymAngle->SetValue(mTree->GetSymAngle());
  
  mConditions->FillListBox(mTree);
  
#if tmwxINSPECTOR_EXTRA
  mNumNodes->SetLabelFormatted(wxT("Num Nodes: %s"), 
    mTree->GetNodes().size());
  mNumEdges->SetLabelFormatted(wxT("Num Edges: %s"), 
    mTree->GetEdges().size());
  mNumPaths->SetLabelFormatted(wxT("Num Paths: %s"), 
    mTree->GetPaths().size());
  mNumPolys->SetLabelFormatted(wxT("Num Polys: %s"), 
    mTree->GetPolys().size());
  mNumVertices->SetLabelFormatted(wxT("Num Vertices: %s"), 
    mTree->GetVertices().size());
  mNumCreases->SetLabelFormatted(wxT("Num Creases: %s"), 
    mTree->GetCreases().size());
  mNumFacets->SetLabelFormatted(wxT("Num Facets: %s"), 
    mTree->GetFacets().size());
  mNumConditions->SetLabelFormatted(wxT("Num Conditions: %s"), 
    mTree->GetConditions().size());
  mNumOwnedNodes->SetLabelFormatted(wxT("Num Owned Nodes: %s"), 
    mTree->GetOwnedNodes().size());
  mNumOwnedEdges->SetLabelFormatted(wxT("Num Owned Edges: %s"), 
    mTree->GetOwnedEdges().size());
  mNumOwnedPaths->SetLabelFormatted(wxT("Num Owned Paths: %s"), 
    mTree->GetOwnedPaths().size());
  mNumOwnedPolys->SetLabelFormatted(wxT("Num Owned Polys: %s"), 
    mTree->GetOwnedPolys().size());
  mNumOwnedConditions->SetLabelFormatted(wxT("Num Owned Conditions: %s"), 
    mTree->GetOwnedConditions().size());
  mIsFeasible->SetLabelFormatted(wxT("Feasible: %s"),
    mTree->IsFeasible());
  mIsPolygonValid->SetLabelFormatted(wxT("Polygon Valid: %s"),
    mTree->IsPolygonValid());
  mIsPolygonFilled->SetLabelFormatted(wxT("Polygon Filled: %s"),
    mTree->IsPolygonFilled());
  mIsVertexDepthValid->SetLabelFormatted(wxT("Vertex Depth Valid: %s"),
    mTree->IsVertexDepthValid());
  mIsFacetDataValid->SetLabelFormatted(wxT("Facet Data Valid: %s"),
    mTree->IsFacetDataValid());
  mIsLocalRootConnectable->SetLabelFormatted(wxT("Local Root Connectable: %s"),
    mTree->IsLocalRootConnectable());
#endif // tmwxINSPECTOR_EXTRA
} 


/*****
Respond to button events. The two buttons to look for are the ones selecting
book versus diagonal symmetry.
*****/
void tmwxTreePanel::OnButton(wxCommandEvent& event)
{
  wxObject* theBtn = event.GetEventObject();
  
  if (theBtn == mDiag) {
    mHasSymmetry->SetValue(1);
    SetSymEnable(true);
    mSymLocX->SetValue(0.5);
    mSymLocY->SetValue(0.5);
    mSymAngle->SetValue(45.0);
    mTree->SetHasSymmetry(true);
    mTree->SetSymLocX(0.5);
    mTree->SetSymLocY(0.5);
    mTree->SetSymAngle(45.0);
  }
  else if (theBtn == mBook) {
    mHasSymmetry->SetValue(1);
    SetSymEnable(true);
    mSymLocX->SetValue(0.5);
    mSymLocY->SetValue(0.5);
    mSymAngle->SetValue(90.0);
    mTree->SetHasSymmetry(true);
    mTree->SetSymLocX(0.5);
    mTree->SetSymLocY(0.5);
    mTree->SetSymAngle(90.0);
  }
  
  gDocManager->GetCurrentDocumentLocal()->
    SubmitCommand(wxT("Edit Tree"));
}


/*****
Respond to the has-symmetry check box.
*****/
void tmwxTreePanel::OnCheckBox(wxCommandEvent& event)
{
  wxObject* theCheckBox = event.GetEventObject();
  if (theCheckBox == mHasSymmetry) {
    SetSymEnable(mHasSymmetry->GetValue());
    mTree->SetHasSymmetry(mHasSymmetry->GetValue());
  }
  else
    TMFAIL("unknown checkbox");
  
  gDocManager->GetCurrentDocumentLocal()->
    SubmitCommand(wxT("Edit Tree"));
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values, and if everything is valid, update the tree.
*****/
void tmwxTreePanel::OnApply(wxCommandEvent&)
{
  // Validate
  double newPaperWidth;
  if (!mPaperWidth->ValidateFloat(wxT("Paper width"), 0.0, false, 
    newPaperWidth)) {
    mPaperWidth->SetValue(mTree->GetPaperWidth());
    return;
  }
  double newPaperHeight;
  if (!mPaperHeight->ValidateFloat(wxT("Paper height"), 0.0, false, 
    newPaperHeight)) {
    mPaperHeight->SetValue(mTree->GetPaperHeight());
    return;
  }
  double newScale;
  if (!mScale->ValidateFloat(wxT("Scale"), 0.0, false, newScale)) {
    mScale->SetValue(mTree->GetScale());
    return;
  }
  double newSymLocX;
  if (!mSymLocX->ValidateFloat(wxT("X loc"), 0.0, true, 
    mTree->GetPaperWidth(), true, newSymLocX)) {
    mSymLocX->SetValue(mTree->GetSymLocX());
    return;
  }
  double newSymLocY;
  if (!mSymLocY->ValidateFloat(wxT("Y loc"), 0.0, true, 
    mTree->GetPaperHeight(), true, newSymLocY)) {
    mSymLocY->SetValue(mTree->GetSymLocY());
    return;
  }
  double newSymAngle;
  if (!mSymAngle->ValidateFloat(wxT("Symmetry angle"), newSymAngle)) {
    mSymAngle->SetValue(mTree->GetSymAngle());
    return;
  }
 
  // Update
  bool modPaperWidth = !mPaperWidth->SameValue(mTree->GetPaperWidth());
  bool modPaperHeight = !mPaperHeight->SameValue(mTree->GetPaperHeight());
  bool modScale = !mScale->SameValue(mTree->GetScale());
  bool modSymLocX = !mSymLocX->SameValue(mTree->GetSymLocX());
  bool modSymLocY = !mSymLocY->SameValue(mTree->GetSymLocY());
  bool modSymAngle = !mSymAngle->SameValue(mTree->GetSymAngle());
  bool modSomething = modPaperWidth || modPaperHeight || modScale || 
    modSymLocX || modSymLocY || modSymAngle;
  if (modSomething) {
    {
      tmTreeCleaner tc(mTree);
      if (modPaperWidth) {
        mTree->SetPaperWidth(newPaperWidth);
        // If user didn't change SymLocX, we'll scale its location to the
        // new width.
        if (!modSymLocX) 
          mTree->SetSymLocX(mTree->GetSymLocX() * newPaperWidth / 
            mTree->GetPaperWidth());
      }
      if (modPaperHeight) {
        mTree->SetPaperHeight(newPaperHeight);
        // If user didn't change SymLocY, we'll scale its loction to the 
        // new height.
        if (!modSymLocY)
          mTree->SetSymLocY(mTree->GetSymLocY() * newPaperHeight / 
            mTree->GetPaperHeight());
      }
      if (modScale) {
        mTree->SetScale(newScale);
      }
      if (modSymLocX) {
        mTree->SetSymLocX(newSymLocX);
      }
      if (modSymLocY) {
        mTree->SetSymLocY(newSymLocY);
      }
      if (modSymAngle) {
        mTree->SetSymAngle(newSymAngle);
      }
    }
    gDocManager->GetCurrentDocumentLocal()->
      SubmitCommand(wxT("Edit Tree"));
    Fill();
  } 
}


/*****
Enable/disable the text controls relating to the symmetry angle
*****/
void tmwxTreePanel::SetSymEnable(bool enable)
{
  mSymLocX->Enable(enable);
  mSymLocY->Enable(enable);
  mSymAngle->Enable(enable);
}


/*****
Event table
*****/

BEGIN_EVENT_TABLE(tmwxTreePanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxTreePanel::OnApply)
  EVT_BUTTON(wxID_ANY, tmwxTreePanel::OnButton)
  EVT_CHECKBOX(wxID_ANY, tmwxTreePanel::OnCheckBox)
  EVT_TEXT_ENTER(wxID_ANY, tmwxTreePanel::OnApply)
END_EVENT_TABLE()

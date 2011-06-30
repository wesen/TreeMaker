/*******************************************************************************
File:         tmwxNodePanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for node inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxNodePanel.h"

// Additional TreeMaker classes
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxTextCtrl.h"
#include "tmwxStaticText.h"
#include "tmwxDoc.h"
#include "tmwxApp.h"
#include "tmwxDocManager.h"
#include "tmwxConditionListBox.h"

/*****
Constructor
*****/
tmwxNodePanel::tmwxNodePanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mNode(0)
{
  AddTextPair(wxT("Node Index = "), mIndex);
  AddTextPair(wxT("X = "), mLocX, wxT("%.4f"));
  AddTextPair(wxT("Y = "), mLocY, wxT("%.4f"));
  AddTextPair(wxT("Label = "), mLabel);
  AddApplyButton();
  AddStatLine();
  AddConditionListBox(mConditions, wxSize(10, 150));
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mNodeOwner);
  AddStatTextGridExtra(mIsLeafNode);
  AddStatTextGridExtra(mIsSubNode);
  AddStatTextGridExtra(mIsBorderNode);
  AddStatTextGridExtra(mIsPinnedNode);
  AddStatTextGridExtra(mIsPolygonNode);
  AddStatTextGridExtra(mIsJunctionNode);
  AddStatTextGridExtra(mIsConditionedNode);
  AddStatTextRowExtra(mEdges);
  AddStatTextRowExtra(mLeafPaths);
  AddStatTextRowExtra(mOwnedVertices);
  AddStatTextGridExtra(mElevation);
  AddStatTextGridExtra(mDepth);
  AddStatTextGridExtra(mDiscreteDepth);
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Set the contents of the panel from the passed node, which can be NULL.
*****/
void tmwxNodePanel::Fill(tmNode* aNode)
{
  mNode = aNode;
  Fill();
}


/*****
Set the contents of the panel from the current node, which can be NULL.
*****/
void tmwxNodePanel::Fill()
{
  if (!mNode) return;
  mPanelBox->SetLabel(wxT("Node"));
  mIndex->SetValue(mNode->GetIndex());
  mLocX->SetValue(mNode->GetLocX());
  mLocY->SetValue(mNode->GetLocY());
  mLabel->SetValue(mNode->GetLabel());
  
  mConditions->FillListBox((tmNode*)mNode);
  
#if tmwxINSPECTOR_EXTRA
  mNodeOwner->SetLabelFormatted(wxT("Owner: %s"), 
    mNode->GetNodeOwner());
  mIsLeafNode->SetLabelFormatted(wxT("Leaf Node: %s"), 
    mNode->IsLeafNode());
  mIsSubNode->SetLabelFormatted(wxT("Sub Node: %s"), 
    mNode->IsSubNode());
  mIsBorderNode->SetLabelFormatted(wxT("Border Node: %s"), 
    mNode->IsBorderNode());
  mIsPinnedNode->SetLabelFormatted(wxT("Pinned Node: %s"), 
    mNode->IsPinnedNode());
  mIsPolygonNode->SetLabelFormatted(wxT("Polygon Node: %s"), 
    mNode->IsPolygonNode());
  mIsJunctionNode->SetLabelFormatted(wxT("Junction Node: %s"), 
    mNode->IsConditionedNode());
  mIsConditionedNode->SetLabelFormatted(wxT("Conditioned Node: %s"), 
    mNode->IsConditionedNode());
  mEdges->SetLabelFormatted(wxT("Edges: %s"), 
    mNode->GetEdges());
  mLeafPaths->SetLabelFormatted(wxT("Leaf Paths: %s"), 
    mNode->GetLeafPaths());
  mOwnedVertices->SetLabelFormatted(wxT("Owned Vertices: %s"), 
    mNode->GetOwnedVertices());
  mElevation->SetLabelFormatted(wxT("Elevation: %.4f"), 
    mNode->GetElevation());
  mDepth->SetLabelFormatted(wxT("Depth: %.4f"), 
    mNode->GetDepth());
  if (mNode->IsTreeNode())
    mDiscreteDepth->SetLabelFormatted(wxT("Discrete Depth: %.4f"), 
      mNode->GetDepth());
  else
    mDiscreteDepth->SetLabel(wxT("Discrete Depth: -"));
#endif
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxNodePanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mNode->GetTree();
  
  // Validate
  size_t newIndex;
  if (!mIndex->ValidateNodeIndex(theTree, newIndex)) {
    mIndex->SetValue(mNode->GetIndex());
    return;
  }
   double newXLoc;
  if (!mLocX->ValidateXCoord(theTree, newXLoc)) {
    mLocX->SetValue(mNode->GetLocX());
    return;
  }
  double newYLoc;
  if (!mLocY->ValidateYCoord(theTree, newYLoc)) {
    mLocY->SetValue(mNode->GetLocY());
    return;
  }
  wxString newLabel;
  if (!mLabel->ValidatePartLabel(newLabel)) {
    mLabel->SetValue(mNode->GetLabel());
    return;
  }

  // Update
  bool modIndex = !mIndex->SameValue(mNode->GetIndex());
  bool modLocX = !mLocX->SameValue(mNode->GetLocX());
  bool modLocY = !mLocY->SameValue(mNode->GetLocY());
  bool modLabel = !mLabel->SameValue(mNode->GetLabel());
  bool modSomething = modIndex || modLocX || modLocY || modLabel;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetPartIndex(mNode, newIndex);
      }
      if (modLocX) {
        mNode->SetLocX(newXLoc);
      }
      if (modLocY) {
        mNode->SetLocY(newYLoc);
      }
      if (modLabel) {
        mNode->SetLabel(newLabel.mb_str());
      }
    }
    gDocManager->GetCurrentDocumentLocal()->
      SubmitCommand(wxT("Edit Node"));
    Fill();
  }
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxNodePanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxNodePanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxNodePanel::OnApply)
END_EVENT_TABLE()



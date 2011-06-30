/*******************************************************************************
File:         tmwxEdgePanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for node inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxEdgePanel.h"

// Additional wxWidgets classes
#include "wx/statline.h"

// Additional TreeMaker classes
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxTextCtrl.h"
#include "tmwxStaticText.h"
#include "tmwxDoc.h"
#include "tmwxConditionListBox.h"
#include "tmwxApp.h"
#include "tmwxDocManager.h"

/*****
Constructor
*****/
tmwxEdgePanel::tmwxEdgePanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mEdge(0)
{
  AddTextPair(wxT("Edge Index = "), mIndex);
  AddTextPair(wxT("Length = "), mLength, wxT("%.4f"));
  AddTextPair(wxT("Strain = "), mStrain, wxT("%.4f"));
  AddTextPair(wxT("Stiffness = "), mStiffness, wxT("%.4f"));
  AddTextPair(wxT("Label = "), mLabel);
  AddApplyButton();
  AddStatLine();
  AddConditionListBox(mConditions, wxSize(10, 150));
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mEdgeOwner);
  AddStatTextGridExtra(mStrainedLength);
  AddStatTextGridExtra(mIsPinnedEdge);
  AddStatTextGridExtra(mIsConditionedEdge);
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel with values taken from aEdge, which can be NULL.
*****/
void tmwxEdgePanel::Fill(tmEdge* aEdge)
{
  mEdge = aEdge;
  Fill();
}


/*****
Fill the panel with values taken from mEdge, which can be NULL.
*****/
void tmwxEdgePanel::Fill()
{
  if (!mEdge) return;
  wxString title = wxString::Format(wxT("Edge %s (Node %s - Node %s)"), 
   tmwxStr(mEdge).c_str(), tmwxStr(mEdge->GetNodes().front()).c_str(), 
   tmwxStr(mEdge->GetNodes().back()).c_str());
  mPanelBox->SetLabel(title);
  mIndex->SetValue(mEdge->GetIndex());
  mLength->SetValue(mEdge->GetLength());
  mLabel->SetValue(mEdge->GetLabel());
  mStrain->SetValue(mEdge->GetStrain());
  mStiffness->SetValue(mEdge->GetStiffness());
  mConditions->FillListBox((tmEdge*)mEdge);
#if tmwxINSPECTOR_EXTRA
  mEdgeOwner->SetLabelFormatted(wxT("Owner: %s"), 
    mEdge->GetEdgeOwner());
  mStrainedLength->SetLabelFormatted(wxT("Strained Length: %.4f"), 
    mEdge->GetStrainedLength());
  mIsPinnedEdge->SetLabelFormatted(wxT("Pinned Edge: %s"), 
    mEdge->IsPinnedEdge());
  mIsConditionedEdge->SetLabelFormatted(wxT("Conditioned: %s"), 
    mEdge->IsConditionedEdge());
#endif // tmwxINSPECTOR_EXTRA
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxEdgePanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mEdge->GetTree();
  
  // Validate
  size_t newIndex;
  if (!mIndex->ValidateEdgeIndex(theTree, newIndex)) {
    mIndex->SetValue(mEdge->GetIndex());
    return;
  }
  double newLength;
  if (!mLength->ValidateFloat(wxT("Edge length"), tmEdge::MIN_EDGE_LENGTH, 
    false, newLength)) {
    mLength->SetValue(mEdge->GetLength());
    return;
  }
  double newStrain;
  if (!mStrain->ValidateFloat(wxT("Strain"), -1.0, false, newStrain)) {
    mStrain->SetValue(mEdge->GetStrain());
    return;
  }
  double newStiffness;
  if (!mStiffness->ValidateFloat(wxT("Stiffness"), 0.0, false, newStiffness)) {
    mStiffness->SetValue(mEdge->GetStiffness());
    return;
  }
  wxString newLabel;
  if (!mLabel->ValidatePartLabel(newLabel)) {
    mLabel->SetValue(mEdge->GetLabel());
    return;
  }

  // Update
  bool modIndex = !mIndex->SameValue(mEdge->GetIndex());
  bool modLength = !mLength->SameValue(mEdge->GetLength());
  bool modStrain = !mStrain->SameValue(mEdge->GetStrain());
  bool modStiffness = !mStiffness->SameValue(mEdge->GetStiffness());
  bool modLabel = !mLabel->SameValue(mEdge->GetLabel());
  bool modSomething = modIndex || modLength || modStrain || modStiffness ||
    modLabel;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetPartIndex(mEdge, newIndex);
      }    
      if (modLength) {
        mEdge->SetLength(newLength);
      }
      if (modStrain) {
        mEdge->SetStrain(newStrain);
      }
      if (modStiffness) {
        mEdge->SetStiffness(newStiffness);
      }
      if (modLabel) {
        mEdge->SetLabel(newLabel.mb_str());
      }
    }
    gDocManager->GetCurrentDocumentLocal()->
      SubmitCommand(wxT("Edit Edge"));
    Fill();
  }  
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxEdgePanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxEdgePanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxEdgePanel::OnApply)
END_EVENT_TABLE()



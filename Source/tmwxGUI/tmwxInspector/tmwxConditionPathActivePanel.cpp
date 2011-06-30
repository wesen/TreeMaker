/*******************************************************************************
File:         tmwxConditionPathActivePanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmConditionPathActive inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionPathActivePanel.h"

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

/*****
Constructor
*****/
tmwxConditionPathActivePanel::tmwxConditionPathActivePanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mConditionPathActive(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("From Node "), mNode1);
  AddTextPair(wxT("To Node "), mNode2);
  AddApplyButton();
  AddSpacer(5);
  AddStaticText(mIsFeasible);
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mPath);
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aConditionPathActive, which may be NULL
*****/
void tmwxConditionPathActivePanel::Fill(
  tmConditionPathActive* aConditionPathActive)
{
  mConditionPathActive = aConditionPathActive;
  Fill();
}


/*****
Fill the panel from mConditionPathActive, which may be NULL
*****/
void tmwxConditionPathActivePanel::Fill()
{
  if (!mConditionPathActive) return;
  mPanelBox->SetLabel(wxT("Condition: Path Active (v4)"));
  mIndex->SetValue(mConditionPathActive->GetIndex());
  mNode1->SetValue(mConditionPathActive->GetNode1()->GetIndex());
  mNode2->SetValue(mConditionPathActive->GetNode2()->GetIndex());
  mIsFeasible->SetLabelFeasible(mConditionPathActive);
#if tmwxINSPECTOR_EXTRA
  mPath->SetLabelFormatted(wxT("Path: %s"), 
    tmwxStr(mConditionPathActive->GetPath()));
#endif
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionPathActivePanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionPathActive->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionPathActive->GetIndex());
    return;
  }
  tmNode* newNode1;
  if (!mNode1->ValidateNodeIndexForCondition(theTree, newNode1)) {
    mNode1->SetValue(mConditionPathActive->GetNode1()->GetIndex());
    return;
  }
  tmNode* newNode2;
  if (!mNode2->ValidateNodeIndexForCondition(theTree, newNode2)) {
    mNode2->SetValue(mConditionPathActive->GetNode2()->GetIndex());
    return;
  }
  if (newNode1 == newNode2) {
    tmwxAlertError(wxT("Nodes must be different"));
    mNode1->SetValue(mConditionPathActive->GetNode1()->GetIndex());
    mNode2->SetValue(mConditionPathActive->GetNode2()->GetIndex());
    return;
  }
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionPathActive->GetIndex());
  bool modNode1 = 
    !mNode1->SameValue(mConditionPathActive->GetNode1()->GetIndex());
  bool modNode2 =
    !mNode2->SameValue(mConditionPathActive->GetNode2()->GetIndex());
  bool modSomething = modIndex || modNode1 || modNode2;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionPathActive, newIndex);
      }
      if (modNode1 || modNode2) {
        mConditionPathActive->SetNodes(newNode1, newNode2);
      }
    }
    gDocManager->GetCurrentDocumentLocal()->
      SubmitCommand(wxT("Edit Node Condition"));
    Fill();
  } 
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxConditionPathActivePanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxConditionPathActivePanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionPathActivePanel::OnApply)
END_EVENT_TABLE()

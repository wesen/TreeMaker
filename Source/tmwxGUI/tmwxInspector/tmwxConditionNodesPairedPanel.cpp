/*******************************************************************************
File:         tmwxConditionNodesPairedPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmwxConditionNodesPaired inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionNodesPairedPanel.h"

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
tmwxConditionNodesPairedPanel::tmwxConditionNodesPairedPanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mConditionNodesPaired(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("Node 1 = "), mNode1);
  AddTextPair(wxT("Node 2 = "), mNode2);
  AddApplyButton();
  AddSpacer(5);
  AddStaticText(mIsFeasible);
#if tmwxINSPECTOR_EXTRA
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aConditionNodesPaired, which may be NULL.
*****/
void tmwxConditionNodesPairedPanel::Fill(
  tmConditionNodesPaired* aConditionNodesPaired)
{
  mConditionNodesPaired = aConditionNodesPaired;
  Fill();
}


/*****
Fill the panel from mConditionNodesPaired, which may be NULL.
*****/
void tmwxConditionNodesPairedPanel::Fill()
{
  if (!mConditionNodesPaired) return;
  mPanelBox->SetLabel(wxT("Condition: Nodes Paired"));
  mIndex->SetValue(mConditionNodesPaired->GetIndex());
  mNode1->SetValue(mConditionNodesPaired->GetNode1()->GetIndex());
  mNode2->SetValue(mConditionNodesPaired->GetNode2()->GetIndex());
  mIsFeasible->SetLabelFeasible(mConditionNodesPaired);
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionNodesPairedPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionNodesPaired->GetTree();
  
  // Validation
 size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionNodesPaired->GetIndex());
    return;
  }
  tmNode* newNode1;
  if (!mNode1->ValidateNodeIndexForCondition(theTree, newNode1)) {
    mNode1->SetValue(mConditionNodesPaired->GetNode1()->GetIndex());
    return;
  }
  tmNode* newNode2;
  if (!mNode2->ValidateNodeIndexForCondition(theTree, newNode2)) {
    mNode2->SetValue(mConditionNodesPaired->GetNode2()->GetIndex());
    return;
  }
  if (newNode1 == newNode2) {
    tmwxAlertError(wxT("Nodes must be different"));
    mNode1->SetValue(mConditionNodesPaired->GetNode1()->GetIndex());
    mNode2->SetValue(mConditionNodesPaired->GetNode2()->GetIndex());
    return;
  }
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionNodesPaired->GetIndex());
  bool modNode1 = 
    !mNode1->SameValue(mConditionNodesPaired->GetNode1()->GetIndex());
  bool modNode2 = 
    !mNode2->SameValue(mConditionNodesPaired->GetNode2()->GetIndex());
  bool modSomething = modIndex || modNode1 || modNode2;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionNodesPaired, newIndex);
      }
      if (modNode1 || modNode2) {
        mConditionNodesPaired->SetNode1(newNode1);
        mConditionNodesPaired->SetNode2(newNode2);
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
BEGIN_EVENT_TABLE(tmwxConditionNodesPairedPanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxConditionNodesPairedPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionNodesPairedPanel::OnApply)
END_EVENT_TABLE()

/*******************************************************************************
File:         tmwxConditionNodeSymmetricPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmConditionNodeSymmetric inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionNodeSymmetricPanel.h"

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
tmwxConditionNodeSymmetricPanel::tmwxConditionNodeSymmetricPanel(
  wxWindow* parent) : 
  tmwxInspectorPanel(parent), mConditionNodeSymmetric(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("Node = "), mNode);
  AddApplyButton();
  AddSpacer(5);
  AddStaticText(mIsFeasible);
#if tmwxINSPECTOR_EXTRA
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aConditionNodeSymmetric, which may be NULL.
*****/
void tmwxConditionNodeSymmetricPanel::Fill(
  tmConditionNodeSymmetric* aConditionNodeSymmetric)
{
  mConditionNodeSymmetric = aConditionNodeSymmetric;
  Fill();
}


/*****
Fill the panel from mConditionNodeSymmetric, which may be NULL.
*****/
void tmwxConditionNodeSymmetricPanel::Fill()
{
  if (!mConditionNodeSymmetric) return;
  mPanelBox->SetLabel(wxT("Condition: Node Symmetric (v4)"));
  mIndex->SetValue(mConditionNodeSymmetric->GetIndex());
  mNode->SetValue(mConditionNodeSymmetric->GetNode()->GetIndex());
  mIsFeasible->SetLabelFeasible(mConditionNodeSymmetric);
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionNodeSymmetricPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionNodeSymmetric->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionNodeSymmetric->GetIndex());
    return;
  }
  tmNode* newNode;
  if (!mNode->ValidateNodeIndexForCondition(theTree, newNode)) {
    mNode->SetValue(mConditionNodeSymmetric->GetNode()->GetIndex());
    return;
  }
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionNodeSymmetric->GetIndex());
  bool modNode = 
    !mNode->SameValue(mConditionNodeSymmetric->GetNode()->GetIndex());
  bool modSomething = modIndex || modNode;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionNodeSymmetric, newIndex);
      }
      if (modNode) {
        mConditionNodeSymmetric->SetNode(newNode);
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
BEGIN_EVENT_TABLE(tmwxConditionNodeSymmetricPanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxConditionNodeSymmetricPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionNodeSymmetricPanel::OnApply)
END_EVENT_TABLE()

/*******************************************************************************
File:         tmwxConditionNodeOnCornerPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmConditionNodeOnCorner inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionNodeOnCornerPanel.h"

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
tmwxConditionNodeOnCornerPanel::tmwxConditionNodeOnCornerPanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mConditionNodeOnCorner(0)
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
Fill the panel from aConditionNodeOnCorner, which may be NULL.
*****/
void tmwxConditionNodeOnCornerPanel::Fill(
  tmConditionNodeOnCorner* aConditionNodeOnCorner)
{
  mConditionNodeOnCorner = aConditionNodeOnCorner;
  Fill();
}


/*****
Fill the panel from mConditionNodeOnCorner, which may be NULL.
*****/
void tmwxConditionNodeOnCornerPanel::Fill()
{
  if (!mConditionNodeOnCorner) return;
  mPanelBox->SetLabel(wxT("Condition: Node on Corner (v4)"));
  mIndex->SetValue(mConditionNodeOnCorner->GetIndex());
  mNode->SetValue(mConditionNodeOnCorner->GetNode()->GetIndex());
  mIsFeasible->SetLabelFeasible(mConditionNodeOnCorner);
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionNodeOnCornerPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionNodeOnCorner->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionNodeOnCorner->GetIndex());
    return;
  }
  tmNode* newNode;
  if (!mNode->ValidateNodeIndexForCondition(theTree, newNode)) {
    mNode->SetValue(mConditionNodeOnCorner->GetNode()->GetIndex());
    return;
  }
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionNodeOnCorner->GetIndex());
  bool modNode = 
    !mNode->SameValue(mConditionNodeOnCorner->GetNode()->GetIndex());
  bool modSomething = modIndex || modNode;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionNodeOnCorner, newIndex);
      }
      if (modNode) {
        mConditionNodeOnCorner->SetNode(newNode);
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
BEGIN_EVENT_TABLE(tmwxConditionNodeOnCornerPanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxConditionNodeOnCornerPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionNodeOnCornerPanel::OnApply)
END_EVENT_TABLE()



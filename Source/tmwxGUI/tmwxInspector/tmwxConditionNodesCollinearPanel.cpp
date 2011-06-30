/*******************************************************************************
File:         tmwxConditionNodesCollinearPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmwxConditionNodesCollinear inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionNodesCollinearPanel.h"

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
tmwxConditionNodesCollinearPanel::tmwxConditionNodesCollinearPanel(
  wxWindow* parent) : 
  tmwxInspectorPanel(parent), mConditionNodesCollinear(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("Node 1 = "), mNode1);
  AddTextPair(wxT("Node 2 = "), mNode2);
  AddTextPair(wxT("Node 3 = "), mNode3);
  AddApplyButton();
  AddSpacer(5);
  AddStaticText(mIsFeasible);
#if tmwxINSPECTOR_EXTRA
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aConditionNodesCollinear, which may be NULL.
*****/
void tmwxConditionNodesCollinearPanel::Fill(
  tmConditionNodesCollinear* aConditionNodesCollinear)
{
  mConditionNodesCollinear = aConditionNodesCollinear;
  Fill();
}


/*****
Fill the panel from mConditionNodesCollinear, which may be NULL.
*****/
void tmwxConditionNodesCollinearPanel::Fill()
{
  if (!mConditionNodesCollinear) return;
  mPanelBox->SetLabel(wxT("Condition: Nodes Collinear"));
  mIndex->SetValue(mConditionNodesCollinear->GetIndex());
  mNode1->SetValue(mConditionNodesCollinear->GetNode1()->GetIndex());
  mNode2->SetValue(mConditionNodesCollinear->GetNode2()->GetIndex());
  mNode3->SetValue(mConditionNodesCollinear->GetNode3()->GetIndex());
  mIsFeasible->SetLabelFeasible(mConditionNodesCollinear);
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionNodesCollinearPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionNodesCollinear->GetTree();

  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionNodesCollinear->GetIndex());
    return;
  }
  tmNode* newNode1;
  if (!mNode1->ValidateNodeIndexForCondition(theTree, newNode1)) {
    mNode1->SetValue(mConditionNodesCollinear->GetNode1()->GetIndex());
    return;
  }
  tmNode* newNode2;
  if (!mNode2->ValidateNodeIndexForCondition(theTree, newNode2)) {
    mNode2->SetValue(mConditionNodesCollinear->GetNode2()->GetIndex());
    return;
  }
  tmNode* newNode3;
  if (!mNode3->ValidateNodeIndexForCondition(theTree, newNode3)) {
    mNode3->SetValue(mConditionNodesCollinear->GetNode3()->GetIndex());
    return;
  }
  if (newNode1 == newNode2 || newNode1 == newNode2 || newNode2 == newNode3) {
    tmwxAlertError(wxT("Nodes must be different"));
    mNode1->SetValue(mConditionNodesCollinear->GetNode1()->GetIndex());
    mNode2->SetValue(mConditionNodesCollinear->GetNode2()->GetIndex());
    mNode3->SetValue(mConditionNodesCollinear->GetNode3()->GetIndex());
    return;
  }
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionNodesCollinear->GetIndex());
  bool modNode1 = 
    !mNode1->SameValue(mConditionNodesCollinear->GetNode1()->GetIndex());
  bool modNode2 = 
    !mNode2->SameValue(mConditionNodesCollinear->GetNode2()->GetIndex());
  bool modNode3 = 
    !mNode3->SameValue(mConditionNodesCollinear->GetNode3()->GetIndex());
  bool modSomething = modIndex || modNode1 || modNode2 || modNode3;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionNodesCollinear, newIndex);
      }
      if (modNode1 || modNode1 || modNode3) {
        mConditionNodesCollinear->SetNode1(newNode1);
        mConditionNodesCollinear->SetNode2(newNode2);
        mConditionNodesCollinear->SetNode3(newNode3);
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
BEGIN_EVENT_TABLE(tmwxConditionNodesCollinearPanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxConditionNodesCollinearPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionNodesCollinearPanel::OnApply)
END_EVENT_TABLE()

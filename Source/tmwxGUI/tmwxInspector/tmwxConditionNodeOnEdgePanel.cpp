/*******************************************************************************
File:         tmwxConditionNodeOnEdgePanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmwxConditionNodeOnEdge inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionNodeOnEdgePanel.h"

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
tmwxConditionNodeOnEdgePanel::tmwxConditionNodeOnEdgePanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mConditionNodeOnEdge(0)
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
Fill the panel from aConditionNodeOnEdge, which may be NULL.
*****/
void tmwxConditionNodeOnEdgePanel::Fill(
  tmConditionNodeOnEdge* aConditionNodeOnEdge)
{
  mConditionNodeOnEdge = aConditionNodeOnEdge;
  Fill();
}


/*****
Fill the panel from mConditionNodeOnEdge, which may be NULL.
*****/
void tmwxConditionNodeOnEdgePanel::Fill()
{
  if (!mConditionNodeOnEdge) return;
  mPanelBox->SetLabel(wxT("Condition: Node on Edge (v4)"));
  mIndex->SetValue(mConditionNodeOnEdge->GetIndex());
  mNode->SetValue(mConditionNodeOnEdge->GetNode()->GetIndex());
  mIsFeasible->SetLabelFeasible(mConditionNodeOnEdge);
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionNodeOnEdgePanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionNodeOnEdge->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionNodeOnEdge->GetIndex());
    return;
  }
  tmNode* newNode;
  if (!mNode->ValidateNodeIndexForCondition(theTree, newNode)) {
    mNode->SetValue(mConditionNodeOnEdge->GetNode()->GetIndex());
    return;
  }
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionNodeOnEdge->GetIndex());
  bool modNode = 
    !mNode->SameValue(mConditionNodeOnEdge->GetNode()->GetIndex());
  bool modSomething = modIndex || modNode;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionNodeOnEdge, newIndex);
      }
      if (modNode) {
        mConditionNodeOnEdge->SetNode(newNode);
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
BEGIN_EVENT_TABLE(tmwxConditionNodeOnEdgePanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxConditionNodeOnEdgePanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionNodeOnEdgePanel::OnApply)
END_EVENT_TABLE()



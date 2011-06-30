/*******************************************************************************
File:         tmwxConditionEdgesSameStrainPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmwxConditionEdgesSameStrain inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionEdgesSameStrainPanel.h"

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
tmwxConditionEdgesSameStrainPanel::tmwxConditionEdgesSameStrainPanel(
  wxWindow* parent) : 
  tmwxInspectorPanel(parent), mConditionEdgesSameStrain(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("Edge 1 = "), mEdge1);
  AddTextPair(wxT("Edge 2 = "), mEdge2);
  AddApplyButton();
  AddSpacer(5);
  AddStaticText(mIsFeasible);
#if tmwxINSPECTOR_EXTRA
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aConditionEdgesSameStrain, which can be NULL.
*****/
void tmwxConditionEdgesSameStrainPanel::Fill(
  tmConditionEdgesSameStrain* aConditionEdgesSameStrain)
{
  mConditionEdgesSameStrain = aConditionEdgesSameStrain;
  Fill();
}


/*****
Fill the panel from mConditionEdgesSameStrain, which can be NULL.
*****/
void tmwxConditionEdgesSameStrainPanel::Fill()
{
  if (!mConditionEdgesSameStrain) return;
  mPanelBox->SetLabel(wxT("Condition: Edges Same Strain"));
  mIndex->SetValue(mConditionEdgesSameStrain->GetIndex());
  mEdge1->SetValue(mConditionEdgesSameStrain->GetEdge1()->GetIndex());
  mEdge2->SetValue(mConditionEdgesSameStrain->GetEdge2()->GetIndex());
  mIsFeasible->SetLabelFeasible(mConditionEdgesSameStrain);
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionEdgesSameStrainPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionEdgesSameStrain->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionEdgesSameStrain->GetIndex());
    return;
  }
  tmEdge* newEdge1;
  if (!mEdge1->ValidateEdgeIndexForCondition(theTree, newEdge1)) {
    mEdge1->SetValue(mConditionEdgesSameStrain->GetEdge1()->GetIndex());
    return;
  }
  tmEdge* newEdge2;
  if (!mEdge2->ValidateEdgeIndexForCondition(theTree, newEdge2)) {
    mEdge2->SetValue(mConditionEdgesSameStrain->GetEdge2()->GetIndex());
    return;
  }
  if (newEdge1 == newEdge2) {
    tmwxAlertError(wxT("Edges must be different"));
    mEdge1->SetValue(mConditionEdgesSameStrain->GetEdge1()->GetIndex());
    mEdge2->SetValue(mConditionEdgesSameStrain->GetEdge2()->GetIndex());
    return;
  }  
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionEdgesSameStrain->GetIndex());
  bool modEdge1 = 
    !mEdge1->SameValue(mConditionEdgesSameStrain->GetEdge1()->GetIndex());
  bool modEdge2 = 
    !mEdge2->SameValue(mConditionEdgesSameStrain->GetEdge2()->GetIndex());
  bool modSomething = modIndex || modEdge1 || modEdge2;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionEdgesSameStrain, newIndex);
      }
      if (modEdge1 || modEdge2) {
        mConditionEdgesSameStrain->SetEdge1(newEdge1);
        mConditionEdgesSameStrain->SetEdge2(newEdge2);
      }
    }
    gDocManager->GetCurrentDocumentLocal()->
      SubmitCommand(wxT("Edit Edge Condition"));
    Fill();
  } 
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxConditionEdgesSameStrainPanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxConditionEdgesSameStrainPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionEdgesSameStrainPanel::OnApply)
END_EVENT_TABLE()



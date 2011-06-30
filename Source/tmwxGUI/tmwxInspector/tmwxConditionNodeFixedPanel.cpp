/*******************************************************************************
File:         tmwxConditionNodeFixedPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for node inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionNodeFixedPanel.h"

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
#include "tmwxCheckBoxSmall.h"

/*****
Constructor
*****/
tmwxConditionNodeFixedPanel::tmwxConditionNodeFixedPanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mConditionNodeFixed(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("Node = "), mNode);
  AddCheckPair(mXFixed, wxT("X Fixed to"), mXFixValue, wxT("%.4f"));
  AddCheckPair(mYFixed, wxT("Y Fixed to"), mYFixValue, wxT("%.4f"));
  AddApplyButton();
  AddSpacer(5);
  AddStaticText(mIsFeasible);
#if tmwxINSPECTOR_EXTRA
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aConditionNodeFixed, which may be NULL.
*****/
void tmwxConditionNodeFixedPanel::Fill(
  tmConditionNodeFixed* aConditionNodeFixed)
{
  mConditionNodeFixed = aConditionNodeFixed;
  Fill();
}


/*****
Fill the panel from mConditionNodeFixed, which may be NULL.
*****/
void tmwxConditionNodeFixedPanel::Fill()
{
  if (!mConditionNodeFixed) return;
  mPanelBox->SetLabel(wxT("Condition: Node Fixed (v4)"));
  mIndex->SetValue(mConditionNodeFixed->GetIndex());
  mNode->SetValue(mConditionNodeFixed->GetNode()->GetIndex());
  mXFixed->SetValue(mConditionNodeFixed->GetXFixed());
  mXFixValue->SetValue(mConditionNodeFixed->GetXFixValue());
  mYFixed->SetValue(mConditionNodeFixed->GetYFixed());
  mYFixValue->SetValue(mConditionNodeFixed->GetYFixValue());
  mIsFeasible->SetLabelFeasible(mConditionNodeFixed);
}


/*****
Respond to the x-fixed or y-fixed checkbox. Enable/disable text fields,
then validate and update if appropriate.
*****/
void tmwxConditionNodeFixedPanel::OnCheckBox(wxCommandEvent& event)
{
  wxObject* theCheckBox = event.GetEventObject();
  
  if (theCheckBox == mXFixed) {
    mXFixValue->Enable(mXFixed->GetValue());
  }
  else if (theCheckBox == mYFixed) {
    mYFixValue->Enable(mYFixed->GetValue());
  }
  else TMFAIL("unknown checkbox");
  OnApply(event);
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionNodeFixedPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionNodeFixed->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionNodeFixed->GetIndex());
    return;
  }
  tmNode* newNode;
  if (!mNode->ValidateNodeIndexForCondition(theTree, newNode)) {
    mNode->SetValue(mConditionNodeFixed->GetNode()->GetIndex());
    return;
  }
  bool newXFixed = mXFixed->GetValue();
  double newXFixValue;
  if (newXFixed) {
    if (!mXFixValue->ValidateXCoord(theTree, newXFixValue)) {
      mXFixValue->SetValue(mConditionNodeFixed->GetXFixValue());
      return;
    }
  }
  bool newYFixed = mYFixed->GetValue();
  double newYFixValue;
  if (newYFixed) {
    if (!mYFixValue->ValidateYCoord(theTree, newYFixValue)) {
      mYFixValue->SetValue(mConditionNodeFixed->GetYFixValue());
      return;
    }
  } 
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionNodeFixed->GetIndex());
  bool modNode = newNode != mConditionNodeFixed->GetNode();
  bool modXFixed = newXFixed != mConditionNodeFixed->GetXFixed();
  bool modXFixValue = 
    !mXFixValue->SameValue(mConditionNodeFixed->GetXFixValue());
  bool modYFixed = newYFixed != mConditionNodeFixed->GetYFixed();
  bool modYFixValue = 
    !mYFixValue->SameValue(mConditionNodeFixed->GetYFixValue());
  bool modSomething = modIndex || modNode || modXFixed || modYFixed ||
    (newXFixed && modXFixValue) || (newYFixed && modYFixValue);
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionNodeFixed, newIndex);
      }
      if (modNode) {
        mConditionNodeFixed->SetNode(newNode);
      }
      if (modXFixed) {
        mConditionNodeFixed->SetXFixed(newXFixed);
      }
      if (newXFixed && modXFixValue) {
          mConditionNodeFixed->SetXFixValue(newXFixValue);
      }
      if (modYFixed) {
        mConditionNodeFixed->SetYFixed(newYFixed);
      }
      if (newYFixed && modYFixValue) {
          mConditionNodeFixed->SetYFixValue(newYFixValue);
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
BEGIN_EVENT_TABLE(tmwxConditionNodeFixedPanel, wxPanel)
  EVT_CHECKBOX(wxID_ANY, tmwxConditionNodeFixedPanel::OnCheckBox)
  EVT_BUTTON(wxID_APPLY, tmwxConditionNodeFixedPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionNodeFixedPanel::OnApply)
END_EVENT_TABLE()



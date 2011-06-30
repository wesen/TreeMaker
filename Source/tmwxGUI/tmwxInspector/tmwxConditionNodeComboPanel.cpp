/*******************************************************************************
File:         tmwxConditionNodeComboPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for node inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-22
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionNodeComboPanel.h"

// Additional wxWidgets classes
#include "wx/statline.h"

// Additional TreeMaker classes
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxTextCtrl.h"
#include "tmwxStaticText.h"
#include "tmwxDoc.h"
#include "tmwxCheckBoxSmall.h"
#include "tmwxApp.h"
#include "tmwxDocManager.h"

/*****
Constructor
*****/
tmwxConditionNodeComboPanel::tmwxConditionNodeComboPanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mConditionNodeCombo(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("Node = "), mNode);
  AddCheckBox(mToSymmetryLine, wxT("Fixed to Symmetry Line"));
  AddCheckBox(mToPaperEdge, wxT("Fixed to Paper Edge"));
  AddCheckBox(mToPaperCorner, wxT("Fixed to Paper Corner"));
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
Fill the panel from aConditionNodeCombo, which may be NULL.
*****/
void tmwxConditionNodeComboPanel::Fill(
  tmConditionNodeCombo* aConditionNodeCombo)
{
  mConditionNodeCombo = aConditionNodeCombo;
  Fill();
}


/*****
Fill the panel from mConditionNodeCombo, which may be NULL.
*****/
void tmwxConditionNodeComboPanel::Fill()
{
  if (!mConditionNodeCombo) return;
  mPanelBox->SetLabel(wxT("Condition: Node Fixed"));
  mIndex->SetValue(mConditionNodeCombo->GetIndex());
  mNode->SetValue(mConditionNodeCombo->GetNode()->GetIndex());
  mToSymmetryLine->SetValue(mConditionNodeCombo->GetToSymmetryLine());
  mToPaperEdge->SetValue(mConditionNodeCombo->GetToPaperEdge());
  mToPaperCorner->SetValue(mConditionNodeCombo->GetToPaperCorner());
  mXFixed->SetValue(mConditionNodeCombo->GetXFixed());
  mXFixValue->SetValue(mConditionNodeCombo->GetXFixValue());
  mXFixValue->Enable(mConditionNodeCombo->GetXFixed());
  mYFixed->SetValue(mConditionNodeCombo->GetYFixed());
  mYFixValue->SetValue(mConditionNodeCombo->GetYFixValue());
  mYFixValue->Enable(mConditionNodeCombo->GetYFixed());
  mIsFeasible->SetLabelFeasible(mConditionNodeCombo);
}


/*****
Respond to any of the checkboxes by applying the current settings and setting
focus to the appropriate edit text field if we've turned on or off an item that
has an edit field.
*****/
void tmwxConditionNodeComboPanel::OnCheckBox(wxCommandEvent& event)
{
  OnApply(event);
  wxObject* theCheckBox = event.GetEventObject();
  if (theCheckBox == mXFixed) {
    if (mXFixed->GetValue())
      mXFixValue->SetFocus();
    else
      mIndex->SetFocus();
  }
  else if (theCheckBox == mYFixed) {
    if (mYFixed->GetValue())
      mYFixValue->SetFocus();
    else
      mIndex->SetFocus();
  }
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionNodeComboPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionNodeCombo->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionNodeCombo->GetIndex());
    return;
  }
  tmNode* newNode;
  if (!mNode->ValidateNodeIndexForCondition(theTree, newNode)) {
    mNode->SetValue(mConditionNodeCombo->GetNode()->GetIndex());
    return;
  }
  bool newToSymmetryLine = mToSymmetryLine->GetValue();
  bool newToPaperEdge = mToPaperEdge->GetValue();
  bool newToPaperCorner = mToPaperCorner->GetValue();
  bool newXFixed = mXFixed->GetValue();
  tmFloat newXFixValue;
  if (newXFixed) {
    if (!mXFixValue->ValidateXCoord(theTree, newXFixValue)) {
      mXFixValue->SetValue(mConditionNodeCombo->GetXFixValue());
      return;
    }
  }
  bool newYFixed = mYFixed->GetValue();
  tmFloat newYFixValue;
  if (newYFixed) {
    if (!mYFixValue->ValidateYCoord(theTree, newYFixValue)) {
      mYFixValue->SetValue(mConditionNodeCombo->GetYFixValue());
      return;
    }
  }
  
  // Enabling of edit fields
  mXFixValue->Enable(newXFixed);
  mYFixValue->Enable(newYFixed);
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionNodeCombo->GetIndex());
  bool modNode = newNode != mConditionNodeCombo->GetNode();
  bool modToSymmetryLine = 
    newToSymmetryLine != mConditionNodeCombo->GetToSymmetryLine();
  bool modToPaperEdge = 
    newToPaperEdge != mConditionNodeCombo->GetToPaperEdge();
  bool modToPaperCorner = 
    newToPaperCorner != mConditionNodeCombo->GetToPaperCorner();
  bool modXFixed = newXFixed != mConditionNodeCombo->GetXFixed();
  bool modXFixValue = 
    !mXFixValue->SameValue(mConditionNodeCombo->GetXFixValue());
  bool modYFixed = newYFixed != mConditionNodeCombo->GetYFixed();
  bool modYFixValue = 
    !mYFixValue->SameValue(mConditionNodeCombo->GetYFixValue());
  bool modSomething = modIndex || modNode || modToSymmetryLine || 
    modToPaperEdge || modToPaperCorner || modXFixed || modYFixed ||
    (newXFixed && modXFixValue) || (newYFixed && modYFixValue);
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionNodeCombo, newIndex);
      }
      if (modNode) {
        mConditionNodeCombo->SetNode(newNode);
      }
      if (modToSymmetryLine) {
        mConditionNodeCombo->SetToSymmetryLine(newToSymmetryLine);
      }
      if (modToPaperEdge) {
        mConditionNodeCombo->SetToPaperEdge(newToPaperEdge);
      }
      if (modToPaperCorner) {
        mConditionNodeCombo->SetToPaperCorner(newToPaperCorner);
      }
      if (modXFixed) {
        mConditionNodeCombo->SetXFixed(newXFixed);
      }
      if (newXFixed && modXFixValue) {
          mConditionNodeCombo->SetXFixValue(newXFixValue);
      }
      if (modYFixed) {
        mConditionNodeCombo->SetYFixed(newYFixed);
      }
      if (newYFixed && modYFixValue) {
          mConditionNodeCombo->SetYFixValue(newYFixValue);
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
BEGIN_EVENT_TABLE(tmwxConditionNodeComboPanel, wxPanel)
  EVT_CHECKBOX(wxID_ANY, tmwxConditionNodeComboPanel::OnCheckBox)
  EVT_BUTTON(wxID_APPLY, tmwxConditionNodeComboPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionNodeComboPanel::OnApply)
END_EVENT_TABLE()



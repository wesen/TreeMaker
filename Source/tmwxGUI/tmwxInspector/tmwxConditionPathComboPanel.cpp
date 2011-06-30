/*******************************************************************************
File:         tmwxConditionPathComboPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmConditionPathCombo inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-10
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionPathComboPanel.h"

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
#include "tmwxRadioBoxSmall.h"


/*****
Radio button constants
*****/
const int ACTIVE_ONLY = 0;
const int ANGLE_FIXED = 1;
const int ANGLE_QUANT = 2;


/*****
Constructor
*****/
tmwxConditionPathComboPanel::tmwxConditionPathComboPanel(
  wxWindow* parent) : 
  tmwxInspectorPanel(parent), mConditionPathCombo(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("From Node = "), mNode1);
  AddTextPair(wxT("To Node = "), mNode2);
  
  const wxString choices[3] = {
    wxT("Active Only"),
    wxT("Angle Fixed"),
    wxT("Angle Quantized")
  };
  mRadioBox = new tmwxRadioBoxSmall(this, wxT("Path Constraints"), 3, choices);
  mPanelSizer->Add(mRadioBox, 0, wxEXPAND | wxLEFT|wxRIGHT, 10);
  
  AddTextPair(wxT("Fix Angle = "), mAngle, wxT("%.2f"));
  AddTextPair(wxT("Quantization = "), mQuant);
  AddTextPair(wxT("Offset = "), mQuantOffset, wxT("%.2f"));
  AddApplyButton();
  AddSpacer(5);
  AddStaticText(mIsFeasible);
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mPath);
  AddStatTextGridExtra(mCurAngle);
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aConditionPathCombo, which may be NULL.
*****/
void tmwxConditionPathComboPanel::Fill(
  tmConditionPathCombo* aConditionPathCombo)
{
  mConditionPathCombo = aConditionPathCombo;
  Fill();
}


/*****
Fill the panel from mConditionPathCombo, which may be NULL.
*****/
void tmwxConditionPathComboPanel::Fill()
{
  if (!mConditionPathCombo) return;
  mPanelBox->SetLabel(wxT("Condition: Path Active"));
  mIndex->SetValue(mConditionPathCombo->GetIndex());
  mNode1->SetValue(mConditionPathCombo->GetNode1()->GetIndex());
  mNode2->SetValue(mConditionPathCombo->GetNode2()->GetIndex());
  if (mConditionPathCombo->IsAngleFixed()) {
    mRadioBox->SetSelection(ANGLE_FIXED);
    mAngle->Enable();
    mQuant->Disable();
    mQuantOffset->Disable();
  }
  else if (mConditionPathCombo->IsAngleQuant()) {
    mRadioBox->SetSelection(ANGLE_QUANT);
    mAngle->Disable();
    mQuant->Enable();
    mQuantOffset->Enable();
  }
  else {
    mRadioBox->SetSelection(ACTIVE_ONLY);
    mAngle->Disable();
    mQuant->Disable();
    mQuantOffset->Disable();
  }
  mAngle->SetValue(mConditionPathCombo->GetAngle());
  mQuant->SetValue(mConditionPathCombo->GetQuant());
  mQuantOffset->SetValue(mConditionPathCombo->GetQuantOffset());
  mIsFeasible->SetLabelFeasible(mConditionPathCombo);
#if tmwxINSPECTOR_EXTRA
  mPath->SetLabelFormatted(wxT("Path: %s"), 
    tmwxStr(mConditionPathCombo->GetPath()));
  mCurAngle->SetLabelFormatted(wxT("Current Angle: %.2f"), 
    mConditionPathCombo->GetPath()->GetPositiveAngle());
#endif
}


/*****
Respond to a change of radio box selection. Enable/disable text fields, then
validate as if we had hit TextEnter.
*****/
void tmwxConditionPathComboPanel::OnRadioBox(wxCommandEvent& event)
{
  int sel = mRadioBox->GetSelection();
  switch(sel) {
    case ACTIVE_ONLY: {
      mAngle->Disable();
      mQuant->Disable();
      mQuantOffset->Disable();
      mIndex->SetFocus();
    }
    break;
    case ANGLE_FIXED: {
      mAngle->Enable();
      mQuant->Disable();
      mQuantOffset->Disable();
      mAngle->SetFocus();
    }
    break;
    case ANGLE_QUANT: {
      mAngle->Disable();
      mQuant->Enable();
      mQuantOffset->Enable();
      mQuant->SetFocus();
    }
    break;
    default:
      TMFAIL("undefined case");
  }
  OnApply(event);
}


/*****
On either radio button strike or text entry, validate all data and if valid,
transfer data to tree.
*****/
void tmwxConditionPathComboPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionPathCombo->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionPathCombo->GetIndex());
    return;
  }
  tmNode* newNode1;
  if (!mNode1->ValidateNodeIndexForCondition(theTree, newNode1)) {
    mNode1->SetValue(mConditionPathCombo->GetNode1()->GetIndex());
    return;
  }
  tmNode* newNode2;
  if (!mNode2->ValidateNodeIndexForCondition(theTree, newNode2)) {
    mNode2->SetValue(mConditionPathCombo->GetNode2()->GetIndex());
    return;
  }
 if (newNode1 == newNode2) {
    tmwxAlertError(wxT("Nodes must be different"));
    mNode1->SetValue(mConditionPathCombo->GetNode1()->GetIndex());
    mNode2->SetValue(mConditionPathCombo->GetNode2()->GetIndex());
    return;
  }
  int sel = mRadioBox->GetSelection();
  bool newAngleFixed = (sel == ANGLE_FIXED);
  bool newAngleQuant = (sel == ANGLE_QUANT);
  tmFloat newAngle;
  if (newAngleFixed) {
    if (!mAngle->ValidateFloat(wxT("Fix Angle"), newAngle)) {
      mAngle->SetValue(mConditionPathCombo->GetAngle());
      return;
    }
  }
  size_t newQuant;
  tmFloat newQuantOffset;
  if (newAngleQuant) {
   if (!mQuant->ValidateSizeT(wxT("Quantization"), 2, newQuant)) {
      mQuant->SetValue(mConditionPathCombo->GetQuant());
      return;
    }
    if (!mQuantOffset->ValidateFloat(wxT("Offset"), newQuantOffset)) {
      mQuantOffset->SetValue(mConditionPathCombo->GetQuantOffset());
      return;
    }
  }
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionPathCombo->GetIndex());
  bool modNode1 = 
    !mNode1->SameValue(mConditionPathCombo->GetNode1()->GetIndex());
  bool modNode2 = 
    !mNode2->SameValue(mConditionPathCombo->GetNode2()->GetIndex());
  bool modAngleFixed = 
    (newAngleFixed != mConditionPathCombo->IsAngleFixed());
  bool modAngle = !mAngle->SameValue(mConditionPathCombo->GetAngle());
  bool modAngleQuant =
    (newAngleQuant != mConditionPathCombo->IsAngleQuant());
  bool modQuant = !mQuant->SameValue(mConditionPathCombo->GetQuant());
  bool modQuantOffset = 
    !mQuantOffset->SameValue(mConditionPathCombo->GetQuantOffset());
  bool modSomething = modIndex || modNode1 || modNode2 || modAngleFixed ||
    modAngleQuant || 
    (newAngleFixed && modAngle) ||
    (newAngleQuant && (modQuant || modQuantOffset));
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionPathCombo, newIndex);
      }
      if (modNode1 || modNode2) {
        mConditionPathCombo->SetNodes(newNode1, newNode2);
      }
      if (modAngleFixed || modAngleQuant) {
        mConditionPathCombo->SetAngleFixed(newAngleFixed);
        mConditionPathCombo->SetAngleQuant(newAngleQuant);
      }
      if (newAngleFixed && modAngle) {
        mConditionPathCombo->SetAngle(newAngle);
      }
      if (newAngleQuant && modQuant) {
        mConditionPathCombo->SetQuant(newQuant);
      }
      if (newQuantOffset && modQuantOffset) {
        mConditionPathCombo->SetQuantOffset(newQuantOffset);
      }
    }
    gDocManager->GetCurrentDocumentLocal()->
      SubmitCommand(wxT("Edit Path Condition"));
    Fill();
  } 
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxConditionPathComboPanel, wxPanel)
  EVT_RADIOBOX(wxID_ANY, tmwxConditionPathComboPanel::OnApply)
  EVT_BUTTON(wxID_APPLY, tmwxConditionPathComboPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionPathComboPanel::OnApply)
END_EVENT_TABLE()



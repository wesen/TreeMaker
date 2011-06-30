/*******************************************************************************
File:         tmwxConditionPathAngleQuantPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmConditionPathAngleQuant inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionPathAngleQuantPanel.h"

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
tmwxConditionPathAngleQuantPanel::tmwxConditionPathAngleQuantPanel(
  wxWindow* parent) : 
  tmwxInspectorPanel(parent), mConditionPathAngleQuant(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("From Node = "), mNode1);
  AddTextPair(wxT("To Node = "), mNode2);
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
Fill the panel from aConditionPathAngleQuant, which may be NULL.
*****/
void tmwxConditionPathAngleQuantPanel::Fill(
  tmConditionPathAngleQuant* aConditionPathAngleQuant)
{
  mConditionPathAngleQuant = aConditionPathAngleQuant;
  Fill();
}


/*****
Fill the panel from mConditionPathAngleQuant, which may be NULL.
*****/
void tmwxConditionPathAngleQuantPanel::Fill()
{
  if (!mConditionPathAngleQuant) return;
  mPanelBox->SetLabel(wxT("Condition: Path Angle Quantized (v4)"));
  mIndex->SetValue(mConditionPathAngleQuant->GetIndex());
  mNode1->SetValue(mConditionPathAngleQuant->GetNode1()->GetIndex());
  mNode2->SetValue(mConditionPathAngleQuant->GetNode2()->GetIndex());
  mQuant->SetValue(mConditionPathAngleQuant->GetQuant());
  mQuantOffset->SetValue(mConditionPathAngleQuant->GetQuantOffset());
  mIsFeasible->SetLabelFeasible(mConditionPathAngleQuant);
#if tmwxINSPECTOR_EXTRA
  mPath->SetLabelFormatted(wxT("Path: %s"), 
    tmwxStr(mConditionPathAngleQuant->GetPath()));
  mCurAngle->SetLabelFormatted(wxT("Current Angle: %.2f"), 
    mConditionPathAngleQuant->GetPath()->GetPositiveAngle());
#endif
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionPathAngleQuantPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionPathAngleQuant->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionPathAngleQuant->GetIndex());
    return;
  }
  tmNode* newNode1;
  if (!mNode1->ValidateNodeIndexForCondition(theTree, newNode1)) {
    mNode1->SetValue(mConditionPathAngleQuant->GetNode1()->GetIndex());
    return;
  }
  tmNode* newNode2;
  if (!mNode2->ValidateNodeIndexForCondition(theTree, newNode2)) {
    mNode2->SetValue(mConditionPathAngleQuant->GetNode2()->GetIndex());
    return;
  }
  if (newNode1 == newNode2) {
    tmwxAlertError(wxT("Nodes must be different"));
    mNode1->SetValue(mConditionPathAngleQuant->GetNode1()->GetIndex());
    mNode2->SetValue(mConditionPathAngleQuant->GetNode2()->GetIndex());
    return;
  }
  size_t newQuant;
  if (!mQuant->ValidateSizeT(wxT("Quantization"), 2, newQuant)) {
    mQuant->SetValue(mConditionPathAngleQuant->GetQuant());
    return;
  }
  tmFloat newQuantOffset;
  if (!mQuantOffset->ValidateFloat(wxT("Offset"), newQuantOffset)) {
    mQuantOffset->SetValue(mConditionPathAngleQuant->GetQuantOffset());
    return;
  }
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionPathAngleQuant->GetIndex());
  bool modNode1 = 
    !mNode1->SameValue(mConditionPathAngleQuant->GetNode1()->GetIndex());
  bool modNode2 = 
    !mNode2->SameValue(mConditionPathAngleQuant->GetNode2()->GetIndex());
  bool modQuant = !mQuant->SameValue(mConditionPathAngleQuant->GetQuant());
  bool modQuantOffset = 
    !mQuantOffset->SameValue(mConditionPathAngleQuant->GetQuantOffset());
  bool modSomething = modIndex || modNode1 || modNode2 || modQuant ||
    modQuantOffset;
  if (modSomething) {
    {
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionPathAngleQuant, newIndex);
      }
      if (modNode1 ||
        modNode2) {
        mConditionPathAngleQuant->SetNodes(newNode1, newNode2);
      }
      if (modQuant) {
        mConditionPathAngleQuant->SetQuant(size_t(newQuant));
      }
      if (modQuantOffset) {
        mConditionPathAngleQuant->SetQuantOffset(newQuantOffset);
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
BEGIN_EVENT_TABLE(tmwxConditionPathAngleQuantPanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxConditionPathAngleQuantPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionPathAngleQuantPanel::OnApply)
END_EVENT_TABLE()

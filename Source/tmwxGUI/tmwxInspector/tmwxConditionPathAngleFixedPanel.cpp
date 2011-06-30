/*******************************************************************************
File:         tmwxConditionPathAngleFixedPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for tmConditionPathAngleFixed inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionPathAngleFixedPanel.h"

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
tmwxConditionPathAngleFixedPanel::tmwxConditionPathAngleFixedPanel(
  wxWindow* parent) : 
  tmwxInspectorPanel(parent), mConditionPathAngleFixed(0)
{
  AddTextPair(wxT("Condition Index = "), mIndex);
  AddTextPair(wxT("From Node = "), mNode1);
  AddTextPair(wxT("To Node = "), mNode2);
  AddTextPair(wxT("Angle = "), mAngle, wxT("%.2f"));
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
Fill the panel from aConditionPathAngleFixed, which may be NULL.
*****/
void tmwxConditionPathAngleFixedPanel::Fill(
  tmConditionPathAngleFixed* aConditionPathAngleFixed)
{
  mConditionPathAngleFixed = aConditionPathAngleFixed;
  Fill();
}


/*****
Fill the panel from mConditionPathAngleFixed, which may be NULL.
*****/
void tmwxConditionPathAngleFixedPanel::Fill()
{
  if (!mConditionPathAngleFixed) return;
  mPanelBox->SetLabel(wxT("Condition: Path Angle Fixed (v4)"));
  mIndex->SetValue(mConditionPathAngleFixed->GetIndex());
  mNode1->SetValue(mConditionPathAngleFixed->GetNode1()->GetIndex());
  mNode2->SetValue(mConditionPathAngleFixed->GetNode2()->GetIndex());
  mAngle->SetValue(mConditionPathAngleFixed->GetAngle());
  mIsFeasible->SetLabelFeasible(mConditionPathAngleFixed);
#if tmwxINSPECTOR_EXTRA
  mPath->SetLabelFormatted(wxT("Path: %s"), 
    tmwxStr(mConditionPathAngleFixed->GetPath()));
  mCurAngle->SetLabelFormatted(wxT("Current Angle: %.2f"),
    mConditionPathAngleFixed->GetPath()->GetPositiveAngle());
#endif
}


/*****
If one of our text controls received an Enter key, update the structure with
all text values
*****/
void tmwxConditionPathAngleFixedPanel::OnApply(wxCommandEvent&)
{
  tmTree* theTree = mConditionPathAngleFixed->GetTree();
  
  // Validation
  size_t newIndex;
  if (!mIndex->ValidateConditionIndex(theTree, newIndex)) {
    mIndex->SetValue(mConditionPathAngleFixed->GetIndex());
    return;
  }
  tmNode* newNode1;
  if (!mNode1->ValidateNodeIndexForCondition(theTree, newNode1)) {
    mNode1->SetValue(mConditionPathAngleFixed->GetNode1()->GetIndex());
    return;
  }
  tmNode* newNode2;
  if (!mNode2->ValidateNodeIndexForCondition(theTree, newNode2)) {
    mNode2->SetValue(mConditionPathAngleFixed->GetNode2()->GetIndex());
    return;
  }
  if (newNode1 == newNode2) {
    tmwxAlertError(wxT("Nodes must be different"));
    mNode1->SetValue(mConditionPathAngleFixed->GetNode1()->GetIndex());
    mNode2->SetValue(mConditionPathAngleFixed->GetNode2()->GetIndex());
    return;
  }
  tmFloat newAngle;
  if (!mAngle->ValidateFloat(wxT("Angle"), newAngle)) {
    mAngle->SetValue(mConditionPathAngleFixed->GetAngle());
    return;
  }
  
  // Updating
  bool modIndex = !mIndex->SameValue(mConditionPathAngleFixed->GetIndex());
  bool modNode1 = 
    !mNode1->SameValue(mConditionPathAngleFixed->GetNode1()->GetIndex());
  bool modNode2 = 
    !mNode2->SameValue(mConditionPathAngleFixed->GetNode2()->GetIndex());
  bool modAngle = !mAngle->SameValue(mConditionPathAngleFixed->GetAngle());
  bool modSomething = modIndex || modNode1 || modNode2 || modAngle;
  if (modSomething) {
    {    
      tmTreeCleaner tc(theTree);
      if (modIndex) {
        theTree->SetConditionIndex(mConditionPathAngleFixed, newIndex);
      }
      if (modNode1 || modNode2) {
        mConditionPathAngleFixed->SetNodes(newNode1, newNode2);
      }
      if (modAngle) {
        mConditionPathAngleFixed->SetAngle(newAngle);
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
BEGIN_EVENT_TABLE(tmwxConditionPathAngleFixedPanel, wxPanel)
  EVT_BUTTON(wxID_APPLY, tmwxConditionPathAngleFixedPanel::OnApply)
  EVT_TEXT_ENTER(wxID_ANY, tmwxConditionPathAngleFixedPanel::OnApply)
END_EVENT_TABLE()

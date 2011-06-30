/*******************************************************************************
File:         tmwxDoc_Condition.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker document class, Condtion menu commands
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-10
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDoc.h"
#include "tmwxApp.h"
#include "tmModel.h"
#include "tmwxGetUserInputDialog.h"
#include "tmwxTextCtrl.h"
#include "tmwxCheckBox.h"

using namespace std;

/**********
class tmwxNodeFixedDialog
Utility dialog for setting node position
**********/
class tmwxNodeFixedDialog : public tmwxGetUserInputDialog {
public:
  tmwxCheckBox* mXFixed;
  tmwxTextCtrl* mXFixValueText;
  double mXFixValue;
  tmwxCheckBox* mYFixed;
  tmwxTextCtrl* mYFixValueText;
  double mYFixValue;
  tmTree* mTree;
  bool mMultiple;

  tmwxNodeFixedDialog(tmTree* aTree, bool aMultiple);
  bool Validate();
};


/*****
Constructor
*****/
tmwxNodeFixedDialog::tmwxNodeFixedDialog(tmTree* aTree, bool aMultiple)
  : tmwxGetUserInputDialog(wxT("Node Position Fixed")),
  mTree(aTree),
  mMultiple(aMultiple)
{
  AddCheckPair(mXFixed, wxT("X fixed to"), mXFixValueText, wxT("%.4f"));
  AddCheckPair(mYFixed, wxT("Y fixed to"), mYFixValueText, wxT("%.4f"));
  FinishDialog();
}


/*****
Make sure the user entry is valid
*****/
bool tmwxNodeFixedDialog::Validate()
{
  if (mXFixed->GetValue()) {
    if (!mXFixValueText->ValidateXCoord(mTree, mXFixValue)) 
      return false;
  }
  if (mYFixed->GetValue()) {
    if (!mYFixValueText->ValidateYCoord(mTree, mYFixValue)) 
      return false;
  }
  if (!mXFixed->GetValue() && !mYFixed->GetValue()) {
    tmwxAlertError(
      wxT("You need to check one of the coordinates or cancel the dialog."));
    return false;
  }
  if (mMultiple && mXFixed->GetValue() && mYFixed->GetValue()) {
    tmwxAlertError(
      wxT("You should not fix both coordinates of more than one node to ")
      wxT("the same position."));
    return false;
  }
  return true;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxPathAngleFixedDialog
Utility dialog for setting path fix angle
**********/
class tmwxPathAngleFixedDialog : public tmwxGetUserInputDialog {
public:
  tmwxTextCtrl* mAngleText;
  double mAngle;

  tmwxPathAngleFixedDialog();
  bool Validate();
};


/*****
Constructor
*****/
tmwxPathAngleFixedDialog::tmwxPathAngleFixedDialog()
  : tmwxGetUserInputDialog(wxT("Path Angle Fixed"))
{
  AddTextPair(wxT("Fix angle (degrees): "), mAngleText);
  mAngleText->SetSelection(-1, -1);
  FinishDialog();
  mAngleText->SetFocus();
}


/*****
Make sure the user entry is a valid paper size.
*****/
bool tmwxPathAngleFixedDialog::Validate()
{
  return mAngleText->ValidateAngle(wxT("Fix angle"), mAngle);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxPathAngleQuantDialog
Utility dialog for setting path angle quantization
**********/
class tmwxPathAngleQuantDialog : public tmwxGetUserInputDialog {
public:
  tmwxTextCtrl* mQuantText;
  size_t mQuant;
  tmwxTextCtrl* mQuantOffsetText;
  double mQuantOffset;

  tmwxPathAngleQuantDialog();
  bool Validate();
};


/*****
Constructor
*****/
tmwxPathAngleQuantDialog::tmwxPathAngleQuantDialog()
  : tmwxGetUserInputDialog(wxT("Path Angle Quantization"))
{
  AddTextPair(wxT("Path angle quantization: "), mQuantText);
  AddTextPair(wxT("Quantization offset (degrees): "), mQuantOffsetText, 
    wxT("%.2f"));
  mQuantText->SetSelection(-1, -1);
  mQuantOffsetText->SetValue(0.0);
  FinishDialog();
  mQuantText->SetFocus();
}


/*****
Make sure the user entry is a valid paper size.
*****/
bool tmwxPathAngleQuantDialog::Validate()
{
  if (!mQuantText->ValidateSizeT(wxT("Quantization"), 3, mQuant,
    wxT("Angle quantization must be at least 3.")))
    return false;
  if (!mQuantOffsetText->ValidateAngle(wxT("Offset angle"), mQuantOffset))
    return false;
  return true;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxDoc
TreeMaker document class
**********/

/*****
Enable Conditions->Node Fixed to Symmetry Line
*****/
void tmwxDoc::OnNodeFixedToSymmetryLineUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.not_empty());
}


/*****
Perform Conditions->Node Fixed to Symmetry Line
*****/
void tmwxDoc::OnNodeFixedToSymmetryLine(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  mTree->SetNodesFixedToSymmetryLine(leafNodes);
  SubmitCommand(wxT("Fix Node to Symmetry Line"));
}


/*****
Enable Conditions->Node Fixed to Paper Edge
*****/
void tmwxDoc::OnNodeFixedToPaperEdgeUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.not_empty());
}


/*****
Perform Conditions->Node Fixed to Paper Edge
*****/
void tmwxDoc::OnNodeFixedToPaperEdge(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  mTree->SetNodesFixedToPaperEdge(leafNodes);
  SubmitCommand(wxT("Fix Node to Paper Edge"));
}


/*****
Enable Conditions->Node Fixed to Corner
*****/
void tmwxDoc::OnNodeFixedToCornerUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.not_empty());
}


/*****
Perform Conditions->Node Fixed to Corner
*****/
void tmwxDoc::OnNodeFixedToCorner(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  mTree->SetNodesFixedToPaperCorner(leafNodes);
  SubmitCommand(wxT("Fix Node to Corner"));
}


/*****
Enable Conditions->Node Fixed to Position
*****/
void tmwxDoc::OnNodeFixedToPositionUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.not_empty());
}


/*****
Perform Conditions->Node Fixed to Position
*****/
void tmwxDoc::OnNodeFixedToPosition(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  tmwxNodeFixedDialog dialog(mTree, (leafNodes.size() > 1));
  if (dialog.ShowModal() == wxID_OK) {
    mTree->SetNodesFixedToPosition(leafNodes, dialog.mXFixed->GetValue(), 
      dialog.mXFixValue, dialog.mYFixed->GetValue(), dialog.mYFixValue);
    SubmitCommand(wxT("Fix Node Position"));
  }
}


#ifdef TMDEBUG
/*****
Enable Conditions->Node Fixed to Symmetry Line (v4)
*****/
void tmwxDoc::OnNodeFixedToSymmetryLinev4UpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.not_empty());
}


/*****
Perform Conditions->Node Fixed to Symmetry Line (v4)
*****/
void tmwxDoc::OnNodeFixedToSymmetryLinev4(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  mTree->SetNodesFixedToSymmetryLinev4(leafNodes);
  SubmitCommand(wxT("Fix Node to Symmetry Line (v4)"));
}


/*****
Enable Conditions->Node Fixed to Paper Edge (v4)
*****/
void tmwxDoc::OnNodeFixedToPaperEdgev4UpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.not_empty());
}


/*****
Perform Conditions->Node Fixed to Paper Edge (v4)
*****/
void tmwxDoc::OnNodeFixedToPaperEdgev4(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  mTree->SetNodesFixedToPaperEdgev4(leafNodes);
  SubmitCommand(wxT("Fix Node to Paper Edge (v4)"));
}


/*****
Enable Conditions->Node Fixed to Corner (v4)
*****/
void tmwxDoc::OnNodeFixedToCornerv4UpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.not_empty());
}


/*****
Perform Conditions->Node Fixed to Corner (v4)
*****/
void tmwxDoc::OnNodeFixedToCornerv4(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  mTree->SetNodesFixedToPaperCornerv4(leafNodes);
  SubmitCommand(wxT("Fix Node to Corner (v4)"));
}


/*****
Enable Conditions->Node Fixed to Position (v4)
*****/
void tmwxDoc::OnNodeFixedToPositionv4UpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.not_empty());
}


/*****
Perform Conditions->Node Fixed to Position (v4)
*****/
void tmwxDoc::OnNodeFixedToPositionv4(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  tmwxNodeFixedDialog dialog(mTree, (leafNodes.size() > 1));
  if (dialog.ShowModal() == wxID_OK) {
    mTree->SetNodesFixedToPositionv4(leafNodes, dialog.mXFixed, 
      dialog.mXFixValue, dialog.mYFixed, dialog.mYFixValue);
    SubmitCommand(wxT("Fix Node Position (v4)"));
  }
}
#endif // TMDEBUG


/*****
Enable Conditions->Nodes Paired About Symmetry Line
*****/
void tmwxDoc::OnNodesPairedAboutSymmetryLineUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.size() == 2);
}


/*****
Perform Conditions->Nodes Paired About Symmetry Line
*****/
void tmwxDoc::OnNodesPairedAboutSymmetryLine(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  mTree->GetOrMakeTwoPartCondition<tmConditionNodesPaired, tmNode>(
    leafNodes[0], leafNodes[1]);
  SubmitCommand(wxT("Nodes Paired"));
}


/*****
Enable Conditions->Nodes Collinear
*****/
void tmwxDoc::OnNodesCollinearUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  event.Enable(leafNodes.size() == 3);
}


/*****
Perform Conditions->Nodes Collinear
*****/
void tmwxDoc::OnNodesCollinear(wxCommandEvent&)
{
  tmArray<tmNode*> leafNodes;
  tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
  mTree->GetOrMakeThreePartCondition<tmConditionNodesCollinear, tmNode>(
    leafNodes[0], leafNodes[1], leafNodes[2]);
  SubmitCommand(wxT("Nodes Collinear"));
}


/*****
Enable Conditions->Remove Node Conditions
*****/
void tmwxDoc::OnRemoveNodeConditionsUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->HasNodeConditions());
}


/*****
Perform Conditions->Remove Node Conditions
*****/
void tmwxDoc::OnRemoveNodeConditions(wxCommandEvent&)
{
  mTree->KillNodeConditions();
  SubmitCommand(wxT("Remove Node Conditions"));
}


/*****
Enable Conditions->Edge Length Fixed
*****/
void tmwxDoc::OnEdgeLengthFixedUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmEdge*>& selEdges = mSelection.GetEdges();
  event.Enable(selEdges.not_empty());
}


/*****
Perform Conditions->Edge Length Fixed
*****/
void tmwxDoc::OnEdgeLengthFixed(wxCommandEvent&)
{
  tmArray<tmEdge*>& selEdges = mSelection.GetEdges();
  tmArray<tmConditionEdgeLengthFixed*> clist;
  mTree->GetOrMakeOnePartCondition<tmConditionEdgeLengthFixed, tmEdge>(
    selEdges, clist);
  SubmitCommand(wxT("Fix Edge Length"));
}


/*****
Enable Conditions->Edges Same Strain
*****/
void tmwxDoc::OnEdgesSameStrainUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmEdge*>& selEdges = mSelection.GetEdges();
  event.Enable(selEdges.size() >= 2);
}


/*****
Perform Conditions->Edges Same Strain
*****/
void tmwxDoc::OnEdgesSameStrain(wxCommandEvent&)
{
  tmArray<tmEdge*>& selEdges = mSelection.GetEdges();
  mTree->SetEdgesSameStrain(selEdges);
  SubmitCommand(wxT("Edges Same Strain"));
}


/*****
Enable Conditions->Remove Edge Conditions
*****/
void tmwxDoc::OnRemoveEdgeConditionsUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->HasEdgeConditions());
}


/*****
Perform Conditions->Remove Edge Conditions
*****/
void tmwxDoc::OnRemoveEdgeConditions(wxCommandEvent&)
{
  mTree->KillEdgeConditions();
  SubmitCommand(wxT("Remove Edge Conditions"));
}


/*****
Enable Conditions->Path Active
*****/
void tmwxDoc::OnPathActiveUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  event.Enable(leafPaths.not_empty());
}


/*****
Perform Conditions->Path Active
*****/
void tmwxDoc::OnPathActive(wxCommandEvent&)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  mTree->SetPathsActive(leafPaths);
  SubmitCommand(wxT("Path Active"));
}


/*****
Enable Conditions->Path Angle Fixed
*****/
void tmwxDoc::OnPathAngleFixedUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  event.Enable(leafPaths.not_empty());
}


/*****
Perform Conditions->Path Angle Fixed
*****/
void tmwxDoc::OnPathAngleFixed(wxCommandEvent&)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  tmwxPathAngleFixedDialog dialog;
  if (dialog.ShowModal() == wxID_OK) {
    mTree->SetPathsAngleFixed(leafPaths, dialog.mAngle);
    SubmitCommand(wxT("Path Angle Fixed"));
  }
}


/*****
Enable Conditions->Path Angle Quantized
*****/
void tmwxDoc::OnPathAngleQuantizedUpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  event.Enable(leafPaths.not_empty());
}


/*****
Perform Conditions->Path Angle Quantized
*****/
void tmwxDoc::OnPathAngleQuantized(wxCommandEvent&)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  tmwxPathAngleQuantDialog dialog;
  if (dialog.ShowModal() == wxID_OK) {
    mTree->SetPathsAngleQuant(leafPaths, dialog.mQuant, dialog.mQuantOffset);
    SubmitCommand(wxT("Path Angle Quantized"));
  }
}


#ifdef TMDEBUG
/*****
Enable Conditions->Path Active (v4)
*****/
void tmwxDoc::OnPathActivev4UpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  event.Enable(leafPaths.not_empty());
}


/*****
Perform Conditions->Path Active (v4)
*****/
void tmwxDoc::OnPathActivev4(wxCommandEvent&)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  mTree->SetPathsActivev4(leafPaths);
  SubmitCommand(wxT("Path Active (v4)"));
}


/*****
Enable Conditions->Path Angle Fixed (v4)
*****/
void tmwxDoc::OnPathAngleFixedv4UpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  event.Enable(leafPaths.not_empty());
}


/*****
Perform Conditions->Path Angle Fixed (v4)
*****/
void tmwxDoc::OnPathAngleFixedv4(wxCommandEvent&)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  tmwxPathAngleFixedDialog dialog;
  if (dialog.ShowModal() == wxID_OK) {
    mTree->SetPathsAngleFixedv4(leafPaths, dialog.mAngle);
    SubmitCommand(wxT("Path Angle Fixed (v4)"));
  }
}


/*****
Enable Conditions->Path Angle Quantized (v4)
*****/
void tmwxDoc::OnPathAngleQuantizedv4UpdateUI(wxUpdateUIEvent& event)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  event.Enable(leafPaths.not_empty());
}


/*****
Perform Conditions->Path Angle Quantized (v4)
*****/
void tmwxDoc::OnPathAngleQuantizedv4(wxCommandEvent&)
{
  tmArray<tmPath*> leafPaths;
  GetLeafPathsFromSelection(leafPaths);
  tmwxPathAngleQuantDialog dialog;
  if (dialog.ShowModal() == wxID_OK) {
    mTree->SetPathsAngleQuantv4(leafPaths, dialog.mQuant, dialog.mQuantOffset);
    SubmitCommand(wxT("Path Angle Quantized (v4)"));
  }
}
#endif // TMDEBUG


/*****
Enable Conditions->Remove Path Conditions
*****/
void tmwxDoc::OnRemovePathConditionsUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->HasPathConditions());
}


/*****
Perform Conditions->Remove Path Conditions
*****/
void tmwxDoc::OnRemovePathConditions(wxCommandEvent&)
{
  mTree->KillPathConditions();
  SubmitCommand(wxT("Remove Path Conditions"));
}


/*****
Enable Conditions->Remove All Conditions
*****/
void tmwxDoc::OnRemoveAllConditionsUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->GetConditions().not_empty());
}


/*****
Perform Conditions->Remove All Conditions
*****/
void tmwxDoc::OnRemoveAllConditions(wxCommandEvent&)
{
  mTree->KillConditions();
  SubmitCommand(wxT("Remove All Conditions"));
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Kill a list of conditions. This is invoked by class tmwxConditionListBox from
within a part inspector.
*****/
void tmwxDoc::DoKillConditions(tmArray<tmCondition*>& markedConditions)
{
  mTree->KillSomeConditions(markedConditions);
  SubmitCommand(wxT("Remove Conditions"));
}

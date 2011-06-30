/*******************************************************************************
File:         tmwxPathPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for path inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxPathPanel.h"

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
#include "tmwxConditionListBox.h"

/*****
Constructor
*****/
tmwxPathPanel::tmwxPathPanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mPath(0)
{
  AddStatLine();
  AddStaticText(mMinTreeLength);
  AddStaticText(mActTreeLength);
  AddStaticText(mPathAngle);
  AddStatLine();
  AddConditionListBox(mConditions, wxSize(10, 150));
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mPathOwner);
  AddStatTextGridExtra(mMinPaperLength);
  AddStatTextGridExtra(mActPaperLength);
  AddStatTextGridExtra(mIsLeafPath);
  AddStatTextGridExtra(mIsSubPath);
  AddStatTextGridExtra(mIsFeasiblePath);
  AddStatTextGridExtra(mIsActivePath);
  AddStatTextGridExtra(mIsBorderPath);
  AddStatTextGridExtra(mIsPolygonPath);
  AddStatTextGridExtra(mIsConditionedPath);
  AddStatTextRowExtra(mNodes);
  AddStatTextRowExtra(mEdges);
  AddStatTextGridExtra(mFwdPoly);
  AddStatTextGridExtra(mBkdPoly);
  AddStatTextRowExtra(mOwnedVertices);
  AddStatTextRowExtra(mOwnedCreases);
  AddStatTextGridExtra(mOutsetPath);
  AddStatTextGridExtra(mFrontReduction);
  AddStatTextGridExtra(mBackReduction);
  AddStatTextGridExtra(mMaxOutsetPath);
  AddStatTextGridExtra(mMaxFrontReduction);
  AddStatTextGridExtra(mMaxBackReduction);
  AddStatTextGridExtra(mMinDepth);
  AddStatTextGridExtra(mMinDepthDist);
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel with data from aPath (which can be NULL).
*****/
void tmwxPathPanel::Fill(tmPath* aPath)
{
  mPath = aPath;
  Fill();
}


/*****
Fill the panel with data from mPath (which can be NULL).
*****/
void tmwxPathPanel::Fill()
{
  if (!mPath) return;
  wxString title = wxString::Format(wxT("Path %s (Node %s - Node %s)"),
    tmwxStr(mPath).c_str(), tmwxStr(mPath->GetNodes().front()).c_str(), 
    tmwxStr(mPath->GetNodes().back()).c_str());
  mPanelBox->SetLabel(title);
  mMinTreeLength->SetLabelFormatted(wxT("Min Tree Length: %.4f"), 
    mPath->GetMinTreeLength());
  mActTreeLength->SetLabelFormatted(wxT("Act Tree Length: %.4f"), 
    mPath->GetActTreeLength());
  mPathAngle->SetLabelFormatted(wxT("Path Angle: %.2f"), 
    mPath->GetAngle());
  mConditions->FillListBox((tmPath*)mPath);
#if tmwxINSPECTOR_EXTRA
  mPathOwner->SetLabelFormatted(wxT("Owner: %s"), 
    mPath->GetPathOwner());
  mMinPaperLength->SetLabelFormatted(wxT("Min Paper Length: %.4f"), 
    mPath->GetMinPaperLength());
  mActPaperLength->SetLabelFormatted(wxT("Act Paper Length: %.4f"), 
    mPath->GetActPaperLength());
  mIsLeafPath->SetLabelFormatted(wxT("Leaf Path: %s"), 
    mPath->IsLeafPath());
  mIsSubPath->SetLabelFormatted(wxT("Sub Path: %s"), 
    mPath->IsSubPath());
  mIsFeasiblePath->SetLabelFormatted(wxT("Valid Path: %s"), 
    mPath->IsFeasiblePath());
  mIsActivePath->SetLabelFormatted(wxT("Active Path: %s"), 
    mPath->IsActivePath());
  mIsBorderPath->SetLabelFormatted(wxT("Border Path: %s"), 
    mPath->IsBorderPath());
  mIsPolygonPath->SetLabelFormatted(wxT("Polygon Path: %s"), 
    mPath->IsPolygonPath());
  mIsConditionedPath->SetLabelFormatted(wxT("Conditioned: %s"), 
    mPath->IsConditionedPath());
  mNodes->SetLabelFormatted(wxT("Nodes: %s"), 
    mPath->GetNodes());
  mEdges->SetLabelFormatted(wxT("Edges: %s"), 
    mPath->GetEdges());
  mFwdPoly->SetLabelFormatted(wxT("Fwd Poly: %s"), 
    mPath->GetFwdPoly());
  mBkdPoly->SetLabelFormatted(wxT("Bkd Poly: %s"), 
    mPath->GetBkdPoly());
  mOwnedVertices->SetLabelFormatted(wxT("Owned Vertices: %s"), 
    mPath->GetOwnedVertices());
  mOwnedCreases->SetLabelFormatted(wxT("Owned Creases: %s"), 
    mPath->GetOwnedCreases());
  mOutsetPath->SetLabelFormatted(wxT("Outset Path: %s"), 
    mPath->GetOutsetPath());
  mFrontReduction->SetLabelFormatted(wxT("Front Red'n: %.4f"), 
    mPath->GetFrontReduction());
  mBackReduction->SetLabelFormatted(wxT("Back Red'n: %.4f"), 
    mPath->GetBackReduction());
  tmPath* maxOutsetPath;
  tmFloat maxFrontReduction, maxBackReduction;
  mPath->GetMaxOutsetPath(maxOutsetPath, maxFrontReduction, maxBackReduction);
  mMaxOutsetPath->SetLabelFormatted(wxT("Max Outset Path: %s"), 
    maxOutsetPath);
  mMaxFrontReduction->SetLabelFormatted(wxT("Max Front Red'n: %.4f"), 
    maxFrontReduction);
  mMaxBackReduction->SetLabelFormatted(wxT("Max Back Rred'n: %.4f"), 
    maxBackReduction);
  mMinDepth->SetLabelFormatted(wxT("Min Depth: %.4f"), 
    mPath->GetMinDepth());
  mMinDepthDist->SetLabelFormatted(wxT("Min Depth Dist: %.4f"), 
    mPath->GetMinDepthDist());
#endif
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxPathPanel, wxPanel)
END_EVENT_TABLE()



/*******************************************************************************
File:         tmwxPolyPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for poly inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxPolyPanel.h"

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
tmwxPolyPanel::tmwxPolyPanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mPoly(0)
{
  AddStatLine();
  AddStaticText(mCentroid);
  AddStaticText(mRingNodes);
  AddStaticText(mRingPaths);
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mPolyOwner);
  AddStatTextGridExtra(mIsSubPoly);
  AddStatTextRowExtra(mCrossPaths);
  AddStatTextRowExtra(mInsetNodes);
  AddStatTextRowExtra(mSpokePaths);
  AddStatTextRowExtra(mRidgePath);
  AddStatTextRowExtra(mOwnedNodes);
  AddStatTextRowExtra(mOwnedPaths);
  AddStatTextRowExtra(mOwnedPolys);
  AddStatTextRowExtra(mOwnedCreases);
  AddStatTextRowExtra(mLocalRootVertices);
  AddStatTextRowExtra(mLocalRootCreases);
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aPoly, which can be NULL.
*****/
void tmwxPolyPanel::Fill(tmPoly* aPoly)
{
  mPoly = aPoly;
  Fill();
}


/*****
Fill the panel from mPoly, which can be NULL.
*****/
void tmwxPolyPanel::Fill()
{
  if (!mPoly) return;
  wxString title = wxString::Format(wxT("Poly %s"), tmwxStr(mPoly).c_str());
  mPanelBox->SetLabel(title);
  mCentroid->SetLabelFormatted(wxT("Centroid: (%.4f, %.4f)"), 
    mPoly->GetCentroid());
  mRingNodes->SetLabelFormatted(wxT("Ring Nodes: %s"), 
    mPoly->GetRingNodes());
  mRingPaths->SetLabelFormatted(wxT("Ring Paths: %s"), 
    mPoly->GetRingPaths());
#if tmwxINSPECTOR_EXTRA
  mPolyOwner->SetLabelFormatted(wxT("Owner: %s"), 
    mPoly->GetPolyOwner());
  mIsSubPoly->SetLabelFormatted(wxT("Sub Poly: %s"), 
    mPoly->IsSubPoly());
  mCrossPaths->SetLabelFormatted(wxT("Cross Paths: %s"), 
    mPoly->GetCrossPaths());
  mInsetNodes->SetLabelFormatted(wxT("Inset Nodes: %s"), 
    mPoly->GetInsetNodes());
  mSpokePaths->SetLabelFormatted(wxT("Spoke Paths: %s"), 
    mPoly->GetSpokePaths());
  mRidgePath->SetLabelFormatted(wxT("Ridge Path: %s"), 
    mPoly->GetRidgePath());
  mOwnedNodes->SetLabelFormatted(wxT("Owned Nodes: %s"), 
    mPoly->GetOwnedNodes());
  mOwnedPaths->SetLabelFormatted(wxT("Owned Paths: %s"), 
    mPoly->GetOwnedPaths());
  mOwnedPolys->SetLabelFormatted(wxT("Owned Polys: %s"), 
    mPoly->GetOwnedPolys());
  mOwnedCreases->SetLabelFormatted(wxT("Owned Creases: %s"), 
    mPoly->GetOwnedCreases());
  mLocalRootVertices->SetLabelFormatted(wxT("Local Root Vertices: %s"), 
    mPoly->GetLocalRootVertices());
  mLocalRootCreases->SetLabelFormatted(wxT("Local Root Creases: %s"), 
    mPoly->GetLocalRootCreases());
#endif
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxPolyPanel, wxPanel)
END_EVENT_TABLE()



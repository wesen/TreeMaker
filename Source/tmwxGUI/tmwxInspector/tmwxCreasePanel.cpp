/*******************************************************************************
File:         tmwxCreasePanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for crease inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxCreasePanel.h"

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
tmwxCreasePanel::tmwxCreasePanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mCrease(0)
{
  AddStatLine();
  AddStaticText(mKind);
  AddStaticText(mFold);
  AddStaticText(mCreaseAngle);
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mCreaseOwner);
  AddStatTextRowExtra(mVertices);
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aCrease, which can be NULL.
*****/
void tmwxCreasePanel::Fill(tmCrease* aCrease)
{
  mCrease = aCrease;
  Fill();
}


/*****
Fill the panel from mCrease, which can be NULL.
*****/
void tmwxCreasePanel::Fill()
{
  if (!mCrease) return;
  wxString title = wxString::Format(wxT("Crease %s (Vertex %s - Vertex %s)"),
    tmwxStr(mCrease).c_str(), tmwxStr(mCrease->GetVertices().front()).c_str(), 
    tmwxStr(mCrease->GetVertices().back()).c_str());
  mPanelBox->SetLabel(title);
  mKind->SetLabelFormatted(wxT("Kind: %s"), mCrease->GetKind());
  mFold->SetLabelFormatted(wxT("Fold: %s"), mCrease->GetFold());
  mCreaseAngle->SetLabelFormatted(wxT("Crease Angle: %.2f"), 
    mCrease->GetPositiveAngle());
#if tmwxINSPECTOR_EXTRA
  mCreaseOwner->SetLabelFormatted(wxT("Owner: %s"), 
    mCrease->GetCreaseOwner());
  mVertices->SetLabelFormatted(wxT("Vertices: %s"), 
    (const tmArray<tmVertex*>&)mCrease->GetVertices());
#endif
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxCreasePanel, wxPanel)
END_EVENT_TABLE()



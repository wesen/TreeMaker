/*******************************************************************************
File:         tmwxVertexPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for vertex inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxVertexPanel.h"

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
tmwxVertexPanel::tmwxVertexPanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mVertex(0)
{
  AddStatLine();
  AddStaticText(mLoc);
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mVertexOwner);
  AddStatTextRowExtra(mCreases);
  AddStatTextGridExtra(mIsBorderVertex);
  AddStatTextGridExtra(mElevation);
  AddStatTextGridExtra(mTreeNode);
  AddStatTextGridExtra(mLeftPseudohingeMate);
  AddStatTextGridExtra(mRightPseudohingeMate);
  AddStatTextGridExtra(mDepth);
  AddStatTextGridExtra(mDiscreteDepth);
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aVertex, which can be NULL.
*****/
void tmwxVertexPanel::Fill(tmVertex* aVertex)
{
  mVertex = aVertex;
  Fill();
}


/*****
Fill the panel from mVertex, which can be NULL.
*****/
void tmwxVertexPanel::Fill()
{
  if (!mVertex) return;
  wxString title = wxString::Format(wxT("Vertex %s"), tmwxStr(mVertex).c_str());
  mPanelBox->SetLabel(title);
  mLoc->SetLabelFormatted(wxT("Location: (%.4f,%.4f)"), 
    mVertex->GetLoc());
#if tmwxINSPECTOR_EXTRA
  mVertexOwner->SetLabelFormatted(wxT("Owner: %s"), 
    mVertex->GetVertexOwner());
  mCreases->SetLabelFormatted(wxT("Creases: %s"), 
    mVertex->GetCreases());
  mIsBorderVertex->SetLabelFormatted(wxT("Border Vertex: %s"), 
    mVertex->IsBorderVertex());
  mElevation->SetLabelFormatted(wxT("Elevation: %.4f"), 
    mVertex->GetElevation());
  mTreeNode->SetLabelFormatted(wxT("Tree Node: %s"), 
    mVertex->GetTreeNode());
  mLeftPseudohingeMate->SetLabelFormatted(wxT("Left Pseudohinge Mate: %s"), 
    mVertex->GetLeftPseudohingeMate());
  mRightPseudohingeMate->SetLabelFormatted(wxT("Right Pseudohinge Mate: %s"), 
    mVertex->GetRightPseudohingeMate());
  mDepth->SetLabelFormatted(wxT("Depth: %.4f"), 
    mVertex->GetDepth());
  if (mVertex->GetDiscreteDepth() != size_t(-1))
    mDiscreteDepth->SetLabelFormatted(wxT("Discrete Depth: %s"), 
      mVertex->GetDiscreteDepth());
  else
    mDiscreteDepth->SetLabel(wxT("Discrete Depth: -"));
#endif
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxVertexPanel, wxPanel)
END_EVENT_TABLE()



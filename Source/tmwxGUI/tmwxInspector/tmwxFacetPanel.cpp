/*******************************************************************************
File:         tmwxFacetPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for vertex inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxFacetPanel.h"

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
tmwxFacetPanel::tmwxFacetPanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mFacet(0)
{
  AddStatLine();
  AddStaticText(mColor);
  AddStaticText(mCentroid);
  AddStaticText(mVertices);
  AddStaticText(mCreases);
#if tmwxINSPECTOR_EXTRA
  InitExtra();
  AddStatTextGridExtra(mIsWellFormed);
  AddStatTextGridExtra(mFacetOwner);
  AddStatTextGridExtra(mCorridorEdge);
  AddStatTextRowExtra(mTailFacets);
  AddStatTextRowExtra(mHeadFacets);
  AddStatTextGridExtra(mOrder);
#endif
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aFacet, which can be NULL.
*****/
void tmwxFacetPanel::Fill(tmFacet* aFacet)
{
  mFacet = aFacet;
  Fill();
}


/*****
Fill the panel from mFacet, which can be NULL.
*****/
void tmwxFacetPanel::Fill()
{
  if (!mFacet) return;
  wxString title = wxString::Format(wxT("Facet %s"), tmwxStr(mFacet).c_str());
  mPanelBox->SetLabel(title);
  mColor->SetLabelFormatted(wxT("Color: %s"), 
    mFacet->GetColor());
  mCentroid->SetLabelFormatted(wxT("Centroid: (%.4f, %.4f)"), 
    mFacet->GetCentroid());
  mVertices->SetLabelFormatted(wxT("Vertices: %s"), 
    mFacet->GetVertices());
  mCreases->SetLabelFormatted(wxT("Creases: %s"), 
    mFacet->GetCreases());
#if tmwxINSPECTOR_EXTRA
  mIsWellFormed->SetLabelFormatted(wxT("Well-Formed: %s"), 
    mFacet->IsWellFormed());
  mFacetOwner->SetLabelFormatted(wxT("Owner: %s"), 
    mFacet->GetFacetOwner());
  mCorridorEdge->SetLabelFormatted(wxT("Corridor Edge: %s"), 
    mFacet->GetCorridorEdge());
  mTailFacets->SetLabelFormatted(wxT("Tail Facets: %s"), 
    mFacet->GetTailFacets());
  mHeadFacets->SetLabelFormatted(wxT("Head Facets: %s"), 
    mFacet->GetHeadFacets());
  mOrder->SetLabelFormatted(wxT("Order: %s"), 
    mFacet->GetOrder());
#endif
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxFacetPanel, wxPanel)
END_EVENT_TABLE()



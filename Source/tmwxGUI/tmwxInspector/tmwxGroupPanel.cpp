/*******************************************************************************
File:         tmwxGroupPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for group selection panel
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-20
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxGroupPanel.h"

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
tmwxGroupPanel::tmwxGroupPanel(wxWindow* parent)
  : tmwxInspectorPanel(parent), mCluster(0)
{
  AddStatLine();
  mPanelSizer->Add(0, 5, 0, wxGROW);
  AddStaticText(mNodes);
  AddStaticText(mEdges);
  AddStaticText(mPaths);
  AddStaticText(mPolys);
  AddStaticText(mVertices);
  AddStaticText(mCreases);
  AddStaticText(mFacets);
  AddStaticText(mConditions);
#if tmwxINSPECTOR_EXTRA
#endif // tmwxINSPECTOR_EXTRA
  SetSizerAndFit(mPanelSizer);
}


/*****
Fill the panel from aCluster, which can be NULL.
*****/
void tmwxGroupPanel::Fill(tmCluster* aCluster)
{
  mCluster = aCluster;
  Fill();
}


/*****
Fill the panel from mCluster, which can be NULL.
*****/
void tmwxGroupPanel::Fill()
{
  if (!mCluster) return;
  mPanelBox->SetLabel(wxT("Group Selection"));
  size_t MAX_PARTS = 8;
  int numNodes = int(mCluster->GetNodes().size());
  mNodes->SetLabel(wxString::Format(wxT("%d Nodes: %s"), numNodes,
    tmwxStr(mCluster->GetNodes(), MAX_PARTS).c_str()));
    
  int numEdges = int(mCluster->GetEdges().size());
  mEdges->SetLabel(wxString::Format(wxT("%d Edges: %s"), numEdges,
    tmwxStr(mCluster->GetEdges(), MAX_PARTS).c_str()));

  int numPaths = int(mCluster->GetPaths().size());
  mPaths->SetLabel(wxString::Format(wxT("%d Paths: %s"), numPaths,
    tmwxStr(mCluster->GetPaths(), MAX_PARTS).c_str()));

  int numPolys = int(mCluster->GetPolys().size()); 
  mPolys->SetLabel(wxString::Format(wxT("%d Polys: %s"), numPolys,
    tmwxStr(mCluster->GetPolys(), MAX_PARTS).c_str()));

  int numVertices = int(mCluster->GetVertices().size());
  mVertices->SetLabel(wxString::Format(wxT("%d Vertices: %s"), numVertices,
    tmwxStr(mCluster->GetVertices(), MAX_PARTS).c_str()));

  int numCreases = int(mCluster->GetCreases().size());
  mCreases->SetLabel(wxString::Format(wxT("%d Creases: %s"), numCreases,
    tmwxStr(mCluster->GetCreases(), MAX_PARTS).c_str()));

  int numFacets = int(mCluster->GetFacets().size());
  mFacets->SetLabel(wxString::Format(wxT("%d Facets: %s"), numFacets,
    tmwxStr(mCluster->GetFacets(), MAX_PARTS).c_str()));

  int numConditions = int(mCluster->GetConditions().size());
  mConditions->SetLabel(wxString::Format(wxT("%d Conditions: %s"), 
    numConditions, tmwxStr(mCluster->GetConditions(), MAX_PARTS).c_str()));
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxGroupPanel, wxPanel)
END_EVENT_TABLE()



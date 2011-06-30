/*******************************************************************************
File:         tmwxViewSettingsPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for view settings inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

// Additional wxWidgets classes
#include "wx/statline.h"

// TreeMaker classes
#include "tmwxViewSettingsPanel.h"
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxTextCtrl.h"
#include "tmwxStaticText.h"
#include "tmwxDoc.h"
#include "tmwxApp.h"
#include "tmwxDocManager.h"
#include "tmwxView.h"
#include "tmwxDesignCanvas.h"
#include "tmwxCheckBoxSmall.h"
#include "tmwxRadioBoxSmall.h"
#include "tmwxButtonSmall.h"
#include "tmwxButtonMini.h"

/*****
Constructor
*****/
tmwxViewSettingsPanel::tmwxViewSettingsPanel(wxWindow* parent)
  : tmwxPalettePanel(parent), mViewSettings(0)
{
  // Overall sizer for panel
  wxBoxSizer* panelsizer = new wxBoxSizer(wxVERTICAL);
  
  // Preset buttons for different combinations of settings
  // 1 rows of 6 buttons
  const int BUTTON_WIDTH = 60;
  wxGridSizer* btngridsizer = new wxGridSizer(6);
  panelsizer->Add(btngridsizer, 0, wxGROW);
  btngridsizer->Add(mAllBtn = 
    new tmwxButtonSmall(this, wxT("All"), BUTTON_WIDTH),
    0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 2);
  btngridsizer->Add(mTreeBtn = 
    new tmwxButtonSmall(this, wxT("Tree"), BUTTON_WIDTH),
    0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 2);
  btngridsizer->Add(mDesignBtn = 
    new tmwxButtonSmall(this, wxT("Design"), BUTTON_WIDTH),
    0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 2);
  btngridsizer->Add(mCreasesBtn = 
    new tmwxButtonSmall(this, wxT("Creases"), BUTTON_WIDTH),
    0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 2);
  btngridsizer->Add(mPlanBtn = 
    new tmwxButtonSmall(this, wxT("Plan"), BUTTON_WIDTH),
    0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 2);
  btngridsizer->Add(mNoneBtn = 
    new tmwxButtonSmall(this, wxT("None"), BUTTON_WIDTH),
    0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 2);
  panelsizer->AddSpacer(3);
  
  // four columns of settings
  wxBoxSizer* rowsizer = new wxBoxSizer(wxHORIZONTAL);
  panelsizer->Add(rowsizer, 1, wxGROW);
  wxBoxSizer* colsizer1 = new wxBoxSizer(wxVERTICAL);
  rowsizer->Add(colsizer1, 1, wxGROW);
  wxBoxSizer* colsizer2 = new wxBoxSizer(wxVERTICAL);
  rowsizer->Add(colsizer2, 1, wxGROW);
  wxBoxSizer* colsizer3 = new wxBoxSizer(wxVERTICAL);
  rowsizer->Add(colsizer3, 1, wxGROW);
  wxBoxSizer* colsizer4 = new wxBoxSizer(wxVERTICAL);
  rowsizer->Add(colsizer4, 1, wxGROW);
    
  colsizer1->Add(MakeNodeSettings(), 0, wxGROW | wxALL, 2);
  colsizer1->Add(MakeVertexSettings(), 0, wxGROW | wxALL, 2);
  colsizer1->Add(1, 1, 2);

  colsizer2->Add(MakeEdgeSettings(), 0, wxGROW | wxALL, 2);
  colsizer2->Add(MakeCreaseSettings(), 0, wxGROW | wxALL, 2);
  colsizer2->Add(1, 1, 2);

  colsizer3->Add(MakePathSettings(), 0, wxGROW | wxALL, 2);
  colsizer3->Add(MakeConditionSettings(), 0, wxGROW | wxALL, 2);
  colsizer3->Add(1, 1, 2);

  colsizer4->Add(MakePolySettings(), 0, wxGROW | wxALL, 2);
  colsizer4->Add(MakeFacetSettings(), 0, wxGROW | wxALL, 2);
  colsizer4->Add(MakePaperSettings(), 0, wxGROW | wxALL, 2);
  colsizer4->Add(1, 1, 2);
  
  SetSizerAndFit(panelsizer);
}


/*****
Create the box of paper settings and return its sizer.
*****/
wxSizer* tmwxViewSettingsPanel::MakePaperSettings()
{
  wxStaticBox* paperbox = new wxStaticBox(this, wxID_ANY, wxT("Paper"),
    wxDefaultPosition, wxDefaultSize, TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* papersizer = new wxStaticBoxSizer(paperbox, wxVERTICAL);
  papersizer->Add(mShowSymmetryLines = 
    new tmwxCheckBoxSmall(this, wxT("Symmetry")));
  return papersizer;
}


/*****
Create the box of node settings and return its sizer.
*****/
wxSizer* tmwxViewSettingsPanel::MakeNodeSettings()
{
  wxStaticBox* nodebox = new wxStaticBox(this, wxID_ANY, wxT("Nodes"), 
    wxDefaultPosition, wxDefaultSize, TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* nodesizer = new wxStaticBoxSizer(nodebox, wxVERTICAL);
  AddAllNoneBtns(nodesizer, mNodeAllBtn, mNodeNoneBtn);
  nodesizer->Add(mShowLeafNodes = 
    new tmwxCheckBoxSmall(this, wxT("Leaf Nodes")));
  nodesizer->Add(mShowBranchNodes = 
    new tmwxCheckBoxSmall(this, wxT("Branch Nodes")));
  nodesizer->Add(mShowSubNodes = 
    new tmwxCheckBoxSmall(this, wxT("Sub Nodes")));
  AddStatLine(nodesizer);
  nodesizer->Add(mShowNodeDots = 
    new tmwxCheckBoxSmall(this, wxT("Dots")));
  nodesizer->Add(mShowNodeCircles = 
    new tmwxCheckBoxSmall(this, wxT("Circles")));
  AddStatLine(nodesizer);
  nodesizer->Add(mShowNodeIndices = 
    new tmwxCheckBoxSmall(this, wxT("Indices")));
  nodesizer->Add(mShowNodeCoords = 
    new tmwxCheckBoxSmall(this, wxT("Coordinates")));
  nodesizer->Add(mShowNodeElevation = 
    new tmwxCheckBoxSmall(this, wxT("Elevation")));
  nodesizer->Add(mShowNodeDepth = 
    new tmwxCheckBoxSmall(this, wxT("Depth")));
  nodesizer->Add(mShowNodeLabels = 
    new tmwxCheckBoxSmall(this, wxT("Labels")));
  nodesizer->Add(mShowNodeFlags = 
    new tmwxCheckBoxSmall(this, wxT("Flags")));
  return nodesizer;
}


/*****
Create the box of edge settings and return its sizer.
*****/
wxSizer* tmwxViewSettingsPanel::MakeEdgeSettings()
{
  wxStaticBox* edgebox = new wxStaticBox(this, wxID_ANY, wxT("Edges"),
    wxDefaultPosition, wxDefaultSize, TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* edgesizer = new wxStaticBoxSizer(edgebox, wxVERTICAL);
  AddAllNoneBtns(edgesizer, mEdgeAllBtn, mEdgeNoneBtn);
  edgesizer->Add(mShowAllEdges = 
    new tmwxCheckBoxSmall(this, wxT("All Edges")));
  AddStatLine(edgesizer);
  edgesizer->Add(mShowEdgeDots = 
    new tmwxCheckBoxSmall(this, wxT("Dots")));
  edgesizer->Add(mShowEdgeLines = 
    new tmwxCheckBoxSmall(this, wxT("Lines")));
  AddStatLine(edgesizer);
  edgesizer->Add(mShowEdgeIndices = 
    new tmwxCheckBoxSmall(this, wxT("Indices")));
  edgesizer->Add(mShowEdgeLengths = 
    new tmwxCheckBoxSmall(this, wxT("Lengths")));
  edgesizer->Add(mShowEdgeStrain = 
    new tmwxCheckBoxSmall(this, wxT("Strain")));
  edgesizer->Add(mShowEdgeLabels = 
    new tmwxCheckBoxSmall(this, wxT("Labels")));
  edgesizer->Add(mShowEdgeFlags = 
    new tmwxCheckBoxSmall(this, wxT("Flags")));
  return edgesizer;
}


/*****
Create the box of path settings and return its sizer.
*****/
wxSizer* tmwxViewSettingsPanel::MakePathSettings()
{
  wxStaticBox* pathbox = new wxStaticBox(this, wxID_ANY, wxT("Paths"),
    wxDefaultPosition, wxDefaultSize, TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* pathsizer = new wxStaticBoxSizer(pathbox, wxVERTICAL);
  AddAllNoneBtns(pathsizer, mPathAllBtn, mPathNoneBtn);
  pathsizer->Add(mShowLeafPaths = 
    new tmwxCheckBoxSmall(this, wxT("Leaf Paths")));
  pathsizer->Add(mShowBranchPaths = 
    new tmwxCheckBoxSmall(this, wxT("Branch Paths")));
  pathsizer->Add(mShowSubPaths = 
    new tmwxCheckBoxSmall(this, wxT("Sub Paths")));
  pathsizer->Add(mShowActivePaths = 
    new tmwxCheckBoxSmall(this, wxT("Active Paths")));
  pathsizer->Add(mShowBorderPaths = 
    new tmwxCheckBoxSmall(this, wxT("Border Paths")));
  pathsizer->Add(mShowPolygonPaths = 
    new tmwxCheckBoxSmall(this, wxT("Polygon Paths")));
  pathsizer->Add(mShowInfeasiblePaths = 
    new tmwxCheckBoxSmall(this, wxT("Invalid Paths")));
  AddStatLine(pathsizer);
  pathsizer->Add(mShowPathDots = 
    new tmwxCheckBoxSmall(this, wxT("Dots")));
  pathsizer->Add(mShowPathLines = 
    new tmwxCheckBoxSmall(this, wxT("Lines")));
  AddStatLine(pathsizer);
  pathsizer->Add(mShowPathIndices = 
    new tmwxCheckBoxSmall(this, wxT("Indices")));
  pathsizer->Add(mShowPathLengths = 
    new tmwxCheckBoxSmall(this, wxT("Lengths")));
  pathsizer->Add(mShowPathFlags = 
    new tmwxCheckBoxSmall(this, wxT("Flags")));
  return pathsizer;
}


/*****
Create the box of poly settings and return its sizer.
*****/
wxSizer* tmwxViewSettingsPanel::MakePolySettings()
{
  wxStaticBox* polybox = new wxStaticBox(this, wxID_ANY, wxT("Polygons"),
    wxDefaultPosition, wxDefaultSize, TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* polysizer = new wxStaticBoxSizer(polybox, wxVERTICAL);
  AddAllNoneBtns(polysizer, mPolyAllBtn, mPolyNoneBtn);
  polysizer->Add(mShowMajorPolys = 
    new tmwxCheckBoxSmall(this, wxT("Tree Polys")));
  polysizer->Add(mShowSubPolys = 
    new tmwxCheckBoxSmall(this, wxT("Sub Polys")));
  AddStatLine(polysizer);
  polysizer->Add(mShowPolyDots = 
    new tmwxCheckBoxSmall(this, wxT("Dots")));
  polysizer->Add(mShowPolyLines = 
    new tmwxCheckBoxSmall(this, wxT("Lines")));
  polysizer->Add(mShowPolyFills = 
    new tmwxCheckBoxSmall(this, wxT("Fills")));
  AddStatLine(polysizer);
  polysizer->Add(mShowPolyIndices = 
    new tmwxCheckBoxSmall(this, wxT("Indices")));
  polysizer->Add(mShowPolyFlags = 
    new tmwxCheckBoxSmall(this, wxT("Flags")));
  return polysizer;
}


/*****
Create the box of vertex settings and return its sizer.
*****/
wxSizer* tmwxViewSettingsPanel::MakeVertexSettings()
{
  wxStaticBox* vertexbox = new wxStaticBox(this, wxID_ANY, wxT("Vertices"),
    wxDefaultPosition, wxDefaultSize, TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* vertexsizer = new wxStaticBoxSizer(vertexbox, wxVERTICAL);
  AddAllNoneBtns(vertexsizer, mVertexAllBtn, mVertexNoneBtn);
  vertexsizer->Add(mShowMajorVertices = 
    new tmwxCheckBoxSmall(this, wxT("Major Vertices")));
  vertexsizer->Add(mShowMinorVertices = 
    new tmwxCheckBoxSmall(this, wxT("Minor Vertices")));
  AddStatLine(vertexsizer);
  vertexsizer->Add(mShowVertexDots = 
    new tmwxCheckBoxSmall(this, wxT("Dots")));
  AddStatLine(vertexsizer);
  vertexsizer->Add(mShowVertexIndices = 
    new tmwxCheckBoxSmall(this, wxT("Indices")));
  vertexsizer->Add(mShowVertexCoords = 
    new tmwxCheckBoxSmall(this, wxT("Coordinates")));
  vertexsizer->Add(mShowVertexElevation = 
    new tmwxCheckBoxSmall(this, wxT("Elevation")));
  vertexsizer->Add(mShowVertexDepth = 
    new tmwxCheckBoxSmall(this, wxT("Depth")));
  vertexsizer->Add(mShowVertexDiscreteDepth = 
    new tmwxCheckBoxSmall(this, wxT("Discrete Depth")));
  vertexsizer->Add(mShowVertexTreeNode = 
    new tmwxCheckBoxSmall(this, wxT("Tree Nodes")));
  vertexsizer->Add(mShowVertexFlags = 
    new tmwxCheckBoxSmall(this, wxT("Flags")));
  return vertexsizer;
}


/*****
Create the box of crease settings and return its sizer.
*****/
wxSizer* tmwxViewSettingsPanel::MakeCreaseSettings()
{
  wxStaticBox* creasebox = new wxStaticBox(this, wxID_ANY, wxT("Creases"),
    wxDefaultPosition, wxDefaultSize, TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* creasesizer = new wxStaticBoxSizer(creasebox, wxVERTICAL);
  AddAllNoneBtns(creasesizer, mCreaseAllBtn, mCreaseNoneBtn);
  creasesizer->Add(mShowMajorCreases = 
    new tmwxCheckBoxSmall(this, wxT("Major Creases")));
  creasesizer->Add(mShowMinorCreases = 
    new tmwxCheckBoxSmall(this, wxT("Minor Creases")));
  AddStatLine(creasesizer);
  creasesizer->Add(mShowCreaseDots = 
    new tmwxCheckBoxSmall(this, wxT("Dots")));
  creasesizer->Add(mShowCreaseLines = 
    new tmwxCheckBoxSmall(this, wxT("Lines")));
  AddStatLine(creasesizer);
  wxString choices[2] = {wxT("AGRH Colors"), wxT("MVF Colors")};
  creasesizer->Add(mShowCreaseFolds =
   new tmwxRadioBoxSmall(this, wxT("Coloring"), 2, choices, 1, wxRA_SPECIFY_COLS));
  AddStatLine(creasesizer);
  creasesizer->Add(mShowCreaseIndices = 
    new tmwxCheckBoxSmall(this, wxT("Indices")));
  creasesizer->Add(mShowCreaseFlags = 
    new tmwxCheckBoxSmall(this, wxT("Flags")));
  return creasesizer;
}


/*****
Create the box of facet settings and return its sizer.
*****/
wxSizer* tmwxViewSettingsPanel::MakeFacetSettings()
{
  wxStaticBox* facetbox = new wxStaticBox(this, wxID_ANY, wxT("Facets"),
    wxDefaultPosition, wxDefaultSize, TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* facetsizer = new wxStaticBoxSizer(facetbox, wxVERTICAL);
  AddAllNoneBtns(facetsizer, mFacetAllBtn, mFacetNoneBtn);
  facetsizer->Add(mShowAllFacets = 
    new tmwxCheckBoxSmall(this, wxT("All Facets")));
  AddStatLine(facetsizer);
  facetsizer->Add(mShowFacetDots = 
    new tmwxCheckBoxSmall(this, wxT("Dots")));
  facetsizer->Add(mShowFacetArrows = 
    new tmwxCheckBoxSmall(this, wxT("Arrows")));
  facetsizer->Add(mShowFacetFills = 
    new tmwxCheckBoxSmall(this, wxT("Fills")));
  AddStatLine(facetsizer);
  facetsizer->Add(mShowFacetIndices = 
    new tmwxCheckBoxSmall(this, wxT("Indices")));
  facetsizer->Add(mShowFacetOrder = 
    new tmwxCheckBoxSmall(this, wxT("Order")));
  facetsizer->Add(mShowFacetCorridorEdge = 
    new tmwxCheckBoxSmall(this, wxT("Corridor Edge")));
  facetsizer->Add(mShowFacetFlags = 
    new tmwxCheckBoxSmall(this, wxT("Flags")));
  return facetsizer;
}


/*****
Create the box of condition settings and return its sizer.
*****/
wxSizer* tmwxViewSettingsPanel::MakeConditionSettings()
{
  wxStaticBox* condbox = new wxStaticBox(this, wxID_ANY, wxT("Conditions"),
    wxDefaultPosition, wxDefaultSize, TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* condsizer = new wxStaticBoxSizer(condbox, wxVERTICAL);
  AddAllNoneBtns(condsizer, mConditionAllBtn, mConditionNoneBtn);
  condsizer->Add(mShowNodeConditions = 
    new tmwxCheckBoxSmall(this, wxT("Node Conditions")));
  condsizer->Add(mShowEdgeConditions = 
    new tmwxCheckBoxSmall(this, wxT("Edge Conditions")));
  condsizer->Add(mShowPathConditions = 
    new tmwxCheckBoxSmall(this, wxT("Path Conditions")));
  AddStatLine(condsizer);
  condsizer->Add(mShowConditionDots = 
    new tmwxCheckBoxSmall(this, wxT("Dots")));
  condsizer->Add(mShowConditionLines = 
    new tmwxCheckBoxSmall(this, wxT("Lines")));
  AddStatLine(condsizer);
  condsizer->Add(mShowConditionIndices = 
    new tmwxCheckBoxSmall(this, wxT("Indices")));
  condsizer->Add(mShowConditionTypes = 
    new tmwxCheckBoxSmall(this, wxT("Types")));
  return condsizer;
}


/*****
Add a static line to the given sizer.
*****/
void tmwxViewSettingsPanel::AddStatLine(wxBoxSizer* colSizer)
{
  colSizer->Add(new wxStaticLine(this, wxID_ANY), 
    0, wxEXPAND | wxTOP|wxBOTTOM, 2);
}


/*****
Auxiliary function called in constructor. Adds buttons for "All" and "None" to
the indicated sizer and sets the two reference variables to point to the
buttons.
*****/
void tmwxViewSettingsPanel::AddAllNoneBtns(wxBoxSizer* colSizer, 
  wxButton*& allBtn, wxButton*& noneBtn)
{
  const int BUTTON_WIDTH = 48;
  wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
  colSizer->Add(btnSizer, 0, wxTOP|wxBOTTOM, 2);
  btnSizer->Add(2, 0, 1);
  btnSizer->Add(allBtn = new tmwxButtonMini(this, wxT("All"), BUTTON_WIDTH), 
    0);
  btnSizer->Add(10, 0, 0);
  btnSizer->Add(noneBtn = new tmwxButtonMini(this, wxT("None"), BUTTON_WIDTH),
    0);
  btnSizer->Add(2, 0, 1);
} 



/*****
Set the values of the controls from aViewSettings, which can be NULL.
*****/
void tmwxViewSettingsPanel::Fill(tmwxViewSettings* aViewSettings)
{
  mViewSettings = aViewSettings;  
  Fill();
}


/*****
Set the values of the controls from mViewSettings, which can be NULL.
*****/
void tmwxViewSettingsPanel::Fill()
{
  if (!mViewSettings) return;
  mShowSymmetryLines->SetValue(mViewSettings->mShowSymmetryLines);

  mShowLeafNodes->SetValue(mViewSettings->mShowLeafNodes);
  mShowBranchNodes->SetValue(mViewSettings->mShowBranchNodes);
  mShowSubNodes->SetValue(mViewSettings->mShowSubNodes);
  mShowNodeDots->SetValue(mViewSettings->mShowNodeDots);
  mShowNodeCircles->SetValue(mViewSettings->mShowNodeCircles);
  mShowNodeIndices->SetValue(mViewSettings->mShowNodeIndices);
  mShowNodeCoords->SetValue(mViewSettings->mShowNodeCoords);
  mShowNodeElevation->SetValue(mViewSettings->mShowNodeElevation);
  mShowNodeDepth->SetValue(mViewSettings->mShowNodeDepth);
  mShowNodeLabels->SetValue(mViewSettings->mShowNodeLabels);
  mShowNodeFlags->SetValue(mViewSettings->mShowNodeFlags);
    
  mShowAllEdges->SetValue(mViewSettings->mShowAllEdges);
  mShowEdgeDots->SetValue(mViewSettings->mShowEdgeDots);
  mShowEdgeLines->SetValue(mViewSettings->mShowEdgeLines);
  mShowEdgeIndices->SetValue(mViewSettings->mShowEdgeIndices);
  mShowEdgeLengths->SetValue(mViewSettings->mShowEdgeLengths);
  mShowEdgeStrain->SetValue(mViewSettings->mShowEdgeStrain);
  mShowEdgeLabels->SetValue(mViewSettings->mShowEdgeLabels);
  mShowEdgeFlags->SetValue(mViewSettings->mShowEdgeFlags);
    
  mShowLeafPaths->SetValue(mViewSettings->mShowLeafPaths);
  mShowBranchPaths->SetValue(mViewSettings->mShowBranchPaths);
  mShowSubPaths->SetValue(mViewSettings->mShowSubPaths);
  mShowActivePaths->SetValue(mViewSettings->mShowActivePaths);
  mShowBorderPaths->SetValue(mViewSettings->mShowBorderPaths);
  mShowPolygonPaths->SetValue(mViewSettings->mShowPolygonPaths);
  mShowInfeasiblePaths->SetValue(mViewSettings->mShowInfeasiblePaths);
  mShowPathDots->SetValue(mViewSettings->mShowPathDots);
  mShowPathLines->SetValue(mViewSettings->mShowPathLines);
  mShowPathIndices->SetValue(mViewSettings->mShowPathIndices);
  mShowPathLengths->SetValue(mViewSettings->mShowPathLengths);
  mShowPathFlags->SetValue(mViewSettings->mShowPathFlags);
    
  mShowMajorPolys->SetValue(mViewSettings->mShowMajorPolys);
  mShowSubPolys->SetValue(mViewSettings->mShowSubPolys);
  mShowPolyDots->SetValue(mViewSettings->mShowPolyDots);
  mShowPolyLines->SetValue(mViewSettings->mShowPolyLines);
  mShowPolyFills->SetValue(mViewSettings->mShowPolyFills);
  mShowPolyIndices->SetValue(mViewSettings->mShowPolyIndices);
  mShowPolyFlags->SetValue(mViewSettings->mShowPolyFlags);
  
  mShowMajorVertices->SetValue(mViewSettings->mShowMajorVertices);
  mShowMinorVertices->SetValue(mViewSettings->mShowMinorVertices);
  mShowVertexDots->SetValue(mViewSettings->mShowVertexDots);
  mShowVertexIndices->SetValue(mViewSettings->mShowVertexIndices);
  mShowVertexCoords->SetValue(mViewSettings->mShowVertexCoords);
  mShowVertexElevation->SetValue(mViewSettings->mShowVertexElevation);
  mShowVertexDepth->SetValue(mViewSettings->mShowVertexDepth);
  mShowVertexDiscreteDepth->SetValue(mViewSettings->mShowVertexDiscreteDepth);
  mShowVertexTreeNode->SetValue(mViewSettings->mShowVertexTreeNode);
  mShowVertexFlags->SetValue(mViewSettings->mShowVertexFlags);
    
  mShowMajorCreases->SetValue(mViewSettings->mShowMajorCreases);
  mShowMinorCreases->SetValue(mViewSettings->mShowMinorCreases);
  mShowCreaseDots->SetValue(mViewSettings->mShowCreaseDots);
  mShowCreaseLines->SetValue(mViewSettings->mShowCreaseLines);
  mShowCreaseFolds->SetSelection(mViewSettings->mShowCreaseFolds);
  mShowCreaseIndices->SetValue(mViewSettings->mShowCreaseIndices);
  mShowCreaseFlags->SetValue(mViewSettings->mShowCreaseFlags);
  
  mShowAllFacets->SetValue(mViewSettings->mShowAllFacets);
  mShowFacetDots->SetValue(mViewSettings->mShowFacetDots);
  mShowFacetArrows->SetValue(mViewSettings->mShowFacetArrows);
  mShowFacetFills->SetValue(mViewSettings->mShowFacetFills);
  mShowFacetIndices->SetValue(mViewSettings->mShowFacetIndices);
  mShowFacetOrder->SetValue(mViewSettings->mShowFacetOrder);
  mShowFacetCorridorEdge->SetValue(mViewSettings->mShowFacetCorridorEdge);
  mShowFacetFlags->SetValue(mViewSettings->mShowFacetFlags);
  
  mShowNodeConditions->SetValue(mViewSettings->mShowNodeConditions);
  mShowEdgeConditions->SetValue(mViewSettings->mShowEdgeConditions);
  mShowPathConditions->SetValue(mViewSettings->mShowPathConditions);
  mShowConditionDots->SetValue(mViewSettings->mShowConditionDots);
  mShowConditionLines->SetValue(mViewSettings->mShowConditionLines);
  mShowConditionIndices->SetValue(mViewSettings->mShowConditionIndices);
  mShowConditionTypes->SetValue(mViewSettings->mShowConditionTypes);
}


/*****
Write the settings record from the control values
*****/
void tmwxViewSettingsPanel::ReadPanel()
{
  TMASSERT(mViewSettings);
  mViewSettings->mShowSymmetryLines = mShowSymmetryLines->GetValue();

  mViewSettings->mShowLeafNodes = mShowLeafNodes->GetValue();
  mViewSettings->mShowBranchNodes = mShowBranchNodes->GetValue();
  mViewSettings->mShowSubNodes = mShowSubNodes->GetValue();
  mViewSettings->mShowNodeDots = mShowNodeDots->GetValue();
  mViewSettings->mShowNodeCircles = mShowNodeCircles->GetValue();   
  mViewSettings->mShowNodeIndices = mShowNodeIndices->GetValue();
  mViewSettings->mShowNodeCoords = mShowNodeCoords->GetValue();
  mViewSettings->mShowNodeElevation = mShowNodeElevation->GetValue();
  mViewSettings->mShowNodeDepth = mShowNodeDepth->GetValue();
  mViewSettings->mShowNodeLabels = mShowNodeLabels->GetValue();
  mViewSettings->mShowNodeFlags = mShowNodeFlags->GetValue();
    
  mViewSettings->mShowAllEdges = mShowAllEdges->GetValue();   
  mViewSettings->mShowEdgeDots = mShowEdgeDots->GetValue();
  mViewSettings->mShowEdgeLines = mShowEdgeLines->GetValue();
  mViewSettings->mShowEdgeIndices = mShowEdgeIndices->GetValue();
  mViewSettings->mShowEdgeLengths = mShowEdgeLengths->GetValue();
  mViewSettings->mShowEdgeStrain = mShowEdgeStrain->GetValue();
  mViewSettings->mShowEdgeLabels = mShowEdgeLabels->GetValue();
  mViewSettings->mShowEdgeFlags = mShowEdgeFlags->GetValue();
    
  mViewSettings->mShowLeafPaths = mShowLeafPaths->GetValue();
  mViewSettings->mShowBranchPaths = mShowBranchPaths->GetValue();
  mViewSettings->mShowSubPaths = mShowSubPaths->GetValue();
  mViewSettings->mShowActivePaths = mShowActivePaths->GetValue();
  mViewSettings->mShowBorderPaths = mShowBorderPaths->GetValue();
  mViewSettings->mShowPolygonPaths = mShowPolygonPaths->GetValue();
  mViewSettings->mShowInfeasiblePaths = mShowInfeasiblePaths->GetValue();
  mViewSettings->mShowPathDots = mShowPathDots->GetValue();
  mViewSettings->mShowPathLines = mShowPathLines->GetValue();
  mViewSettings->mShowPathIndices = mShowPathIndices->GetValue();
  mViewSettings->mShowPathLengths = mShowPathLengths->GetValue();
  mViewSettings->mShowPathFlags = mShowPathFlags->GetValue();
    
  mViewSettings->mShowMajorPolys = mShowMajorPolys->GetValue();
  mViewSettings->mShowSubPolys = mShowSubPolys->GetValue();
  mViewSettings->mShowPolyDots = mShowPolyDots->GetValue();
  mViewSettings->mShowPolyLines = mShowPolyLines->GetValue();
  mViewSettings->mShowPolyFills = mShowPolyFills->GetValue();
  mViewSettings->mShowPolyIndices = mShowPolyIndices->GetValue();
  mViewSettings->mShowPolyFlags = mShowPolyFlags->GetValue();
  
  mViewSettings->mShowMajorVertices = mShowMajorVertices->GetValue();
  mViewSettings->mShowMinorVertices = mShowMinorVertices->GetValue();
  mViewSettings->mShowVertexDots = mShowVertexDots->GetValue();
  mViewSettings->mShowVertexIndices = mShowVertexIndices->GetValue();
  mViewSettings->mShowVertexCoords = mShowVertexCoords->GetValue();
  mViewSettings->mShowVertexElevation = mShowVertexElevation->GetValue();
  mViewSettings->mShowVertexDepth = mShowVertexDepth->GetValue();
  mViewSettings->mShowVertexDiscreteDepth = mShowVertexDiscreteDepth->GetValue();
  mViewSettings->mShowVertexTreeNode = mShowVertexTreeNode->GetValue();
  mViewSettings->mShowVertexFlags = mShowVertexFlags->GetValue();
    
  mViewSettings->mShowMajorCreases = mShowMajorCreases->GetValue();
  mViewSettings->mShowMinorCreases = mShowMinorCreases->GetValue();
  mViewSettings->mShowCreaseDots = mShowCreaseDots->GetValue();   
  mViewSettings->mShowCreaseLines = mShowCreaseLines->GetValue();   
  mViewSettings->mShowCreaseFolds = mShowCreaseFolds->GetSelection();   
  mViewSettings->mShowCreaseIndices = mShowCreaseIndices->GetValue();   
  mViewSettings->mShowCreaseFlags = mShowCreaseFlags->GetValue();   
    
  mViewSettings->mShowAllFacets = mShowAllFacets->GetValue();
  mViewSettings->mShowFacetDots = mShowFacetDots->GetValue();   
  mViewSettings->mShowFacetArrows = mShowFacetArrows->GetValue();   
  mViewSettings->mShowFacetFills = mShowFacetFills->GetValue();   
  mViewSettings->mShowFacetIndices = mShowFacetIndices->GetValue();   
  mViewSettings->mShowFacetOrder = mShowFacetOrder->GetValue();   
  mViewSettings->mShowFacetCorridorEdge = mShowFacetCorridorEdge->GetValue();   
  mViewSettings->mShowFacetFlags = mShowFacetFlags->GetValue();   
    
  mViewSettings->mShowNodeConditions = mShowNodeConditions->GetValue();
  mViewSettings->mShowEdgeConditions = mShowEdgeConditions->GetValue();
  mViewSettings->mShowPathConditions = mShowPathConditions->GetValue();
  mViewSettings->mShowConditionDots = mShowConditionDots->GetValue();
  mViewSettings->mShowConditionLines = mShowConditionLines->GetValue();
  mViewSettings->mShowConditionIndices = mShowConditionIndices->GetValue();
  mViewSettings->mShowConditionTypes = mShowConditionTypes->GetValue();   
}


/*****
Any checkbox causes a re-read of the entire panel.
*****/
void tmwxViewSettingsPanel::OnCheckBox(wxCommandEvent&)
{
  ReadPanel();
  tmwxDoc* theDoc = gDocManager->GetCurrentDocumentLocal();
  if (theDoc) theDoc->UpdateAllViews();
}


/*****
Buttons create preset combinations of settings.
*****/
void tmwxViewSettingsPanel::OnButton(wxCommandEvent& event)
{
  TMASSERT(mViewSettings);
  
  // Preset buttons
  wxObject* theBtn = event.GetEventObject();
  if (theBtn == mDesignBtn)
    *mViewSettings = tmwxViewSettings::sDesignView;
  else if (theBtn == mCreasesBtn)
    *mViewSettings = tmwxViewSettings::sCreasesView;
  else if (theBtn == mNoneBtn)
    *mViewSettings = tmwxViewSettings::sNoneView;
  else if (theBtn == mTreeBtn)
    *mViewSettings = tmwxViewSettings::sTreeView;
  else if (theBtn == mPlanBtn)
    *mViewSettings = tmwxViewSettings::sPlanView;
  else if (theBtn == mAllBtn)
    *mViewSettings = tmwxViewSettings::sAllView;
    
  // Node All/None buttons
  else if (theBtn == mNodeAllBtn) {
    mViewSettings->mShowLeafNodes = true;
    mViewSettings->mShowBranchNodes = true;
    mViewSettings->mShowSubNodes = true;
    mViewSettings->mShowNodeDots = true;
    mViewSettings->mShowNodeCircles = true;
    mViewSettings->mShowNodeIndices = true;
    mViewSettings->mShowNodeCoords = true;
    mViewSettings->mShowNodeElevation = true;
    mViewSettings->mShowNodeDepth = true;
    mViewSettings->mShowNodeLabels = true;
    mViewSettings->mShowNodeFlags = true;
  }
  else if (theBtn == mNodeNoneBtn) {
    mViewSettings->mShowLeafNodes = false;
    mViewSettings->mShowBranchNodes = false;
    mViewSettings->mShowSubNodes = false;
    mViewSettings->mShowNodeDots = false;
    mViewSettings->mShowNodeCircles = false;
    mViewSettings->mShowNodeIndices = false;
    mViewSettings->mShowNodeCoords = false;
    mViewSettings->mShowNodeElevation = false;
    mViewSettings->mShowNodeDepth = false;
    mViewSettings->mShowNodeLabels = false;
    mViewSettings->mShowNodeFlags = false;
  }

  // Edge All/None buttons
  else if (theBtn == mEdgeAllBtn) {
    mViewSettings->mShowAllEdges = true;
    mViewSettings->mShowEdgeDots = true;
    mViewSettings->mShowEdgeLines = true;
    mViewSettings->mShowEdgeIndices = true;
    mViewSettings->mShowEdgeLengths = true;
    mViewSettings->mShowEdgeStrain = true;
    mViewSettings->mShowEdgeLabels = true;
    mViewSettings->mShowEdgeFlags = true;
  }
  else if (theBtn == mEdgeNoneBtn) {
    mViewSettings->mShowAllEdges = false;
    mViewSettings->mShowEdgeDots = false;
    mViewSettings->mShowEdgeLines = false;
    mViewSettings->mShowEdgeIndices = false;
    mViewSettings->mShowEdgeLengths = false;
    mViewSettings->mShowEdgeStrain = false;
    mViewSettings->mShowEdgeLabels = false;
    mViewSettings->mShowEdgeFlags = false;
  }

  // Path All/None buttons
  else if (theBtn == mPathAllBtn) {
    mViewSettings->mShowLeafPaths = true;
    mViewSettings->mShowBranchPaths = true;
    mViewSettings->mShowSubPaths = true;
    mViewSettings->mShowActivePaths = true;
    mViewSettings->mShowBorderPaths = true;
    mViewSettings->mShowPolygonPaths = true;
    mViewSettings->mShowInfeasiblePaths = true;
    mViewSettings->mShowPathDots = true;
    mViewSettings->mShowPathLines = true;
    mViewSettings->mShowPathIndices = true;
    mViewSettings->mShowPathLengths = true;
    mViewSettings->mShowPathFlags = true;
  }
  else if (theBtn == mPathNoneBtn) {
    mViewSettings->mShowLeafPaths = false;
    mViewSettings->mShowBranchPaths = false;
    mViewSettings->mShowSubPaths = false;
    mViewSettings->mShowActivePaths = false;
    mViewSettings->mShowBorderPaths = false;
    mViewSettings->mShowPolygonPaths = false;
    mViewSettings->mShowPolygonPaths = false;
    mViewSettings->mShowInfeasiblePaths = false;
    mViewSettings->mShowPathDots = false;
    mViewSettings->mShowPathLines = false;
    mViewSettings->mShowPathIndices = false;
    mViewSettings->mShowPathLengths = false;
    mViewSettings->mShowPathFlags = false;
  }

  // Poly All/None buttons
  else if (theBtn == mPolyAllBtn) {
    mViewSettings->mShowMajorPolys = true;
    mViewSettings->mShowSubPolys = true;
    mViewSettings->mShowPolyDots = true;
    mViewSettings->mShowPolyLines = true;
    mViewSettings->mShowPolyFills = true;
    mViewSettings->mShowPolyIndices = true;
    mViewSettings->mShowPolyFlags = true;
  }
  else if (theBtn == mPolyNoneBtn) {
    mViewSettings->mShowMajorPolys = false;
    mViewSettings->mShowSubPolys = false;
    mViewSettings->mShowPolyDots = false;
    mViewSettings->mShowPolyLines = false;
    mViewSettings->mShowPolyFills = false;
    mViewSettings->mShowPolyIndices = false;
    mViewSettings->mShowPolyFlags = false;
  }

  // Vertex All/None buttons
  else if (theBtn == mVertexAllBtn) {
    mViewSettings->mShowMajorVertices = true;
    mViewSettings->mShowMinorVertices = true;
    mViewSettings->mShowVertexDots = true;
    mViewSettings->mShowVertexIndices = true;
    mViewSettings->mShowVertexCoords = true;
    mViewSettings->mShowVertexElevation = true;
    mViewSettings->mShowVertexDepth = true;
    mViewSettings->mShowVertexDiscreteDepth = true;
    mViewSettings->mShowVertexTreeNode = true;
    mViewSettings->mShowVertexFlags = true;
  }
  else if (theBtn == mVertexNoneBtn) {
    mViewSettings->mShowMajorVertices = false;
    mViewSettings->mShowMinorVertices = false;
    mViewSettings->mShowVertexDots = false;
    mViewSettings->mShowVertexIndices = false;
    mViewSettings->mShowVertexCoords = false;
    mViewSettings->mShowVertexElevation = false;
    mViewSettings->mShowVertexDepth = false;
    mViewSettings->mShowVertexDiscreteDepth = false;
    mViewSettings->mShowVertexTreeNode = false;
    mViewSettings->mShowVertexFlags = false;
  }

  // Crease All/None buttons
  else if (theBtn == mCreaseAllBtn) {
    mViewSettings->mShowMajorCreases = true;
    mViewSettings->mShowMinorCreases = true;
    mViewSettings->mShowCreaseDots = true;
    mViewSettings->mShowCreaseLines = true;
    mViewSettings->mShowCreaseFolds = 0;
    mViewSettings->mShowCreaseIndices = true;
    mViewSettings->mShowCreaseFlags = true;
  }
  else if (theBtn == mCreaseNoneBtn) {
    mViewSettings->mShowMajorCreases = false;
    mViewSettings->mShowMinorCreases = false;
    mViewSettings->mShowCreaseDots = false;
    mViewSettings->mShowCreaseLines = false;
    mViewSettings->mShowCreaseFolds = 0;
    mViewSettings->mShowCreaseIndices = false;
    mViewSettings->mShowCreaseFlags = false;
  }

  // Facet All/None buttons
  else if (theBtn == mFacetAllBtn) {
    mViewSettings->mShowAllFacets = true;
    mViewSettings->mShowFacetDots = true;
    mViewSettings->mShowFacetArrows = true;
    mViewSettings->mShowFacetFills = true;
    mViewSettings->mShowFacetIndices = true;
    mViewSettings->mShowFacetOrder = true;
    mViewSettings->mShowFacetCorridorEdge = true;
    mViewSettings->mShowFacetFlags = true;
  }
  else if (theBtn == mFacetNoneBtn) {
    mViewSettings->mShowAllFacets = false;
    mViewSettings->mShowFacetDots = false;
    mViewSettings->mShowFacetArrows = false;
    mViewSettings->mShowFacetFills = false;
    mViewSettings->mShowFacetIndices = false;
    mViewSettings->mShowFacetOrder = false;
    mViewSettings->mShowFacetCorridorEdge = false;
    mViewSettings->mShowFacetFlags = false;
  }

  // Condition All/None buttons
  else if (theBtn == mConditionAllBtn) {
    mViewSettings->mShowNodeConditions = true;
    mViewSettings->mShowEdgeConditions = true;
    mViewSettings->mShowPathConditions = true;
    mViewSettings->mShowConditionDots = true;
    mViewSettings->mShowConditionLines = true;
    mViewSettings->mShowConditionIndices = true;
    mViewSettings->mShowConditionTypes = true;
  }
  else if (theBtn == mConditionNoneBtn) {
    mViewSettings->mShowNodeConditions = false;
    mViewSettings->mShowEdgeConditions = false;
    mViewSettings->mShowPathConditions = false;
    mViewSettings->mShowConditionDots = false;
    mViewSettings->mShowConditionLines = false;
    mViewSettings->mShowConditionIndices = false;
    mViewSettings->mShowConditionTypes = false;
  }

  else TMFAIL("unknown button");
  
  Fill(mViewSettings);  // make controls match settings
  
  // Refresh the view, if there is a document
  tmwxDoc* theDoc = gDocManager->GetCurrentDocumentLocal();
  if (theDoc) theDoc->UpdateAllViews();
}


/*****
Radio group for changing crease display style
*****/
void tmwxViewSettingsPanel::OnRadioBox(wxCommandEvent&)
{
  TMASSERT(mViewSettings);

  // There's only one radio group, so this is the only possibility. if we ever
  // introduce more groups, then we'll have to implement detection of which was
  // hit.
  mViewSettings->mShowCreaseFolds = mShowCreaseFolds->GetSelection();

  // Refresh the view, if there is a document
  tmwxDoc* theDoc = gDocManager->GetCurrentDocumentLocal();
  if (theDoc) theDoc->UpdateAllViews();
}


/*****
Event table
*****/

BEGIN_EVENT_TABLE(tmwxViewSettingsPanel, wxPanel)
  EVT_BUTTON(wxID_ANY, tmwxViewSettingsPanel::OnButton)
  EVT_CHECKBOX(wxID_ANY, tmwxViewSettingsPanel::OnCheckBox)
  EVT_RADIOBOX(wxID_ANY, tmwxViewSettingsPanel::OnRadioBox)
END_EVENT_TABLE()

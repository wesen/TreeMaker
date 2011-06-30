/*******************************************************************************
File:         tmwxFacetPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for facet inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-28
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXFACETPANEL_H_
#define _TMWXFACETPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmFacet;
class tmwxStaticText;

/**********
class tmwxFacetPanel
Displays information about a facet
**********/

class tmwxFacetPanel : public tmwxInspectorPanel {
private:
  tmFacet* mFacet;
  tmwxStaticText* mColor;
  tmwxStaticText* mCentroid;
  tmwxStaticText* mVertices;
  tmwxStaticText* mCreases;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mIsWellFormed;
  tmwxStaticText* mFacetOwner;
  tmwxStaticText* mCorridorEdge;
  tmwxStaticText* mTailFacets;
  tmwxStaticText* mHeadFacets;
  tmwxStaticText* mOrder;
#endif
public:
  tmwxFacetPanel(wxWindow* parent);
  void Fill(tmFacet* aFacet);
  void Fill();
private:
  DECLARE_EVENT_TABLE()
};

#endif // _TMWXFACETPANEL_H_

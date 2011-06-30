/*******************************************************************************
File:         tmwxGroupPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for group selection panel
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-20
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXGROUPPANEL_H_
#define _TMWXGROUPPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmCluster;
class tmwxStaticText;

/**********
class tmwxGroupPanel
Displays information about a group selection
**********/

class tmwxGroupPanel : public tmwxInspectorPanel {
private:
  tmCluster* mCluster;
  tmwxStaticText* mNodes;
  tmwxStaticText* mEdges;
  tmwxStaticText* mPaths;
  tmwxStaticText* mPolys;
  tmwxStaticText* mVertices;
  tmwxStaticText* mCreases;
  tmwxStaticText* mFacets;
  tmwxStaticText* mConditions;
public:
  tmwxGroupPanel(wxWindow* parent);
  void Fill(tmCluster* aCluster);
  void Fill();
private:
  DECLARE_EVENT_TABLE()
};

#endif // _TMWXGROUPPANEL_H_

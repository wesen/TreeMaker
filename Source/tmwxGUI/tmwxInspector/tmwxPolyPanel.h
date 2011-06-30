/*******************************************************************************
File:         tmwxPolyPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for poly inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXPOLYPANEL_H_
#define _TMWXPOLYPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmPoly;
class tmwxStaticText;

/**********
class tmwxPolyPanel
Displays information about a tmPoly
**********/

class tmwxPolyPanel : public tmwxInspectorPanel {
private:
  tmPoly* mPoly;
  tmwxStaticText* mCentroid;
  tmwxStaticText* mRingNodes;
  tmwxStaticText* mRingPaths;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mPolyOwner;
  tmwxStaticText* mIsSubPoly;
  tmwxStaticText* mCrossPaths;
  tmwxStaticText* mInsetNodes;
  tmwxStaticText* mSpokePaths;
  tmwxStaticText* mRidgePath;
  tmwxStaticText* mOwnedNodes;
  tmwxStaticText* mOwnedPaths;
  tmwxStaticText* mOwnedPolys;
  tmwxStaticText* mOwnedCreases;
  tmwxStaticText* mLocalRootVertices;
  tmwxStaticText* mLocalRootCreases;
#endif
public:
  tmwxPolyPanel(wxWindow* parent);
  void Fill(tmPoly* aPoly);
  void Fill();
  void OnButton(wxCommandEvent& event);
private:
  DECLARE_EVENT_TABLE()
};

#endif //_TMWXPOLYPANEL_H_

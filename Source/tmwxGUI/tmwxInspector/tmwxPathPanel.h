/*******************************************************************************
File:         tmwxPathPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for path inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXPATHPANEL_H_
#define _TMWXPATHPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmPath;
class tmwxStaticText;
class tmwxConditionListBox;

/**********
class tmwxPathPanel
Displays information about the entire tree
**********/

class tmwxPathPanel : public tmwxInspectorPanel {
private:
  tmPath* mPath;
  tmwxStaticText* mMinTreeLength;
  tmwxStaticText* mActTreeLength;
  tmwxStaticText* mPathAngle;
  tmwxConditionListBox* mConditions;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mPathOwner;
  tmwxStaticText* mMinPaperLength;
  tmwxStaticText* mActPaperLength;
  tmwxStaticText* mIsLeafPath;
  tmwxStaticText* mIsSubPath;
  tmwxStaticText* mIsFeasiblePath;
  tmwxStaticText* mIsActivePath;
  tmwxStaticText* mIsBorderPath;
  tmwxStaticText* mIsPolygonPath;
  tmwxStaticText* mIsConditionedPath;
  tmwxStaticText* mNodes;
  tmwxStaticText* mEdges;
  tmwxStaticText* mFwdPoly;
  tmwxStaticText* mBkdPoly;
  tmwxStaticText* mOwnedVertices;
  tmwxStaticText* mOwnedCreases;
  tmwxStaticText* mOutsetPath;
  tmwxStaticText* mFrontReduction;
  tmwxStaticText* mBackReduction;
  tmwxStaticText* mMaxOutsetPath;
  tmwxStaticText* mMaxFrontReduction;
  tmwxStaticText* mMaxBackReduction;
  tmwxStaticText* mMinDepth;
  tmwxStaticText* mMinDepthDist;
#endif
public:
  tmwxPathPanel(wxWindow* parent);
  void Fill(tmPath* aPath);
  void Fill();
private:
  DECLARE_EVENT_TABLE()
};

#endif //_TMWXPATHPANEL_H_

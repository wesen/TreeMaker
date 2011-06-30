/*******************************************************************************
File:         tmwxVertexPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for node inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXVERTEXPANEL_H_
#define _TMWXVERTEXPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmVertex;
class tmwxStaticText;

/**********
class tmwxVertexPanel
Displays information about a vertex
**********/

class tmwxVertexPanel : public tmwxInspectorPanel {
private:
  tmVertex* mVertex;
  tmwxStaticText* mLoc;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mVertexOwner;
  tmwxStaticText* mCreases;
  tmwxStaticText* mIsBorderVertex;
  tmwxStaticText* mElevation;
  tmwxStaticText* mTreeNode;
  tmwxStaticText* mLeftPseudohingeMate;
  tmwxStaticText* mRightPseudohingeMate;
  tmwxStaticText* mDepth;
  tmwxStaticText* mDiscreteDepth;
#endif
public:
  tmwxVertexPanel(wxWindow* parent);
  void Fill(tmVertex* aVertex);
  void Fill();
private:
  DECLARE_EVENT_TABLE()
};

#endif // _TMWXVERTEXPANEL_H_

/*******************************************************************************
File:         tmwxCreasePanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for crease inspector panel
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCREASEPANEL_H_
#define _TMWXCREASEPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional headers
#include "tmwxStr.h"
#include "tmwxInspectorPanel.h"

// Forward declarations
class tmCrease;
class tmwxStaticText;

/**********
class tmwxCreasePanel
Displays information about a crease
**********/

class tmwxCreasePanel : public tmwxInspectorPanel {
private:
  tmCrease* mCrease;
  tmwxStaticText* mKind;
  tmwxStaticText* mFold;
  tmwxStaticText* mCreaseAngle;
#if tmwxINSPECTOR_EXTRA
  tmwxStaticText* mCreaseOwner;
  tmwxStaticText* mVertices;
#endif
public:
  tmwxCreasePanel(wxWindow* parent);
  void Fill(tmCrease* aCrease);
  void Fill();
private:
  DECLARE_EVENT_TABLE()
};

#endif // _TMWXCREASEPANEL_H_

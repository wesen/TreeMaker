/*******************************************************************************
File:         tmwxPalettePanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for any panel that goes in a tool palette
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-14
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXPALETTEPANEL_H_
#define _TMWXPALETTEPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Forward declarations
#include "tmwxGUI_fwd.h"

/**********
class tmwxPalettePanel
Base class for any panel that's part of a floating tool palette. This class
provides a couple of useful utility routines for building up the panel.
**********/

class tmwxPalettePanel : public wxPanel {
public:
  tmwxPalettePanel(wxWindow* parent);
  virtual void Fill();
};

#endif //_TMWXPALETTEPANEL_H_

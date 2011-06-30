/*******************************************************************************
File:         tmwxPalettePanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for any panel that goes in a tool palette
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-14
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

// TreeMaker GUI classes
#include "tmwxPalettePanel.h"

/*****
Constructor
*****/
tmwxPalettePanel::tmwxPalettePanel(wxWindow* parent)
  : wxPanel(parent, wxID_ANY, wxPoint(-1000, -1000))
{
}


/*****
Subclasses override this to implement refilling the palette from the object
whose properties it is displaying. This gets called when the underlying object
has changed.
*****/
void tmwxPalettePanel::Fill()
{
  TMFAIL("ancestor function tmwxPalettePanel::Fill() "\
   "should have been overridden.");
}

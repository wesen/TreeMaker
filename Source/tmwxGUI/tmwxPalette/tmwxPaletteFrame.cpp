/*******************************************************************************
File:         tmwxPaletteFrame.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for TreeMaker view settings frame
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-16
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxPaletteFrame.h"
#include "tmwxDocManager.h"

/**********
class tmwxPaletteFrame
Base class for a palette that can hold one or more switchable panels to display
objects
**********/

/*****
Constructor for tmwxPaletteFrame<wxFrame>
*****/
template <>
tmwxPaletteFrame<wxFrame>::tmwxPaletteFrame(const wxString& title, 
  const int minWidth) :
  tmwxPersistentFrame<wxFrame>(title,
#ifdef __WXMSW__
    wxSTAY_ON_TOP | wxSYSTEM_MENU | wxFRAME_NO_TASKBAR | 
#endif
    wxFRAME_TOOL_WINDOW | wxCAPTION | wxCLOSE_BOX
  ),
  mObj(0), mPanel(0), mSizer(new wxBoxSizer(wxVERTICAL))
{
  mSizer->SetMinSize(minWidth, 0);
  SetSizer(mSizer);
  mSizer->SetSizeHints(this);
  mSizer->Fit(this);

#if wxUSE_ACCEL
  SetAcceleratorTable(tmwxGetAcceleratorTable());
#endif
}


/*****
Constructor for tmwxPaletteFrame<tmwxDocParentFrame>
*****/
template <>
tmwxPaletteFrame<tmwxDocParentFrame>::tmwxPaletteFrame(wxDocManager* docManager,
  const wxString& title, const int minWidth) :
  tmwxPersistentFrame<tmwxDocParentFrame>(docManager, title,
#ifdef __WXMSW__
    wxSTAY_ON_TOP | wxSYSTEM_MENU | wxFRAME_NO_TASKBAR | 
#endif
    wxFRAME_TOOL_WINDOW | wxCAPTION | wxCLOSE_BOX
  ),
  mObj(0), mPanel(0), mSizer(new wxBoxSizer(wxVERTICAL))
{
  mSizer->SetMinSize(minWidth, 0);
  SetSizer(mSizer);
  mSizer->SetSizeHints(this);
  mSizer->Fit(this);

#if wxUSE_ACCEL
  SetAcceleratorTable(tmwxGetAcceleratorTable());
#endif
}

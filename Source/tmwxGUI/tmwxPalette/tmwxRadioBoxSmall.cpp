/*******************************************************************************
File:         tmwxRadioBoxSmall.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for small radio box
Author:       Robert J. Lang
Modified by:  
Created:      2005-08-31
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxRadioBoxSmall.h"

/*****
Constructor
Set the window variant of the child windows (the radio buttons) as well as
that of the window itself.
*****/
tmwxRadioBoxSmall::tmwxRadioBoxSmall(wxWindow* parent, const wxString& title, 
  int n, const wxString choices[], int majorDimension, long style)
  : wxRadioBox(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, n, 
    choices, majorDimension, style)
{
#ifdef __WXMAC__
  SetWindowVariant(wxWINDOW_VARIANT_SMALL);
  const wxWindowList& children = GetChildren();
  for (wxWindowList::compatibility_iterator node = children.GetFirst(); node; 
    node = node->GetNext()) {
    wxWindow* win = node->GetData();
    if(win)win->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
  }
#elif defined(__WXMSW__)
  // There is no need to set it smaller on Windows. Looks acceptable.
#else
#ifdef __LINUX__
  wxFont f = GetFont ();
  SetFont (*(wxTheFontList -> FindOrCreateFont (5 * f.GetPointSize() / 6, 
               f.GetFamily (), f.GetStyle (),
               f.GetWeight (), f.GetUnderlined (),
         f.GetFaceName (), f.GetDefaultEncoding())));
#endif
#endif
}

/*******************************************************************************
File:         tmwxCheckBoxSmall.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for small checkbox
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-23
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxCheckBoxSmall.h"

/*****
Constructor
*****/
tmwxCheckBoxSmall::tmwxCheckBoxSmall(wxWindow* parent, const wxString& title)
  : wxCheckBox(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
#ifdef __WXMAC__
  SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#else
#ifdef __LINUX__
#endif 
  wxFont f = GetFont ();
  SetFont (*(wxTheFontList -> FindOrCreateFont (5 * f.GetPointSize() / 6, 
               f.GetFamily (), f.GetStyle (),
               f.GetWeight (), f.GetUnderlined (),
         f.GetFaceName (), f.GetDefaultEncoding())));
#endif
}

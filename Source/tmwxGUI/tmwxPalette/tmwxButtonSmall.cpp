/*******************************************************************************
File:         tmwxButtonSmall.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for small button
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-23
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxButtonSmall.h"

/*****
Constructor
Height is determined by button style; width is an optional user parameter
*****/
tmwxButtonSmall::tmwxButtonSmall(wxWindow* parent, const wxString& title, const int width)
  : wxButton(parent, wxID_ANY, title, wxDefaultPosition, wxSize(width, wxDefaultCoord)
#ifdef __LINUX__
              , wxBU_EXACTFIT
#endif
             )
{
#ifdef __WXMAC__
  SetWindowVariant(wxWINDOW_VARIANT_SMALL);
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

/*******************************************************************************
File:         tmwxButtonMini.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tiniest button
Author:       Robert J. Lang
Modified by:  
Created:      2004-08-01
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxButtonMini.h"

/*****
tmwxViewSettingsPanel constants
*****/


/*****
Constructor
Height is determined by button style; width is an optional user parameter
*****/
tmwxButtonMini::tmwxButtonMini(wxWindow* parent, const wxString& title, 
  const int width)
  : wxButton(parent, wxID_ANY, title, wxDefaultPosition, 
    wxSize(width, wxDefaultCoord)
#ifdef __LINUX__
              , wxBU_EXACTFIT
#endif
             )
{
#ifdef __WXMAC__
  SetWindowVariant(wxWINDOW_VARIANT_MINI);
#else
#ifdef __LINUX__
  wxFont f = GetFont ();
  SetFont (*(wxTheFontList -> FindOrCreateFont (3 * f.GetPointSize() / 4, 
               f.GetFamily (), f.GetStyle (),
               f.GetWeight (), f.GetUnderlined (),
         f.GetFaceName (), f.GetDefaultEncoding())));
#endif
#endif
}

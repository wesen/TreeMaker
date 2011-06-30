/*******************************************************************************
File:         tmwxStr.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for TreeMaker string reprentation fns
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-21
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxStr.h"
#include "tmwxApp.h"

/**********
Common fonts
**********/
wxFont DEFAULT_FONT_9(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
  wxFONTWEIGHT_NORMAL);
wxFont DEFAULT_FONT_10(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
  wxFONTWEIGHT_NORMAL);
wxFont DEFAULT_FONT_11(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
  wxFONTWEIGHT_NORMAL);
wxFont DEFAULT_FONT_12(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
  wxFONTWEIGHT_NORMAL);


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
tmwxStr() -- Conversion of parts to strings
**********/

/*****
Return a wxString containing a size_t, but return "-1" if it is size_t(-1).
Note that different platforms may represent size_t differently, hence the
static_cast.
*****/
wxString tmwxStr(size_t lu)
{
  if (lu == size_t(-1)) return wxT("-1");
  else return wxString::Format(wxT("%lu"), static_cast<unsigned long>(lu));
}



/*****
Return a wxString containing the index of the given part or a hyphen if it's
a null pointer.
*****/
wxString tmwxStr(const tmPart* aPart)
{
  if (aPart) return wxString::Format(wxT("%lu"), 
    (unsigned long)(aPart->GetIndex()));
  else return wxString(wxT("-"));
}

/*******************************************************************************
File:         tmwxStr.h
Project:      TreeMaker 5.x
Purpose:      Header file for for TreeMaker string reprentation fns
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-21
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXSTR_H_
#define _TMWXSTR_H_

// Common TreeMaker and wxWidgets headers
#include "tmHeader.h"
#include "tmModel.h"
#include "wx/string.h"

// Forward declarations
#include "tmwxGUI_fwd.h"

/**********
Common fonts
**********/
extern wxFont DEFAULT_FONT_9;
extern wxFont DEFAULT_FONT_10;
extern wxFont DEFAULT_FONT_11;
extern wxFont DEFAULT_FONT_12;


/**********
tmwxStr() -- Conversion of parts to strings
**********/
wxString tmwxStr(std::size_t lu);
wxString tmwxStr(const tmPart* aPart);
template <class P>
  wxString tmwxStr(const tmArray<P*>& plist);
template <class P>
  wxString tmwxStr(const tmArray<P*>& plist, std::size_t maxLength);


/**********
Template definitions
**********/

/*****
Return a wxString containing the indices of a list of ptrs to parts.
*****/
template <class P>
wxString tmwxStr(const tmArray<P*>& plist)
{
  wxString text;
  for (size_t i = 0; i < plist.size(); ++i)
    text << tmwxStr(plist[i]) << wxT(" ");
  return text;
}


/*****
Return a wxString containing the indices of a list of ptrs to parts, but if
the number of parts exceeds maxLength, include only the first maxLength indices
and then append ellipsis.
*****/
template <class P>
wxString tmwxStr(const tmArray<P*>& plist, std::size_t maxLength)
{
  wxString text;
  std::size_t n = min_val(plist.size(), maxLength);
  for (size_t i = 0; i < n; ++i)
    text << tmwxStr(plist[i]) << wxT(" ");
  if (n < plist.size())
    text << wxT("...");
  return text;
}


#endif // _TMWXSTR_H_

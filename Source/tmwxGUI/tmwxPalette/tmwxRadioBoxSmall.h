/*******************************************************************************
File:         tmwxRadioBoxSmall.h
Project:      TreeMaker 5.x
Purpose:      Header file for small radio box
Author:       Robert J. Lang
Modified by:  
Created:      2005-08-31
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXRADIOBOXSMALL_H_
#define _TMWXRADIOBOXSMALL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

/**********
class tmwxRadioBoxSmall
Specialization of wxRadioBox, small window variant
**********/
class tmwxRadioBoxSmall : public wxRadioBox {
public:
  // Constructor
  tmwxRadioBoxSmall(wxWindow* parent, const wxString& title, int n, 
  const wxString choices[], int majorDimension = 1, 
  long style = wxRA_SPECIFY_COLS);
};

#endif // _TMWXCHECKBOXSMALL_H_

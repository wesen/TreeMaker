/*******************************************************************************
File:         tmwxCheckBoxSmall.h
Project:      TreeMaker 5.x
Purpose:      Header file for small checkbox
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-23
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCHECKBOXSMALL_H_
#define _TMWXCHECKBOXSMALL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

/**********
class tmwxCheckBoxSmall
Specialization of wxCheckBox, small window variant
**********/
class tmwxCheckBoxSmall : public wxCheckBox {
public:
  // Constructors
  tmwxCheckBoxSmall(wxWindow* parent, const wxString& title);
};

#endif // _TMWXCHECKBOXSMALL_H_

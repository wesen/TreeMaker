/*******************************************************************************
File:         tmwxCheckBox.h
Project:      TreeMaker 5.x
Purpose:      Header file for regular checkbox
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-11
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCHECKBOX_H_
#define _TMWXCHECKBOX_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

/**********
class tmwxCheckBox
Specialization of wxCheckBox
**********/
class tmwxCheckBox : public wxCheckBox {
public:
  // Constructors
  tmwxCheckBox(wxWindow* parent, const wxString& title);
};

#endif // _TMWXCHECKBOX_H_

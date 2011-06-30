/*******************************************************************************
File:         tmwxCheckBox.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for checkbox
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-11
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxCheckBox.h"

/*****
Constructor
*****/
tmwxCheckBox::tmwxCheckBox(wxWindow* parent, const wxString& title)
  : wxCheckBox(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
}

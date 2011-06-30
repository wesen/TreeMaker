/*******************************************************************************
File:         tmwxButtonSmall.h
Project:      TreeMaker 5.x
Purpose:      Header file for small button
Author:       Robert J. Lang
Modified by:  
Created:      2004-08-01
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXBUTTONSMALL_H_
#define _TMWXBUTTONSMALL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

/**********
class tmwxButtonSmall
Specialization of wxButton, small window variant
**********/
class tmwxButtonSmall : public wxButton {
public:
  // Constructors
  tmwxButtonSmall(wxWindow* parent, const wxString& title, 
    const int width = wxDefaultCoord);
};

#endif // _TMWXBUTTONSMALL_H_

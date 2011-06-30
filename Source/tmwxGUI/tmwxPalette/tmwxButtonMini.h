/*******************************************************************************
File:         tmwxButtonMini.h
Project:      TreeMaker 5.x
Purpose:      Header file for tiniest button
Author:       Robert J. Lang
Modified by:  
Created:      2004-08-01
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXBUTTONMINI_H_
#define _TMWXBUTTONMINI_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

/**********
class tmwxButtonMini
Specialization of wxButton, small window variant
**********/
class tmwxButtonMini : public wxButton {
public:
  // Constructors
  tmwxButtonMini(wxWindow* parent, const wxString& title, 
    const int width = wxDefaultCoord);
};

#endif // _TMWXBUTTONMINI_H_

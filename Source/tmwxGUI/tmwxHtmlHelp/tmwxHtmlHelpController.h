/*******************************************************************************
File:         tmwxHtmlHelpController.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker help controller class
Author:       Robert J. Lang
Modified by:  
Created:      2005-11-23
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXHTMLHELPCONTROLLER_H_
#define _TMWXHTMLHELPCONTROLLER_H_

// Specific wxWidgets headers
#include "wx/html/helpctrl.h"

// Forward declarations
class tmwxHtmlHelpFrame;

/**********
class tmwxHtmlHelpController
Controls the display of help information and documentation
**********/

class tmwxHtmlHelpController: public wxHtmlHelpController
{
public:
  tmwxHtmlHelpFrame* mHtmlHelpFrame;
  wxHtmlEasyPrinting* mHtmlEasyPrinting;

  tmwxHtmlHelpController();
  wxHtmlHelpFrame* CreateHelpFrame(wxHtmlHelpData* data);
  void CreateHelpWindow();
};


#endif // _TMWXHTMLHELPCONTROLLER_H_

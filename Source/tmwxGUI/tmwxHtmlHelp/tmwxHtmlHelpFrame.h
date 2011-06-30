/*******************************************************************************
File:         tmwxHtmlHelpFrame.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker help frame class
Author:       Robert J. Lang
Modified by:  
Created:      2005-11-23
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXHTMLHELPFRAME_H_
#define _TMWXHTMLHELPFRAME_H_

// Specific wxWidgets headers
#include "wx/html/helpfrm.h"

// Forward declarations

/**********
class tmwxHtmlHelpFrame
Displays help information and documentation and prints it
**********/

class tmwxHtmlHelpFrame: public wxHtmlHelpFrame
{
public:
  tmwxHtmlHelpFrame(wxHtmlHelpData* data);
  void SetHtmlHelpPrinter(wxHtmlEasyPrinting* printer);
  
  // Event handling
  void OnActivate(wxActivateEvent& event);
  void OnPrintUpdateUI(wxUpdateUIEvent& event);
  void OnPrint(wxCommandEvent& event);
  void OnPrintPreviewUpdateUI(wxUpdateUIEvent& event);
  void OnPrintPreview(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};


#endif // _TMWXHTMLHELPFRAME_H_

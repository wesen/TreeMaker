/*******************************************************************************
File:         tmwxDocManager.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker document manager class
Author:       Robert J. Lang
Modified by:  
Created:      2005-11-30
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXDOCMANAGER_H_
#define _TMWXDOCMANAGER_H_

// Additional wxWidgets headers
#include "wx/docview.h"

// Forward declarations
class tmwxDoc;
class tmwxView;

/**********
class tmwxDocManager
specializes document management class
**********/
class tmwxDocManager: public wxDocManager {
public:
  tmwxDocManager() { mDoc = NULL; }

  // Workaround to broken activation philosphy
  void SetCurrentDocumentLocal(tmwxDoc* doc);
  tmwxDoc* GetCurrentDocumentLocal();
  tmwxView* GetCurrentViewLocal();
    
  // Handle closure of last document
  void CheckLastDoc();
  
  // Event handling
  void OnFileClose(wxCommandEvent& event);
  void OnFileRevertUpdateUI(wxUpdateUIEvent& event);
  void OnPrint(wxCommandEvent& event);

private:
  tmwxDoc* mDoc;

  DECLARE_EVENT_TABLE()
};


#endif // _TMWXDOCMANAGER_H_

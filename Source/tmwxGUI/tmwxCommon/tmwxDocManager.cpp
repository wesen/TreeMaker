/*******************************************************************************
File:         tmwxDocManager.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker document manager class
Author:       Robert J. Lang
Modified by:  
Created:      2005-11-30
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDocManager.h"
#include "tmwxApp.h"
#include "tmwxDoc.h"
#include "tmwxView.h"
#include "tmwxPrintout.h"

/**********
class tmwxDocManager
specializes document management class
**********/

/*
These next three routines replace the (non-virtual) framework routines
GetCurrentDocument() and GetCurrentView(), which, due to broken activation,
force us to maintain our own parallel copy of the local document. Since the
routines we're replacing are non-virtual, I've given their replacement new
names rather than simply overloading the old ones, since the framework will
(possibly confusingly) continue to use the base class routines rather than the
overloaded ones.
*/

/*****
Set local copy of the currently-active document
*****/
void tmwxDocManager::SetCurrentDocumentLocal(tmwxDoc* doc)
{
  // It should not be necessary to have this method
  // but due to clearly broken activation philosphy we store current doc
  mDoc = doc;
}


/*****
Return a ptr to local copy of the currently-active document
*****/
tmwxDoc* tmwxDocManager::GetCurrentDocumentLocal()
{
  // That's how it should look:
  // return wxDynamicCast(wxDocManager::GetCurrentDocumentLocal(), tmwxDoc);
  // but due to clearly broken activation philosphy we store current doc
  return mDoc;
}


/*****
Return a ptr to the currently-active view
*****/
tmwxView* tmwxDocManager::GetCurrentViewLocal()
{
  // That's how it should look:
  // return wxDynamicCast(wxDocManager::GetCurrentViewLocal(), tmwxView);
  // but due to clearly broken activation philosphy we store current doc
  if (!mDoc) return NULL;
  return wxDynamicCast(mDoc->GetFirstView(), tmwxView);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
If the document that we're closing is the last document, behavior is platform-
specific. Called in tmwxDocManager::OnFileClose() (menu closure) and
tmwxDesignFrame::OnClose() (window closure).
*****/
void tmwxDocManager::CheckLastDoc()
{
#if defined(__WXMAC__)
  return;
#elif defined(__WXMSW__) || defined(__LINUX__)
  size_t num_docs = GetDocuments().GetCount();
  if (num_docs == 1) gApp->Exit();
#elif defined(__LINUX_CREATE_NEW_AFTER_CLOSE__)
  size_t num_docs = GetDocuments().GetCount();
  if (num_docs == 0) CreateDocument(wxEmptyString, wxDOC_NEW);
#else
  #error "unspecified platform"
#endif
}



#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Handle closure of a file. If, after we close the file, there are no more,
quit the app on non-Mac platforms.
*****/
void tmwxDocManager::OnFileClose(wxCommandEvent& event)
{
  event.Skip();
  CheckLastDoc();
}


/*****
Update the Revert menu item. We must handle because framework doesn't handle
properly: don't enable the menu item if we haven't modified the file.
*****/
void tmwxDocManager::OnFileRevertUpdateUI(wxUpdateUIEvent& event)
{
  wxDocument* doc = GetCurrentDocumentLocal();
  event.Enable((doc != NULL) && doc->IsModified());
}


/*****
Print the current document.
*****/
void tmwxDocManager::OnPrint(wxCommandEvent& WXUNUSED(event))
{
  if (!GetCurrentDocumentLocal()) return;
  wxPrintDialogData printDialogData(gApp->GetPrintData());
  wxPrinter printer(&printDialogData);
  tmwxPrintout printout(GetCurrentDocumentLocal()->GetView());
  if (!printer.Print(gApp->GetTopWindow(), &printout, true)) {
    if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
      // TBD - customize message?
      tmwxAlertError(
        wxT("There was a problem printing.\n")
        wxT("Perhaps your current printer is not set correctly?"), 
        wxT("Printing Error"));
  } 
  else
    gApp->GetPrintData() = printer.GetPrintDialogData().GetPrintData();
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxDocManager, wxDocManager)
  EVT_MENU(wxID_CLOSE, tmwxDocManager::OnFileClose)
  EVT_UPDATE_UI(wxID_REVERT, tmwxDocManager::OnFileRevertUpdateUI)
  EVT_MENU(wxID_PRINT, tmwxDocManager::OnPrint)
END_EVENT_TABLE()

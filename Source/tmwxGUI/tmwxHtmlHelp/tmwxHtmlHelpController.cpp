/*******************************************************************************
File:         tmwxHtmlHelpController.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmwxHtmlHelpController
Author:       Robert J. Lang
Modified by:  
Created:      2005-11-23
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxHtmlHelpController.h"
#include "tmwxHtmlHelpFrame.h"
#include "tmwxApp.h"

/*****
Constructor
*****/
tmwxHtmlHelpController::tmwxHtmlHelpController() : 
  wxHtmlHelpController(wxHF_TOOLBAR | wxHF_CONTENTS | wxHF_SEARCH | 
    wxHF_BOOKMARKS | wxHF_PRINT),
  mHtmlHelpFrame(0),
  mHtmlEasyPrinting(0)
{
}


/*****
Return a help frame. We override this so that we can set up the printer
parameters that go with this frame.
*****/
wxHtmlHelpFrame* tmwxHtmlHelpController::CreateHelpFrame(wxHtmlHelpData* data)
{
  mHtmlHelpFrame = new tmwxHtmlHelpFrame(data);
  mHtmlEasyPrinting = new wxHtmlEasyPrinting();
  mHtmlEasyPrinting->SetFooter(
    wxT("<hr><p align=\"right\">page @PAGENUM@ of @PAGESCNT@</p>"), 
    wxPAGE_ALL);
  int fontsizes[] = { 6, 8, 10, 12, 14, 16, 18 }; 
  mHtmlEasyPrinting->SetFonts(wxEmptyString, wxEmptyString, fontsizes);
  mHtmlHelpFrame->SetHtmlHelpPrinter(mHtmlEasyPrinting);
  return mHtmlHelpFrame;
}


/*****
Create the help window. Overridden so that on Mac we can replace the menu bar
of the frame.
*****/
void tmwxHtmlHelpController::CreateHelpWindow()
{
#ifdef __WXMAC__
  // Here we simply reproduce the ancestor method, except for the extra call
  // to replace the menu bar partway through.
  if (m_helpFrame) {
    m_helpFrame->Raise();
    return ;
  }
  if (m_Config == NULL) {
    m_Config = wxConfigBase::Get(false);
    if (m_Config != NULL)
      m_ConfigRoot = _T("wxWindows/wxHtmlHelpController");
  }
  m_helpFrame = CreateHelpFrame(&m_helpData);
  m_helpFrame->SetController(this);
  if (m_Config)
    m_helpFrame->UseConfig(m_Config, m_ConfigRoot);
  m_helpFrame->Create(NULL, wxID_HTML_HELPFRAME, wxEmptyString, m_FrameStyle);
  gApp->MakeMenuBar(m_helpFrame);  // replaced menu bar with our own
  m_helpFrame->SetTitleFormat(m_titleFormat);
  m_helpFrame->Show(true);
#else
  wxHtmlHelpController::CreateHelpWindow();
#endif // __WXMAC__
}

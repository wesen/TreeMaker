/*******************************************************************************
File:         tmwxDoc_View.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker document class, View menu commands
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-10
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDoc.h"
#include "tmwxApp.h"
#include "tmModel.h"
#include "tmwxView.h"
#include "tmwxViewSettings.h"
#include "tmwxDesignCanvas.h"
#include "tmwxGetUserInputDialog.h"
#include "tmwxTextCtrl.h"

using namespace std;

/**********
class tmwxSetPaperSizeDialog
Utility dialog for obtaining paper size from the user
**********/
class tmwxSetPaperSizeDialog : public tmwxGetUserInputDialog {
public:
  tmwxTextCtrl* mPaperSizeText;
  double mPaperSize;

  tmwxSetPaperSizeDialog(const tmFloat& paperSize);
  bool Validate();
};


/*****
Constructor
*****/
tmwxSetPaperSizeDialog::tmwxSetPaperSizeDialog(const tmFloat& paperSize)
  : tmwxGetUserInputDialog(wxT("Set Paper Size"))
{
  AddTextPair(wxT("Paper size (in): "), mPaperSizeText, wxT("%.2f"));
  mPaperSizeText->SetValue(paperSize);
  mPaperSizeText->SetSelection(-1, -1);
  FinishDialog();
  mPaperSizeText->SetFocus();
}


/*****
Make sure the user entry is a valid paper size.
*****/
bool tmwxSetPaperSizeDialog::Validate()
{
  return mPaperSizeText->ValidateFloat(wxT("Paper size"), 0., false,
    mPaperSize,
    wxT("Paper size must be positive."));
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxDoc
TreeMaker document class
**********/

/*****
Enable View->Default View
*****/
void tmwxDoc::OnDefaultViewUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
  event.Check(
    GetDesignCanvas()->GetViewSettings() == tmwxViewSettings::sDesignView);
}


/*****
Perform View->Default View
*****/
void tmwxDoc::OnDefaultView(wxCommandEvent&)
{
  GetDesignCanvas()->SetViewSettings(tmwxViewSettings::sDesignView);
  // No changes to document, just update views
  UpdateAllViews();
}


/*****
Enable View->Tree View
*****/
void tmwxDoc::OnTreeViewUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
  event.Check(
    GetDesignCanvas()->GetViewSettings() == tmwxViewSettings::sTreeView);
}


/*****
Perform View->Tree View
*****/
void tmwxDoc::OnTreeView(wxCommandEvent&)
{
  GetDesignCanvas()->SetViewSettings(tmwxViewSettings::sTreeView);
  // No changes to document, just update views
  UpdateAllViews();
}


/*****
Enable View->Creases View
*****/
void tmwxDoc::OnCreasesViewUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
  event.Check(
    GetDesignCanvas()->GetViewSettings() == tmwxViewSettings::sCreasesView);
}


/*****
Perform View->Creases View
*****/
void tmwxDoc::OnCreasesView(wxCommandEvent&)
{
  GetDesignCanvas()->SetViewSettings(tmwxViewSettings::sCreasesView);
  // No changes to document, just update views
  UpdateAllViews();
}


/*****
Enable View->Plan View
*****/
void tmwxDoc::OnPlanViewUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
  event.Check(
    GetDesignCanvas()->GetViewSettings() == tmwxViewSettings::sPlanView);
}


/*****
Perform View->Plan View
*****/
void tmwxDoc::OnPlanView(wxCommandEvent&)
{
  GetDesignCanvas()->SetViewSettings(tmwxViewSettings::sPlanView);
  // No changes to document, just update views
  UpdateAllViews();
}


/*****
Enable View->Fit To Screen
*****/
void tmwxDoc::OnFitToScreenUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(!GetDesignCanvas()->IsFitPaperSize());
}


/*****
Perform View->Fit To Screen
*****/
void tmwxDoc::OnFitToScreen(wxCommandEvent&)
{
  if (GetDesignCanvas()->IsFitPaperSize()) return;
  GetDesignCanvas()->FitPaperSize();
  // No changes to document, just update views
  UpdateAllViews();
}


/*****
Enable View->Fit To Width
*****/
void tmwxDoc::OnFitToWidthUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(!GetDesignCanvas()->IsFitPaperWidth());
}


/*****
Perform View->Fit To Width
*****/
void tmwxDoc::OnFitToWidth(wxCommandEvent&)
{
  if (GetDesignCanvas()->IsFitPaperWidth()) return;
  GetDesignCanvas()->FitPaperWidth();
  // No changes to document, just update views
  UpdateAllViews();
}


/*****
Enable View->Fit To Height
*****/
void tmwxDoc::OnFitToHeightUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(!GetDesignCanvas()->IsFitPaperHeight());
}


/*****
Perform View->Fit To Height
*****/
void tmwxDoc::OnFitToHeight(wxCommandEvent&)
{
  if (GetDesignCanvas()->IsFitPaperHeight()) return;
  GetDesignCanvas()->FitPaperHeight();
  // No changes to document, just update views
  UpdateAllViews();
}


/*****
Enable View->Set Paper Size
*****/
void tmwxDoc::OnSetPaperSizeUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
}


/*****
Perform View->Set Paper Size
*****/
void tmwxDoc::OnSetPaperSize(wxCommandEvent&)
{
  tmwxSetPaperSizeDialog dialog(GetDesignCanvas()->GetPaperSize());
  if (dialog.ShowModal() == wxID_OK) {
    GetDesignCanvas()->SetPaperSize(dialog.mPaperSize);
    // No changes to document, just update views
    UpdateAllViews();
  }
}

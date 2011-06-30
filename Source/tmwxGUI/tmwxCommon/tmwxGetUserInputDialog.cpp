/*******************************************************************************
File:         tmwxGetUserInputDialog.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmwxGetUserInputDialog class
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-11
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxGetUserInputDialog.h"
#include "tmwxApp.h"
#include "tmwxTextCtrl.h"
#include "tmwxStaticText.h"
#include "tmwxCheckBox.h"

// Additional wxWidgets headers
#include "wx/statline.h"

/**********
class tmwxGetUserInputDialog
Utility dialog for getting input from the user
**********/

/*****
Constructor
*****/
tmwxGetUserInputDialog::tmwxGetUserInputDialog(const wxString& title)
  : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, 
    wxDefaultSize, 
//     TMWX_MODAL_DIALOG_STYLE
    wxCAPTION
#ifdef __WXMSW__
                   | wxSYSTEM_MENU | wxCLOSE_BOX
#endif
            )
{
  mDialogSizer = new wxBoxSizer(wxVERTICAL);
  mDialogSizer->Add(300, 5, 0);
}


/*****
Add a tmwxStaticText caption and tmwxTextCtrl in a single row.
*****/
void tmwxGetUserInputDialog::AddTextPair(const wxString& caption, 
  tmwxTextCtrl*& textctrl, const wxString& format)
{
  wxGridSizer* gridsizer = new wxGridSizer(2, 0, 5);
  gridsizer->Add(CreateTextSizer(caption), 
    wxSizerFlags().Right().Border(wxALL, 2));
  gridsizer->Add(textctrl = new tmwxTextCtrl(this, format), 
    wxSizerFlags().Left().Border(wxALL, 2));
  mDialogSizer->Add(gridsizer, wxSizerFlags().Expand());
}


/*****
Add a tmwxCheckBox and a twmxTextCtrl in a single row.
*****/
void tmwxGetUserInputDialog::AddCheckPair(tmwxCheckBox*& checkbox, 
  const wxString& caption, tmwxTextCtrl*& textctrl, const wxString format)
{
  wxGridSizer* gridsizer = new wxGridSizer(2, 0, 5);
  gridsizer->Add(checkbox = new tmwxCheckBox(this, caption), 
    wxSizerFlags().Right().Border(wxALL, 2));
  gridsizer->Add(textctrl = new tmwxTextCtrl(this, format), 
    wxSizerFlags().Left().Border(wxALL, 2));
  mDialogSizer->Add(gridsizer, wxSizerFlags().Expand());
}


/*****
Add a wxRadioButton and a twmxTextCtrl in a single row that starts a radio
group.
*****/
void tmwxGetUserInputDialog::StartRadioPair(wxRadioButton*& radiobtn, 
  const wxString& caption, tmwxTextCtrl*& textctrl, const wxString format)
{
  wxGridSizer* gridsizer = new wxGridSizer(2, 0, 5);
  gridsizer->Add(radiobtn = new wxRadioButton(this, wxID_ANY, caption, 
    wxDefaultPosition, wxDefaultSize, wxRB_GROUP), 
    wxSizerFlags().Left().Border(wxALL, 2));
  gridsizer->Add(textctrl = new tmwxTextCtrl(this, format), 
    wxSizerFlags().Left().Border(wxALL, 2));
  mDialogSizer->Add(gridsizer, wxSizerFlags().Expand());
}


/*****
Add a wxRadioButton and a twmxTextCtrl in a single row.
*****/
void tmwxGetUserInputDialog::AddRadioPair(wxRadioButton*& radiobtn, 
  const wxString& caption, tmwxTextCtrl*& textctrl, const wxString format)
{
  wxGridSizer* gridsizer = new wxGridSizer(2, 0, 5);
  gridsizer->Add(radiobtn = new wxRadioButton(this, wxID_ANY, caption, 
    wxDefaultPosition, wxDefaultSize), 
    wxSizerFlags().Left().Border(wxALL, 2));
  gridsizer->Add(textctrl = new tmwxTextCtrl(this, format), 
    wxSizerFlags().Left().Border(wxALL, 2));
  mDialogSizer->Add(gridsizer, wxSizerFlags().Expand());
}


/*****
Finish creating the dialog
*****/
void tmwxGetUserInputDialog::FinishDialog()
{
  mDialogSizer->AddSpacer(10);
  mDialogSizer->Add(CreateButtonSizer(wxOK | wxCANCEL),
    wxSizerFlags().Expand().Border(wxALL, 10));
  SetAutoLayout(true);
  SetSizerAndFit(mDialogSizer);
  Center(wxBOTH);
}

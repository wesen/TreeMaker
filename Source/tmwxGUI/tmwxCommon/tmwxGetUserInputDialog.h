/*******************************************************************************
File:         tmwxGetUserInputDialog.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmwxGetUserInputDialog class
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-11
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXGETUSERINPUTDIALOG_H_
#define _TMWXGETUSERINPUTDIALOG_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// forward declarations
class tmwxCheckBox;
class tmwxTextCtrl;

/**********
class tmwxGetUserInputDialog
Utility dialog for getting input from the user
**********/
class tmwxGetUserInputDialog : public wxDialog {
public:
  tmwxGetUserInputDialog(const wxString& title);
protected:
  wxBoxSizer* mDialogSizer;
  
  void AddTextPair(const wxString& caption, tmwxTextCtrl*& textctrl,
    const wxString& format = wxEmptyString);
  void AddCheckPair(tmwxCheckBox*& checkbox, const wxString& caption, 
    tmwxTextCtrl*& textctrl, const wxString format = wxEmptyString);
  void StartRadioPair(wxRadioButton*& radiobtn, const wxString& caption, 
    tmwxTextCtrl*& textctrl, const wxString format = wxEmptyString);
  void AddRadioPair(wxRadioButton*& radiobtn, const wxString& caption, 
    tmwxTextCtrl*& textctrl, const wxString format = wxEmptyString);
  void FinishDialog();
};


#endif // _TMWXGETUSERINPUTDIALOG_H_

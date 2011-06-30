/*******************************************************************************
File:         tmwxInspectorPanel.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for any panel that goes in the Inspector
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-07
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

// Additional wxWidgets classes
#include "wx/statline.h"

// TreeMaker GUI classes
#include "tmwxInspectorPanel.h"
#include "tmwxStr.h"
#include "tmwxStaticText.h"
#include "tmwxTextCtrl.h"
#include "tmwxButtonSmall.h"
#include "tmwxCheckBoxSmall.h"
#include "tmwxConditionListBox.h"
#include "tmwxApp.h"
#include "tmwxDocManager.h"

/*****
Constructor
*****/
tmwxInspectorPanel::tmwxInspectorPanel(wxWindow* parent)
  : tmwxPalettePanel(parent)
{
  mPanelBox = new wxStaticBox(this, wxID_ANY, wxT("-"));
  mPanelSizer = new wxStaticBoxSizer(mPanelBox, wxVERTICAL);
#if tmwxINSPECTOR_EXTRA
  mGridExtraSizer = 0;
#endif // tmwxINSPECTOR_EXTRA
}


/*****
Add a tmwxStaticText caption and tmwxTextCtrl in a single row.
*****/
void tmwxInspectorPanel::AddTextPair(const wxString& caption, 
  tmwxTextCtrl*& textctrl, const wxString& format)
{
  wxGridSizer* gridsizer = new wxGridSizer(2);
  gridsizer->Add(new tmwxStaticText(this, 11, caption), 
   0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL | wxTOP|wxBOTTOM, 2);
  gridsizer->Add(textctrl = new tmwxTextCtrl(this, format), 
   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL | wxTOP|wxBOTTOM, 2);
  mPanelSizer->Add(gridsizer, wxSizerFlags(1).Expand());
}


/*****
Add a checkbox.
*****/
void tmwxInspectorPanel::AddCheckBox(wxCheckBox*& checkbox, 
  const wxString& caption)
{
  mPanelSizer->Add(checkbox = new tmwxCheckBoxSmall(this, caption),
    0, wxALIGN_LEFT | wxTOP|wxBOTTOM, 2);
}


/*****
Add a twmxCheckboxSmall and a twmxTextCtrl in a single row.
*****/
void tmwxInspectorPanel::AddCheckPair(wxCheckBox*& checkbox, 
  const wxString& caption, tmwxTextCtrl*& textctrl, const wxString format)
{
  wxGridSizer* gridsizer = new wxGridSizer(2);
  gridsizer->Add(checkbox = new tmwxCheckBoxSmall(this, caption), 
   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL | wxTOP|wxBOTTOM, 2);
  gridsizer->Add(textctrl = new tmwxTextCtrl(this, format), 
   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL | wxTOP|wxBOTTOM, 2);
  mPanelSizer->Add(gridsizer, wxSizerFlags(1).Expand());
}


/*****
Add a twmxStaticText element in standard size.
*****/
void tmwxInspectorPanel::AddStaticText(tmwxStaticText*& stattext)
{
  mPanelSizer->Add(stattext = new tmwxStaticText(this, 11, wxEmptyString));
}


/*****
Add an "Apply" button to the panel.
*****/
void tmwxInspectorPanel::AddApplyButton()
{
  tmwxButtonSmall* applyButton = new tmwxButtonSmall(this, wxT("Apply"));
  applyButton->SetId(wxID_APPLY);
  mPanelSizer->Add(applyButton,
   0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL | wxTOP|wxBOTTOM, 2);
}


/*****
Add a tmwxConditionListBox.
*****/
void tmwxInspectorPanel::AddConditionListBox(tmwxConditionListBox*& clistbox, 
  const wxSize size)
{
  wxStaticBoxSizer* conditionBoxSizer = new wxStaticBoxSizer(
    new wxStaticBox(this, wxID_ANY, wxT("Conditions")),
    wxVERTICAL);
  conditionBoxSizer->Add(clistbox = new tmwxConditionListBox(this, size),
    1, wxGROW);
  mPanelSizer->Add(conditionBoxSizer,
    0, wxGROW);
  
}


/*****
Add a static line divider
*****/
void tmwxInspectorPanel::AddStatLine()
{
  mPanelSizer->Add(new wxStaticLine(this, wxID_ANY), 0, 
    wxEXPAND | wxTOP|wxBOTTOM, 2);
}


/*****
Add some blank space
*****/
void tmwxInspectorPanel::AddSpacer(int size)
{
  mPanelSizer->AddSpacer(size);
}


#if tmwxINSPECTOR_EXTRA
/*****
Start the section of extra data
*****/
void tmwxInspectorPanel::InitExtra()
{
  AddStatLine();
  mGridExtraSizer = new wxGridSizer(2);
  mPanelSizer->Add(mGridExtraSizer, 0, wxEXPAND);
}


/*****
Add a twmxStaticText element in "extra" size to the extra sizer.
*****/
void tmwxInspectorPanel::AddStatTextGridExtra(tmwxStaticText*& stattext)
{
  TMASSERT(mGridExtraSizer);
  mGridExtraSizer->Add(stattext = new tmwxStaticText(this, 9, wxEmptyString));
}


/*****
Add a twmxStaticText element in "extra" size to the extra sizer, but giving
it an entire row (used for array data).
*****/
void tmwxInspectorPanel::AddStatTextRowExtra(tmwxStaticText*& stattext)
{
  mPanelSizer->Add(stattext = new tmwxStaticText(this, 9, wxEmptyString));
}

#endif // tmwxINSPECTOR_EXTRA

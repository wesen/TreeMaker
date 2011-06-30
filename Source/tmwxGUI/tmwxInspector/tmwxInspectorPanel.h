/*******************************************************************************
File:         tmwxInspectorPanel.h
Project:      TreeMaker 5.x
Purpose:      Header file for any panel that goes in a tool palette
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-07
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXINSPECTORPANEL_H_
#define _TMWXINSPECTORPANEL_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Forward declarations
#include "tmwxGUI_fwd.h"

// Ancestor class
#include "tmwxPalettePanel.h"


/**********
Preprocessor symbols
**********/
// This switch specifies whether to show "extra" information in the
// inspector panels. Yes for debug builds, no for release builds.
#ifdef TMDEBUG
  #define tmwxINSPECTOR_EXTRA 1
#else
  #define tmwxINSPECTOR_EXTRA 0
#endif


/**********
class tmwxInspectorPanel
Base class for any panel that goes in the Inspector. This class
provides a couple of useful utility routines for building up the panel.
**********/

class tmwxInspectorPanel : public tmwxPalettePanel {
public:
  tmwxInspectorPanel(wxWindow* parent);
protected:
  wxStaticBoxSizer* mPanelSizer;
  wxStaticBox* mPanelBox;

#if tmwxINSPECTOR_EXTRA
  wxGridSizer* mGridExtraSizer;
#endif // tmwxINSPECTOR_EXTRA

  void AddTextPair(const wxString& caption, tmwxTextCtrl*& textctrl, 
    const wxString& format = wxEmptyString);
  void AddCheckBox(wxCheckBox*& checkbox, const wxString& caption);
  void AddCheckPair(wxCheckBox*& checkbox, const wxString& caption, 
    tmwxTextCtrl*& textctrl, const wxString format = wxEmptyString);
  void AddStaticText(tmwxStaticText*& stattext);
  void AddApplyButton();
  void AddConditionListBox(tmwxConditionListBox*& clistbox, 
    const wxSize size);
  void AddStatLine();
  void AddSpacer(int size);

#if tmwxINSPECTOR_EXTRA
  void InitExtra();
  void AddStatTextGridExtra(tmwxStaticText*& stattext);
  void AddStatTextRowExtra(tmwxStaticText*& stattext);
#endif // tmwxINSPECTOR_EXTRA
};

#endif //_TMWXINSPECTORPANEL_H_

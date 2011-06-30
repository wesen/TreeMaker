/*******************************************************************************
File:         tmwxPaletteFrame.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker frame class for palettes
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-03
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXPALETTEFRAME_H_
#define _TMWXPALETTEFRAME_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Forward declarations
#include "tmwxGUI_fwd.h"

// Addition headers
#include "tmwxPersistentFrame.h"
#include "tmwxPalettePanel.h"

/**********
class tmwxPaletteFrame
Base class for a palette that can hold one or more switchable panels to display
objects
**********/
template <class Frame>
class tmwxPaletteFrame: public tmwxPersistentFrame<Frame> {
public:
  tmwxPaletteFrame(const wxString& title, const int minWidth);
  tmwxPaletteFrame(wxDocManager* manager, const wxString& title, 
    const int minWidth);
  void SetSelection();
  virtual void FillPanel();

  void SetOnTopStyle(bool isActive);
  
protected:
  void* mObj;          // currently visible object
  tmwxPalettePanel* mPanel;  // currently visible panel
  wxBoxSizer* mSizer;      // the sizer

  virtual void UninstallPanel();
  virtual void InstallPanel(tmwxPalettePanel* aPanel);
};


/**********
Template functions
**********/

/*****
Activate/deactivate on top style.
*****/
template <class Frame>
void tmwxPaletteFrame<Frame>::SetOnTopStyle(bool isActive)
{
#ifdef __WXMSW__
  if(isActive) {
    if(!HasFlag(wxSTAY_ON_TOP)) {
      wxWindow *win = ::wxGetActiveWindow();
      SetWindowStyle(GetWindowStyle() | wxSTAY_ON_TOP );
      if(win)win->SetFocus();
    }
  } else if(HasFlag(wxSTAY_ON_TOP)) {
    SetWindowStyle(GetWindowStyle() & ~wxSTAY_ON_TOP );
    Lower();
  }
#endif //__WXMSW__
}


/*****
Refill the currently-visible panel. This gets called when the underlying data
has changed.
*****/
template <class Frame>
void tmwxPaletteFrame<Frame>::FillPanel()
{
  if (mPanel) mPanel->Fill();
}


/*****
Set the selection to nothing, i.e., get rid of the current contents.
*****/
template <class Frame>
void tmwxPaletteFrame<Frame>::SetSelection()
{
  if (mObj == 0) return;
  mObj = 0;
  UninstallPanel();
}


/*****
If there is a panel visible, hide it and detach it from our sizer. If no panel
is visible, then do nothing.
*****/
template <class Frame>
void tmwxPaletteFrame<Frame>::UninstallPanel()
{
  if (mPanel) {
    mPanel->Hide();
    mSizer->Detach(mPanel);
    mSizer->Layout();
    mSizer->SetSizeHints(this);
    mSizer->Fit(this);
    mPanel = 0;
  }
}

/*****
Make the given panel visible and resize the frame to fit it. While
UninstallPanel() can be safely double-called, this should only be called if
there is no panel installed.
*****/
template <class Frame>
void tmwxPaletteFrame<Frame>::InstallPanel(tmwxPalettePanel* aPanel)
{
  TMASSERT(mPanel == 0);
  mPanel = aPanel;
  mPanel->Show();  // need to make it visible first so sizer recognizes it
  mSizer->Add(mPanel, 0, wxGROW | wxALL,
#ifdef __WXMSW__
    // don't make non-native border around palette panel
    0
#else
    5
#endif
  );
  mSizer->Layout();
  mSizer->SetSizeHints(this);
  mSizer->Fit(this);
}


#endif //_TMWXPALETTEFRAME_H_

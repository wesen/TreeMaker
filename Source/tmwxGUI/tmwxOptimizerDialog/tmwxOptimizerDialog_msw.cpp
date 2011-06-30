/*******************************************************************************
File:         tmwxOptimizerDialog_msw.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for class tmwxOptimizerDialog, specialization for MSW
Author:       TBD
Modified by:  TBD
Created:      TBD
Copyright:    TBD
*******************************************************************************/

#ifdef __WXMSW__

// Platform-specific includes
// TBD

#include "tmwxOptimizerDialog.h"
#include "wx/evtloop.h"


/**********
class tmwxOptimizerDialog
Dialog for long, cancellable calculations
**********/

/*****
Put up and show the modal dialog. This code is basically everything that happens
in a call to wxDialog::ShowModal(), up to the start of the dialog event loop.
*****/
void tmwxOptimizerDialog::DoStartModal()
{
  Show();
  mWindowDisabler = new wxWindowDisabler(this);
}


/*****
Process a single event. Subclasses that implement their own event loop must
call this routine periodically through the loop.
*****/
void tmwxOptimizerDialog::DoEventLoopOnce()
{
  if(wxEventLoopBase::GetActive()->Pending())
    wxEventLoopBase::GetActive()->Dispatch();
}


/*****
Take down the modal dialog and do cleanup. This code is basically
wxDialog::Show(false), followed by everything that happens in a call to
wxDialog::ShowModal() after the event loop.
*****/
void tmwxOptimizerDialog::DoFinishModal()
{
  if(mWindowDisabler)
  {
    delete mWindowDisabler;
    mWindowDisabler = NULL;
  }
  Hide();
}

#endif // __WXMSW__

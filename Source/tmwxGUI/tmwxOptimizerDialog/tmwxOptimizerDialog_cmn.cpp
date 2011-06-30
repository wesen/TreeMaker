/*******************************************************************************
File:         tmwxOptimizerDialog_cmn.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for class tmwxOptimizerDialog, common routines
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-22
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxOptimizerDialog.h"
#include "tmwxStr.h"
#include "tmwxStaticText.h"
#include "tmwxDoc.h"
#include "tmwxApp.h"

/* Notes.
This class, tmwxOptimizerDialog, puts up a small dialog during a
calculation that both tells the user that the calculation is going on and
allows the user to cancel the calculation. Cancellation can take place by (1)
clicking the cancel button, (2) typing the platform-standard key combination
for cancellation (ctrl-C or cmd-period).

This file contains the platform-independent routines.
*/

/**********
class tmwxOptimizerDialog
Dialog for long, cancellable calculations
**********/

/*****
Constructor. Create the dialog, add a description string and a cancel button.
*****/
tmwxOptimizerDialog::tmwxOptimizerDialog(tmwxDoc* aDoc, tmOptimizer* aOptimizer,
  const wxString& descr) :
  wxDialog(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
    TMWX_MODAL_DIALOG_STYLE | wxSTAY_ON_TOP), 
  mDoc(aDoc),
  mOptimizer(aOptimizer),
  mStatus(IN_LOOP),
  mReason(0)
#ifdef __WXMSW__
  ,mWindowDisabler(NULL)
#endif
{
  wxBoxSizer* colsizer = new wxBoxSizer(wxVERTICAL);  
  colsizer->Add(new tmwxStaticText(this, 12, descr), 
    0, wxALIGN_CENTER | wxTOP|wxLEFT|wxRIGHT, 10);
  colsizer->Add(mProgress = new tmwxStaticText(this, 9, wxT(".")), 
    0, wxALIGN_CENTER | wxTOP|wxLEFT|wxRIGHT, 10);
  colsizer->Add(CreateStdDialogButtonSizer(wxCANCEL),
    0, wxALIGN_CENTER | wxALL, 10);
    
  // We need to receive keystrokes to check for cancellation.
  SetFocus();

  SetSizerAndFit(colsizer);  
  Center();
}


/*****
Put up the dialog and run the dialog loop until a user has cancelled
*****/
int tmwxOptimizerDialog::ShowModal()
{
  DoStartModal();
  DoEventLoopModal();
  DoFinishModal();
  return mStatus;
}


/*****
OVERRIDE
This gets called if the dialog has received a cancel event (either from
hitting the button or a key combination).
*****/
void tmwxOptimizerDialog::EndModal(int)
{
  SetStatus(USER_CANCELLED);
}


/*****
This replaces the event loop of the
dialog with our own event loop that launches the calculation routine. For this
to work, we rely on the fact that the Optimize() call will periodically call
back to this dialog's UpdateUI() method, which will periodically make calls to
DoEventLoopOnce().
*****/
void tmwxOptimizerDialog::DoEventLoopModal()
{
  try {
    // Process some events to get the modal dialog up and fully displayed before
    // we do anything.
    size_t NUM_EVENTS = 5;
    for (size_t i = 0; i < NUM_EVENTS; ++i)  DoEventLoopOnce();
      
    // Initialize both watches, start the Math watch.
    mGUIWatch.Start();
    mGUIWatch.Pause();
    mMathWatch.Start();
    mOptimizer->Optimize();  // this calls UpdateUI() repeatedly
    SetStatus(NORMAL_TERMINATION);
  }
  catch(EX_USER_CANCELLED) {
    SetStatus(USER_CANCELLED);
  }
  catch(tmNLCO::EX_BAD_CONVERGENCE ebc) {
    // an EX-BAD_CONVERGENCE may provide an implementation-dependent reason for
    // failure to converge (e.g., the "inform" variable in CFSQP). We'll store
    // this in mReason so that clients can query it to give more detailed
    // information about failure to converge.
    SetStatus(OTHER_TERMINATION);
    SetReason(ebc.mReason);
  }
  catch(...) {
    // Any other exceptions, we'll still catch them. mReason will be left at
    // its initial value, which is 0.
    SetStatus(OTHER_TERMINATION);
  }
}


/*****
Signals if the dialog should finish.
*****/
void tmwxOptimizerDialog::SetStatus(int aStatus)
{
  mStatus = aStatus;
}


/*****
Provide additional information about early termination.
*****/
void tmwxOptimizerDialog::SetReason(int aReason)
{
  mReason = aReason;
}


/*****
Overrides the tmNLCOUpdater::UpdateUI() method to do screen image updating and check
for user calculation (using the tmwxOptimizerDialog class). We also compare
the time on our two timers; if the math timer has gotten more time than the GUI
timer, we let the GUI process an event; otherwise, we continue do some more
math.
*****/
void tmwxOptimizerDialog::UpdateUI()
{
  const tmFloat GUI_TIME_RATIO = 2.0;  // split between GUI time and math time
  mMathWatch.Pause();
  
  // Now both watches are paused. Compare the times to see who's gotten more
  // time since the calculation started. If math has gotten more time, then it's
  // time to do some GUI processing.
  long mathTime = mMathWatch.Time();
  long guiTime = mGUIWatch.Time();
  if (guiTime < GUI_TIME_RATIO * mathTime) {
    mGUIWatch.Resume();
    mOptimizer->DataToTree();
    mDoc->UpdateDocViews();
    mProgress->SetLabel(mProgress->GetLabel() + wxT("."));
    SetFocus();
    // Do a burst of events; enough to get the screen fully updated each time
    size_t NUM_EVENTS = 5;
    for (size_t i = 0; i < NUM_EVENTS; ++i)  DoEventLoopOnce();
    if (GetStatus() == USER_CANCELLED) 
      throw EX_USER_CANCELLED();
    mGUIWatch.Pause();
  }
  mMathWatch.Resume();
}


/*****
Respond to keys while we're the focus
*****/
void tmwxOptimizerDialog::OnKeyDown(wxKeyEvent& event)
{
  switch (event.GetKeyCode()) {
    case WXK_ESCAPE:
    case WXK_CANCEL:
      EndModal(USER_CANCELLED);
      break;
    case '.':
    case 'c':
      if (event.MetaDown()) 
        EndModal(USER_CANCELLED);
      else event.Skip();
      break;
    default:
      event.Skip();
  }
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxOptimizerDialog, wxDialog)
  EVT_KEY_DOWN(tmwxOptimizerDialog::OnKeyDown)
END_EVENT_TABLE()

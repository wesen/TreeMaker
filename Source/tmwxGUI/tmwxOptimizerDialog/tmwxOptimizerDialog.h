/*******************************************************************************
File:         tmwxOptimizerDialog.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmwxOptimizerDialog
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-22
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXOPTIMIZERDIALOG_H_
#define _TMWXOPTIMIZERDIALOG_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"
#include "tmNLCO.h"

// Forward declarations
class tmwxDoc;
class tmOptimizer;
class tmwxStaticText;


/**********
class tmwxOptimizerDialog
Dialog for long, cancellable calculations
**********/
class tmwxOptimizerDialog: public wxDialog, public tmNLCOUpdater
{
public:
  // return values
  enum {
    IN_LOOP = 0,
    NORMAL_TERMINATION = 1,
    USER_CANCELLED = 2,
    OTHER_TERMINATION = 3
  };
  class EX_USER_CANCELLED {};
  // Ctor
  tmwxOptimizerDialog(tmwxDoc* aDoc, tmOptimizer* aOptimizer, 
    const wxString& descr);
  
  // Dialog action (this is the only accessible wxDialog routine)
  int ShowModal();

  // Queries
  int GetStatus() const {return mStatus;};
  int GetReason() const {return mReason;};
protected:
  // Implementation (some is platform-specific)
  virtual void DoStartModal();
  virtual void DoEventLoopModal();
  virtual void DoEventLoopOnce();
  virtual void DoFinishModal();
  void EndModal(int retCode);
  void SetStatus(int aStatus);
  void SetReason(int aReason);
  
  // Updating
  void UpdateUI();            // override tmNLCOUpdater::UpdateUI()
private:
  tmwxDoc* mDoc;              // doc that this applies to
  tmwxStaticText* mProgress;  // to display progress
  wxStopWatch mMathWatch;     // timer for calculation
  wxStopWatch mGUIWatch;      // timer for GUI
  tmOptimizer* mOptimizer;    // object that performs the optimization
  int mStatus;                // also provides return value from ShowModal()
  int mReason;                // additional reason for OTHER_TERMINATION

  // Windows specific modal dialog disabler
#ifdef __WXMSW__
  wxWindowDisabler* mWindowDisabler;
#endif

  // Event handling
  bool AcceptsFocus() const {return true;};
  void OnKeyDown(wxKeyEvent& event);
  DECLARE_EVENT_TABLE()
};


#endif //_TMWXOPTIMIZERDIALOG_H_

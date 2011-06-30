/*******************************************************************************
File:         tmwxPersistentFrame.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmwxPersistentFrame
Author:       Robert J. Lang
Modified by:  
Created:      2005-02-19
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXPERSISTENTFRAME_H_
#define _TMWXPERSISTENTFRAME_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional wxWidgets headers
#include "wx/config.h"
#include "wx/docview.h"

// Additional TreeMaker headers
#include "tmwxApp.h"  // to pick up TM_USE_MDI

// Forward declarations
#include "tmwxGUI_fwd.h"

/*
This class implements a frame that remembers its position and size from run to
run, by storing the value in wxConfig. Subclasses implement the method
GetPositionSizeInfo(), which returns the key strings used in wxConfig and the
default values for the parameters. Subclasses should call InitPositionAndSize()
in their constructors to fetch the previous position and size from wxConfig.

The flag mPositionAndSizeInitialized is used and checked because some sizing
happens before frames are fully constructed; it prevents calls to
GetPositionSizeInfo() before the class is fully constructed.

The class is templated so that we can use either plain wxFrame or
tmwxDocParentFrame as the base class.
*/


#ifdef TM_USE_MDI
  #include "wx/docmdi.h"
  #define tmwxDocParentFrame wxDocMDIParentFrame
#else
  #define tmwxDocParentFrame wxDocParentFrame
#endif

/**********
class tmwxPersistentFrame
A wxFrame that remembers its position and size in wxConfig
**********/
template <class Frame>
class tmwxPersistentFrame: public Frame {
public:
  enum Key {X_KEY, Y_KEY, W_KEY, H_KEY};
  tmwxPersistentFrame(const wxString& title, long style);
  tmwxPersistentFrame(wxDocManager* docManager, const wxString& title, 
    long style);

  // Event handling
  void OnMove(wxMoveEvent& event);
  void OnSize(wxSizeEvent& event);
  DECLARE_EVENT_TABLE()
protected:
  virtual void InitPositionAndSize();
  virtual void GetPositionSizeInfo(Key key, wxString& keystr, int& val);
private:
  bool mPositionAndSizeInitialized;
};


/**********
Template functions
**********/

/*****
Set the position and size from the stored information. Subclasses should call
this in their constructor to set the position and size of the frame. We include
a check for window position in case the client has changed the monitor size
since the last run.
*****/
template <class Frame>
void tmwxPersistentFrame<Frame>::InitPositionAndSize()
{
  mPositionAndSizeInitialized = true;
  wxString keystr;
  int val;
  // Get previous size
  int w, h;
  GetPositionSizeInfo(W_KEY, keystr, val);
  wxConfig::Get()->Read(keystr, &w, val);
  GetPositionSizeInfo(H_KEY, keystr, val);
  wxConfig::Get()->Read(keystr, &h, val);
  this->SetSize(wxSize(w,h));
  
  // Get previous position, but don't set it quite yet...
  int x, y;
  GetPositionSizeInfo(X_KEY, keystr, val);
  wxConfig::Get()->Read(keystr, &x, val);
  GetPositionSizeInfo(Y_KEY, keystr, val);
  wxConfig::Get()->Read(keystr, &y, val);
  
  // Get client display rect because we want to insure that the window
  // remains on-screen. We'll allow windows to be partially off-screen, but at
  // least MIN_WIDTH pixels of their width needs to be visible.
  const int MIN_WIDTH = 20;
  wxRect display = wxGetClientDisplayRect();
  if (x + w < display.x + MIN_WIDTH) // too far left
    x = display.x + MIN_WIDTH - w;
   if (x > display.x + display.width - MIN_WIDTH) // too far right
    x = display.x + display.width - MIN_WIDTH;
  if (y < display.y) // too far up
    y = display.y;
  if (y > display.y + display.height - MIN_WIDTH) // too far down
    y = display.y + display.height - MIN_WIDTH;
  
  // Position check finished; now move to the new position.
  this->Move(wxPoint(x, y));
}


/*****
Subclasses should override this, returning a unique key string and default
value for each of the four key values.
*****/
template <class Frame>
void tmwxPersistentFrame<Frame>::GetPositionSizeInfo(Key, wxString&, int&)
{
  TMFAIL("tmwxPersistentFrame<Frame>::GetPositionSizeInfo() must be overridden");
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Respond to move event. Store the position of the frame in wxConfig.
*****/
template <class Frame>
void tmwxPersistentFrame<Frame>::OnMove(wxMoveEvent& event)
{
  event.Skip();
  if (!mPositionAndSizeInitialized) return;
  int x, y;
  this->GetPosition(&x, &y);
  wxString keystr;
  int val;
  GetPositionSizeInfo(X_KEY, keystr, val);
  wxConfig::Get()->Write(keystr, x);
  GetPositionSizeInfo(Y_KEY, keystr, val);
  wxConfig::Get()->Write(keystr, y);
  wxConfig::Get()->Flush();
}


/*****
Respond to size event. Store the size of the frame in wxConfig.
*****/
template <class Frame>
void tmwxPersistentFrame<Frame>::OnSize(wxSizeEvent& event)
{
  event.Skip();
  if (!mPositionAndSizeInitialized) return;
  int w, h;
  this->GetSize(&w, &h);
  wxString keystr;
  int val;
  GetPositionSizeInfo(W_KEY, keystr, val);
  wxConfig::Get()->Write(keystr, w);
  GetPositionSizeInfo(H_KEY, keystr, val);
  wxConfig::Get()->Write(keystr, h);
  wxConfig::Get()->Flush();
}


#endif //_TMWXPERSISTENTFRAME_H_

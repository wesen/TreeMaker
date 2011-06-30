/*******************************************************************************
File:         tmwxDesignFrame.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmwxDesignFrame
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDesignFrame.h"
#include "tmModel.h"
#include "tmwxDoc.h"
#include "tmwxView.h"
#include "tmwxDesignCanvas.h"
#include "tmwxApp.h"
#include "tmwxInspectorFrame.h"
#include "tmwxViewSettingsFrame.h"
#include "tmwxStr.h"
#include "tmwxDocManager.h"

/*****
Constructor
*****/
tmwxDesignFrame::tmwxDesignFrame(tmwxDoc* aDoc, tmwxView* aView, 
  tmwxDocParentFrame* parent) : 
  tmwxDocChildFrame(aDoc, aView, parent, wxID_ANY, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
  mDoc(aDoc),
  mView(aView),
  mDesignCanvas(NULL)
{
#if defined(__LINUX__) || defined(__WXMSW__)
  SetIcon (gApp->GetDocIcon ());
#endif

  // Create the standard menus
  tmwxApp::MakeMenuBar(this, mDoc);
  
  // create a status bar
  CreateStatusBar(1);

  // Create a canvas and put it into the frame.
  int width, height;
  GetClientSize(&width, &height);
  mDesignCanvas = new tmwxDesignCanvas(this, wxSize(width, height));
  
  // Set paper size to the largest value that fits the window within the screen.
  mDesignCanvas->FitPaperSize();

#if wxUSE_DRAG_AND_DROP
  mDesignCanvas->SetDropTarget(new tmwxDropTarget());
  SetDropTarget(new tmwxDropTarget());
#endif // wxUSE_DRAG_AND_DROP
}


/*****
Respond to activation events. We let the framework handle most of it, but we
also need to update our floating windows with the currently-active palette.
*****/
void tmwxDesignFrame::OnActivate(wxActivateEvent& event)
{
  tmwxDocChildFrame::OnActivate(event); // let base class finish handling the event.

  if (event.GetActive()) {
    // Make sure the application now knows we're the top window in order to
    // update the menu bar properly in the presence of floating windows.
    wxTheApp->SetTopWindow(this);

    // Tell the inspector and view settings inspectors to show the parameters
    // of the top document window, which is us. 
    gApp->SetPalettes(mDoc);
  }
}


/*****
Respond to attempt to close a window.
*****/
void tmwxDesignFrame::OnClose(wxCloseEvent& event)
{
  // Clear the palettes
  gApp->SetPalettes();
  // Continue processing the event
  event.Skip();
  // Let document manager have a chance at quitting the app if this is the
  // last document.
  gDocManager->CheckLastDoc();
}


/*****
Event table
*****/

BEGIN_EVENT_TABLE(tmwxDesignFrame, tmwxDocChildFrame)
  EVT_ACTIVATE(tmwxDesignFrame::OnActivate)
  EVT_CLOSE(tmwxDesignFrame::OnClose)
END_EVENT_TABLE()


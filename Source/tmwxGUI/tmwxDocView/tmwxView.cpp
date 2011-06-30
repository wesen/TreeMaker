/*******************************************************************************
File:         tmwxView.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker view class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxView.h"
#include "tmModel.h"
#include "tmwxInspectorFrame.h"
#include "tmwxDoc.h"
#include "tmwxDesignFrame.h"
#include "tmwxDesignCanvas.h"
#include "tmwxStr.h"
#include "tmwxPrintout.h"

/**********
class tmwxView
**********/

/*****
Constructor. Member pointers will get assigned in OnCreate() method.
*****/
tmwxView::tmwxView()
  :mDoc(NULL), mDesignFrame(NULL), mDesignCanvas(NULL)
{
}


/*****
Destructor
*****/
tmwxView::~tmwxView()
{
  // Set the inspector to show nothing
  gInspectorFrame->SetSelection();
}


/*****
Return a ptr to the tree.
*****/
tmTree* const tmwxView::GetTree() const 
{
  return mDoc->mTree;
}


/*****
Return our printout object
*****/
wxPrintout* tmwxView::OnCreatePrintout()
{
  return new tmwxPrintout(this);
}


/*****
What to do when a view is created. Creates frame(s) for displaying the view.
*****/
bool tmwxView::OnCreate(wxDocument *doc, long WXUNUSED(flags))
{
  // Get the tmwxDoc that owns this view
  mDoc = wxDynamicCast(doc, tmwxDoc);
  wxASSERT(mDoc);

  // Create the design frame, which is the official document child frame
  mDesignFrame = new tmwxDesignFrame(mDoc, this, gInspectorFrame);
  mDesignCanvas = mDesignFrame->GetDesignCanvas();
  
  // Set tmwxDoc's member variable
  mDoc->mView = this;

  // Note that frame is still invisible; we'll only show it after we know that
  // we've got something valid in it.
  return true;
}


/*****
Gets used for default print/preview.
We use the contents of the design frame for printing.
*****/
void tmwxView::OnDraw(wxDC* dc)
{
  GetDesignCanvas()->OnDraw(*dc);
}


/*****
Update the view (e.g., after a change or revert).
Refresh() forces a complete redraw.
*****/
void tmwxView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
  if (mDesignFrame)
    mDesignFrame->Refresh();
}


/*****
Clean up windows used for displaying the view.
*****/
bool tmwxView::OnClose(bool deleteWindow)
{
  if (!GetDocument()->Close())
    return FALSE;
  
  wxString s(wxTheApp->GetAppName());
  if (mDesignFrame)
    mDesignFrame->SetTitle(s);
  
  SetFrame((wxFrame *) NULL);
  
  Activate(FALSE);
  
  if (deleteWindow)
  {
    delete mDesignFrame;
    return true;
  }
  return true;
}


/*****
Declare dynamically creatable -- required by app
*****/
IMPLEMENT_DYNAMIC_CLASS(tmwxView, wxView)


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxView, wxView)
END_EVENT_TABLE()

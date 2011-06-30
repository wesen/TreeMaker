/*******************************************************************************
File:         tmwxDoc_Debug.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker document class, Debug menu commands
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-10
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDoc.h"
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxInspectorFrame.h"
#ifdef TMDEBUG
#include "tmwxLogFrame.h"
#endif // TMDEBUG
#include "tmwxDocManager.h"
#include "tmwxDesignFrame.h"
#include "tmwxDesignCanvas.h"
#include "tmwxView.h"

using namespace std;

/**********
class tmwxDoc
TreeMaker document class
**********/

#ifdef TMDEBUG
/*****
Enable Debug->Quit Cleanup Early
*****/
void tmwxDoc::OnToggleQuitCleanupEarlyUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
  event.Check(mTree->GetQuitCleanupEarly());
}


/*****
Perform Debug->Quit Cleanup Early
*****/
void tmwxDoc::OnToggleQuitCleanupEarly(wxCommandEvent&)
{
  mTree->SetQuitCleanupEarly(!mTree->GetQuitCleanupEarly());
}


/*****
Enable Debug->Make Tree Blank
*****/
void tmwxDoc::OnMakeTreeBlankUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
}


/*****
Perform Debug->Make Tree Blank
*****/
void tmwxDoc::OnMakeTreeBlank(wxCommandEvent&)
{
  DoReplaceTree(tmTree::MakeTreeBlank());
}


/*****
Enable Debug->Make Tree Unoptimized
*****/
void tmwxDoc::OnMakeTreeUnoptimizedUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
}


/*****
Perform Debug->Make Tree Unoptimized
*****/
void tmwxDoc::OnMakeTreeUnoptimized(wxCommandEvent&)
{
  DoReplaceTree(tmTree::MakeTreeUnoptimized());
}


/*****
Enable Debug->Make Tree Optimized
*****/
void tmwxDoc::OnMakeTreeOptimizedUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
}


/*****
Perform Debug->Make Tree Optimized
*****/
void tmwxDoc::OnMakeTreeOptimized(wxCommandEvent&)
{
  DoReplaceTree(tmTree::MakeTreeOptimized());
}


/*****
Enable Debug->Make Tree Gusset
*****/
void tmwxDoc::OnMakeTreeGussetUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
}


/*****
Perform Debug->Make Tree Gusset
*****/
void tmwxDoc::OnMakeTreeGusset(wxCommandEvent&)
{
  DoReplaceTree(tmTree::MakeTreeGusset());
}


/*****
Enable Debug->Make Tree Conditioned
*****/
void tmwxDoc::OnMakeTreeConditionedUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
}


/*****
Perform Debug->Make Tree Conditioned
*****/
void tmwxDoc::OnMakeTreeConditioned(wxCommandEvent&)
{
  DoReplaceTree(tmTree::MakeTreeConditioned());
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Delete the old tree and replace it with this new one; also a little bookkeeping
to make sure the inspector panel is appropriately updated.
*****/
void tmwxDoc::DoReplaceTree(tmTree* newTree)
{
  if (mTree) delete mTree;
  mTree = newTree;
  gInspectorFrame->SetSelection((tmPart*)newTree);
  SubmitCommand(wxT("Replace Tree"));
}
#endif // TMDEBUG

/*******************************************************************************
File:         tmwxDoc.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker document class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDoc.h"
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxView.h"
#include "tmwxInspectorFrame.h"
#include "tmwxViewSettingsFrame.h"
#include "tmwxFoldedFormFrame.h"
#include "tmwxDesignFrame.h"
#include "tmwxDesignCanvas.h"
#include "tmwxApp.h"
#include "tmwxCommand.h"
#include "tmwxDocManager.h"

// Standard libraries
#include <fstream>

using namespace std;

/**********
class tmwxDoc
TreeMaker document class
**********/

/*****
Constructor
*****/
tmwxDoc::tmwxDoc() : 
  mTree(NULL),
  mView(NULL)
{
  // Create the tree and make a serialized copy for reference.
  mTree = tmTree::MakeTreeBlank();
  mTree->PutSelf(mCleanState);
}


/*****
Destructor
*****/
tmwxDoc::~tmwxDoc(void)
{
  if (mTree) delete mTree;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
If exactly one node is selected (and nothing else) return a pointer to the
node, NULL otherwise.
*****/
tmNode* tmwxDoc::ExactlyOneNodeSelected()
{
  if (mSelection.GetNumAllParts() != 1) return NULL;
  if (mSelection.GetNumParts<tmNode>() != 1) return NULL;
  return mSelection.GetNodes().front();
}


/*****
Remove all items from the current selection AND update the views to show it.
*****/
void tmwxDoc::ClearSelection()
{
  mSelection.ClearAllParts();
  UpdateAllViews();
}


/*****
This is a little utility that filters the current selection into a list of
paths that potentially get a condition applied to them. It transfers any
selected leaf paths to the list from the selection. Alternatively, if the user
has selected exactly two leaf nodes, then we'll return the single leaf path
that connects those two leaf nodes.
*****/
void tmwxDoc::GetLeafPathsFromSelection(tmArray<tmPath*>& leafPaths)
{
  leafPaths.clear();
  tmPath::FilterLeafPaths(leafPaths, mSelection.GetPaths());
  if (leafPaths.empty()) {
    tmArray<tmNode*> leafNodes;
    tmNode::FilterLeafNodes(leafNodes, mSelection.GetNodes());
    if (leafNodes.size() == 2) 
      leafPaths.push_back(mTree->GetLeafPath(leafNodes[0], leafNodes[1]));
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
The document (tree) has been changed. Submit a command with the given name.
Note that Submit also calls UpdateAllViews().
*****/
void tmwxDoc::SubmitCommand(const wxString& name)
{
  GetCommandProcessor()->Submit(new tmwxCommand(name, this));
  GetDesignCanvas()->SetFocus();
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
OVERRIDE
Make this a new document.
*****/
bool tmwxDoc::OnNewDocument()
{
  bool ret = wxDocument::OnNewDocument();
  if (ret) {
    GetDesignFrame()->Show(true);
  }
  return ret;
}


/*****
Read a tree in from a file. This is called by both OnOpenDocument() and by
Revert(). Return true if we were successful. If not, return false, and leave
the tree in its original state.
*****/
bool tmwxDoc::ReadTreeFromFile(const wxString& file)
{
  TMASSERT(!wxTheApp->GetAppName().empty());
  wxString msgTitle(wxT("File Read Error"));

  wxSTD ifstream fin(file.mb_str(), ios_base::binary);
  if (fin.fail() || fin.bad()) {
    wxString msg = wxT("Sorry, could not open file ");
    msg += file;
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  try {
    LoadObject(fin);
  }
  catch (tmTree::EX_IO_BAD_TREE_TAG exc) {
    wxString strToken = wxString::FromAscii(exc.mToken.c_str());
    wxString msg = wxString::Format(
      wxT("Sorry, this file doesn't look like a TreeMaker file: "\
      "(initial tag = %s)"), strToken.c_str());
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  catch (tmTree::EX_IO_BAD_TREE_VERSION exc) {
    wxString strToken = wxString::FromAscii(exc.mToken.c_str());
    wxString msg = wxString::Format(
      wxT("Sorry, this file has an unrecognized (possibly newer) version "\
      "than this program can read: (version = %s)"), strToken.c_str());
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  catch (tmTree::EX_IO_BAD_REF_INDEX exc) {
    wxString strToken = wxString::FromAscii(exc.mToken.c_str());
    wxString msg = wxString::Format(
      wxT("Sorry, this file appears to be corrupted (bad reference index "\
      "= %s)"), strToken.c_str());
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  catch (tmTree::EX_IO_BAD_TAG exc ) {
    wxString strToken = wxString::FromAscii(exc.mToken.c_str());
    wxString msg = wxString::Format(
      wxT("Sorry, this file appears to be corrupted (bad object tag size: "\
      "tag = %s)"), strToken.c_str());
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  catch (tmTree::EX_IO_UNRECOGNIZED_TAG exc) {
    wxString strToken = wxString::FromAscii(exc.mToken.c_str());
    wxString msg = wxString::Format(
      wxT("Sorry, this file appears to be corrupted (unrecognized object tag: "\
      "tag = %s)"), strToken.c_str());
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  catch (tmTree::EX_IO_TOO_LONG_STRING exc) {
    wxString strToken = wxString::FromAscii(exc.mToken.c_str());
    wxString msg = wxString::Format(
      wxT("Sorry, text label %s in this file was too long"), 
      strToken.c_str());
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  catch (tmTree::EX_IO_BAD_ESCAPE exc) {
    wxString strToken = wxString::FromAscii(exc.mToken.c_str());
    wxString msg = wxString::Format(
      wxT("Sorry, a text label contained a bad escape sequence: "\
      "(%s)"), strToken.c_str());
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  catch (tmTree::EX_IO_BAD_TOKEN exc) {
    wxString strToken = wxString::FromAscii(exc.mToken.c_str());
    wxString msg = wxString::Format(
      wxT("Sorry, a bad token (%s) was found. File corrupted."), 
      strToken.c_str());
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  catch(...) {
    wxString msg = wxT("Sorry, an unhandled exception was thrown. Please")
      wxT("report this bug to the TreeMaker development team.");
    tmwxAlertError(msg, msgTitle, GetDocumentWindow());
    return false;
  }
  return true;
}


/*****
Open a document. This is the same as wxDocument::OnOpenDocument() with the
addition that we catch exceptions thrown by the model as we read in the
document from a file.
*****/
bool tmwxDoc::OnOpenDocument(const wxString& file)
{
  if (!OnSaveModified()) return false;
  if (!ReadTreeFromFile(file)) return false;
  SetFilename(file, true);
  Modify(false);
  m_savedYet = true;
  GetDesignFrame()->Show(true);
  UpdateAllViews();
  return true;
}


/*****
Close the document. In addition to the default behavior, we also check
to see if we're the last document to be closed and if we are,
we blank the view settings palette and the folded form frame.
*****/
bool tmwxDoc::OnCloseDocument()
{
  gDocManager->SetCurrentDocumentLocal(NULL); // clear our private copy
  bool ret = wxDocument::OnCloseDocument();
  size_t num_docs = gDocManager->GetDocuments().GetCount();
  if (num_docs == 1) {
    gViewSettingsFrame->SetSelection();
    gFoldedFormFrame->SetDoc(0);
  }
  return ret;
}


/*****
Revert to the last saved version.
*****/
bool tmwxDoc::Revert()
{
  if (! IsModified ())
    return false; // or true?
  wxString fname = GetFilename();
  TMASSERT(fname != wxEmptyString);
  if (!ReadTreeFromFile(fname)) {
    tmwxAlertError(fname + wxT(" could not be used to revert the tree"),
      wxT("File Revert Error"));
    return false;
  }
  SubmitCommand(wxT("Revert"));
  Modify (false);
  return true;
}


/*****
Put the object to a (file) stream.
*****/
tmwxDoc::tmwxOutputStream& tmwxDoc::SaveObject(tmwxOutputStream& stream)
{
  wxDocument::SaveObject(stream);
  mTree->PutSelf(stream);
  return stream;
}


/*****
Read the object from a (file) stream.
*****/
tmwxDoc::tmwxInputStream& tmwxDoc::LoadObject(tmwxInputStream& stream)
{
  wxDocument::LoadObject(stream);
  tmTree* theTree = new tmTree();
  try {
    theTree->GetSelf(stream);
  }
  catch (tmTree::EX_IO_UNRECOGNIZED_CONDITION exc) {
    // This is a mild error: we encountered some unrecognized conditions. This
    // is not fatal -- we've already ignored the offenders -- but we'll let the
    // user know what happened.
    wxString msg;
    wxString msgTitle = wxTheApp->GetAppName();
    if (exc.mNumMissed == 1)
      msg.Printf(
        wxT("There was one unrecognized condition. It will be ignored."));
    else
      msg.Printf(
        wxT("There were %s unrecognized conditions. They will be ignored."), 
          tmwxStr(exc.mNumMissed).c_str());
    tmwxAlertInfo(msg, msgTitle, GetDocumentWindow());
    // continue as if we were successful.
  }
  // Note: GetSelf() could throw other exceptions if the stream contained
  // corrupted data. If that happens, theTree can be in an unspecified
  // partially-built state. If that occurs, we'll abandon it. This creates a
  // memory leak, but at the moment, the Put/Get routines don't clean up memory
  // when they throw an exception. Sometime in the future we'll have to rewrite
  // all of the Put/Get routines so that they clean up properly. But if no
  // exceptions were thrown, we can safely replace the existing tree with the
  // read-in one.
  delete mTree;
  mTree = theTree;
  mCleanState.str("");
  mTree->PutSelf(mCleanState);
  return stream;
}


/*****
Override to update the contents of the document views AND the tool palettes.
*****/
void tmwxDoc::UpdateAllViews(wxView* sender, wxObject* hint)
{
  // Ancestor method takes care of updating the design frame.
  wxDocument::UpdateAllViews(sender, hint);
#ifdef __WXGTK__
  /* In wxGTK (and according to wx documents, wxWin) 2.6.2, wxUpdateUIEvents
     are not handled at any idle time; e.g., menu update events are only
     processed when the menu is posted, but not when a key event
     is processed. Therefore, the following is possible:
     - a menu M is opened; UI updates are handled, disabling an item and its
     associated hotkey
     - the hotkey remains disabled even after commands (other than opening M)
     change the context logically enabling it
     - reopening M (even without activating any item) magically enables the
     hotkey
     A simple case is <scale everything>; Control-4; Control-Alt-4. The latter
     key is disabled if the first action is done via menu.

     This is a quick & dirty fix: forcing UI update handling anytime a view
     changes.
   */
  GetView()->GetDesignFrame()->UpdateWindowUI();
#endif
  
  // Also update the floating window palettes.
  gApp->SetPalettes(this);
  
  // Also update the scale in the status bar
  gDocManager->GetCurrentViewLocal()->GetDesignFrame()->SetStatusText(
    wxString::Format(wxT("Scale = %.4f"), mTree->GetScale()));
}


/*****
Update only the document views, but not the tool palettes. This is used to only
update the document views during an optimization, but (for speed) we don't
update the tool palettes until we're all done with the optimization.
*****/
void tmwxDoc::UpdateDocViews()
{
  // Only call the ancestor method, not our overridden method, so that we don't
  // bother with updating palettes.
  wxDocument::UpdateAllViews();
}


/*****
Dynamic class implementation
*****/
IMPLEMENT_DYNAMIC_CLASS(tmwxDoc, wxDocument)


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxDoc, wxDocument)
  // File menu
  EVT_UPDATE_UI(tmwxID_EXPORT_V4, tmwxDoc::OnExportv4UpdateUI)
  EVT_MENU(tmwxID_EXPORT_V4, tmwxDoc::OnExportv4)
  
  // Edit menu
  EVT_UPDATE_UI(wxID_CUT, tmwxDoc::OnCutUpdateUI)
  EVT_MENU(wxID_CUT, tmwxDoc::OnCut)
  EVT_UPDATE_UI(wxID_COPY, tmwxDoc::OnCopyUpdateUI)
  EVT_MENU(wxID_COPY, tmwxDoc::OnCopy)
  EVT_UPDATE_UI(wxID_PASTE, tmwxDoc::OnPasteUpdateUI)
  EVT_MENU(wxID_PASTE, tmwxDoc::OnPaste)
  EVT_UPDATE_UI(wxID_CLEAR, tmwxDoc::OnClearUpdateUI)
  EVT_MENU(wxID_CLEAR, tmwxDoc::OnClear)
  // Edit->Select menu
  EVT_UPDATE_UI(wxID_SELECTALL, tmwxDoc::OnSelectAllUpdateUI)
  EVT_MENU(wxID_SELECTALL, tmwxDoc::OnSelectAll)
  EVT_UPDATE_UI(tmwxID_SELECT_NONE, tmwxDoc::OnSelectNoneUpdateUI)
  EVT_MENU(tmwxID_SELECT_NONE, tmwxDoc::OnSelectNone)
  EVT_UPDATE_UI(tmwxID_SELECT_PART_BY_INDEX, tmwxDoc::OnSelectPartByIndexUpdateUI)
  EVT_MENU(tmwxID_SELECT_PART_BY_INDEX, tmwxDoc::OnSelectPartByIndex)
  EVT_UPDATE_UI(tmwxID_SELECT_MOVABLE_PARTS, tmwxDoc::OnSelectMovablePartsUpdateUI)
  EVT_MENU(tmwxID_SELECT_MOVABLE_PARTS, tmwxDoc::OnSelectMovableParts)
  EVT_UPDATE_UI(tmwxID_SELECT_PATH_FROM_NODES, tmwxDoc::OnSelectPathFromNodesUpdateUI)
  EVT_MENU(tmwxID_SELECT_PATH_FROM_NODES, tmwxDoc::OnSelectPathFromNodes)
  EVT_UPDATE_UI(tmwxID_SELECT_CORRIDOR_FACETS, tmwxDoc::OnSelectCorridorFacetsUpdateUI)
  EVT_MENU(tmwxID_SELECT_CORRIDOR_FACETS, tmwxDoc::OnSelectCorridorFacets)
  // Edit->Nodes menu
  EVT_UPDATE_UI(tmwxID_MAKE_NODE_ROOT, tmwxDoc::OnMakeNodeRootUpdateUI)
  EVT_MENU(tmwxID_MAKE_NODE_ROOT, tmwxDoc::OnMakeNodeRoot)
  EVT_UPDATE_UI(tmwxID_PERTURB_SELECTED_NODES, tmwxDoc::OnPerturbSelectedNodesUpdateUI)
  EVT_MENU(tmwxID_PERTURB_SELECTED_NODES, tmwxDoc::OnPerturbSelectedNodes)
  EVT_UPDATE_UI(tmwxID_PERTURB_ALL_NODES, tmwxDoc::OnPerturbAllNodesUpdateUI)
  EVT_MENU(tmwxID_PERTURB_ALL_NODES, tmwxDoc::OnPerturbAllNodes)
  // Edit->Edges menu
  EVT_UPDATE_UI(tmwxID_SET_EDGE_LENGTH, tmwxDoc::OnSetEdgeLengthUpdateUI)
  EVT_MENU(tmwxID_SET_EDGE_LENGTH, tmwxDoc::OnSetEdgeLength)
  EVT_UPDATE_UI(tmwxID_SCALE_EDGE_LENGTH, tmwxDoc::OnScaleEdgeLengthUpdateUI)
  EVT_MENU(tmwxID_SCALE_EDGE_LENGTH, tmwxDoc::OnScaleEdgeLength)
  EVT_UPDATE_UI(tmwxID_RENORMALIZE_TO_EDGE, tmwxDoc::OnRenormalizeToEdgeUpdateUI)
  EVT_MENU(tmwxID_RENORMALIZE_TO_EDGE, tmwxDoc::OnRenormalizeToEdge)
  EVT_UPDATE_UI(tmwxID_RENORMALIZE_TO_UNIT_SCALE, tmwxDoc::OnRenormalizeToUnitScaleUpdateUI)
  EVT_MENU(tmwxID_RENORMALIZE_TO_UNIT_SCALE, tmwxDoc::OnRenormalizeToUnitScale)
  // Edit->Absorb menu
  EVT_UPDATE_UI(tmwxID_ABSORB_SELECTED_NODES, tmwxDoc::OnAbsorbSelectedNodesUpdateUI)
  EVT_MENU(tmwxID_ABSORB_SELECTED_NODES, tmwxDoc::OnAbsorbSelectedNodes)
  EVT_UPDATE_UI(tmwxID_ABSORB_REDUNDANT_NODES, tmwxDoc::OnAbsorbRedundantNodesUpdateUI)
  EVT_MENU(tmwxID_ABSORB_REDUNDANT_NODES, tmwxDoc::OnAbsorbRedundantNodes)
  EVT_UPDATE_UI(tmwxID_ABSORB_SELECTED_EDGES, tmwxDoc::OnAbsorbSelectedEdgesUpdateUI)
  EVT_MENU(tmwxID_ABSORB_SELECTED_EDGES, tmwxDoc::OnAbsorbSelectedEdges)
  // Edit->Split menu
  EVT_UPDATE_UI(tmwxID_SPLIT_SELECTED_EDGE, tmwxDoc::OnSplitSelectedEdgeUpdateUI)
  EVT_MENU(tmwxID_SPLIT_SELECTED_EDGE, tmwxDoc::OnSplitSelectedEdge)
  // Edit->Strain menu
  EVT_UPDATE_UI(tmwxID_REMOVE_SELECTION_STRAIN, tmwxDoc::OnRemoveSelectionStrainUpdateUI)
  EVT_MENU(tmwxID_REMOVE_SELECTION_STRAIN, tmwxDoc::OnRemoveSelectionStrain)
  EVT_UPDATE_UI(tmwxID_REMOVE_ALL_STRAIN, tmwxDoc::OnRemoveAllStrainUpdateUI)
  EVT_MENU(tmwxID_REMOVE_ALL_STRAIN, tmwxDoc::OnRemoveAllStrain)
  EVT_UPDATE_UI(tmwxID_RELIEVE_SELECTION_STRAIN, tmwxDoc::OnRelieveSelectionStrainUpdateUI)
  EVT_MENU(tmwxID_RELIEVE_SELECTION_STRAIN, tmwxDoc::OnRelieveSelectionStrain)
  EVT_UPDATE_UI(tmwxID_RELIEVE_ALL_STRAIN, tmwxDoc::OnRelieveAllStrainUpdateUI)
  EVT_MENU(tmwxID_RELIEVE_ALL_STRAIN, tmwxDoc::OnRelieveAllStrain)
  // Edit->Stub menu
  EVT_UPDATE_UI(tmwxID_PICK_STUB_NODES, tmwxDoc::OnPickStubNodesUpdateUI)
  EVT_MENU(tmwxID_PICK_STUB_NODES, tmwxDoc::OnPickStubNodes)
  EVT_UPDATE_UI(tmwxID_PICK_STUB_POLY, tmwxDoc::OnPickStubPolyUpdateUI)
  EVT_MENU(tmwxID_PICK_STUB_POLY, tmwxDoc::OnPickStubPoly)
  EVT_UPDATE_UI(tmwxID_ADD_LARGEST_STUB_NODES, tmwxDoc::OnAddLargestStubNodesUpdateUI)
  EVT_MENU(tmwxID_ADD_LARGEST_STUB_NODES, tmwxDoc::OnAddLargestStubNodes)
  EVT_UPDATE_UI(tmwxID_ADD_LARGEST_STUB_POLY, tmwxDoc::OnAddLargestStubPolyUpdateUI)
  EVT_MENU(tmwxID_ADD_LARGEST_STUB_POLY, tmwxDoc::OnAddLargestStubPoly)
  EVT_UPDATE_UI(tmwxID_TRIANGULATE_TREE, tmwxDoc::OnTriangulateTreeUpdateUI)
  EVT_MENU(tmwxID_TRIANGULATE_TREE, tmwxDoc::OnTriangulateTree)

  // View Menu
  EVT_UPDATE_UI(tmwxID_DEFAULT_VIEW, tmwxDoc::OnDefaultViewUpdateUI)
  EVT_MENU(tmwxID_DEFAULT_VIEW, tmwxDoc::OnDefaultView)
  EVT_UPDATE_UI(tmwxID_TREE_VIEW, tmwxDoc::OnTreeViewUpdateUI)
  EVT_MENU(tmwxID_TREE_VIEW, tmwxDoc::OnTreeView)
  EVT_UPDATE_UI(tmwxID_CREASES_VIEW, tmwxDoc::OnCreasesViewUpdateUI)
  EVT_MENU(tmwxID_CREASES_VIEW, tmwxDoc::OnCreasesView)
  EVT_UPDATE_UI(tmwxID_PLAN_VIEW, tmwxDoc::OnPlanViewUpdateUI)
  EVT_MENU(tmwxID_PLAN_VIEW, tmwxDoc::OnPlanView)
  EVT_UPDATE_UI(tmwxID_FIT_TO_SCREEN, tmwxDoc::OnFitToScreenUpdateUI)
  EVT_MENU(tmwxID_FIT_TO_SCREEN, tmwxDoc::OnFitToScreen)
  EVT_UPDATE_UI(tmwxID_FIT_TO_WIDTH, tmwxDoc::OnFitToWidthUpdateUI)
  EVT_MENU(tmwxID_FIT_TO_WIDTH, tmwxDoc::OnFitToWidth)
  EVT_UPDATE_UI(tmwxID_FIT_TO_HEIGHT, tmwxDoc::OnFitToHeightUpdateUI)
  EVT_MENU(tmwxID_FIT_TO_HEIGHT, tmwxDoc::OnFitToHeight)
  EVT_UPDATE_UI(tmwxID_SET_PAPER_SIZE, tmwxDoc::OnSetPaperSizeUpdateUI)
  EVT_MENU(tmwxID_SET_PAPER_SIZE, tmwxDoc::OnSetPaperSize)

  // Action menu
  EVT_UPDATE_UI(tmwxID_SCALE_EVERYTHING, tmwxDoc::OnScaleEverythingUpdateUI)
  EVT_MENU(tmwxID_SCALE_EVERYTHING, tmwxDoc::OnScaleEverything)
  EVT_UPDATE_UI(tmwxID_SCALE_SELECTION, tmwxDoc::OnScaleSelectionUpdateUI)
  EVT_MENU(tmwxID_SCALE_SELECTION, tmwxDoc::OnScaleSelection)
  EVT_UPDATE_UI(tmwxID_MINIMIZE_STRAIN, tmwxDoc::OnMinimizeStrainUpdateUI)
  EVT_MENU(tmwxID_MINIMIZE_STRAIN, tmwxDoc::OnMinimizeStrain)
  EVT_UPDATE_UI(tmwxID_BUILD_CREASE_PATTERN, tmwxDoc::OnBuildCreasePatternUpdateUI)
  EVT_MENU(tmwxID_BUILD_CREASE_PATTERN, tmwxDoc::OnBuildCreasePattern)
  EVT_UPDATE_UI(tmwxID_KILL_CREASE_PATTERN, tmwxDoc::OnKillCreasePatternUpdateUI)
  EVT_MENU(tmwxID_KILL_CREASE_PATTERN, tmwxDoc::OnKillCreasePattern)
  
  // Condition menu
  EVT_UPDATE_UI(tmwxID_NODE_FIXED_TO_SYMMETRY_LINE, tmwxDoc::OnNodeFixedToSymmetryLineUpdateUI)
  EVT_MENU(tmwxID_NODE_FIXED_TO_SYMMETRY_LINE, tmwxDoc::OnNodeFixedToSymmetryLine)
  EVT_UPDATE_UI(tmwxID_NODE_FIXED_TO_PAPER_EDGE, tmwxDoc::OnNodeFixedToPaperEdgeUpdateUI)
  EVT_MENU(tmwxID_NODE_FIXED_TO_PAPER_EDGE, tmwxDoc::OnNodeFixedToPaperEdge)
  EVT_UPDATE_UI(tmwxID_NODE_FIXED_TO_CORNER, tmwxDoc::OnNodeFixedToCornerUpdateUI)
  EVT_MENU(tmwxID_NODE_FIXED_TO_CORNER, tmwxDoc::OnNodeFixedToCorner)
  EVT_UPDATE_UI(tmwxID_NODE_FIXED_TO_POSITION, tmwxDoc::OnNodeFixedToPositionUpdateUI)
  EVT_MENU(tmwxID_NODE_FIXED_TO_POSITION, tmwxDoc::OnNodeFixedToPosition)
#ifdef TMDEBUG
  EVT_UPDATE_UI(tmwxID_NODE_FIXED_TO_SYMMETRY_LINE_V4, tmwxDoc::OnNodeFixedToSymmetryLinev4UpdateUI)
  EVT_MENU(tmwxID_NODE_FIXED_TO_SYMMETRY_LINE_V4, tmwxDoc::OnNodeFixedToSymmetryLinev4)
  EVT_UPDATE_UI(tmwxID_NODE_FIXED_TO_PAPER_EDGE_V4, tmwxDoc::OnNodeFixedToPaperEdgev4UpdateUI)
  EVT_MENU(tmwxID_NODE_FIXED_TO_PAPER_EDGE_V4, tmwxDoc::OnNodeFixedToPaperEdgev4)
  EVT_UPDATE_UI(tmwxID_NODE_FIXED_TO_CORNER_V4, tmwxDoc::OnNodeFixedToCornerv4UpdateUI)
  EVT_MENU(tmwxID_NODE_FIXED_TO_CORNER_V4, tmwxDoc::OnNodeFixedToCornerv4)
  EVT_UPDATE_UI(tmwxID_NODE_FIXED_TO_POSITION_V4, tmwxDoc::OnNodeFixedToPositionv4UpdateUI)
  EVT_MENU(tmwxID_NODE_FIXED_TO_POSITION_V4, tmwxDoc::OnNodeFixedToPositionv4)
#endif // TMDEBUG
  EVT_UPDATE_UI(tmwxID_NODES_PAIRED_ABOUT_SYMMETRY_LINE, tmwxDoc::OnNodesPairedAboutSymmetryLineUpdateUI)
  EVT_MENU(tmwxID_NODES_PAIRED_ABOUT_SYMMETRY_LINE, tmwxDoc::OnNodesPairedAboutSymmetryLine)
  EVT_UPDATE_UI(tmwxID_NODES_COLLINEAR, tmwxDoc::OnNodesCollinearUpdateUI)
  EVT_MENU(tmwxID_NODES_COLLINEAR, tmwxDoc::OnNodesCollinear)
  EVT_UPDATE_UI(tmwxID_REMOVE_NODE_CONDITIONS, tmwxDoc::OnRemoveNodeConditionsUpdateUI)
  EVT_MENU(tmwxID_REMOVE_NODE_CONDITIONS, tmwxDoc::OnRemoveNodeConditions)
  EVT_UPDATE_UI(tmwxID_EDGE_LENGTH_FIXED, tmwxDoc::OnEdgeLengthFixedUpdateUI)
  EVT_MENU(tmwxID_EDGE_LENGTH_FIXED, tmwxDoc::OnEdgeLengthFixed)
  EVT_UPDATE_UI(tmwxID_EDGES_SAME_STRAIN, tmwxDoc::OnEdgesSameStrainUpdateUI)
  EVT_MENU(tmwxID_EDGES_SAME_STRAIN, tmwxDoc::OnEdgesSameStrain)
  EVT_UPDATE_UI(tmwxID_REMOVE_EDGE_CONDITIONS, tmwxDoc::OnRemoveEdgeConditionsUpdateUI)
  EVT_MENU(tmwxID_REMOVE_EDGE_CONDITIONS, tmwxDoc::OnRemoveEdgeConditions)
  EVT_UPDATE_UI(tmwxID_PATH_ACTIVE, tmwxDoc::OnPathActiveUpdateUI)
  EVT_MENU(tmwxID_PATH_ACTIVE, tmwxDoc::OnPathActive)
  EVT_UPDATE_UI(tmwxID_PATH_ANGLE_FIXED, tmwxDoc::OnPathAngleFixedUpdateUI)
  EVT_MENU(tmwxID_PATH_ANGLE_FIXED, tmwxDoc::OnPathAngleFixed)
  EVT_UPDATE_UI(tmwxID_PATH_ANGLE_QUANTIZED, tmwxDoc::OnPathAngleQuantizedUpdateUI)
  EVT_MENU(tmwxID_PATH_ANGLE_QUANTIZED, tmwxDoc::OnPathAngleQuantized)
#ifdef TMDEBUG
  EVT_UPDATE_UI(tmwxID_PATH_ACTIVE_V4, tmwxDoc::OnPathActivev4UpdateUI)
  EVT_MENU(tmwxID_PATH_ACTIVE_V4, tmwxDoc::OnPathActivev4)
  EVT_UPDATE_UI(tmwxID_PATH_ANGLE_FIXED_V4, tmwxDoc::OnPathAngleFixedv4UpdateUI)
  EVT_MENU(tmwxID_PATH_ANGLE_FIXED_V4, tmwxDoc::OnPathAngleFixedv4)
  EVT_UPDATE_UI(tmwxID_PATH_ANGLE_QUANTIZED_V4, tmwxDoc::OnPathAngleQuantizedv4UpdateUI)
  EVT_MENU(tmwxID_PATH_ANGLE_QUANTIZED_V4, tmwxDoc::OnPathAngleQuantizedv4)
#endif // TMDEBUG
  EVT_UPDATE_UI(tmwxID_REMOVE_PATH_CONDITIONS, tmwxDoc::OnRemovePathConditionsUpdateUI)
  EVT_MENU(tmwxID_REMOVE_PATH_CONDITIONS, tmwxDoc::OnRemovePathConditions)
  EVT_UPDATE_UI(tmwxID_REMOVE_ALL_CONDITIONS, tmwxDoc::OnRemoveAllConditionsUpdateUI)
  EVT_MENU(tmwxID_REMOVE_ALL_CONDITIONS, tmwxDoc::OnRemoveAllConditions)
  
  // Debug menu
#ifdef TMDEBUG
  EVT_UPDATE_UI(tmwxID_TOGGLE_QUIT_CLEANUP_EARLY, tmwxDoc::OnToggleQuitCleanupEarlyUpdateUI)
  EVT_MENU(tmwxID_TOGGLE_QUIT_CLEANUP_EARLY, tmwxDoc::OnToggleQuitCleanupEarly)
  EVT_UPDATE_UI(tmwxID_MAKE_TREE_BLANK, tmwxDoc::OnMakeTreeBlankUpdateUI)
  EVT_MENU(tmwxID_MAKE_TREE_BLANK, tmwxDoc::OnMakeTreeBlank)
  EVT_UPDATE_UI(tmwxID_MAKE_TREE_UNOPTIMIZED, tmwxDoc::OnMakeTreeUnoptimizedUpdateUI)
  EVT_MENU(tmwxID_MAKE_TREE_UNOPTIMIZED, tmwxDoc::OnMakeTreeUnoptimized)
  EVT_UPDATE_UI(tmwxID_MAKE_TREE_OPTIMIZED, tmwxDoc::OnMakeTreeOptimizedUpdateUI)
  EVT_MENU(tmwxID_MAKE_TREE_OPTIMIZED, tmwxDoc::OnMakeTreeOptimized)
  EVT_UPDATE_UI(tmwxID_MAKE_TREE_GUSSET, tmwxDoc::OnMakeTreeGussetUpdateUI)
  EVT_MENU(tmwxID_MAKE_TREE_GUSSET, tmwxDoc::OnMakeTreeGusset)
  EVT_UPDATE_UI(tmwxID_MAKE_TREE_CONDITIONED, tmwxDoc::OnMakeTreeConditionedUpdateUI)
  EVT_MENU(tmwxID_MAKE_TREE_CONDITIONED, tmwxDoc::OnMakeTreeConditioned)
#endif // TMDEBUG

END_EVENT_TABLE()

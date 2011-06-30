/*******************************************************************************
File:         tmwxFoldedFormFrame.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmwxFoldedFormFrame
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-27
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmModel.h"
#include "tmwxFoldedFormFrame.h"
#include "tmwxDoc.h"
#include "tmwxDocManager.h"

#include "wx/config.h"

/**********
class tmwxFoldedFormFrame
A tool palette that shows the folded form of the base
**********/

// constants
const int LEFT_BORDER = 20;
const int TOP_BORDER = 20;
const int RIGHT_BORDER = 20;
const int BOTTOM_BORDER = 20;

// line colors (defined twmxDesignCanvas.cpp)
extern const wxColor CREASE_AXIAL_COLOR;
extern const wxColor CREASE_GUSSET_COLOR;
extern const wxColor CREASE_RIDGE_COLOR;
extern const wxColor CREASE_UNFOLDED_HINGE_COLOR;
extern const wxColor CREASE_FOLDED_HINGE_COLOR;
extern const wxColor CREASE_PSEUDOHINGE_COLOR;

// colors unique to this window
const wxColor BACKGROUND_COLOR      (223, 223, 223);
const wxColor BLACK_COLOR           (0, 0, 0);
const wxColor FACET_SELECTED_COLOR  (255, 191, 255);
const wxColor FACET_COLOR           (255, 255, 255);


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Constructor
*****/
tmwxFoldedFormFrame::tmwxFoldedFormFrame()
  : tmwxPersistentFrame<wxFrame>(wxT("Folded Form"),
    wxFRAME_TOOL_WINDOW | wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxFULL_REPAINT_ON_RESIZE
#ifdef __WXMSW__
    | wxSYSTEM_MENU
#endif
    ),
  mDoc(0),
  mPrinting(false)
{
  InitPositionAndSize();
  
#ifdef __WXMAC__
  // On Mac, we need some subwindow or wxW doesn't know this frame gets paint
  // events. Other platforms don't seem to have this problem.
  new wxPanel(this);
#endif // __WXMAC__

#if wxUSE_ACCEL
  SetAcceleratorTable(tmwxGetAcceleratorTable());
#endif
}


/*****
Return the key string and default value for the given position/size component.
*****/
void tmwxFoldedFormFrame::GetPositionSizeInfo(Key key, wxString& keystr, int& val)
{
  switch(key) {
    case X_KEY:
      keystr = wxT("FoldedFormFrameX");
      val = 30;
      break;
    case Y_KEY:
      keystr = wxT("FoldedFormFrameY");
      val = 70;
      break;
    case W_KEY:
      keystr = wxT("FoldedFormFrameW");
      val = 280;
      break;
    case H_KEY:
      keystr = wxT("FoldedFormFrameH");
      val = 280;
      break;
    default:
      TMFAIL("unknown key encountered in "\
        "tmwxFoldedFormFrame::GetPositionSizeInfo(..)");
      break;
  }
}


/*****
Set the document which this frame should be displaying and force a redraw
(even if it's the same doc; this gets called for updating, too).
*****/
void tmwxFoldedFormFrame::SetDoc(tmwxDoc* aDoc)
{
  mDoc = aDoc;
  Refresh();
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Set the printing flag (which affects display)
*****/
void tmwxFoldedFormFrame::SetPrinting(bool printing)
{
  mPrinting = printing;
}


/*****
Return the frame coordinates for the given vertex
*****/
wxPoint tmwxFoldedFormFrame::TreeToFrame(const tmVertex* aVertex)
{
  return wxPoint(mXoff + int(mMag * aVertex->GetElevation()), 
    mYoff + int(mMag * aVertex->GetDepth()));
}


/*****
Draw the given vertex
*****/
void tmwxFoldedFormFrame::DrawVertex(wxDC& dc, tmVertex* aVertex)
{
  wxColor theColor = BLACK_COLOR;
  int theWidth = (mDoc->mSelection.Contains(aVertex)) ? 5 : 1;
  dc.SetPen(wxPen(theColor, theWidth, wxSOLID));
  wxPoint pt = TreeToFrame(aVertex);
  dc.DrawLine(pt, pt);
}


/*****
Draw the given crease
*****/
void tmwxFoldedFormFrame::DrawCrease(wxDC& dc, tmCrease* aCrease)
{
  wxColor theColor;
  switch(aCrease->GetKind()) {
    case tmCrease::AXIAL:
      theColor = CREASE_AXIAL_COLOR;
      break;
    case tmCrease::GUSSET:
      theColor = CREASE_GUSSET_COLOR;
      break;
    case tmCrease::RIDGE:
      theColor = CREASE_RIDGE_COLOR;
      break;
    case tmCrease::UNFOLDED_HINGE:
      theColor = CREASE_UNFOLDED_HINGE_COLOR;
      break;
    case tmCrease::FOLDED_HINGE:
      theColor = CREASE_FOLDED_HINGE_COLOR;
      break;
    case tmCrease::PSEUDOHINGE:
      theColor = CREASE_PSEUDOHINGE_COLOR;
      break;
    default:
      TMFAIL("unknown Kind encountered in "\
        "tmwxFoldedFormFrame::DrawCrease(wxDC&, tmCrease*)");
  }
  int theWidth = (mDoc->mSelection.Contains(aCrease)) ? 3 : 1;
  dc.SetPen(wxPen(theColor, theWidth, wxSOLID));
  const tmVertex* v1 = aCrease->GetFrontVertex();
  const tmVertex* v2 = aCrease->GetBackVertex();
  wxPoint pts[2] = {
    TreeToFrame(v1),
    TreeToFrame(v2)
  };
  dc.DrawLines(2, pts);
}


/*****
Draw the given facet
*****/
void tmwxFoldedFormFrame::DrawFacet(wxDC& dc, tmFacet* aFacet)
{
  dc.SetPen(*wxTRANSPARENT_PEN);
  if (IsFacetDrawnSelected(aFacet))
    dc.SetBrush(wxBrush(FACET_SELECTED_COLOR, wxSOLID));
  else
    dc.SetBrush(wxBrush(FACET_COLOR, wxSOLID));
  size_t n = aFacet->GetVertices().size();
  wxPoint* pts = new wxPoint[n];
  for (size_t j = 0; j < n; ++j) {
    tmVertex* theVertex = aFacet->GetVertices()[j];
    pts[j] = TreeToFrame(theVertex);
  }
  dc.DrawPolygon(int(n), pts);
  delete[] pts;
}


/*****
Return true if the facet is selected, or it's a non-pseudohinge facet that
belongs to a corridor that corresponds to an edge that is selected.
*****/
bool tmwxFoldedFormFrame::IsFacetDrawnSelected(tmFacet* aFacet)
{
  if (mDoc->mSelection.Contains(aFacet)) return true;
  tmEdge* theEdge = aFacet->GetCorridorEdge();
  if (theEdge && mDoc->mSelection.Contains(theEdge)) return true;
  return false;
}


/*****
Return true if we can print the folded form, i.e., the window is visible and
we can draw it.
*****/
bool tmwxFoldedFormFrame::CanPrint()
{
  return IsShown() && CanDraw();
}


/*****
Return true if we can draw the folded form, i.e., we have a document and the
tree has valid depth information
*****/
bool tmwxFoldedFormFrame::CanDraw()
{
  return (mDoc && mDoc->mTree->IsVertexDepthValid());
}


/*****
Draw the base in this DC. The drawing is always sized to the DC so that it
fully fills the dc.
*****/
void tmwxFoldedFormFrame::OnDraw(wxDC& dc)
{
  wxFont theFont;
  theFont.SetFamily(wxSWISS);
  theFont.SetPointSize(18);
  dc.SetFont(theFont);
  dc.SetTextForeground(wxColor(127, 127, 127));

  int w, h;
  dc.GetSize(&w, &h);
  
  if (!CanDraw()) {
    wxString text;
    if (!mDoc) text = wxT("No Document");
    else {
      tmTree* theTree = mDoc->mTree;
      if (!theTree->IsPolygonValid())
        text = wxT("Polygons Not Valid");
      else if (!theTree->IsPolygonFilled())
        text = wxT("Polygons Not Filled");
      else if (!theTree->IsVertexDepthValid())
        text = wxT("Vertex Depth Not Valid");
      else TMFAIL("Bug in tmwxFoldedFormFrame::OnDraw(wxDC& dc): not all "\
        "flag combinations are accounted for");
    }
    int fontW, fontH, fontD;
    dc.GetTextExtent(text, &fontW, &fontH, &fontD);
    wxPoint tloc((w - fontW) / 2, (h - fontD) / 2);
    dc.DrawText(text, tloc);
    return;
  }
  
  // Get size of the base
  tmTree* theTree = mDoc->mTree;
  tmFloat xmin = 0, xmax = 0, ymin = 0, ymax = 0;
  for (size_t i = 0; i < theTree->GetVertices().size(); ++i) {
    tmVertex* theVertex = theTree->GetVertices()[i];
    if (xmin > theVertex->GetElevation()) xmin = theVertex->GetElevation();
    if (ymin > theVertex->GetDepth()) ymin = theVertex->GetDepth();
    if (xmax < theVertex->GetElevation()) xmax = theVertex->GetElevation();
    if (ymax < theVertex->GetDepth()) ymax = theVertex->GetDepth();
  }
  
  // Calibrate the image so the base always fills the wxDC and is aligned in the
  // upper left corner of the drawing area.
  tmFloat xmag = (w - LEFT_BORDER - RIGHT_BORDER) / (xmax - xmin);
  tmFloat ymag = (h - LEFT_BORDER - RIGHT_BORDER) / (ymax - ymin);
  mMag = min_val(xmag, ymag);
  mXoff = (LEFT_BORDER - int(mMag * xmin));
  mYoff = (TOP_BORDER - int(mMag * ymin));
  
  // Draw the background if we're not printing
  if (!mPrinting) {
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(BACKGROUND_COLOR, wxSOLID));
    dc.DrawRectangle(0, 0, w, h);
  }

  // Draw every facet that's not selected
  for (size_t i = 0; i < theTree->GetFacets().size(); ++i) {
    tmFacet* theFacet = theTree->GetFacets()[i];
    if (IsFacetDrawnSelected(theFacet)) continue;
    DrawFacet(dc, theFacet);
  }
  
  // Draw every facet that's selected (so color shows up on top)
  for (size_t i = 0; i < theTree->GetFacets().size(); ++i) {
    tmFacet* theFacet = theTree->GetFacets()[i];
    if (!IsFacetDrawnSelected(theFacet)) continue;
    DrawFacet(dc, theFacet);
  }
  
  // Draw all unfolded creases
  for (size_t i = 0; i < theTree->GetCreases().size(); ++i) {
    tmCrease* theCrease = theTree->GetCreases()[i];
    if (theCrease->GetKind() != tmCrease::UNFOLDED_HINGE) continue;
    DrawCrease(dc, theCrease);
  }
  
  // Draw all folded creases
  for (size_t i = 0; i < theTree->GetCreases().size(); ++i) {
    tmCrease* theCrease = theTree->GetCreases()[i];
    if (theCrease->GetKind() == tmCrease::UNFOLDED_HINGE) continue;
    DrawCrease(dc, theCrease);
  }

  // Draw every vertex
  for (size_t i = 0; i < theTree->GetVertices().size(); ++i) {
    tmVertex* theVertex = theTree->GetVertices()[i];
    DrawVertex(dc, theVertex);
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Respond to paint events by creating a DC and drawing into it.
*****/
void tmwxFoldedFormFrame::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    OnDraw(dc);
}


/*****
Attempt to close should only hide the window except at shutdown.
*****/
void tmwxFoldedFormFrame::OnClose(wxCloseEvent& event) 
{
  if (gApp->IsQuitting())
    event.Skip();
  else
    if (event.CanVeto()) {
      event.Veto();
      Hide();
    }
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxFoldedFormFrame, tmwxPersistentFrame<wxFrame>)
  EVT_PAINT(tmwxFoldedFormFrame::OnPaint)
  EVT_CLOSE(tmwxFoldedFormFrame::OnClose)
END_EVENT_TABLE()

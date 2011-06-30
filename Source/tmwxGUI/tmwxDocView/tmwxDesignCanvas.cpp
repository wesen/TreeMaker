/*******************************************************************************
File:         tmwxDesignCanvas.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for TreeMaker design view canvas class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDesignCanvas.h"
#include "tmModel.h"
#include "tmwxDesignFrame.h"
#include "tmwxStr.h"
#include "tmwxInspectorFrame.h"
#include "tmwxDoc.h"

#ifndef __WXMAC__
#include "wx/dcbuffer.h"
#endif

using namespace std;

/*****
Conditional compilation switches (useful in debugging facet ordering)
*****/
#define HIGHLIGHT_LOCAL_ROOTS 0
#define IDENTIFY_SRCS_SINKS 0


/*****
tmwxDesignCanvas constants
*****/

// Borders around paper (pixels)
const int LEFT_BORDER = 15;
const int TOP_BORDER = 35;
const int RIGHT_BORDER = 52;
const int BOTTOM_BORDER = 29;
const int HEADER_OFFSET = 22;     // shift scale above top of paper
const int PIXELS_PER_INCH = 72;   // scaling
const int LABEL_TEXT_SIZE = 9;    // font size for screen labels
const int HEADER_TEXT_SIZE = 12;  // font size for printed header info

// colors
const wxColor BACKGROUND_COLOR            (223, 223, 223);
const wxColor SQUARE_COLOR                (0, 0, 0);
const wxColor SYM_LINE_COLOR              (191, 191, 191);
const wxColor NODE_COLOR                  (63, 63, 63);
const wxColor EDGE_COLOR                  (127, 191, 255);
const wxColor EDGE_PINNED_COLOR           (127, 255, 255);
const wxColor PATH_INTERNAL_COLOR         (255, 191, 127);
const wxColor PATH_VALID_COLOR            (255, 191, 0);
const wxColor PATH_ACTIVE_COLOR           (0, 255, 0);
const wxColor PATH_INFEASIBLE_COLOR       (255, 0, 0);
const wxColor TRIANGLE_COLOR              (63, 191, 191);
const wxColor POLY_COLOR                  (255, 255, 63);
const wxColor VERTEX_COLOR                (127, 63, 0);
extern const wxColor CREASE_AXIAL_COLOR         (0, 0, 0);
extern const wxColor CREASE_GUSSET_COLOR        (127, 127, 127);
extern const wxColor CREASE_RIDGE_COLOR         (255, 0, 0);
extern const wxColor CREASE_UNFOLDED_HINGE_COLOR(191, 191, 255);
extern const wxColor CREASE_FOLDED_HINGE_COLOR  (0, 0, 255);
extern const wxColor CREASE_PSEUDOHINGE_COLOR   (0, 127, 127);
const wxColor CREASE_FLAT_COLOR           (191, 191, 191);
const wxColor CREASE_MOUNTAIN_COLOR       (0, 0, 0);
const wxColor CREASE_VALLEY_COLOR         (255, 31, 127);
const wxColor CREASE_BORDER_COLOR         (0, 0, 0);

const wxColor FACET_NOT_ORIENTED_COLOR    (255, 255, 255);
const wxColor FACET_WHITE_UP_COLOR        (255, 191, 255);
const wxColor FACET_COLOR_UP_COLOR        (255, 127, 255);
const wxColor FACET_LINE_COLOR            (127, 0, 191);
const wxColor CONDITION_COLOR             (191, 127, 255);
const wxColor CONDITION_INFEASIBLE_COLOR  (255, 127, 127);

// Line widths
const int SQUARE_WIDTH = 1;     // width of square outline
const int SYM_LINE_WIDTH = 1;   // width of symmetry line
const int EDGE_WIDTH = 2;     // width of edge lines
const int STD_WIDTH = 1;      // width of most other lines
const int DELTA_WIDTH = 2;      // width increase when hiliting selected objects
const int CONDITION_WIDTH = 5;    // width of condition dots for parts
const int CONDITION_CONN_WIDTH = 1; // width of cond'n lines that connect parts

// other dimensions
const int DOT_SIZE = 4;     // dots used as click points
const int ARROW_SIZE = 7;   // facet order graph arrows
const tmFloat ARROW_ANGLE = 30.;  // arrowhead angle

const tmFloat CLICK_DIST = 4;   // distance for click selection

// Angles for flags on different types of conditions. Angles are different so
// that multiple conditions on the same node don't overlap.
const tmFloat CONDITION_NODE_COMBO_ANGLE = 80.;
const tmFloat CONDITION_NODE_FIXED_ANGLE = 50.;
const tmFloat CONDITION_NODE_ON_CORNER_ANGLE = 20.;
const tmFloat CONDITION_NODE_ON_EDGE_ANGLE = -70.;
const tmFloat CONDITION_NODE_SYMMETRIC_ANGLE = -40.;
const tmFloat CONDITION_EDGE_LENGTH_FIXED_ANGLE = -100.;

const int CONDITION_FLAG_LENGTH = 10; // length of condition flag on points
const int CONDITION_LAT_OFFSET = 5;   // lateral offset for 2-node conditions
const tmFloat CONDITION_LAT_SPLIT_1 = 0.55;  // asymmetry in 2-node conditions
const tmFloat CONDITION_LAT_SPLIT_2 = 0.45;

const wxDash VALLEY_DASHES[2] = {5, 3};   // dash pattern for valley lines
wxDash DC_VALLEY_DASHES[2];               // scaled to DC pixels

/**********
Color utilities
**********/

/*****
Darken the given color toward black by a fractional amount x.
*****/
wxColor Darken(const wxColor& color, const tmFloat& x)
{
  tmFloat xm = 1. - x;
  typedef unsigned char uchar;
  wxColor theColor = color;
  theColor.Set(uchar(xm * theColor.Red()), uchar(xm * theColor.Green()), 
    uchar(xm * theColor.Blue()));
  return theColor;
}


/*****
Lighten the given color toward white by a fractional amount x.
*****/
wxColor Lighten(const wxColor& color, const tmFloat& x)
{
  tmFloat xm = 1. - x;
  typedef unsigned char uchar;
  uchar xc = uchar(x * 255);
  wxColor theColor = color;
  theColor.Set(uchar(xc + xm * theColor.Red()), 
    uchar(xc + xm * theColor.Green()), 
    uchar(xc + xm * theColor.Blue()));
  return theColor;
}


/**********
class tmwxDesignCanvas
contains the scrolling part of the design view of a tmTree
**********/

/*****
Constructor
*****/
tmwxDesignCanvas::tmwxDesignCanvas(tmwxDesignFrame* aFrame, wxSize aSize)
  : wxScrolledWindow(aFrame, wxID_ANY, wxPoint(0, 0), aSize, wxHSCROLL|wxVSCROLL), 
  mDoc(aFrame->GetDoc()),
  mDesignFrame(aFrame),
  mViewSettings(tmwxViewSettings::sDesignView),
  mBorderScrollWidth(0),
  mBorderScrollHeight(0),
  mPaperSize(0.0),
  mPaperSizeScreen(0.0),
  mPaperSizeDC(0.0),
  mDCScale(1.0),
  mFontW(0),
  mFontH(0),
  mFontD(0),
  mDragStart(0, 0),
  mDragOffset(0, 0),
  mDragging(false),
  mPrinting(false)
{
  SetBackgroundColour(BACKGROUND_COLOR);
  CalcBorderScrollSize();
}


/*****
Return a ptr to the tree.
*****/
tmTree* const tmwxDesignCanvas::GetTree() const 
{
  return mDoc->mTree;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Compute member variables that include the border and scrollbar widths. This
data will not change, unless the user somehow manages to change window style
while the program is running.
*****/
void tmwxDesignCanvas::CalcBorderScrollSize()
{
  // temporarily force instantiation of scrollbars. No need to refresh, since
  // callers of this routine will reset the scrollbars to their proper values.
  SetScrollbars(1, 1, 10000, 10000, 0, 0, true);
  // Get current visible area of canvas (smaller since scrollbars are present)
  int cw, ch;
  GetClientSize(&cw, &ch);
  // Get current exterior size of window
  int fw, fh;
  mDesignFrame->GetSize(&fw, &fh);
  // Calculate space taken up by borders and scroll bar, and keep it around
  // in member variable.
  mBorderScrollWidth = fw - cw;
  mBorderScrollHeight = fh - ch;
}


/*****
Adjust the virtual size of the image in response to a change in paper
dimensions within the model or a change in the paper size as it should be
displayed on the screen.
*****/
void tmwxDesignCanvas::UpdatePaperSize()  
{
  // Get the size of the paper image based on the (possibly new) paper size.
  int imageWidth, imageHeight;
  GetImageSizeScreen(imageWidth, imageHeight);
  
  // Now set the window size; ideally displays the full image, but we may have
  // to reduce or reposition the wondow to keep it on the screen.
  int newWidth = imageWidth + mBorderScrollWidth;
  int newHeight = imageHeight + mBorderScrollHeight;
  
  // Compute the new size of the frame; match the image size if possible, but 
  // no larger than the available screen area.
  wxRect displayRect = wxGetClientDisplayRect();
  newWidth = min_val(newWidth, displayRect.width);
  newHeight = min_val(newHeight, displayRect.height);
  mDesignFrame->SetSizeHints(100, 100, newWidth, newHeight);
  
  // Reposition the window if necessary to keep it from going off the screen.
  int newX, newY;
  mDesignFrame->GetPosition(&newX, &newY);
  newX = min_val(newX, displayRect.GetRight() - newWidth);
  newX = max_val(newX, displayRect.GetLeft());
  newY = min_val(newY, displayRect.GetBottom() - newHeight);
  newY = max_val(newY, displayRect.GetTop());
  mDesignFrame->SetSize(newX, newY, newWidth, newHeight);
  
#ifdef __WXGTK__
  /* some environments (GNOME, KDE) require broadening hints otherwise
     the previous SetSize is sometimes ignored.
     The min/max sizes below are somewhat of a guess - CAF.
   */
  mDesignFrame->SetSizeHints(30, 30, 
                             2*displayRect.width, 2*displayRect.height);
#endif
  // Set the image size, which will update the scroll bars
  SetVirtualSize(imageWidth, imageHeight);
}


/*****
Set a new paper size and adjust the virtual size appropriately.
*****/
void tmwxDesignCanvas::SetPaperSize(const tmFloat& newPaperSize)  
{
  mPaperSize = newPaperSize;
  mPaperSizeScreen = newPaperSize * PIXELS_PER_INCH;
  mPaperSizeDC = mPaperSizeScreen;
  UpdatePaperSize();
}


/*****
Return true if the paper is properly fit to the screen in the specified manner.
*****/
bool tmwxDesignCanvas::IsFitPaper(FittingType fitting)
{
  TMASSERT(fitting == FITTING_WIDTH || fitting == FITTING_HEIGHT || \
    fitting == FITTING_SIZE);
    
  wxRect displayRect = wxGetClientDisplayRect();
  wxRect frameRect = mDesignFrame->GetRect();
  
  // First, compare frame position against display rect. If any portion of the
  // frame is off-screen, we are not properly fit.
  if (frameRect.GetLeft() < displayRect.GetLeft()) 
    return false;
  if (frameRect.GetRight() > displayRect.GetRight()) 
    return false;
  if (frameRect.GetTop() < displayRect.GetTop()) 
    return false;
  if (frameRect.GetBottom() > displayRect.GetBottom()) 
    return false;
  
  // Next, compare frame size against display size.
  if (fitting == FITTING_WIDTH && frameRect.width != displayRect.width)
    return false;
  if (fitting == FITTING_HEIGHT && frameRect.height != displayRect.height)
    return false;
  if (fitting == FITTING_SIZE && min_val(displayRect.width - frameRect.width,
    displayRect.height - frameRect.height) != 0)
    return false;
  
  // Next, compare image size against frame size.
  int imageWidth, imageHeight;
  GetImageSizeScreen(imageWidth, imageHeight);
  const int fw = imageWidth + mBorderScrollWidth;
  const int fh = imageHeight + mBorderScrollHeight;
  if (fitting == FITTING_WIDTH && fw != frameRect.width) 
    return false;
  if (fitting == FITTING_HEIGHT && fh != frameRect.height) 
    return false;
  if (fitting == FITTING_SIZE && 
    min_val(frameRect.width - fw, frameRect.height - fh) != 0)
    return false;
  return true;
}


/*****
Set the paper size to the maximum value that displays it completely and
reposition the window accordingly.
*****/
void tmwxDesignCanvas::FitPaper(FittingType fitting)
{
  TMASSERT(fitting == FITTING_WIDTH || fitting == FITTING_HEIGHT || \
    fitting == FITTING_SIZE);
    
  wxRect displayRect = wxGetClientDisplayRect();
  
  // Maximum space available for width and height of paper
  const int maxPaperWidth = displayRect.width - mBorderScrollWidth - 
    LEFT_BORDER - RIGHT_BORDER;
  const int maxPaperHeight = displayRect.height - mBorderScrollHeight - 
    TOP_BORDER - BOTTOM_BORDER
#ifdef __WXGTK__
    /* as of wxGTK 2.6.2, wxGetClientDisplayRect for GTK does not take
       in account the space eventually taken by system menus and task
       bars in specific window managers. When fitting size, this may
       cause the bottom of a doc window (and worse yet, the resize
       decoration) to be left offscreen. As a hack, assume most such
       menus and bars use only vertical space and discount it here
       (obviously this will waste screen estate in some window
       managers) */
    - 50 // leave an arbitrary slack
#endif
    ;
    
  // Compute new paper size that fits into this space.
  const tmFloat paperSizeFromW = maxPaperWidth / GetTree()->GetPaperWidth();
  const tmFloat paperSizeFromH = maxPaperHeight / GetTree()->GetPaperHeight();

  if(fitting == FITTING_WIDTH)
    mPaperSizeScreen = paperSizeFromW;
  else if(fitting == FITTING_HEIGHT)
    mPaperSizeScreen = paperSizeFromH;
  else // FITTING_SIZE
    mPaperSizeScreen = min_val(paperSizeFromW, paperSizeFromH);

  mPaperSizeDC = mPaperSizeScreen;
  mPaperSize = mPaperSizeScreen / PIXELS_PER_INCH;
  
  // Now we can call UpdatePaperSize() to finish the job, by updating the
  // window size, position, and/or scroll bar settings.
  UpdatePaperSize();
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Set the printing flag, which affects both what parts of the image are drawn
and how the image is scaled to the DC.
*****/
void tmwxDesignCanvas::SetPrinting(bool printing)
{
  mPrinting = printing;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the total width and height of the drawn image of the paper in screen
pixels, including paper and borders around the paper.
*****/
void tmwxDesignCanvas::GetImageSizeScreen(int& width, int& height) const
{
  width = LEFT_BORDER + RIGHT_BORDER + 
    int(0.5 + GetTree()->GetPaperWidth() * mPaperSizeScreen);
  height = TOP_BORDER + BOTTOM_BORDER + 
    int(0.5 + GetTree()->GetPaperHeight() * mPaperSizeScreen);
}


/*****
Convert a screen pixel distance to a distance on the current DC, which could
be for screen, print preview, or print job.
*****/
int tmwxDesignCanvas::PixelsToDC(int i) const
{
  return int(0.5 + i * mDCScale);
}


/*****
Convert a screen pixel distance to a distance on the current DC, which could
be for screen, print preview, or print job.
*****/
int tmwxDesignCanvas::PixelsToDC(const tmFloat& f) const
{
  return int(0.5 + f * mDCScale);
}


/*****
Convert a distance from model coordinates to screen or printer DC coordinates.
We round to the nearest integral pixel value.
*****/
int tmwxDesignCanvas::TreeToDC(const tmFloat& f) const
{
    return int(0.5 + f * mPaperSizeDC);
}


/*****
Convert a point from model coordinates to screen or printer DC coordinates
*****/
wxPoint tmwxDesignCanvas::TreeToDC(const tmPoint& fp) const
{
  return wxPoint(PixelsToDC(LEFT_BORDER) + TreeToDC(fp.x),
    PixelsToDC(TOP_BORDER) + TreeToDC(GetTree()->GetPaperHeight() - fp.y));
}


/*****
Convert a distance from DC coordinates to model coordinates
*****/
tmFloat tmwxDesignCanvas::DCToTree(int i) const
{
    return i / mPaperSizeDC;
}


/*****
Convert a point from DC coordinates to model coordinates
*****/
tmPoint tmwxDesignCanvas::DCToTree(const wxPoint& p) const
{
  return tmPoint(DCToTree(p.x - PixelsToDC(LEFT_BORDER)),
    GetTree()->GetPaperHeight() - DCToTree(p.y - PixelsToDC(TOP_BORDER)));
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*
Notes on visibility. If an object is visible, it is displayed and clicks on it
are detected. Visibility of classes of object are set by various flags in
mViewSettings. If an object is currently selected, it is made visible no matter
what the settings flags are. Otherwise, the settings are followed. For objects
with multiple check boxes, any one of them is sufficient to make the object
visible.
*/


/*****
Return true if aNode is visible.
*****/
template <>
bool tmwxDesignCanvas::IsVisible<tmNode>(tmNode* aNode)
{
  if (mDoc->mSelection.Contains(aNode)) return true;
  if (mViewSettings.mShowLeafNodes && aNode->IsLeafNode()) return true;
  if (mViewSettings.mShowBranchNodes && aNode->IsBranchNode()) return true;
  if (mViewSettings.mShowSubNodes && aNode->IsSubNode()) return true;
  return false;
}


/*****
Return true if aEdge is visible.
*****/
template <>
bool tmwxDesignCanvas::IsVisible<tmEdge>(tmEdge* aEdge)
{
  if (mDoc->mSelection.Contains(aEdge)) return true;
  if (mViewSettings.mShowAllEdges) return true;
  return false;
}


/*****
Return true if aPath is visible.
*****/
template <>
bool tmwxDesignCanvas::IsVisible<tmPath>(tmPath* aPath)
{
  if (mDoc->mSelection.Contains(aPath)) return true;
  if (mViewSettings.mShowLeafPaths && aPath->IsLeafPath()) return true;
  if (mViewSettings.mShowBranchPaths && !aPath->IsLeafPath()) return true;
  if (mViewSettings.mShowSubPaths && aPath->IsSubPath()) return true;
  if (mViewSettings.mShowActivePaths && aPath->IsActivePath()) return true;
  if (mViewSettings.mShowBorderPaths && aPath->IsBorderPath()) return true;
  if (mViewSettings.mShowPolygonPaths && aPath->IsPolygonPath()) return true;
  if (mViewSettings.mShowInfeasiblePaths && aPath->IsLeafPath() && 
    aPath->IsInfeasiblePath()) return true;
  if (mViewSettings.mShowPathConditions && aPath->IsConditionedPath()) 
    return true;
  return false;
}


/*****
Return true if aPoly is visible.
*****/
template <>
bool tmwxDesignCanvas::IsVisible<tmPoly>(tmPoly* aPoly)
{
  if (mDoc->mSelection.Contains(aPoly)) return true;
  if (mViewSettings.mShowMajorPolys && aPoly->IsMajorPoly()) return true;
  if (mViewSettings.mShowSubPolys && aPoly->IsSubPoly()) return true;
  return false;
}


/*****
Return true if aVertex is visible.
*****/
template <>
bool tmwxDesignCanvas::IsVisible<tmVertex>(tmVertex* aVertex)
{
  if (mDoc->mSelection.Contains(aVertex)) return true;
  if (mViewSettings.mShowMajorVertices && aVertex->IsMajorVertex()) return true;
  if (mViewSettings.mShowMinorVertices && aVertex->IsMinorVertex()) return true;
  return false;
}


/*****
Return true if aCrease is visible.
*****/
template <>
bool tmwxDesignCanvas::IsVisible<tmCrease>(tmCrease* aCrease)
{
  if (mDoc->mSelection.Contains(aCrease)) return true;
  if (mViewSettings.mShowMajorCreases && aCrease->IsMajorCrease()) return true;
  if (mViewSettings.mShowMinorCreases && aCrease->IsMinorCrease()) return true;
  return false;
}


/*****
Return true if aFacet is visible.
*****/
template <>
bool tmwxDesignCanvas::IsVisible<tmFacet>(tmFacet* aFacet)
{
  if (mDoc->mSelection.Contains(aFacet)) return true;
  if (mViewSettings.mShowAllFacets) return true;
  return false;
}


/*****
Return true if aCondition is visible.
*****/
template <>
bool tmwxDesignCanvas::IsVisible<tmCondition>(tmCondition* aCondition)
{
  if (mDoc->mSelection.Contains(aCondition)) return true;
  if (mViewSettings.mShowNodeConditions && aCondition->IsNodeCondition()) 
    return true;
  if (mViewSettings.mShowEdgeConditions && aCondition->IsEdgeCondition()) 
    return true;
  if (mViewSettings.mShowPathConditions && aCondition->IsPathCondition())
    return true;
  return false;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Calculate the point in tree coordinates of this tmNode. If it is part of the
list mMovingNodes, then offset it by mDragOffset (transformed into tree
coords). Also, clamp the coordinates to always lie within the paper. This lets
us see the tree coordinates update in real time during a drag. Since the
position of other parts are determined by the nodes that go into them, this
routine ends up affecting how all parts are drawn in a tmwxDesignCanvas.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmNode>(const tmNode* aNode)
{
  tmPoint loc = aNode->GetLoc();
  if (mMovingNodes.contains(aNode)) {
    tmTree* theTree = GetTree();
    if (mDragging) {
      loc.x += mDragOffset.x / (mPaperSizeScreen);
      loc.y -= mDragOffset.y / (mPaperSizeScreen);
    }
    loc.x = min_val(max_val(0., loc.x), theTree->GetPaperWidth());
    loc.y = min_val(max_val(0., loc.y), theTree->GetPaperHeight());
  }
  return loc;
}


/*****
Calculate the single point at which this tmEdge's text should be written.
This is also the point used for tmCondition attachment. Note that if we're
dragging any nodes, the edges may be affected. Dot is slightly offset from
center so that in overlapping paths and edges the labels and dots don't
overlap. (Paths get the midpoint.)
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmEdge>(const tmEdge* aEdge)
{
  tmPoint p1 = CalcLoc(aEdge->GetNodes().front());
  tmPoint p2 = CalcLoc(aEdge->GetNodes().back());
  return 0.55 * p1 + 0.45 * p2;
}

/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionNodeCombo>(
  const tmConditionNodeCombo* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode()), 
    CONDITION_NODE_COMBO_ANGLE);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionNodeFixed>(
  const tmConditionNodeFixed* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode()), 
    CONDITION_NODE_FIXED_ANGLE);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionNodeOnCorner>(
  const tmConditionNodeOnCorner* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode()), 
    CONDITION_NODE_ON_CORNER_ANGLE);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionNodeOnEdge>(
  const tmConditionNodeOnEdge* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode()), 
    CONDITION_NODE_ON_EDGE_ANGLE);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionNodeSymmetric>(
  const tmConditionNodeSymmetric* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode()), 
    CONDITION_NODE_SYMMETRIC_ANGLE);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionNodesPaired>(
  const tmConditionNodesPaired* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode1()), 
    CalcLoc(aCondition->GetNode2()), true);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionNodesCollinear>(
  const tmConditionNodesCollinear* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode1()), 
    CalcLoc(aCondition->GetNode2()), 
    CalcLoc(aCondition->GetNode3()));
}

/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionEdgeLengthFixed>(
  const tmConditionEdgeLengthFixed* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetEdge()), 
    CONDITION_EDGE_LENGTH_FIXED_ANGLE);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionEdgesSameStrain>(
  const tmConditionEdgesSameStrain* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetEdge1()), 
    CalcLoc(aCondition->GetEdge2()), true);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionPathCombo>(
  const tmConditionPathCombo* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode1()), 
    CalcLoc(aCondition->GetNode2()), false);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionPathActive>(
  const tmConditionPathActive* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode1()), 
    CalcLoc(aCondition->GetNode2()), false);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionPathAngleFixed>(
  const tmConditionPathAngleFixed* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode1()), 
    CalcLoc(aCondition->GetNode2()), false);
}


/*****
Calculate the point at which this condition's text should be drawn.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmConditionPathAngleQuant>(
  const tmConditionPathAngleQuant* aCondition)
{
  TMASSERT(aCondition);
  return CalcLocOffset(CalcLoc(aCondition->GetNode1()), 
    CalcLoc(aCondition->GetNode2()), false);
}


/*****
Calculate the single point at which this tmPath's text should be written.
This is also the point used for tmCondition attachment.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmPath>(const tmPath* aPath)
{
  tmPoint p1 = CalcLoc(aPath->GetNodes().front());
  tmPoint p2 = CalcLoc(aPath->GetNodes().back());
  return 0.50 * p1 + 0.50 * p2;
}


/*****
Calculate the single point at which this tmPoly's text should be written.
Use the centroid of the nodes that make up its vertices.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmPoly>(const tmPoly* aPoly)
{
  tmPoint ctr(0, 0);
  size_t n = aPoly->GetRingNodes().size();
  for (size_t i = 0; i < n; ++i) 
    ctr += CalcLoc(aPoly->GetRingNodes()[i]);
  return ctr / n;
}


/*****
Calculate the single point at which this tmVertex's text should be written.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmVertex>(const tmVertex* aVertex)
{
  // Vertices are unaffected by dragging and are always constructed within
  // the square, so no corrections need be applied.
  return aVertex->GetLoc();
}


/*****
Calculate the single point at which this tmCrease's text should be written.
This is offset from the center so that coincident tmPath & tmCrease don't have
their text overlap.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmCrease>(const tmCrease* aCrease)
{
  tmPoint p1 = aCrease->GetFrontVertex()->GetLoc();
  tmPoint p2 = aCrease->GetBackVertex()->GetLoc();
  return 0.60 * p1 + 0.40 * p2;
}


/*****
Calculate the single point at which this tmFacet's text should be written.
Use the centroid of its vertices.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmFacet>(const tmFacet* aFacet)
{
  tmPoint ctr(0, 0);
  size_t n = aFacet->GetVertices().size();
  for (size_t i = 0; i < n; ++i) 
    ctr += CalcLoc(aFacet->GetVertices()[i]);
  return ctr / n;
}


/*****
Calculate the single point at which this tmCondition's dot and text should be
written. The value depends on the specific type of condition, which we get from
our lookup table.
*****/
template <>
tmPoint tmwxDesignCanvas::CalcLoc<tmCondition>(const tmCondition* aCondition)
{
  return (this->*GetCalcLocFns()[aCondition->GetTag()])(aCondition);
}


/*****
Return the point p, offset by CONDITION_FLAG_LENGTH in the given angle [degrees]
*****/
tmPoint tmwxDesignCanvas::CalcLocOffset(const tmPoint& p, const tmFloat& angle)
{
  return p + DCToTree(PixelsToDC(CONDITION_FLAG_LENGTH)) * 
    UnitVector(angle * DEGREES);
}


/*****
Return a point bit beyond the midpoint of the line from p1 to p2, offset a
distance between CONDITION_LAT_OFFSET and 2 * CONDITION_LAT_OFFSET
perpendicularly from the line between p1 and p2. The offset distance is larger
if the separation between the points is larger. Both the non-symmetric split
and the variability of the lateral offset is so that collinear pairs of points
have separable condition flags.
*****/
tmPoint tmwxDesignCanvas::CalcLocOffset(const tmPoint& p1, const tmPoint& p2, 
  bool up)
{
  tmPoint ctr = CONDITION_LAT_SPLIT_1 * p1 + CONDITION_LAT_SPLIT_2 * p2;
  tmPoint offset = RotateCCW90(p2 - p1);
  if ((up && offset.y < 0) || (!up && offset.y > 0)) offset *= -1;
  tmFloat rm = Mag(offset);
  tmFloat tdist = DCToTree(PixelsToDC(CONDITION_LAT_OFFSET));
  if (rm == 0) return ctr + tdist * UnitVector(0.);
  tmFloat dist = tdist * (1 + rm / Mag(GetTree()->GetPaperSize()));
  return ctr + (dist / rm) * offset;
}


/*****
Return the point p, offset by CONDITION_FLAG_LENGTH perpendicularly from the
incenter of the triangle formed by p1, p2, and p3; the offset
direction is perpendicular to the longest axis of the triangle.
*****/
tmPoint tmwxDesignCanvas::CalcLocOffset(const tmPoint& p1, 
  const tmPoint& p2, const tmPoint& p3)
{
  tmPoint ctr = Incenter(p1, p2, p3);
  tmPoint d1 = p1 - ctr;
  tmPoint d2 = p2 - ctr;
  tmPoint d3 = p3 - ctr;
  tmFloat d1m = Mag(d1);
  tmFloat d2m = Mag(d2);
  tmFloat d3m = Mag(d3);
  tmPoint offset = 
    (d1m > d2m) ? ((d1m > d3m) ? d1 : d3) : ((d2m > d3m) ? d2 : d3);
  tmFloat rm = Mag(offset);
  tmFloat tdist = DCToTree(PixelsToDC(CONDITION_FLAG_LENGTH));
  if (rm == 0) return ctr + tdist * UnitVector(90.);
  offset = RotateCCW90(offset);
  if (offset.y < 0) offset *= -1;
  return ctr + (tdist / rm) * offset;
}


/*****
STATIC
Make if necessary and return our lookup table
*****/
tmArray<tmwxDesignCanvas::CalcLocFn>& tmwxDesignCanvas::GetCalcLocFns()
{
  static tmArray<CalcLocFn> sCalcLocFns;
  if (sCalcLocFns.empty()) {
    TMASSERT(tmPart::TypesAreInitialized());
    tmPart::MakeTypeArray<CalcLocFn, CalcLocFnT>(sCalcLocFns);
  }
  return sCalcLocFns;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Modify theColor if the part is selected (darker).
*****/
wxColor tmwxDesignCanvas::HiliteColor(const wxColor& theColor, bool isSelected)
{
  if (isSelected) return Darken(theColor, .1);
  else return theColor;
}


/*****
Modify theColor for drawing Text (darker).
*****/
template <>
wxColor tmwxDesignCanvas::TweakColor<tmwxDesignCanvas::Text>(
  const wxColor& theColor)
{
  return Darken(theColor, .5);
}


/*****
Modify theColor for drawing Lines (no change).
*****/
template <>
wxColor tmwxDesignCanvas::TweakColor<tmwxDesignCanvas::Lines>(
  const wxColor& theColor)
{
  return theColor;
}


/*****
Modify theColor for drawing Fills (lighter).
*****/
template <>
wxColor tmwxDesignCanvas::TweakColor<tmwxDesignCanvas::Fill>(
  const wxColor& theColor)
{
  return Lighten(theColor, .75);
}


/*****
Return the appropriate color for part p used for drawing 
S = {Text, Lines, Fill}, taking into account whether the part is selected or
not.
*****/
template <class S, class P>
wxColor tmwxDesignCanvas::GetPartColor(P* p, bool isSelected)
{
  return HiliteColor(TweakColor<S>(GetBasePartColor(p)), isSelected);
}


/*****
Draw a dot at the given location in the given color and size.
*****/
void tmwxDesignCanvas::DrawDot(wxDC& dc, const wxPoint& loc,
  const wxColor& theColor, const int theWidth)
{
#if (wxMAC_USE_CORE_GRAPHICS == 1)
  // Mac Core Graphics means we have to draw circles
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(wxBrush(theColor, wxSOLID));
  dc.DrawCircle(loc, theWidth / 2);
#else
  // Otherwise we can draw zero-length lines, which create dots
  dc.SetPen(wxPen(theColor, theWidth, wxSOLID));
  dc.DrawLine(loc, loc);
#endif
}


/*****
Draw the text that goes along with the paper -- the scale.
*****/
template <>
void tmwxDesignCanvas::DrawPaper<tmwxDesignCanvas::Text>(wxDC& dc)
{
  tmTree* theTree = GetTree();
  wxString text;
  if (mPrinting) {
    // In the printout, we'll display the filename and scale right on the image.
    dc.SetTextForeground(*wxBLACK);
    wxPoint p1 = TreeToDC(tmPoint(0, theTree->GetPaperHeight()));
    p1.y -= PixelsToDC(HEADER_OFFSET);
    mDoc->GetPrintableName(text);
    dc.DrawText(text, p1);
    wxPoint p2 = TreeToDC(tmPoint(theTree->GetPaperWidth(), 
      theTree->GetPaperHeight()));
    p2.y -= PixelsToDC(HEADER_OFFSET);
    text.Printf(wxT("scale = %.4f"), theTree->GetScale());
    int fontW, fontH, fontD;
    dc.GetTextExtent(text, &fontW, &fontH, &fontD);
    p2.x -= fontW;
    dc.DrawText(text, p2);
  }
}


/*****
Draw the linear features in the paper -- square outline and symmetry line
*****/
template <>
void tmwxDesignCanvas::DrawPaper<tmwxDesignCanvas::Lines>(wxDC& dc)
{
  tmTree* theTree = GetTree();
  
  // Draw the paper outline
  int theWidth = SQUARE_WIDTH;
  dc.SetPen(wxPen(SQUARE_COLOR, theWidth, wxSOLID));
  tmFloat pwidth = theTree->GetPaperWidth();
  tmFloat pheight = theTree->GetPaperHeight();
  wxPoint square[5];
  square[0] = square[4] = TreeToDC(tmPoint(0, 0));
  square[1] = TreeToDC(tmPoint(pwidth, 0));
  square[2] = TreeToDC(tmPoint(pwidth, pheight));
  square[3] = TreeToDC(tmPoint(0, pheight));
  dc.DrawLines(5, square);
  
  // Draw the symmetry line, which is clipped to the paper rectangle
  if (theTree->HasSymmetry() && mViewSettings.mShowSymmetryLines) {
    int theWidth = SYM_LINE_WIDTH;
    dc.SetPen(wxPen(SYM_LINE_COLOR, theWidth, wxSOLID));
    tmPoint q1, q2;
    ClipLineToRect(theTree->GetSymLoc(), theTree->GetSymDir(),
      tmPoint(0, 0), tmPoint(pwidth, pheight), q1, q2);
    wxPoint pts[2];
    pts[0] = TreeToDC(q1);
    pts[1] = TreeToDC(q2);
    dc.DrawLines(2, pts);
  }
}


/*****
Draw the paper fill color
*****/
template <>
void tmwxDesignCanvas::DrawPaper<tmwxDesignCanvas::Fill>(wxDC& dc)
{
  tmTree* theTree = GetTree();
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(*wxWHITE_BRUSH);
  
  // Draw the paper
  tmFloat pwidth = theTree->GetPaperWidth();
  tmFloat pheight = theTree->GetPaperHeight();
  wxPoint square[5];
  square[0] = square[4] = TreeToDC(tmPoint(0, 0));
  square[1] = TreeToDC(tmPoint(pwidth, 0));
  square[2] = TreeToDC(tmPoint(pwidth, pheight));
  square[3] = TreeToDC(tmPoint(0, pheight));
  dc.DrawPolygon(5, square);
}


/*****
Return the base color for drawing this tmNode.
*****/
template <>
wxColor tmwxDesignCanvas::GetBasePartColor<tmNode>(tmNode*)
{
  return NODE_COLOR;
}


/*****
Draw the dot and text for a tmNode.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmNode>(
  wxDC& dc, tmNode* aNode, bool isSelected)
{
  // Dot
  wxColor theColor = GetPartColor<Text>(aNode, isSelected);
  int theWidth = !isSelected ? DOT_SIZE : DOT_SIZE + DELTA_WIDTH;
  tmPoint tloc = CalcLoc(aNode);
  wxPoint cloc = TreeToDC(tloc);
  if (mViewSettings.mShowNodeDots || isSelected) 
    DrawDot(dc, cloc, theColor, theWidth);
  // Text
  cloc.x += 5;
  cloc.y -= (mFontH - mFontD);
  wxString text;
  dc.SetTextForeground(theColor);
  // Draw the index
  if (mViewSettings.mShowNodeIndices) {
    text = tmwxStr(aNode);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw coordinates
  if (mViewSettings.mShowNodeCoords) {
    text.Printf(wxT("(%.4f,"), tloc.x);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
    text.Printf(wxT(" %.4f)"), tloc.y);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw elevation
  if (mViewSettings.mShowNodeElevation) {
    text.Printf(wxT("E:%.4f"), aNode->GetElevation());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw depth
  if (mViewSettings.mShowNodeDepth) {
    text.Printf(wxT("D:%.4f"), aNode->GetDepth());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw flags
  if (mViewSettings.mShowNodeFlags) {
    text = wxEmptyString;
    text += ((aNode->IsLeafNode()) ? 'L' : 'l');
    text += ((aNode->IsSubNode()) ? 'S' : 's');
    text += ((aNode->IsBorderNode()) ? 'B' : 'b');
    text += ((aNode->IsPinnedNode()) ? 'Q' : 'q');
    text += ((aNode->IsPolygonNode()) ? 'P' : 'p');
    text += ((aNode->IsConditionedNode()) ? 'C' : 'c');
    dc.DrawText(text, cloc);  
    cloc.y += mFontH;
  } 
  // Draw the label, last since it's usually blank
  if (mViewSettings.mShowNodeLabels) {
    text = wxString::FromAscii(aNode->GetLabel());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
}


/*****
Draw the lines for a tmNode.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Lines, tmNode>(
  wxDC& dc, tmNode* aNode, bool isSelected)
{
  if (mViewSettings.mShowNodeCircles) {
    if (!aNode->IsLeafNode()) return;
    wxColor theColor = GetPartColor<Lines>(aNode, isSelected);
    int theWidth = 
      !isSelected ? STD_WIDTH : STD_WIDTH + DELTA_WIDTH;
    dc.SetPen(wxPen(theColor, theWidth, wxSOLID));
    dc.SetBrush(*wxTRANSPARENT_BRUSH );
    wxPoint ctr = TreeToDC(CalcLoc(aNode));
    tmFloat elen = (aNode->GetEdges().not_empty()) ? 
      aNode->GetEdges()[0]->GetStrainedLength() : 1.0;
    wxCoord rad = TreeToDC(elen * GetTree()->GetScale());
    dc.DrawCircle(ctr, rad);
  }
}


/*****
Return the base color for drawing this tmEdge.
*****/
template <>
wxColor tmwxDesignCanvas::GetBasePartColor<tmEdge>(tmEdge* aEdge)
{
  if (aEdge->IsPinnedEdge())
    return EDGE_PINNED_COLOR;
  else
    return EDGE_COLOR;
}


/*****
Draw the dot and text for a tmEdge.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmEdge>(
  wxDC& dc, tmEdge* aEdge, bool isSelected)
{
  // Dot
  wxColor theColor = GetPartColor<Text>(aEdge, isSelected);
  int theWidth = 
    !isSelected ? DOT_SIZE : DOT_SIZE + DELTA_WIDTH;
  wxPoint cloc = TreeToDC(CalcLoc(aEdge));
  if (mViewSettings.mShowEdgeDots || isSelected) 
    DrawDot(dc, cloc, theColor, theWidth);
  // Text
  cloc.x += 5;
  cloc.y -= (mFontH - mFontD);
  wxString text;
  dc.SetTextForeground(theColor);
  // Draw the index
  if (mViewSettings.mShowEdgeIndices) {
    text = tmwxStr(aEdge);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw the length with strain if present
  if (mViewSettings.mShowEdgeLengths) {
    if (aEdge->GetStrain() == 0 || !mViewSettings.mShowEdgeStrain)
      text.Printf(wxT("%.4f"), aEdge->GetLength());
    else
      text.Printf(wxT("%.4f%+.0f%%"), aEdge->GetLength(), 
        100. * aEdge->GetStrain());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw flags
  if (mViewSettings.mShowEdgeFlags) {
    text = wxEmptyString;
    text += ((aEdge->IsPinnedEdge()) ? 'Q' : 'q');
    text += ((aEdge->IsConditionedEdge()) ? 'C' : 'c');
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw the label (last since it's usually blank)
  if (mViewSettings.mShowEdgeLabels) {
    text = wxString::FromAscii(aEdge->GetLabel());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
}


/*****
Draw the lines for a tmEdge.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Lines, tmEdge>(
  wxDC& dc, tmEdge* aEdge, bool isSelected)
{
  // Draw the edge line, always if selected
  if (mViewSettings.mShowEdgeLines || isSelected) {
    wxColor theColor = GetPartColor<Lines>(aEdge, isSelected);
    int theWidth = 
      !isSelected ? EDGE_WIDTH : EDGE_WIDTH + DELTA_WIDTH;
    wxPoint pts[2];
    pts[0] = TreeToDC(CalcLoc(aEdge->GetNodes().front()));
    pts[1] = TreeToDC(CalcLoc(aEdge->GetNodes().back()));
    dc.SetPen(wxPen(theColor, theWidth, wxSOLID));
    dc.DrawLines(2, pts);
  }
}


/*****
Return the appropriate color for this path, which depends on its flag values.
*****/
template <>
wxColor tmwxDesignCanvas::GetBasePartColor<tmPath>(tmPath* aPath)
{
  if (!aPath->IsLeafPath()) return PATH_INTERNAL_COLOR;
  else if (!aPath->IsFeasiblePath()) return PATH_INFEASIBLE_COLOR;
  else if (aPath->IsActivePath()) return PATH_ACTIVE_COLOR;
  else return PATH_VALID_COLOR;
}


/*****
Draw the dot and text for a tmPath.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmPath>(
  wxDC& dc, tmPath* aPath, bool isSelected)
{
  // Dot
  wxColor theColor = GetPartColor<Text>(aPath, isSelected);
  int theWidth = 
    !isSelected ? DOT_SIZE : DOT_SIZE + DELTA_WIDTH;
  wxPoint cloc = TreeToDC(CalcLoc(aPath));
  if (mViewSettings.mShowPathDots || isSelected) 
    DrawDot(dc, cloc, theColor, theWidth);
  // Text
  cloc.x += 5;
  cloc.y -= (mFontH - mFontD);
  wxString text;
  dc.SetTextForeground(theColor);
  // Draw the index
  if (mViewSettings.mShowPathIndices) {
    text = tmwxStr(aPath);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw the actual path length and minimum path length in tree units
  if (mViewSettings.mShowPathLengths) {
#ifdef __WXMAC__
    text.Printf(wxT("%.4f³%.4f"), 
#else
    text.Printf(wxT("%.4f>=%.4f"), 
#endif
      aPath->GetActTreeLength(), aPath->GetMinTreeLength());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw flags
  if (mViewSettings.mShowPathFlags) {
    text = wxEmptyString;
    text += ((aPath->IsLeafPath()) ? 'L' : 'l');
    text += ((aPath->IsSubPath()) ? 'S' : 's');
    text += ((aPath->IsFeasiblePath()) ? 'V' : 'v');
    text += ((aPath->IsActivePath()) ? 'A' : 'a');
    text += ((aPath->IsBorderPath()) ? 'B' : 'b');
    text += ((aPath->IsPolygonPath()) ? 'P' : 'p');
    text += ((aPath->IsConditionedPath()) ? 'C' : 'c');
  dc.DrawText(text, cloc);
  }   
}


/*****
Draw the lines for a tmPath.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Lines, tmPath>(
  wxDC& dc, tmPath* aPath, bool isSelected)
{
  // Draw the path lines, always if selected
  if (mViewSettings.mShowPathLines || isSelected) {
    wxColor theColor = GetPartColor<Lines>(aPath, isSelected);
    int theWidth = 
      !isSelected ? STD_WIDTH : STD_WIDTH + DELTA_WIDTH;
    dc.SetPen(wxPen(theColor, theWidth, wxSOLID));
    wxPoint pts[2];
    pts[0] = TreeToDC(CalcLoc(aPath->GetNodes().front()));
    pts[1] = TreeToDC(CalcLoc(aPath->GetNodes().back()));
    dc.DrawLines(2, pts);
  }
}


/*****
Return the base color for drawing this tmPoly.
*****/
template <>
wxColor tmwxDesignCanvas::GetBasePartColor<tmPoly>(tmPoly*)
{
  return POLY_COLOR;
}


/*****
Draw the dot and text for a tmPoly.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmPoly>(
  wxDC& dc, tmPoly* aPoly, bool isSelected)
{
  // Dot  
  wxColor theColor = GetPartColor<Text>(aPoly, isSelected);
  int theWidth = 
    !isSelected ? DOT_SIZE : DOT_SIZE + DELTA_WIDTH;
  wxPoint cloc = TreeToDC(CalcLoc(aPoly));
  if (mViewSettings.mShowPolyDots || isSelected) 
    DrawDot(dc, cloc, theColor, theWidth);
  // Text
  dc.SetTextForeground(theColor);
  cloc.x += 5;
  cloc.y -= (mFontH - mFontD);
  wxString text;
  // Draw the index
  if (mViewSettings.mShowPolyIndices) {
    text = tmwxStr(aPoly);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw flags
  if (mViewSettings.mShowPolyFlags) {
    text = wxEmptyString;
    text += ((aPoly->IsSubPoly()) ? 'S' : 's');
  dc.DrawText(text, cloc);
  }   
}


/*****
Draw the lines for a tmPoly.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Lines, tmPoly>(
  wxDC& dc, tmPoly* aPoly, bool isSelected)
{
  // Draw poly outlines
  if (mViewSettings.mShowPolyLines) {
    wxColor theColor = GetPartColor<Lines>(aPoly, isSelected);
    int theWidth = 
      !isSelected ? STD_WIDTH : STD_WIDTH + DELTA_WIDTH;
    dc.SetPen(wxPen(theColor, theWidth, wxSOLID));
    size_t numPts = aPoly->GetRingNodes().size();
    wxPoint* pts = new wxPoint[numPts + 1];
    for (size_t i = 0; i < numPts; ++i)
      pts[i] = TreeToDC(CalcLoc(aPoly->GetRingNodes()[i]));
    pts[numPts] = pts[0];
    dc.DrawLines(int(numPts + 1), pts);
    delete[] pts;
  }
}


/*****
Draw the fill for a tmPoly.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Fill, tmPoly>(
  wxDC& dc, tmPoly* aPoly, bool isSelected)
{
  // Draw poly fills, always if selected
  if (mViewSettings.mShowPolyFills || isSelected) {
    wxColor theColor = GetPartColor<Fill>(aPoly, isSelected);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(theColor, wxSOLID));
    size_t n = aPoly->GetRingNodes().size();
    wxPoint* pts = new wxPoint[n];
    for (size_t i = 0; i < n; ++i) 
      pts[i] = TreeToDC(CalcLoc(aPoly->GetRingNodes()[i]));
    dc.DrawPolygon(int(n), pts);
    delete[] pts;
  }
}


/*****
Return the base color for drawing this tmVertex.
*****/
template <>
wxColor tmwxDesignCanvas::GetBasePartColor<tmVertex>(tmVertex*)
{
  return VERTEX_COLOR;
}


/*****
Draw the dot and text for a tmVertex.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmVertex>(
  wxDC& dc, tmVertex* aVertex, bool isSelected)
{
  // Dot
  wxColor theColor = GetPartColor<Text>(aVertex, isSelected);
  int theWidth = 
    !isSelected ? DOT_SIZE : DOT_SIZE + DELTA_WIDTH;

#if HIGHLIGHT_LOCAL_ROOTS
  int EXTRA = 0;
  if (!mViewSettings.mShowCreaseFolds) {
    for (size_t i = 0; i < GetTree()->GetOwnedPolys().size(); ++i) {
      tmPoly* thePoly = GetTree()->GetOwnedPolys()[i];
      if (thePoly->GetLocalRootVertices().contains(aVertex)) EXTRA = 4;
    }
    theWidth += EXTRA;
  }
#endif

  tmPoint tloc = CalcLoc(aVertex);
  wxPoint cloc = TreeToDC(tloc);
  if (mViewSettings.mShowVertexDots || isSelected) 
    DrawDot(dc, cloc, theColor, theWidth);
  // Text
  dc.SetTextForeground(theColor);
  cloc.x += 5;
  cloc.y -= (mFontH - mFontD);
  wxString text;
  // Draw the index
  if (mViewSettings.mShowVertexIndices) {
    text = tmwxStr(aVertex);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw coordinates
  if (mViewSettings.mShowVertexCoords) {
    text.Printf(wxT("(%.4f,"), tloc.x);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
    text.Printf(wxT(" %.4f)"), tloc.y);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw elevation
  if (mViewSettings.mShowVertexElevation) {
    text.Printf(wxT("E:%.4f"), aVertex->GetElevation());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw depth
  if (mViewSettings.mShowVertexDepth) {
    text.Printf(wxT("D:%.4f"), aVertex->GetDepth());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw discrete depth, if appropriate
  if (mViewSettings.mShowVertexDiscreteDepth) {
    text = wxT("DD:");
    text += tmwxStr(aVertex->GetDiscreteDepth());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw tree node, if appropriate
  if (mViewSettings.mShowVertexTreeNode) {
    text = tmwxStr(aVertex->GetTreeNode());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw flags
  if (mViewSettings.mShowVertexFlags) {
    text = wxEmptyString;
    text += ((aVertex->IsBorderVertex()) ? 'B' : 'b');
    dc.DrawText(text, cloc);
  }   
}


/*****
Return the appropriate color for the crease as a function of its kind or fold
type, depending on the view settings choice.
*****/
template <>
wxColor tmwxDesignCanvas::GetBasePartColor<tmCrease>(tmCrease* aCrease)
{
  if (mViewSettings.mShowCreaseFolds)
    // Use MVF coloring
    switch (aCrease->GetFold()) {
      case tmCrease::FLAT:
        return CREASE_FLAT_COLOR;
      case tmCrease::MOUNTAIN:
        return CREASE_MOUNTAIN_COLOR;
      case tmCrease::VALLEY:
        return CREASE_VALLEY_COLOR;
      case tmCrease::BORDER:
        return CREASE_BORDER_COLOR;
      default:
        TMFAIL("unknown Fold in "\
          "tmwxDesignCanvas::GetBasePartColor<tmCrease>()");
        return *wxBLACK;
    }
  else
    // Use AGRH coloring
    switch (aCrease->GetKind()) {
      case tmCrease::AXIAL:
        return CREASE_AXIAL_COLOR;
      case tmCrease::GUSSET:
        return CREASE_GUSSET_COLOR;
      case tmCrease::RIDGE:
        return CREASE_RIDGE_COLOR;
      case tmCrease::UNFOLDED_HINGE:
        return CREASE_UNFOLDED_HINGE_COLOR;
      case tmCrease::FOLDED_HINGE:
        return CREASE_FOLDED_HINGE_COLOR;
      case tmCrease::PSEUDOHINGE:
        return CREASE_PSEUDOHINGE_COLOR;
      default:
        TMFAIL("unknown Kind in "\
          "tmwxDesignCanvas::GetBasePartColor<tmCrease>()");
        return *wxBLACK;
    }
}


/*****
Draw the dot and text for a tmCrease.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmCrease>(
  wxDC& dc, tmCrease* aCrease, bool isSelected)
{
  // Dot
  wxColor theColor = GetPartColor<Text>(aCrease, isSelected);
  int theWidth = 
    !isSelected ? DOT_SIZE : DOT_SIZE + DELTA_WIDTH;
  wxPoint cloc = TreeToDC(CalcLoc(aCrease));
  if (mViewSettings.mShowCreaseDots || isSelected) 
    DrawDot(dc, cloc, theColor, theWidth);
  // Text
  dc.SetTextForeground(theColor);
  cloc.x += 5;
  cloc.y -= (mFontH - mFontD);
  wxString text;
  // Draw the index
  if (mViewSettings.mShowCreaseIndices) {
    text = tmwxStr(aCrease);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Draw flags
  if (mViewSettings.mShowCreaseFlags) {
    text = wxEmptyString;
    switch (aCrease->GetKind()) {
      case tmCrease::AXIAL: text += 'A'; break;
      case tmCrease::GUSSET: text += 'G'; break;
      case tmCrease::RIDGE: text += 'R'; break;
      case tmCrease::UNFOLDED_HINGE: text += wxT("UH"); break;
      case tmCrease::FOLDED_HINGE: text += wxT("FH"); break;
      case tmCrease::PSEUDOHINGE: text += wxT("PH"); break;
      default: TMFAIL("unknown Kind in "\
        "tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmCrease>()");
    }
    text += '-';
    switch (aCrease->GetFold()) {
      case tmCrease::FLAT: text += 'F'; break;
      case tmCrease::MOUNTAIN: text += 'M'; break;
      case tmCrease::VALLEY: text += 'V'; break;
      case tmCrease::BORDER: text += 'B'; break;
      default: TMFAIL("unknown Fold in "\
        "tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmCrease>()");
    }
    dc.DrawText(text, cloc);
  }   
}


/*****
Draw the lines for a tmCrease.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Lines, tmCrease>(
  wxDC& dc, tmCrease* aCrease, bool isSelected)
{
  // Draw crease lines, always if selected
  if (mViewSettings.mShowCreaseLines || isSelected) {
    wxColor theColor = GetPartColor<Lines>(aCrease, isSelected);
    int theWidth = 
      !isSelected ? STD_WIDTH : STD_WIDTH + DELTA_WIDTH;
#ifdef __WXMSW__
    if (mPrinting) {
      theWidth = max_val( PixelsToDC( theWidth ) / 2 , theWidth );
    }
#endif

#if HIGHLIGHT_LOCAL_ROOTS
    int EXTRA = 0;
    if (!mViewSettings.mShowCreaseFolds) {
      for (size_t i = 0; i < GetTree()->GetOwnedPolys().size(); ++i) {
        tmPoly* thePoly = GetTree()->GetOwnedPolys()[i];
        if (thePoly->GetLocalRootCreases().contains(aCrease)) EXTRA = 2;
      }
      theWidth += EXTRA;
    }
#endif

    if (mViewSettings.mShowCreaseFolds && 
      (aCrease->GetFold() == tmCrease::VALLEY)) {
      wxPen thePen(theColor, theWidth, wxUSER_DASH);
      thePen.SetDashes(2, DC_VALLEY_DASHES);
      dc.SetPen(thePen);
    }
    else
      dc.SetPen(wxPen(theColor, theWidth, wxSOLID));
    wxPoint pts[2];
    pts[0] = TreeToDC(CalcLoc(aCrease->GetFrontVertex()));
    pts[1] = TreeToDC(CalcLoc(aCrease->GetBackVertex()));
    dc.DrawLines(2, pts);
  }
}


/*****
Return the base color for drawing this tmFacet.
*****/
template <>
wxColor tmwxDesignCanvas::GetBasePartColor<tmFacet>(tmFacet* aFacet)
{
  switch(aFacet->GetColor()) {
    case tmFacet::NOT_ORIENTED:
      return FACET_NOT_ORIENTED_COLOR;
    case tmFacet::WHITE_UP:
      return FACET_WHITE_UP_COLOR;
    case tmFacet::COLOR_UP:
      return FACET_COLOR_UP_COLOR;
    default:
      TMFAIL("unknown Color in "\
        "tmwxDesignCanvas::GetBasePartColor<tmFacet>()");
  }
  return *wxBLACK;
}


/*****
Draw the dot and text for a tmFacet.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmFacet>(
  wxDC& dc, tmFacet* aFacet, bool isSelected)
{
  // Dot
  wxColor theColor = GetPartColor<Text>(aFacet, isSelected);
  int theWidth = 
    !isSelected ? DOT_SIZE : DOT_SIZE + DELTA_WIDTH;

#if IDENTIFY_SRCS_SINKS
  if (!mViewSettings.mShowCreaseFolds) {
    bool isSrc = (aFacet->IsSourceFacet());
    bool isSink = (aFacet->IsSinkFacet());
    if (isSrc || isSink) theWidth += 4;
    if (isSrc) theColor = Darken(theColor, .5);
    if (isSink) theColor = Lighten(theColor, .5);
  }
#endif

  wxPoint cloc = TreeToDC(CalcLoc(aFacet));
  if (mViewSettings.mShowFacetDots || isSelected) 
    DrawDot(dc, cloc, theColor, theWidth);
  // Prepare to draw text information.
  dc.SetTextForeground(theColor);
  cloc.x += 5;
  cloc.y -= (mFontH - mFontD);
  wxString text;
  // Draw the index
  if (mViewSettings.mShowFacetIndices) {
    text = tmwxStr(aFacet);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
   // Draw order
  if (mViewSettings.mShowFacetOrder) {
    text = wxT("O: ");
    text += tmwxStr(aFacet->GetOrder());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }   
  // Draw corridor edge
  if (mViewSettings.mShowFacetCorridorEdge) {
    text = wxT("CE: ");
    text += tmwxStr(aFacet->GetCorridorEdge());
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
 }
 // Draw flags
  if (mViewSettings.mShowFacetFlags) {
    text = wxEmptyString;
    switch (aFacet->GetColor()) {
      case tmFacet::NOT_ORIENTED: text += wxT("-"); break;
      case tmFacet::WHITE_UP: text += wxT("W"); break;
      case tmFacet::COLOR_UP: text += wxT("C"); break;
      default: 
        TMFAIL("unknown color in "\
          "tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmFacet>()");
    }
    if (!aFacet->IsWellFormed())
      text += wxT(".NWF");
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }   
}


/*****
Draw the lines for a tmFacet.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Lines, tmFacet>(
  wxDC& dc, tmFacet* aFacet, bool)
{
  // Draw ordering arrows to all head facets. Ordering arrows are natural cubic
  // splines that pass through the common crease if the two facets are adjacent.
  if (mViewSettings.mShowFacetArrows) {
    int theWidth = 
      STD_WIDTH;
    dc.SetPen(wxPen(FACET_LINE_COLOR, theWidth, wxSOLID));
    // Draw arrows to all this facet's head facets.
    for (size_t i = 0; i < aFacet->GetHeadFacets().size(); ++i) {
      tmFacet* headFacet = aFacet->GetHeadFacets()[i];
      // Get control points for ordering arrow
      tmPoint p1 = aFacet->GetCentroid();
      tmPoint p3 = headFacet->GetCentroid();
      tmPoint p2;       // midpoint of arrow in tree coordinates
      wxPoint arrowTip; // same thing in canvas coordinates
      tmCrease* crossCrease = GetTree()->GetCrease(aFacet, headFacet);
      if (crossCrease) {
        // If facets are adjacent, the midpoint of the arrow is the midpoint of
        // the separating crease, and for the arrow we draw a natural cubic
        // spline from the tail facet through the midpoint to the head facet.
        // p2c is the control point that puts the spline through p2.
        p2 = 0.5 * (crossCrease->GetFrontVertex()->GetLoc() +
          crossCrease->GetBackVertex()->GetLoc());
        tmPoint p2c = 1.3333 * p2 - 0.1667 * (p1 + p3);
        arrowTip = TreeToDC(p2);
        wxPoint pts[3] = {TreeToDC(p1), TreeToDC(p2c), 
          TreeToDC(p3)};
        dc.DrawSpline(3, pts);
      }
      else {
        // If, on the other hand, the facets are NOT adjacent, then the midpoint
        // of the arrow is just the midpoint of the line from one facet to the
        // other, and we draw the arrow as a single line segment.
        p2 = 0.5 * (p1 + p3);
        arrowTip = TreeToDC(p2);
        wxPoint pts[2] = {TreeToDC(p1), TreeToDC(p3)};
        dc.DrawLines(2, pts);
      }
      // Put arrowhead in middle of spline (to avoid crowding at endpoints).
      tmPoint dir = Normalize(p3 - p1);
      dir.x *= -1.;
      const tmFloat ARROW_SIZE_DC = tmFloat(PixelsToDC(ARROW_SIZE));
      const tmFloat ARROW_ANGLE_DEGREES = ARROW_ANGLE * DEGREES;
      tmPoint dir1 = RotateCCW(dir, ARROW_ANGLE_DEGREES);
      tmPoint dir2 = RotateCW(dir, ARROW_ANGLE_DEGREES);
      wxPoint arrow[3] = {
        wxPoint(arrowTip.x + int(ARROW_SIZE_DC * dir1.x), 
          arrowTip.y + int(ARROW_SIZE_DC * dir1.y)),
        arrowTip,
        wxPoint(arrowTip.x + int(ARROW_SIZE_DC * dir2.x), 
          arrowTip.y + int(ARROW_SIZE_DC * dir2.y))
      };
      dc.DrawLines(3, arrow);
    }
  }
}


/*****
Draw the fill for a tmFacet.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Fill, tmFacet>(
  wxDC& dc, tmFacet* aFacet, bool isSelected)
{
  // Draw facet fills, always if selected
  if (mViewSettings.mShowFacetFills || isSelected) {
    wxColor theColor = GetPartColor<Fill>(aFacet, isSelected);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(theColor, wxSOLID));
    size_t n = aFacet->GetVertices().size();
    wxPoint* pts = new wxPoint[n];
    for (size_t i = 0; i < n; ++i) 
      pts[i] = TreeToDC(CalcLoc(aFacet->GetVertices()[i]));
    dc.DrawPolygon(int(n), pts);
    delete[] pts;
  }
}


/*****
Return the base color for drawing this tmCondition.
*****/
template <>
wxColor tmwxDesignCanvas::GetBasePartColor<tmCondition>(tmCondition* aCondition)
{
  if (aCondition->IsFeasibleCondition()) return CONDITION_COLOR;
  else return CONDITION_INFEASIBLE_COLOR;
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionNodeCombo.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionNodeCombo>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionNodeCombo* c = dynamic_cast<tmConditionNodeCombo*>(p);
  TMASSERT(c);
  wxString text(wxT("pos-"));
  if (c->GetToSymmetryLine()) text += wxT("s");
  if (c->GetToPaperEdge()) text += wxT("e");
  if (c->GetToPaperCorner()) text += wxT("c");
  if (c->GetXFixed()) text += wxT("x");
  if (c->GetYFixed()) text += wxT("y");
  dc.DrawText(text, cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionNodeFixed.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionNodeFixed>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionNodeFixed* c = dynamic_cast<tmConditionNodeFixed*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("fixed(v4)"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionNodeOnCorner.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionNodeOnCorner>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionNodeOnCorner* c = dynamic_cast<tmConditionNodeOnCorner*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("corner(v4)"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionNodeOnEdge.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionNodeOnEdge>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionNodeOnEdge* c = dynamic_cast<tmConditionNodeOnEdge*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("edge(v4)"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionNodeSymmetric.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionNodeSymmetric>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionNodeSymmetric* c = dynamic_cast<tmConditionNodeSymmetric*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("symmetric(v4)"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionNodesPaired.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionNodesPaired>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionNodesPaired* c = dynamic_cast<tmConditionNodesPaired*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("paired"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionNodesCollinear.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionNodesCollinear>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionNodesCollinear* c = dynamic_cast<tmConditionNodesCollinear*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("collinear"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionEdgeLengthFixed.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionEdgeLengthFixed>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionEdgeLengthFixed* c = dynamic_cast<tmConditionEdgeLengthFixed*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("fixed"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionEdgesSameStrain.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionEdgesSameStrain>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionEdgesSameStrain* c = dynamic_cast<tmConditionEdgesSameStrain*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("same"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionPathCombo.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionPathCombo>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionPathCombo* c = dynamic_cast<tmConditionPathCombo*>(p);
  TMASSERT(c);
  wxString text(wxT("active"));
  if (c->IsAngleFixed()) text += wxT("+f");
  if (c->IsAngleQuant()) text += wxT("+q");
  dc.DrawText(text, cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionPathActive.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionPathActive>(wxDC& dc, 
tmPart* p, const wxPoint& cloc)
{
  tmConditionPathActive* c = dynamic_cast<tmConditionPathActive*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("active(v4)"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionPathAngleFixed.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionPathAngleFixed>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionPathAngleFixed* c = dynamic_cast<tmConditionPathAngleFixed*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("fixed(v4)"), cloc);
}


/*****
Draw the text unique to each type of condition. Specialization for
tmConditionPathAngleQuant.
*****/
template <>
void tmwxDesignCanvas::DrawConditionText<tmConditionPathAngleQuant>(wxDC& dc, 
  tmPart* p, const wxPoint& cloc)
{
  tmConditionPathAngleQuant* c = dynamic_cast<tmConditionPathAngleQuant*>(p);
  TMASSERT(c);
  dc.DrawText(wxT("quant(v4)"), cloc);
}


/*****
Draw the dot and text for a tmCondition.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Text, tmCondition>(
  wxDC& dc, tmCondition* aCondition, bool isSelected)
{
  // Dot
  wxColor theColor = GetPartColor<Text>(aCondition, isSelected);
  int theWidth = 
    !isSelected ? CONDITION_WIDTH : CONDITION_WIDTH + DELTA_WIDTH;
  wxPoint cloc = TreeToDC(CalcLoc(aCondition));
  if (mViewSettings.mShowConditionDots || isSelected) 
    DrawDot(dc, cloc, theColor, theWidth);
  // Text
  dc.SetTextForeground(theColor);
  cloc.x += 5;
  cloc.y -= (mFontH - mFontD);
  wxString text;
  // Draw the index
  if (mViewSettings.mShowConditionIndices) {
    text = tmwxStr(aCondition);
    dc.DrawText(text, cloc);
    cloc.y += mFontH;
  }
  // Condition type-dependent text
  if (mViewSettings.mShowConditionTypes) {
    (this->*GetDrawConditionTextFns()[aCondition->GetTag()])(dc, aCondition, cloc);
    cloc.y += mFontH;
  }
}


/*****
Draw connector to the affected tmNode.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionNodeCombo>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionNodeCombo* c = dynamic_cast<tmConditionNodeCombo*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode())));
}


/*****
Draw connector to the affected tmNode.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionNodeFixed>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionNodeFixed* c = dynamic_cast<tmConditionNodeFixed*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode())));
}


/*****
Draw connector to the affected tmNode.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionNodeOnCorner>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionNodeOnCorner* c = dynamic_cast<tmConditionNodeOnCorner*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode())));
}


/*****
Draw connector to the affected tmNode.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionNodeOnEdge>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionNodeOnEdge* c = dynamic_cast<tmConditionNodeOnEdge*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode())));
}


/*****
Draw connector to the affected tmNode(s).
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionNodesCollinear>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionNodesCollinear* c = dynamic_cast<tmConditionNodesCollinear*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode1())));
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode2())));
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode3())));
}


/*****
Draw connector to the affected tmNode(s).
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionNodesPaired>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionNodesPaired* c = dynamic_cast<tmConditionNodesPaired*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode1())));
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode2())));
}


/*****
Draw connector to the affected tmNode.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionNodeSymmetric>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionNodeSymmetric* c = dynamic_cast<tmConditionNodeSymmetric*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetNode())));
}


/*****
Draw connector to the affected tmEdge.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionEdgeLengthFixed>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionEdgeLengthFixed* c = 
    dynamic_cast<tmConditionEdgeLengthFixed*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetEdge())));
}


/*****
Draw connector to the affected tmEdge(s).
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionEdgesSameStrain>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionEdgesSameStrain* c = 
    dynamic_cast<tmConditionEdgesSameStrain*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetEdge1())));
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetEdge2())));
}


/*****
Draw connector to the affected tmPath.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionPathCombo>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionPathCombo* c = 
    dynamic_cast<tmConditionPathCombo*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetPath())));
}


/*****
Draw connector to the affected tmPath.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionPathActive>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionPathActive* c = 
    dynamic_cast<tmConditionPathActive*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetPath())));
}


/*****
Draw connector to the affected tmPath.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionPathAngleFixed>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionPathAngleFixed* c = 
    dynamic_cast<tmConditionPathAngleFixed*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetPath())));
}


/*****
Draw connector to the affected tmPath.
*****/
template <>
void tmwxDesignCanvas::DrawConditionLines<tmConditionPathAngleQuant>(wxDC& dc, 
  tmPart* aPart, bool, const wxPoint& cloc)
{
  tmConditionPathAngleQuant* c = 
    dynamic_cast<tmConditionPathAngleQuant*>(aPart);
  TMASSERT(c);
  dc.DrawLine(cloc, TreeToDC(CalcLoc(c->GetPath())));
}


/*****
Draw the lines for a tmCondition.
*****/
template <>
void tmwxDesignCanvas::DrawPart<tmwxDesignCanvas::Lines, tmCondition>(
  wxDC& dc, tmCondition* aCondition, bool isSelected)
{
  // Draw condition lines from condition to affected part(s), always if selected
  if (mViewSettings.mShowConditionLines || isSelected) {
    wxColor theColor = GetPartColor<Lines>(aCondition, isSelected);
    int theWidth = !isSelected ? CONDITION_CONN_WIDTH : 
      CONDITION_CONN_WIDTH + DELTA_WIDTH;
    dc.SetPen(wxPen(theColor, theWidth, wxSOLID));
    wxPoint cloc = TreeToDC(CalcLoc(aCondition));
    (this->*GetDrawConditionLinesFns()[aCondition->GetTag()])(
      dc, aCondition, isSelected, cloc);
  }
}


/*****
STATIC
Make if necessary and return our array of condition line drawing functions
*****/
tmArray<tmwxDesignCanvas::DrawConditionLinesFn>& 
  tmwxDesignCanvas::GetDrawConditionLinesFns()
{
  static tmArray<DrawConditionLinesFn> sConditionLinesFns;
  if (sConditionLinesFns.empty()) {
    TMASSERT(tmPart::TypesAreInitialized());
    tmPart::MakeTypeArray<DrawConditionLinesFn, DrawConditionLinesFnT>(
      sConditionLinesFns);
  }
  return sConditionLinesFns;
}


/*****
STATIC
Make if necessary and return our array of condition text drawing functions.
*****/
tmArray<tmwxDesignCanvas::DrawConditionTextFn>& 
  tmwxDesignCanvas::GetDrawConditionTextFns()
{
  static tmArray<DrawConditionTextFn> sDrawConditionTextFns;
  if (sDrawConditionTextFns.empty()) {
    TMASSERT(tmPart::TypesAreInitialized());
    tmPart::MakeTypeArray<DrawConditionTextFn, DrawConditionTextFnT>(
      sDrawConditionTextFns);
  }
  return sDrawConditionTextFns;
}


/*****
Draw {S = Text, Lines, or Fill} for all the parts of the part type P
*****/
template <class S, class P>
void tmwxDesignCanvas::DrawAllParts(wxDC& dc, const tmDpptrArray<P>& partList)
{
  for (size_t i = 0; i < partList.size(); ++i) {
    P* p = partList[i];
    if (IsVisible(p)) DrawPart<S, P>(dc, p, mDoc->mSelection.Contains(p));
  }
}


/*****
Draw everything. The wxDC could be (1) a portion of the screen display, (2) the
Print Preview image, or (3) a wxPrinterDC. In the latter two cases, we modify
the image for printing, leaving out the background, showing some additional
text information, and most importantly, scaling the image so that the printed
image maximally fills up the printed page. We do our own scaling to the DC,
rather than using DC::SetUserScale(), because this lets us perform all our
scaling in floating-point math, giving more accurate conversion at high print
resolution.
*****/
void tmwxDesignCanvas::OnDraw(wxDC& dc)
{
  mPaperSizeDC = mPaperSizeScreen;
  if (mPrinting) {
    // Printing images should be scaled to exactly fit the DC
    int dcWidth, dcHeight;
    dc.GetSize(&dcWidth, &dcHeight);
    int imageWidth = LEFT_BORDER + TreeToDC(GetTree()->GetPaperWidth()) + 
      RIGHT_BORDER;
    int imageHeight = TOP_BORDER + TreeToDC(GetTree()->GetPaperHeight()) + 
      BOTTOM_BORDER;
    double xscale = double(dcWidth) / imageWidth;
    double yscale = double(dcHeight) / imageHeight;
    mDCScale = min_val(xscale, yscale);
    mPaperSizeDC = PixelsToDC(mPaperSizeScreen);
  }
  else {
    // For non-printing images, the DC scale is the same as screen scale
    mDCScale = 1.0;
  }
  
  // Scale the dashing to use for valley lines
  for (size_t i = 0; i < 2; ++i)
    DC_VALLEY_DASHES[i] = PixelsToDC(VALLEY_DASHES[i]);

  // Set the font and size for all drawing and record its metrics in member
  // variables
  wxFont theFont;
  theFont.SetFamily(wxSWISS);
  theFont.SetPointSize(PixelsToDC(LABEL_TEXT_SIZE));
  dc.SetFont(theFont);
  wxString text = wxT("m");
  dc.GetTextExtent(text, &mFontW, &mFontH, &mFontD);

  // Redraw the background because antialiased text will look funny if we don't.
  // But if we're printing, we don't need to draw the background at all.
  if (!mPrinting) {
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(BACKGROUND_COLOR, wxSOLID));
    wxCoord w, h;
    GetVirtualSize(&w, &h);
    dc.DrawRectangle(0, 0, w, h);
  };
  
  DrawPaper<Fill>(dc);
  DrawAllParts<Fill>(dc, GetTree()->GetParts<tmPoly>());
  DrawAllParts<Fill>(dc, GetTree()->GetParts<tmFacet>());

  DrawPaper<Lines>(dc);
  DrawAllParts<Lines>(dc, GetTree()->GetParts<tmPoly>());
  DrawAllParts<Lines>(dc, GetTree()->GetParts<tmPath>());
  DrawAllParts<Lines>(dc, GetTree()->GetParts<tmEdge>());
#ifndef __WXGTK__
  // Node circles are clipped, but on wxGTK clipping has to come last
  {
    wxRect clipRect(TreeToDC(tmPoint(0.0, GetTree()->GetPaperHeight())), 
      TreeToDC(tmPoint(GetTree()->GetPaperWidth(), 0.0)));
    wxDCClipper dcc(dc, clipRect);
    DrawAllParts<Lines>(dc, GetTree()->GetParts<tmNode>()); // includes circles
  }
#endif // __WXGTK__
  DrawAllParts<Lines>(dc, GetTree()->GetParts<tmFacet>());
  DrawAllParts<Lines>(dc, GetTree()->GetParts<tmCrease>());
  DrawAllParts<Lines>(dc, GetTree()->GetParts<tmCondition>());

  DrawAllParts<Text>(dc, GetTree()->GetParts<tmPoly>());
  DrawAllParts<Text>(dc, GetTree()->GetParts<tmPath>());
  DrawAllParts<Text>(dc, GetTree()->GetParts<tmEdge>());
  DrawAllParts<Text>(dc, GetTree()->GetParts<tmNode>());
  DrawAllParts<Text>(dc, GetTree()->GetParts<tmFacet>());
  DrawAllParts<Text>(dc, GetTree()->GetParts<tmCrease>());
  DrawAllParts<Text>(dc, GetTree()->GetParts<tmVertex>());
  DrawAllParts<Text>(dc, GetTree()->GetParts<tmCondition>());
  
  // Paper text gets drawn last; this includes the printing header
  wxFont headerFont;
  headerFont.SetFamily(wxSWISS);
  headerFont.SetPointSize(PixelsToDC(HEADER_TEXT_SIZE));
  headerFont.SetWeight(wxBOLD);
  dc.SetFont(headerFont);
  DrawPaper<Text>(dc);
  dc.SetFont(theFont);  

#ifdef TM_WITH_RANGE_SELECTION
  if (!mPrinting &&
       mDragging &&
      (mMovingNodes.size() == 0) &&
      (mDragOffset.x != 0 || mDragOffset.y != 0)) {
    dc.SetPen(wxPen(wxT("BLACK"), 1, wxDOT_DASH));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(mDragStart.x, mDragStart.y, mDragOffset.x, mDragOffset.y);
  }
#endif // TM_WITH_RANGE_SELECTION

#ifdef __WXGTK__
  // Clipped node circles come last under GTK because wxW can't turn off
  // clipping
  {
    wxRect clipRect(TreeToDC(tmPoint(0.0, GetTree()->GetPaperHeight())), 
      TreeToDC(tmPoint(GetTree()->GetPaperWidth(), 0.0)));
    wxDCClipper dcc(dc, clipRect);
    DrawAllParts<Lines>(dc, GetTree()->GetParts<tmNode>());
  }
#endif // __WXGTK__

  // Restore mPaperSizeDC to screen values so that mouse calculations involving
  // TreeToDC or DCToTree use the right value.
  mPaperSizeDC = mPaperSizeScreen;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return true if the click is within CLICK_DIST pixels of point q.
*****/
bool tmwxDesignCanvas::ClickOnPoint(const wxPoint& where, const tmPoint& q)
{
  tmPoint p = DCToTree(where);
  tmFloat d = Mag(p - q) * mPaperSizeScreen;
  return (d < CLICK_DIST);
}
    

/*****
Return true if the click is within CLICK_DIST pixels of the line from q1 to q2.
*****/
bool tmwxDesignCanvas::ClickOnLine(const wxPoint& where, const tmPoint& q1, 
  const tmPoint& q2)
{
  tmPoint p = DCToTree(where) - q1;
  
  // check distance parallel to line segment
  tmPoint qu = q2 - q1;
  tmFloat u = Inner(p, qu) / Mag2(qu);
  if ((u < 0) || (u > 1)) return false;
  
  // Check perpendicular distance
  tmPoint qv(qu.y, -qu.x);
  tmFloat v = Inner(p, qv) / Mag(qv);
  if (fabs(v * mPaperSizeScreen) < CLICK_DIST) return true;
  return false;
}


/*****
If we clicked on a tmNode, return a ptr to the tmNode. Otherwise return a null
ptr.
*****/
template <>
tmNode* tmwxDesignCanvas::ClickOn<tmNode>(const wxPoint& where)
{
  tmArrayIterator<tmNode*> iNodes(GetTree()->GetNodes());
  tmNode* clickedNode;
  while (iNodes.Next(&clickedNode))
    if ((IsVisible(clickedNode)) && 
      (ClickOnPoint(where, clickedNode->GetLoc())))
      return clickedNode;
  return NULL;
}
    

/*****
If we clicked on an tmEdge, return a ptr to the tmEdge. Otherwise return a null
ptr.
*****/
template <>
tmEdge* tmwxDesignCanvas::ClickOn<tmEdge>(const wxPoint& where)
{
  tmArrayIterator<tmEdge*> iEdges(GetTree()->GetEdges());
  tmEdge* clickedEdge;
  while (iEdges.Next(&clickedEdge))
    if (IsVisible(clickedEdge) && 
      ClickOnLine(where, clickedEdge->GetNodes().front()->GetLoc(),
      clickedEdge->GetNodes().back()->GetLoc())) return clickedEdge;
  return NULL;
}

  
/*****
If we clicked on a tmPath, return a ptr to the tmPath. Otherwise return a null
ptr.
*****/
template <>
tmPath* tmwxDesignCanvas::ClickOn<tmPath>(const wxPoint& where)
{
  tmArrayIterator<tmPath*> iPaths(GetTree()->GetPaths());
  tmPath* clickedPath;
  while (iPaths.Next(&clickedPath))
    if (IsVisible(clickedPath) && 
      ClickOnLine(where, clickedPath->GetNodes().front()->GetLoc(),
      clickedPath->GetNodes().back()->GetLoc())) return clickedPath;
  return NULL;
}

  
/*****
If we clicked on a tmPoly, return a ptr to the tmPoly. Otherwise return a null
ptr.
*****/
template <>
tmPoly* tmwxDesignCanvas::ClickOn<tmPoly>(const wxPoint& where)
{
  // First, the easy part; check for clicks on the control point.
  tmArrayIterator<tmPoly*> iPolys1(GetTree()->GetPolys());
  tmPoly* clickedPoly;
  while (iPolys1.Next(&clickedPoly))
    if (IsVisible(clickedPoly) && 
      ClickOnPoint(where, CalcLoc(clickedPoly))) return clickedPoly;
  if (!mViewSettings.mShowPolyFills) return NULL;
  
  // If we're showing fills, then we also need to check clicks in fills.  
  // Determining clicks on polys is tricky since a sub-poly can overlap
  // its parent poly. 
  tmPoint fp = DCToTree(where);
  
  tmArray<tmPoly*> hitPolys;
  
  tmArrayIterator<tmPoly*> iPolys(GetTree()->GetPolys());
  while (iPolys.Next(&clickedPoly))
    if (IsVisible(clickedPoly) && clickedPoly->ConvexEncloses(fp))
      hitPolys.push_back(clickedPoly);
  
  // What we do depends on how many we found. 
  if (hitPolys.empty()) {
    clickedPoly = 0;
    return NULL;
  }
  
  if (hitPolys.size() == 1) {
    clickedPoly = hitPolys.front();
    return clickedPoly;
  }
  
  // Oh-oh, one poly must enclose another. We'll repeatedly go through
  // the list and eliminate the outermost one.
    
  // Go through our list (which may have multiple polys) and eliminate
  // any polys that own (i.e., enclose) another.  
  tmArrayIterator<tmPoly*> iHitPolys(hitPolys);
  tmPoly* poly1;
  tmArrayIterator<tmPoly*> iHitPolys2(hitPolys);
  tmPoly* poly2;
  while (iHitPolys.Next(&poly1)) {
    while (iHitPolys2.Next(&poly2)) {
      if (poly1 == poly2) continue;
      if (poly1->SuperOwns(poly2)) {
        hitPolys.erase_remove(poly1);
        iHitPolys.ResetTo(tmArray_START);
        break;
      }
    }
  }
  clickedPoly = hitPolys.front();
  return clickedPoly; 
}

  
/*****
If we clicked on a tmVertex, return a ptr to the tmVertex. Otherwise return a
null ptr.
*****/
template <>
tmVertex* tmwxDesignCanvas::ClickOn<tmVertex>(const wxPoint& where)
{
  tmArrayIterator<tmVertex*> iVertices(GetTree()->GetVertices());
  tmVertex* clickedVertex;
  while (iVertices.Next(&clickedVertex))
    if (IsVisible(clickedVertex) && 
      ClickOnPoint(where, clickedVertex->GetLoc())) return clickedVertex;
  return NULL;
}

  
/*****
If we clicked on a tmCrease, return a ptr to the tmCrease. Otherwise return a
null ptr.
*****/
template <>
tmCrease* tmwxDesignCanvas::ClickOn<tmCrease>(const wxPoint& where)
{
  tmArrayIterator<tmCrease*> iCreases(GetTree()->GetCreases());
  tmCrease* clickedCrease;
  while (iCreases.Next(&clickedCrease))
    if (IsVisible(clickedCrease) && 
      ClickOnLine(where, clickedCrease->GetFrontVertex()->GetLoc(),
      clickedCrease->GetBackVertex()->GetLoc())) return clickedCrease;
  return NULL;
}

  
/*****
If we clicked on a tmFacet, return a ptr to the tmFacet. Otherwise return a
null ptr.
*****/
template <>
tmFacet* tmwxDesignCanvas::ClickOn<tmFacet>(const wxPoint& where)
{
  tmArrayIterator<tmFacet*> iFacets(GetTree()->GetFacets());
  tmFacet* clickedFacet;
  while (iFacets.Next(&clickedFacet))
    if (IsVisible(clickedFacet) && 
      ClickOnPoint(where, CalcLoc(clickedFacet))) return clickedFacet;
  if (!mViewSettings.mShowFacetFills) return NULL;
  
  // If fills are visible, we also need to check clicks in fills.
  if (!mViewSettings.mShowFacetFills) return NULL;
  tmPoint fp = DCToTree(where);
  for (size_t i = 0; i < GetTree()->GetFacets().size(); ++i) {
    tmFacet* theFacet = GetTree()->GetFacets()[i];
    if (theFacet->ConvexEncloses(fp)) return theFacet;
  }
  return NULL;
}

  
/*****
If we clicked on a tmCondition, return a ptr to the tmCondition. Otherwise
return a null ptr. Conditions allow you to click only on the condition point
*****/
template <>
tmCondition* tmwxDesignCanvas::ClickOn<tmCondition>(const wxPoint& where)
{
  tmArrayIterator<tmCondition*> iCondition(GetTree()->GetConditions());
  tmCondition* theCondition;
  while (iCondition.Next(&theCondition))
    if (IsVisible(theCondition) && 
      ClickOnPoint(where, CalcLoc(theCondition))) return theCondition;
  return NULL;
}


/*****
Respond to context menu
*****/
void tmwxDesignCanvas::OnContextMenu(wxContextMenuEvent& event)
{
  wxPoint point = event.GetPosition();
  // If from keyboard
  if (point.x == -1 && point.y == -1) {
    wxSize size = GetSize();
    point.x = size.x / 2;
    point.y = size.y / 2;
  } else {
    point = ScreenToClient(point);
  }

  PopupMenu(mDesignFrame->GetMenuBar()->GetMenu(1), point.x, point.y);
}

/*****
Respond to clicks and drags
*****/
void tmwxDesignCanvas::OnMouse(wxMouseEvent& event)
{
  if (!event.LeftDown() &&
      !event.Dragging() &&
      !event.LeftUp()) {
    event.Skip();
    return;
  }

  // Record for posterity whether click was shifted (used to extend a selection)
  // or otherwise modified (used to force dragging of pinned nodes).
  bool clickWasShifted = event.ShiftDown();
  bool clickWasModified =
#if defined(__WXMAC__) || defined(__WXMSW__)
    event.AltDown();
#else
    // LINUX & everyone else
    event.AltDown () || event.ControlDown();
#endif // defined(__WXMAC__) || defined(__WXMSW__)

  wxClientDC dc(this);
  PrepareDC(dc);

  if (event.LeftDown()) {
    //////////////////////////////////////////////////////////
    //
    //  INITIAL CLICK
    //
    //////////////////////////////////////////////////////////
    // Extend or reduce selection by clicked-on item. tmwxView
    // will take care of immediate redraw showing results of selection.
    // With overlapping items, the first clicked-on object gets the click.
    wxPoint where = event.GetLogicalPosition(dc);
    // first priority goes to point-like objects (tmNode, tmCondition, tmVertex)
    if (tmNode* aNode = ClickOn<tmNode>(where)) 
      mDoc->ExtendSelection(aNode, clickWasShifted);
    else if (tmCondition* aCondition = ClickOn<tmCondition>(where))
      mDoc->ExtendSelection(aCondition, clickWasShifted);
    else if (tmVertex* aVertex = ClickOn<tmVertex>(where)) 
      mDoc->ExtendSelection(aVertex, clickWasShifted);
    // second priority goes to line-like objects (tmEdge, tmPath, tmCrease)
    else if (tmEdge* aEdge = ClickOn<tmEdge>(where))
      mDoc->ExtendSelection(aEdge, clickWasShifted);
    else if (tmPath* aPath = ClickOn<tmPath>(where))
      mDoc->ExtendSelection(aPath, clickWasShifted);
    else if (tmCrease* aCrease = ClickOn<tmCrease>(where))
      mDoc->ExtendSelection(aCrease, clickWasShifted);
    // third priority goes to polygon-like objects (tmPoly, tmFacet)
    else if (tmFacet* aFacet = ClickOn<tmFacet>(where))
      mDoc->ExtendSelection(aFacet, clickWasShifted);
    else if (tmPoly* aPoly = ClickOn<tmPoly>(where))
      mDoc->ExtendSelection(aPoly, clickWasShifted);
    // If we didn't click on anything, design is empty, create a node
    else if (GetTree()->GetNodes().empty()) {
      tmNode* newNode;
      tmEdge* newEdge;
      GetTree()->AddNode(0, DCToTree(where), newNode, newEdge);
      mDoc->ExtendSelection(newNode, clickWasShifted);
      mDoc->SubmitCommand(wxT("Add Node"));
    }
    // If design is not empty, see if exactly one non-sub-node was 
    // previously selected.
    else {
      tmNode* selNode = mDoc->ExactlyOneNodeSelected();
      if (selNode && !selNode->IsSubNode()) {
        // If it was, then we add a new node at the click location.
        tmNode* newNode;
        tmEdge* newEdge;
        GetTree()->AddNode(selNode, DCToTree(where), newNode, newEdge);
        // And we add it to the selection, unless the modifier key was down,
        // which lets us repeatedly add edges to the same node.
        if (! clickWasModified)
          mDoc->ExtendSelection(newNode, clickWasShifted);
        mDoc->SubmitCommand(wxT("Add Node"));
     }
      else if (!clickWasShifted)
        // if there wasn't exactly one (non-sub-) node selected and we weren't
        // trying to extend the selection (with a shift-click), clear the
        // selection.
        mDoc->ClearSelection();
    }
    
    // Now, we prepare for a drag. We go through the selected nodes and Edges
    // and see if any of the selected parts can move.
    mMovingNodes.clear();
    tmDpptrArray<tmNode>& selectedNodes = mDoc->mSelection.GetNodes();
    for (size_t i = 0; i < selectedNodes.size(); ++i) {
      tmNode* aNode = selectedNodes[i];
      if (clickWasModified || !aNode->IsPinnedNode()) 
        mMovingNodes.push_back(aNode);
    }
    tmDpptrArray<tmEdge>& selectedEdges = mDoc->mSelection.GetEdges();
    for (size_t i = 0; i < selectedEdges.size(); ++i) {
      tmEdge* aEdge = selectedEdges[i];
      tmNode* aNode = aEdge->GetNodes().front();
      if (clickWasModified || !aNode->IsPinnedNode()) 
        mMovingNodes.union_with(aNode);
      aNode = aEdge->GetNodes().back();
      if (clickWasModified || !aNode->IsPinnedNode()) 
        mMovingNodes.union_with(aNode);
    }

#ifndef TM_WITH_RANGE_SELECTION
    // If nothing can move, then there's no value in continuing the drag so
    // return.
    if (mMovingNodes.empty()) return;
#endif // !TM_WITH_RANGE_SELECTION
    
    // initialize dragging with current position and zero offset
    mDragStart = where;
    mDragOffset = wxPoint(0, 0);
    // We'll want to handle all mouse events until the mouse button is released.
    CaptureMouse();
    mDragging = true;
  }
  else if (event.Dragging()) {
    //////////////////////////////////////////////////////////
    //
    //  DRAGGING (INCLUDES AUTO-SCROLLING OF WINDOW)
    //
    //////////////////////////////////////////////////////////

    int xv, yv;     // virtual size of image
    GetVirtualSize(&xv, &yv);
    
    int xc, yc;     // client size (visible area)
    GetClientSize(&xc, &yc);
    
    int xstart, ystart; // scroll position (origin of visible portion)
    GetViewStart(&xstart, &ystart);
    
    int xmax = xv - xc; // maximum value for xstart (min vals are both 0)
    int ymax = yv - yc;
    
    int xpos, ypos;   // position relative to visible origin
    event.GetPosition(&xpos, &ypos);
    
    const int SCROLL_DIST = 10; // scrolling increment in pixels
    int xnew = xstart;  // new scroll position
    int ynew = ystart;
    // Calculate new desired scroll position, clipped to acceptable range
    if (xpos < 0) {
      xnew = xstart - SCROLL_DIST;
      xnew = max_val(xnew, 0);
    }
    if (xpos > xc) {
      xnew = xstart + SCROLL_DIST;
      xnew = min_val(xnew, xmax);
    }
    if (ypos < 0) {
      ynew = ystart - SCROLL_DIST;
      ynew = max_val(ynew, 0);
    }
    if (ypos > yc) {
      ynew = ystart + SCROLL_DIST;
      ynew = min_val(ynew, ymax);
    }
    
    int dx = xnew - xstart; // scroll increment
    int dy = ynew - ystart;
    
    if (dx != 0 || dy != 0) {
      // update mDragOffset so that redrawing invoked by Scroll happens
      // properly.
      mDragOffset.x = event.GetLogicalPosition(dc).x - mDragStart.x + dx;
      mDragOffset.y = event.GetLogicalPosition(dc).y - mDragStart.y + dy;
      // Scroll to new position; the portion of the window uncovered by
      // scrolling gets redrawn immediately.
      Scroll(xnew, ynew);
    }
    else {  // no scrolling, just update mDragOffset
      mDragOffset.x = event.GetLogicalPosition(dc).x - mDragStart.x;
      mDragOffset.y = event.GetLogicalPosition(dc).y - mDragStart.y;
    }
    // Now do a total redraw to show the result of the drag.
    if(mDragging || dx != 0 || dy != 0 ) {
#ifndef __WXMAC__
      // double-buffering, trade some memory for flicker-free redraws
      wxSize bufferArea (xv, yv);
      wxBufferedDC bDC (&dc, bufferArea, wxBUFFER_VIRTUAL_AREA);
      OnDraw(bDC);
#else
      OnDraw(dc);
#endif
    }
  }
  else if (event.LeftUp()) {
    //////////////////////////////////////////////////////////
    //
    //  FINISHED WITH CLICK AND/OR DRAG
    //
    //////////////////////////////////////////////////////////
    // If we had moving nodes AND we dragged by some distance, update the tree
    // with new locations of dragged nodes.
    size_t numMovingNodes = mMovingNodes.size();
    if (numMovingNodes > 0 && (mDragOffset.x != 0 || mDragOffset.y != 0)) {
      // Move the nodes to their new location, which we do by making an array
      // of the new locations and passing it to a tmTree function.
      static tmArray<tmPoint> newLocs;
      newLocs.resize(numMovingNodes);
      for (size_t i = 0; i < numMovingNodes; ++i)
        newLocs[i] = CalcLoc(mMovingNodes[i]);
      GetTree()->SetNodeLocs(mMovingNodes, newLocs);
      // Clear the offset we used during dragging so that the redraw that
      // happens upon command submission is drawn without any offset.
      mDragOffset.x = mDragOffset.y = 0;
      mDoc->SubmitCommand(wxT("Drag"));
    }
#ifdef TM_WITH_RANGE_SELECTION
    // if we were doing selection, add interior of selection rectangle
    else if (numMovingNodes == 0 && (mDragOffset.x != 0 || mDragOffset.y != 0)) {
      wxPoint pt = mDragStart + mDragOffset;
      wxPoint pt1(min_val(mDragStart.x,pt.x),min_val(mDragStart.y,pt.y));
      wxPoint pt2(max_val(mDragStart.x,pt.x),max_val(mDragStart.y,pt.y));
      wxRect rect(pt1,pt2);
      tmArrayIterator<tmNode*> iNodes(GetTree()->GetNodes());
      tmNode* queryNode;
      bool isFirst = true;
      while (iNodes.Next(&queryNode)) {
        wxPoint loc = TreeToDC(queryNode->GetLoc());
        if (IsVisible(queryNode) && rect.Inside(loc))
          mDoc->ExtendSelection(queryNode, clickWasShifted || !isFirst);
        isFirst = false;
      }
      // also edges and other parts can be queried for adding to selection
      Refresh();
    }
#endif // TM_WITH_RANGE_SELECTION

    // Keep the focus to receive keyboard input (like tab or delete)
    SetFocus();
    // Finally, we can release the mouse and stop capturing all mouse events.
    if (mDragging) ReleaseMouse();
    mDragging = false;
    mDragOffset.x = mDragOffset.y = 0;
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Respond to keys while we're the focus
*****/
void tmwxDesignCanvas::OnKeyDown(wxKeyEvent& event)
{
  switch (event.GetKeyCode()) {
    // These keys simply clear the selection
    case WXK_TAB:
    case WXK_ESCAPE:
    case WXK_CANCEL:
    case WXK_RETURN:
      mDoc->ClearSelection();
      // selection change only, no command needed
      mDoc->UpdateAllViews();
      break;
    // These keys attempt to delete the current selection
    case WXK_DELETE:
    case WXK_CLEAR:
    case 8: // backspace
    case WXK_NUMPAD_DELETE:
      mDoc->DoKillSelection();
      break;
    // Pass other key events along to the next event handler
    default:
      event.Skip();
  }
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxDesignCanvas, wxScrolledWindow)
  EVT_CONTEXT_MENU(tmwxDesignCanvas::OnContextMenu)
  EVT_MOUSE_EVENTS(tmwxDesignCanvas::OnMouse)
  EVT_KEY_DOWN(tmwxDesignCanvas::OnKeyDown)
END_EVENT_TABLE()

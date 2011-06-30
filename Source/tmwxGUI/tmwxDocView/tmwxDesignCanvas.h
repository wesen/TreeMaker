/*******************************************************************************
File:         tmwxDesignCanvas.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker design view canvas class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXDESIGNCANVAS_H_
#define _TMWXDESIGNCANVAS_H_

// Common TreeMaker and wxWidgets headers
#include "tmHeader.h"

// TreeMaker classes
#include "tmPoint.h"
#include "tmDpptrArray.h"
#include "tmPart.h"

#include "tmwxViewSettings.h"

// Additional wxWidgets headers
#include "wx/docview.h"

// Forward declarations
#include "tmModel_fwd.h"

class tmwxDoc;
class tmwxDesignFrame;

/**********
class tmwxDesignCanvas
contains the scrolling part of the design view of a tmTree
**********/

class tmwxDesignCanvas: public wxScrolledWindow
{
public:
  // Constructor
  tmwxDesignCanvas(tmwxDesignFrame* aFrame, wxSize aSize);
    
  // View settings
  const tmwxViewSettings& GetViewSettings() const { return mViewSettings; };
  tmwxViewSettings& GetEditableViewSettings() { return mViewSettings; };
  void SetViewSettings(const tmwxViewSettings& viewSettings) {
    mViewSettings = viewSettings; };
  
  // tmTree object drawing
  tmTree* const GetTree() const;

  // Paper size
  tmFloat GetPaperSize() const { return mPaperSize; };
  void SetPaperSize(const tmFloat& newPaperSize);

  bool IsFitPaperSize() { return IsFitPaper(FITTING_SIZE); };
  bool IsFitPaperWidth() { return IsFitPaper(FITTING_WIDTH); };
  bool IsFitPaperHeight() { return IsFitPaper(FITTING_HEIGHT); };

  void FitPaperSize() { FitPaper(FITTING_SIZE); };
  void FitPaperWidth() { FitPaper(FITTING_WIDTH); };
  void FitPaperHeight() { FitPaper(FITTING_HEIGHT); };

  // Printing
  void SetPrinting(bool printing);
  
  // Event handling
  void OnDraw(wxDC& dc);    
  void OnMouse(wxMouseEvent& event);
  void OnContextMenu(wxContextMenuEvent& event);
  void OnKeyDown(wxKeyEvent& event);
  DECLARE_EVENT_TABLE()

private:
  tmwxDoc* mDoc;
  tmwxDesignFrame* mDesignFrame;
  tmwxViewSettings mViewSettings;
  enum FittingType {
    FITTING_SIZE = 0,
    FITTING_WIDTH,
    FITTING_HEIGHT
  };
  int mBorderScrollWidth;         // width of frame borders & scroll bar
  int mBorderScrollHeight;        // height of frame borders & scroll bar
  tmFloat mPaperSize;             // size of the paper in inches
  tmFloat mPaperSizeScreen;       // size of the paper in screen pixels
  tmFloat mPaperSizeDC;           // size of the paper in DC pixels
  tmFloat mDCScale;               // scaling factor from screen to DC
  wxCoord mFontW, mFontH, mFontD; // dimensions of font used for most text
  wxPoint mDragStart;             // beginning of a drag
  wxPoint mDragOffset;            // offset of a drag
  bool mDragging;                 // true if we're dragging
  bool mPrinting;                 // true if we're printing
  tmArray<const tmNode*> mMovingNodes;  // nodes that get dragged

  // Paper size
  void CalcBorderScrollSize();
  void UpdatePaperSize();
  bool IsFitPaper(FittingType fitting);
  void FitPaper(FittingType fitting);
  
  // Coordinate transformation (both drawing and mousing)
  void GetImageSizeScreen(int& width, int& height) const;
  int PixelsToDC(const int i) const;
  int PixelsToDC(const tmFloat& f) const;
  int TreeToDC(const tmFloat& f) const;
  wxPoint TreeToDC(const tmPoint& fp) const;
  tmFloat DCToTree(const int i) const;
  tmPoint DCToTree(const wxPoint& p) const; 
  template <class P>
    bool IsVisible(P* p);
    
  // Calculation of location
  template <class P>
    tmPoint CalcLoc(const P* p);
  template <class P>
    tmPoint CalcLocP(const tmPart* p);
  typedef tmPoint (tmwxDesignCanvas::*CalcLocFn)(const tmPart*);
  template <class P>
    class CalcLocFnT {
    public:
      CalcLocFn operator()() const;
    };
  static tmArray<CalcLocFn>& GetCalcLocFns();
  
  tmPoint CalcLocOffset(const tmPoint& p, const tmFloat& angle);
  tmPoint CalcLocOffset(const tmPoint& p1, const tmPoint& p2, bool up);
  tmPoint CalcLocOffset(const tmPoint& p1, const tmPoint& p2, 
    const tmPoint& p3);
  
  // Drawing
  class Text {};
  class Lines {};
  class Fill {};
  wxColor HiliteColor(const wxColor& theColor, bool isSelected);
  template <class S>
    wxColor TweakColor(const wxColor& theColor);
  template <class P>
    wxColor GetBasePartColor(P* p);
  template <class S, class P>
    wxColor GetPartColor(P* p, bool isSelected);
  void DrawDot(wxDC& dc, const wxPoint& loc, const wxColor& theColor, 
    const int theWidth);
  template <class S>
      void DrawPaper(wxDC& dc); 
  template <class S, class P>
    void DrawPart(wxDC& dc, P* p, bool isSelected);
    
  template <class P>
    void DrawConditionText(wxDC& dc, tmPart* p, const wxPoint& cloc);
  typedef void (tmwxDesignCanvas::*DrawConditionTextFn)(wxDC&, tmPart*, 
    const wxPoint&);
  template <class P>
    class DrawConditionTextFnT {
    public:
      DrawConditionTextFn operator()() const;
    };
  static tmArray<DrawConditionTextFn>& GetDrawConditionTextFns();
    
  template <class P>
    void DrawConditionLines(wxDC& dc, tmPart* p, bool isSelected, 
      const wxPoint& cloc);
  typedef void (tmwxDesignCanvas::*DrawConditionLinesFn)(wxDC&, tmPart*, bool, 
    const wxPoint&);
  template <class P>
    class DrawConditionLinesFnT {
    public:
      DrawConditionLinesFn operator()() const;
    };
  static tmArray<DrawConditionLinesFn>& GetDrawConditionLinesFns();
  
  template <class S, class P>
    void DrawAllParts(wxDC& dc, const tmDpptrArray<P>& partList);

  // Mousing
  bool ClickOnPoint(const wxPoint& where, const tmPoint& q);
  bool ClickOnLine(const wxPoint& where, const tmPoint& q1, const tmPoint& q2);
  template <class P>
    P* ClickOn(const wxPoint& where);

  // friends
  friend class tmwxPrintout;
  TM_TEMPLATE_FRIENDS(CalcLocFnT)
  TM_TEMPLATE_FRIENDS(DrawConditionTextFnT)
  TM_TEMPLATE_FRIENDS(DrawConditionLinesFnT)
};


#ifdef __MWERKS__
  #pragma mark -
#endif


/*********
Template functions
**********/

/*****
Default template definition. Editable or visible objects all have template
specializations.
*****/
template <class P>
tmPoint tmwxDesignCanvas::CalcLoc(const P*)
{
  TMFAIL("default template definition encountered for "\
    "tmwxDesignCanvas::CalcLoc(const P*)");
  return tmPoint(0, 0);
}


/*****
Same as CalcLoc(), except we take a generic const tmPart*, while CalcLoc()
takes a typed ptr
*****/
template <class P>
tmPoint tmwxDesignCanvas::CalcLocP(const tmPart* p)
{
  const P* pp = dynamic_cast<const P*>(p);
  return CalcLoc(pp);
}


/*****
Typecast to undifferentiated function ptr
*****/
template <class P>
tmwxDesignCanvas::CalcLocFn tmwxDesignCanvas::CalcLocFnT<P>::operator()() const
{
  return &tmwxDesignCanvas::CalcLocP<P>;
}


/*****
Default template instantiation needed for non-tmCondition classes
*****/
template <class P>
void tmwxDesignCanvas::DrawConditionText(wxDC&, tmPart*, const wxPoint&)
{
  TMFAIL("default definition encountered for "\
    "tmwxDesignCanvas::DrawConditionText(wxDC&, tmPart*, const wxPoint&)");
}


/*****
Return function ptr to member function of templated type
*****/
template <class P>
tmwxDesignCanvas::DrawConditionTextFn 
  tmwxDesignCanvas::DrawConditionTextFnT<P>::operator()() const
{
  return &tmwxDesignCanvas::DrawConditionText<P>;
}


/*****
Default template instantiation (needed for non-tmCondition classes)
*****/
template <class P>
void tmwxDesignCanvas::DrawConditionLines(wxDC&, tmPart*, bool, const wxPoint&)
{
  TMFAIL("default definition encountered for "\
    "tmwxDesignCanvas::DrawConditionLines(wxDC&, tmPart*, bool, const wxPoint&)");
}


/*****
Return function ptr to member function of templated type
*****/
template <class P>
tmwxDesignCanvas::DrawConditionLinesFn 
  tmwxDesignCanvas::DrawConditionLinesFnT<P>::operator()() const
{
  return &tmwxDesignCanvas::DrawConditionLines<P>;
}


#endif // _TMWXDESIGNCANVAS_H_

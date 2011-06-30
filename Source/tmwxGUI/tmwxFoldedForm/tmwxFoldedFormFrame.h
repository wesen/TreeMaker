/*******************************************************************************
File:         tmwxFoldedFormFrame.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmwxFoldedFormFrame
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-27
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXFOLDEDFORMFRAME_H_
#define _TMWXFOLDEDFORMFRAME_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Forward declarations
#include "tmwxGUI_fwd.h"

// Additional GUI headers
#include "tmwxPersistentFrame.h"

/**********
class tmwxFoldedFormFrame
A tool palette that shows the folded form of the base
**********/
class tmwxFoldedFormFrame: public tmwxPersistentFrame<wxFrame> {
public:
  tmwxFoldedFormFrame();

  // Specify which document to show
  void SetDoc(tmwxDoc* aDoc);
  
  // Drawing
  void SetPrinting(bool printing);
  bool CanPrint();
  bool CanDraw();
  wxPoint TreeToFrame(const tmVertex* aVertex);
  void DrawVertex(wxDC& dc, tmVertex* aVertex);
  void DrawCrease(wxDC& dc, tmCrease* aCrease);
  void DrawFacet(wxDC& dc, tmFacet* aFacet);
  bool IsFacetDrawnSelected(tmFacet* aFacet);
  void OnDraw(wxDC& dc);
  
  // Event handling
  void OnPaint(wxPaintEvent& event);
  void OnClose(wxCloseEvent& event);
  DECLARE_EVENT_TABLE()

protected:
  void GetPositionSizeInfo(Key key, wxString& keystr, int& val);

private:
  tmwxDoc* mDoc;
  bool mPrinting;
  tmFloat mMag;
  int mXoff;
  int mYoff;
};

#endif //_TMWXFOLDEDFORMFRAME_H_

/*******************************************************************************
File:         tmwxView.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker view class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXVIEW_H_
#define _TMWXVIEW_H_

// TreeMaker classes
#include "tmDpptrArray.h"
#include "tmTree.h"

// Additional wxWidgets headers
#include "wx/docview.h"

// Forward declarations
class tmTree;
#include "tmwxGUI_fwd.h"

/**********
class tmwxView
View object that coordinates interactions between the wxDocument and the various
frames that allow views of a tmTree.
**********/
class tmwxView: public wxView {
  DECLARE_DYNAMIC_CLASS(tmwxView)
public:
  // Ctor/Dtor
  tmwxView();
  virtual ~tmwxView();
  
  // Getters
  tmwxDoc* GetDoc() const { return mDoc; };
  tmwxDesignFrame* GetDesignFrame() const { return mDesignFrame; };
  tmwxDesignCanvas* GetDesignCanvas() const { return mDesignCanvas; };
  tmTree* const GetTree() const;
  
  // Printing
  virtual wxPrintout* OnCreatePrintout();
  
  // Event handling
  virtual bool OnCreate(wxDocument *doc, long flags);
  virtual void OnDraw(wxDC *dc);
  virtual void OnUpdate(wxView *sender, wxObject* hint = NULL);
  virtual bool OnClose(bool deleteWindow = true);  
  DECLARE_EVENT_TABLE()

private:
  tmwxDoc* mDoc;
  tmwxDesignFrame* mDesignFrame;
  tmwxDesignCanvas* mDesignCanvas;
};


#endif // _TMWXVIEW_H_

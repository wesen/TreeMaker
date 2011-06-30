/*******************************************************************************
File:         tmwxDesignFrame.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker design frame class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXDESIGNFRAME_H_
#define _TMWXDESIGNFRAME_H_

// Specific wxWidgets headers
#include "wx/docview.h"

// Forward declarations
class tmwxDoc;
class tmwxDesignCanvas;

/**********
class tmwxDesignFrame
contains the design view of a tmTree
**********/

#include "tmwxPersistentFrame.h"
#ifdef TM_USE_MDI
  #include "wx/docmdi.h"
  #define tmwxDocChildFrame wxDocMDIChildFrame
#else
  #define tmwxDocChildFrame wxDocChildFrame
#endif

class tmwxDesignFrame: public tmwxDocChildFrame
{
public:
  // Ctor
  tmwxDesignFrame(tmwxDoc* aDoc, tmwxView* aView, tmwxDocParentFrame* parent);
  
  // Getters
  tmwxDoc* GetDoc() { return mDoc; };
  tmwxView* GetView() { return mView; };
  tmwxDesignCanvas* GetDesignCanvas() { return mDesignCanvas; };
  
  // Activation
  void OnActivate(wxActivateEvent& event);
  
  // Closing
  void OnClose(wxCloseEvent& event);

  // Event handling
  DECLARE_EVENT_TABLE()
private:
  tmwxDoc* mDoc;
  tmwxView* mView;
  tmwxDesignCanvas* mDesignCanvas;
};


#endif // _TMWXDESIGNFRAME_H_

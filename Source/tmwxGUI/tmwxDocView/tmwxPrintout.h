/*******************************************************************************
File:         tmwxPrintout.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker printout class
Author:       Robert J. Lang
Modified by:  
Created:      2004-06-21
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXPRINTOUT_H_
#define _TMWXPRINTOUT_H_

// TreeMaker classes
#include "tmDpptrArray.h"
#include "tmTree.h"

// Additional wxWidgets headers
#include "wx/docview.h"

// Forward declarations
class tmTree;
#include "tmwxGUI_fwd.h"

/**********
class tmwxPrintout
Implements printing of a crease pattern
**********/
class tmwxPrintout: public wxDocPrintout {
public:
  // Ctor/dtor
  tmwxPrintout(tmwxView* view, const wxString& title = wxEmptyString);

  // implementation
  virtual void GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo);
  virtual bool HasPage(int pageNum);
  virtual bool OnPrintPage(int page);
private:
  tmwxDoc* mDoc;
  tmwxView* mView;
};


#endif // _TMWXPRINTOUT_H_

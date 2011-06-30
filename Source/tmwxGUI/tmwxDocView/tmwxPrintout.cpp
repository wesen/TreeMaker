/*******************************************************************************
File:         tmwxPrintout.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker printout class
Author:       Robert J. Lang
Modified by:  
Created:      2004-06-21
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxPrintout.h"
#include "tmwxView.h"
#include "tmwxDesignCanvas.h"
#include "tmwxFoldedFormFrame.h"
#include "tmwxStr.h"
#include "tmModel.h"

/**********
class tmwxPrintout
**********/

/*****
Constructor
*****/
tmwxPrintout::tmwxPrintout(tmwxView* view, const wxString& title)
  : wxDocPrintout(view, title), mDoc(view->GetDoc()), mView(view)
{
  TMASSERT(mView);
  TMASSERT(mDoc);
}


/*****
Called by the framework to obtain information about page range.
*****/
void tmwxPrintout::GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, 
  int* selPageTo)
{
  *minPage = 1;
  *maxPage = gFoldedFormFrame->CanPrint() ? 2 : 1;
  *selPageFrom = 1;
  *selPageTo = 1;
}


/*****
Return true if the printout can provide the given page.
*****/
bool tmwxPrintout::HasPage(int pageNum)
{
  if (pageNum == 1) return true;
  if (pageNum == 2 && gFoldedFormFrame->CanPrint()) return true;
  return false;
}


/*****
Print a single page of the document.
*****/
bool tmwxPrintout::OnPrintPage(int page)
{
  switch(page) {
    case 1: {
      // Draw the canvas image on the DC to create the printed image, letting
      // the canvas know to tweak its presentation for printing and scale it to
      // fit the DC.
      tmwxDesignCanvas* canvas = mView->GetDesignCanvas();
      wxDC* dc = GetDC();
      canvas->SetPrinting(true);
      canvas->OnDraw(*dc);
      canvas->SetPrinting(false);
      return true;
    }
    case 2: {
      // Draw the folded form, if it exists; otherwise return false.
      tmwxFoldedFormFrame* theFrame = gFoldedFormFrame;
      if (theFrame->CanPrint()) {
        wxDC* dc = GetDC();
        theFrame->SetPrinting(true);
        theFrame->OnDraw(*dc);
        theFrame->SetPrinting(false);
        return true;
      }
      return false;
    }
    default:
      return false;
  }
}

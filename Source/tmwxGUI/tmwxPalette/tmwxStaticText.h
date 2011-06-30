/*******************************************************************************
File:         tmwxStaticText.h
Project:      TreeMaker 5.x
Purpose:      Header file for small static text displaying part information
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-29
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXMINITEXT_H_
#define _TMWXMINITEXT_H_

// Common Treemaker and wxWidgets headers
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel.h"

/**********
class tmwxStaticText
Displays information about parts in small type
**********/
class tmwxStaticText : public wxStaticText {
public:
  // Constructor
  tmwxStaticText(wxWindow* parent, int fontSize, const wxString& text = wxEmptyString);

  // Setters
  void SetLabelFormatted(const wxChar* format, const wxString& c);
  void SetLabelFormatted(const wxChar* format, bool b);
  void SetLabelFormatted(const wxChar* format, std::size_t i);
  void SetLabelFormatted(const wxChar* format, const tmFloat& f);
  void SetLabelFormatted(const wxChar* format, const tmPoint& pt);
  void SetLabelFormatted(const wxChar* format, const tmCrease::Kind k);
  void SetLabelFormatted(const wxChar* format, const tmCrease::Fold f);
  void SetLabelFormatted(const wxChar* format, const tmFacet::Color c);
  void SetLabelFormatted(const wxChar* format, const tmNodeOwner* p);
  void SetLabelFormatted(const wxChar* format, const tmEdgeOwner* p);
  void SetLabelFormatted(const wxChar* format, const tmPathOwner* p);
  void SetLabelFormatted(const wxChar* format, const tmPolyOwner* p);
  void SetLabelFormatted(const wxChar* format, const tmVertexOwner* p);
  void SetLabelFormatted(const wxChar* format, const tmCreaseOwner* p);
  void SetLabelFormatted(const wxChar* format, const tmFacetOwner* p);
  template <class P>
    void SetLabelFormatted(const wxChar* format, const P* p);
  template <class P>
    void SetLabelFormatted(const wxChar* format, const tmDpptr<P>& p);
  template <class P>
    void SetLabelFormatted(const wxChar* format, const tmArray<P*>& plist);
  template <class P>
    void SetLabelFormatted(const wxChar* format, const tmDpptrArray<P>& plist);
    
  void SetLabelFeasible(const tmCondition* c);
};


/**********
Template definitions
**********/

/*****
Write a ptr-to-P as its index. The format string should contain %s.
A null pointer will print as "-".
*****/
template <class P>
void tmwxStaticText::SetLabelFormatted(const wxChar* format, const P* p)
{
  wxString str = tmwxStr(p);
  SetLabelFormatted(format, str);
}


/*****
Write a dangle-proof-ptr-to-P as its index. The format string should contain %s.
A null pointer will print as "-".
*****/
template <class P>
void tmwxStaticText::SetLabelFormatted(const wxChar* format, const tmDpptr<P>& p)
{
  wxString str = tmwxStr((const P*) p);
  SetLabelFormatted(format, str);
}


/*****
Write a list-of-P* as a sequence of their respective indices. The format string
should contain %s for the list of indices.
*****/
template <class P>
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmArray<P*>& plist)
{
  wxString str = tmwxStr(plist);
  SetLabelFormatted(format, str);
}


/*****
Write a list-of-P* as a sequence of their respective indices. The format string
should contain %s for the list of indices.
*****/
template <class P>
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmDpptrArray<P>& plist)
{
  wxString str = tmwxStr((const tmArray<P*>&)(plist));
  SetLabelFormatted(format, str);
}


#endif //_TMWXMINITEXT_H_

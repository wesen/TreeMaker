/*******************************************************************************
File:         tmwxStaticText.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for small static text displaying part information
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-29
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxStaticText.h"
#include "tmwxStr.h"

/*
Class twmxwStaticText is a specialization of wxStaticText that allows one to
specify the font size in the constructor. Also, it provides a set of routines
for setting the value from higher-level objects using a formatting string. For
most POD types (floats, ints, unsigned, char, char*, etc.) use the regular
formatting codes. For bools and all higher-level objects, e.g.,
pointers-to-tmPart (which are referenced by their indices) use the formatting
code %s.
*/

/*****
Constructor
*****/
tmwxStaticText::tmwxStaticText(wxWindow* parent, int fontSize, 
  const wxString& text)
  : wxStaticText(parent, wxID_ANY, text, wxPoint(-1000, -1000))
{
  switch (fontSize) {
    case 9:
      SetFont(DEFAULT_FONT_9); 
      break;
    case 10:
      SetFont(DEFAULT_FONT_10);  
      break;
    case 11:
      SetFont(DEFAULT_FONT_11);  
      break;
    case 12:
      SetFont(DEFAULT_FONT_12);  
      break;
    default:
      TMFAIL("unknown font size encountered in "\
        "tmwxStaticText::tmwxStaticText(..)");
  }
}


/*****
Set the values from a C string. The format should contain %s.
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, const wxString& c)
{
  wxString text;
  text.Printf(format, c.c_str());
  SetLabel(text);
}


/*****
Set the values from a bool. The format should contain %s (which will be
replaced by "true" or "false", as appropriate).
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, bool b)
{
  wxString text;
  if (b) text.Printf(format, wxT("true"));
  else text.Printf(format,wxT("false"));
  SetLabel(text);
}


/*****
Set the values from a size_t. The format should contain %s (not %lu or %d).
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, size_t i)
{
  wxString text;
  text.Printf(format, tmwxStr(i).c_str());
  SetLabel(text);
}


/*****
Set the value from a tmFloat. The format can be any of the usual floating-point
formats, e.g., %.4f, or its kin.
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, const tmFloat& f)
{
  wxString text;
  text.Printf(format, f);
  SetLabel(text);
}


/*****
Set the value from a tmPoint. The format should contain two floating-point
values, e.g., "(%.4f, %.4f)".
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, const tmPoint& pt)
{
  wxString text;
  text.Printf(format, pt.x, pt.y);
  SetLabel(text);
}


/*****
Set the value from a crease Kind. The format should contain %s.
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmCrease::Kind k)
{
  wxString text, ktext;
  switch(k) {
    case tmCrease::AXIAL:
      ktext = wxT("axial");
      break;
    case tmCrease::GUSSET:
      ktext = wxT("gusset");
      break;
    case tmCrease::RIDGE:
      ktext = wxT("ridge");
      break;
    case tmCrease::UNFOLDED_HINGE:
      ktext = wxT("unfolded hinge");
      break;
    case tmCrease::FOLDED_HINGE:
      ktext = wxT("folded hinge");
      break;
    case tmCrease::PSEUDOHINGE:
      ktext = wxT("pseudohinge");
      break;
    default:
      TMFAIL("unknown Kind encountered in "\
        "tmwxStaticText::SetLabelFormatted(..)");
  }
  text.Printf(format, ktext.c_str());
  SetLabel(text);
}


/*****
Set the value from a crease Fold. The format should contain %s.
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmCrease::Fold f)
{
  wxString text, ktext;
  switch(f) {
    case tmCrease::MOUNTAIN:
      ktext = wxT("mountain");
      break;
    case tmCrease::VALLEY:
      ktext = wxT("valley");
      break;
    case tmCrease::FLAT:
      ktext = wxT("flat");
      break;
    case tmCrease::BORDER:
      ktext = wxT("border");
      break;
   default:
      TMFAIL("unknown Fold encountered in "\
        "tmwxStaticText::SetLabelFormatted(..)");
  }
  text.Printf(format, ktext.c_str());
  SetLabel(text);
}


/*****
Set the value from a Color field. The format should contain %s.
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmFacet::Color c)
{
  wxString text, ktext;
  switch(c) {
    case tmFacet::NOT_ORIENTED:
      ktext = wxT("not oriented");
      break;
    case tmFacet::COLOR_UP:
      ktext = wxT("color up");
      break;
    case tmFacet::WHITE_UP:
      ktext = wxT("white up");
      break;
    default:
      TMFAIL("unknown Color encountered in "\
        "tmwxStaticText::SetLabelFormatted(..)");
  }
  text.Printf(format, ktext.c_str());
  SetLabel(text);
}


/*****
Set the value from a tmNodeOwner*. The format should contain %s.
A tmNodeOwner can be a tmTree or a tmPoly
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmNodeOwner* p)
{
  wxString text;
  const tmTree* theTree = dynamic_cast<const tmTree*>(p);
  const tmPoly* thePoly = dynamic_cast<const tmPoly*>(p);
  if (theTree) text.Printf(wxT("tree %s"), tmwxStr(theTree).c_str());
  else if (thePoly) text.Printf(wxT("poly %s"), tmwxStr(thePoly).c_str());
  else text.Printf(wxT("-"));
  SetLabelFormatted(format, text);
}


/*****
Set the value from a tmEdgeOwner*. The format should contain %s.
A tmEdgeOwner can only be a tmTree
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmEdgeOwner* p)
{
  wxString text;
  const tmTree* theTree = dynamic_cast<const tmTree*>(p);
  if (theTree) text.Printf(wxT("tree %s"), tmwxStr(theTree).c_str());
  else text.Printf(wxT("-"));
  SetLabelFormatted(format, text);
}


/*****
Set the value from a tmPathOwner*. The format should contain %s.
A tmPathOwner can be a tmTree or a tmPoly
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmPathOwner* p)
{
  wxString text;
  const tmTree* theTree = dynamic_cast<const tmTree*>(p);
  const tmPoly* thePoly = dynamic_cast<const tmPoly*>(p);
  if (theTree) text.Printf(wxT("tree %s"), tmwxStr(theTree).c_str());
  else if (thePoly) text.Printf(wxT("poly %s"), tmwxStr(thePoly).c_str());
  else text.Printf(wxT("-"));
  SetLabelFormatted(format, text);
}


/*****
Set the value from a tmPolyOwner*. The format should contain %s.
A tmPolyOwner can be a tmTree or a tmPoly
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmPolyOwner* p)
{
  wxString text;
  const tmTree* theTree = dynamic_cast<const tmTree*>(p);
  const tmPoly* thePoly = dynamic_cast<const tmPoly*>(p);
  if (theTree) text.Printf(wxT("tree %s"), tmwxStr(theTree).c_str());
  else if (thePoly) text.Printf(wxT("poly %s"), tmwxStr(thePoly).c_str());
  else text.Printf(wxT("-"));
  SetLabelFormatted(format, text);
}


/*****
Set the value from a tmVertexOwner*. The format should contain %s.
A tmVertexOwner can be a tmNode or a tmPath
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmVertexOwner* p)
{
  wxString text;
  const tmNode* theNode = dynamic_cast<const tmNode*>(p);
  const tmPath* thePath = dynamic_cast<const tmPath*>(p);
  if (theNode) text.Printf(wxT("node %s"), tmwxStr(theNode).c_str());
  else if (thePath) text.Printf(wxT("path %s"), tmwxStr(thePath).c_str());
  else text.Printf(wxT("-"));
  SetLabelFormatted(format, text);
}


/*****
Set the value from a tmCreaseOwner*. The format should contain %s.
A tmCreaseOwner can be a tmPath or a tmPoly
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmCreaseOwner* p)
{
  wxString text;
  const tmPath* thePath = dynamic_cast<const tmPath*>(p);
  const tmPoly* thePoly = dynamic_cast<const tmPoly*>(p);
  if (thePath) text.Printf(wxT("path %s"), tmwxStr(thePath).c_str());
  else if (thePoly) text.Printf(wxT("poly %s"), tmwxStr(thePoly).c_str());
  else text.Printf(wxT("-"));
  SetLabelFormatted(format, text);
}


/*****
Set the value from a tmFacetOwner*. The format should contain %s.
A tmFacetOwner can only be a tmPoly
*****/
void tmwxStaticText::SetLabelFormatted(const wxChar* format, 
  const tmFacetOwner* p)
{
  wxString text;
  const tmPoly* thePoly = dynamic_cast<const tmPoly*>(p);
  if (thePoly) text.Printf(wxT("poly %s"), tmwxStr(thePoly).c_str());
  else text.Printf(wxT("-"));
  SetLabelFormatted(format, text);
}


/*****
Set the value from the feasibility of a tmCondition.
*****/
void tmwxStaticText::SetLabelFeasible(const tmCondition* c)
{
  if (c->IsFeasibleCondition())
    SetLabel(wxT("condition currently feasible"));
  else
    SetLabel(wxT("condition currently infeasible"));
}

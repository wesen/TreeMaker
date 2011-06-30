/*******************************************************************************
File:         tmwxConditionListBox.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for condition listing
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxConditionListBox.h"
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxInspectorFrame.h"
#include "tmwxDoc.h"
#include "tmwxDocManager.h"


/**********
class tmwxConditionListBox
Displays a list of conditions
**********/

/*****
Constructor
*****/
tmwxConditionListBox::tmwxConditionListBox(wxWindow* parent, wxSize aSize)
  : wxListBox(parent, wxID_ANY, wxDefaultPosition, aSize, 0, NULL, wxLB_EXTENDED)
{
  SetFont(DEFAULT_FONT_10);
}


/*****
Add a single condition to the list, specialization for tmConditionNodeCombo
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionNodeCombo>(tmCondition* cc)
{
  tmConditionNodeCombo* c = dynamic_cast<tmConditionNodeCombo*>(cc);
  TMASSERT(c);
  size_t nc = 0;
  if (c->GetToSymmetryLine()) nc++;
  if (c->GetToPaperEdge()) nc++;
  if (c->GetToPaperCorner()) nc++;
  if (c->GetXFixed()) nc++;
  if (c->GetYFixed()) nc++;
  wxString text;
  if (nc == 1)
    text.Printf(wxT("node %s pos'n fixed 1 way"), 
      tmwxStr(c->GetNode()).c_str());
  else
    text.Printf(wxT("node %s pos'n fixed %s ways"), 
      tmwxStr(c->GetNode()).c_str(), tmwxStr(nc).c_str());
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionNodeFixed
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionNodeFixed>(tmCondition* cc)
{
  tmConditionNodeFixed* c = dynamic_cast<tmConditionNodeFixed*>(cc);
  TMASSERT(c);
  wxString text;
  if (c->GetXFixed())
    if (c->GetYFixed())
      text.Printf(wxT("node %s pos'n fixed to (%.4f, %.4f)"), 
        tmwxStr(c->GetNode()).c_str(), c->GetXFixValue(), c->GetYFixValue());
    else
      text.Printf(wxT("node %s has X fixed to %.4f"),
        tmwxStr(c->GetNode()).c_str(), c->GetXFixValue());
  else
    if (c->GetYFixed())
      text.Printf(wxT("node %s has Y fixed to %.4f"),
        tmwxStr(c->GetNode()).c_str(), c->GetYFixValue());
    else
      text.Printf(wxT("node %s, no coordinates fixed"),
        tmwxStr(c->GetNode()).c_str());
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionNodeOnCorner
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionNodeOnCorner>(tmCondition* cc)
{
  tmConditionNodeOnCorner* c = dynamic_cast<tmConditionNodeOnCorner*>(cc);
  TMASSERT(c);
  wxString text;
  text.Printf(wxT("node %s pos'n fixed to corner"), 
    tmwxStr(c->GetNode()).c_str());
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionNodeOnEdge
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionNodeOnEdge>(tmCondition* cc)
{
  tmConditionNodeOnEdge* c = dynamic_cast<tmConditionNodeOnEdge*>(cc);
  TMASSERT(c);
  wxString text;
  text.Printf(wxT("node %s pos'n fixed to edge"), 
    tmwxStr(c->GetNode()).c_str());
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionNodesCollinear
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionNodesCollinear>(
  tmCondition* cc)
{
  tmConditionNodesCollinear* c = dynamic_cast<tmConditionNodesCollinear*>(cc);
  TMASSERT(c);
  wxString text;
  text.Printf(wxT("nodes %s, %s, and %s collinear"),
    tmwxStr(c->GetNode1()).c_str(), tmwxStr(c->GetNode2()).c_str(), 
    tmwxStr(c->GetNode3()).c_str());
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionNodesPaired
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionNodesPaired>(
  tmCondition* cc)
{
  tmConditionNodesPaired* c = dynamic_cast<tmConditionNodesPaired*>(cc);
  TMASSERT(c);
  wxString text;
  text.Printf(wxT("nodes %s and %s symmetrically paired"),
    tmwxStr(c->GetNode1()).c_str(), tmwxStr(c->GetNode2()).c_str());
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionNodeSymmetric
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionNodeSymmetric>(
  tmCondition* cc)
{
  tmConditionNodeSymmetric* c = dynamic_cast<tmConditionNodeSymmetric*>(cc);
  TMASSERT(c);
  wxString text;
  text.Printf(wxT("node %s on symmetry line"), 
    tmwxStr(c->GetNode()).c_str());
  Append(text);
}


/*****
Add a single condition to the list, specialization for
tmConditionEdgeLengthFixed
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionEdgeLengthFixed>(
  tmCondition* cc)
{
  tmConditionEdgeLengthFixed* c = dynamic_cast<tmConditionEdgeLengthFixed*>(cc);
  TMASSERT(c);
  wxString text;
  text.Printf(wxT("edge %s length fixed to %.4f"), 
    tmwxStr(c->GetEdge()).c_str(),
    c->GetEdge()->GetLength());
  Append(text);
}


/*****
Add a single condition to the list, specialization for
tmConditionEdgesSameStrain
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionEdgesSameStrain>(
  tmCondition* cc)
{
  tmConditionEdgesSameStrain* c = dynamic_cast<tmConditionEdgesSameStrain*>(cc);
  TMASSERT(c);
  wxString text;
  text.Printf(wxT("edges %s and %s same strain"),
    tmwxStr(c->GetEdge1()).c_str(), tmwxStr(c->GetEdge2()).c_str());
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionPathCombo
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionPathCombo>(tmCondition* cc)
{
  tmConditionPathCombo* c = dynamic_cast<tmConditionPathCombo*>(cc);
  TMASSERT(c);
  wxString text;
  tmPath* aPath = c->GetTree()->FindLeafPath(c->GetNode1(), c->GetNode2());
  text.Printf(wxT("path %s (nodes %s - %s) active"),
    tmwxStr(aPath).c_str(), tmwxStr(c->GetNode1()).c_str(), 
    tmwxStr(c->GetNode2()).c_str());
  if (c->IsAngleFixed()) {
    text += wxString::Format(wxT(" at angle %.2f"), c->GetAngle());
  }
  if (c->IsAngleQuant()) {
    text += wxString::Format(wxT(" at angle %.2f+360/%s"),
      c->GetQuantOffset(), tmwxStr(c->GetQuant()).c_str());
  }
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionPathActive
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionPathActive>(tmCondition* cc)
{
  tmConditionPathActive* c = dynamic_cast<tmConditionPathActive*>(cc);
  TMASSERT(c);
  wxString text;
  tmPath* aPath = c->GetTree()->FindLeafPath(c->GetNode1(), c->GetNode2());
  text.Printf(wxT("path %s (nodes %s - %s) active"),
    tmwxStr(aPath).c_str(), tmwxStr(c->GetNode1()).c_str(), 
    tmwxStr(c->GetNode2()).c_str());
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionPathAngleFixed
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionPathAngleFixed>(
  tmCondition* cc)
{
  tmConditionPathAngleFixed* c = dynamic_cast<tmConditionPathAngleFixed*>(cc);
  TMASSERT(c);
  wxString text;
  tmPath* aPath = c->GetTree()->FindLeafPath(c->GetNode1(), c->GetNode2());
  text.Printf(wxT("path %s (nodes %s - %s) active at angle %.2f"),
    tmwxStr(aPath).c_str(), tmwxStr(c->GetNode1()).c_str(), 
    tmwxStr(c->GetNode2()).c_str(), c->GetAngle());
  Append(text);
}


/*****
Add a single condition to the list, specialization for tmConditionPathAngleQuant
*****/
template <>
void tmwxConditionListBox::AddCondition<tmConditionPathAngleQuant>(
  tmCondition* cc)
{
  tmConditionPathAngleQuant* c = dynamic_cast<tmConditionPathAngleQuant*>(cc);
  TMASSERT(c);
  wxString text;
  tmPath* aPath = c->GetTree()->FindLeafPath(c->GetNode1(), c->GetNode2());
  text.Printf(wxT("path %s (nodes %s - %s) active at angle %.2f+360/%s"),
    tmwxStr(aPath).c_str(), tmwxStr(c->GetNode1()).c_str(), 
    tmwxStr(c->GetNode2()).c_str(), c->GetQuantOffset(), 
    tmwxStr(c->GetQuant()).c_str());
  Append(text);
}


/*****
STATIC
Make if necessary and return our list of condition addition functions.
*****/
tmArray<tmwxConditionListBox::AddConditionFn>& 
  tmwxConditionListBox::GetAddConditionFns()
{
  static tmArray<AddConditionFn> sAddConditionFns;
  if (sAddConditionFns.empty()) {
    TMASSERT(tmPart::TypesAreInitialized());
    tmPart::MakeTypeArray<AddConditionFn, AddConditionFnT>(sAddConditionFns);
  }
  return sAddConditionFns;
}


/*****
Fill in the new wxListBox entries from mConditions.
*****/
void tmwxConditionListBox::FillListBoxFromConditionList()
{
  Clear();
  for (size_t i = 0; i < mConditions.size(); ++i) {
    tmCondition* c = mConditions[i];
    (this->*GetAddConditionFns()[c->GetTag()])(c);
  }
}


/*****
Double-clicking on one or more conditions makes them the current selection.
*****/
void tmwxConditionListBox::OnDoubleClick(wxCommandEvent&)
{
  wxArrayInt sels;
  GetSelections(sels);
  tmwxDoc* doc = gDocManager->GetCurrentDocumentLocal();
  doc->mSelection.ClearAllParts();
  for (size_t i = 0; i < sels.GetCount(); ++i)
    doc->mSelection.AddPart(mConditions[size_t(sels[i])]);
  doc->UpdateAllViews();
}


/*****
Enable/disable relevant edit menu items
*****/
void tmwxConditionListBox::OnUpdateUI(wxUpdateUIEvent& event)
{
  switch(event.GetId()) {
    case wxID_CUT:
    case wxID_CLEAR:
      event.Enable(true);
      break;
    case wxID_COPY:
    case wxID_PASTE:
      event.Enable(false);
      break;
    default:
      event.Skip();
  }
}


/*****
Respond to other command events; specifically, on cut or clear, delete the
selected conditions.
*****/
void tmwxConditionListBox::OnCommand(wxCommandEvent& event)
{
  switch(event.GetId()) {
    case wxID_CUT:
    case wxID_CLEAR: {
      // Delete the selected conditions
      wxArrayInt sels;
      GetSelections(sels);
      tmwxDoc* doc = gDocManager->GetCurrentDocumentLocal();
      tmArray<tmCondition*> markedConditions;
      for (size_t i = 0; i < sels.GetCount(); ++i)
        markedConditions.push_back(mConditions[size_t(sels[i])]);
      doc->DoKillConditions(markedConditions);
      break;
    }
    default:
      event.Skip();
  }
}


/*****
Respond to keys while we're the focus
*****/
void tmwxConditionListBox::OnKeyDown(wxKeyEvent& event)
{
// int dbg_keycode = event.GetKeyCode();

  switch (event.GetKeyCode()) {
    case WXK_NUMPAD_DELETE:
    case WXK_BACK:
    case WXK_DELETE: {
      // Delete the selected conditions
      wxArrayInt sels;
      GetSelections(sels);
      tmwxDoc* doc = gDocManager->GetCurrentDocumentLocal();
      tmArray<tmCondition*> markedConditions;
      for (size_t i = 0; i < sels.GetCount(); ++i)
        markedConditions.push_back(mConditions[size_t(sels[i])]);
      doc->DoKillConditions(markedConditions);
      break;
    }
    default:
      event.Skip();
  }
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxConditionListBox, wxListBox)
  EVT_LISTBOX_DCLICK(wxID_ANY, tmwxConditionListBox::OnDoubleClick)
  EVT_UPDATE_UI(wxID_ANY, tmwxConditionListBox::OnUpdateUI)
  EVT_MENU(wxID_ANY, tmwxConditionListBox::OnCommand)
  EVT_KEY_DOWN(tmwxConditionListBox::OnKeyDown)
END_EVENT_TABLE()

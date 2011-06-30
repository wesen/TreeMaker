/*******************************************************************************
File:         tmwxConditionListBox.h
Project:      TreeMaker 5.x
Purpose:      Header file for condition listing
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-28
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCONDITIONLISTBOX_H_
#define _TMWXCONDITIONLISTBOX_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Treemaker classes
#include "tmArray.h"
#include "tmPart.h"

// Forward declarations
#include "tmModel_fwd.h"

/**********
class tmwxConditionListBox
Displays a list of conditions
**********/

class tmwxConditionListBox : public wxListBox {
public:
  tmwxConditionListBox(wxWindow* parent, wxSize aSize);
  template <class P>
    void FillListBox(P* p);
private:
  tmArray<tmCondition*> mConditions;
  template <class P>
    void AddCondition(tmCondition* c);
  typedef void (tmwxConditionListBox::*AddConditionFn)(tmCondition* c);
  template <class P>
    class AddConditionFnT {
    public:
      AddConditionFn operator()() const;
    };
  static tmArray<AddConditionFn>& GetAddConditionFns();
  void FillListBoxFromConditionList();
  void OnDoubleClick(wxCommandEvent& event);
  void OnUpdateUI(wxUpdateUIEvent& event);
  void OnCommand(wxCommandEvent& event);
  void OnKeyDown(wxKeyEvent& event);
  
  DECLARE_EVENT_TABLE()

  // friend functions
  TM_TEMPLATE_FRIENDS(AddConditionFnT)
};


/**********
Template definitions
**********/

/*****
Fill the condition list with all conditions affecting the given part
*****/
template <class P>
void tmwxConditionListBox::FillListBox(P* p)
{
  p->GetTree()->GetAffectingConditions(p, mConditions);
  FillListBoxFromConditionList();
}


/*****
This is the function used for the default instantiation of AddConditionFnT. It
gets called if we encounter a condition for which you haven't defined a template 
specialization of AddCondition<P>().
*****/
template <class P>
void tmwxConditionListBox::AddCondition(tmCondition*)
{
  TMFAIL("default definition encountered for "\
   "tmwxConditionListBox::AddCondition(tmCondition* c)");
}


/*****
Return ptr to template function that adds a condition to listbox
*****/
template <class P>
tmwxConditionListBox::AddConditionFn 
  tmwxConditionListBox::AddConditionFnT<P>::operator()() const
{
  return &tmwxConditionListBox::AddCondition<P>;
}


#endif //_TMWXCONDITIONLISTBOX_H_

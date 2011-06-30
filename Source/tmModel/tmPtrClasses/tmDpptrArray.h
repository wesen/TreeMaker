/*******************************************************************************
File:         tmDpptrArray.h
Project:      TreeMaker 5.x
Purpose:      Header file for dangle-proof array of pointers
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMDPPTRARRAY_H_
#define _TMDPPTRARRAY_H_

#include <algorithm>

#include "tmArray.h"
#include "tmDpptr.h"

// We overload casts in a couple of places, but not under GCC 3.x, which
// (wrongly) complains.
#if defined(__GNUC__)
  #if (__GNUC__ < 4)
    #define TM_OVERLOAD_CASTS 0
  #else
    #define TM_OVERLOAD_CASTS 1
  #endif // (__GNUC__ < 4)
#else // non-GCC compilers
  #define TM_OVERLOAD_CASTS 1
#endif // defined(__GNUC__)


/*
You can store pointers-to-T in lists using the class tmDpptrArray<T>, the same
way you might store references in a list of T*. Here, the behavior is a bit
different; when the object gets deleted, the pointer is entirely removed from
the tmDpptrArray<T>.

Note: you should only use the routines below to remove items from a
tmDpptrArray. If you use, for example, algorithms like erase() or remove(), the
list of DpptrSrcs in the objects will get corrupted.

Since the tmArray member functions are not virtual, if you cast a
tmDpptrArray<T> to a tmArray<T*> and then call a modifying tmArray member
function, you will totally bypass the dangle-proof bookkeeping. Unfortunately,
this creates unpredictable behavior that is even harder to track down than a
dangling pointer. To prevent this, you can only cast a tmDpptrArray<T> to a
const tmArray<T*>, which won't allow modifying operations.

Note, however, the following interesting behavior:

tmDpptrArray<T> foo_array;
// add some elements to foo_array
const tmArray<T*>& const_array = foo_array;
T* elem = const_array.front();
delete elem;

This deletion has the side effect of removing the first element from
const_array, even though we declared it as const! Errors based on this
phenomenon are fairly hard to track down.
*/

/**********
class tmDpptrArray<T>
A dangle-proof list of pointers-to-T. When one of the objects of type T is
deleted, its pointer is completely removed from the list.
**********/
template <class T>
class tmDpptrArray : public tmArray<T*>, private tmDpptrSrc
{
public:
  // typenames
  typedef typename tmArray<T*>::reference reference;
  typedef typename tmArray<T*>::const_reference const_reference;
  typedef typename tmArray<T*>::iterator iterator;
  typedef typename tmArray<T*>::const_iterator const_iterator;
  typedef typename tmArray<T*>::size_type size_type;
  typedef typename tmArray<T*>::difference_type difference_type;
  typedef T* value_type;

  // It's useful to have a ptr-to-T type defined
  typedef T* ptr_t;
  
  // Constructor/destructor
  tmDpptrArray() {};
  tmDpptrArray(const tmDpptrArray<T>& aList);
  tmDpptrArray(const tmArray<T*>& aList);
  ~tmDpptrArray();
  
  // Assignment
  const tmDpptrArray& operator=(const tmDpptrArray<T>& aList);
  const tmDpptrArray& operator=(const tmArray<T*>& aList);
  
  // Cast to ancestor: allow only read-only cast.
#if TM_OVERLOAD_CASTS
  operator const tmArray<T*>() const {return *this;};
#endif // TM_OVERLOAD_CASTS
  
  // Subscripting returns read-only reference to array element so you can't
  // overwrite an element and circumvent bookkeeping.
  const ptr_t& operator[](std::size_t i) const {
    return tmArray<T*>::operator[](i);};
  const ptr_t& operator[](std::size_t i) {
    return tmArray<T*>::operator[](i);};

  // Modifying operations
  void push_back(T* p);
  void push_front(T* p);
  void union_with(T* p);    
  void erase_remove(T* p);
  void replace_with(T*& told, T*& tnew);
  void clear();
  void InsertItemAt(std::size_t n, T* p);
  void ReplaceItemAt(std::size_t n, T* p);
  void KillItems();
  void merge_with(const tmArray<T*>& aList);    
  void union_with(const tmArray<T*>& aList);
  void intersect_with(const tmArray<T*>& aList);
private:
  // used in implementation
  void RemoveDpptrTarget(tmDpptrTarget* aDpptrTarget);
  
  // non-const overload not allowed (if compiler allows overloading)
#if TM_OVERLOAD_CASTS
  operator tmArray<T*>();
#endif // TM_OVERLOAD_CASTS
  
  // tmArray<T> members, not yet supported
  void assign_all(const ptr_t& t);
  tmDpptrArray<T>& RemoveItemAt(std::size_t n);
  
  // vector<T> members, not yet supported
  void pop_back();
  void pop_front();
  iterator insert(iterator position, const value_type& x);
  void insert(iterator position, size_type n, const value_type& x);
  iterator erase(iterator position);
  iterator erase(iterator first, iterator last);
};


/**********
class DpptrTarget_EqualTo<T>
A predicate class used to compare T* to tmDpptrTarget*
**********/
template <class T>
class DpptrTarget_EqualTo {
  typedef tmDpptrTarget* r_ptr;  // ptr to tmDpptrTarget
  typedef T* t_ptr;        // ptr to T
  r_ptr mDpptrTarget;        // ptr to the tmDpptrTarget being compared to
public:
  DpptrTarget_EqualTo(const r_ptr& aDpptrTarget) : mDpptrTarget(aDpptrTarget) {};
  bool operator() (const t_ptr& ptr) const {return r_ptr(ptr) == mDpptrTarget;};
};


/**********
Template definitions
**********/

/*****
Copy constructor from tmDpptrArray<T>
*****/
template <class T>
tmDpptrArray<T>::tmDpptrArray(const tmDpptrArray<T>& aList)
{
  merge_with(aList);
}


/*****
Copy constructor from tmArray<T*>
*****/
template <class T>
tmDpptrArray<T>::tmDpptrArray(const tmArray<T*>& aList)
{
  merge_with(aList);
}


/*****
Assignment from tmDpptrArray<T>
*****/
template <class T>
const tmDpptrArray<T>& tmDpptrArray<T>::operator=(const tmDpptrArray<T>& aList)
{
  clear();
  merge_with(aList);
}


/*****
Assignment from tmArray<T*>
*****/
template <class T>
const tmDpptrArray<T>& tmDpptrArray<T>::operator=(const tmArray<T*>& aList)
{
  clear();
  merge_with(aList);
}


/*****
Destructor. Inform all objects that they're no longer referenced by us
*****/
template <class T>
tmDpptrArray<T>::~tmDpptrArray()
{
  for (std::size_t i = 0; i < this->size(); ++i) 
    DstRemoveMeAsDpptrSrc((*this)[i]);
}


/*****
Add this object to the list and tell it we're now referencing it
*****/
template <class T>
void tmDpptrArray<T>::push_back(T* p)
{
  tmArray<T*>::push_back(p);
  DstAddMeAsDpptrSrc(p);
}


/*****
Add this object to the list and tell it we're now referencing it
*****/

template <class T>
void tmDpptrArray<T>::push_front(T* pt)
{
  tmArray<T*>::insert(this->begin(), pt);
  DstAddMeAsDpptrSrc(pt);
}


/*****
Add this object to the list if it isn't there already and tell it we're now
referencing it
*****/
template <class T>
void tmDpptrArray<T>::union_with(T* pt)
{
  if (!contains(pt)) push_back(pt);
}


/*****
Remove all copies of this item from the list.
*****/
template <class T>
void tmDpptrArray<T>::erase_remove(T* pt)
{
  if (contains(pt)) {
    tmArray<T*>::erase_remove(pt);
    DstRemoveMeAsDpptrSrc(pt);
  };
}


/*****
Replace all copies of one item with another
*****/
template <class T>
void tmDpptrArray<T>::replace_with(T*& told, T*& tnew)
{
  if (told == tnew) return;
  iterator p = this->begin();
  bool removedMe = false;
  while ((p = find(p, this->end(), told)) != this->end()) {
    if (!removedMe) {
      DstRemoveMeAsDpptrSrc(*p);
      removedMe = true;
    }
    *p = tnew;
    DstAddMeAsDpptrSrc(*p);
  }
}


/*****
Remove all items and inform referenced objects
*****/
template <class T>
void tmDpptrArray<T>::clear()
{
  for (std::size_t i = 0; i < this->size(); ++i) 
    DstRemoveMeAsDpptrSrc((*this)[i]);
  tmArray<T*>::clear();
}


/*****
Insert a new item into the list
*****/
template <class T>
void tmDpptrArray<T>::InsertItemAt(std::size_t n, T* pt)
{
  tmArray<T*>::InsertItemAt(n, pt);
  DstAddMeAsDpptrSrc(pt);
}


/*****
Replace one item with another
*****/
template <class T>
void tmDpptrArray<T>::ReplaceItemAt(std::size_t n, T* pt)
{
  T* qt = this->NthItem(n);
  tmArray<T*>::ReplaceItemAt(n, pt);
  DstAddMeAsDpptrSrc(pt);
  DstRemoveMeAsDpptrSrc(qt);
}


/*****
Delete all of the items in the list (which clears out the list). Note that if
the list contains duplicates, then deleting that item will remove more than one
item from the list. So, to be safe (and efficient) we will repeatedly delete
the last item from the list.
*****/
template <class T>
void tmDpptrArray<T>::KillItems()
{
  while (this->not_empty()) delete this->back();
}


/*****
Combine two lists including duplicates
*****/
template <class T>
void tmDpptrArray<T>::merge_with(const tmArray<T*>& aList)
{
  for (std::size_t i = 0; i < aList.size(); ++i) 
    push_back(aList[i]);
}


/*****
Combine two lists, not including duplicates
*****/
template <class T>
void tmDpptrArray<T>::union_with(const tmArray<T*>& aList)
{
  for (std::size_t i = 0; i < aList.size(); ++i) 
    union_with(aList[i]);
}


/*****
Take the intersection of two lists
*****/
template <class T>
void tmDpptrArray<T>::intersect_with(const tmArray<T*>& aList)
{
  for (std::size_t i = 0; i < this->size(); ++i) {
    T* pt = aList[i];
    if (!aList.contains(pt)) {
      erase_remove(pt);
      --i;
    }
  }
}


/*****
Remove all references to the passed object from the array, because it's being
destroyed. This prevents the array from holding a dangling pointer to a
destroyed object.
Called by:
~tmDpptrTarget()
*****/
template <class T>
void tmDpptrArray<T>::RemoveDpptrTarget(tmDpptrTarget* aDpptrTarget)
{
  tmArray<T*>::erase(std::remove_if(this->begin(), this->end(), 
    DpptrTarget_EqualTo<T>(aDpptrTarget)), this->end());
}


#endif // _TMDPPTRARRAY_H_

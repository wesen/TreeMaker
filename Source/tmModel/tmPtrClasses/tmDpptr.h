/*******************************************************************************
File:         tmDpptr.h
Project:      TreeMaker 5.x
Purpose:      Header file for dangle-proof pointer class tmDpptr<T>
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMDPPTR_H_
#define _TMDPPTR_H_

/*  
A tmDpptr<T> is a dangle-proof pointer-to-T. Use a tmDpptr<T> anywhere you
would use a T*. Class T must be descended from tmDpptrTarget. The tmDpptrTarget
keeps track of all tmDpptr<T>'s that point to it and when the object is
deleted, all subsequent attempts to dereference the tmDpptr<T> will return
NULL.

Once its target has been deleted, a tmDpptr<T> becomes a null pointer. Of
course you can also make it a null pointer by initializing as NULL or assigning
it to NULL.

Obviously, there is some cost in time and space in doing this, so you should
only use tmDpptr<T> (and tmDpptrArray<T>) when you can't easily avoid dangling
pointers by conventional means.

if you could cast a tmDpptr<T> to a T* and then change it (e.g., by reassigning
it), you will totally bypass the dangle-proof bookkeeping, which creates
insidious and unpredictable behavior that is even harder to track down than a
dangling pointer. To prevent this, you can only cast a tmDpptr<T> to a (T*
const) (that is, a constant pointer-to-T).
*/

#include "tmDpptrSrc.h"

/**********
class tmDpptr<T>
This class implements a dangle-proof pointer-to-T.
**********/
template <class T>
class tmDpptr : private tmDpptrSrc
{
public:
  // pointer types
  typedef T* ptr_t;
  typedef T* const ptr_t_const;
  
  // Ctor & dtor
  tmDpptr() : mTarget(0) {};
  tmDpptr(const tmDpptr<T>& t);
  tmDpptr(ptr_t_const t);
  virtual ~tmDpptr();
  
  // Assignment has the side effect of updating reference counts    
  T* operator=(T* t);
  
  // Cast to ptr_t; only allowed is cast to ptr_t_const
  operator ptr_t_const() const {return mTarget;};
  
  // Comparison with raw types (typically ptr_t_const, const tmDpptr<T>&, or
  // int (null ptr))
  template <class R>
    bool operator==(R r) const {return mTarget == (ptr_t_const)(r);};
  template <class R>
    bool operator!=(R r) const {return mTarget != (ptr_t_const)(r);};
  
  // Dereferencing
  T& operator *() const {return *mTarget;};
  ptr_t operator ->() const {return mTarget;};

private:  
  ptr_t mTarget;  // the thing we are pointing to
  void RemoveDpptrTarget(tmDpptrTarget* aDpptrTarget);
};


/*
Comparison with raw types with tmDpptr<T> as second argument. Not declared as a
friend function within the class because CW doesn't support it.
*/

/*****
Equality testing
*****/
template <class R, class T>
inline bool operator==(R r, const tmDpptr<T>& dt) {
  return (T* const)(r) == (T* const)(dt);};


/*****
Inequality testing
*****/
template <class R, class T>
inline bool operator!=(R r, const tmDpptr<T>& dt) {
  return (T* const)(r) != (T* const)(dt);};


/**********
Template definitions
**********/

/*****
Copy constructor
*****/
template <class T>
tmDpptr<T>::tmDpptr(const tmDpptr<T>& t)
  : mTarget(t.mTarget)
{
  if (mTarget) DstAddMeAsDpptrSrc(mTarget);
}


/*****
Constructor from const pointer-to-T
*****/
template <class T>
tmDpptr<T>::tmDpptr(ptr_t_const t)
  : mTarget(t)
{
  if (mTarget) DstAddMeAsDpptrSrc(mTarget);
}


/*****
Destructor. Notify the tmDpptrTarget that it no longer has me pointing at it
*****/
template <class T>
tmDpptr<T>::~tmDpptr()
{
  if (mTarget) DstRemoveMeAsDpptrSrc(mTarget);
}


/*****
Assignment of a pointer-to-descendant-of-tmDpptrTarget notifies the old and new
objects of the change in reference.
*****/
template <class T>
T* tmDpptr<T>::operator=(T* t)
{
  if (mTarget) DstRemoveMeAsDpptrSrc(mTarget);
  mTarget = t;
  if (mTarget) DstAddMeAsDpptrSrc(mTarget);
  return mTarget;
}


/*****
Clear all references to this tmDpptrTarget.
Called by:
~tmDpptrTarget()
*****/
template <class T>
void tmDpptr<T>::RemoveDpptrTarget(tmDpptrTarget*)
{
  mTarget = 0;
}

#endif // _TMDPPTR_H_

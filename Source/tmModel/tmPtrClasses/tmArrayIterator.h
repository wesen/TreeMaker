/*******************************************************************************
File:         tmArrayIterator.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker array iterator classe
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMARRAYITERATOR_H_
#define _TMARRAYITERATOR_H_

/*
tmArrayIterator - redefinition of the PowerPlant class LArrayIterator into a template
class that implements the minimal behavior of an LArrayIterator acting on a tmArray<T>.
A LArrayIterator is not at all like an STL iterator; you increment it using Next()
(rather than operator++()) and test for end on the return value of Next() rather than
by comparing it to end().

This version implments the minimum behavior necessary to use the roughly 200 
LArrayIterator usages sprinkled throughout the TreeMaker code with minimal changes.

This class will be deprecated and future iterating functions should use either for 
loops or STL iterators.
*/

#include "tmArray.h"

// Constants shared by all templated tmArrayIterator<T> classes
enum {
  tmArray_START = 0L,      // Where to start iterating
  tmArray_END = -1L
};

/**********
class tmArrayIterator<P>
A reimplementation of the PowerPlant LArrayIterator class.
**********/
template <class T>
class  tmArrayIterator
{
public:
  // Constructor / Destructor
  tmArrayIterator(const tmArray<T>& inArray, long inPosition = tmArray_START);
  ~tmArrayIterator() {};
  // Positioning
  void ResetTo(long inPosition);
  // Get copy of fixed-size element
  bool Next(T* outItem);  
  bool Previous(T* outItem);  
private:
  const tmArray<T>& mArray;    // reference to the current array
  std::size_t mCurrIndex;        // current index (1-based, not offset) into array
  bool Current(T* outItem);
};


/**********
template definitions
**********/

/*****
tmArrayIterator<T>::tmArrayIterator(const tmArray<T>& inArray, long inPosition)
Constructor. Initialize with an array and a starting position.
*****/
template <class T>
tmArrayIterator<T>::tmArrayIterator(const tmArray<T>& inArray, long inPosition)
  : mArray(inArray)
{
  ResetTo(inPosition);
}


/*****
tmArrayIterator<T>::void ResetTo(long inPosition)
Reset the position to the beginning or end or middle of a sequence.
*****/
template <class T>
void tmArrayIterator<T>::ResetTo(long inPosition)
{
  switch(inPosition) {
    case tmArray_START: 
      mCurrIndex = tmArray_START;
      break;
    case tmArray_END:
      mCurrIndex = 1 + mArray.size();
      break;
    default:
      mCurrIndex = std::size_t(inPosition);  // no range-checking
      break;
  }
}


/*****
bool tmArrayIterator<T>::Next(T* outItem)
Set *outItem to the next item and return true, or return false
if we're at the end of the array.
*****/
template <class T>
bool tmArrayIterator<T>::Next(T* outItem)
{
  mCurrIndex++;
  return Current(outItem);
}


/*****
bool tmArrayIterator<T>::Previous(T* outItem)
Set outItem to the previous item and return true, or return false
if no item is current.
*****/
template <class T>
bool tmArrayIterator<T>::Previous(T* outItem)
{
  mCurrIndex--;
  return Current(outItem);
}


/*****
bool tmArrayIterator<T>::Current(T* outItem)
Set *outItem to the current Item and return true, or return false
if no item is current.
*****/
template <class T>
bool tmArrayIterator<T>::Current(T* outItem)
{
  if (mCurrIndex > 0 && mCurrIndex <= mArray.size()) {
    *outItem = mArray[mCurrIndex - 1];
    return true;
  }
  else {
    *outItem = NULL;
    return false;
  }
}

#endif // _TMARRAYITERATOR_H_

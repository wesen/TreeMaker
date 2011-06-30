/*******************************************************************************
File:         tmArray.h
Project:      TreeMaker 5.x
Purpose:      Header file for general-purpose TreeMaker container class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMARRAY_H_
#define _TMARRAY_H_

/*
Class tmArray<T> is a general-purpose container used throughout TreeMaker.
TreeMaker previously used a variety of container classes that go back well
before the standard library existed (and even back to the days when TreeMaker
was written in Object Pascal). Previous versions used a mixture of hand-rolled
container classes, Metrowerks PowerPlant containers, and re-implementations of
old Pascal arrays. I've now rolled all past containers into the tmArray<T>
class, which is implemented on top of the C++ standard library std::vector<T>
class.

To support past classes, this class supports both 0-based and 1-based indexing
(via different calls); it also provides several set-like operations. As time
goes on, I will eliminate 1-based indexing calls in order to simplify the
interface and reduce the potential for ambiguity. Calls that take 1-based index
have the index specified as "n" or have the word "index" in their name. 0-based
indexing uses "i" and/or the word "offset."

Names that begin with an upper-case letter are implementations of legacy
functions. The standard library calls, of course, begin with lower-case. I've
also added a few new member functions for convenience and, like standard
library functions, they begin with lower-case. So you can take lower-case names
to be an indication that I intend to keep the function around.
*/

#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

#include "tmHeader.h"

/**********
class tmArray<T>
Template container class. 
**********/
template <class T>
class tmArray : public std::vector<T>
{
public:
  // typenames
  typedef typename std::vector<T>::reference reference;
  typedef typename std::vector<T>::const_reference const_reference;
  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;
  typedef typename std::vector<T>::size_type size_type;
  typedef typename std::vector<T>::difference_type difference_type;
  typedef T value_type;

  enum {
    BAD_OFFSET = std::size_t(-1), 
    BAD_INDEX = std::size_t(0)
  };
  
  // assignment to every element
  void assign_all(const T& t) {assign(this->size(), t);};
  
  // 0-based indexing uses checked access in debugging builds, defaults to
  // unchecked access (operator[]()) in release builds
#ifdef TMDEBUG
  T& operator[](std::size_t i);
  const T& operator[](std::size_t i) const;
#endif
  
  // 1-based indexing (deprecated)
  T& NthItem(std::size_t n);
  const T& NthItem(std::size_t n) const;
  
  // addition and removal-by-value
  void push_front(const T& t);
  void union_with(const T& t);
  void erase_remove(const T& t);
  void replace_with(const T& told, const T& tnew);
  
  // 1-based editing (deprecated)
  std::size_t GetIndex(const T& t) const;
  tmArray<T>& RemoveItemAt(std::size_t n);
  tmArray<T>& InsertItemAt(std::size_t n, const T& t);
  tmArray<T>& ReplaceItemAt(std::size_t n, const T& t);
  void SwapItems(std::size_t inIndexA, std::size_t inIndexB);
  void MoveItem(std::size_t inFromIndex, std::size_t inToIndex);
  
  // order rearrangement
  void rotate_left();
  void rotate_right();
  
  // 0-based addressing
  std::size_t GetOffset(const T& t) const;
  
  // testing of contents
  bool contains(const T& t) const {
    // Return true if the array contains the given element at least once   
    return std::find(this->begin(), this->end(), t) != this->end();
  };
  std::size_t count(const T& t) const {
    // Return the number of occurrences of the given element in the array
    return std::count(this->begin(), this->end(), t);
  };
  bool not_empty() const {
    // Return true if the array is not empty. Note that empty() is inherited.
    return !this->empty();
  };
  
  // Set operations on other lists
  void merge_with(const tmArray<T>& aList);    // allows duplicate items
  void union_with(const tmArray<T>& aList);    // only one of any item
  void intersect_with(const tmArray<T>& aList);
  bool intersects(const tmArray<T>& aList) const;
  
  // return a string of the contents
  std::string GetStr() const;
};


/**********
Template definitions
**********/

/*****
Debugging builds perform range checking.
*****/
#ifdef TMDEBUG
template <class T>
T& tmArray<T>::operator[](std::size_t i)
{
  TMASSERT(i < this->size());
  return this->at(i);
}
#endif


/*****
Debugging builds perform range checking.
*****/
#ifdef TMDEBUG
template <class T>
const T& tmArray<T>::operator[](std::size_t i) const
{
  TMASSERT(i < this->size());
  return this->at(i);
}
#endif


/*****
Return the nth item in the list. Debugging builds perform range-checking.
*****/
template <class T>
T& tmArray<T>::NthItem(std::size_t n)
{
  TMASSERT((n >= 1) && (n <= this->size()));
  return (*this)[n - 1];
}


/*****
Add an element to the beginning of the list
*****/
template <class T>
void tmArray<T>::push_front(const T& t)
{
  insert(this->begin(), t);
}



/*****
Add an element to the list if it isn't already there
*****/
template <class T>
void tmArray<T>::union_with(const T& t)
{
  if (find(this->begin(), this->end(), t) == this->end()) push_back(t);
}


/*****
Return the 1-based index of the given item. Return BAD_INDEX if it doesn't
exist.
*****/
template <class T>
std::size_t tmArray<T>::GetIndex(const T& t) const
{
  std::size_t n = 1 + 
    std::size_t(find(this->begin(), this->end(), t) - this->begin());
  if (n <= this->size()) return n;
  else return BAD_INDEX;
}
  

/*****
Remove an item from a list given its index
*****/
template <class T>
tmArray<T>& tmArray<T>::RemoveItemAt(std::size_t n)
{
  erase(this->begin() + ptrdiff_t(n) - 1);
  return *this;
}


/*****
Remove an item from a list given its value
*****/
template <class T>
void tmArray<T>::erase_remove(const T& t)
{
  erase(remove(this->begin(), this->end(), t), this->end());
}


/*****
Replace all occurrences of an item with a different item
*****/
template <class T>
void tmArray<T>::replace_with(const T& told, const T& tnew)
{
  if (told == tnew) return;
  iterator p = this->begin();
  while ((p = find(p, this->end(), told)) != this->end()) *p = tnew;
}


/*****
Insert an item at a specified index
*****/
template <class T>
tmArray<T>& tmArray<T>::InsertItemAt(std::size_t n, const T& t)
{
  TMASSERT(n > 0); // 1-based indexing
  insert(this->begin() + ptrdiff_t(n) - 1, t);
  return *this;
}


/*****
Replace an item in a list with another, based on location
*****/
template <class T>
tmArray<T>& tmArray<T>::ReplaceItemAt(std::size_t n, const T& t)
{
  TMASSERT(n > 0); // 1-based indexing
  (*this)[std::size_t(n - 1)] = t;
  return (*this);
}


/*****
Swap two items in a list.
*****/
template <class T>
void tmArray<T>::SwapItems(std::size_t inIndexA, std::size_t inIndexB)
{
  TMASSERT(inIndexA > 0); // 1-based indexing
  TMASSERT(inIndexB > 0); // 1-based indexing
  T t = (*this)[std::size_t(inIndexA - 1)];
  (*this)[std::size_t(inIndexA - 1)] = (*this)[std::size_t(inIndexB - 1)];
  (*this)[std::size_t(inIndexB - 1)] = t;
}


/*****
Move one item to another location in a list based on location
*****/
template <class T>
void tmArray<T>::MoveItem(std::size_t inFromIndex, std::size_t inToIndex)
{
  TMASSERT(inFromIndex > 0); // 1-based indexing
  TMASSERT(inToIndex > 0);   // 1-based indexing
  T t = (*this)[std::size_t(inFromIndex - 1)];
  RemoveItemAt(inFromIndex);
  InsertItemAt(inToIndex, t);
}


/*****
Rotate all items toward the front of the array by one position.
*****/
template <class T>
void tmArray<T>::rotate_left()
{
  if (this->empty()) return;
  T t = this->front();
  erase(this->begin());
  this->push_back(t);
}


/*****
Rotate all items toward the back of the array by one position.
*****/
template <class T>
void tmArray<T>::rotate_right()
{
  if (this->empty()) return;
  T t = this->back();
  erase(this->rbegin());
  this->push_front(t);
}


/*****
Return the offset of the given item. Return BAD_OFFSET if it doesn't exist.
*****/
template <class T>
std::size_t tmArray<T>::GetOffset(const T& t) const
{
  std::size_t i = 
    std::size_t(find(this->begin(), this->end(), t) - this->begin());
  if (i < this->size()) return i;
  else return BAD_OFFSET;
}


/*****
Append all elements of another list
*****/

template <class T>
void tmArray<T>::merge_with(const tmArray<T>& aList)
{
  insert(this->end(), aList.begin(), aList.end());
}


/*****
Append the elements of another list that are different to this one.
*****/
template <class T>
void tmArray<T>::union_with(const tmArray<T>& aList)
{
  for (std::size_t i = 0; i < aList.size(); ++i) union_with(aList[i]);
}


/*****
Retain only the elements contained by another list
*****/
template <class T>
void tmArray<T>::intersect_with(const tmArray<T>& aList)
{
  for (std::size_t i = 0; i < this->size(); ++i) {
    T& t = (*this)[i];
    if (!aList.contains(t)) {
      erase_remove(t);
      --i;
    }
  }
}


/*****
Return true if the two lists have any common element
*****/
template <class T>
bool tmArray<T>::intersects(const tmArray<T>& aList) const
{
  for (std::size_t n = this->size(); n > 0; --n) {
    const T& t = (*this)[n - 1];
    if (aList.contains(t)) return true;
  };
  return false;
}


#ifdef TMDEBUG
/*****
Return a string description of an array of pointers to tmPart classes: return
size and indices of contents. Note that this will cause a compile error if
invoked for anything other than an array of ptrs to tmPart or descendants.
*****/
template <class T>
std::string tmArray<T>::GetStr() const
{
  std::ostringstream ss;
  ss << "[" << this->size() << "]: ";
  for (std::size_t i = 0; i < this->size(); ++i)
    ss << (*this)[i]->GetIndex() << " ";
  return ss.str();
}
#endif


#endif // _TMARRAY_H_

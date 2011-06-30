/*******************************************************************************
File:         tmArrayTester.cpp
Project:      TreeMaker 5.x
Purpose:      Test application for the tmArray container classes
Author:       Robert J. Lang
Modified by:  
Created:      2005-09-27
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

// Standard libraries

#include <iostream>
#include <sstream>
using namespace std;

// My libraries
#include "tmArray.h"          // my general-purposes container class
#include "tmArrayIterator.h"  // my redefinition of PowerPlant tmArrayIterator


// tmArrayIterator test
void test1(tmArray<char*>& aList);
void test1(tmArray<char*>& aList)
{
  char ch;
  tmArrayIterator<char*> i(aList);
  char* c;
  while (i.Next(&c)) ch = *c;
}


// for loop test
void test2(tmArray<char*>& aList);
void test2(tmArray<char*>& aList)
{
  char ch;
  size_t nmax = aList.size();
  for (size_t i = 1; i <= nmax; ++i) ch = *(aList.NthItem(i));
}


// for loop with repeated size()
void test2a(tmArray<char*>& aList);
void test2a(tmArray<char*>& aList)
{
  char ch;
  for (size_t i = 1; i <= aList.size(); ++i) ch = *(aList.NthItem(i));
}


// class for test of stored pointers
class Foo {
  public:
    void DoSomething() {cout << "foo!" << endl;}
};


// stream output for a list (helps in testing)
template <class T>
ostream& operator<<(ostream& os, tmArray<T>& aList);
template <class T>
ostream& operator<<(ostream& os, tmArray<T>& aList)
{
  os << "<";
  for (size_t i = 0; i < aList.size(); ++i) {
    os << aList[i];
    if (i != aList.size() - 1) os << ", ";
  }
  os << ">";
  return os;
}


// Main test program
int main(void)
{
  cout << "Hello World.\n";
  
  char* a = "A";
  char* b = "B";
  char* c = "C";
  char* d = "D";
  char* e = "E";
  char* f = "F";
  char* g = "G";
  char* h = "H";
  
  tmArray<char*> alist;
  
  alist.push_back(b);
  alist.push_back(c);
  alist.push_front(a);
  
  cout << "alist is " << alist << endl;
  cout << "size() is " << alist.size() << endl;
  cout << "front = " << *(alist.front()) << endl;
  cout << "NthItem(2) = " << *(alist.NthItem(2)) << endl;
  cout << "back = " << *(alist.back()) << endl;
  cout << endl;
  
  alist.union_with(a);
  alist.union_with(d);
  cout << "After union_with(a, d) the list is " << alist << endl;
  
  alist.erase_remove(d);
  cout << "After erase_remove(d) the list is " << alist << endl;
  
  
  alist.erase_remove(b);
  cout << "After erase_remove(b) the list is " << alist << endl;
  cout << "2nd item is now " << *(alist.NthItem(2)) << endl;
  cout << "size() is now " << alist.size() << endl;
  cout << endl;
  
  cout << "list contains a? " << (int) alist.contains(a) << endl;
  cout << "list contains b? " << (int) alist.contains(b) << endl;
  cout << "list contains c? " << (int) alist.contains(c) << endl;
  cout << endl;
  
  alist.ReplaceItemAt(1, d);
  cout << "After ReplaceItemAt(1, d) the list is " << alist << endl;
  cout << "1st item is now " << *(alist.NthItem(1)) << endl;
  cout << "Index of 'd' is " << alist.GetIndex(d) << endl;
  cout << "Offset of 'd' is " << alist.GetOffset(d) << endl;
  cout << "alist[0] = " << *(alist[0]) << endl;
  cout << endl;
  
  alist.push_back(e);
  cout << "After push_back(e) the list is " << alist << endl;
  
  alist.replace_with(d, a);
  cout << "After replace_with(d, a) the list is " << alist << endl;
  
  // tmArrayIterator/tmArrayIterator test
  tmArrayIterator<char*> i(alist);
  char* dp;
  cout << "Forward iterator test." << endl;
  while (i.Next(&dp)) cout << "an element is " << dp << endl;
  cout << "Backward iterator test." << endl;
  i.ResetTo(tmArray_END);
  while (i.Previous(&dp)) cout << "an element is " << dp << endl;
  cout << endl;
  
  // operator[] returns an lvalue 
  cout << endl;
  alist[0] = f;
  cout << "Set alist[0] = f; alist = " << alist << endl;
  cout << endl;
  
  // combinational routines 
  tmArray<char*> blist;
  blist.push_back(f);
  blist.push_back(g);
  blist.push_back(h);
  cout << "alist = " << alist << endl;
  cout << "blist = " << blist << endl;
  blist.intersect_with(alist);
  cout << "blist.intersect_with(alist) = " << blist << endl;
  blist.union_with(alist);
  cout << "blist.union_with(alist) = " << blist << endl;
  
  // intersection routines
  tmArray<int> clist;
  clist.push_back(1);
  clist.push_back(2);
  clist.push_back(3);
  clist.push_back(4);
  tmArray<int> dlist;
  dlist.push_back(5);
  dlist.push_back(4);
  cout << "clist = " << clist << endl;
  cout << "dlist = " << dlist << endl;
  cout << "clist.intersects(dlist) = " << (clist.intersects(dlist)) << endl;
  cout << "dlist.intersects(clist) = " << (dlist.intersects(clist)) << endl;
}

/*******************************************************************************
File:         tmDpptrTester.cpp
Project:      TreeMaker 5.x
Purpose:      Test application for the tmDpptr and tmDpptrArray container classes
Author:       Robert J. Lang
Modified by:  
Created:      2005-09-27
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include <iostream>
#include <string>

#include "tmDpptr.h"
#include "tmDpptrArray.h"

using namespace std;

/*
This program illustrates usage of the two reference classes tmDpptr<T> and
tmDpptrArray<T>.

A tmDpptr<T> is essentially a dangle-proof pointer-to-T; it behaves like a T*
and can be used anywhere you would normally use a T*, with one enhancement;
when the object of a tmDpptr<T> is deleted, the tmDpptr<T> sets itself to NULL.
Any subsequent attempt to dereference the pointer generates an exception.

A tmDpptrArray<T> acts like a tmArray<T*> (which is basically a vector<T*>),
but when any of the pointed-to objects are deleted, the pointer is completely
removed from the list.

You can give your own container classes the capability to interact with RefObjs
by descending them from the tmDpptrSrc class and overriding the appropriate
access member functions. See the implementation of tmDpptr and tmDpptrArray for
examples of how to do this.
*/

/* Main program */

class A;
class B;

// Classes A and B hold references to each other.

class A : public virtual tmDpptrTarget
{
  public:
    A() {cout << "member of class A created" << endl;}
    virtual ~A() {cout << "member of class A deleted" << endl;}
    tmDpptr<B> ab;
};


class B : public virtual tmDpptrTarget
{
  public:
    B() {cout << "member of class B created" << endl;}
    virtual ~B() {cout << "member of class B deleted" << endl;}
    tmDpptr<A> ba;
    
    void Test() {cout << "test B!" << endl;}
};


class D : public virtual tmDpptrTarget
{
  public:
    D(char* aName) : tmDpptrTarget() {strcpy(mName, aName); 
      cout << mName << " created" << endl;}
    virtual ~D() {cout << mName << " deleted" << endl;}
  private:
    char mName[20];
};


int main(void)
{
  cout << "Hello World\n";
  
  // Create two objects and assign their references to each other.

  A* a = new A();   // a->ab is initialized to NULL.
  B* b = new B();   // b->ba is initialized to NULL.
  
  b->ba = a;
  a->ab = b;
  
  tmDpptr<B> c = b; // create another reference to b
  
  cout << "a has " << a->GetNumSrcs() << " references to it" << endl;
  cout << "b has " << b->GetNumSrcs() << " references to it" << endl;
  
  delete a; // Lots of stuff happens right here:
    // If ab and ba had been ordinary pointers, b->ba would dangle. But since
    // we're using references, b->ba has been automatically set to null. Also,
    // the number of references to b has been decreased (since a->ab no longer
    // exists).
        
  cout << "a is deleted; now b has " << b->GetNumSrcs() << 
    " references to it" << endl;
  
  // Test out the various ways of using a reference like a pointer.
  
  cout << "Test of pointer usage:" << endl;
  
  ((B*) c)->Test(); // cast to a pointer
  (*c).Test();    // dereference
  c->Test();      // indirect dereference

  c = 0;    // Reassigning c removes the last reference to b.
  
  cout << endl;
  
  // Now try out a tmDpptrArray that automatically removes objects as they are
  // are deleted.
  
  tmDpptrArray<D> rld;    // create a list of references

  D* d1 = new D("d1");  // create a bunch of objects to put into the list
  D* d2 = new D("d2");
  D* d3 = new D("d3");
  
  tmDpptrTarget* rd1 = d1;
  tmDpptrTarget* rd2 = d2;
  tmDpptrTarget* rd3 = d3;
  
  rld.push_back(d1);    // put them into the list.
  rld.push_back(d2);
  rld.push_back(d3);
  rld.push_back(d1);    // also check out effect of multiple references.
  
  // Now we'll delete the original objects one by one and watch as they are
  // automatically removed from the list.
  
  cout << "Initially rld has " << rld.size() << " elements." << endl;
  
  delete d1;
  cout << "After delete d1 rld has " << rld.size() << " elements." << endl;
  
  delete d2;
  cout << "After delete d2 rld has " << rld.size() << " elements." << endl;
  
  delete d3;
  cout << "After delete d3 rld has " << rld.size() << " elements." << endl;
  
  // Try it again but this time test the clear() command
  
  D* d4 = new D("d4");  // create a bunch of objects to put into the list
  D* d5 = new D("d5");
  D* d6 = new D("d6");
  
  rld.push_back(d4);    // put them into the list.
  rld.push_back(d5);
  rld.push_back(d6);
  rld.push_back(d4);    // also check out effect of multiple references.
  cout << "Initially rld has " << rld.size() << " elements." << endl;
  
  rld.clear();
  cout << "After clear() rld has " << rld.size() << " elements." << endl;

  // done

  cout << "Bye...\n";
}

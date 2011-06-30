/*******************************************************************************
File:         tmCreaseOwner.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmCreaseOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmCreaseOwner.h"
#include "tmModel.h"

using namespace std;

/*
tmCreaseOwner keeps a list of Creases that it owns in a list; when this object
is deleted, it automatically deletes all the objects in the list. Similarly,
when a tmCrease is deleted, it tells its owner, which removes it from the list.
*/

/**********
class tmCreaseOwner
Base class for an object that owns creases and is responsible for their
deletion.
Subclasses: tmPath, tmPoly
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


#ifdef __MWERKS__
  #pragma mark --PROTECTED--
#endif


/*****
Destructor deletes all owned objects
*****/
tmCreaseOwner::~tmCreaseOwner()
{
  mOwnedCreases.KillItems();
}


#ifdef __MWERKS__
  #pragma mark --PRIVATE--
#endif


/*****
Return the crease that runs between these two Vertices. If one doesn't exist,
create one of the given kind.
*****/
tmCrease* tmCreaseOwner::GetOrMakeCrease(tmVertex* aVertex1, tmVertex* aVertex2, 
  tmCrease::Kind aKind)
{
  tmCrease* theCrease = GetCrease(aVertex1, aVertex2);
  if (theCrease) return theCrease;
  return new tmCrease(aVertex1->mTree, this, aVertex1, aVertex2, aKind);
}


/*****
Return the crease that connects these two vertices. Return 0 if none exists.
*****/
tmCrease* tmCreaseOwner::GetCrease(tmVertex* aVertex1, tmVertex* aVertex2)
{
  for (size_t i = 0; i < mOwnedCreases.size(); ++i) {
    tmCrease* theCrease = mOwnedCreases[i];
    if (((theCrease->mVertices.front() == aVertex1) && 
       (theCrease->mVertices.back() == aVertex2)) ||
      ((theCrease->mVertices.front() == aVertex2) && 
       (theCrease->mVertices.back() == aVertex1))) return theCrease;
  }
  return 0;
}

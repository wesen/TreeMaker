/*******************************************************************************
File:         tmDpptrTarget.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmDpptrTarget
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmDpptrTarget.h"
#include "tmDpptrSrc.h"

#include <algorithm>

using namespace std;

/*
A tmDpptrTarget is an object that can be the target of a tmDpptrSrc object, which is
a dangle-proof pointer. The tmDpptrTarget keeps a list of all DpptrSrcs that point to it. 
When the tmDpptrTarget is deleted, it notifies the tmDpptrSrc, which nulls its pointer. 
When a tmDpptrSrc is destroyed (or reassigned), it notifies the tmDpptrTarget which 
removes the tmDpptr from its list of DpptrSrcs. Thus, no tmDpptrSrc will ever dangle,
that is, point incorrectly into memory.
*/


/**********
class tmDpptrTarget
Base class for any object that is pointed to by a tmDpptr<T> or a tmDpptrArray<T>.
**********/

/*****
tmDpptrTarget::tmDpptrTarget()
Constructor. Nothing special happens.
*****/
tmDpptrTarget::tmDpptrTarget()
{
}


/*****
tmDpptrTarget::tmDpptrTarget(const tmDpptrTarget&)
Copy constructor. If we clone a target, the clone will start out with no sources
pointing at it.
*****/
tmDpptrTarget::tmDpptrTarget(const tmDpptrTarget&)
  : mDpptrSrcs()
{
}


/*****
tmDpptrTarget& tmDpptrTarget::operator=(const tmDpptrTarget&)
Assignment. If we're assigning one target's value to another, we won't copy over
the list of sources.
*****/
tmDpptrTarget& tmDpptrTarget::operator=(const tmDpptrTarget&)
{
  // Do nothing
  return *this;
}


/*****
tmDpptrTarget::~tmDpptrTarget()
Notify any DpptrSrcs that point to me to clear their pointers to me.
*****/
tmDpptrTarget::~tmDpptrTarget()
{
  //  Note: a tmDpptrArray<T> can hold multiple references to the same object, in which
  //  case mDpptrSrcs will hold multiple pointers to the same tmDpptrArray<T>.
  vector<tmDpptrSrc*> theDpptrSrcs(mDpptrSrcs);
  for (size_t i = 0; i < theDpptrSrcs.size(); ++i) {
    tmDpptrSrc* theDpptrSrc = theDpptrSrcs[i];
    theDpptrSrc->RemoveDpptrTarget(this);
  }
}


/*****
void tmDpptrTarget::AddDpptrSrc(tmDpptrSrc* r)
Add a pointer-to-me
called by:
tmDpptrSrc::DstAddMeAsDpptrSrc(tmDpptrTarget*)
*****/
void tmDpptrTarget::AddDpptrSrc(tmDpptrSrc* r)
{
  mDpptrSrcs.push_back(r);
}


/*****
void tmDpptrTarget::RemoveDpptrSrc(tmDpptrSrc* r)
Remove a pointer-to-me
called by:
tmDpptrSrc::DstRemoveMeAsDpptrSrc(tmDpptrTarget*)
*****/

void tmDpptrTarget::RemoveDpptrSrc(tmDpptrSrc* r)
{
  mDpptrSrcs.erase(remove(mDpptrSrcs.begin(), mDpptrSrcs.end(), r), mDpptrSrcs.end());
}

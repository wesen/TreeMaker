/*******************************************************************************
File:         tmPathOwner.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmPathOwner
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmPathOwner.h"
#include "tmModel.h"

using namespace std;

/*
Class tmPathOwner keeps a list of paths that it owns in a list; when this
object is deleted, it automatically deletes all the objects in the list.
Similarly, when a path is deleted, it tells its owner, which removes it from
the list.
*/

/**********
class tmPathOwner
Base class for an object that owns paths and is responsible for their deletion.
Subclasses: tmPoly, tmTree
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


/*****
Return the path (if any) owned by this tmPathOwner that connects the two given
nodes. If no path is found, return a null pointer. If we're only looking for
leaf paths, this routine is more efficient than FindAnyPath(..) below; it's
O(N).
*****/
tmPath* tmPathOwner::FindLeafPath(tmNode* node1, tmNode* node2) const
{
  TMASSERT(node1);
  TMASSERT(node2);
  tmArray<tmPath*>& plist = node1->mLeafPaths;
  for (size_t i = 0; i < plist.size(); ++i) {
    tmPath* thePath = plist[i];
    if (thePath->mNodes.front() == node2 || thePath->mNodes.back() == node2)
      return thePath;
  }
  return 0;
}


/*****
Return the path (if any) owned by this tmPathOwner that connects the two given
nodes. If no path is found, return a null pointer. This is less efficient than
FindLeafPath (it's O(N^2) but it works for paths between non-leaf nodes.
(However, it requires that mOwnedPaths exists, which is not the case during
stream I/O.)
*****/
tmPath* tmPathOwner::FindAnyPath(tmNode* node1, tmNode* node2) const
{
  TMASSERT(node1);
  TMASSERT(node2);
  tmArrayIterator<tmPath*> i(mOwnedPaths);
  tmPath* aPath;
  while (i.Next(&aPath)) {
    if ((aPath->mNodes.front() == node1) &&
      (aPath->mNodes.back() == node2)) return aPath;
    if ((aPath->mNodes.front() == node2) &&
      (aPath->mNodes.back() == node1)) return aPath;
  }
  return 0;
}


#ifdef __MWERKS__
  #pragma mark --PROTECTED--
#endif


/*****
Destructor deletes all owned paths
*****/
tmPathOwner::~tmPathOwner()
{
  mOwnedPaths.KillItems();
}


#ifdef __MWERKS__
  #pragma mark --PRIVATE--
#endif

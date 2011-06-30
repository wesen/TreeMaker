/*******************************************************************************
File:         tmTreeCleaner.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for the tmTreeCleaner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmTreeCleaner.h"
#include "tmModel.h"

/*  
Class tmTreeCleaner is a stack class to create an instance of when the tmTree
is about to be modified. The usage format is:
  
{
  tmTreeCleaner tc(aTree);
  
  // ...call tree modifier routines, which may themselves instantiate
  // tmTreeCleaner objects
    
}  
  // when the top-level tmTreeCleaner ("aTreeCleaner" in this example) is
  // destroyed (goes out of scope), it cleans up the tmTree
  
You should always create an instance of a tmTreeCleaner before you do anything
that changes any aspect of the tree directly. When the tmTreeCleaner goes out
of scope, it calls the private routine tmTree::CleanupAfterEdit(), which does
exactly what its name implies -- cleans things up, destroying objects that got
invalidated and resetting variable flags.

Note that all of the structural routines call CleanupAfterEdit via a
tmTreeCleaner object. The class is designed so that even if you make multiple
calls to structural routines or make multiple changes, CleanupAfterEdit() is
only called once when the outermost tmTreeCleaner is destructed.

Consequently, you should NEVER create a tmTreeCleaner on the heap (via new()),
because that circumvents the entire system and Bad Things Will Happen.
  
HOW IT WORKS

The tmTree has a member variable, mNeedsCleanup, which is normally false. When
you create an automatic tmTreeCleaner object, it notes the previous status of
this flag in its member variable mTreeNeededCleanup and sets mNeedsCleanup to
true. When the tmTreeCleaner object is destructed, it calls
tmTree::CleanupAfterEdit() only if the tree was dirty when it was created. In
this way, you can make multiple calls to structural routines that change the
tree but CleanupAfterEdit() will only be called once, at the destruction of the
outermost tmTreeCleaner.
*/
  
/**********
class tmTreeCleaner
A stack class that handles cleaning up a tmTree after editing
**********/
  
/*****
Record the tmTree and its state of dirty
*****/
tmTreeCleaner::tmTreeCleaner(tmTree* aTree)
{
  mTree = aTree;
  mTreeNeededCleanup = mTree->mNeedsCleanup;
  mTree->mNeedsCleanup = true;
}


/*****tmTreeCleaner::~tmTreeCleaner()
If tmTree wasn't dirty when this was created, call CleanupAfterEdit().
*****/
tmTreeCleaner::~tmTreeCleaner()
{
  if (mTreeNeededCleanup) return;
  mTree->CleanupAfterEdit();
  mTree->mNeedsCleanup = false;
}

/*******************************************************************************
File:         tmEdgeOwner.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmEdgeOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmEdgeOwner.h"
#include "tmModel.h"

/*
Class tmEdgeOwner keeps a list of Edges that it owns in a list; when this
object is deleted, it automatically deletes all the objects in the list.
Similarly, when a tmEdge is deleted, it tells its owner, which removes it from
the list.
*/

/**********
class tmEdgeOwner
Base class for an object that owns edges and is responsible for their deletion.
Subclasses: tmTree
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


#ifdef __MWERKS__
  #pragma mark --PROTECTED--
#endif


/*****
Destructor deletes all owned edges
*****/
tmEdgeOwner::~tmEdgeOwner()
{
  mOwnedEdges.KillItems();
}


#ifdef __MWERKS__
  #pragma mark --PRIVATE--
#endif

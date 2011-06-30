/*******************************************************************************
File:         tmNodeOwner.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmNodeOwner
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmNodeOwner.h"
#include "tmModel.h"

/*
Class tmNodeOwner keeps a list of nodes that it owns in a list; when this
object is deleted, it automatically deletes all the objects in the list.
Similarly, when a tmNode is deleted, it tells its owner, which removes it from
the list.
*/


/**********
class tmNodeOwner
Base class for an object that owns nodes and is responsible for their deletion.
Subclasses: tmPoly, tmTree
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


#ifdef __MWERKS__
  #pragma mark --PROTECTED--
#endif


/*****
Destructor deletes all owned nodes
*****/
tmNodeOwner::~tmNodeOwner()
{
  mOwnedNodes.KillItems();
}


#ifdef __MWERKS__
  #pragma mark --PRIVATE--
#endif



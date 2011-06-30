/*******************************************************************************
File:         tmVertexOwner.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmVertexOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmVertexOwner.h"
#include "tmModel.h"

/*
tmVertexOwner is a class that keeps a list of Vertices that it owns in a list;
when this object is deleted, it automatically deletes all the objects in the
list. Similarly, when a tmVertex is deleted, it tells its owner, which removes
it from the list.
*/

/**********
class tmVertexOwner
Class that owns vertices and is responsible for their deletion.
Subclasses: tmNode, tmPath
**********/

/*****
Destructor deletes all owned vertices
*****/
tmVertexOwner::~tmVertexOwner()
{
  mOwnedVertices.KillItems();
}

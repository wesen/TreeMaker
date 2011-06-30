/*******************************************************************************
File:         tmConditionOwner.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionOwner.h"
#include "tmModel.h"

/*
Class tmConditionOwner keeps a list of Conditions that
it owns in a list; when this object is deleted, it automatically
deletes all the objects in the list. Similarly, when a tmCondition is
deleted, it tells its owner, which removes it from the list.
Subclasses:
tmTree
*/

/**********
class tmConditionOwner
Base class for an object that owns conditions and is responsible for their deletion
**********/

/*****
tmConditionOwner::~tmConditionOwner()
Destructor deletes all owned objects
*****/
tmConditionOwner::~tmConditionOwner()
{
  mOwnedConditions.KillItems();
}

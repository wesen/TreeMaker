/*******************************************************************************
File:         tmwxCommand.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker command class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxCommand.h"
#include "tmModel.h"
#include "tmwxDoc.h"

using namespace std;

/*
TreeMaker command processing scheme.
Even a fairly simple command -- like deleting a single node -- can have 
far-reaching effects, like the deletion of many nodes, edges, paths, polys,
vertices, creases, conditions, and resetting of numerous flags. So, each
command holds a serialized version of the tree before and after the command.

Since tmwxDoc submits a command AFTER the tree has been modified, tmwxDoc
holds a serialized copy of the previous tree state in its mCleanState variable.
We use this to get our "before" state.
*/


/*****
Constructor
*****/
tmwxCommand::tmwxCommand(const wxString& name, tmwxDoc* aDoc) : 
  wxCommand(true, name), 
  mDoc(aDoc), 
  mFirstDo(true)
{
}


/*****
Destructor
*****/
tmwxCommand::~tmwxCommand()
{
}


/*****
Perform the command. The first time we Do() the command, we don't need to do
anything, but subsequent calls (e.g., Undo followed by Redo) will need to work
from our stored copy.
*****/
bool tmwxCommand::Do()
{
  if (mFirstDo) {
    mBefore << mDoc->mCleanState.str();
    mDoc->mTree->PutSelf(mAfter);
    mDoc->mCleanState.str("");
    mDoc->mCleanState << mAfter.str();
    mFirstDo = false;
  }
  else {
    mDoc->mTree->GetSelf(mAfter.seekg(0));
    TMASSERT(mAfter.eof());
    mAfter.clear(ios_base::goodbit);
    mDoc->mCleanState.str("");
    mDoc->mCleanState << mAfter.str();
  }
  mDoc->Modify(true);
  mDoc->UpdateAllViews();
  return true;
}


/*****
Undo the command.
*****/
bool tmwxCommand::Undo()
{
  mDoc->mTree->GetSelf(mBefore.seekg(0));
  TMASSERT(mBefore.eof());
  mBefore.clear(ios_base::goodbit);
  mDoc->mCleanState.str("");
  mDoc->mCleanState << mBefore.str();
  mDoc->Modify(true);
  mDoc->UpdateAllViews();
  return true;
}


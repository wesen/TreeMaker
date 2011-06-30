/*******************************************************************************
File:         tmwxCommand.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker command class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXCOMMAND_H_
#define _TMWXCOMMAND_H_

// Additional wxWidgets classes
#include "wx/cmdproc.h"

// Additional libraries
#include <sstream>

// forward declarations
class tmTree;
class tmwxDoc;

/**********
class tmwxCommand
**********/
class tmwxCommand: public wxCommand
{
protected:
  tmwxDoc* mDoc;              // the document this command deals with
  bool mFirstDo;              // true = we're on the first invocation of Do()
  std::stringstream mBefore;  // state of tree before command execution
  std::stringstream mAfter;   // state of tree after command execution
public:
  tmwxCommand(const wxString& name, tmwxDoc* adoc);
  ~tmwxCommand();
  
  bool Do();
  bool Undo();
};

#endif // _TMWXCOMMAND_H_

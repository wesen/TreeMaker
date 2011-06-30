/*******************************************************************************
File:         tmConditionOwner.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2004-06-14
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONOWNER_H_
#define _TMCONDITIONOWNER_H_

// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmDpptrArray.h"


/**********
class tmConditionOwner
Base class for an object that owns conditions and is responsible for their
deletion
Subclasses: tmTree
**********/
class tmConditionOwner {
public:
  const tmDpptrArray<tmCondition>& GetOwnedConditions() const {
      return mOwnedConditions;};
protected:
  virtual ~tmConditionOwner();    
private:
  tmDpptrArray<tmCondition> mOwnedConditions;
  friend class tmTree;
  friend class tmCondition;
};


#endif // _TMCONDITIONOWNER_H_

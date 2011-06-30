/*******************************************************************************
File:         tmTreeCleaner.h
Project:      TreeMaker 5.x
Purpose:      Header file for classes tmTreeCleaner
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMTREECLEANER_H_
#define _TMTREECLEANER_H_

// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"


/**********
class tmTreeCleaner
A stack class that handles cleaning up a tmTree after editing
**********/
class tmTreeCleaner {
public:
  tmTreeCleaner(tmTree* aTree);
  ~tmTreeCleaner();
  
  tmTree* GetTree() const {
    // return the tree that this cleaner is responsible for.
    return mTree;
  };
  
private:
  tmTree* mTree;
  bool mTreeNeededCleanup;
};


#endif // _TMTREECLEANER_H_

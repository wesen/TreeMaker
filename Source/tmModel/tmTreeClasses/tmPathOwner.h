/*******************************************************************************
File:         tmPathOwner.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmPathOwner
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMPATHOWNER_H_
#define _TMPATHOWNER_H_

// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmDpptrArray.h"


/**********
class tmPathOwner
Base class for an object that owns paths and is responsible for their deletion.
Subclasses: tmPoly, tmTree
**********/
class tmPathOwner {
public:
  const tmDpptrArray<tmPath>& GetOwnedPaths() const {
    // Return read-only list of all owned paths.
    return mOwnedPaths;
  };
  
  tmPath* FindLeafPath(tmNode* node1, tmNode* node2) const;
  tmPath* FindAnyPath(tmNode* node1, tmNode* node2) const;
  
protected:
  virtual ~tmPathOwner();
  virtual tmTree* PathOwnerAsTree() = 0;
  virtual tmPoly* PathOwnerAsPoly() = 0;
  
private:
  tmDpptrArray<tmPath> mOwnedPaths;
  
  friend class tmTree;
  friend class tmPath;
  friend class tmPoly;
};


#endif // _TMPATHOWNER_H_

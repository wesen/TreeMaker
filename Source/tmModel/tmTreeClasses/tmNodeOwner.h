/*******************************************************************************
File:         tmNodeOwner.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmNodeOwner
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMNODEOWNER_H_
#define _TMNODEOWNER_H_


// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmDpptrArray.h"


/**********
class tmNodeOwner
Base class for an object that owns nodes and is responsible for their deletion.
Subclasses: tmPoly, tmTree
**********/
class tmNodeOwner {
public:
  const tmDpptrArray<tmNode>& GetOwnedNodes() {
    // Return read-only list of all owned nodes.
    return mOwnedNodes;
  };
  
protected:
  virtual ~tmNodeOwner();
  
private:
  tmDpptrArray<tmNode> mOwnedNodes;

  virtual tmTree* NodeOwnerAsTree() = 0;
  virtual tmPoly* NodeOwnerAsPoly() = 0;
  
  friend class tmTree;
  friend class tmNode;
  friend class tmPoly;
//  friend class tmScaleOptimizer;
};


#endif // _TMNODEOWNER_H_

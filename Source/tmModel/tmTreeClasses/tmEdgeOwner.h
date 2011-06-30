/*******************************************************************************
File:         tmEdgeOwner.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmEdgeOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMEDGEOWNER_H_
#define _TMEDGEOWNER_H_

// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmDpptrArray.h"


/**********
class tmEdgeOwner
Base class for an object that owns edges and is responsible for their deletion.
Subclasses: tmTree
**********/
class tmEdgeOwner {
public:
  const tmDpptrArray<tmEdge>& GetOwnedEdges() const {
    // Return read-only list of all owned edges.
    return mOwnedEdges;
  };
  
protected:
  virtual ~tmEdgeOwner(); 
  
private:
  tmDpptrArray<tmEdge> mOwnedEdges;
  
  friend class tmTree;
  friend class tmEdge;
};


#endif // _TMEDGEOWNER_H_

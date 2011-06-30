/*******************************************************************************
File:         tmVertexOwner.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmVertexOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMVERTEXOWNER_H_
#define _TMVERTEXOWNER_H_

// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmDpptrArray.h"
#include "tmPoint.h"


/**********
class tmVertexOwner
Class that owns vertices and is responsible for their deletion.
Subclasses: tmNode, tmPath
**********/
class tmVertexOwner {
public:
  const tmDpptrArray<tmVertex>& GetOwnedVertices() const {
    // Return read-only list of all owned vertices.
    return mOwnedVertices;
  };
  
protected:
  virtual ~tmVertexOwner();
  virtual tmNode* VertexOwnerAsNode() = 0;
  virtual tmPath* VertexOwnerAsPath() = 0;
  
private:
  tmDpptrArray<tmVertex> mOwnedVertices;
  
  friend class tmTree;
  friend class tmNode;
  friend class tmPath;
  friend class tmPoly;
  friend class tmVertex;
  friend class tmRootNetwork;
};


#endif // _TMVERTEXOWNER_H_

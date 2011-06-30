/*******************************************************************************
File:         tmPolyOwner.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmPolyOwner
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMPOLYOWNER_H_
#define _TMPOLYOWNER_H_

// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmPoint.h"
#include "tmNodeOwner.h"
#include "tmPathOwner.h"
#include "tmDpptrArray.h"


/**********
class tmPolyOwner
Base class for an object that owns polys and is responsible for their deletion.
Subclasses: tmPoly, tmTree
**********/
class tmPolyOwner : public tmNodeOwner, public tmPathOwner
{
public:
  const tmDpptrArray<tmPoly>& GetOwnedPolys() const {
    // Return read-only list of all owned polys.
    return mOwnedPolys;
  };
  
  bool SuperOwns(tmPoly* aPoly) const;
  
protected:
  virtual ~tmPolyOwner();
  virtual tmTree* PolyOwnerAsTree() = 0;
  virtual tmPoly* PolyOwnerAsPoly() = 0;
  
private:
  tmDpptrArray<tmPoly> mOwnedPolys;
  
  // Polygon network construction
  bool CanStartPolyFwd(tmPath* aPath, const tmPoint& centroid);
  bool CanStartPolyBkd(tmPath* aPath, const tmPoint& centroid);
  void GetNextPathAndNode(tmPath* thisPath, tmNode* thisNode,
    tmPath*& nextPath, tmNode*& nextNode);    
  void BuildPolysFromPaths(tmArray<tmPath*>& aPathList,
    tmArray<tmNode*>& borderNodes);
  
  friend class tmTree;
  friend class tmPoly;
  friend class tmStubFinder;
};

#endif // _TMPOLYOWNER_H_

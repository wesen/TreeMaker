/*******************************************************************************
File:         tmStubFinder.h
Project:      TreeMaker 5.x
Purpose:      Header file for the tmStubFinder class
Author:       Robert J. Lang
Modified by:  
Created:      2004-01-04
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMSTUBFINDER_H_
#define _TMSTUBFINDER_H_

// TreeMaker Headers
#include "tmHeader.h"

// Other TreeMaker classes
#include "tmPoint.h"
#include "tmNewtonRaphson.h"
#include "tmArray.h"

// Forward declarations
class tmTree;
class tmNode;
class tmEdge;
class tmPoly;

/**********
struct tmStubInfo
A collection of records that characterizes a single solution to the stub
equations
**********/
struct tmStubInfo {
  tmEdge* mEdge;    // the tmEdge to be split
  tmFloat mLength;    // the length of the stub
  tmFloat mEdgeloc; // the distance from the first tmNode of the tmEdge
  tmPoint mLoc;   // location of the new tmNode
  tmArray<tmNode*> mActiveNodes;  // all nodes that form active paths with stub
  
  tmStubInfo(tmEdge* aEdge = 0, const tmFloat& aLength = 0, 
    const tmFloat& aEdgeloc = 0, const tmPoint& aLoc = tmPoint(0., 0.));
  bool IsBlank() const {return !mEdge;};
  bool IsNotBlank() const {return !!mEdge;};
  bool operator==(const tmStubInfo& rhs) const;
  bool operator!=(const tmStubInfo& rhs) const;
  bool operator<(const tmStubInfo& fhs) const;
};


/**********
class tmStubFinder
Class that solves for stubs added to the tree that give 4 (or more) active
paths.
**********/
class tmStubFinder : private tmNewtonRaphson<tmFloat> {
public:
  // Exceptions
  class EX_BAD_POLY_SIZE {};
  class EX_NO_EDGES {};
  class EX_BAD_POLY_KIND {};
  // Constructor
  tmStubFinder(tmTree* aTree);
  // Actions
  void FindAllStubs(const tmArray<tmNode*>& aNodeList, 
    tmArray<tmStubInfo>& sInfoList);
  tmStubInfo FindLargestStub(const tmArray<tmNode*>& aNodeList);
  void AddStubToTree(const tmStubInfo& stubInfo);
  void TriangulateTree();
protected:
  void UserFn(const std::vector<tmFloat>& x, tmMatrix<tmFloat>& a, 
    std::vector<tmFloat>& b);
private:
  tmTree* mTree;          // the current tree
  tmFloat mScale;         // scale of the tree
  tmMatrix<tmFloat> mParms;   // parameters that define eqns to solve
  tmArray<tmNode*> mLeafNodes;  // leaf nodes of the tree
  tmArray<tmEdge*> mSpanningEdges;// spanning edges of the set of nodes
  tmNode* mTrialNodes[4];     // one combination of four nodes
  tmEdge* mTrialEdge;       // and one edge
  tmStubFinder();
  tmStubFinder(const tmStubFinder& aStubFinder);
  void TestOneCombo(tmArray<tmStubInfo>& sInfoList);
};

#endif // _TMSTUBFINDER_H_

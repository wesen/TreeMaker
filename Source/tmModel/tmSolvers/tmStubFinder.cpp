/*******************************************************************************
File:         tmStubFinder.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for the tmStubFinder class
Author:       Robert J. Lang
Modified by:  
Created:      2004-01-04
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmStubFinder.h"

#include "tmArray.h"
#include "tmArrayIterator.h"
#include "tmTreeCleaner.h"
#include "tmTree.h"
#include "tmNode.h"
#include "tmEdge.h"
#include "tmPath.h"
#include "tmPoly.h"
#include "tmNewtonRaphson.h"

using namespace std;

/*
tmStubFinder is an object that breaks up a polygon network by adding a new
tmNode to the tree in such a way that it makes at least four active paths with
nodes of the polygon. Applied to a quad, it triangulates it; repeated
applications can fully triangulate any network of active paths.

5-3-98. I've generalized the algorithm so that you can apply a stub to any
group of four nodes, whether or not they're part of a polygon.

The private member variables mScale and mParms are parameters that are shared
between FindAllStubs() and UserFn() (which is overloaded from class
tmNewtonRaphson). Their meanings are:

mScale = scale of tree (mTree->mScale)
mParms[i][0] = x coordinate of ith polygon node
mParms[i][1] = y coordinate of ith polygon node
mParms[i][2] = length of path from ith polygon node to first node in split edge
mParms[i][3] = -1 if path from polygon node to first node in split edge
      contains the split edge, +1 if it doesn't
*/


/**********
class tmStubInfo
A collection of records that characterizes a single solution to the stub
equations
**********/

/*****
Constructor. Initialize data fields.
*****/
tmStubInfo::tmStubInfo(tmEdge* aEdge, const tmFloat& aLength, 
  const tmFloat& aEdgeloc, const tmPoint& aLoc)
  : mEdge(aEdge), mLength(aLength), mEdgeloc(aEdgeloc), mLoc(aLoc)
{
}


/*****
Equality comparison. Two StubInfo records are considered to be the same if the
same set of active nodes are involved.
*****/  
bool tmStubInfo::operator==(const tmStubInfo& rhs) const
{
  if (mActiveNodes.size() != rhs.mActiveNodes.size()) return false;
  for (size_t i = 0; i < mActiveNodes.size(); ++i)
    if (mActiveNodes[i] != rhs.mActiveNodes[i]) return false;
  return true;
}

/*****
Inequality comparison. Two StubInfo records are considered to be different if
they are not equal.
*****/  
bool tmStubInfo::operator!=(const tmStubInfo& rhs) const
{
  return !(*this == rhs);
}

/*****
Ordering. Used in sorting. The longest stubs come earlier in sorting.
*****/
bool tmStubInfo::operator<(const tmStubInfo& rhs) const
{
  return mLength > rhs.mLength;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/**********
class tmStubFinder
Class that solves for stubs added to the tree that give 4 (or more) active paths.
**********/

/*****
Constructor -- requires initialization from a tmTree.
*****/
tmStubFinder::tmStubFinder(tmTree* aTree)
  : mTree(aTree), mScale(aTree->mScale), mParms(4, 4, 0.0)
{
}


/*****
Calculate the gradients and function values to be zeroed.
*****/
void tmStubFinder::UserFn(const vector<tmFloat>& x, 
  tmMatrix<tmFloat>& a, vector<tmFloat>& b)
{
  for (size_t i = 0; i < 4; ++i) {
    tmFloat temp = sqrt(pow(x[2] - mParms[i][0], 2) + 
      pow(x[3] - mParms[i][1], 2));
    b[i] = temp - mScale * (x[0] + mParms[i][3] * x[1] + mParms[i][2]);
    a[i][0] = -mScale;
    a[i][1] = -mScale * mParms[i][3];
    a[i][2] = (x[2] - mParms[i][0]) / temp;
    a[i][3] = (x[3] - mParms[i][1]) / temp;
  }
}


/*****
Find all possible stubs that form four (or more) active paths with nodes in the
list, such that all paths to other nodes in the tmTree are valid. This can be
used to break up a polygon into smaller ones or to add a node that pins a group
of nodes with the maximum number of active paths. Users must insure that there
are at least four leaf nodes in the list passed in. aNodeList can contain both
leaf and branch nodes but the branch nodes will be filtered out and if there
are not at least 4 leaf nodes we will return an empty list. Note that it's
possible to find no stubs even for valid input. Routines calling this should
take into account that the returned list could be empty.
*****/
void tmStubFinder::FindAllStubs(const tmArray<tmNode*>& aNodeList, 
  tmArray<tmStubInfo>& sInfoList)
{
  sInfoList.clear();
  tmArray<tmNode*> leafNodeList;
  tmNode::FilterLeafNodes(leafNodeList, aNodeList);
  size_t numNodes = leafNodeList.size();
  if (numNodes < 4) return;
  
  // make a list of all Edges in the minimal spanning subtree of the nodes in
  // question. These are the edges that are candidates for being split. 
  mTree->GetSpanningEdges(leafNodeList, mSpanningEdges);
  
  // Make a list of all leaf nodes which we'll use later in the validation
  // phase.
  mTree->GetLeafNodes(mLeafNodes);
  
  // Go through every possible combination of four nodes in the poly and edge
  // in the poly and look for a valid solution for a stub tmNode that makes
  // four active paths. 
  for (size_t i0 = 0; i0 < numNodes; i0++) {
    mTrialNodes[0] = leafNodeList[i0];
    for (size_t i1 = i0 + 1; i1 < numNodes; i1++) {
      mTrialNodes[1] = leafNodeList[i1];
      for (size_t i2 = i1 + 1; i2 < numNodes; i2++) {
        mTrialNodes[2] = leafNodeList[i2];
        for (size_t i3 = i2 + 1; i3 < numNodes; i3++) {
          mTrialNodes[3] = leafNodeList[i3];
          for (size_t i4 = 0; i4 < mSpanningEdges.size(); ++i4) {
            mTrialEdge = mSpanningEdges[i4];
            TestOneCombo(sInfoList);
          }
        }
      }
    }
  }
  // Sort the list in order of stub length
  sort(sInfoList.begin(), sInfoList.end());
}


/*****
Try a single combination of nodes (stored in mTrialNodes) and split edge
(mTrialEdge) and if the combination yields a solution, push it onto the list
sInfoList.
*****/
void tmStubFinder::TestOneCombo(tmArray<tmStubInfo>& sInfoList)
{
  tmNode* edgeFirstNode = mTrialEdge->mNodes.front();
  for (size_t i = 0; i < 4; ++i) {
    mParms[i][0] = mTrialNodes[i]->mLoc.x;
    mParms[i][1] = mTrialNodes[i]->mLoc.y;
    if (edgeFirstNode == mTrialNodes[i]) {
      mParms[i][2] = 0;
      mParms[i][3] = 1;
    }
    else {
      tmPath* aPath = mTree->FindAnyPath(edgeFirstNode, mTrialNodes[i]);
      mParms[i][2] = aPath->mMinTreeLength;
      mParms[i][3] = (aPath->mEdges.contains(mTrialEdge)) ? -1 : 1;
    }
  }
  
  // 11-20-96. If all four nodes lie on the same side of the edge,
  // the problem is ill-posed; so we don't need to go any
  // farther with its analysis.
  if ((mParms[0][3] == mParms[1][3]) && (mParms[0][3] == mParms[2][3]) &&
    (mParms[0][3] == mParms[3][3])) return;
    
  // set up initial conditions on search for a stub.
  vector<tmFloat> u(4);     // this will hold the trial solution
  u[0] = 0.1;           // initial stub length is 0.1
  u[1] = 0.5 * mTrialEdge->GetStrainedLength(); // initial split is halfway 
  tmPoint uu = 0.25 * (mTrialNodes[0]->mLoc + mTrialNodes[1]->mLoc +
    mTrialNodes[2]->mLoc + mTrialNodes[3]->mLoc);
  
  u[2] = uu.x;  // initial location is the average of the 4 nodes.
  u[3] = uu.y;
    
  // Find a solution. Since these are simple algebraic equations, I'll set
  // a fairly tight tolerance on the required solution.
  try {
    SolveEqns(10, u, 1.0e-8, 1.0e-8);
  }
  catch(EX_TOO_MANY_ITERATIONS) {
    // no solution, so go on to the next case
    return;
  }
  catch(EX_SINGULAR_MATRIX) {
    // no solution, so go on to the next case
    return;
  }
    
  // Now that we've got a numerical solution, we need to validate it.
  if (u[0] < 0) return;     // length of stub must be positive
  if (tmPart::IsTiny(u[0])) return;  // stub must have finite length
  if (u[1] < 0) return;     // stub must lie within the split edge
  if (u[1] > mTrialEdge->GetStrainedLength()) return; // ditto
  if ((u[2] < 0) || (u[2] > mTree->mPaperWidth)) return; // must be in square
  if ((u[3] < 0) || (u[3] > mTree->mPaperHeight)) return; // ditto
  tmStubInfo stubInfo(mTrialEdge, u[0], u[1], tmPoint(u[2], u[3]));
    
  // Now we gotta make sure it's feasible with paths to all the
  // other leaf nodes in the tree.
  tmArrayIterator<tmNode*> iLeafNodes(mLeafNodes);
  tmNode* testNode;
  while (iLeafNodes.Next(&testNode)) {
    // get the distance from ostensible new tmNode to testNode
    tmFloat actDist = Mag(testNode->mLoc - tmPoint(u[2], u[3]));
      
    // get minimum distance defined by tmPath constraints
    tmFloat minDist = u[0];
    if (testNode == edgeFirstNode) minDist += u[1];
    else {
      tmPath* aPath = mTree->FindAnyPath(edgeFirstNode, testNode);
      minDist += aPath->mMinTreeLength;
      if (aPath->mEdges.contains(mTrialEdge)) minDist -= u[1];
      else minDist += u[1];
    }
    minDist *= mScale;
    
    // compare distances; if actual distance is less than minimum,
    // the path is invalid and we'll have to reject this solution.
    if (!tmPath::TestIsFeasible(actDist, minDist)) return;
    
    // But if they're equal, it's an active path and we need to make
    // a note of it.
    if (tmPath::TestIsActive(actDist, minDist))
      stubInfo.mActiveNodes.push_back(testNode);
  }
  // Now that we've constructed all nodes that make active paths with the stub
  // (at least the 4 from our equation, but there could be more), we'll sort the
  // list of nodes lexicographically by address, then test for duplication.
  sort(stubInfo.mActiveNodes.begin(), stubInfo.mActiveNodes.end());
  if (sInfoList.contains(stubInfo)) return;  // we've already found this one
    
  // if we're still here, it's a valid distinct solution; add it to the list.
  sInfoList.push_back(stubInfo);
}


/*****
Find the largest stub circle that can be added to any edge spanning the given
nodes in such a way as to create four active Paths. As with FindAllStubs, the
passed-in list of nodes may contain branch nodes, which will be ignored. If no
stub was found, return a null tmStubInfo.
*****/
tmStubInfo tmStubFinder::FindLargestStub(const tmArray<tmNode*>& aNodeList)
{
  tmArray<tmStubInfo> sInfoList;
  FindAllStubs(aNodeList, sInfoList);
  tmStubInfo sbest;
  for (size_t i = 0; i < sInfoList.size(); ++i) {
    tmStubInfo sthis = sInfoList[i];
    if (sbest.mLength < sthis.mLength) sbest = sthis;
  } 
  return sbest;
}


/*****
Add a stub to the tree, at a location and size specified by stubInfo
*****/
void tmStubFinder::AddStubToTree(const tmStubInfo& stubInfo)
{
  TMASSERT(stubInfo.IsNotBlank());
  TMASSERT(stubInfo.mLength > 0);
  mTree->AddStub(stubInfo.mEdge, stubInfo.mLength, stubInfo.mEdgeloc, 
    stubInfo.mLoc);
}


/*****
Repeatedly add stubs to a tree until all polys have only three sides.
*****/
void tmStubFinder::TriangulateTree()
{
  // Repeatedly go through the polys looking for one of order-4 or higher; when
  // we find one, fracture it and start over at the beginning. When we've gone
  // through all the polys without finding one, we're done fracturing; at that
  // point, we'll build the polys again, but this time build subpolys and
  // contents. Note that it is possible for a tree to not be fully triangulable,
  // i.e., FindLargestStub can return a blank stubInfo.
  mTree->BuildTreePolys();
  bool keepSearching = true;
  while (keepSearching) {
    keepSearching = false;
    for (size_t i = 0; i < mTree->mOwnedPolys.size(); ++i ) {
      tmPoly* thePoly = mTree->mOwnedPolys[i];
      if (thePoly->GetSize() < 4) continue;
      tmStubInfo stubInfo = FindLargestStub(thePoly->mRingNodes);
      if (!stubInfo.IsBlank()) {
        AddStubToTree(stubInfo);
        mTree->BuildTreePolys();
        keepSearching = true;
        break;
      }
    }
  }
}

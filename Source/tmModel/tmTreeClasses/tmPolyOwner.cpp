/*******************************************************************************
File:         tmPolyOwner.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmPolyOwner
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmPolyOwner.h"
#include "tmModel.h"

using namespace std;

/*
Class tmPolyOwner keeps a list of Polys that it owns in a list; when this
object is deleted, it automatically deletes all the objects in the list.
Similarly, when a tmPoly is deleted, it tells its owner, which removes it from
the list.
*/

/**********
class tmPolyOwner
Base class for an object that owns polys and is responsible for their deletion.
Subclasses: tmPoly, tmTree
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


/*****
Return true if this tmPolyOwner or any of its sub-polys own the referenced
tmPoly. Provided for GUI to distinguish clicks inside inner versus outer polys.
*****/
bool tmPolyOwner::SuperOwns(tmPoly* aPoly) const
{
  tmArrayIterator<tmPoly*> iOwnedPolys(mOwnedPolys);
  tmPoly* aSubPoly;
  while (iOwnedPolys.Next(&aSubPoly)) {
    if (aSubPoly == aPoly) return true;
    if (aSubPoly->SuperOwns(aPoly)) return true;
  }
  return false;
}


#ifdef __MWERKS__
  #pragma mark --PROTECTED--
#endif


/*****
Destructor deletes all owned polys
*****/
tmPolyOwner::~tmPolyOwner()
{
  mOwnedPolys.KillItems();
}


#ifdef __MWERKS__
  #pragma mark --PRIVATE--
#endif


/*****
Return true if this path is eligible to start a new polygon in the forward
direction.
*****/
bool tmPolyOwner::CanStartPolyFwd(tmPath* aPath, const tmPoint& centroid)
{
  if (aPath->mFwdPoly != 0) return false;
  if (!aPath->IsBorderPath()) return true;
  return AreCCW(aPath->mNodes.front()->mLoc, 
    aPath->mNodes.back()->mLoc, centroid);
}  


/*****
Return true if this path is eligible to start a new polygon in the backward
direction.
*****/
bool tmPolyOwner::CanStartPolyBkd(tmPath* aPath, const tmPoint& centroid)
{
  if (aPath->mBkdPoly != 0) return false;
  if (!aPath->IsBorderPath()) return true;
  return AreCW(aPath->mNodes.front()->mLoc, 
    aPath->mNodes.back()->mLoc, centroid);
}  


/*****
Find the next polygon path emanating from thisNode after thisPath, going
counterclockwise. Also return the tmNode (nextNode) at the opposite end of the
path. 
*****/
void tmPolyOwner::GetNextPathAndNode(tmPath* thisPath, tmNode* thisNode,
  tmPath*& nextPath, tmNode*& nextNode)
{
  // Get the angle of thisPath.
  tmNode* thatNode = thisPath->mNodes.front();
  if (thatNode == thisNode) thatNode = thisPath->mNodes.back();
  tmFloat thisAngle = Angle(thatNode->mLoc - thisNode->mLoc);
  
  // delta is the increment in angle from thisPath to nextPath. Start with the
  // largest possible value. Then go through each of the Paths emanating from
  // thisNode and compare its angle to the angle of thisPath. We'll keep the
  // smallest positive angle we find.
  tmFloat delta = TWO_PI;
  nextPath = thisPath;
  nextNode = 0;
  for (size_t i = 0; i < thisNode->mLeafPaths.size(); ++i) {
    tmPath* thatPath = thisNode->mLeafPaths[i];
    if ((thatPath == thisPath) || (!thatPath->IsPolygonPath())) continue;
    
    // Get the angle of thatPath and the tmNode at the other end of thatPath
    // (thatNode).
    thatNode = thatPath->mNodes.front();
    if (thatNode == thisNode) thatNode = thatPath->mNodes.back();
    tmFloat nextAngle = Angle(thatNode->mLoc - thisNode->mLoc);
    
    // Find the angular increment to the new path. Constrain it to lie
    // between 0 and 2 PI.
    tmFloat newDelta = thisAngle - nextAngle;
    while (newDelta < 0) newDelta += TWO_PI;
    while (newDelta >= TWO_PI) newDelta -= TWO_PI;
    
    // If the angular increment is less than our current champion, we'll
    // replace the current values with those of the new tmPath. When we're
    // done, we'll be left with the tmPath that makes the smallest angle with
    // the current tmPath.
    if (newDelta < delta) {
      delta = newDelta;
      nextPath = thatPath;
      nextNode = thatNode;
    }
  }
  
  // Make sure we found something
  TMASSERT(nextPath != thisPath);
  TMASSERT(nextNode);
}


/*****
Starting with the list of tmPath*, some of which have mIsPolygonPath set to
true, construct a complete network of counterclockwise Polys. If the network is
well-formed, one of the Polys will be the border polygon, which runs clockwise;
kill it and set the paths that used to point to it to this if this is a tmPoly
and to the null ptr if it's a tmTree.
*****/
void tmPolyOwner::BuildPolysFromPaths(tmArray<tmPath*>& aPathList,
  tmArray<tmNode*>& borderNodes)
{
  // First thing we'll do is collect all the polygon paths from the list.
  // Although in theory, there cannot be any crossing polygon paths, in
  // practice, it's possible due to roundoff errors (for example, a sliver
  // rectangle with polygon paths along both diagonals). This situation is
  // rare, but if it happens, it screws our algorithm. Within slivers, we're
  // more concerned with consistency than precision, so we'll adopt the simple
  // expedient that as we build our list, we won't accept any polygon paths
  // that intersect ones that already exist.
  tmArray<tmPath*> polygonPaths;
  size_t numIntersections = 0;
  for (size_t i = 0; i < aPathList.size(); ++i) {
    tmPath* aPath = aPathList[i];
    if (aPath->IsPolygonPath()) {
      // Check for intersection against all preceding paths.
      for (size_t j = 0; j < polygonPaths.size(); ++j) {
        if (aPath->IntersectsInterior(polygonPaths[j])) {
          aPath->mIsPolygonPath = false;
          ++numIntersections;
          break;
        }
      }
      if (aPath->IsPolygonPath()) polygonPaths.push_back(aPath);
    };
  }
  
  // Construct the centroid of the border nodes, used in eligibility
  // calculation
  tmPoint centroid(0., 0.);
  for (size_t i = 0; i < borderNodes.size(); ++i)
    centroid += borderNodes[i]->mLoc;
  centroid /= borderNodes.size();

  // Now build new polygons. Each polygon path should point to exactly 2
  // polygons via the mFwdPoly and mBkdPoly members (polygon paths that are
  // also border paths point to only 1 poly). We build polygons by cycling
  // through polygon paths and starting a polygon if either mFwdPoly or
  // mBkdPoly are unused AND the the path is oriented in such a way as to
  // create a CCW polygon inside, rather than outside, the border.
  tmNode *firstNode, *thisNode, *nextNode;
  tmPath *thisPath, *nextPath;
  tmPoly* aPoly;
  
  for (size_t i = 0; i < polygonPaths.size(); ++i) {
    tmPath* aPath = polygonPaths[i];
    if (CanStartPolyFwd(aPath, centroid)) {
      // build up a poly in the forward direction
      aPath->mFwdPoly = aPoly = new tmPoly(aPath->mTree, this);
      firstNode = aPath->mNodes.front();
      thisPath = aPath;
      thisNode = aPath->mNodes.back();
      aPoly->mRingNodes.push_back(firstNode);
      aPoly->mRingPaths.push_back(thisPath);
      size_t tooMany = 0;
      do {
        GetNextPathAndNode(thisPath, thisNode, nextPath, nextNode);
        aPoly->mRingNodes.push_back(thisNode);
        aPoly->mRingPaths.push_back(nextPath);
        if (nextPath->mNodes.front() == thisNode) nextPath->mFwdPoly = aPoly;
        else nextPath->mBkdPoly = aPoly;
        thisPath = nextPath;
        thisNode = nextNode;
        tooMany++;
        TMASSERT(tooMany < 100);  // Only for debugging, to avoid infinite loops
      } while (nextNode != firstNode);
      aPoly->CalcContents();
    }
    if (CanStartPolyBkd(aPath, centroid)) {
      // build up a poly in the backward direction
      aPath->mBkdPoly = aPoly = new tmPoly(aPath->mTree, this);
      firstNode = aPath->mNodes.back();
      thisPath = aPath;
      thisNode = aPath->mNodes.front();
      aPoly->mRingNodes.push_back(firstNode);
      aPoly->mRingPaths.push_back(thisPath);
      size_t tooMany = 0;
      do {
        GetNextPathAndNode(thisPath, thisNode, nextPath, nextNode);
        aPoly->mRingNodes.push_back(thisNode);
        aPoly->mRingPaths.push_back(nextPath);
        if (nextPath->mNodes.front() == thisNode) nextPath->mFwdPoly = aPoly;
        else nextPath->mBkdPoly = aPoly;
        thisPath = nextPath;
        thisNode = nextNode;
        tooMany++;
        TMASSERT(tooMany < 100);
        
      } while (nextNode != firstNode);
      aPoly->CalcContents();
    }
  }
  
  // The last thing we do is to make sure each new poly contains a complete
  // list of all paths that criss-cross it.
  for (size_t i = 0; i < mOwnedPolys.size(); ++i) {
    tmPoly* aPoly = mOwnedPolys[i];
    if (aPoly->mCrossPaths.empty()) aPoly->CalcCrossPaths();
  }
}

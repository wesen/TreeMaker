/*******************************************************************************
File:         tmPath.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmPath
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-25
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmPath.h"
#include "tmModel.h"

#ifdef TMDEBUG
  #include <fstream>
#endif

using namespace std;

/**********
class tmPath
This class represents a path from one node to another along the tree, but a
subset of the leaf paths also serves as the basis of the crease pattern.
**********/

/*****
Common initialization
*****/
void tmPath::InitPath()
{
  // Register with tmTree
  mTree->mPaths.push_back(this);
  mIndex = mTree->mPaths.size();
  
  // Initialize member vbls to default values.
  mMinTreeLength = 0;
  mMinPaperLength = 0;
  mActTreeLength = 0;
  mActPaperLength = 0;
  
  mIsLeafPath = false;
  mIsFeasiblePath = false;
  mIsActivePath = false;
  mIsBorderPath = false;
  mIsPolygonPath = false;
  mIsConditionedPath = false;
  mIsSubPath = false;
  
  mOutsetPath = 0;
  mFrontReduction = 0;
  mBackReduction = 0;
  mMinDepth = DEPTH_NOT_SET;
  mMinDepthDist = DEPTH_NOT_SET;
}

/*****
Constructor for bare tmPath.
NOTE: Ownership and settings are not set in this constructor.
*****/
tmPath::tmPath(tmTree* aTree) : 
  tmPart(aTree), tmDpptrTarget()
{
  // Common initialization
  InitPath();
}


/*****
Constructor for tmPath cloned from an tmEdge
*****/
tmPath::tmPath(tmTree* aTree, tmEdge* aEdge) : 
  tmPart(aTree), tmDpptrTarget()
{
  // Common initialization
  InitPath();
  
  // Register with Owner
  mPathOwner = aTree;
  mPathOwner->mOwnedPaths.push_back(this);

  // Set settings
  mMinTreeLength = aEdge->mLength;

  // Set references
  mEdges.push_back(aEdge);
  mNodes.push_back(aEdge->mNodes.front());
  mNodes.push_back(aEdge->mNodes.back());
}


/*****
Constructor for tmPath cloned from a tmPath
*****/
tmPath::tmPath(tmTree* aTree, tmPath* aPath) : 
  tmPart(aTree), tmDpptrTarget()
{
  // Common initialization
  InitPath();
  
  // Register with Owner
  mPathOwner = aTree;
  mPathOwner->mOwnedPaths.push_back(this);
    
  // Set settings
  mMinTreeLength = aPath->mMinTreeLength;
  mMinPaperLength = aPath->mMinPaperLength;

  // Set flags
  mIsLeafPath = aPath->mIsLeafPath;
  mIsFeasiblePath = aPath->mIsFeasiblePath;
  mIsActivePath = aPath->mIsActivePath;
  mIsBorderPath = aPath->mIsBorderPath;
  mIsPolygonPath = aPath->mIsPolygonPath;
  mIsConditionedPath = aPath->mIsConditionedPath;
  mIsSubPath = aPath->mIsSubPath;
  
  // Set references
  mNodes.merge_with(aPath->mNodes);
  mEdges.merge_with(aPath->mEdges);
}


/*****
Constructor for tmPath owned by a tmPoly. This is a subpath, e.g., a
circumferential or crossing path within an inset molecule.
*****/
tmPath::tmPath(tmPoly* aPoly, tmNode* aNode1, tmNode* aNode2)  : 
  tmPart(aPoly->mTree), tmDpptrTarget()
{
  // Common initialization
  InitPath();
  
  // Register with Owner
  mPathOwner = aPoly;
  mPathOwner->mOwnedPaths.push_back(this);
    
  // Set flags
  mIsSubPath = true;
  
  // Set references
  mNodes.push_back(aNode1);
  mNodes.push_back(aNode2);
}


/*****
Constructor for tmPath owned by a tmTree. Used for spoke and final ridge paths.
*****/
tmPath::tmPath(tmTree* aTree, tmNode* aNode1, tmNode* aNode2)
  : tmPart(aTree), tmDpptrTarget()
{
  // Common initialization
  InitPath();
  
  // Register with Owner
  mPathOwner = aTree;
  mPathOwner->mOwnedPaths.push_back(this);
    
  // Set flags
  mIsSubPath = true;
  
  // Set references
  mNodes.push_back(aNode1);
  mNodes.push_back(aNode2);
}


/*****
Destructor for tmPath - when a tmPath is destroyed, it kills any polygons that
were attached to it. We rely on this behavior in many places.
*****/
tmPath::~tmPath()
{
  if (mFwdPoly != 0) delete (tmPoly*) mFwdPoly;
  if (mBkdPoly != 0) delete (tmPoly*) mBkdPoly;
}


/*****
Return the vector (in the paper) corresponding to this path. This is usually
only meaningful for leaf paths.
*****/
const tmPoint tmPath::GetVector() const
{
  return mNodes.back()->mLoc - mNodes.front()->mLoc;
}


/*****
Return the angle of this path in degrees. The value lies within [-180, 180].
*****/
const tmFloat tmPath::GetAngle() const
{
  return RADIAN * Angle(mNodes.back()->mLoc - mNodes.front()->mLoc);
}


/*****
Return the angle of this path, with direction defined so it is within [0, 180).
*****/
const tmFloat tmPath::GetPositiveAngle() const
{
  tmFloat theAngle = GetAngle();
  if (theAngle >= 0) return theAngle;
  return theAngle + 180.;
}


/*****
Return the maximally outset path from this one, i.e., the original leaf path
from which this path may have been inset. Also compute the accumulated front
and back reduction from all of the insettings.
*****/
void tmPath::GetMaxOutsetPath(tmPath*& maxOutsetPath, 
  tmFloat& maxFrontReduction, tmFloat& maxBackReduction)
{
  if (!mOutsetPath) {
    maxOutsetPath = this;
    maxFrontReduction = 0;
    maxBackReduction = 0;
  }
  else {
    mOutsetPath->GetMaxOutsetPath(maxOutsetPath, 
      maxFrontReduction, maxBackReduction);
    maxFrontReduction += mFrontReduction;
    maxBackReduction += mBackReduction;
  }
}


/*****
Return the node at the other end of this path from aNode, which MUST be at one
end or the other or an assertion will fail in debug builds.
*****/
tmNode* tmPath::GetOtherNode(const tmNode* aNode) const
{
  if (aNode == mNodes.front()) return mNodes.back();
  else {
    TMASSERT(aNode == mNodes.back());
    return mNodes.front();
  }
}


/*****
STATIC
Copy all leaf paths from srcList into dstList.
*****/
void tmPath::FilterLeafPaths(tmArray<tmPath*>& dstList, 
  const tmArray<tmPath*>& srcList)
{
  dstList.clear();
  for (size_t i = 0; i < srcList.size(); ++i)
    if (srcList[i]->IsLeafPath()) dstList.push_back(srcList[i]);
}


/*****
Calculate all of the length-like member variables for this tree path. Also set
whether the path is valid and/or active based on the calculated lengths.
*****/
void tmPath::TreePathCalcLengths()
{
  // Only call this for tree paths.
  TMASSERT(IsTreePath());
  
  // compute the minimum length of each path based on the lengths of its edges 
  // and any strain that is present
  mMinTreeLength = 0.0;
  for (size_t i = 0; i < mEdges.size(); ++i)
    mMinTreeLength += mEdges[i]->GetStrainedLength();
  mMinPaperLength = mMinTreeLength * mTree->mScale;
  
  // compute the actual length of the path, based on the coordinates of its
  // nodes. this is only meaningful for leaf paths; for non-leaf paths we set
  // the length to an innocuous value, i.e., zero.
  if (mIsLeafPath && mNodes.size() > 0)
    mActPaperLength = Mag(mNodes.front()->mLoc - mNodes.back()->mLoc);
  else
    mActPaperLength = 0;
  mActTreeLength = mActPaperLength / mTree->mScale;
  
  // Also, calculate feasibility and activity since we now have this
  // information; again, only meaningful for leaf paths.
  if (mIsLeafPath) {
    mIsFeasiblePath = TestIsFeasible(mActPaperLength, mMinPaperLength);
    mIsActivePath = TestIsActive(mActPaperLength, mMinPaperLength);
  }
  else {
    mIsFeasiblePath = false;
    mIsActivePath = false;
  }
}


/*****
STATIC
This routine provides a uniform test whether a path should be considered
feasible. i.e., whether its actual length on the paper is greater than or equal
to its minimum length, to within a specified tolerance. The two lengths should
be scaled lengths (i.e., distances on the paper).
*****/
bool tmPath::TestIsFeasible(const tmFloat& actLen, const tmFloat& minLen)
{
  return actLen >= minLen - DistTol();
}


/*****
STATIC
This routine provides a uniform test whether a path should be considered
active. i.e., whether its actual length is equal to its minimum length, to
within a specified tolerance. The two lengths should be scaled lengths (i.e.,
distances on the paper).
*****/
bool tmPath::TestIsActive(const tmFloat& actLen, const tmFloat& minLen)
{
  return IsTiny(actLen - minLen);
}


/*****
Return true if this path starts or ends on the given node.
*****/
bool tmPath::StartsOrEndsWith(tmNode* aNode) const
{
  if (mNodes.front() == aNode) return true;
  if (mNodes.back() == aNode) return true;
  return false;
}


/*****
Return true if the given path starts or ends with the same node that this path
starts or ends with.
*****/
bool tmPath::SharesEndNodeWith(tmPath* aPath) const
{
  if (StartsOrEndsWith(aPath->mNodes.front())) return true;
  if (StartsOrEndsWith(aPath->mNodes.back())) return true;
  return false;
}


/*****
Return true if these two paths intersect in their interior, but don't share an
endpoint.
*****/
bool tmPath::IntersectsInterior(tmPath* aPath) const
{
  if (SharesEndNodeWith(aPath)) return false;
  const tmPoint& p = mNodes.front()->mLoc;
  const tmPoint rp = mNodes.back()->mLoc - p;
  const tmPoint& q = aPath->mNodes.front()->mLoc;
  const tmPoint rq = aPath->mNodes.back()->mLoc - q;
  tmFloat tp, tq;
  if (!GetLineIntersectionParms(p, rp, q, rq, tp, tq)) return false;
  if (tp <= 0 || tp >= 1 || tq <= 0 || tq >= 1) return false;
  return true;
}


/*****
Return true if this path is incident to a poly that has been filled.
*****/
bool tmPath::IsIncidentToFilledPoly() const
{
  if (mFwdPoly && mFwdPoly->HasPolyContents()) return true;
  if (mBkdPoly && mBkdPoly->HasPolyContents()) return true;
  return false;
}


/*****
Return an existing vertex or create a new one along the path. If the point lies
within tolerance of an endpoint of the path, create (or return) a vertex owned
by the node at the end of the path.
*****/
tmVertex* tmPath::GetOrMakeVertex(const tmPoint& p, tmNode* aTreeNode)
{
  tmVertex* theVertex = 0;
  
  tmNode* frontNode = mNodes.front();
  tmNode* backNode = mNodes.back();
  // Check front end of path
  if (tmVertex::VerticesSameLoc(p, frontNode->mLoc))
    theVertex = frontNode->GetOrMakeVertexSelf();
  else
    // Check back end of path
    if (tmVertex::VerticesSameLoc(p, backNode->mLoc))
      theVertex = backNode->GetOrMakeVertexSelf();
  else
    // Check for existing vertex owned by the path (i.e., a vertex somewhere along
    // the path).
    for (size_t i = 0; i < mOwnedVertices.size(); ++i) {
      tmVertex* testVertex = mOwnedVertices[i];
      if (tmVertex::VerticesSameLoc(p, testVertex->mLoc)) {
        theVertex = testVertex;
        break;
      }
    }
  // And if we didn't find one, we'll have to make a new one.
  if (!theVertex) theVertex = MakeVertex(p, aTreeNode);
  
  // If the vertex didn't have a tree node and we passed one in, give it the
  // passed-in tree node.
  if (!theVertex->mTreeNode && aTreeNode) theVertex->mTreeNode = aTreeNode;
  
  return theVertex;
}


/*****
Create a new vertex along the path. Note that we should complete all path-owned
vertex creation before we create any creases owned by the path.
*****/
tmVertex* tmPath::MakeVertex(const tmPoint& p, tmNode* aTreeNode)
{
  // Create the new vertex and insert it into the list at the appropriate spot.
  tmPoint& p1 = mNodes.front()->mLoc;
  tmPoint& p2 = mNodes.back()->mLoc;
  tmFloat dist_p = Mag(p - p1);
  tmFloat x = dist_p / Mag(p2 - p1);
  tmFloat elevation = (1 - x) * mNodes.front()->mElevation + x * mNodes.back()->mElevation;
  tmVertex* theVertex;
  theVertex = new tmVertex(mTree, this, p, elevation, mIsBorderPath, aTreeNode);
  for (size_t i = 0; i < mOwnedVertices.size() - 1; ++i) {
    tmVertex* testVertex = mOwnedVertices[i];
    tmFloat dist_t = Mag(testVertex->mLoc - p1);
    if (dist_p < dist_t) {
      mOwnedVertices.MoveItem(mOwnedVertices.size(), (i + 1));
      break;
    }
  }
  
  // TBD: Maybe no longer need to do following crease check, 
  // since we now create all vertices first.
  // For debugging, if we needed it, write the result to the log window.

  // If this path owns any creases that overlap the vertex, split the 
  // crease into two new ones.
  for (size_t i = 0; i < mOwnedCreases.size(); ++i) {
    tmCrease* theCrease = mOwnedCreases[i];
    tmVertex* frontVertex = theCrease->mVertices.front();
    tmVertex* backVertex = theCrease->mVertices.back();
    // For each crease we'll check to see if the new vertex falls in the
    // interior of the crease. If it does, then we split the crease, creating
    // new ones and deleting the old ones.
    const tmPoint& pc1 = frontVertex->mLoc;
    const tmPoint& pc2 = backVertex->mLoc;
    const tmPoint pc21 = pc2 - pc1;
    tmFloat x = Inner(p - pc1, pc21) / Mag2(pc21);
    if (x > 0 && x < 1) {
      TMLOG("tmPath::MakeVertex(..) -- crease split during vertex creation");
      new tmCrease(mTree, this, frontVertex, theVertex, theCrease->mKind);
      new tmCrease(mTree, this, theVertex, backVertex, theCrease->mKind);
      delete theCrease;
      return theVertex;
    }
  }
  return theVertex;
}


/*****
Return front vertex in the path, i.e., the vertex corresponding to the front
node. Note: this is NOT the front vertex in mOwnedVertices. Return 0 if the
front node doesn't have a vertex yet.
*****/
tmVertex* tmPath::GetFrontVertex() const
{
  return mNodes.front()->GetVertex();
}


/*****
Return back vertex in the path, i.e., the vertex corresponding to the back
node. Note: this is NOT the back vertex in mOwnedVertices. Return 0 if the node
doesn't have a vertex yet.
*****/
tmVertex* tmPath::GetBackVertex() const
{
  return mNodes.back()->GetVertex();
}


/*****
Build the vertices along this path. Inactive paths only build their endpoints.
Active paths (active axial and gusset creases) get vertices corresponding to
every branch node along the path.
*****/
void tmPath::BuildSelfVertices()
{
  // This should only be called for active paths (axial or gusset) or
  // inactive axial paths. Technically, you can also safely call it for active
  // inset ring paths, but there's no reason to do so.
  TMASSERT(IsActivePath() || IsAxialPath());

  // Create the vertices at the ends of the path.
  tmVertex* frontVertex = mNodes.front()->GetOrMakeVertexSelf();
  tmVertex* backVertex = mNodes.back()->GetOrMakeVertexSelf();

  // If intermediate already exist, we don't need to do it again.
  if (mOwnedVertices.not_empty()) return;
  
  // If it's an active path (axial or gusset), create vertices along the path
  // corresponding to each of the branch nodes of the tree encountered along
  // the path.
 if (mIsActivePath) {
    tmPoint q1 = frontVertex->mLoc;
    tmPoint q2 = backVertex->mLoc;
    tmPoint qu = q2 - q1;
    qu /= mActPaperLength;
    tmPath* maxOutsetPath;
    tmFloat maxFrontReduction, maxBackReduction;
    GetMaxOutsetPath(maxOutsetPath, maxFrontReduction, maxBackReduction);
    tmFloat curPos = -maxFrontReduction;
    // Step through the nodes and edges of the path. Only create a vertex if
    // the position falls within the path.
    TMASSERT(maxOutsetPath->mEdges.not_empty());
    for (size_t i = 0; i < maxOutsetPath->mEdges.size(); ++i) {
      tmNode* curNode = maxOutsetPath->mNodes[i + 1];
      curPos += maxOutsetPath->mEdges[i]->GetStrainedScaledLength();
      if (curPos <= 0.0) continue;
      if (curPos >= mActPaperLength) break;
      GetOrMakeVertex(q1 + qu * curPos, curNode);
    }
  }
}


/*****
Connect the vertices at the ends of the path and the vertices owned by the path
to create all the creases along the path. All vertices, including those at the
endpoints, must have been constructed.
*****/
void tmPath::ConnectSelfVertices(tmCrease::Kind aKind)
{
  tmVertex* frontVertex = GetFrontVertex();
  TMASSERT(frontVertex);
  for (size_t i = 0; i < mOwnedVertices.size(); ++i) {
    tmVertex* backVertex = mOwnedVertices[i];
    GetOrMakeCrease(frontVertex, backVertex, aKind);
    frontVertex = backVertex;
  }
  tmVertex* backVertex = GetBackVertex();
  TMASSERT(backVertex);
  GetOrMakeCrease(frontVertex, backVertex, aKind);
}


/*****
Set the depth of the given vertex relative to the coordinate system established
for this path.
*****/
void tmPath::SetVertexDepth(tmVertex* aVertex) const
{
  tmPoint p = aVertex->mLoc;
  tmPoint p1 = mNodes.front()->mLoc;
  tmPoint p2 = mNodes.back()->mLoc;
  tmFloat d = Inner(p - p1, p2 - p1) / Mag(p2 - p1);
  if (d < mMinDepthDist)
    aVertex->mDepth = mMinDepth + mMinDepthDist - d;
  else
    aVertex->mDepth = mMinDepth + d - mMinDepthDist;
}


/*****
Return the first crease owned by this path that has aVertex as one of its
vertices. Return 0 if none was found.
*****/
tmCrease* tmPath::GetFirstCrease(tmVertex* aVertex) const
{
  for (size_t i = 0; i < mOwnedCreases.size(); ++i) {
    tmCrease* theCrease = mOwnedCreases[i];
    if (theCrease->mVertices.front() == aVertex ||
      theCrease->mVertices.back() == aVertex) return theCrease;
  }
  return 0;
}


/*****
Return the crease owned by this path that is incident on aVertex and is not
aCrease; we can use this to navigate along the chain of creases owned by this
path. That is, we assume the connection runs: aCrease - aVertex - nextCrease -
nextVertex. Return 0 for both when we get to the end of the chain.
*****/
void tmPath::GetNextCreaseAndVertex(tmCrease* aCrease, tmVertex* aVertex,
  tmCrease*& nextCrease, tmVertex*& nextVertex) const
{
  for (size_t i = 0; i < mOwnedCreases.size(); ++i) {
    tmCrease* theCrease = mOwnedCreases[i];
    if (theCrease == aCrease) continue;
    if (theCrease->mVertices.front() == aVertex) {
      nextCrease = theCrease;
      nextVertex = theCrease->mVertices.back();
      return;
    }
    if (theCrease->mVertices.back() == aVertex) {
      nextCrease = theCrease;
      nextVertex = theCrease->mVertices.front();
      return;
    }
  }
  nextCrease = 0;
  nextVertex = 0;
}  


/*****
Put a tmPath in version 5 format.
*****/
void tmPath::Putv5Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex);  
  PutPOD(os, mMinTreeLength);
  PutPOD(os, mMinPaperLength);  
  PutPOD(os, mActTreeLength);
  PutPOD(os, mActPaperLength);  
  PutPOD(os, mIsLeafPath);
  PutPOD(os, mIsSubPath);
  PutPOD(os, mIsFeasiblePath);
  PutPOD(os, mIsActivePath);
  PutPOD(os, mIsBorderPath);
  PutPOD(os, mIsPolygonPath);
  PutPOD(os, mIsConditionedPath);
  mTree->PutPtr(os, mFwdPoly);
  mTree->PutPtr(os, mBkdPoly);
  mTree->PutPtrArray(os, mNodes);
  mTree->PutPtrArray(os, mEdges);
  mTree->PutPtr(os, mOutsetPath);
  PutPOD(os, mFrontReduction);
  PutPOD(os, mBackReduction);
  PutPOD(os, mMinDepth);
  PutPOD(os, mMinDepthDist);
  mTree->PutPtrArray(os, mOwnedVertices);
  mTree->PutPtrArray(os, mOwnedCreases);
  mTree->PutOwnerPtr(os, mPathOwner);
}


/*****
Get a tmPath in version 5 format.
*****/
void tmPath::Getv5Self(istream& is)
{
  CheckTagStr<tmPath>(is);  
  GetPOD(is, mIndex);  
  GetPOD(is, mMinTreeLength);
  GetPOD(is, mMinPaperLength);  
  GetPOD(is, mActTreeLength);
  GetPOD(is, mActPaperLength);  
  GetPOD(is, mIsLeafPath);
  GetPOD(is, mIsSubPath);
  GetPOD(is, mIsFeasiblePath);
  GetPOD(is, mIsActivePath);
  GetPOD(is, mIsBorderPath);
  GetPOD(is, mIsPolygonPath);
  GetPOD(is, mIsConditionedPath);  
  mTree->GetPtr(is, mFwdPoly, true);
  mTree->GetPtr(is, mBkdPoly, true);  
  mTree->GetPtrArray(is, mNodes);
  mTree->GetPtrArray(is, mEdges);
  mTree->GetPtr(is, mOutsetPath, true);
  GetPOD(is, mFrontReduction);
  GetPOD(is, mBackReduction);
  GetPOD(is, mMinDepth);
  GetPOD(is, mMinDepthDist);
  mTree->GetPtrArray(is, mOwnedVertices);  
  mTree->GetPtrArray(is, mOwnedCreases);  
  mTree->GetOwnerPtr(is, mPathOwner);
}


/*****
Put a tmPath in version 4 format. Note that we do not put any
polys, vertices, or creases.
*****/
void tmPath::Putv4Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex);  
  PutPOD(os, mMinTreeLength);
  PutPOD(os, mMinPaperLength);  
  PutPOD(os, mIsLeafPath);
  PutPOD(os, mIsSubPath);
  PutPOD(os, mIsActivePath);
  PutPOD(os, mIsBorderPath);
  PutPOD(os, mIsPolygonPath);
  PutPOD(os, mIsConditionedPath);
  PutPOD(os, 0);  // no mOwnedVertices
  PutPOD(os, 0);  // no mFwdPoly
  PutPOD(os, 0);  // no mBkdPoly
  mTree->PutPtrArray(os, mNodes);
  mTree->PutPtrArray(os, mEdges);
  mTree->PutOwnerPtr(os, mPathOwner);
}


/*****
Get a tmPath in version 4 format.
*****/
void tmPath::Getv4Self(istream& is)
{
  CheckTagStr<tmPath>(is);  
  GetPOD(is, mIndex);  
  GetPOD(is, mMinTreeLength);
  GetPOD(is, mMinPaperLength);  
  GetPOD(is, mIsLeafPath);
  GetPOD(is, mIsSubPath);
  GetPOD(is, mIsActivePath);
  GetPOD(is, mIsBorderPath);
  GetPOD(is, mIsPolygonPath);
  GetPOD(is, mIsConditionedPath);  
  mTree->GetPtrArray(is, mOwnedVertices);  
  mTree->GetPtr(is, mFwdPoly, true);
  mTree->GetPtr(is, mBkdPoly, true);  
  mTree->GetPtrArray(is, mNodes);
  mTree->GetPtrArray(is, mEdges);  
  mTree->GetOwnerPtr(is, mPathOwner);
}


/*****
Get a path in version 3 format. Getv3Self() creates the appropriate
conditions for the constraints that were previously part of the path.
*****/
void tmPath::Getv3Self(istream& is)
{
  CheckTagStr<tmPath>(is);
  GetPOD(is, mIndex);
  GetPOD(is, mMinTreeLength);

  // Fixed path length: if it's the same as mMinTreeLength, then it's really
  // an active path constraint. However, we can't create the constraint
  // until we've read in all of the nodes, so we'll defer this until the
  // end of this routine.
  bool pathFixedLength, pathFixedAngle;
  tmFloat pathFixedLengthValue, pathFixedAngleValue;
  GetPOD(is, pathFixedLength);    // GetPOD(mFixedLength);
  GetPOD(is, pathFixedLengthValue);  // GetPOD(mFixedLengthValue);
  GetPOD(is, pathFixedAngle);    // GetPOD(mFixedAngle);
  GetPOD(is, pathFixedAngleValue);  // GetPOD(mFixedAngleValue);
  
  // Topological flags
  GetPOD(is, mIsLeafPath);
  mIsSubPath = false;
  
  // Dimensional flags
  GetPOD(is, mIsActivePath);
  GetPOD(is, mIsBorderPath);
  GetPOD(is, mIsPolygonPath);  
  mIsConditionedPath = false;
  
  // No tmPoly information for the tmPath, so we ignore what was stored.
  int idum;
  GetPOD(is, idum);          // GetPtr(mFwdPoly, true);
  GetPOD(is, idum);          // GetPtr(mBkdPoly, true);
  mFwdPoly = 0;
  mBkdPoly = 0;
  
  mTree->GetPtrArray(is, mNodes);
  mTree->GetPtrArray(is, mEdges);
  
  // Now create the tmCondition(s), if needed. 
  if (pathFixedLength && (mMinTreeLength == pathFixedLengthValue))
    new tmConditionPathActive(mTree, this);
  
  if (pathFixedAngle) {
    tmConditionPathAngleFixed* c = new tmConditionPathAngleFixed(mTree);
    c->mNode1 = mNodes.front();
    c->mNode2 = mNodes.back();
    c->mAngle = pathFixedAngle;
  }
  
  // Calculate new fields of the tmPath  
  TreePathCalcLengths();
  
  // Set ownership  
  mTree->mOwnedPaths.push_back(this);
  mPathOwner = mTree;
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmPath, "path")

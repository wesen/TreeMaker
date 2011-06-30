/*******************************************************************************
File:         tmPoly.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmPoly
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-25
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmPoly.h"
#include "tmModel.h"

#ifdef TMDEBUG
  #include <fstream>
#endif

using namespace std;

/*
Class tmPoly is used in two related ways: top-level polygons, which are owned
by the tmTree, are the active polygons of the tree network. In addition,
subpolygons are also represented by tmPoly objects, which are owned by other
Polys.

Note: the RingNodes and RingPaths of a tmPoly are owned by the same tmPolyOwner
that owns the tmPoly. This tmPoly is a tmPolyOwner (as well as a tmNodeOwner
and tmPathOwner), but it owns the RingNodes and RingPaths of its subPolys.
*/

/**********
class tmPoly
Class that represents a polygon in the crease pattern, which gets filled with
molecule crease patterns.
**********/

/*****
Common initialization
*****/
void tmPoly::InitPoly()
{
  // Register with tmTree
  mTree->mPolys.push_back(this);
  mIndex = mTree->mPolys.size();
  
  // Initialize member vbls
  mCentroid = tmPoint(0., 0.);
  
  // Initialize flags
  mIsSubPoly = false;
  
  // Misc ptrs
  mRidgePath = 0;
  
  // owner
  mPolyOwner = 0;
}


/*****
Constructor for a bare tmPoly.
NOTE: ownership and settings are not set by this constructor.
*****/
tmPoly::tmPoly(tmTree* aTree) : 
  tmPart(aTree), tmDpptrTarget()
{
  InitPoly();
}


/*****
Constructor for a full tmPoly
*****/
tmPoly::tmPoly(tmTree* aTree, tmPolyOwner* aPolyOwner) : 
  tmPart(aTree), tmDpptrTarget()
{
  InitPoly();
  
  // Register with Owner
  mPolyOwner = aPolyOwner;
  aPolyOwner->mOwnedPolys.push_back(this);

  // Set flag
  mIsSubPoly = GetOwnerAsPoly();
}


/*****
Destructor. Only needed so we can break on tmPoly deletion during debugging
*****/
tmPoly::~tmPoly()
{
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return true if the point lies in the interior of this polygon. Note: the
algorithm is only guaranteed for convex polygons, so that's all we use it for.
It works by checking if the point lies on the same side of each edge as the
centroid.
*****/
bool tmPoly::ConvexEncloses(tmPoint p) const
{
#ifdef TMDEBUG
  TMASSERT(CalcPolyIsConvex());
#endif

  // check to see if the point lies on the same side of each path as the
  // centroid.
  size_t nmax = mRingPaths.size();
  for (size_t i = 0; i < nmax; ++i) {
    tmPath* aPath = mRingPaths[i];
    tmPoint p1 = aPath->mNodes.front()->mLoc;
    tmPoint p2 = aPath->mNodes.back()->mLoc;
    tmPoint q = RotateCCW90(p2 - p1);
    if (Inner(mCentroid - p1, q) < 0) q *= -1;
    if (Inner(p - p1, q) < 0) {
      return false;
    }
  }
  return true;  
}


/*****
Return the number of nodes (corners) of this polygon.
*****/
size_t tmPoly::GetSize() const
{
  TMASSERT(mRingNodes.size() == mRingPaths.size());
  return mRingNodes.size();
}


/*****
Return the nodes and edges in the subtree spanned by this poly.
*****/
void tmPoly::GetSubTree(tmArray<tmNode*>& nlist, tmArray<tmEdge*>& elist) const
{
  nlist.clear();
  elist.clear();
  for (size_t i = 0; i < mRingPaths.size(); ++i) {
    tmPath* thePath = mRingPaths[i];
    for (size_t j = 0; j < thePath->mEdges.size(); ++j) {
      tmEdge* theEdge = thePath->mEdges[j];
      if (!elist.contains(theEdge)) {
        elist.push_back(theEdge);
        nlist.union_with(theEdge->mNodes.front());
        nlist.union_with(theEdge->mNodes.back());
      }
    }
  }
}


/*****
Return true if this poly contains the given node, which must be a tree node.
*****/
bool tmPoly::SubTreeContains(tmNode* aNode) const
{
  TMASSERT(aNode->IsTreeNode());
  for (size_t i = 0; i < mRingPaths.size(); ++i) {
    tmPath* thePath = mRingPaths[i];
    if (thePath->mNodes.contains(aNode)) return true;
  }
  return false;
}


/*****
Return true if this poly contains the given edge.
*****/
bool tmPoly::SubTreeContains(tmEdge* aEdge) const
{
  for (size_t i = 0; i < mRingPaths.size(); ++i) {
    tmPath* thePath = mRingPaths[i];
    if (thePath->mEdges.contains(aEdge)) return true;
  }
  return false;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the owner as a tmTree*. Return 0 if the owner is something else (tmPoly).
*****/
tmTree* tmPoly::GetOwnerAsTree()
{
  return mPolyOwner->PolyOwnerAsTree();
}


/*****
Return the owner as a tmPoly*. Return 0 if the owner is something else (tmTree).
*****/
tmPoly* tmPoly::GetOwnerAsPoly()
{
  return mPolyOwner->PolyOwnerAsPoly();
}


/*****
Return a ptr to the top-level poly that this poly is a part of. If this is a
top-level poly, return ptr to self.
*****/
tmPoly* tmPoly::GetOutermostPoly()
{
  tmPoly* ownerPoly = GetOwnerAsPoly();
  if (ownerPoly) return ownerPoly->GetOutermostPoly();
  else return this;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Dump the current state of this poly to a file in text format. Used in debugging.
*****/
#ifdef TMDEBUG
void tmPoly::DumpToFileAsText(const char* filename)
{
  ofstream fout(filename);
  fout << "Ring nodes\n";
  // ring nodes
  for (size_t i = 0; i < mRingNodes.size(); ++i) {
    tmNode* theNode = mRingNodes[i];
    fout << "node " << theNode->GetIndex() << " (" << theNode->GetLocX() <<
      ", " << theNode->GetLocY() << ")\n";
  };
  // inset nodes
  fout << "Inset nodes\n";
  for (size_t i = 0; i < mOwnedNodes.size(); ++i) {
    tmNode* theNode = mOwnedNodes[i];
    fout << "node " << theNode->GetIndex() << " (" << theNode->GetLocX() <<
      ", " << theNode->GetLocY() << ")\n";
  };
  fout.close();
}
#endif


/*****
Dump the current state of this poly to a file in Mathematica format. Used in
debugging. Mathematica format lets us see the geometric shape of the thing.
*****/
#ifdef TMDEBUG
void tmPoly::DumpToFileAsMath(const char* filename)
{
  ofstream gout(filename);
  gout << "g = Graphics[{ ";
  // ring nodes
  gout << "GrayLevel[0],";
  for (size_t i = 0; i < mRingNodes.size(); ++i) {
    tmNode* theNode = mRingNodes[i];
    gout << "Point[{" << theNode->GetLocX() << "," << theNode->GetLocY() << 
      "}], Text[ToString[" << theNode->GetIndex() << "], {" <<
      theNode->GetLocX() << "," << theNode->GetLocY() << "},{-1,0}],";
  };
  gout << "{},";
  // inset nodes
  gout << "RGBColor[1,0,0],";
  for (size_t i = 0; i < mOwnedNodes.size(); ++i) {
    tmNode* theNode = mOwnedNodes[i];
    gout << "Point[{" << theNode->GetLocX() << "," << theNode->GetLocY() << 
      "}], Text[ToString[" << theNode->GetIndex() << "], {" <<
      theNode->GetLocX() << "," << theNode->GetLocY() << "},{-1,0}],";
  };
  gout << "{}}];\n";
  gout << "Show[g, AspectRatio->Automatic, Frame->True];\n";
  gout.close();
}
#endif


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Calculate the centroid of this tmPoly and set related member variables, which
are the centroid proper and a vector of coordinates of the nodes.
*****/
void tmPoly::CalcContents()
{
  size_t numRingNodes = mRingNodes.size();
  mNodeLocs.resize(numRingNodes);
  mCentroid = tmPoint(0.0, 0.0);
  for (size_t i = 0; i < numRingNodes; ++i) 
    mCentroid += (mNodeLocs[i] = mRingNodes[i]->mLoc);
  mCentroid /= numRingNodes;
#ifdef TMDEBUG
  // This is also a good place to verify the integrity of the poly, by making
  // sure that the ring nodes are distinct; duplicate ring nodes will cause big
  // problems.
  for (size_t i = 0; i < numRingNodes - 1; ++i) {
    tmNode* aNode = mRingNodes[i];
    for (size_t j = i + 1; j < numRingNodes; ++j) {
      tmNode* bNode = mRingNodes[j];
      TMASSERT(aNode != bNode);
    }
  }
#endif // TMDEBUG
}


/*****
Return true if this tmPoly is convex to within our tolerance, when the vertices
are taken in a CCW direction. Note that this will return false for a CW polygon
(e.g., the border polygon) even if it is otherwise convex.
*****/
bool tmPoly::CalcPolyIsConvex() const
{
  size_t n = mRingNodes.size();
  TMASSERT(n >= 3);
  for (size_t i = 0; i < n - 2; ++i) {
    tmFloat a = AngleChange(mRingNodes[i]->mLoc, 
      mRingNodes[(i + 1) % n]->mLoc, 
      mRingNodes[(i + 2) % n]->mLoc);
    if (a < -tmTree::ConvexityTol()) return false;
  }
  return true;
}


/*****
Return true if this tmPoly encloses any node in the list in its interior. Note
that this is only works for convex polygons since tmPoly::ConvexEncloses() only
works for convex polys.
*****/
bool tmPoly::CalcPolyEnclosesNode(const tmArray<tmNode*> nlist) const
{
  TMASSERT(CalcPolyIsConvex());
  for (size_t i = 0; i < nlist.size(); ++i) {
    tmNode* aNode = nlist[i];
    if (!mRingNodes.contains(aNode) && ConvexEncloses(aNode->mLoc)) 
      return true;
  }
  return false;
}


/*****
Return true if this poly is still valid, i.e., none of its nodes have moved
and all the other things that made this a polygon of the tree remain true.
*****/
bool tmPoly::CalcPolyIsValid() const
{
  // If it's a sub poly, it's always deemed to be valid (but if its owner
  //  dies, then it dies).
  if (mIsSubPoly) return true;
  
  // If the number of nodes has changed, e.g., through deletion of a node, then
  // it's invalid.
  size_t numRingNodes = mRingNodes.size();
  if (mNodeLocs.size() != numRingNodes) {
    return false;
  }
  
  // If any of the nodes have moved more than our tolerance, return that we're
  // invalid.
  for (size_t i = 0; i < numRingNodes; ++i) {
    tmFloat moveDist = Mag(mNodeLocs[i] - mRingNodes[i]->mLoc);
    if (moveDist > MoveTol()) {
      return false;
    }
  }
  
  // If any of its paths are no longer polygon paths, it's invalid.
  for (size_t i = 0; i < mRingPaths.size(); ++i)
    if (!mRingPaths[i]->IsPolygonPath()) {
      return false;
    }
    
  // If it's nonconvex, it's invalid.
  if (!CalcPolyIsConvex()) {
    return false;
  }
  
  // If it encloses a leaf node that's not part of its ring, it's invalid.
  tmArray<tmNode*> leafNodes;
  mTree->GetLeafNodes(leafNodes);
  if (CalcPolyEnclosesNode(leafNodes)) {
    return false;
  }
  
  // If we passed all the tests, it's still valid.
  return true;
}


/*****
Calculate the paths that criss-cross this tmPoly (not the ring Paths). Note
that the owner of this tmPoly also owns the nodes and Paths from which it is
constructed, so we turn to that Owner to find the CrossPaths.
*****/
void tmPoly::CalcCrossPaths()
{
  // Create a reference to a cross path for every path between non-consecutive
  // nodes. Note that the wrap-around case is special.
  mCrossPaths.clear();
  size_t nn = mRingNodes.size();
  
  for (size_t i = 2; i < nn; ++i)
    for (size_t j = 0; j < i - 1; ++j) {
      if ((i == nn - 1) && (j == 0)) continue;
      tmNode* aNode1 = mRingNodes[i];
      tmNode* aNode2 = mRingNodes[j];
      tmPath* aPath = mPolyOwner->FindAnyPath(aNode1, aNode2);
      TMASSERT(aPath);
      mCrossPaths.union_with(aPath);
    }
}


/*****
Return a ptr to an inset tmNode that has this coordinate. If it doesn't exist,
create a new inset tmNode and return a ptr to the new tmNode. If it does exist,
return the existing node, but also set the node's flag as being a junction node
since junction nodes, by definition, are nodes that are inset from multiple ring
nodes.
*****/
tmNode* tmPoly::GetOrMakeInsetNode(const tmPoint& p)
{
  // First look for the point already existing.
  for (size_t i = 0; i < mOwnedNodes.size(); ++i) {
    tmNode* aNode = mOwnedNodes[i];
    if (IsTiny(Mag(aNode->mLoc - p))) {
      aNode->mIsJunctionNode = true;
      return aNode;
    }
  }
  
  // Still here? then we didn't find it and need to create a new one.
  tmNode* newNode = new tmNode(mTree, this, p);
  return newNode;
}


/*****
Recursively build up the list of nodes and paths that constitutes the ridgeline
that connects the two given nodes, which should be CCW consecutive nodes around
the outside of this polygon. The two arrays should initially be empty. This
routine calls itself recursively, and will leave in the array the sequence of
nodes and paths that climbs up to the peak (possibly in some inner subpoly) and
then descends back down to the next node. Called by BuildPolyContents() and
GetRidgelineVertices().
*****/
void tmPoly::GetRidgelineNodesAndPaths(tmNode* frontNode, tmNode* backNode, 
  tmArray<tmNode*>& ridgeNodes, tmArray<tmPath*>& ridgePaths)
{
  // We'll need offsets for both nodes in order to get access to the appropriate
  // inset nodes. The two nodes MUST be consecutive ring nodes.
  size_t frontOffset = mRingNodes.GetOffset(frontNode);
  TMASSERT(frontOffset != tmArray<tmNode*>::BAD_OFFSET);
  size_t backOffset = mRingNodes.GetOffset(backNode);
  TMASSERT(backOffset != tmArray<tmNode*>::BAD_OFFSET);
  TMASSERT(((backOffset + mRingNodes.size() - frontOffset) % \
    mRingNodes.size()) == 1);

  // If they're different, then we need to get part of the ridgeline from the
  // subpoly and/or inset nodes (depending on the number of the latter).
  ridgeNodes.push_back(frontNode);
  ridgePaths.push_back(mSpokePaths[frontOffset]);
  switch(mOwnedNodes.size()) {
    case 1: {
      // only 1 distinct inset node; we just have to add that to the list.
      ridgeNodes.push_back(mOwnedNodes.front());
      break;
    }
    case 2: {
      // 2 distinct inset nodes; that means there's a ridge between them. The
      // ridge may or may not be part of the ridgeline; it depends on whether
      // the front and back inset nodes are distinct.
      tmNode* frontInsetNode = mInsetNodes[frontOffset];
      tmNode* backInsetNode = mInsetNodes[backOffset];
      ridgeNodes.push_back(frontInsetNode);
      if (frontInsetNode != backInsetNode) {
        TMASSERT(mRidgePath);
        ridgePaths.push_back(mRidgePath);
        ridgeNodes.push_back(backInsetNode);
      }
      break;
    }
    default: {
      // 3 or more distinct inset nodes; get the inner bits from the subpoly.
      tmNode* frontInsetNode = mInsetNodes[frontOffset];
      tmNode* backInsetNode = mInsetNodes[backOffset];
      if (frontInsetNode == backInsetNode) {
        // if they're the same node, we don't need to go to a subpoly; we've
        // reached the peak of this ridgeline. Add it in, and we can start
        // descending.
        ridgeNodes.push_back(frontInsetNode);
      }
      else {
        // But if they're not the same node, we have to get the missing bits
        // from the subpoly. Here's where the recursion kicks in. We find the
        // subpoly whose ring path is the inset version of the path in
        // question. There should be only one.
        tmPoly* subPoly = 0;
        for (size_t i = 0; i < mOwnedPolys.size(); ++i) {
          tmPoly* thePoly = mOwnedPolys[i];
          if (!thePoly->mRingNodes.contains(frontInsetNode) ||
            !thePoly->mRingNodes.contains(backInsetNode)) continue;
          subPoly = thePoly;
        }
        TMASSERT(subPoly);
        subPoly->GetRidgelineNodesAndPaths(frontInsetNode, backInsetNode, 
          ridgeNodes, ridgePaths);
      }
    }
  }
  // Finally, add the back node and spoke path to the list.
  ridgePaths.push_back(mSpokePaths[backOffset]);
  ridgeNodes.push_back(backNode);
  // There should always be one more node than path.
  TMASSERT(ridgeNodes.size() == ridgePaths.size() + 1);

#ifdef TMDEBUG
  // Make sure each node and path appear only once in their respective lists.
  for (size_t i = 0; i < ridgeNodes.size(); ++i) {
    TMASSERT(ridgeNodes.count(ridgeNodes[i]) == 1);
  }
  for (size_t i = 0; i < ridgePaths.size(); ++i) {
    TMASSERT(ridgePaths.count(ridgePaths[i]) == 1);
  }
#endif // TMDEBUG
  
}


/*****
Initialize a ridge vertex to be sorted by passing with the front and back
vertices of the path that defines the sorting order. That order is in CW order
around the midpoint of the path. We compute the angle, mapping the front point
to -pi/2 radians and the back point to +pi/2; they will then be sorted by
angle. We use angle sorting rather than dot product along the sorting path
because the vertices at each end of 90 degree ridge creases have the same dot
product and would be ordered by numerical roundoff.
*****/
tmPoly::SortableRidgeVertex::SortableRidgeVertex(tmVertex* aVertex, 
  const tmPoint& frontPt, const tmPoint& backPt) :
  mVertex(aVertex)
{
  tmPoint pu = Normalize(backPt - frontPt);
  tmPoint pv = RotateCCW90(pu);
  tmPoint dm = 0.5 * (backPt + frontPt);
  tmPoint dp = aVertex->GetLoc() - dm;
  tmFloat du = Inner(dp, pu);
  tmFloat dv = Inner(dp, pv);
  mSortValue = atan2(du, dv);
}


/*****
Sorting function
*****/
bool tmPoly::SortableRidgeVertex::operator< (const SortableRidgeVertex& rhs)
  const
{
  return mSortValue < rhs.mSortValue;
}


/*****
Construct a sorted list of the vertices along the ridgeline above the path that
connects the two given nodes. We specify the nodes, rather than just passing a
tmPath*, because we need the nodes properly ordered for the call to
BuildRidgelineNodesAndPaths(); frontNode is not
necessarily thePath->mNodes.front(). This is used in two places in
BuildPolyContents(). Note the use of struct SortableRidgeVertex for sorting
vertices.
*****/
void tmPoly::GetRidgelineVertices(tmNode* frontNode, tmNode* backNode, 
  tmArray<tmVertex*>& ridgeVertices)
{
  // ridgeVertices should initially be empty.
  TMASSERT(ridgeVertices.empty());
  
  // Get the ridgeline nodes and paths. Note that ridgeNodes will be ordered
  // along the ridgeline from frontNode to backNode.
  tmArray<tmNode*> ridgeNodes;
  tmArray<tmPath*> ridgePaths;
  GetRidgelineNodesAndPaths(frontNode, backNode, ridgeNodes, ridgePaths);

  // Prepare to associate a sorting value with each of the vertices in the
  // ridgeline. Although ridgeNodes is ordered, we don't accumulate vertices
  // in order so we'll have to go back and sort them once we've accumulated
  // them all based on the sorting value we've associated with each vertex.
  tmArray<SortableRidgeVertex> vdlist;
  tmPoint p1 = frontNode->mLoc;
  tmPoint p2 = backNode->mLoc;
  tmPoint pu = Normalize(p2 - p1);

  // Accumulate all node-owned vertices in the ridgeline. If there is any
  // junction node that doesn't yet have a vertex, give it one; all junction
  // vertices should have vertices, but ridgeline peaks above inactive border
  // paths might have escaped so far.
  for (size_t i = 0; i < ridgeNodes.size(); ++i) {
    tmNode* ridgeNode = ridgeNodes[i];
    if (ridgeNode->IsJunctionNode()) ridgeNode->GetOrMakeVertexSelf();
    tmVertex* ridgeVertex = ridgeNode->GetVertex();
    if (ridgeVertex)
      vdlist.push_back(SortableRidgeVertex(ridgeVertex, p1, p2));
  }

  // Accumulate all path-owned vertices in the ridgeline
  for (size_t i = 0; i < ridgePaths.size(); ++i) {
    tmPath* ridgePath = ridgePaths[i];
    for (size_t j = 0; j < ridgePath->mOwnedVertices.size(); ++j) {
      tmVertex* ridgeVertex = ridgePath->mOwnedVertices[j];
      vdlist.push_back(SortableRidgeVertex(ridgeVertex,p1, p2));
    }
  }

  // Sort all the entries according to their distance along the path.
  sort(vdlist.begin(), vdlist.end());
  
  // Finally, copy the ordered list into the return array.
  ridgeVertices.reserve(vdlist.size());
  for (size_t i = 0; i < vdlist.size(); ++i)
    ridgeVertices.push_back(vdlist[i].mVertex);
  
  // Make sure we haven't double-listed any vertices.
#ifdef TMDEBUG
  for (size_t i = 0; i < ridgeVertices.size(); ++i) {
    TMASSERT(ridgeVertices.count(ridgeVertices[i]) == 1);
  }
#endif // TMDEBUG
}


/*****
Return true if the poly has contents, i.e., it has had BuildPolyContents called
for it. The easiest way to check for this is to look at whether it owns any
nodes.
*****/
bool tmPoly::HasPolyContents()
{
  return mOwnedNodes.not_empty();
}


/*****
Build the contents of this tmPoly and its subPolys. This routine gets called
after a tmPoly has been created with at least three sides. Note that subpolys
get killed automatically when this object is destroyed because it owns them
through mOwnedPolys. This routine may safely be called for an already-built
tmPoly (nothing will happen).
*****/
void tmPoly::BuildPolyContents()
{
  // If this tmPoly already contains any inset nodes, it's already been built
  // and we can stop here.
  if (HasPolyContents()) return;
  
  // We can assume that mRingNodes and mRingPaths are created and valid.
  // mRingNodes is an ordered list of nodes running around the outside of the
  // tmPoly. mRingPaths is an ordered list of Paths running around the outside
  // of the tmPoly.

#ifdef TMDEBUG
  {
    // To avoid infinite recursion (and the ickiness that entails), we need to
    // make sure that if this is a subpoly, it's smaller than its owner.
    tmPoly* aPoly = GetOwnerAsPoly();
    if (aPoly) {
    TMASSERT(mRingNodes.size() < aPoly->mRingNodes.size());
    }
  }
#endif // TMDEBUG
  
  ///////////////////////////////////
  // Construction of subpolys
  ///////////////////////////////////
  
  tmNode* aNode;
  tmPoint p;
  size_t nn = mRingNodes.size();
  TMASSERT(mRingNodes.size() >= 3);
  if (mRingNodes.size() == 3) {
    // If there are 3 sides, we don't need to go through the insetting
    // calculation; we can just create a single inset tmNode and map each of
    // the outer ring nodes onto it.
    tmPoint& p1 = mRingNodes[0]->mLoc;
    tmPoint& p2 = mRingNodes[1]->mLoc;
    tmPoint& p3 = mRingNodes[2]->mLoc;
    p = Incenter(p1, p2, p3);
    aNode = new tmNode(mTree, this, p);
    aNode->mIsJunctionNode = true;
    aNode->mElevation = mRingNodes.front()->mElevation + Inradius(p1, p2, p3);

    mInsetNodes.push_back(aNode); // all three nodes map to the inset tmNode
    mInsetNodes.push_back(aNode);
    mInsetNodes.push_back(aNode);

    mSpokePaths.push_back(new tmPath(this, mRingNodes[0], aNode));
    mSpokePaths.push_back(new tmPath(this, mRingNodes[1], aNode));
    mSpokePaths.push_back(new tmPath(this, mRingNodes[2], aNode));
  }
  else {
    // If there are 4 or more sides, then we need to inset the tmPoly and break
    // it up into one or more smaller Polys. This is a much more complicated
    // sequence of actions!
      
    //**********************
    // Part I: find the inset distance that produces a reduced polygon
    //**********************
    
    // Construct tables of vectors used in the insetting process
    
    vector<tmPoint> r(nn);  // bisector vector at each corner
    vector<tmPoint> rp(nn); // normalized vector to previous corner
    vector<tmPoint> rn(nn); // normalized vector to next corner
    vector<tmFloat> mr(nn); // magnitude of projection of r along previous side
  
    size_t i, j;
    for (i = 0; i < nn; ++i) {    
      // get offsets of previous (ip) and next (in) corner, including
      // wrap-around effects.
     size_t ip = size_t((i + nn - 1) % nn);
      size_t in = size_t((i + 1) % nn);
      
      tmPoint nip = mRingNodes[ip]->mLoc;
      tmPoint nii = mRingNodes[i]->mLoc;
      tmPoint nin = mRingNodes[in]->mLoc;
      
      // construct bisector and the magnitude of its projections along the
      // previous and next sides. All quantities are normalized to a unit inset
      // distance h.
      rp[i] = Normalize(nip - nii);       // vector to previous corner
      rn[i] = Normalize(nin - nii);       // vector to next corner
      tmPoint bis = Normalize(RotateCCW90(rn[i] - rp[i]));  // angle bisector
      r[i] = bis / Inner(bis, RotateCCW90(rn[i]));  // normalize to unit inset
      mr[i] = Inner(r[i], rp[i]);         // cotangent of bisected angle
    } 
    
    // Now compute the maximum value of the inset distance h that satifies the
    // reduced path condition for every path in the polygon. We handle RingPaths
    // and CrossPaths differently. i and j are the indices of the corners of the
    // path.
    const tmFloat HMAX = 1.0e10;
    tmFloat h = HMAX; // the best inset distance to use
    for (i = 0; i < nn - 1; ++i)
      for (j = i + 1; j < nn; ++j) {      
  
        // If the angle bisectors are parallel and are pointing the same
        // direction, there's no solution, so go on to the next corner.
        if (AreParallel(r[i], r[j]) && (Inner(r[i], r[j]) > 0)) continue;
        
        // Get coordinates of the two nodes that we're checking
        tmPoint ni = mRingNodes[i]->mLoc;
        tmPoint nj = mRingNodes[j]->mLoc;
        
        // for paths between adjacent nodes, we'll use the intersection of
        // the bisectors to determine the maximum inset.
        if ((j == i + 1) || ((i == 0) && (j == nn - 1))) {
        
          tmPoint bi;
          GetLineIntersection(ni, r[i], nj, r[j], bi);
          tmFloat h1 = Inner((bi - ni), RotateCCW90(rn[i]));
  
          // If the computed inset (h1) is smaller than the current inset (h),
          // reduce the current inset. Something to check for: if the angle
          // bisectors are barely NOT parallel, then we might have a very large
          // positive or negative h, which is bogus. We don't worry about
          // positive values, but negative values would cause big problems. So
          // we'll ignore negative values of h1 when we do the comparison.
          if ((h1 > 0) && (h > h1)) h = h1;
        }
        else {      
          // for paths between nonadjacent nodes, we'll compute the inset
          // distance (using the cotangents of the base angles) that makes the
          // given path active. If the computed inset distance is either complex
          // or negative, there's no solution. We'll keep the smallest inset
          // distance that we find.
  
          // Note: an alternate way of structuring this would be to iterate
          // through the mRingPaths and mCrossPaths separately; I wouldn't have
          // to call FindPath. But I reference the tmNode information in the
          // vectors by what amounts to the index of the mRingNode. So I do it
          // this way. A better way, which I should implement in the future, is
          // to put the vector data into scratch fields of the tmNode.
  
          // Note that if the reduced path length comes out to be negative,
          // we've found a spurious solution; so we have to detect and
          // eliminate that case.
          tmFloat lij = mPolyOwner->FindAnyPath(mRingNodes[i], 
            mRingNodes[j])->mMinPaperLength;
          tmPoint u = ni - nj;
          tmPoint v = r[i] - r[j];
          tmFloat w = mr[i] + mr[j];
          tmFloat a = Mag2(v) - pow(w, 2);
          tmFloat b = Inner(u, v) + lij * w;
          tmFloat c = Mag2(u) - pow(lij, 2);
          tmFloat d = pow(b, 2) - a * c;
          if (d < 0) continue;      // both solutions are complex
          
          tmFloat h1 = (-b + sqrt(d)) / a; // trial solution for inset distance
          tmFloat lijp = lij - h1 * (mr[i] + mr[j]);  // reduced path length
          if ((lijp > 0) && (h1 > 0) && (h > h1)) h = h1;
          
          h1 = (-b - sqrt(d)) / a;          // other trial solution
          lijp = lij - h1 * (mr[i] + mr[j]);      // reduced path length
          if ((lijp > 0) && (h1 > 0) && (h > h1)) h = h1;
        }
      }
      
    // If we didn't find an acceptable value of the inset, then something's
    // wrong.
    TMASSERT(h != HMAX);
    
    // At this point, the variable h now holds the valid inset distance.
    
    //**********************
    // Part II: build a list of inset nodes and inset paths. These become the
    // mOwnedNodes and mOwnedPaths of this poly.
    //**********************
    
    // We'll go through the list of corners and inset each corner, making a new
    // list of distinct inset corners. In many cases, two or more corners have
    // been inset to the same point (i.e., there's a net reduction in the
    // number of corners); so we'll also construct a list of references mapping
    // old corners to new, inset corners. We create new nodes (these are
    // sub-nodes owned by this tmPoly) as needed; the member list mInsetNodes
    // maintains the mapping, while a list of the distinct subnodes is provided
    // by mOwnedNodes.  
    for (i = 0; i < nn; ++i) {
      p = mRingNodes[i]->mLoc + h * r[i];       // inset corner location
      mInsetNodes.push_back(GetOrMakeInsetNode(p));
    }
    size_t numOwnedNodes = mOwnedNodes.size();
    
    // Set elevation of newly-created nodes
    for (size_t i = 0; i < numOwnedNodes; ++i)
      mOwnedNodes[i]->mElevation = mRingNodes.front()->mElevation + h;
    
#ifdef TMDEBUG
    // As an error check, make sure that all of the inset corners are still
    // enclosed by the outer ring.
    bool goodEnclosure = true;
    for (i = 0; i < numOwnedNodes; ++i) {
      goodEnclosure &= ConvexEncloses(mOwnedNodes[i]->mLoc);
      if (!goodEnclosure) {
        // If enclosure was bad, let's dump some diagnostic information to
        // files.
        DumpToFileAsText("BAD_BUILD_SUBPOLY.txt");
        DumpToFileAsMath("BAD_BUILD_SUBPOLY.ma");
  
        // break on this call to find the offending node
        ConvexEncloses(mOwnedNodes[i]->mLoc);
        TMFAIL("In tmPoly::BuildPolyContents() inset corner was not enclosed");
      }
    }
#endif // TMDEBUG
  
    // Now we have a valid inset. The next action depends on how many distinct
    // inset nodes we have.
    if ((numOwnedNodes == 1) || (numOwnedNodes == 2)) {
      // if 1 or 2 nodes, we're done building molecules. The last thing we do
      // in preparation for building the Creases is to create Paths for the
      // spokes of the reduction.
      for (i = 0; i < nn; ++i)
        mSpokePaths.push_back(new tmPath(this, mRingNodes[i], mInsetNodes[i])); 
      // If there are exactly two distinct inset nodes, we create a tmPath
      // corresponding to the ridge.
      if (numOwnedNodes == 2) 
        mRidgePath = new tmPath(this, mOwnedNodes[0], mOwnedNodes[1]);    
    }
    else {
      // If there are 3 or more distinct inset nodes, this tmPoly will have at
      // least one sub-tmPoly. We build a new complete set of reduced paths
      // from which we can construct new polys.

      // construct the distinct inset paths that link the inset nodes. These
      // will consist of ring paths and cross paths. Since multiple outer nodes
      // may map to a single inset node, multiple outset paths can potentially
      // map to the same inset path. For each ring path of the inset poly, we
      // want its outset path to be a ring path of the outset poly; that is, a
      // path whose nodes are consecutive members of mRingNodes. So, we'll go
      // through every distinct pair of nodes in mRingNodes, but choosing an
      // order such that we go through all the consecutive pairs of mRingNodes
      // before eventually trying every possible pair.
      bool madeActiveCrossPath = false;
      for (size_t dij = 1; dij < nn; ++dij)
        for (size_t i = 0; i <= nn - dij; ++i) {
          size_t j = (i + dij) % nn;
          tmNode* ni = mRingNodes[i];
          tmNode* nj = mRingNodes[j];
          tmNode* rni = mInsetNodes[i];
          tmNode* rnj = mInsetNodes[j];
          
          // if both outer nodes map to the same inner tmNode, go on to the next
          // pair.
          if (rni == rnj) continue;
          
          // If a path already exists between the two inset nodes go on to the
          // next pair.
          if (FindLeafPath(rni, rnj)) continue;
          
          // if we didn't find it, need to create a new path.
          tmPath* outsetPath = mPolyOwner->FindAnyPath(ni, nj);
          TMASSERT(outsetPath);
          tmFloat iReduction = h * mr[i];
          tmFloat jReduction = h * mr[j];
    
          // When we create the new path, we want it in the same orientation as
          // the path from which it is inset, so order of rni, rnj is important.
          tmPath* thePath;
          if (outsetPath->mNodes.front() == ni) {
            // rni should be front node
            thePath = new tmPath(this, rni, rnj);
            thePath->mFrontReduction = iReduction;
            thePath->mBackReduction = jReduction;
          }
          else {
            // rnj should be front node
            thePath = new tmPath(this, rnj, rni);
            thePath->mFrontReduction = jReduction;
            thePath->mBackReduction = iReduction;
          }
          
          // Connect it to the ring nodes it was created from
          rni->mLeafPaths.push_back(thePath);
          rnj->mLeafPaths.push_back(thePath);
          
          // Set length-related variables
          thePath->mOutsetPath = outsetPath;
          thePath->mMinPaperLength = outsetPath->mMinPaperLength -
            (thePath->mFrontReduction + thePath->mBackReduction);
          thePath->mActPaperLength = Mag((rni->mLoc) - (rnj->mLoc));
          thePath->mMinTreeLength = thePath->mMinPaperLength / mTree->mScale;
          thePath->mActTreeLength = thePath->mActPaperLength / mTree->mScale;
          
          // Set active status; we're definitely active if the path we're inset
          // from is active, but could be active even if our outset path is not.
          thePath->mIsActivePath = outsetPath->IsActivePath();
    
          // if the reduced path length is equal to its physical length, it's
          // active whether its outset path was active or not. (If it's outset
          // path was not active and this one is, then it's an active cross
          // path.)
          thePath->mIsActivePath |= thePath->TestIsActive(
            thePath->mActPaperLength, thePath->mMinPaperLength);
    
          // if the nodes are consecutive, the inset path is a border path and
          // is therefore also a polygon path. If it isn't a border path, note
          // whether it's active.
          if (dij == 1) thePath->mIsBorderPath = true;
          else madeActiveCrossPath |= thePath->IsActivePath();
          
          // If it's either active or border, it's a polygon path
          thePath->mIsPolygonPath = 
            thePath->IsActivePath() || thePath->IsBorderPath();
        }
      
#ifdef TMDEBUG
      // count the number of polygon paths we created
      size_t numPolygonPaths = 0;
      for (size_t i = 0; i < mOwnedPaths.size(); ++i)
        if (mOwnedPaths[i]->IsPolygonPath()) ++numPolygonPaths;
      // Another debugging check: we must have either reduced the number of
      // inset nodes or created an active CrossPath. If we didn't do either,
      // something is wrong.
      TMASSERT(madeActiveCrossPath || (mOwnedNodes.size() < mRingNodes.size()));
#endif
      
      // Now create sub-polygons from the network of paths. We pass the inset
      // nodes as the list of border nodes, from which the centroid of the
      // border nodes (which is also the convex hull) is computed.
      BuildPolysFromPaths(mOwnedPaths, mInsetNodes);  
      
      // For the newly-built polys, call their BuildSubPolys routines to
      // recursively build all levels.
      tmPoly* aPoly;
      tmArrayIterator<tmPoly*> iOwnedPolys(mOwnedPolys);
      while (iOwnedPolys.Next(&aPoly)) aPoly->BuildPolyContents();
      
      // The last thing we do in preparation for building the Creases is to
      // create Paths for the spokes of the reduction.
      for (size_t i = 0; i < nn; ++i)
        mSpokePaths.push_back(new tmPath(this, mRingNodes[i], mInsetNodes[i]));
    } // End of branch with numOwnedNodes >= 3
  } // End of branch with (mRingNodes.size() > 3)
  
  /////////////////////////////////
  // Build creases
  /////////////////////////////////
  
  // Build the crease pattern within this poly (or, if it's a subpoly, build
  // certain portions of the crease pattern -- specifically, those parts
  // associated with gusset paths and creases. Note that all ridge and hinge
  // creases should end up owned by the outermost poly, even if we're creating
  // them from within a subpoly.
  for (size_t i = 0; i < nn; ++i) {
    tmNode* frontNode = mRingNodes[i];
    tmNode* backNode = mRingNodes[(i + 1) % nn];
    tmPath* thePath = mRingPaths[i];

    // For ALL axial paths and gusset paths, build vertices along the path.
    if (thePath->IsAxialPath() || thePath->IsGussetPath())
      thePath->BuildSelfVertices();

    // For active-axial and gusset (but not active inset ring paths),
    // build the ridgeline that goes from one end of thePath to the other;
    // then build all hinge creases that propagate up to the ridgeline. This
    // will create some vertices along the ridgeline.
    if (thePath->IsActiveAxialPath() || thePath->IsGussetPath()) {
      tmArray<tmNode*> ridgeNodes;
      tmArray<tmPath*> ridgePaths;
      GetRidgelineNodesAndPaths(frontNode, backNode, ridgeNodes, ridgePaths);
      tmPoint p1 = frontNode->mLoc;
      tmPoint p2 = backNode->mLoc;
      for (size_t j = 0; j < thePath->mOwnedVertices.size(); ++j) {
        tmVertex* botVertex = thePath->mOwnedVertices[j];
        for (size_t k = 0; k < ridgePaths.size(); ++k) {
          tmPath* ridgePath = ridgePaths[k];
          tmNode* nq1 = ridgePath->mNodes.front();
          tmNode* nq2 = ridgePath->mNodes.back();
          tmPoint& q1 = nq1->mLoc;
          tmPoint& q2 = nq2->mLoc;
          tmPoint q;
          if (ProjectPtoQ(p1, p2, botVertex->mLoc, q1, q2, q)) {
            tmVertex* topVertex = 
              ridgePath->GetOrMakeVertex(q, botVertex->mTreeNode);
            GetOutermostPoly()->
              GetOrMakeCrease(botVertex, topVertex, tmCrease::UNFOLDED_HINGE);
          }
        }
      }
    }
  }
  
  // We need to restart the loop over paths to insure that every ridgeline
  // has had every opportunity to have its vertices constructed, before these
  // next couple of constructions.
  for (size_t i = 0; i < nn; ++i) {
    tmNode* frontNode = mRingNodes[i];
    tmNode* backNode = mRingNodes[(i + 1) % nn];
    tmPath* thePath = mRingPaths[i];
    
    // For all axial paths and gusset paths, create creases connecting the
    // tops of all the ridgeline vertices. Note that this means that all ridge
    // creases, like hinge creases, will be owned by polys, while axial and
    // gusset creases will be owned by their associated paths. 
    if (thePath->IsAxialPath() || thePath->IsGussetPath()) {
      tmArray<tmVertex*> ridgeVertices;
      GetRidgelineVertices(frontNode, backNode, ridgeVertices);
      TMASSERT(ridgeVertices.not_empty());
      tmVertex* ridgeVertex = ridgeVertices[0];
      for (size_t m = 1; m < ridgeVertices.size(); ++m) {
        tmVertex* nextRidgeVertex = ridgeVertices[m];
        GetOutermostPoly()->
          GetOrMakeCrease(ridgeVertex, nextRidgeVertex, tmCrease::RIDGE);
        ridgeVertex = nextRidgeVertex;
      }
      // For the inactive axial paths, we haven't created any hinge creases yet;
      // hinge creases were propagated upward for active-axial and gusset paths.
      // For inactive-axial paths, we propagate creases downward. But not from
      // all vertices. If the ridge vertex has a tree node, it should get a
      // proper hinge crease. If it doesn't have a tree node, but the vertices on
      // either side have the same tree node, then it gets a pseudohinge crease.
      if (thePath->IsAxialPath() && !thePath->IsActivePath()) {
        TMASSERT(frontNode->GetVertex());
        TMASSERT(backNode->GetVertex());
        TMASSERT(frontNode->GetVertex() != backNode->GetVertex());
        tmPoint& p1 = frontNode->GetVertex()->mLoc;
        tmPoint& p2 = backNode->GetVertex()->mLoc;
        tmCrease* crease0 = 0;  // used to identify pseudohinge mates
        tmCrease* crease1 = 0;
        tmCrease* crease2 = 0;
        for (size_t m = 1; m < ridgeVertices.size() - 1; ++m) {
          bool needsCrease = false;
          tmCrease::Kind theKind = tmCrease::UNFOLDED_HINGE;
          tmVertex* ridgeVertex = ridgeVertices[m];
          if (ridgeVertex->mTreeNode) {
            // it's a regular hinge crease
            needsCrease = true;
          }
          else {        
            tmVertex* prevRidgeVertex = ridgeVertices[m - 1];
            tmVertex* nextRidgeVertex = ridgeVertices[m + 1];
            if (prevRidgeVertex->mTreeNode && nextRidgeVertex->mTreeNode &&
              (prevRidgeVertex->mTreeNode == nextRidgeVertex->mTreeNode)) {
              // it's a pseudohinge crease
              needsCrease = true;
              theKind = tmCrease::PSEUDOHINGE;
            }
          }
          if (!needsCrease) continue;
          tmPoint q = ridgeVertex->mLoc;
          tmPoint p;
          if (ProjectQtoP(q, p1, p2, p)) {
            tmVertex* botVertex = 
              thePath->GetOrMakeVertex(p, ridgeVertex->mTreeNode);
            crease2 = crease1;
            crease1 = crease0;
            crease0 = GetOrMakeCrease(botVertex, ridgeVertex, theKind);
          }
          // If the last crease created completes a pseudohinge triplet, assign
          // mate-ship among the lower vertices of the two regular hinges.
          if (crease0 && crease1 && crease2 && 
            crease0->mKind == tmCrease::UNFOLDED_HINGE &&
            crease1->mKind == tmCrease::PSEUDOHINGE &&
            crease2->mKind == tmCrease::UNFOLDED_HINGE) {
            tmVertex* mateVertex0 = crease0->GetLowerVertex();
            tmVertex* mateVertex2 = crease2->GetLowerVertex();
            mateVertex0->mRightPseudohingeMate = mateVertex2;
            mateVertex2->mLeftPseudohingeMate = mateVertex0;
          }
        }
      }
    }

    // We can now be assured that we have constructed all vertices along
    // gusset  or axial paths, so we can now connect the vertices in the
    // path with creases.
    if (thePath->IsAxialPath() || thePath->IsGussetPath()) {
      tmCrease::Kind creaseKind =
        thePath->IsAxialPath() ? tmCrease::AXIAL : tmCrease::GUSSET;
      thePath->ConnectSelfVertices(creaseKind);
    }
  }

  // Remaining actions only apply to major polys.
  if (mIsSubPoly) return;
  
  /////////////////////////////////
  // Build facets
  /////////////////////////////////
  
  // All creases are built; now construct the facets within this poly. First,
  // make a list of all the creases from which we will be building facets.
  // Those are the creases owned by this poly, as well as the creases owned
  // by the paths around the boundary of the poly. Then we'll build the facets
  // from these creases.
  tmArray<tmCrease*> facetCreases( mOwnedCreases );
  for (size_t i = 0; i < mRingPaths.size(); ++i)
    facetCreases.merge_with(mRingPaths[i]->mOwnedCreases);
  
  // Construct the facets from these creases.
  BuildFacetsFromCreases(facetCreases);
}


/*****
Return the number of inactive border paths around the outside of this poly.
*****/
size_t tmPoly::GetNumInactiveBorderPaths()
{
  size_t n = 0;
  for (size_t i = 0; i < mRingPaths.size(); ++i) {
    tmPath* thePath = mRingPaths[i];
    if (!thePath->IsActivePath()) ++n;
  }
  return n;
}


/*****
Set the corridor edge of this facet and propagate to adjacent facets within
the same corridor and this poly (but not outside the poly).
*****/
void tmPoly::SetFacetCorridorEdge(tmFacet* aFacet, tmEdge* aEdge)
{
  aFacet->mCorridorEdge = aEdge;
  for (size_t i = 0; i < aFacet->mCreases.size(); ++i) {
    tmCrease* theCrease = aFacet->mCreases[i];
    if (theCrease->IsRegularHingeCrease()) continue;
    tmFacet* otherFacet = theCrease->GetOtherFacet(aFacet);
    if (!otherFacet) continue;
    if (!mOwnedFacets.contains(otherFacet)) continue;
    if (otherFacet->mCorridorEdge) continue;
    SetFacetCorridorEdge(otherFacet, aEdge);
  }
}


/*****
For every facet, give each facet a ptr to the edge corresponding to the
corridor that contains the facet.
*****/
void tmPoly::CalcFacetCorridorEdges()
{
  // We implement this by launching corridor edge assignment from each of the
  // axial facets for which we can identify the corridor edge.
  for (size_t i = 0; i < mOwnedFacets.size(); ++i) {
    tmFacet* theFacet = mOwnedFacets[i];
    if (theFacet->mCorridorEdge) continue;
    if (!theFacet->IsAxialFacet()) continue;
    tmCrease* botCrease = theFacet->GetBottomCrease();
    tmNode* n1 = botCrease->mVertices.front()->mTreeNode;
    tmNode* n2 = botCrease->mVertices.back()->mTreeNode;
    if (!n1 || !n2) continue;
    tmEdge* theEdge = mTree->GetEdge(n1, n2);
    TMASSERT(theEdge);
    SetFacetCorridorEdge(theFacet, theEdge);
  }
}



#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Construct bend information within this poly, i.e., information about which
creases are folded and which hinge creases and vertices are local root. Also
construct bend-related lists of local root vertices and creases.
*****/
void tmPoly::CalcBend()
{
  // First, we pass on the message to the creases, which can calculate their
  // own bend information.
  for (size_t i = 0; i < mOwnedCreases.size(); ++i)
    mOwnedCreases[i]->CalcBend();
  
  // Make a list of all vertices in this polygon. Collect them from the interior
  // creases, then add in the corners of the polygon.
  tmArray<tmVertex*> allVertices;
  for (size_t i = 0; i < mOwnedCreases.size(); ++i)
    allVertices.union_with(mOwnedCreases[i]->mVertices);
  for (size_t i = 0; i < mRingNodes.size(); ++i)
    allVertices.union_with(mRingNodes[i]->GetVertex());

  // Now we reconstruct our lists of local root hinge creases and vertices by
  // searching through the list of all vertices. (Note that there might not be
  // any local root hinge creases if a corner of the polygon is the root
  // vertex.) Each time we find a smaller discrete depth, we restart our
  // accumulation of both vertices and creases.
  mLocalRootVertices.clear();
  mLocalRootCreases.clear();
  size_t minDiscreteDepth = size_t(-1);
  for (size_t i = 0; i < allVertices.size(); ++i) {
    tmVertex* theVertex = allVertices[i];
    size_t discreteDepth = theVertex->GetDiscreteDepth();
    if (minDiscreteDepth > discreteDepth) {
      minDiscreteDepth = discreteDepth;
      mLocalRootVertices.clear();
      mLocalRootCreases.clear();
    }
    if (minDiscreteDepth == discreteDepth) {
      mLocalRootVertices.union_with(theVertex);
      for (size_t j = 0; j < theVertex->mCreases.size(); ++j) {
        tmCrease* theCrease = theVertex->mCreases[j];
        if (theCrease->IsHingeCrease() && mOwnedCreases.contains(theCrease)) 
          mLocalRootCreases.union_with(theCrease);
      }
    }
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the unique facet that is interior to this poly and that is incident to
the given axial crease. Since the crease is incident to only two facets, we
just check the two facets directly.
*****/
tmFacet* tmPoly::GetIncidentInteriorFacet(tmCrease* axialCrease)
{
  TMASSERT(axialCrease);
  TMASSERT(axialCrease->IsAxialCrease());
  tmFacet* theFacet = axialCrease->mFwdFacet;
  if (!!theFacet && mOwnedFacets.contains(theFacet))
    return theFacet;
  theFacet = axialCrease->mBkdFacet;
  TMASSERT(theFacet);
  TMASSERT(mOwnedFacets.contains(theFacet));
  return theFacet;
}


/*****
Return the unique crease (hinge or ridge) that is interior to this poly and
that is incident to the given axial vertex. We search over the vertices
incident to the vertex making sure it's either hinge or ridge, and then make
sure that this poly owns the crease to screen out creases in adjacent polys.
*****/
tmCrease* tmPoly::GetIncidentInteriorCrease(tmVertex* axialVertex)
{
  TMASSERT(axialVertex);
  TMASSERT(axialVertex->IsAxialVertex());
  for (size_t i = 0; i < axialVertex->mCreases.size(); ++i) {
    tmCrease* theCrease = axialVertex->mCreases[i];
    if ((theCrease->IsHingeCrease() || theCrease->IsRidgeCrease()) && 
      mOwnedCreases.contains(theCrease)) return theCrease;
  }
  TMFAIL("In tmPoly::GetIncidentInteriorCrease() couldn't find crease");
  return 0;
}


/*****
Build the facet ordering(s) that run along corridors, sourced from fromFacet.
We construct a tail-to-head ordering relationship from fromFacet to any facets
incident to creases opposite fromCrease, *unless* such a relationship already
exists, in either direction.
*****/
void tmPoly::BuildCorridorLinks(tmCrease* fromCrease, tmFacet* fromFacet)
{
  tmCrease* botCrease = fromFacet->GetBottomCrease();
  if (botCrease == fromCrease) {
    // going up: propagate across all ridge creases incident to the facet
    for (size_t i = 0; i < fromFacet->mCreases.size(); ++i) {
      tmCrease* nextCrease = fromFacet->mCreases[i];
      if (!nextCrease->IsRidgeCrease()) continue;
      tmFacet* nextFacet = nextCrease->GetOtherFacet(fromFacet);
      if (botCrease->IsAxialCrease()) {
        // Don't need to cross ridges that will be crossed as part of the
        // axial loop.
        if (nextFacet == fromFacet->GetLeftFacet()) continue;
        if (nextFacet == fromFacet->GetRightFacet()) continue;
      }
      if (tmFacet::AreLinked(fromFacet, nextFacet)) continue;
      fromFacet->LinkTo(nextFacet);
      BuildCorridorLinks(nextCrease, nextFacet);
    }
  }
  else {
    if (botCrease->IsGussetCrease()) {
      // going down, bottom crease is gusset. Propagate across the bottom crease.
      tmFacet* nextFacet = botCrease->GetOtherFacet(fromFacet);
      fromFacet->LinkTo(nextFacet);
      BuildCorridorLinks(botCrease, nextFacet);
    }
    else {
      // going down, bottom crease is axial. If we're on a normal facet,
      // stop propagating.
      if (fromFacet->IsNotPseudohingeFacet()) return;
      // But if we are on a pseudohinge facet, then we connect to the
      // adjacent pseudohinge facet and start back upward.
      tmCrease* phCrease = fromFacet->GetLeftCrease();
      tmFacet* nextFacet;
      if (phCrease->IsPseudohingeCrease())
        nextFacet = phCrease->GetLeftFacet();
      else {
        phCrease = fromFacet->GetRightCrease();
        TMASSERT(phCrease->IsPseudohingeCrease());
        nextFacet = phCrease->GetRightFacet();
      }
      fromFacet->LinkTo(nextFacet);
      tmCrease* nextBotCrease = nextFacet->GetBottomCrease();
      BuildCorridorLinks(nextBotCrease, nextFacet);
    }
  }
}


/*****
Construct the facet ordering graph within this poly. We construct a complete
axial loop and all corridor crossings. Breaking the axial loop at any local
root vertex gives a valid facet ordering graph for the entire molecule.
*****/
void tmPoly::CalcLocalFacetOrder()
{
  // Start by clearing all ordering affecting this poly's facets.
  for (size_t i = 0; i < mOwnedFacets.size(); ++i)
    mOwnedFacets[i]->ClearLinks();
  
  // Find a local root axial vertex from which to start.
  tmVertex* startVertex = 0;
  for (size_t i = 0; i < mLocalRootVertices.size(); ++i) {
    startVertex = mLocalRootVertices[i];
    if (startVertex->IsAxialVertex()) break;
  }
  TMASSERT(startVertex);
  tmFacet* startFacet = 
    GetIncidentInteriorCrease(startVertex)->GetRightNonPseudohingeFacet();
  
  // Now circulate CCW through all creases and facets. Add a link from each
  // facet to the next one, forming a complete axial loop. In addition, launch
  // corridor crossings if appropriate at each encountered facet.
  tmFacet* curFacet = startFacet;
  do {
    tmFacet* nextFacet = curFacet->GetRightNonPseudohingeFacet();
    curFacet->LinkTo(nextFacet);
    BuildCorridorLinks(curFacet->GetBottomCrease(), curFacet);
    curFacet = nextFacet;
  } while (curFacet != startFacet);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Put a poly in version 5 format
*****/
void tmPoly::Putv5Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex); 
  PutPOD(os, mCentroid);
  PutPOD(os, mIsSubPoly);
  mTree->PutPtrArray(os, mRingNodes);
  mTree->PutPtrArray(os, mRingPaths);
  mTree->PutPtrArray(os, mCrossPaths);
  mTree->PutPtrArray(os, mInsetNodes);
  mTree->PutPtrArray(os, mSpokePaths);
  mTree->PutPtr(os, mRidgePath);  
  PutPOD(os, mNodeLocs);
  mTree->PutPtrArray(os, mLocalRootVertices);
  mTree->PutPtrArray(os, mLocalRootCreases);
  mTree->PutPtrArray(os, mOwnedNodes);
  mTree->PutPtrArray(os, mOwnedPaths);
  mTree->PutPtrArray(os, mOwnedPolys);
  mTree->PutPtrArray(os, mOwnedCreases);
  mTree->PutPtrArray(os, mOwnedFacets);
  mTree->PutOwnerPtr(os, mPolyOwner);
}


/*****
Get a poly in version 5 format
*****/
void tmPoly::Getv5Self(istream& is)
{
  CheckTagStr<tmPoly>(is);
  GetPOD(is, mIndex);
  GetPOD(is, mCentroid);
  GetPOD(is, mIsSubPoly);
  mTree->GetPtrArray(is, mRingNodes);
  mTree->GetPtrArray(is, mRingPaths);
  mTree->GetPtrArray(is, mCrossPaths);
  mTree->GetPtrArray(is, mInsetNodes);
  mTree->GetPtrArray(is, mSpokePaths);
  mTree->GetPtr(is, mRidgePath, true);
  GetPOD(is, mNodeLocs);
  mTree->GetPtrArray(is, mLocalRootVertices);
  mTree->GetPtrArray(is, mLocalRootCreases);
  mTree->GetPtrArray(is, mOwnedNodes);
  mTree->GetPtrArray(is, mOwnedPaths);
  mTree->GetPtrArray(is, mOwnedPolys);
  mTree->GetPtrArray(is, mOwnedCreases);
  mTree->GetPtrArray(is, mOwnedFacets);
  mTree->GetOwnerPtr(is, mPolyOwner);
}


/*****
Put a poly in version 4 format
*****/
void tmPoly::Putv4Self(ostream& os)
{
  TMFAIL("tmPoly::Putv4Self");
}


/*****
Get a poly in version 4 format
*****/
void tmPoly::Getv4Self(istream& is)
{
  CheckTagStr<tmPoly>(is);
  GetPOD(is, mIndex);
  GetPOD(is, mCentroid);
  GetPOD(is, mNodeLocs);
  GetPOD(is, mIsSubPoly);
  mTree->GetPtrArray(is, mOwnedNodes);
  mTree->GetPtrArray(is, mOwnedPaths);
  mTree->GetPtrArray(is, mOwnedPolys);
  mTree->GetPtrArray(is, mOwnedCreases);
  mTree->GetPtrArray(is, mRingNodes);
  mTree->GetPtrArray(is, mRingPaths);
  mTree->GetPtrArray(is, mCrossPaths);
  mTree->GetPtrArray(is, mInsetNodes);
  mTree->GetPtrArray(is, mSpokePaths);
  mTree->GetPtr(is, mRidgePath, true);
  mTree->GetOwnerPtr(is, mPolyOwner);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmPoly, "poly")

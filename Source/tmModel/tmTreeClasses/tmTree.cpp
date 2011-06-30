/*******************************************************************************
File:         tmTree.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmTree
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmTree.h"
#include "tmModel.h"

#ifdef TMDEBUG
  #include <fstream>
#endif
#include <algorithm>

using namespace std;

/**********
class tmTree
The core class of TreeMaker. It contains the full mathematical model.
**********/

/*
Class tmTree is the core class of the TreeMaker application. A tmTree object
contains a complete mathematical model of the tmTree, the crease pattern, and
the underlying data structures that support the computation and analysis of
what creases are necessary to fold a given tree.

The tmTree also provides the primary interface for modifying the tmTree. You
don't directly create or destroy nodes, edges, polys, or creases; you call the
appropriate tmTree member functions.

All parts of the tree are descended from class tmPart, which gives them an
index (which is 1-based) and a reference to the tree that they belong to.

Somewhat non-intutively, a tmTree is also a tmPart with an index of 0. We do
this so tmTree can have access to some tmPart stream I/O routines; also
GetTree() returns a pointer to the tree itself.
*/


#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


/*****
Return the direction vector for the symmetry line.
*****/
tmPoint tmTree::GetSymDir() const
{
  return tmPoint(cos(mSymAngle * DEGREES), sin(mSymAngle * DEGREES));
}


/*****
Return the number of editable parts (nodes, edges)
*****/
size_t tmTree::GetNumEditableParts() const
{
  return tmCluster::GetNumEditableParts();
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Change the width of the paper.
*****/
void tmTree::SetPaperWidth(const tmFloat& aWidth)
{
  if (mPaperWidth == aWidth) return;
  tmTreeCleaner tc(this);
  mPaperWidth = aWidth;
}


/*****
Change the height of the paper.
*****/
void tmTree::SetPaperHeight(const tmFloat& aHeight)
{
  if (mPaperHeight == aHeight) return;
  tmTreeCleaner tc(this);
  mPaperHeight = aHeight;
}


/*****
Change the scale of the tree.
*****/
void tmTree::SetScale(const tmFloat& aScale)
{
  if (mScale == aScale) return;
  tmTreeCleaner tc(this);
  mScale = aScale;
}


/*****
Set the symmetry flag. We use a tmTreeCleaner because when we turn on symmetry, that will affect whether any existing conditions are feasible or not.
*****/
void tmTree::SetHasSymmetry(bool aHasSymmetry)
{
  if (mHasSymmetry == aHasSymmetry) return;
  tmTreeCleaner tc(this);
  mHasSymmetry = aHasSymmetry;
}


/*****
Set the symmetry location.
*****/
void tmTree::SetSymLoc(const tmPoint& aSymLoc)
{
  if (mSymLoc == aSymLoc) return;
  tmTreeCleaner tc(this);
  mSymLoc = aSymLoc;
}


/*****
Set the symmetry location x coordinate
*****/
void tmTree::SetSymLocX(const tmFloat& aSymLocX)
{
  if (mSymLoc.x == aSymLocX) return;
  tmTreeCleaner tc(this);
  mSymLoc.x = aSymLocX;
}


/*****
Set the symmetry location y coordinate
*****/
void tmTree::SetSymLocY(const tmFloat& aSymLocY)
{
  if (mSymLoc.y == aSymLocY) return;
  tmTreeCleaner tc(this);
  mSymLoc.y = aSymLocY;
}


/*****
Set the symmetry angle [degrees].
*****/
void tmTree::SetSymAngle(const tmFloat& aSymAngle)
{
  if (mSymAngle == aSymAngle) return;
  tmTreeCleaner tc(this);
  mSymAngle = aSymAngle;
}


/*****
Turn on symmetry and use this location and angle [degrees].
*****/
void tmTree::SetSymmetry(const tmPoint& aSymLoc, const tmFloat& aSymAngle)
{
  if (mHasSymmetry && (mSymLoc == aSymLoc) && (mSymAngle == aSymAngle)) return;
  mHasSymmetry = true;
  mSymLoc = aSymLoc;
  mSymAngle = aSymAngle;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return true if this tree contains any nodes of degree 2 wrt edges.
*****/
bool tmTree::HasRedundantNodes() const
{
  return tmNode::ContainsRedundantNodes(mNodes);
}


/*****
Return true if this tree contains any edges with nonzero strain
*****/
bool tmTree::HasStrainedEdges() const
{
  return tmEdge::ContainsStrainedEdges(mEdges);
}


/*****
Return true if this tree contains any conditions on nodes
*****/
bool tmTree::HasNodeConditions() const
{
  for (size_t i = 0; i < mConditions.size(); ++i)
    if (mConditions[i]->IsNodeCondition()) return true;
  return false;
}


/*****
Return true if this tree contains any conditions on edges
*****/
bool tmTree::HasEdgeConditions() const
{
  for (size_t i = 0; i < mConditions.size(); ++i)
    if (mConditions[i]->IsEdgeCondition()) return true;
  return false;
}


/*****
Return true if this tree has any conditions on paths.
*****/
bool tmTree::HasPathConditions() const
{
  for (size_t i = 0; i < mConditions.size(); ++i)
    if (mConditions[i]->IsPathCondition()) return true;
  return false;
}


/*****
Return true if this tree has any conditions at all. Provided as a convenience.
*****/
bool tmTree::HasConditions() const
{
  return mConditions.not_empty();
}


/*****
Return true if this tree has any polys of order 4 or higher. If this is the
case, then there is a fracturable poly.
*****/
bool tmTree::HasNonTrianglePoly() const
{
  for (size_t i = 0; i < mOwnedPolys.size(); ++i) {
    tmPoly* thePoly = mOwnedPolys[i];
    if (thePoly->GetSize() >= 4) return true;
  }
  return false;
}


/*****
Return the root node, i.e., the node with index 1. That means it's the first
node in the mNodes array -- NOT necessarily the first in the mOwnedNodes array.
*****/
tmNode* tmTree::GetRootNode() const
{
  return mNodes.front();
}


/*****
Return the tree edge that connects these two leaf nodes. If they're not
connected by an edge, return 0.
*****/
tmEdge* tmTree::GetEdge(const tmNode* node1, const tmNode* node2) const
{
  for (size_t i = 0; i < node1->mEdges.size(); ++i) {
    tmEdge* theEdge = node1->mEdges[i];
    if ((theEdge->mNodes.front() == node2) || (theEdge->mNodes.back() == node2))
      return theEdge;
  }
  return 0;
}


/*****
Return the path that connects these two nodes, which can be either leaf or
branch nodes. This is not terribly efficient because it requires a search
through all paths, which is O(N^2).
*****/
tmPath* tmTree::GetPath(const tmNode* node1, const tmNode* node2) const
{
  for (size_t i = 0; i < mPaths.size(); ++i) {
    tmPath* thePath = mPaths[i];
    if ((thePath->mNodes.front() == node1 && thePath->mNodes.back() == node2) ||
      (thePath->mNodes.front() == node2 && thePath->mNodes.back() == node1))
      return thePath;
  }
  TMFAIL("GetPath() failed to find a path between the two nodes.");
  return NULL;
}


/*****
Return the path that connects these two nodes, which must be leaf nodes. This
is considerably more efficient than GetPath() because we only search through a
subset of the total paths, so it's O(N).
*****/
tmPath* tmTree::GetLeafPath(const tmNode* leafNode1, 
  const tmNode* leafNode2) const
{
  TMASSERT(leafNode1->IsLeafNode());
  TMASSERT(leafNode2->IsLeafNode());
  const tmArray<tmPath*>& n1paths = leafNode1->mLeafPaths;
  for (size_t i = 0; i < n1paths.size(); ++i) {
    tmPath* aPath = n1paths[i];
    if (aPath->GetOtherNode(leafNode1) == leafNode2) return aPath;
  }
  TMFAIL("GetLeafPath() failed to find a leaf path between the two nodes.");
  return NULL;
}


/*****
Count the number of leaf nodes.
*****/
size_t tmTree::GetNumLeafNodes() const
{
  size_t numLeafNodes = 0;
  for (size_t i = 0; i < mOwnedNodes.size(); ++i)
    if (mOwnedNodes[i]->IsLeafNode()) ++numLeafNodes;
  return numLeafNodes;
}


/*****
Count the number of unpinned nodes and stretchable edges; return the two in the
passed variables and return the total as the function value.
*****/
size_t tmTree::GetNumMovableParts(size_t& numNodes, size_t& numEdges) const
{
  numNodes = 0;
  for (size_t i = mOwnedNodes.size(); i > 0; --i) {
    tmNode* theNode = mOwnedNodes[i-1];
    if (theNode->IsLeafNode() && !theNode->IsPinnedNode())
      ++numNodes;
  }
  numEdges = 0;
  for (size_t i = mOwnedEdges.size(); i > 0; --i) {
    tmEdge* theEdge = mOwnedEdges[i-1];
    if (!theEdge->IsPinnedEdge() && 
      !mTree->IsConditioned<tmConditionEdgeLengthFixed, tmEdge>(theEdge))
      ++numEdges;
  }
  return numNodes + numEdges;
}


/*****
Count the number of unpinned nodes and stretchable edges; just return the total
as the function value.
*****/
size_t tmTree::GetNumMovableParts() const
{
  size_t numNodes(0), numEdges(0);
  return GetNumMovableParts(numNodes, numEdges);
}


/*****
Return the crease shared by these two facets. If these facets don't share a
crease, return a NULL pointer. Both facets must exist.
*****/
tmCrease* tmTree::GetCrease(tmFacet* facet1, tmFacet* facet2) const
{
  // Rather than searching through all the creases for one that points to both
  // facets, it's more efficient to just examine the 4 creases around one of
  // the facets.
  TMASSERT(facet1);
  TMASSERT(facet2);
  for (size_t i = 0; i < facet1->mCreases.size(); ++i) {
    tmCrease* theCrease = facet1->mCreases[i];
    if (facet2->mCreases.contains(theCrease)) return theCrease;
  }
  return 0;
}


/*****
Return true if corridor information is sufficiently constructed that we can
get the corridor facets associated with an edge.
*****/
bool tmTree::CanGetCorridorFacets() const
{
  return mIsFacetDataValid;
}


/*****
Get the facets that belong to the corridors associated with the given set of
edges. Clients should check CanGetCorridorFacets() before calling this.
*****/
void tmTree::GetCorridorFacets(const tmArray<tmEdge*>& edgeList, 
  tmArray<tmFacet*>& facetList) const
{
  TMASSERT(CanGetCorridorFacets());
  facetList.clear();
  for (size_t i = 0; i < mFacets.size(); ++i) {
    tmFacet* theFacet = mFacets[i];
    if (edgeList.contains(theFacet->mCorridorEdge))
      facetList.push_back(theFacet);
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*  The following routines change the topology of the tree. All changes should
be made by calling one of these routines; you can't create or destroy parts
directly. */


/*****
Add a tmNode to the tree. This routine creates a new tmNode and edge
connected to the tree at fromNode, and returns a reference to both the new
tmNode and new edge created. It also updates all references to which nodes and
paths are leaf nodes and leaf paths.
*****/
void tmTree::AddNode(tmNode* fromNode, const tmPoint& where, 
  tmNode*& newNode, tmEdge*& newEdge)
{
  tmTreeCleaner tc(this);
  
  // If fromNode == NULL then the new tmNode should be the first tmNode in the
  // tree. if fromNode != NULL, then the tree should contain fromNode as an
  // owned node ( not a subnode). Users should check this before calling
  // AddNode().
  TMASSERT((fromNode == NULL) ? (mOwnedNodes.empty()) : \
    (mOwnedNodes.contains(fromNode)));
  
  // Create the new tmNode and add it to the tree.
  newNode = new tmNode(this, this, where);
  newNode->mIsLeafNode = true;
  if (fromNode == NULL) return; // if this was the first tmNode, we're done.
  
  // Now add an edge connecting the newly-created tmNode to the old one.
  newEdge = new tmEdge(this, fromNode, newNode);
  
  // After adding the new tmNode, fromNode will never be a leaf node unless it
  // was the first node and newNode is the second. In this special case, the
  // only leaf path it will have is the one connecting it to newNode, which
  // doesn't get created till later. Thus, we can safely go through all of its
  // paths and make them non-leaf paths.
  fromNode->mIsLeafNode = (mOwnedNodes.size() == 2);
  for (size_t ip = 0; ip < fromNode->mLeafPaths.size(); ip++) 
    fromNode->mLeafPaths[ip]->mIsLeafPath = false;
  fromNode->mLeafPaths.clear();

  // Build all new paths that include this tmNode by cloning old ones that end
  // on fromNode and adding the new tmNode and edge to the end of the cloned
  // path. We'll also take this opportunity to update the tmNode::mLeafPaths
  // list of leaf paths for affected nodes. To avoid wasting time checking
  // newly-created paths, we'll start at the end and work backward toward the
  // beginning.
  tmArrayIterator<tmPath*> iOwnedPaths(mOwnedPaths, tmArray_END);
  tmPath* newPath;
  tmNode* otherNode;
  tmPath* aPath;
  while (iOwnedPaths.Previous(&aPath)) {
    if (aPath->mNodes.front() == fromNode) {
      newPath = new tmPath(this, aPath);
      newPath->mNodes.push_front(newNode);
      newPath->mEdges.push_front(newEdge);
      otherNode = aPath->mNodes.back();
      if (otherNode->IsLeafNode()) {
        if (fromNode->IsLeafNode()) otherNode->mLeafPaths.erase_remove(aPath);
        otherNode->mLeafPaths.push_back(newPath);
        newPath->mIsLeafPath = true;
        newNode->mLeafPaths.push_back(newPath);
      }
    }
    else if (aPath->mNodes.back() == fromNode) {
      newPath = new tmPath(this, aPath);
      newPath->mNodes.push_back(newNode);
      newPath->mEdges.push_back(newEdge);
      otherNode = aPath->mNodes.front();
      if (otherNode->IsLeafNode()) {
        if (fromNode->IsLeafNode()) otherNode->mLeafPaths.erase_remove(aPath);
        otherNode->mLeafPaths.push_back(newPath);
        newPath->mIsLeafPath = true;
        newNode->mLeafPaths.push_back(newPath);
      }
    }
  }
  
  // Also create the path corresponding to the newly created edge.
  newPath = new tmPath(this, newEdge);
  if (fromNode->IsLeafNode()) {
    newPath->mIsLeafPath = true;
    fromNode->mLeafPaths.push_back(newPath);
    newNode->mLeafPaths.push_back(newPath);
  }
}


/*****
Add a new tmNode along an edge, breaking the edge into two new edges. splitLoc
is the distance (in tree units) along the edge, relative to the strained length
of the edge. Return newNode as a reference to the newly-created tmNode.
*****/
void tmTree::SplitEdge(tmEdge* aEdge, const tmFloat& splitLoc, tmNode*& newNode)
{
  tmTreeCleaner tc(this);
  
  // Before we do anything, find any top-level poly whose subtree contains the
  // edge to be split and kill the entire poly, since splitting an edge
  // invalidates the crease pattern within the poly and crease pattern is built
  // as part of poly contents construction.
  tmArray<tmPoly*> affectedPolys;
  for (size_t i = 0; i < mOwnedPolys.size(); ++i) {
    tmPoly* thePoly = mOwnedPolys[i];
    if (thePoly->SubTreeContains(aEdge))
      affectedPolys.push_back(thePoly);
  }
  for (size_t i = 0; i < affectedPolys.size(); ++i)
    delete affectedPolys[i];
 
  // Get references to nodes at each end of the edge to split.
  tmNode* node1 = aEdge->mNodes.front();
  tmNode* node2 = aEdge->mNodes.back();

  // Range-check the split location and throw an exception if the requested
  // location falls outside the edge.
  if ((splitLoc <= 0) || (splitLoc >= aEdge->GetStrainedLength())) 
    throw EX_BAD_SPLIT_EDGE();
  
  // create the tmNode and add it to the tree
  tmPoint where = ((aEdge->GetStrainedLength() - splitLoc) * node1->mLoc +
    splitLoc * node2->mLoc) / aEdge->GetStrainedLength();
  newNode = new tmNode(this, this, where);
  
  // create two new edges connecting this tmNode to the tree with the
  // appropriate lengths. The stiffness and strain are copied from the original
  // edge.
  tmEdge* edge1 = new tmEdge(this, node1, newNode);
  edge1->mLength = splitLoc / (1 + aEdge->mStrain);
  edge1->mStrain = aEdge->mStrain;
  edge1->mStiffness = aEdge->mStiffness;
  
  tmEdge* edge2 = new tmEdge(this, newNode, node2);
  edge2->mLength = (aEdge->GetStrainedLength() - splitLoc) / 
    (1 + aEdge->mStrain);
  edge2->mStrain = aEdge->mStrain;
  edge2->mStiffness = aEdge->mStiffness;
  
  // We need to create all of the new paths that start or stop on the new
  // tmNode. We'll do this by cloning paths that start or stop on one of the
  // end nodes (and that don't contain the splitting edge), then add the new
  // tmNode and the appropriate edge to the path.
  tmArrayIterator<tmPath*> iOwnedPaths(mOwnedPaths);
  tmPath *aPath, *newPath;
  while (iOwnedPaths.Next(&aPath)) {
    tmNode* nodeA = aPath->mNodes.front();
    tmNode* nodeB = aPath->mNodes.back();
    if ((nodeA == node1) || (nodeA == node2) || (nodeB == node1) || 
      (nodeB == node2)) {
      if (aPath->mEdges.contains(aEdge)) continue;
      newPath = new tmPath(this, aPath);
      if (nodeA == node1) {
        newPath->mNodes.push_front(newNode);
        newPath->mEdges.push_front(edge1);
        newPath->mMinTreeLength += edge1->mLength;
      }
      else if (nodeA == node2) {
        newPath->mNodes.push_front(newNode);
        newPath->mEdges.push_front(edge2);
        newPath->mMinTreeLength += edge2->mLength;
      }
      else if (nodeB == node1) {
        newPath->mNodes.push_back(newNode);
        newPath->mEdges.push_back(edge1);
        newPath->mMinTreeLength += edge1->mLength;
      }
      else /* if (nodeB == node2) */ {
        newPath->mNodes.push_back(newNode);
        newPath->mEdges.push_back(edge2);
        newPath->mMinTreeLength += edge2->mLength;
      }
    }
  }
  
  // Also create the new paths between the new tmNode and the endpoints of the
  // split nodes. We can do this simply by use the tmPath(tmEdge*) constructor. 
  newPath = new tmPath(this, edge1);  
  newPath = new tmPath(this, edge2);
  
  // Now clean up references to nodes and edges in existing paths. In each path, 
  // we'll replace any occurence of aEdge with the new edges AND we'll insert
  // the new tmNode into the list of nodes. 
  iOwnedPaths.ResetTo(tmArray_START);
  while (iOwnedPaths.Next(&aPath)) {
    size_t ne = aPath->mEdges.GetIndex(aEdge);
    if (ne == aPath->mEdges.BAD_INDEX) continue;
    size_t nn1 = aPath->mNodes.GetIndex(node1);
    size_t nn2 = aPath->mNodes.GetIndex(node2);
  
    // Now, remove the old edge and replace it with the new ones. The order
    // of replacement depends on the order of the nodes.    
    aPath->mEdges.erase_remove(aEdge);
    
    if (nn2 == nn1 + 1) {
      aPath->mEdges.InsertItemAt(ne, edge1);
      aPath->mEdges.InsertItemAt(ne + 1, edge2);
      aPath->mNodes.InsertItemAt(nn2, newNode);
    }
    else /* if (nn1 == nn2 + 1) */ {
      aPath->mEdges.InsertItemAt(ne, edge2);
      aPath->mEdges.InsertItemAt(ne + 1, edge1);
      aPath->mNodes.InsertItemAt(nn1, newNode);
    }
  }
  
  // Now get rid of the old edge. Use of tmDpptr classes automatically removes
  // potentially dangling references to aEdge.  
  delete aEdge;
}


/*****
Return true if we can absorb this node.
*****/
bool tmTree::CanAbsorbNode(tmNode* aNode) const
{
  return (aNode->IsRedundant());
}


/*****
Absorb a tmNode that joins two edges, creating a single edge in the process.
(This is the opposite of SplitEdge.)
*****/
void tmTree::AbsorbNode(tmNode* aNode, tmEdge*& newEdge)
{
  tmTreeCleaner tc(this);
  
  // Verify that this tmNode satisfies the conditions for absorption; namely,
  // that it's an internal tmNode connected to exactly two edges. 
  if (!CanAbsorbNode(aNode)) throw EX_BAD_ABSORB_NODE();
  
  // Get references to edges at each end of the tmNode and to the nodes at the
  // other ends of the edges. 
  tmEdge* edge1 = aNode->mEdges.front();
  tmEdge* edge2 = aNode->mEdges.back();
  
  tmNode* node1 = edge1->mNodes.front();
  if (node1 == aNode) 
    node1 = edge1->mNodes.back();
  
  tmNode* node2 = edge2->mNodes.front();
  if (node2 == aNode) 
    node2 = edge2->mNodes.back();
  
  // Killing paths, which we will be doing, screws up the internal structure of
  // any polys that contain the path. Rather than trying to identify just those
  // polys that have gotten corrupted, we'll kill all of them (which also wipes
  // out any crease pattern).
  KillPolysAndCreasePattern();
  
  // Create a new edge connecting the nodes surrounding the absorbed tmNode.
  // The new edge is unstrained, but its length is the sum of the strained
  // lengths of the two original edges. 
  newEdge = new tmEdge(this, node1, node2);
  newEdge->mLength = edge1->GetStrainedLength() + edge2->GetStrainedLength();
  
  // Delete all paths that start or stop on the absorbed tmNode
  for (size_t i = mOwnedPaths.size(); i > 0; --i) {
    tmPath* thePath = mOwnedPaths[i - 1];
    if ((thePath->mNodes.front() == aNode) || (thePath->mNodes.back() == aNode))
      delete thePath;
  }
  
  // In any paths that contained both edges, replace one of the edges with the
  // new edge; references to the other edge will be removed (through the magic
  // of tmDpptrArray<T>) when we delete the two edges and the node.
  for (size_t i = 0; i < mOwnedPaths.size(); ++i) {
    tmPath* thePath = mOwnedPaths[i];
    if ((thePath->mEdges.contains(edge1)) && (thePath->mEdges.contains(edge2)))
      thePath->mEdges.replace_with(edge1, newEdge);
  }
  
  // Delete the absorbed node and associated edges. 
  delete aNode;
  delete edge1;
  delete edge2;
}


/*****
Return true if we can absorb all nodes in the list.
*****/
bool tmTree::CanAbsorbNodes(const tmArray<tmNode*>& nodeList) const
{
  for (size_t i = 0; i < nodeList.size(); ++i)
    if (!CanAbsorbNode(nodeList[i])) 
      return false;
  return true;
}


/*****
Absorb all nodes in the list.
*****/
void tmTree::AbsorbNodes(const tmArray<tmNode*>& nodeList)
{
  // Make a local copy of the array. If nodeList happens to be a tmDpptrArray,
  // then as each node is absorbed, it will be removed from the array, causing
  // the array to shrink. Since we can't know if this is happening, we'll have
  // to work from a copy of the list.
  const tmArray<tmNode*> nodeListCopy(nodeList);
  tmTreeCleaner tc(this);
  if (!CanAbsorbNodes(nodeListCopy)) 
    throw EX_BAD_ABSORB_NODE();
  tmEdge* newEdge;
  for (size_t i = 0; i < nodeListCopy.size(); ++i)
    AbsorbNode(nodeListCopy[i], newEdge);
}


/*****
Return true if there are any redundant nodes that can be absorbed.
*****/
bool tmTree::CanAbsorbRedundantNodes() const
{
  return HasRedundantNodes();
}


/*****
Absorb all nodes that only have two edges connected to them.
*****/
void tmTree::AbsorbRedundantNodes()
{
  tmTreeCleaner tc(this);
  for (size_t i = mOwnedNodes.size(); i > 0; --i) {
    tmNode* theNode = mOwnedNodes[i - 1];
    tmEdge* theEdge;
    if (theNode->IsRedundant())
      AbsorbNode(theNode, theEdge);
  }
}


/*****
Return true if this edge can be safely absorbed. Any edge can be absorbed.
*****/
bool tmTree::CanAbsorbEdge(tmEdge*) const
{
  return true;
}


/*****
Absorb this edge, coalescing its two nodes.
*****/
void tmTree::AbsorbEdge(tmEdge* aEdge)
{
  tmTreeCleaner tc(this);
  
  // Pick one node to keep and one to kill.
  tmNode* keepNode = aEdge->mNodes.front();
  tmNode* killNode = aEdge->mNodes.back();
  
  // Killing paths, which we will be doing, screws up the internal structure of
  // any polys that contain the path. Rather than trying to identify just those
  // polys that have gotten corrupted, we'll kill all of them (which also wipes
  // out any crease pattern).
  KillPolysAndCreasePattern();
  
  // First, get rid of superfluous paths, working backwards since
  // we're deleting some items as we go.
  for (size_t i = mOwnedPaths.size(); i > 0; --i) {
    tmPath* thePath = mOwnedPaths[i - 1];
    // Paths that end on killNode are deleted.
    if (thePath->StartsOrEndsWith(killNode))
      delete thePath;
    else if (thePath->mNodes.contains(killNode))
      if (thePath->mNodes.contains(keepNode))
        // Paths that contain killNode and keepNode get the reference to 
        // killNode removed. 
        thePath->mNodes.erase_remove(killNode);
      else
        // Paths that only contain killNode get the reference replaced by
        // a reference to keepNode.
        thePath->mNodes.replace_with(killNode, keepNode);
    // Remove the edge from the path
    if (thePath->mEdges.contains(aEdge))
      thePath->mEdges.erase_remove(aEdge);
  }
  
  // Now for all edges, replace any references to killNode with keepNode.
  // Note that aEdge will end up with two copies of keepNode, but that's not
  // a problem because we'll kill it before we're done.
  for (size_t i = 0; i < mEdges.size(); ++i) {
    tmEdge* theEdge = mEdges[i];
    theEdge->mNodes.replace_with(killNode, keepNode);
  }
  
  // For node keepNode, it should now point to all edges that killNode used
  // to point to. It will end up pointing to aEdge twice, but that's not a
  // problem because when we kill aEdge, all copies will be removed from the
  // list.
  keepNode->mEdges.union_with(killNode->mEdges);

  // Delete the absorbed node and edge.
  delete aEdge;
  delete killNode;
  
  // Update the leaf-ness of the kept node and its incident paths. Note that
  // absorbing an edge can convert a branch node into a leaf node or vice-versa,
  // so any path incident to keepNode could also go from leaf to non-leaf also.
  keepNode->mIsLeafNode = (keepNode->mEdges.size() == 1);
  if (!keepNode->mIsLeafNode)
    keepNode->mLeafPaths.clear();
  for (size_t i = 0; i < mOwnedPaths.size(); ++i) {
    tmPath* thePath = mOwnedPaths[i];
    tmNode* frontNode = thePath->mNodes.front();
    tmNode* backNode = thePath->mNodes.back();
    if (frontNode == keepNode || backNode == keepNode) {
      thePath->mIsLeafPath = (frontNode->mIsLeafNode && backNode->mIsLeafNode);
      if (thePath->mIsLeafPath)
        keepNode->mLeafPaths.union_with(thePath);
      else
        keepNode->mLeafPaths.erase_remove(thePath);
    }
  }
}


/*****
Return true if this list of edges can be safely absorbed.
*****/
bool tmTree::CanAbsorbEdges(const tmArray<tmEdge*>& ) const
{
  return true;
}


/*****
Absorb this list of edges, coalescing each pair of nodes.
*****/
void tmTree::AbsorbEdges(const tmArray<tmEdge*>& edgeList)
{
  // Make a copy first because if edgeList happens to be a tmDpptrArray, as
  // each edge is deleted from the list, the array will shrink by one. Since
  // we can't know if this is happening, we'll just have to work from our own
  // copy of the list.
  const tmArray<tmEdge*> edgeListCopy(edgeList);
  for (size_t i = 0; i < edgeListCopy.size(); ++i)
    AbsorbEdge(edgeListCopy[i]);
}


/*****
Add a stub consisting of a new tmNode and a new edge to the given edge. We must
be careful if the edge is strained; edgeloc is given relative to the strained
length of the edge.
*****/
void tmTree::AddStub(tmEdge* aEdge, const tmFloat& length, 
  const tmFloat& edgeloc, const tmPoint& where)
{
  TMASSERT(edgeloc > 0);
  TMASSERT(edgeloc < aEdge->GetStrainedLength()); 
  tmTreeCleaner tc(this);
  
  // If it's located at the end of an edge, don't create any new nodes. 
  tmNode* newNode;
  tmEdge* newEdge;
  if (IsTiny(edgeloc))  
    AddNode(aEdge->mNodes.front(), where, newNode, newEdge);
  else if (IsTiny(edgeloc - aEdge->GetStrainedLength()))
    AddNode(aEdge->mNodes.back(), where, newNode, newEdge);
  else {
    tmNode* splitNode;
    SplitEdge(aEdge, edgeloc, splitNode); 
    AddNode(splitNode, where, newNode, newEdge);
  }
  newEdge->mLength = length;
}


/*****
Remove a stub (an edge connected to a leaf node) and if it was the only edge
connected to the internal tmNode at the other end, absorb the tmNode as well.
You can only call this for an edge connecting a leaf node to an internal node.
If the internal node has more than 2 edges, it won't be absorbed and newEdge
will be set to NULL.
*****/
void tmTree::RemoveStub(tmEdge* aEdge, tmEdge*& newEdge)
{
  tmTreeCleaner tc(this);
  
  // Make sure that the passed edge is truly a stub
  tmNode* node1 = aEdge->mNodes.front();
  tmNode* node2 = aEdge->mNodes.back();
  tmNode* aNode;  // this is the tmNode left behind
  if (node1->IsLeafNode() && !node2->IsLeafNode()) aNode = node2;
  else if (!node1->IsLeafNode() && node2->IsLeafNode()) aNode = node1;
  else throw EX_BAD_REMOVE_STUB();
  
  // Remove the edge from the tree  
  tmArray<tmNode*> markedNodes;
  tmArray<tmEdge*> markedEdges;
  markedEdges.push_back(aEdge);
  KillSomeNodesAndEdges(markedNodes, markedEdges);
  
  // Check the tmNode left behind; if it only has two edges, then absorb it,
  // but if it has three or more, don't absorb and return NULL for the new
  // edge.  
  if (aNode->mEdges.size() == 2) AbsorbNode(aNode, newEdge);
  else (newEdge = 0);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Remove a number of nodes and edges from this tree. This routine removes the
selected nodes and edges; in addition it removes orphaned nodes and edges,
removes paths that included the nodes or edges and removes altered polygons.
Dangling pointers are avoided by using the tmDpptr<T> and tmDpptrArray<T>
classes within the model to maintain references. You should never explicitly
delete a tmNode or tmEdge that has been attached to a tree or bad things will
happen; use this instead. If the deletion was successful, this routine empties
out both lists passed to it. If the deletion would split the tree, it throws an
EX_BAD_KILL_PARTS exception.
*****/
void tmTree::KillSomeNodesAndEdges(tmArray<tmNode*>& markedNodes,
  tmArray<tmEdge*>& markedEdges)
{
  tmTreeCleaner tc(this);
  
  // First filter the list of nodes: we don't allow the user to delete subnodes.
  tmArray<tmNode*> delNodes;
  for (size_t i = 0; i < markedNodes.size(); ++i) {
    tmNode* theNode = markedNodes[i];
    if (theNode->IsTreeNode()) delNodes.push_back(theNode);
  }
  
  // If a tmNode is killed, then every edge attached to that tmNode must also
  // be killed.
  tmArray<tmEdge*> delEdges = markedEdges;
  for (size_t i = 0; i < delNodes.size(); ++i) {
    tmNode* theNode = delNodes[i];
    for (size_t j = 0; j < theNode->mEdges.size(); ++j) {
      tmEdge* theEdge = theNode->mEdges[j];
      delEdges.union_with(theEdge);
    }
  }
  
  // If any major tmNode has every edge to be killed, then that tmNode should
  // also be killed.
  for (size_t i = 0; i < mOwnedNodes.size(); ++i) {
    tmNode* theNode = mOwnedNodes[i];
    bool saveNode = false;
    for (size_t j = 0; j < theNode->mEdges.size(); ++j) {
      tmEdge* theEdge = theNode->mEdges[j];
      if (!delEdges.contains(theEdge)) {
        saveNode = true;
        break;
      }
    }
    if (!saveNode) delNodes.union_with(theNode);
  }
  
  // Also make a list of all Paths that get killed. Note: when a tmPath is
  // killed, it automatically kills any Polys that it's connected to.
  tmArray<tmPath*> delPaths;
  
  // If any path contains a marked tmNode or tmEdge, then that path should be
  // killed.
  for (size_t i = 0; i < mOwnedPaths.size(); ++i) {
    tmPath* thePath = mOwnedPaths[i];
    if (thePath->mNodes.intersects(delNodes) ||
      thePath->mEdges.intersects(delEdges)) delPaths.push_back(thePath);
  }

  // Check validity of deletion: we don't allow a deletion that would break the
  // tree into two or more pieces.
  size_t nodesLeft = mOwnedNodes.size() - delNodes.size();
  size_t pathsLeft = mOwnedPaths.size() - delPaths.size();
  if ((nodesLeft * (nodesLeft - 1)) / 2 != pathsLeft) throw EX_BAD_KILL_PARTS();
  
  // Delete all of the marked parts.
  for (size_t in = 0; in < delNodes.size(); ++in) delete delNodes[in];
  for (size_t ie = 0; ie < delEdges.size(); ++ie) delete delEdges[ie];
  for (size_t ip = 0; ip < delPaths.size(); ++ip) delete delPaths[ip];
  
  // Now go through the remaining parts and re-set the structural flags, i.e.,
  // the flags that indicate which nodes and paths are leaf. Also
  // Rebuild the lists of leaf paths inside each tmNode.
  for (size_t i = 0; i < mOwnedNodes.size(); ++i) {
    tmNode* theNode = mOwnedNodes[i];
    theNode->mIsLeafNode = (theNode->mEdges.size() == 1);
  }
  for (size_t i = 0; i < mOwnedPaths.size(); ++i) {
    tmPath* thePath = mOwnedPaths[i];
    tmNode* frontNode = thePath->mNodes.front();
    tmNode* backNode = thePath->mNodes.back();
    thePath->mIsLeafPath = (frontNode->IsLeafNode() && backNode->IsLeafNode());
    if (thePath->mIsLeafPath) {
      frontNode->mLeafPaths.union_with(thePath);
      backNode->mLeafPaths.union_with(thePath);
    }
  }
  
  // Clear out the arrays that were passed to us originally
  markedNodes.clear();
  markedEdges.clear();
}


/*****
Kill a bunch of conditions.
*****/
void tmTree::KillSomeConditions(tmArray<tmCondition*>& markedConditions)
{
  tmTreeCleaner tc(this);
  tmArray<tmCondition*> delConditions(markedConditions);
  for (size_t i = 0; i < delConditions.size(); ++i) delete delConditions[i];
}


/*****
Kill all conditions that apply to nodes.
*****/
void tmTree::KillNodeConditions()
{
  tmTreeCleaner tc(this);
  tmArray<tmCondition*> delConditions;
  for (size_t i = 0; i < mConditions.size(); ++i)
    if (mConditions[i]->IsNodeCondition())
      delConditions.push_back(mConditions[i]);
  KillSomeConditions(delConditions);
}


/*****
Kill all conditions that apply to edges.
*****/
void tmTree::KillEdgeConditions()
{
  tmTreeCleaner tc(this);
  tmArray<tmCondition*> delConditions;
  for (size_t i = 0; i < mConditions.size(); ++i)
    if (mConditions[i]->IsEdgeCondition())
      delConditions.push_back(mConditions[i]);
  KillSomeConditions(delConditions);
}


/*****
Kill all conditions that apply to paths.
*****/
void tmTree::KillPathConditions()
{
  tmTreeCleaner tc(this);
  tmArray<tmCondition*> delConditions;
  for (size_t i = 0; i < mConditions.size(); ++i)
    if (mConditions[i]->IsPathCondition())
      delConditions.push_back(mConditions[i]);
  KillSomeConditions(delConditions);
}


/*****
Get rid of all Conditions.
*****/
void tmTree::KillConditions()
{
  KillSomeConditions(mConditions);
}


/*****
Kill nodes, edges, and conditions, which are the objects that might be in a
user selection.
*****/
void tmTree::KillSomeParts(tmArray<tmNode*>& markedNodes, 
  tmArray<tmEdge*>& markedEdges,  tmArray<tmCondition*>& markedConditions)
{
  tmTreeCleaner tc(this);
  KillSomeConditions(markedConditions);
  KillSomeNodesAndEdges(markedNodes, markedEdges);
}


/*****
Kill all elements of the crease pattern.
*****/
void tmTree::KillCreasePattern()
{
  tmTreeCleaner tc(this);
  mVertices.KillItems();
  mCreases.KillItems();
  mFacets.KillItems();
}


/*****
Kill all polys and their substructure, including crease pattern. Note that 
facets (which are owned by polys) will be killed along with their owner polys,
but some of the vertices and creases (owned by nodes and paths, respectively)
may be left behind, so we'll have to kill those, too.
*****/
void tmTree::KillPolysAndCreasePattern()
{
  tmTreeCleaner tc(this);
  KillCreasePattern();
  mPolys.KillItems();
}


/*****
Wipe the tree clean (in preparation for replacing it with another).
*****/
void tmTree::KillAllParts()
{
  tmTreeCleaner tc(this);
  KillConditions();
  KillSomeNodesAndEdges(mOwnedNodes, mOwnedEdges);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*
The following routines are dimensional rather than topological; they change
values or attributes of the tree without changing the numbers of nodes, edges,
paths, etc.
*/


/*****
Return true if this node is eligible to be made the root node; basically, it
must be a tree node that is not already the root node.
*****/
bool tmTree::CanMakeNodeRoot(tmNode* aNode) const
{
  return (aNode->mIndex != 1 && aNode->IsTreeNode());
}


/*****
Make this node the root node. Clients should validate with CanMakeNodeRoot()
before calling.
*****/
void tmTree::MakeNodeRoot(tmNode* aNode)
{
  TMASSERT(CanMakeNodeRoot(aNode));
  SetPartIndex(aNode, 1);
}


/*****
Use this routine to change the index of a condition rather than the template
routine SetPartIndex<P>(P*, size_t).
*****/
void tmTree::SetConditionIndex(tmCondition* c, size_t n)
{
  SetPartIndex(c, n);
}


/*****
Change the location of a group of nodes. This is the only allowed way to make
such a change (typically done in response to a drag operation); it must be
bottlenecked through this routine to insure that cleanup happens. it is safe to
call this routine with an empty array.
*****/
void tmTree::SetNodeLocs(const tmArray<const tmNode*>& movingNodes, 
  const tmArray<tmPoint>& newLocs)
{
  TMASSERT(movingNodes.size() == newLocs.size());
  if (movingNodes.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < movingNodes.size(); ++i) {
    tmNode* theNode = const_cast<tmNode*>(movingNodes[i]);
    theNode->SetLoc(newLocs[i]);
  }
}


/*****
Set a group of edges to have a common length and clear the strain at the same
time.
*****/
void tmTree::SetEdgeLengths(tmArray<tmEdge*>& aEdgeList, tmFloat aLength)
{
  tmTreeCleaner tc(this);
  tmArrayIterator<tmEdge*> iEdgeList(aEdgeList);
  tmEdge* aEdge;
  while (iEdgeList.Next(&aEdge)) {
    aEdge->mLength = aLength;
    aEdge->mStrain = 0;
  }
} 


/*****
Multiply the length of all edges in the list by the given factor, which should
be a positive number.
*****/
void tmTree::ScaleEdgeLengths(const tmArray<tmEdge*>& aEdgeList, 
  const tmFloat& factor)
{
  TMASSERT(factor > 0.0);
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < aEdgeList.size(); ++i)
    aEdgeList[i]->mLength *= factor;
}


/*****
Reduce the scale by scaleFactor and increase each tmEdge length by scaleFactor.
A utility routine used by the next 3 routines.
*****/
void tmTree::ScaleTree(tmFloat scaleFactor)
{
  tmTreeCleaner tc(this); 
  mScale /= scaleFactor;  
  tmArrayIterator<tmEdge*> iEdgeList(mOwnedEdges);
  tmEdge* aEdge;
  while (iEdgeList.Next(&aEdge)) aEdge->mLength *= scaleFactor;
} 


/*****
Renormalize the entire tmTree so that this tmEdge has unit length, changing
both the overall scale and the length of every other tmEdge.
*****/
void tmTree::RenormalizeToUnitEdge(tmEdge* theEdge)
{
  ScaleTree(1/(theEdge->mLength));
} 


/*****
Renormalize the entire tmTree so that this tmPath has unit length, changing
both the overall scale and the length of every tmEdge.
*****/
void tmTree::RenormalizeToUnitPath(tmPath* thePath)
{
  ScaleTree(1/(thePath->mMinTreeLength));
} 


/*****
Renormalize the entire tmTree so that the scale is equal to 1.0, changing both
the overall scale and the length of every tmEdge.
*****/
void tmTree::RenormalizeToUnitScale()
{
  ScaleTree(mScale);
} 


/*****
Set all strains in a list of Edges to zero. Leave the list untouched.
*****/
void tmTree::RemoveStrain(tmArray<tmEdge*>& aEdgeList)
{
  tmTreeCleaner tc(this);
  tmArrayIterator<tmEdge*> iEdgeList(aEdgeList);
  tmEdge* aEdge;
  while (iEdgeList.Next(&aEdge)) aEdge->mStrain = 0;
} 


/*****
Set all strains in the entire tree to zero
*****/
void tmTree::RemoveAllStrain()
{
  RemoveStrain(mOwnedEdges);
} 


/*****
Relieve all strains in a list of Edges by increasing edge lengths proportionally.
*****/
void tmTree::RelieveStrain(tmArray<tmEdge*>& aEdgeList)
{
  tmTreeCleaner tc(this);
  tmArrayIterator<tmEdge*> iEdgeList(aEdgeList);
  tmEdge* aEdge;
  while (iEdgeList.Next(&aEdge)) {
    aEdge->mLength = aEdge->GetStrainedLength();
    aEdge->mStrain = 0;
  }
} 


/*****
Relieve all strains in the entire tmTree  by increasing edge lengths
proportionally.
*****/
void tmTree::RelieveAllStrain()
{
  RelieveStrain(mOwnedEdges);
}


/*****
Perturb the coordinate values of the nodes in the list. This is sometimes
useful when reinitializing a search. In order to provide some repeatability in
testing, we'll always re-seed the random number generator for each call.
*****/
void tmTree::PerturbNodes(const tmArray<tmNode*>& aNodeList)
{
  const tmFloat PERTURBATION_SIZE = 1.0e-2;
  tmTreeCleaner tc(this);
  srand(0);
  for (size_t i = 0; i < aNodeList.size(); ++i) {
    tmNode* theNode = aNodeList[i];
    tmFloat mag = 2 * (tmFloat(rand()) / RAND_MAX) - 0.5; // random w/in (-1, 1)
    theNode->mLoc.x += PERTURBATION_SIZE * mag;
    mag = 2 * (tmFloat(rand()) / RAND_MAX) - 0.5;  // again
    theNode->mLoc.y += PERTURBATION_SIZE * mag;
  }
}


/*****
Return true if we can perturb the entire tree.
*****/
bool tmTree::CanPerturbAllNodes() const
{
  return mOwnedNodes.not_empty();
}


/*****
Perturb the coordinate values of all the tree nodes in the tree.
*****/
void tmTree::PerturbAllNodes()
{
  PerturbNodes(mOwnedNodes);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Construct a list of all the leaf nodes in the tree.
*****/
void tmTree::GetLeafNodes(tmArray<tmNode*>& aNodeList)
{
  aNodeList.clear();
  tmArrayIterator<tmNode*> iOwnedNodes(mOwnedNodes);
  tmNode* aNode;
  while (iOwnedNodes.Next(&aNode)) 
    if (aNode->IsLeafNode()) aNodeList.push_back(aNode);
}


/*****
Construct a list of all the border nodes in the tree.
*****/
void tmTree::GetBorderNodes(tmArray<tmNode*>& aNodeList)
{
  aNodeList.clear();
  tmArrayIterator<tmNode*> iOwnedNodes(mOwnedNodes);
  tmNode* aNode;
  while (iOwnedNodes.Next(&aNode)) 
    if (aNode->IsBorderNode()) aNodeList.push_back(aNode);
}


/*****
Construct a list of all the leaf paths in the tree.
*****/
void tmTree::GetLeafPaths(tmArray<tmPath*>& aPathList)
{
  tmArrayIterator<tmPath*> iOwnedPaths(mOwnedPaths);
  tmPath* aPath;
  while (iOwnedPaths.Next(&aPath)) 
   if (aPath->mIsLeafPath) aPathList.push_back(aPath);
}


/*****
Given a list of nodes, construct the list of Edges that comprise the minimal
subtree that spans the list of nodes. Note: the input nodes may include branch
nodes.
*****/
void tmTree::GetSpanningEdges(const tmArray<tmNode*>& aNodeList, 
  tmArray<tmEdge*>& aEdgeList)
{
  aEdgeList.clear();  // Empty the list.
  
  // Go through all the Paths and if any tmPath spans two of our nodes, add all
  // of its Edges to the list.  
  tmArrayIterator<tmPath*> iPaths(mPaths);
  tmPath* aPath;
  while (iPaths.Next(&aPath))
    if (aNodeList.contains(aPath->mNodes.front()) &&
      (aNodeList.contains(aPath->mNodes.back())))
      aEdgeList.union_with(aPath->mEdges);

}


/*****
Given a list of nodes, construct the list of Paths that span the minimal
subtree that spans the list of nodes. Note: the nodes may include internal
nodes.
*****/
void tmTree::GetSpanningPaths(const tmArray<tmNode*>& aNodeList, 
  tmArray<tmPath*>& aPathList)
{
  aPathList.clear();  // Empty the list.
  
  // Go through all the Paths and if any tmPath spans two of our nodes, add it
  // to the list.
  tmArrayIterator<tmPath*> iPaths(mPaths);
  tmPath* aPath;
  while (iPaths.Next(&aPath))
    if (aNodeList.contains(aPath->mNodes.front()) &&
      (aNodeList.contains(aPath->mNodes.back()))) aPathList.union_with(aPath);

}


/*****
Filter the given list to include only leaf nodes.
*****/
void tmTree::FilterLeafNodes(tmDpptrArray<tmNode>& aNodeList)
{
  for (size_t i = aNodeList.size(); i > 0; --i) {
    tmNode* theNode = aNodeList[i-1];
    if (!theNode->IsLeafNode()) aNodeList.erase_remove(theNode);
  }
}


/*****
Filter the given list to those parts that are allowed to move, i.e. unpinned
leaf nodes and unpinned edges lacking tmConditionEdgeLengthFixed conditions.
*****/
void tmTree::FilterMovableParts(tmDpptrArray<tmNode>& aNodeList, 
  tmDpptrArray<tmEdge>& aEdgeList)
{
  // Filter out non-leaf nodes and pinned nodes.
  for (size_t i = aNodeList.size(); i > 0; --i) {
    tmNode* theNode = aNodeList[i-1];
    if (!theNode->IsLeafNode() || theNode->IsPinnedNode())
      aNodeList.erase_remove(theNode);
  }
  
  // Filter out pinned edges or equivalently, edges that have
  // tmConditionEdgeLengthFixed attached to them.
  for (size_t i = aEdgeList.size(); i > 0; --i) {
    tmEdge* theEdge = aEdgeList[i-1];
    if (theEdge->IsPinnedEdge() || 
      mTree->IsConditioned<tmConditionEdgeLengthFixed, tmEdge>(theEdge))
      aEdgeList.erase_remove(theEdge);
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Turn on fixed to symmetry line for the given list of nodes.
*****/
void tmTree::SetNodesFixedToSymmetryLine(const tmArray<tmNode*>& nodes)
{
  if (nodes.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < nodes.size(); ++i) {
    tmConditionNodeCombo* c = 
      GetOrMakeOnePartCondition<tmConditionNodeCombo, tmNode>(nodes[i]);
    c->SetToSymmetryLine(true);
  }
}


/*****
Turn on fixed to paper edge for the given list of nodes.
*****/
void tmTree::SetNodesFixedToPaperEdge(const tmArray<tmNode*>& nodes)
{
  if (nodes.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < nodes.size(); ++i) {
    tmConditionNodeCombo* c = 
      GetOrMakeOnePartCondition<tmConditionNodeCombo, tmNode>(nodes[i]);
    c->SetToPaperEdge(true);
  }
}


/*****
Turn on fixed to paper corner for the given list of nodes.
*****/
void tmTree::SetNodesFixedToPaperCorner(const tmArray<tmNode*>& nodes)
{
  if (nodes.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < nodes.size(); ++i) {
    tmConditionNodeCombo* c = 
      GetOrMakeOnePartCondition<tmConditionNodeCombo, tmNode>(nodes[i]);
    c->SetToPaperCorner(true);
  }
}


/*****
Turn on fixed to position for the given list of nodes.
*****/
void tmTree::SetNodesFixedToPosition(const tmArray<tmNode*>& nodes, bool xFixed,
  tmFloat xFixValue, bool yFixed, tmFloat yFixValue)
{
  if (nodes.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < nodes.size(); ++i) {
    tmConditionNodeCombo* c = 
      GetOrMakeOnePartCondition<tmConditionNodeCombo, tmNode>(nodes[i]);
    c->SetXFixed(xFixed);
    if (xFixed) 
      c->SetXFixValue(xFixValue);
    c->SetYFixed(yFixed);
    if (yFixed) 
      c->SetYFixValue(yFixValue);
  }
}


/*****
Turn on fixed to symmetry line (v4) for the given list of nodes.
*****/
void tmTree::SetNodesFixedToSymmetryLinev4(const tmArray<tmNode*>& nodes)
{
  if (nodes.empty()) return;
  tmTreeCleaner tc(this);
  tmArray<tmConditionNodeSymmetric*> clist;
  GetOrMakeOnePartCondition<tmConditionNodeSymmetric, tmNode>(nodes, clist);
}


/*****
Turn on fixed to paper edge (v4) for the given list of nodes.
*****/
void tmTree::SetNodesFixedToPaperEdgev4(const tmArray<tmNode*>& nodes)
{
  if (nodes.empty()) return;
  tmTreeCleaner tc(this);
  tmArray<tmConditionNodeOnEdge*> clist;
  GetOrMakeOnePartCondition<tmConditionNodeOnEdge, tmNode>(nodes, clist);
}


/*****
Turn on fixed to paper corner (v4) for the given list of nodes.
*****/
void tmTree::SetNodesFixedToPaperCornerv4(const tmArray<tmNode*>& nodes)
{
  if (nodes.empty()) return;
  tmTreeCleaner tc(this);
  tmArray<tmConditionNodeOnCorner*> clist;
  GetOrMakeOnePartCondition<tmConditionNodeOnCorner, tmNode>(nodes, clist);
}


/*****
Turn on fixed to position (v4) for the given list of nodes.
*****/
void tmTree::SetNodesFixedToPositionv4(const tmArray<tmNode*>& nodes, bool xFixed,
  tmFloat xFixValue, bool yFixed, tmFloat yFixValue)
{
  if (nodes.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < nodes.size(); ++i) {
    tmConditionNodeFixed* c = 
      GetOrMakeOnePartCondition<tmConditionNodeFixed, tmNode>(nodes[i]);
    c->SetXFixed(xFixed);
    if (xFixed) 
      c->SetXFixValue(xFixValue);
    c->SetYFixed(yFixed);
    if (yFixed) 
      c->SetYFixValue(yFixValue);
  }
}


/*****
Set all of the edges in the list to have the same strain.
*****/
void tmTree::SetEdgesSameStrain(const tmArray<tmEdge*>& edges)
{
  if (edges.size() < 2) return;
  tmTreeCleaner tc(this);
  for (size_t i = 1; i < edges.size(); ++i)
    mTree->GetOrMakeTwoPartCondition<tmConditionEdgesSameStrain, tmEdge>(
      edges[0], edges[i]);
}


/*****
Set all of the paths in the list to be active.
*****/
void tmTree::SetPathsActive(const tmArray<tmPath*>& paths)
{
  if (paths.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < paths.size(); ++i)
    GetOrMakeOnePartCondition<tmConditionPathCombo, tmPath>(paths[i]);
}


/*****
Set all of the paths in the list to have fixed path angle
*****/
void tmTree::SetPathsAngleFixed(const tmArray<tmPath*>& paths, tmFloat angle)
{
  if (paths.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < paths.size(); ++i) {
    tmConditionPathCombo* c = 
      GetOrMakeOnePartCondition<tmConditionPathCombo, tmPath>(paths[i]);
    c->SetAngleFixed(true);
    c->SetAngle(angle);
  }
}


/*****
Set all of the paths in the list to have quantized path angle
*****/
void tmTree::SetPathsAngleQuant(const tmArray<tmPath*>& paths, size_t quant,
  tmFloat quantOffset)
{
  if (paths.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < paths.size(); ++i) {
    tmConditionPathCombo* c =
      GetOrMakeOnePartCondition<tmConditionPathCombo, tmPath>(paths[i]);
    c->SetAngleQuant(true);
    c->SetQuant(quant);
    c->SetQuantOffset(quantOffset);
  }
}


/*****
Set all of the paths in the list to be active (v4).
*****/
void tmTree::SetPathsActivev4(const tmArray<tmPath*>& paths)
{
  if (paths.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < paths.size(); ++i)
    GetOrMakeOnePartCondition<tmConditionPathActive, tmPath>(paths[i]);
}


/*****
Set all of the paths in the list to have fixed path angle (v4)
*****/
void tmTree::SetPathsAngleFixedv4(const tmArray<tmPath*>& paths, tmFloat angle)
{
  if (paths.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < paths.size(); ++i) {
    tmConditionPathAngleFixed* c =
      GetOrMakeOnePartCondition<tmConditionPathAngleFixed, tmPath>(paths[i]);
    c->SetAngle(angle);
  }
}


/*****
Set all of the paths in the list to have quantized path angle (v4)
*****/
void tmTree::SetPathsAngleQuantv4(const tmArray<tmPath*>& paths, size_t quant,
  tmFloat quantOffset)
{
  if (paths.empty()) return;
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < paths.size(); ++i) {
    tmConditionPathCombo* c =
      GetOrMakeOnePartCondition<tmConditionPathCombo, tmPath>(paths[i]);
    c->SetAngleQuant(true);
    c->SetQuant(quant);
    c->SetQuantOffset(quantOffset);
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Build all of the tree polygons, but not their contents. Normally not called by
users, but called by tmStubFinder::TriangulateTree(). It is safe to call this
for unoptimized or partially- optimized trees.
*****/
void tmTree::BuildTreePolys()
{
  // BuildTreePolys() relies on the accuracy of various dimensional flags and
  // the validity of any existing polys; for this reason, it should only be
  // called for clean Trees (ones for which CleanupAfterEdit() has been
  // executed).
  TMASSERT(!mNeedsCleanup);
  tmTreeCleaner tc(this);

  // Build new polygons from the network of leaf paths. 
  tmArray<tmPath*> leafPaths;
  GetLeafPaths(leafPaths);
  tmArray<tmNode*> borderNodes;
  GetBorderNodes(borderNodes);
  BuildPolysFromPaths(leafPaths, borderNodes);
  
  // But we'll delete any polygons that are either (a) nonconvex, or (b)
  // enclose another leaf node. Note: this is a problem unique to top-level
  // poly construction. As usual, since we're potentially removing elements of
  // the list, we work backwards.
  tmArray<tmNode*> leafNodes;
  GetLeafNodes(leafNodes);
  for (size_t i = mOwnedPolys.size(); i > 0; --i) {
    tmPoly* aPoly = mOwnedPolys[i - 1];
    if (!aPoly->CalcPolyIsConvex() || aPoly->CalcPolyEnclosesNode(leafNodes))
      delete aPoly;
  }
}


/*****
Rebuild all of the polygons and their polygon contents (subpolys
and the crease pattern). It is safe to call this for unoptimized or partially-
optimized trees.
*****/
void tmTree::BuildPolysAndCreasePattern()
{
#ifdef TMDEBUG
  // Debug builds record tree state at the beginning of this call for dumping
  // in case of assertion.
  extern stringstream& DbgPreBuildCreasesStringStream();
  DbgPreBuildCreasesStringStream().str("");
  Putv5Self(DbgPreBuildCreasesStringStream());
#endif // TMDEBUG

  // Like BuildTreePolys(), this should only be called for clean trees.
  TMASSERT(!mNeedsCleanup);
  
  // Build all the tree polygons (but not their contents). Note that
  // BuildTreePolys() will invoke cleanup when its tmTreeCleaner goes out of
  // scope.
  BuildTreePolys();
  
  // Check that we don't have any underlength edges, because that will screw
  // up crease pattern generation.
  for (size_t i = 0; i < mEdges.size(); ++i) {
    tmEdge* theEdge = mEdges[i];
    if (theEdge->GetStrainedLength() < tmEdge::MIN_EDGE_LENGTH) return;
  }

  // Build all subpolys for the polys that remain. This will also build
  // vertices, facets, and creases. We'll create a new tmTreeCleaner so that
  // we clean up again.
  tmTreeCleaner tc(this);
  for (size_t i = 0; i < mOwnedPolys.size(); ++i)
    mOwnedPolys[i]->BuildPolyContents();
}


/*****
Return true if the given tree has a complete crease pattern with full crease
assignment.
*****/
bool tmTree::HasFullCP()
{
  return mIsPolygonValid && mIsPolygonFilled && mIsVertexDepthValid &&
    mIsFacetDataValid & mIsLocalRootConnectable;
}


/*****
Return a status code indicating whether the given tree has a complete crease
pattern, and if it doesn't, return the offending parts in the referenced lists.
*****/
tmTree::CPStatus tmTree::GetCPStatus(tmArray<tmEdge*>& badEdges, 
  tmArray<tmPoly*>& badPolys, tmArray<tmVertex*>& badVertices, 
  tmArray<tmCrease*>& badCreases, tmArray<tmFacet*>& badFacets)
{
  badEdges.resize(0);
  badPolys.resize(0);
  badVertices.resize(0);
  badFacets.resize(0);
  
  // Check whether any edge lengths are too short and if so, return the list of
  // too-short edges.
  for (size_t i = 0; i < mEdges.size(); ++i) {
    tmEdge* theEdge = mEdges[i];
    if (theEdge->GetStrainedLength() < tmEdge::MIN_EDGE_LENGTH) 
      badEdges.push_back(theEdge);
  }
  if (badEdges.not_empty()) return EDGES_TOO_SHORT;
  
  if (!mIsPolygonValid) {
    // Convex hull is not entirely filled with polygons. Return status code
    // and that's all.
    return POLYS_NOT_VALID;
  }
  
  if (!mIsPolygonFilled) {
    // Some tree polygons are not filled with subpolys. Return the offending
    // polygons in the passed list.
    for (size_t i = 0; i < mOwnedPolys.size(); ++i) {
      tmPoly* thePoly = mOwnedPolys[i];
      if (!thePoly->HasPolyContents())
        badPolys.push_back(thePoly);
    }
    return POLYS_NOT_FILLED;
  }
  
  for (size_t i = 0; i < mOwnedPolys.size(); ++i) {
    tmPoly* thePoly = mOwnedPolys[i];
    if (thePoly->GetNumInactiveBorderPaths() > 1)
      badPolys.push_back(thePoly);
  }
  if (badPolys.not_empty()) {
    // Some tree polygons contain more than one inactive path on their border.
    // This can result in missing hinge creases, which causes problems in
    // several places, including calculations of corridor connectedness.
    return POLYS_MULTIPLE_IBPS;
  }
  
  if (!mIsVertexDepthValid) {
    // Some vertices don't have valid depth information. Return the offending
    // vertices in the passed list.
    for (size_t i = 0; i < mVertices.size(); ++i) {
      tmVertex* theVertex = mVertices[i];
      if (theVertex->mDepth == tmVertex::DEPTH_NOT_SET)
        badVertices.push_back(theVertex);
    }
    return VERTICES_LACK_DEPTH;
  }
  
  if (!mIsFacetDataValid) {
    // Either some facets are not well-formed or some interior vertices are of
    // odd degree, or both. Return the offending facets and vertices in the
    // passed lists.
    for (size_t i = 0; i < mVertices.size(); ++i) {
      tmVertex* theVertex = mVertices[i];
      if (!theVertex->IsBorderVertex() && (theVertex->mCreases.size() % 2) != 0)
        badVertices.push_back(theVertex);
    }
    for (size_t i = 0; i < mFacets.size(); ++i) {
      tmFacet* theFacet = mFacets[i];
      if (!theFacet->IsWellFormed())
        badFacets.push_back(theFacet);
    }
    return FACETS_NOT_VALID;
  }
  
  if (!mIsLocalRootConnectable) {
    CalcWhyNotLocalRootConnectable(badVertices, badCreases);
    return NOT_LOCAL_ROOT_CONNECTABLE;
  }
  
  // Still here? then the pattern must have a complete crease pattern.
  return HAS_FULL_CP;
}


#ifdef __MWERKS__
  #pragma mark -
  #pragma mark --PRIVATE--
#endif


/*****
Common initialization
*****/
void tmTree::InitTree()
{
  mPaperWidth = 1;
  mPaperHeight = 1;
  mScale = 0.2;
  mHasSymmetry = false;
  mSymLoc = tmPoint(0.5, 0.5);
  mSymAngle = 90;
  mIsFeasible = false;
  mIsPolygonValid = false;
  mIsPolygonFilled = false;
  mIsVertexDepthValid = false;
  mIsFacetDataValid = false;
  mIsLocalRootConnectable = false;
  mNeedsCleanup = false;
  
#ifdef TMDEBUG
  mQuitCleanupEarly = false;
#endif // TMDEBUG
}

/*****
Default constructor
*****/
tmTree::tmTree() : 
  tmPart(this)
{
  InitTree();
}


/*****
Creator constructor. Note that the argument is ignored; WE are the tree.
*****/
tmTree::tmTree(tmTree*) : 
  tmPart(this)
{
  InitTree();
}


/*****
Destructor. All parts are automatically destroyed by their owner class
*****/
tmTree::~tmTree()
{
}


/*****
Return a deep copy of this tree.
*****/
tmTree* tmTree::Clone()
{
  stringstream ss;
  PutSelf(ss);
  tmTree* theTree = new tmTree();
  theTree->GetSelf(ss);
  return theTree;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Recalculate the leafness of nodes and paths and each node's individual list of
leaf paths.
TBD: use this one routine in all of the structural editing methods.
*****/
// void tmTree::CalcLeafness()
// {
//   // A node is a leaf node if it has a single incident edge. Branch nodes
//   // get their mLeafPaths list cleared immediately.
//   for (size_t i = 0; i < mOwnedNodes.size(); ++i) {
//     tmNode* theNode = mOwnedNodes[i];
//     theNode->mIsLeafNode = (theNode->mEdges.size() == 1);
//     if (!theNode->mIsLeafNode)
//       theNode->mLeafPaths.clear();
//   }
//   // A path is a leaf path if the nodes at either end are leaf nodes. While
//   // we're looping, we make sure that the nodes at each end have this path in
//   // their lists of leaf paths.
//   for (size_t i = 0; i < mOwnedPaths.size(); ++i) {
//     tmPath* thePath = mOwnedPaths[i];
//     tmNode* frontNode = thePath->mNodes.front();
//     tmNode* backNode = thePath->mNodes.back();
//     thePath->mIsLeafPath = (frontNode->IsLeafNode() && backNode->IsLeafNode());
//     if (thePath->mIsLeafPath) {
//       frontNode->mLeafPaths.union_with(thePath);
//       backNode->mLeafPaths.union_with(thePath);
//     }
//   }
//   // Last bit of cleanup, we have to go through the nodes again and verify that
//   // any paths in the mLeafPaths list of a leaf node really are leaf paths, and
//   // not formerly leaf paths that lost their leaf-ness in whatever editing
//   // action has just transpired.
//   for (size_t i = 0; i < mOwnedNode.size(); ++i) {
//     tmNode* theNode = mOwnedNodes[i];
//     if (theNode->mIsLeafNode)
//       for (size_t j = theNode->mLeafPaths.size(); j > 0; --j) {
//         tmPath* thePath = theNode->mLeafPaths[j - 1];
//         if (!thePath->mIsLeafPath)
//           theNode->mLeafPaths.erase_remove(thePath);
//       }
//   }
// }


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Compute the border nodes (the convex hull) and border paths from the given list
of leaf nodes. Set tmNode::mIsBorderNode and tmPath::mIsBorderPath flags of the
affected nodes and paths.
*****/
void tmTree::CalcBorderNodesAndPaths(tmArray<tmNode*>& leafNodes)
{
  // we need at least 3 nodes present.
  if (leafNodes.size() < 3) return;
  
  // First: we find a tmNode somewhere on the hull. We do this by picking a
  // starting point that lies below and outside the paper; then look for the
  // tmNode for which the angle of the line connecting it to the start point is
  // minimized. This gives us a tmNode and a line tangent to the tmNode and, by
  // extension, to the hull.
  tmPoint startPt = tmPoint(-1., -1.);
  tmFloat startAngle = TWO_PI;
  tmNode* startNode = 0;
  for (size_t i = 0; i < leafNodes.size(); ++i) {
    tmNode* thisNode = leafNodes[i];
    tmFloat thisAngle = Angle(thisNode->mLoc - startPt);
    if (startAngle > thisAngle) {
      startAngle = thisAngle;
      startNode = thisNode;
    }
  }
  TMASSERT(startNode);    // if we didn't find one, something bad has happened.
  tmArray<tmNode*> borderNodes;
  borderNodes.push_back(startNode); // store the first tmNode

  // Now go through and accumulate nodes in the hull. The next tmNode is
  // defined to be the node whose loc makes the smallest angle with the triple
  // consisting of the previous 2 pts. There's a slight complication, though;
  // since nodes are constrained by the sides of the paper, there is a strong
  // possibility that we'll have (nearly) collinear border nodes. Due to
  // roundoff error, one or the other might have a "smaller" angle. To resolve
  // this ambiguity, if two nodes have approximately the same angle, we'll take
  // the one closest to the last node.
  tmNode* prevNode = startNode;
  tmPoint prevPt = startPt;
  tmNode* thisNode = startNode;
  tmPoint thisPt = thisNode->mLoc;
  for (;;) {
    tmNode* bestNode = 0;
    tmFloat bestAngle = TWO_PI;
    tmFloat bestDist = HUGE_VAL;
    for (size_t i = 0; i < leafNodes.size(); ++i) {
      tmNode* theNode = leafNodes[i];
      if (theNode == prevNode || theNode == thisNode) continue;
      tmPoint& thePt = theNode->mLoc;
      tmFloat theAngle = AngleChange(prevPt, thisPt, thePt);
      // A subtle point here. All values of theAngle will lie between 0
      // (straight line) and PI (doubling-back); we want the smallest angle,
      // i.e., the "least bend" at the vertex. However, because of our
      // tolerance of slight concavity, it's possible to obtain an angle change
      // that's actually slightly larger than PI, which therefore wraps around
      // to a value near -PI. These are spurious points; they'll never be part
      // of the convex hull; but we have to manually detect them and throw them
      // away.
      if (theAngle < -PI/2) continue;
      
      tmFloat theDist = Mag(thePt - thisPt);
      if ((theAngle < (bestAngle - ConvexityTol())) || 
        (fabs(theAngle - bestAngle) < ConvexityTol() && theDist < bestDist)) {
        bestNode = theNode;
        bestAngle = theAngle;
        bestDist = theDist;
      }
    }
    TMASSERT(bestNode);
    if (bestNode == startNode) break;
    borderNodes.push_back(bestNode);
    prevNode = thisNode;
    prevPt = thisPt;
    thisNode = bestNode;
    thisPt = thisNode->mLoc;
  }
  
  // Now that we've found all the border nodes, we'll set their flags; also
  // identify the border paths and set their flags as well.
  borderNodes[0]->mIsBorderNode = true;
  for (size_t i = 1; i < borderNodes.size(); ++i) {
    borderNodes[i]->mIsBorderNode = true;
    GetLeafPath(borderNodes[i-1], borderNodes[i])->mIsBorderPath = true;
  }
  GetLeafPath(borderNodes.back(), borderNodes.front())->mIsBorderPath = true;
}


/*****
Set the pinned status of all nodes and edges.
*****/
void tmTree::CalcPinnedNodesAndEdges(tmArray<tmNode*>& leafNodes, 
  tmArray<tmPath*>& leafPaths)
{
  // Note which of the leaf nodes are pinned.
  for (size_t i = 0; i < leafNodes.size(); ++i)
    leafNodes[i]->CalcIsPinnedNode();
  
  // Now identify pinned edges, which are edges that are part of active paths
  // between pinned nodes. We only need to search through the leaf paths.
  for (size_t i = 0; i < leafPaths.size(); ++i) {
    tmPath* aPath = leafPaths[i];
    if ((aPath->IsActivePath()) && (aPath->mNodes.front()->mIsPinnedNode) &&
      (aPath->mNodes.back()->mIsPinnedNode))
      for (size_t j = 0; j < aPath->mEdges.size(); ++j)
        aPath->mEdges[j]->mIsPinnedEdge = true;
  }
}


/*****
Construct the network of polygon nodes, polygon paths, and build the polygons
therefrom; delete polygons that have become invalidated.
*****/
void tmTree::CalcPolygonNetwork(tmArray<tmNode*>& leafNodes, 
  tmArray<tmPath*>& leafPaths)
{
  // 12-3-96. BIG change in the algorithm. This algorithm is much broader than
  // before, and takes into account the realization that border nodes don't
  // have to be pinned and border paths don't have to be active.

  // Identify the polygon network. Initially, a path will be a polygon path if
  // it is (1) active (if it's an interior path) or (2) BOTH border and valid.
  // We only need to check the leaf paths.
  tmPath* aPath;
  tmArrayIterator<tmPath*> iLeafPaths(leafPaths); 
  while (iLeafPaths.Next(&aPath)) aPath->mIsPolygonPath = 
    (aPath->IsActivePath() || 
    (aPath->IsBorderPath() && aPath->IsFeasiblePath()));
    
  // Similarly, a tmNode can be a polygon tmNode if it is (1) pinned or (2) a
  // border tmNode.
  tmArrayIterator<tmNode*> iLeafNodes(leafNodes);
  tmNode* aNode;
  while (iLeafNodes.Next(&aNode))
    if (aNode->mIsPinnedNode || aNode->IsBorderNode())
      aNode->mIsPolygonNode = true;
    
  // But if any nodes are connected to infeasible polygon paths, they cannot be
  // polygon nodes (or pinned, for that matter).  
  iLeafPaths.ResetTo(tmArray_START);
  while (iLeafPaths.Next(&aPath)) if (!aPath->IsFeasiblePath()) {
    aNode = aPath->mNodes.front();
    aNode->mIsPolygonNode = aNode->mIsPinnedNode = false;
    aNode = aPath->mNodes.back();
    aNode->mIsPolygonNode = aNode->mIsPinnedNode = false;
  }
    
  // But a polygon tmNode must also have at least two polygon paths coming from
  // it and a polygon path must connect two polygon nodes. So we will
  // iteratively go through both lists and "snip off" parts that don't satisfy
  // the conditions above until we've settled on a stable network.  
  bool somethingChanged;
  do {
    somethingChanged = false;
    
    // Polygon paths must connect two polygon nodes   
    iLeafPaths.ResetTo(tmArray_START);
    while (iLeafPaths.Next(&aPath)) {
      if (!aPath->IsPolygonPath()) continue;
      if (!(aPath->mNodes.front()->IsPolygonNode() &&
        aPath->mNodes.back()->IsPolygonNode())) {
        aPath->mIsPolygonPath = false;
        somethingChanged = true;
      }
    }
      
    // Polygon nodes must have 2 or more polygon paths        
    iLeafNodes.ResetTo(tmArray_START);
    while (iLeafNodes.Next(&aNode)) {
      if (!aNode->IsPolygonNode()) continue;
      size_t polyPaths = 0;
      tmArrayIterator<tmPath*> j1(aNode->mLeafPaths);
      while (j1.Next(&aPath)) if (aPath->IsPolygonPath()) polyPaths++;
      if (polyPaths < 2) {
        aNode->mIsPolygonNode = false;
        somethingChanged = true;
      }
    }
  } while (somethingChanged);

  // Check to see if any tmPoly has become invalid; if so, kill it. Note that
  // this will also kill all vertices, facets, and creases interior to the
  // poly, but not the vertices and creases on the border of the poly. Note
  // that deletion of a poly will also delete its subpolys, changing the list,
  // so we first collect the doomed, then delete.
  tmArray<tmPoly*> doomedPolys;
  for (size_t i = 0; i < mPolys.size(); ++i) {
    tmPoly* thePoly = mPolys[i];
    if (!thePoly->CalcPolyIsValid()) {
      doomedPolys.push_back(thePoly);
    }
  }
  for (size_t i = 0; i < doomedPolys.size(); ++i) {
    delete doomedPolys[i];
  }
}


/*****
Check for total polygon validity of the tree, i.e., whether we have a complete
network of polygons. Basically, we require that (a) every leaf node is
connected to at least two polygon paths, (b) every non-border polygon path has
both polygons assigned, (c) every border path has one polygon assigned.
*****/
void tmTree::CalcPolygonValidity(tmArray<tmNode*>& leafNodes)
{
  mIsPolygonValid = true;
  for (size_t i = 0; i < leafNodes.size(); ++i)
    if (leafNodes[i]->GetNumPolygonPaths() < 2) {
      mIsPolygonValid = false;
      break;
    }
  if (mIsPolygonValid)
    for (size_t i = 0; i < mOwnedPaths.size(); ++i) {
      tmPath* thePath = mOwnedPaths[i];
      if (!thePath->IsPolygonPath()) continue;
      if (thePath->IsBorderPath()) {
        if (thePath->mFwdPoly == 0 && thePath->mBkdPoly == 0) {
          mIsPolygonValid = false;
          break;
        }
      }
      else if (thePath->mFwdPoly == 0 || thePath->mBkdPoly == 0) {
        mIsPolygonValid = false;
        break;
      }
    }
}


/*****
Seek out and kill any orphan creases or vertices.
*****/
void tmTree::KillOrphanVerticesAndCreases()
{
  // Find and kill orphaned creases. Creases can be owned by polys (if they're
  // in the interior of a poly) or paths (if they're on the boundary of a
  // poly). A crease is orphaned if it's owned by a path, but not incident to
  // any filled polys. In which case it's not doing any good, so we kill it.
  for (size_t i = mCreases.size(); i > 0; --i) {
    tmCrease* theCrease = mCreases[i - 1];
    tmPoly* thePoly = theCrease->GetOwnerAsPoly();
    if (thePoly) continue;
    tmPath* thePath = theCrease->GetOwnerAsPath();
    if (thePath) {
      if (thePath->IsSubPath()) continue;
      if (thePath->IsIncidentToFilledPoly()) continue;
    }
    delete theCrease;
  }
  
  // Find and kill orphaned vertices. Vertices are orphaned under three cases,
  // described below.
  for (size_t i = mVertices.size(); i > 0; --i) {
    tmVertex* theVertex = mVertices[i - 1];
    tmNode* theNode = theVertex->GetOwnerAsNode();
    if (theNode) {
      if (theNode->IsSubNode()) continue;
      if (theNode->IsBranchNode()) {
        // If the vertex belongs to a branch node (which happens if a leaf node
        // owned a vertex but we added another node to the tree, converting the
        // leaf node to a branch node). 
        delete theVertex;
        continue;
      }
      // If the vertex belongs to a leaf node, it's an orphan if it is not
      // incident to any filled polygon (just like orphan creases). 
        bool keep = false;
        for (size_t j = 0; j < theNode->mLeafPaths.size(); ++j) {
          tmPath* thePath = theNode->mLeafPaths[j];
          if (thePath->IsIncidentToFilledPoly()) {
            keep = true;
            break;
          }
        }
        if (keep) continue;
      }
    // If the vertex is owned by a path, then the path must be incident to a
    // filled poly.
    tmPath* thePath = theVertex->GetOwnerAsPath();
    if (thePath) {
      if (thePath->IsSubPath()) continue;
      if (thePath->IsIncidentToFilledPoly()) continue;
    }
    delete theVertex;
  }
}


/*****
Renumber all of the parts of the tree.
*****/
void tmTree::CalcPartIndices()
{
  RenumberParts<tmNode>();
  RenumberParts<tmEdge>();
  RenumberParts<tmPath>();
  RenumberParts<tmPoly>();
  RenumberParts<tmVertex>();
  RenumberParts<tmCrease>();
  RenumberParts<tmFacet>();
  RenumberParts<tmCondition>();
}


/*****
Check whether all polygons are filled with contents (subpolys and creases).
Return false if any polygon is not filled, or if there are no polygons at all.
*****/
void tmTree::CalcPolygonFilled()
{
  mIsPolygonFilled = false;
  if (mOwnedPolys.empty()) return;
  for (size_t i = 0; i < mOwnedPolys.size(); ++i) {
    tmPoly* thePoly = mOwnedPolys[i];
    if (!thePoly->HasPolyContents()) return;
  }
  mIsPolygonFilled = true;
}


/*****
Calculate the mDepth field for tree nodes and all vertices.
mDepth is the distance from the root node along the tree, and translates into
position along the axis in the folded base. If we were successful, calculate the bend status for all creases.
*****/
void tmTree::CalcDepthAndBend()
{
  if (!mIsPolygonValid) return;
  if (mNodes.empty()) return;
  
  // Set the depth of all tree nodes (which includes both leaf and branch
  // nodes, but not subnodes). First the root node, which, by definition, has a
  // depth of zero; then the other tree nodes, for whom the depth is the
  // length of the path to the root node.
  tmNode* rootNode = GetRootNode();
  rootNode->mDepth = 0;
  for (size_t i = 0; i < mOwnedPaths.size(); ++i) {
    tmPath* thePath = mOwnedPaths[i];
    tmNode* theNode;
    if (thePath->mNodes.front() == rootNode) 
      theNode = thePath->mNodes.back();
    else if (thePath->mNodes.back() == rootNode) 
      theNode = thePath->mNodes.front();
    else continue;
    theNode->mDepth = thePath->GetMinPaperLength();
  }
  
  // Reset the depth of every path.
  for (size_t i = 0; i < mPaths.size(); ++i) {
    tmPath* thePath = mPaths[i];
    thePath->mMinDepth = DEPTH_NOT_SET;
    thePath->mMinDepthDist = 0;
  }
    
  // For each leaf path set mMinDepth and mMinDepthDist. This establishes a
  // local depth metric whenever the path, or an inset version of it, becomes
  // active, so we can determine the depth of any point relative to the path.
  for (size_t i = 0; i < mPaths.size(); ++i) {
    tmPath* thePath = mPaths[i];
//    if (thePath->mIsSubPath && thePath->mIsLeafPath) {
    if (thePath->IsLeafPath()) {
      thePath->mMinDepth = thePath->mNodes.front()->mDepth;
      thePath->mMinDepthDist = 0;
      for (size_t j = 1; j < thePath->mNodes.size(); ++j) {
        tmNode* pathNode = thePath->mNodes[j];
        tmFloat nodeDepth = pathNode->mDepth;
        if (thePath->mMinDepth > nodeDepth) {
          thePath->mMinDepth = nodeDepth;
          thePath->mMinDepthDist += 
            thePath->mEdges[j - 1]->GetStrainedScaledLength();
        }
      }
    }
  }
  
  // For each gusset path, set mMinDepth and mMinDepthDist from the maximally
  // outset path, which would be one of the leaf paths.
  for (size_t i = 0; i < mPaths.size(); ++i) {
    tmPath* thePath = mPaths[i];
    if (!thePath->IsGussetPath()) continue;
    tmPath* maxOutsetPath;
    tmFloat maxFrontReduction, maxBackReduction;
    thePath->GetMaxOutsetPath(maxOutsetPath, maxFrontReduction, 
      maxBackReduction);
    thePath->mMinDepth = maxOutsetPath->mMinDepth;
    thePath->mMinDepthDist = maxOutsetPath->mMinDepthDist - maxFrontReduction;
  }
  
  // Reset the depth of every vertex.
  for (size_t i = 0; i < mVertices.size(); ++i)
    mVertices[i]->mDepth = DEPTH_NOT_SET;
  
  // Go through active axial and gusset paths in each poly and assign depth to
  // ridgeline and path-owned vertices.
  for (size_t i = 0; i < mPolys.size(); ++i) {
    tmPoly* thePoly = mPolys[i];
    size_t nn = thePoly->mRingNodes.size();
    for (size_t j = 0; j < nn; ++j) {
      tmNode* frontNode = thePoly->mRingNodes[j];
      tmNode* backNode = thePoly->mRingNodes[(j + 1) % nn];
      tmPath* thePath = thePoly->mRingPaths[j];
      if (!(thePath->IsActiveAxialPath() || thePath->IsGussetPath())) continue;
      tmArray<tmVertex*> ridgeVertices;
      thePoly->GetRidgelineVertices(frontNode, backNode, ridgeVertices);
      // Assign depth to ridgeline vertices
      for (size_t k = 0; k < ridgeVertices.size(); ++k)
        thePath->SetVertexDepth(ridgeVertices[k]);
      // Assign depth to path vertices
      for (size_t k = 0; k < thePath->mOwnedVertices.size(); ++k)
        thePath->SetVertexDepth(thePath->mOwnedVertices[k]);
    }
  }
  // Go through inactive border paths and assign depth of the base vertices
  // from the vertex at the upper end of all hinge creases.
  for (size_t i = 0; i < mOwnedPaths.size(); ++i) {
    tmPath* thePath = mOwnedPaths[i];
    if (!thePath->IsBorderPath() || thePath->IsActivePath()) continue;
    for (size_t j = 0; j < thePath->mOwnedVertices.size(); ++j) {
      tmVertex* theVertex = thePath->mOwnedVertices[j];
      for (size_t k = 0; k < theVertex->mCreases.size(); ++k) {
        tmCrease* theCrease = theVertex->mCreases[k];
        if (theCrease->IsHingeCrease()) {
          tmVertex* ridgeVertex = theCrease->GetOtherVertex(theVertex);
          theVertex->mDepth = ridgeVertex->mDepth;
          break;
        }
      }
    }
  }

#if 0
// We're turning off this calculation so that border node vertices with no
// incident active paths do NOT get their depth set.

  // It occasionally occurs that both paths incident on a border node are not
  // active, which means that we were never able to assign a depth to the
  // node's vertex. However, let's look at the creases incident on the vertex;
  // if the vertex at the other end of the crease has a depth, we can compute
  // the depth appropriate for the given vertex from the distance to the vertex
  // at the other end of the crease. This is the one situation where a node's
  // own vertex has a depth NOT equal to the depth of the node. This trick only
  // works for polys with no two consecutive inactive border paths.
  for (size_t i = 0; i < mOwnedNodes.size(); ++i) {
    tmNode* theNode = mOwnedNodes[i];
    if (!theNode->IsBorderNode()) continue;
    tmVertex* theUnrVertex = theNode->GetVertex();
    if (!theUnrVertex) continue;
    if (theUnrVertex->mDepth == DEPTH_NOT_SET) {
      for (size_t j = 0; j < theUnrVertex->mCreases.size(); ++j) {
        tmCrease* theCrease = theUnrVertex->mCreases[j];
        tmVertex* otherVertex = theCrease->GetOtherVertex(theUnrVertex);
        if (otherVertex->mDepth != DEPTH_NOT_SET) {
          tmFloat dist = Mag(theUnrVertex->mLoc - otherVertex->mLoc);
          if (theNode == rootNode)
            theUnrVertex->mDepth = otherVertex->mDepth - dist;
          else
            theUnrVertex->mDepth = otherVertex->mDepth + dist;
        }
        if (theUnrVertex->mDepth != DEPTH_NOT_SET) break;
      }
      if (theUnrVertex->mDepth != DEPTH_NOT_SET) break;
    }
  }
#endif // 0
  
  // The preceding algorithm assigns a numerical value of the depth to every
  // vertex in the crease pattern; this value is used to compute the folded
  // form. But there's also the discrete depth, which is the graph theoretic
  // depth within the rooted tree. This value is only meaningful for vertices 
  // whose projection is a tree node (i.e., with non-NULL mTreeNode). We
  // use discrete depth in several places -- notably to compute bend -- and so 
  // we compute it here.
  for (size_t i = 0; i < mVertices.size(); ++i) {
   tmVertex* theVertex = mVertices[i];
   if (theVertex->mTreeNode) theVertex->mDiscreteDepth = 
     theVertex->mTreeNode->CalcDiscreteDepth();
   else
     theVertex->mDiscreteDepth = size_t(-1);
  }
  
  // If there were two consecutive inactive border paths, then the vertex
  // between them never got its depth assigned. If any vertices are undepthed,
  // we can't go any farther in computing the bend status or the crease
  // pattern. So we'll return.
  for (size_t i = 0; i < mVertices.size(); ++i)
    if (mVertices[i]->mDepth == DEPTH_NOT_SET) return;
  
  // And with depth in place, we can now calculate the crease bend, that is,
  // which creases are folded.
  for (size_t i = 0; i < mOwnedPolys.size(); ++i)
    mOwnedPolys[i]->CalcBend();
}


/*****
Calculate the vertex depth validity; basically, check to make sure we've
managed to assign a depth to every vertex. If we have no vertices at all,
then we're not valid.
*****/
void tmTree::CalcVertexDepthValidity()
{
  mIsVertexDepthValid = false;
  if (mVertices.empty()) return;
  for (size_t i = 0; i < mVertices.size(); ++i)
    if (mVertices[i]->mDepth == DEPTH_NOT_SET) return;
  mIsVertexDepthValid = true;
}


/*****
Calculate the facet data validity: basically, that the facets are well-formed
and the crease pattern is two-colorable.
*****/
void tmTree::CalcFacetDataValidity()
{
  mIsFacetDataValid = false;
  if (mFacets.empty()) return;
  
  for (size_t i = 0; i < mFacets.size(); ++i) {
    tmFacet* theFacet = mFacets[i];
    if (!theFacet->IsWellFormed()) return;
  }
  
  for (size_t i = 0; i < mVertices.size(); ++i) {
    tmVertex* theVertex = mVertices[i];
    if (theVertex->IsBorderVertex()) continue;
    if ((theVertex->mCreases.size() % 2) != 0) return;
  }
  mIsFacetDataValid = true;
}


/*****
Compute the facet corridor edges for the tree.
*****/
void tmTree::CalcFacetCorridorEdges()
{
  TMASSERT(mIsFacetDataValid);
  for (size_t i = 0; i < mOwnedPolys.size(); ++i) {
    tmPoly* thePoly = mOwnedPolys[i];
    thePoly->CalcFacetCorridorEdges();
  }
}


/*****
Compute the color for all facets. Each facet gets a color assignment,
either WHITE_UP or COLOR_UP. We set one facet and let the result propagate
throughout the crease pattern.
*****/
void tmTree::CalcFacetColor()
{
  // Initialize by clearing all the facet colors and recording the sole source
  // facet.
  tmFacet* sourceFacet = 0;
  for (size_t i = 0; i < mFacets.size(); ++i) {
    tmFacet* theFacet = mFacets[i];
    theFacet->mColor = tmFacet::NOT_ORIENTED;
    if (theFacet->IsSourceFacet()) sourceFacet = theFacet;
  }
  TMASSERT(sourceFacet);
  
  // This facet will always be color up. The rest of the facets will have their
  // color set by recursive propagation of color across creases.
  sourceFacet->CalcColor(tmFacet::COLOR_UP);
}


/*****
Compute the crease fold direction.
*****/
void tmTree::CalcFoldDirections()
{
  for (size_t i = 0; i < mCreases.size(); ++i)
    mCreases[i]->CalcFold();
}


/*****
This gets called by the tmTreeCleaner class after any changes to the tree
topology or changes to part attributes. It does the following:
Clamp all tmNode positions to lie within the bounds of the paper
Recompute the lengths of all of the Paths
Updates dimensional flags of nodes, Edges, and Paths (topological flags are set
by the routines that directly alter the topology of the tree).
Delete parts that have become invalid (polys and their sub-structure, vertices, 
creases, and conditions with invalid references)
Renumber all the indices of all the tmParts
Recalculate the depth for nodes and vertices.
*****/
void tmTree::CleanupAfterEdit()
{
#ifdef TMDEBUG
  // Debug builds record tree state at the beginning of this call for dumping
  // in case of assertion.
  extern stringstream& DbgPreCleanupStringStream();
  DbgPreCleanupStringStream().str("");
  Putv5Self(DbgPreCleanupStringStream());
  
  // A first consistency check compares number of owned nodes against owned
  // paths.
  size_t numOwnedNodes = mOwnedNodes.size();
  size_t numOwnedPaths = mOwnedPaths.size();
  if (numOwnedNodes > 0)
    TMASSERT(numOwnedPaths == (numOwnedNodes * (numOwnedNodes - 1)) / 2);
#endif // TMDEBUG

  // Clear flags that should get set later in this routine but might not if
  // we bail out early.
  mIsFeasible = false;
  mIsPolygonValid = false;
  mIsPolygonFilled = false;
  mIsVertexDepthValid = false;
  mIsFacetDataValid = false;
  
  // Clear any Conditions that have become invalid (e.g., because the parts
  // they refer to were deleted). Since deleting removes elements from the
  // list, we'll work from a copy of mConditions.
  tmArray<tmCondition*> clist(mConditions);
  for (size_t ic = 0; ic < clist.size(); ic++)
    if (!clist[ic]->IsValidCondition()) delete clist[ic];
    
  // if there are no nodes, there's nothing else to check.
  if (mOwnedNodes.empty()) return;
  
  // Make a list of all leaf nodes
  tmArray<tmNode*> leafNodes;
  GetLeafNodes(leafNodes);
  
  // Clamp the location of each tmNode to be within the square
  // and clear all the dimensional flags on nodes.
  for (size_t in = 0; in < mOwnedNodes.size(); in++) {
    tmNode* theNode = mOwnedNodes[in];  
    if (theNode->mLoc.x < 0) theNode->mLoc.x = 0;
    if (theNode->mLoc.y < 0) theNode->mLoc.y = 0;
    if (theNode->mLoc.x > mPaperWidth) theNode->mLoc.x = mPaperWidth;
    if (theNode->mLoc.y > mPaperHeight) theNode->mLoc.y = mPaperHeight;
    theNode->mIsBorderNode = false;
    theNode->mIsPinnedNode = false;
    theNode->mIsPolygonNode = false;
    theNode->mIsConditionedNode = false;
  }
  
  // Clear all dimensional flags on edges.
  for (size_t ie = 0; ie < mOwnedEdges.size(); ie++) {
    tmEdge* theEdge = mOwnedEdges[ie];
    theEdge->mIsPinnedEdge = false;
    theEdge->mIsConditionedEdge = false;
  }
    
  // make a list of all the leaf paths  
  tmArray<tmPath*> leafPaths;
  GetLeafPaths(leafPaths);
  
  // Go through all paths and set which paths are feasible and active. Feasible
  // paths are paths whose actual length is greater or equal to their minimum
  // length. Active paths are those for which equality holds. Only need to do 
  // this for owned paths, because polys will set the relevant flags for their
  // subpolys at construction (and any change to a poly wipes its contents).
  tmArrayIterator<tmPath*> iOwnedPaths(mOwnedPaths);
  tmPath* aPath;
  while (iOwnedPaths.Next(&aPath)) {
  
    // compute the length of each path based on its edges and any strain that
    // is present; also set the flags for validity and activity, which depend
    // on these lengths.
    aPath->TreePathCalcLengths();
    
    // Also clear flags we'll be setting shortly    
    aPath->mIsBorderPath = false;
    aPath->mIsPolygonPath = false;
    aPath->mIsConditionedPath = false;
  }
  
  // With path feasibility set, we can now set the feasibility of the entire
  // tree, which basically requires that all leaf paths and conditions be
  // feasible.
  mIsFeasible = true;
  for (size_t i = 0; i < leafPaths.size(); ++i) {
    tmPath* thePath = leafPaths[i];
    mIsFeasible &= thePath->IsFeasiblePath();
    if (!mIsFeasible) break;
  }
  for (size_t i = 0; i < mOwnedConditions.size(); ++i) {
    tmCondition* theCondition = mOwnedConditions[i];
    theCondition->CalcFeasibility();
    mIsFeasible &= theCondition->IsFeasibleCondition();
  }
  
  // Set the mIsConditioned flags by going through all tmParts and Conditions
  // and checking to see if the tmCondition uses the tmPart.
  for (size_t ic = 0; ic < mConditions.size(); ic++) {
    tmCondition* c = mConditions[ic];
    for (size_t in = 0; in < mOwnedNodes.size(); in++)
      mOwnedNodes[in]->mIsConditionedNode |= (c->Uses(mOwnedNodes[in]));
    for (size_t ie = 0; ie < mOwnedEdges.size(); ie++)
      mOwnedEdges[ie]->mIsConditionedEdge |= (c->Uses(mOwnedEdges[ie]));
    for (size_t ip = 0; ip < mOwnedPaths.size(); ip++)
      mOwnedPaths[ip]->mIsConditionedPath |= (c->Uses(mOwnedPaths[ip]));
  }
      
  // Find the border nodes, which comprise the convex hull of the set of nodes,
  // and the border paths, which connect them.
  CalcBorderNodesAndPaths(leafNodes);

  // Compute pinned status for nodes and edges.
  CalcPinnedNodesAndEdges(leafNodes, leafPaths);
  
  // Construct the polygon network and associated flags on nodes and paths.
  CalcPolygonNetwork(leafNodes, leafPaths);
    
  // Calculate polygon validity; basically, insuring that the convex hull of the
  // nodes is entirely filled with valid polygons.
  CalcPolygonValidity(leafNodes);
  
  // Seek out and kill any "orphan" vertices. These would be vertices owned
  // by tree nodes or by axial paths that aren't needed by any filled polygon.
  KillOrphanVerticesAndCreases();
  
  // Depth and bend will require that the root node be a tree node. No matter
  // what the user may have done with SetPartIndex<tmNode>(..), we'll move the
  // tree node with the lowest index to the first position in mNodes so that it
  // will end up as the root node in what follows and after renumbering.
  for (size_t i = 0; i < mNodes.size(); ++i) {
    tmNode* theNode = mNodes[i];
    if (theNode->IsTreeNode()) {
      if (i == 0) break;
      mNodes.MoveItem(i + 1, 1);
      break;
    }
  }

  // Part construction/destruction is done. Renumber all part indices.
  CalcPartIndices();
  
  // Clear crease pattern data that will get recalculated later on if we don't
  // bail out first.
  for (size_t i = 0; i < mVertices.size(); ++i)
    mVertices[i]->ClearCleanupData();
  for (size_t i = 0; i < mCreases.size(); ++i)
    mCreases[i]->ClearCleanupData();
  for (size_t i = 0; i < mFacets.size(); ++i)
    mFacets[i]->ClearCleanupData();
  // Our own flags were already cleared (up at the beginning of the routine).
  
  // Make sure that *all* tree polygons have contents (subpolys and creases);
  // if not, there's no point to trying to compute depth, bend, or any of the
  // other attributes of crease patterns.
  CalcPolygonFilled();
  if (!mIsPolygonFilled) return;
  
  // Recalculate depth for all nodes and vertices and bend for all vertices and
  // creases.
  CalcDepthAndBend();
  
  // Calculate vertex depth validity. If it's not valid, then we're done.
  CalcVertexDepthValidity();
  if (!mIsVertexDepthValid) return;
  
  // Check that all facets are well-formed and the crease pattern is
  // two-colorable. If not, then we can't proceed any farther with computing
  // facet ordering, color, or crease assignment.
  CalcFacetDataValidity();
  if (!mIsFacetDataValid) return;

  // Just in debug builds, we have the option to quit cleanup at this point,
  // before we try to calculate the facet order -- which is a very complex
  // process! So in debug builds, the model supports a runtime switch to bail
  // out at this point. But release versions should not allow this possibility.
#ifdef TMDEBUG
  if (mQuitCleanupEarly) return;
#endif

  // For every facet, find the edge corresponding to the corridor that this
  // facet is part of.
  CalcFacetCorridorEdges();
    
  // Once we have depth and bend and assurance that the facets are well-formed
  // and the crease pattern is two-colorable, we can proceed with the
  // construction of the facet ordering graph and the calculation of facet
  // order values.
  CalcFacetOrder();
  if (!mIsLocalRootConnectable) return;

  // Compute the color orientation of all of the facets. This needs facet order
  // only to get the identity of the source facet in the facet ordering graph.
  CalcFacetColor();
  
  // Compute the fold direction for all of the creases. This information comes
  // from the facet color and the facet order for the facets on either side of
  // each crease.
  CalcFoldDirections();
}

/*******************************************************************************
File:         tmNode.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmNode
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-25
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmNode.h"
#include "tmModel.h"

using namespace std;

/**********
class tmNode
**********/

/*****
Common initialization
*****/
void tmNode::InitNode()
{
  // Register with tmTree
  mTree->mNodes.push_back(this);
  mIndex = mTree->mNodes.size();

  // Initialize member data
  strcpy(mLabel, "");
  mLoc = tmPoint(0. ,0.);
  mDepth = DEPTH_NOT_SET;
  mElevation = 0.0;

  // Initialize flags
  mIsLeafNode = false;
  mIsSubNode = false;
  mIsPinnedNode = false;
  mIsBorderNode = false;
  mIsPolygonNode = false;
  mIsJunctionNode = false;
  mIsConditionedNode = false;
  
  // Clear owner
  mNodeOwner = 0;
}


/*****
Bare constructor for tmNode, used in stream I/O.
*****/
tmNode::tmNode(tmTree* aTree)
  : tmPart(aTree), tmDpptrTarget()
{
  InitNode();
}


/*****
Full constructor for tmNode
*****/
tmNode::tmNode(tmTree* aTree, tmNodeOwner* aNodeOwner, const tmPoint& aLoc)
  : tmPart(aTree), tmDpptrTarget()
{
  InitNode();

  // Register with Owner
  mNodeOwner = aNodeOwner;
  aNodeOwner->mOwnedNodes.push_back(this);

  // Initialize member data
  mLoc = aLoc;

  // Initialize flags
  mIsSubNode = !GetOwnerAsTree();
}


/*****
Return the owner as a tmTree* if it is one and as 0 otherwise.
*****/
tmTree* tmNode::GetOwnerAsTree()
{
  return mNodeOwner->NodeOwnerAsTree();
}


/*****
Return the owner as a tmPoly* if it is one and as 0 otherwise.
*****/
tmPoly* tmNode::GetOwnerAsPoly()
{
  return mNodeOwner->NodeOwnerAsPoly();
}


/*****
Change the label. Users should do their own range-checking on the length of the
string, but if they don't, the actual label will be truncated to fit available
space.
*****/
void tmNode::SetLabel(const char* aLabel)
{
  TMASSERT(strlen(aLabel) <= MAX_LABEL_LEN);
  strncpy(mLabel, aLabel, MAX_LABEL_LEN);
}


/*****
Change the location of a node.
*****/
void tmNode::SetLoc(const tmPoint& aLoc)
{
  if (mLoc == aLoc) return;
  tmTreeCleaner tc(mTree);
  mLoc = aLoc;
}


/*****
Change the x-location of a node.
*****/
void tmNode::SetLocX(const tmFloat& ax)
{
  if (mLoc.x == ax) return;
  tmTreeCleaner tc(mTree);
  mLoc.x = ax;
}


/*****
Change the y-location of a node.
*****/
void tmNode::SetLocY(const tmFloat& ay)
{
  if (mLoc.y == ay) return;
  tmTreeCleaner tc(mTree);
  mLoc.y = ay;
}


/*****
Return true if this node has exactly two incident edges.
*****/
bool tmNode::IsRedundant() const
{
  return mEdges.size() == 2;
}


/*****
Return the number of hops from this node to the root node. This is a useful
metric for determining the local root hinge within a molecule. This should only
be called for a tree node.
*****/
size_t tmNode::CalcDiscreteDepth() const 
{
  TMASSERT(IsTreeNode());
  if (mTree->GetRootNode() == this) return 0;
  return mTree->GetPath(mTree->GetRootNode(), this)->GetEdges().size();
} 


/*****
Return the number of active leaf paths incident on this node. Works for both
tree nodes and sub nodes.
*****/
size_t tmNode::GetNumActivePaths() const
{
  size_t n = 0;
  for (size_t i = 0; i < mLeafPaths.size(); ++i)
    if (mLeafPaths[i]->IsActivePath()) ++n;
  return n;
}


/*****
STATIC
Return true if the given list contains any redundant nodes.
*****/
bool tmNode::ContainsRedundantNodes(const tmArray<tmNode*>& nlist)
{
  for (size_t i = 0; i < nlist.size(); ++i)
    if (nlist[i]->IsRedundant()) return true;
  return false;
}


/*****
STATIC
Copy all leaf nodes from srcList into dstList.
*****/
void tmNode::FilterLeafNodes(tmArray<tmNode*>& dstList, 
  const tmArray<tmNode*>& srcList)
{
  dstList.clear();
  for (size_t i = 0; i < srcList.size(); ++i)
    if (srcList[i]->IsLeafNode()) dstList.push_back(srcList[i]);
}


/*****
Set the value of the mIsPinnedNode flag, which depends on the angles of the
active paths around the node and/or whether it is constrained by the edge of
the paper.
*****/
void tmNode::CalcIsPinnedNode()
{
  mIsPinnedNode = false;        // false to begin with

  // First, count the active paths coming from this tmNode and make a list of
  // the angles of the paths.  
  tmArray<tmFloat> angles;
  for (size_t i = 0; i < mLeafPaths.size(); ++i) {
    tmPath* thePath = mLeafPaths[i];
    if (!thePath->IsActivePath()) continue;
    if (thePath->mNodes.front() == this) 
      angles.push_back(Angle(thePath->mNodes.back()->mLoc - mLoc));
    else
      angles.push_back(Angle(thePath->mNodes.front()->mLoc - mLoc));
  }
  
  // Paper edges have the same effect as active paths, so we'll create
  // fictitious angles if the nodes lies on the edge of the paper.
  if (IsTiny(mLoc.x)) angles.push_back(-PI);          // left
  if (IsTiny(mLoc.x - mTree->mPaperWidth)) angles.push_back(0);    // right
  if (IsTiny(mLoc.y - mTree->mPaperHeight)) angles.push_back(PI/2);  // top
  if (IsTiny(mLoc.y)) angles.push_back(-PI/2);          // bottom
  
  // First, if there's not at least two active paths, it isn't pinned.  
  if (angles.size() < 2) return;
  
  // Now sort the angles by size.  
  sort(angles.begin(), angles.end());
  
  // The node is unpinned if there are any obtuse angles, i.e., the difference
  // between adjacent angles is greater than PI. Note that the last angle is a
  // special case.
  for (size_t i = 0; i < angles.size() - 1; ++i)
    if (angles[i + 1] - angles[i] > PI + ConvexityTol()) return;
  if (angles.front() - angles.back() + PI > ConvexityTol()) return;

  // If we're still here, then there are at least two active paths and all
  // angles are acute or 180 degrees; therefore the node is pinned.
  mIsPinnedNode = true;
}


/*****
Return the vertex corresponding to this node, building a new one if
necessary. If the node is a tree node, tell the vertex about it.
*****/
tmVertex* tmNode::GetOrMakeVertexSelf()
{
  TMASSERT(mOwnedVertices.size() <= 1);
  if (mOwnedVertices.not_empty()) return mOwnedVertices.front();
  tmVertex* theVertex;
  if (IsTreeNode()) theVertex = 
    new tmVertex(mTree, this, mLoc, mElevation, mIsBorderNode, this);
  else theVertex =
    new tmVertex(mTree, this, mLoc, mElevation, mIsBorderNode);
  return theVertex;
}


/*****
Return the vertex owned by this node. Return a null ptr if it doesn't exist
yet.
*****/
tmVertex* tmNode::GetVertex() const
{
  if (mOwnedVertices.not_empty())  return mOwnedVertices.front();
  return 0;
}


/*****
Return the number of polygon paths incident upon this tmNode.
*****/
size_t tmNode::GetNumPolygonPaths() const
{
  return count_if(mLeafPaths.begin(), mLeafPaths.end(),
    mem_fun(&tmPath::IsPolygonPath));
}


/*****
Put a tmNode to a file in version 5 format.
*****/    
void tmNode::Putv5Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex);  
  PutPOD(os, mLabel);
  PutPOD(os, mLoc);  
  PutPOD(os, mDepth);  
  PutPOD(os, mElevation);  
  PutPOD(os, mIsLeafNode);
  PutPOD(os, mIsSubNode);
  PutPOD(os, mIsBorderNode);
  PutPOD(os, mIsPinnedNode);
  PutPOD(os, mIsPolygonNode);
  PutPOD(os, mIsJunctionNode);
  PutPOD(os, mIsConditionedNode);  
  mTree->PutPtrArray(os, mEdges);
  mTree->PutPtrArray(os, mLeafPaths);  
  mTree->PutPtrArray(os, mOwnedVertices);
  mTree->PutOwnerPtr(os, mNodeOwner);
}
    

/*****
Get a tmNode from a file in version 5 format.
*****/    
void tmNode::Getv5Self(istream& is)
{
  CheckTagStr<tmNode>(is);  
  GetPOD(is, mIndex);  
  GetPOD(is, mLabel);
  GetPOD(is, mLoc);    
  GetPOD(is, mDepth);    
  GetPOD(is, mElevation);    
  GetPOD(is, mIsLeafNode);
  GetPOD(is, mIsSubNode);
  GetPOD(is, mIsBorderNode);
  GetPOD(is, mIsPinnedNode);
  GetPOD(is, mIsPolygonNode);
  GetPOD(is, mIsJunctionNode);
  GetPOD(is, mIsConditionedNode);
  mTree->GetPtrArray(is, mEdges);
  mTree->GetPtrArray(is, mLeafPaths);
  mTree->GetPtrArray(is, mOwnedVertices);
  mTree->GetOwnerPtr(is, mNodeOwner);
}


/*****
Put a tmNode to a file in version 4 format. Note that we do not put any
polys, vertices, or creases.
*****/    
void tmNode::Putv4Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex);  
  PutPOD(os, mLabel);
  PutPOD(os, mLoc);  
  PutPOD(os, mIsLeafNode);
  PutPOD(os, mIsSubNode);
  PutPOD(os, mIsBorderNode);
  PutPOD(os, mIsPinnedNode);
  PutPOD(os, mIsPolygonNode);
  PutPOD(os, mIsConditionedNode);
  PutPOD(os, 0); // no owned vertices
  mTree->PutPtrArray(os, mEdges);
  mTree->PutPtrArray(os, mLeafPaths);  
  mTree->PutOwnerPtr(os, mNodeOwner);
}
    

/*****
Get a tmNode from a file in version 4 format.
*****/    
void tmNode::Getv4Self(istream& is)
{
  CheckTagStr<tmNode>(is);  
  GetPOD(is, mIndex);  
  GetPOD(is, mLabel);
  GetPOD(is, mLoc);    
  GetPOD(is, mIsLeafNode);
  GetPOD(is, mIsSubNode);
  GetPOD(is, mIsBorderNode);
  GetPOD(is, mIsPinnedNode);
  GetPOD(is, mIsPolygonNode);
  GetPOD(is, mIsConditionedNode);
  mTree->GetPtrArray(is, mOwnedVertices);
  mTree->GetPtrArray(is, mEdges);
  mTree->GetPtrArray(is, mLeafPaths);
  mTree->GetOwnerPtr(is, mNodeOwner);
}


/*****
Get a node from a file in version 3 format. Note that when we read a version 3
document, we create conditions, which were previously implemented as member
variables.
*****/  
void tmNode::Getv3Self(istream& is)
{
  CheckTagStr<tmNode>(is);
  
  // Get the index of the node to have on hand.
  size_t i;        // index of this node
  GetPOD(is, i);
  mIndex = i;
  
  GetPOD(is, mLabel);
  GetPOD(is, mLoc);
  
  // Symmetric node
  bool nodeIsSymmetric;
  GetPOD(is, nodeIsSymmetric);  // GetPOD(mIsSymmetric);
  
  if (nodeIsSymmetric) {
    tmConditionNodeSymmetric* c = new tmConditionNodeSymmetric(mTree);
    c->mNode = this;
  }
  
  // Paired node
  bool nodeIsPaired;
  GetPOD(is, nodeIsPaired);     // GetPOD(mIsPaired);  
  size_t j;            // index of the paired node
  GetPOD(is, j);          // GetPtr(mPairNode, !(mIsPaired));
  TMASSERT(j != 0);
  
  // Only create a tmCondition if tmNode i has a higher index. Choose one case
  // so that we only create one tmCondition. Choose highest since owned nodes
  // don't get assigned to until they've been fully read in.
  if (nodeIsPaired && (i > j)) {
    tmConditionNodesPaired* c = new tmConditionNodesPaired(mTree);
    c->mNode1 = this;
    c->mNode2 = mTree->mOwnedNodes[j-1];
  }
  
  // Fixed node
  bool nodeXFixed, nodeYFixed;
  tmFloat nodeXFixValue, nodeYFixValue;
  GetPOD(is, nodeXFixed);     // GetPOD(mXFixed);
  GetPOD(is, nodeYFixed);     // GetPOD(mYFixed);
  GetPOD(is, nodeXFixValue);   // GetPOD(mXFixValue);
  GetPOD(is, nodeYFixValue);   // GetPOD(mYFixValue);
  if (nodeXFixed || nodeYFixed) {
    tmConditionNodeFixed* c = new tmConditionNodeFixed(mTree);
    c->mNode = this;
    c->mXFixed = nodeXFixed;
    if (nodeXFixed) c->mXFixValue = nodeXFixValue;
    else c->mXFixValue = 0;
    c->mYFixed = nodeYFixed;
    if (nodeYFixed) c->mYFixValue = nodeYFixValue;
    else c->mYFixValue = 0;
  }

  // edge node
  bool nodeStickToEdge;
  GetPOD(is, nodeStickToEdge); // GetPOD(mStickToEdge);
  if (nodeStickToEdge) {
    tmConditionNodeOnEdge* c = new tmConditionNodeOnEdge(mTree);
    c->mNode = this;
  }
  
  // Collinear nodes
  size_t k;        // index of the third node in the triplet
  bool nodeIsCollinear;
  GetPOD(is, nodeIsCollinear);  // GetPOD(mIsCollinear);
  GetPOD(is, j);         // GetPtr(mCollinearNode1, !(mIsCollinear));
  TMASSERT(j != 0);
  GetPOD(is, k);         // GetPtr(mCollinearNode2, !(mIsCollinear));
  TMASSERT(k != 0);
  
  // Only create the tmCondition if this tmNode has the highest index of the
  // triplet Choose one case so we only create one triplet condition. Choose
  // highest so we've already read in mOwnedNodes referenced.
  if (nodeIsCollinear && (i > j) && (i > k)) {
    tmConditionNodesCollinear* c = new tmConditionNodesCollinear(mTree);
    c->mNode1 = this;
    c->mNode2 = mTree->mOwnedNodes[j-1];
    c->mNode3 = mTree->mOwnedNodes[k-1];
  }
  
  // Get remaining information
  GetPOD(is, mIsLeafNode);
  mIsSubNode = false;
  
  GetPOD(is, mIsBorderNode);
  GetPOD(is, mIsPinnedNode);
  GetPOD(is, mIsPolygonNode);
  
  mIsConditionedNode = false;
  
  mTree->GetPtrArray(is, mEdges);
  mTree->GetPtrArray(is, mLeafPaths);
  
  // Set ownership
  mTree->mOwnedNodes.push_back(this);
  mNodeOwner = mTree;
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmNode, "node")

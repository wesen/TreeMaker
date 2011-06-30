/*******************************************************************************
File:         tmNode.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmNode
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-25
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMNODE_H_
#define _TMNODE_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmPart.h"
#include "tmVertexOwner.h"
#include "tmDpptrTarget.h"
#include "tmPoint.h"
#include "tmDpptrArray.h"


/**********
class tmNode
Represents a node on the tree graph
**********/
class tmNode : public tmPart, public tmVertexOwner, public tmDpptrTarget {
public:
  // Getters
  const char* GetLabel() const {
    // Return the label for this node. The label isn't used for anything within
    // TreeMaker; it's for the convenience of the user.
    return mLabel;};
    
  const tmPoint& GetLoc() const {
    // Return the location of this node on the paper.
    return mLoc;};
    
  const tmFloat& GetLocX() const {
    // Return the x-coordinate of this node on the paper.
    return mLoc.x;};
    
  const tmFloat& GetLocY() const {
    // Return the y-coordinate of this node on the paper.
    return mLoc.y;};
    
  const tmFloat& GetDepth() const {
    // Return the scaled distance from this node to the root node, which also
    // becomes the y-coordinate in the folded form. Only meaningful for tree
    // nodes. Depth depends on which node of the tree was the root node.
    return mDepth;};
    
  const tmFloat& GetElevation() const {
    // Return the scaled distance from this node to the axis, which also
    // becomes the x-coordinate in the folded form. Elevation doesn't depend on
    // which node was root node.
    return mElevation;};
    
  bool IsLeafNode() const {
    // A node is a leaf node if it is a tree node and is incident to one edge
    // of the tree.
    return mIsLeafNode;};
    
  bool IsBranchNode() const {
    // A node is a branch node if it is a tree node and is incident to 2 or
    // more edges of the tree.
    return !mIsLeafNode && !mIsSubNode;};
    
  bool IsSubNode() const {
    // A node is a sub node if it was created in the interior of some poly.
    return mIsSubNode;};
    
  bool IsTreeNode() const {
    // A node is a tree node if it is part of the tree, i.e., was not created
    // in the interior of some poly.
    return !mIsSubNode;};
    
  bool IsBorderNode() const {
    // A node is  border node if it lies on the convex hull of all the tree
    // nodes or of a group of inset nodes that will be split into sub-polys.
    return mIsBorderNode;};
    
  bool IsPinnedNode() const {
    // A node is pinned if it cannot be moved without violating some
    // constraints.
    return mIsPinnedNode;};
    
  bool IsPolygonNode() const {
    // A node is a polygon node if it will be the corner of a polygon or
    // subpoly.
    return mIsPolygonNode;};
    
  bool IsJunctionNode() const {
    // A node is a junction node if it is an inset node (sub node) that is the
    // insetting of at least two ring nodes.
    return mIsJunctionNode;};
    
  bool IsConditionedNode() const {
    // A node is conditioned if it has one or more conditions attached to it.
    return mIsConditionedNode;};
  
  const tmDpptrArray<tmEdge>& GetEdges() const {
    // Return the list of edges incident to this node. The list is nonempty
    // only for tree nodes.
    return mEdges;};
    
  const tmDpptrArray<tmPath>& GetLeafPaths() const {
    // Return the list of leaf paths incident to this node. These paths could
    // be tree paths or subpaths; the latter if this node is an inset node of a
    // subpoly.
    return mLeafPaths;};
    
  const tmNodeOwner* GetNodeOwner() const {
    // Return the object that owns this node.
    return mNodeOwner;};
  
  // Coordinate access (these are used by the interface to TTL)
  const tmFloat& x() const {return mLoc.x;};
  const tmFloat& y() const {return mLoc.y;};

  // Setters
  void SetLabel(const char* aLabel);
  void SetLoc(const tmPoint& aLoc);
  void SetLocX(const tmFloat& ax);
  void SetLocY(const tmFloat& ay);
  
  // Lexicographic comparison (used by TTL)
  static bool LessThan(const tmNode* node1, const tmNode* node2) {
    return node1->x() < node2->x() || 
    (node1->x() == node2->x() && node1->y() < node2->y());};
  
  // Misc queries
  bool IsRedundant() const;
  std::size_t CalcDiscreteDepth() const;
  std::size_t GetNumActivePaths() const;
  static bool ContainsRedundantNodes(const tmArray<tmNode*>& nlist);
  
  // List filter
  static void FilterLeafNodes(tmArray<tmNode*>& dstList, 
   const tmArray<tmNode*>& srcList);
  
private:
  // node settings
  char mLabel[MAX_LABEL_LEN + 1];
  tmPoint mLoc;
  tmFloat mDepth;
  tmFloat mElevation;
  
  // Topological flags
  bool mIsLeafNode;
  bool mIsSubNode;
  
  // Dimensional flags
  bool mIsBorderNode;
  bool mIsPinnedNode;
  bool mIsPolygonNode;
  bool mIsJunctionNode;
  bool mIsConditionedNode;

  // Structural data
  tmDpptrArray<tmEdge> mEdges;
  tmDpptrArray<tmPath> mLeafPaths;
  
  // owner
  tmNodeOwner* mNodeOwner;
  
  // Ctors
  void InitNode();
  tmNode(tmTree*);
  tmNode(tmTree* aTree, tmNodeOwner* aNodeOwner, const tmPoint& aLoc);
  
  // Ownership
  tmTree* GetOwnerAsTree();
  tmPoly* GetOwnerAsPoly();
  tmNode* VertexOwnerAsNode() {return this;};
  tmPath* VertexOwnerAsPath() {return 0;};

  // Miscellaneous
  void CalcIsPinnedNode();
  tmVertex* GetOrMakeVertexSelf();
  tmVertex* GetVertex() const;
  std::size_t GetNumPolygonPaths() const;

  // Stream I/O
  void Putv5Self(std::ostream& os);
  void Getv5Self(std::istream& is);
  void Putv4Self(std::ostream& os);
  void Getv4Self(std::istream& is);
  void Getv3Self(std::istream& is);
  
  // Class tag for stream I/O
  TM_DECLARE_TAG()
  
  // Friend classes
  friend class tmPart;
  friend class tmPart::StringT<tmNode>;
  friend class tmPart::CreatorFnT<tmNode>;
  friend class tmTree;
  friend class tmNodeOwner;
  friend class tmEdge;
  friend class tmPath;
  friend class tmPathOwner;
  friend class tmPoly;
  friend class tmPolyOwner;
  friend class tmConditionNodeFixed;
  friend class tmConditionNodeCombo;
  friend class tmConditionNodeOnCorner;
  friend class tmConditionNodeOnEdge;
  friend class tmConditionNodeSymmetric;
  friend class tmConditionNodesPaired;
  friend class tmConditionNodesCollinear;
  friend class tmConditionPathCombo;
  friend class tmConditionPathActive;
  friend class tmConditionPathAngleFixed;
  friend class tmConditionPathAngleQuant;
  friend class tmStubFinder;
};

#endif // _TMNODE_H_

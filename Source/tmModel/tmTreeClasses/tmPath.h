/*******************************************************************************
File:         tmPath.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmPath
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-25
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMPATH_H_
#define _TMPATH_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmPart.h"
#include "tmVertexOwner.h"
#include "tmCreaseOwner.h"
#include "tmDpptrTarget.h"
#include "tmDpptrArray.h"
#include "tmDpptr.h"
#include "tmTree.h"


/**********
class tmPath
This class represents a path from one node to another along the tree, but a
subset of the leaf paths also serves as the basis of the crease pattern.
**********/
class tmPath : public tmPart, public tmVertexOwner, public tmCreaseOwner, 
  public tmDpptrTarget {
public:
  // Getters
  const tmFloat& GetMinTreeLength() const {
    // return the minimum length of this path in tree units as specified by the
    // tree. Only nonzero for axis-parallel paths.
    return mMinTreeLength;};
    
  const tmFloat& GetMinPaperLength() const {
    // Return the minimum length of this path in paper units as specified by
    // the tree. Only nonzero for axis-parallel paths.
    return mMinPaperLength;};
    
  const tmFloat& GetActTreeLength() const {
    // Return the actual length of this path in tree units as determined from
    // the positions of its endpoint nodes. Only nonzero for axis-parallel
    // paths.
    return mActTreeLength;};
    
  const tmFloat& GetActPaperLength() const {
    // Return the actual length of this path in paper units as determined from
    // the positions of its endpoint nodes. Only nonzero for axis-parallel
    // paths.
   return mActPaperLength;};
   
  const tmPoint GetVector() const;
  const tmFloat GetAngle() const;
  const tmFloat GetPositiveAngle() const;
  
  bool IsLeafPath() const {
    // A path is a leaf path if it connects two leaf nodes of the tree; it is
    // always a tree path
    return mIsLeafPath;};
  bool IsAxialPath() const {
    // A path is an axial path if it connects two leaf nodes of the tree and is
    // a polygon path.
    return mIsLeafPath && mIsPolygonPath;};
    
  bool IsSubPath() const {
    // A path is a sub path if it is a reduced path, i.e., it was created
    // during the construction of sub-polygons; can be inset ring, spoke, or
    // ridge
    return mIsSubPath;};
    
  bool IsTreePath() const {
    // A path is a tree path if it connects two nodes on the tree (leaf or
    // branch) rather than being a sub path.
    return !mIsSubPath;};
    
  bool IsFeasiblePath() const {
    // A path is feasible if its actual length is greater than or equal to its
    // minimum length. Only meaningful for axis-parallel paths.
    return mIsFeasiblePath;};
    
  bool IsInfeasiblePath() const {
    // An path is infeasible if its actual length is less than its minimum
    // length. Only meaningful for axis-parallel paths.
    return !mIsFeasiblePath;};
    
  bool IsActivePath() const {
    // An path is active if its actual length is equal to its minimum length
    // (within the specified tolerance). Active paths can be leaf paths (major
    // paths) or subpaths.
   return mIsActivePath;};
   
  bool IsBorderPath() const {
    // A path is a border path if it lies on the convex hull of the leaf nodes
    // (in which case the paths are all leaf paths) or if it lies on the convex
    // hull of a set of inset nodes within a poly that are to be broken up into
    // one or more subpolys.
    return mIsBorderPath;};
    
  bool IsPolygonPath() const {
    // A poth is a polygon path if it is one of the edges of a polygon or
    // subpoly.
    return mIsPolygonPath;};
    
  bool IsConditionedPath() const {
    // A path is conditioned if it has one or more conditions placed upon it.
    return mIsConditionedPath;};
    
  bool IsActiveAxialPath() const {
    // A path is active-axial if it is an axial path (i.e., a leaf path) and it
    // is active. Note that an axial path on the border of the convex hull
    // might not be active.
    return mIsActivePath && mIsLeafPath;};
    
  bool IsGussetPath() const {
    // A gusset path is an active path within an inset polygon, i.e., a cross
    // path that has become active.
    return mIsActivePath && !mIsBorderPath;};
    
  bool IsAxisParallelPath() const {
    // True if this path is axis-parallel, i.e., an axial leaf path (for major
    // paths) or a cross path (for inset paths). Note that we use the minimum
    // tree length as the check, since the default (for all other values) is 0.
    return mMinTreeLength != 0.0;};
    
  const tmDpptrArray<tmNode>& GetNodes() const {
    // Returns the nodes that make up this path. If it's a major path, they're
    // all the nodes along the tree path. If it's a subpath, then it will just
    // contain the two endpoints of the path.
    return mNodes;};
    
  const tmDpptrArray<tmEdge>& GetEdges() const {
    // Returns the edges that make up this path in the same order as mNodes. If
    // it's a subpath, this will be empty.
    return mEdges;};
    
  const tmDpptr<tmPoly>& GetFwdPoly() const {
    // Return the polygon to the left of this path when it is oriented from
    // front-to-back or NULL if none exists.
    return mFwdPoly;};
    
  const tmDpptr<tmPoly>& GetBkdPoly() const {
    // Return the polygon to the right of this path when it is oriented from
    // front-to-back or NULL if none exists.
    return mBkdPoly;};
    
  const tmPathOwner* GetPathOwner() const {
    // Return the owner of this path.
    return mPathOwner;};
    
  const tmDpptr<tmPath>& GetOutsetPath() const {
    // Return the path that this path has been immediately inset from.
    return mOutsetPath;};
    
  const tmFloat& GetMinDepth() const {
    // Return the minimum depth encountered along this path.
    return mMinDepth;};
    
  const tmFloat& GetMinDepthDist() const {
    // Return the distance from the front of the path at which the minimum
    // depth is encountered.
    return mMinDepthDist;};
    
  const tmFloat& GetFrontReduction() const {
    // Return the length reduction at the front of this path, relative to the
    // length of its outset path.
    return mFrontReduction;};
    
  const tmFloat& GetBackReduction() const {
    // Return the length reduction at the back of this path, relative to the
    // length of its outset path.
    return mBackReduction;};
  
  void GetMaxOutsetPath(tmPath*& maxOutsetPath, 
   tmFloat& maxFrontReduction, tmFloat& maxBackReduction);
  tmNode* GetOtherNode(const tmNode* aNode) const;
  
  // Filter on list
  static void FilterLeafPaths(tmArray<tmPath*>& dstList, 
   const tmArray<tmPath*>& srcList);
  
private:
  // tmPath settings
  tmFloat mMinTreeLength;
  tmFloat mMinPaperLength;
  tmFloat mActTreeLength;
  tmFloat mActPaperLength;
  
  // Topological flags
  bool mIsLeafPath;
  bool mIsSubPath;
  
  // Dimensional flags
  bool mIsFeasiblePath;
  bool mIsActivePath;
  bool mIsBorderPath;
  bool mIsPolygonPath;
  bool mIsConditionedPath;
  
  // Structural references
  tmDpptrArray<tmNode> mNodes;
  tmDpptrArray<tmEdge> mEdges;
  tmDpptr<tmPoly> mFwdPoly;
  tmDpptr<tmPoly> mBkdPoly;
  
  // More length-related vbls
  tmDpptr<tmPath> mOutsetPath;
  tmFloat mFrontReduction;
  tmFloat mBackReduction;
  
  // vbls added for depth calc
  tmFloat mMinDepth;
  tmFloat mMinDepthDist;
  
  // owner
  tmPathOwner* mPathOwner;

  // Constructors
  void InitPath();
  tmPath(tmTree* aTree);
  tmPath(tmTree* aTree, tmEdge* aEdge);
  tmPath(tmTree* aTree, tmPath* aPath);
  tmPath(tmTree* aTree, tmNode* aNode1, tmNode* aNode2);
  tmPath(tmPoly* aPoly, tmNode* aNode1, tmNode* aNode2);
  
  // Destructor
  ~tmPath();
  
  // Ownership
  tmNode* VertexOwnerAsNode() {return 0;};
  tmPath* VertexOwnerAsPath() {return this;};
  tmPath* CreaseOwnerAsPath() {return this;};
  tmPoly* CreaseOwnerAsPoly() {return 0;};

  // Miscellaneous utilities
  void TreePathCalcLengths();
  static bool TestIsFeasible(const tmFloat& actLen, const tmFloat& minLen);
  static bool TestIsActive(const tmFloat& actLen, const tmFloat& minLen);
  bool StartsOrEndsWith(tmNode* aNode) const;
  bool SharesEndNodeWith(tmPath* aPath) const;
  bool IntersectsInterior(tmPath* aPath) const;
  bool IsIncidentToFilledPoly() const;
  tmVertex* GetOrMakeVertex(const tmPoint& p, tmNode* aTreeNode = 0);
  tmVertex* MakeVertex(const tmPoint& p, tmNode* aTreeNode = 0);  
  tmVertex* GetFrontVertex() const;
  tmVertex* GetBackVertex() const;
  void BuildSelfVertices();
  void ConnectSelfVertices(tmCrease::Kind aKind);
  void SetVertexDepth(tmVertex* aVertex) const;
  tmCrease* GetFirstCrease(tmVertex* aVertex) const;
  void GetNextCreaseAndVertex(tmCrease* aCrease, tmVertex* aVertex,
    tmCrease*& nextCrease, tmVertex*& nextVertex) const;
  
  // Stream I/O
  void Putv5Self(std::ostream& os);
  void Getv5Self(std::istream& is);
  void Putv4Self(std::ostream& os);
  void Getv4Self(std::istream& is);
  void Getv3Self(std::istream& is);
  
  // Class tag for stream I/O
  TM_DECLARE_TAG()
  
  // Friend classes
  friend class tmDpptrArray<tmPath>;
  friend class tmPart;
  friend class tmPart::StringT<tmPath>;
  friend class tmPart::CreatorFnT<tmPath>;
  friend class tmTree;
  friend class tmNode;
  friend class tmPathOwner;
  friend class tmPoly;
  friend class tmPolyOwner;
  friend class tmConditionPathCombo;
  friend class tmConditionPathActive;
  friend class tmConditionPathAngleFixed;
  friend class tmConditionPathAngleQuant;
  friend class tmStubFinder;
};

#endif // _TMPATH_H_

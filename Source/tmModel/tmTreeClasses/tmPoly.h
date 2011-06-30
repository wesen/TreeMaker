/*******************************************************************************
File:         tmPoly.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmPoly
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-25
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMPOLY_H_
#define _TMPOLY_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmPart.h"
#include "tmPolyOwner.h"
#include "tmCreaseOwner.h"
#include "tmFacetOwner.h"
#include "tmDpptrTarget.h"
#include "tmDpptrArray.h"
#include "tmPoint.h"
#include "tmArray.h"


/**********
class tmPoly
Class that represents a polygon in the crease pattern, which gets filled with
molecule crease patterns.
**********/
class tmPoly : public tmPart, public tmPolyOwner, public tmCreaseOwner, 
  public tmFacetOwner, public tmDpptrTarget {
public:
  // Getters
  const tmPoint& GetCentroid() const {
    // Return the centroid of this polygon.
    return mCentroid;
  };
    
  bool IsSubPoly() const {
    // Return true if this poly is a subpoly, i.e., enclosed by another poly.
    return mIsSubPoly;
  };
    
  bool IsMajorPoly() const {
    // Return true if this poly is a tree poly, i.e., an outermost poly.
    return !mIsSubPoly;};
    
  const tmArray<tmNode*>& GetRingNodes() const {
    // Return the list of nodes that are the corners of this poly, going in the
    // CCW direction.
    return mRingNodes;
  };
    
  const tmArray<tmPath*>& GetRingPaths() const {
    // Return the list of paths that are the sides of this poly, going in the
    // CCW direction.
    return mRingPaths;
  };
    
  const tmArray<tmPath*>& GetCrossPaths() const {
    // Return the list of paths that connect non-consecutive nodes of this
    // poly. The ring paths plus the cross paths make up all the paths between
    // the nodes that are the corners of this poly.
    return mCrossPaths;
  };
    
  const tmArray<tmNode*>& GetInsetNodes() const {
    // Return the inset nodes of this poly, which become the corner of one or
    // more subpolys. Note that there is a 1-1 relationship between each of
    // mInsetNodes and each of mRingNodes; thus, a node may appear multiple
    // times in mInsetNodes.
    return mInsetNodes;
  };
    
  const tmArray<tmPath*>& GetSpokePaths() const {
    // Return the paths that connect the ring nodes with the inset nodes.
    return mSpokePaths;
  };
    
  tmPath* GetRidgePath() const {
    // If there are exactly two distinct inset nodes, this will be the path
    // between them (and NULL otherwise).
    return mRidgePath;
  };
  
  const tmArray<tmPoint>& GetNodeLocs() const {
    // Get the last location stored for the ring nodes of this poly. Normally,
    // each entry will be the same as mLoc of the corresponding ring node, but
    // we use this to detect when a ring node has shifted position (which
    // forces a complete reconstruction of the poly contents).
    return mNodeLocs;
  };
  
  const tmPolyOwner* GetPolyOwner() const {
    // Return the owner of this poly, which can be the tree (for tree polys) or
    // another poly (for subpolys).
    return mPolyOwner;
  };
  
  const tmArray<tmVertex*>& GetLocalRootVertices() const {
    // return the vertices that are locally root within this poly.
    return mLocalRootVertices;
  };
  
  const tmArray<tmCrease*>& GetLocalRootCreases() const {
    // return the hinge creases that are locally root within this poly.
    return mLocalRootCreases;
  };
  
  // Queries
  bool ConvexEncloses(tmPoint p) const;
  std::size_t GetSize() const;
  void GetSubTree(tmArray<tmNode*>& nlist, tmArray<tmEdge*>& elist) const;
  bool SubTreeContains(tmNode* aNode) const;
  bool SubTreeContains(tmEdge* aEdge) const;
  
  // Tolerance
  static tmFloat MoveTol() {
    // Return the tolerance we allow a node to move before we consider that the
    // poly has been invalidated.
    return 1.0e-6;
  };
  
private:
  // quantitative data
  tmPoint mCentroid;
  
  // Topological flags
  bool mIsSubPoly;
  
  // Structural information
  // These don't need to be Dpptr-based since they're all killed together.
  tmArray<tmNode*> mRingNodes;
  tmArray<tmPath*> mRingPaths;
  tmArray<tmPath*> mCrossPaths;
  tmArray<tmNode*> mInsetNodes;
  tmArray<tmPath*> mSpokePaths;
  tmPath* mRidgePath;   
  tmArray<tmPoint> mNodeLocs;

  // Rebuilt at cleanup
  tmArray<tmVertex*> mLocalRootVertices;
  tmArray<tmCrease*> mLocalRootCreases;
  
  // owner
  tmPolyOwner* mPolyOwner;

  // Constructors/destructors
  void InitPoly();
  tmPoly(tmTree* aTree);
  tmPoly(tmTree* aTree, tmPolyOwner* aPolyOwner);
  ~tmPoly();
  
  // Ownership
  tmTree* GetOwnerAsTree();
  tmPoly* GetOwnerAsPoly();
  tmPoly* GetOutermostPoly();
  tmTree* NodeOwnerAsTree() {return 0;};
  tmPoly* NodeOwnerAsPoly() {return this;};
  tmTree* PathOwnerAsTree() {return 0;};
  tmPoly* PathOwnerAsPoly() {return this;};
  tmTree* PolyOwnerAsTree() {return 0;};
  tmPoly* PolyOwnerAsPoly() {return this;};
  tmPoly* FacetOwnerAsPoly() {return this;};
  tmPath* CreaseOwnerAsPath() {return 0;};
  tmPoly* CreaseOwnerAsPoly() {return this;};
  
#ifdef TMDEBUG
  // Debugging support
  void DumpToFileAsText(const char* filename);
  void DumpToFileAsMath(const char* filename);
#endif

  // Construction of subpolys and interior creases
  void CalcContents();
  bool CalcPolyIsConvex() const;
  bool CalcPolyEnclosesNode(const tmArray<tmNode*> nlist) const;
  bool CalcPolyIsValid() const;
  void CalcCrossPaths();
  tmNode* GetOrMakeInsetNode(const tmPoint& p);
  void GetRidgelineNodesAndPaths(tmNode* frontNode, tmNode* backNode, 
    tmArray<tmNode*>& ridgeNodes, tmArray<tmPath*>& ridgePaths);
  struct SortableRidgeVertex {
    tmVertex* mVertex;
    tmFloat mSortValue;
    SortableRidgeVertex(tmVertex* aVertex, const tmPoint& frontPt,
      const tmPoint& backPt);
    bool operator< (const SortableRidgeVertex& rhs) const;
  };
  void GetRidgelineVertices(tmNode* frontNode, tmNode* backNode, 
    tmArray<tmVertex*>& ridgeVertices);
  bool HasPolyContents();
  void BuildPolyContents();
  std::size_t GetNumInactiveBorderPaths();
  void SetFacetCorridorEdge(tmFacet* aFacet, tmEdge* aEdge);
  void CalcFacetCorridorEdges();
  
  // Construction of depth information
  void CalcBend();
  
  // Construction of facet ordering
  tmFacet* GetIncidentInteriorFacet(tmCrease* axialCrease);
  tmCrease* GetIncidentInteriorCrease(tmVertex* axialVertex);
  void BuildCorridorLinks(tmCrease* fromCrease, tmFacet* fromFacet);
  void CalcLocalFacetOrder();
  
  // Stream I/O
  void Putv5Self(std::ostream& os);
  void Getv5Self(std::istream& is);
  void Putv4Self(std::ostream& os);
  void Getv4Self(std::istream& is);
  
  // Class tag for stream I/O
  TM_DECLARE_TAG()
  
  // Friend classes
  friend class tmPart;
  friend class tmPart::StringT<tmPoly>;
  friend class tmPart::CreatorFnT<tmPoly>;
  friend class tmPath;
  friend class tmPolyOwner;
  friend class tmDpptrArray<tmPoly>;
  friend class tmTree;
  friend class tmFacetOwner;
  friend class tmStubFinder;
  friend class tmRootNetwork;
};

#endif // _TMPOLY_H_

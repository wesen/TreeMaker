/*******************************************************************************
File:         tmVertex.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmVertex class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-25
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMVERTEX_H_
#define _TMVERTEX_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmPart.h"
#include "tmDpptrTarget.h"
#include "tmPoint.h"
#include "tmDpptrArray.h"


/**********
class tmVertex
Class that represents a vertex of a crease pattern.
**********/
class tmVertex : public tmPart, public tmDpptrTarget {
public:
  // Getters
  
  // Computed at construction
  
  const tmPoint& GetLoc() const {
    // Return the location of this vertex on the paper.
    return mLoc;};
  
  const tmFloat& GetElevation() const {
    // Return the elevation of this vertex, i.e., its distance from the axis.
    return mElevation;};
  
  bool IsBorderVertex() const {
    // Return true if this vertex is on the border of the crease pattern.
    return mIsBorderVertex;};
  
  tmNode* GetTreeNode() const {
    // Return the tree node, if any, that this vertex is the projection of.
    return mTreeNode;
  };
  
  tmVertex* GetLeftPseudohingeMate() const {
    // If this vertex is an axial hinge vertex incident to a pseudohinge facet
    // on the left, then it has a mating vertex on the other side of the
    // pseudohinge that figures in the facet ordering graph. Return this other
    // vertex.
    return mLeftPseudohingeMate;
  };
  
  tmVertex* GetRightPseudohingeMate() const {
    // If this vertex is an axial hinge vertex incident to a pseudohinge facet
    // on the right, then it has a mating vertex on the other side of the
    // pseudohinge that figures in the facet ordering graph. Return this other
    // vertex.
    return mRightPseudohingeMate;
  };
  
  const tmDpptrArray<tmCrease>& GetCreases() const {
    // Return the creases incident to this vertex.
    return mCreases;
  };
  
  const tmVertexOwner* GetVertexOwner() const {
    // Return the owner of this vertex, which could be a node or path.
    return mVertexOwner;
  };
  
  // Recomputed at cleanup
  
  const tmFloat& GetDepth() const {
    // Return the depth of this vertex, i.e., its depth relative to the root
    // node in tree units.
    return mDepth;
  };
  
  std::size_t GetDiscreteDepth() const {
    // Return the discrete depth of this vertex, i.e., its depth relative to
    // the root node in number of hops along the rooted tree graph. Only
    // meaningful for vertices that project to a tree node. Others will return
    // size_t(-1).
    return mDiscreteDepth;
  };
 
  // Misc queries 
  bool IsMajorVertex() const;
  bool IsMinorVertex() const;

private:
  // computed at construction
  tmPoint mLoc;
  tmFloat mElevation;
  bool mIsBorderVertex;
  tmNode* mTreeNode;
  tmVertex* mLeftPseudohingeMate;
  tmVertex* mRightPseudohingeMate;
  tmDpptrArray<tmCrease> mCreases;
    
  // recomputed at cleanup
  tmFloat mDepth;
  std::size_t mDiscreteDepth;
  
  // scratch pad for facet ordering
  int mCCFlag;
  int mSTFlag;
  
  // owner
  tmVertexOwner* mVertexOwner;
  
  // Constructors
  void InitVertex();
  tmVertex(tmTree* aTree);
  tmVertex(tmTree* aTree, tmVertexOwner* aVertexOwner, tmPoint aLoc,
    tmFloat aElevation, bool aIsBorderVertex, tmNode* aTreeNode = 0);
  
  // Ownership
  tmNode* GetOwnerAsNode();
  tmPath* GetOwnerAsPath();

  // Topological queries
  std::size_t GetNumMajorCreases() const;
  std::size_t GetNumHingeCreases() const;
  bool IsAxialVertex() const;
  bool IsAxialOrGussetVertex() const;
  bool IsHingeVertex() const;
  
  // Topological utilities
  std::size_t GetDegree(const tmArray<tmCrease*>& creaseList) const;
  tmCrease* GetHingeCrease() const;
  void GetAxialOrGussetCreases(tmCrease*& crease1, tmCrease*& crease2) const;
  void GetHingeCreases(tmCrease*& crease1, tmCrease*& crease2) const;
  
  // Facet linking
  void SwapLinks();
  
  // Clear cleanup data
  void ClearCleanupData();
  
  // Sameness test
  static bool VerticesSameLoc(const tmPoint& p1, const tmPoint& p2);
  
  // Stream I/O
  void Putv5Self(std::ostream& os);
  void Getv5Self(std::istream& is);
  void Putv4Self(std::ostream& os);
  void Getv4Self(std::istream& is);
  
  // Class tag for stream I/O
  TM_DECLARE_TAG()
  
  // Friend classes
  friend class tmPart;
  friend class tmPart::StringT<tmVertex>;
  friend class tmPart::CreatorFnT<tmVertex>;
  friend class tmTree;
  friend class tmNode;
  friend class tmPath;
  friend class tmPoly;
  friend class tmVertexOwner;
  friend class tmFacet;
  friend class tmFacetOwner;
  friend class tmCrease;
  friend class tmCreaseOwner;
  friend class tmRootNetwork;
};

#endif // _TMVERTEX_H_

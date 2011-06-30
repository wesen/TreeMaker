/*******************************************************************************
File:         tmFacet.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmFacet class
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-24
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMFACET_H_
#define _TMFACET_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmPart.h"
#include "tmDpptrTarget.h"
#include "tmDpptrArray.h"


/**********
class tmFacet
Class that represents a facet in a crease pattern.
**********/
class tmFacet : public tmPart, public tmDpptrTarget {
public:
  const tmPoint GetCentroid() const {
    // Return the centroid of this facet.
    return mCentroid;
  };
  
  bool IsWellFormed() const {
    // Return whether this facet is well-formed, i.e., it has a bottom crease
    // and ridge crease vertices project to distinct tree nodes.
    return mIsWellFormed;
  };
  
  const tmArray<tmVertex*>& GetVertices() const {
    // Return the vertices around this facet, which are in CCW order.
    // Once a facet is fully constructed, the first two vertices will be the
    // two bottom vertices of the facet.
    return mVertices;
  };
  
  const tmArray<tmCrease*>& GetCreases() const {
    // Return the creases around this facet, which are in CCW order. Once a
    // facet is fully constructed, the creases will start with the bottom
    // crease, either axial or gusset.
    return mCreases;
  };
  
  tmEdge* GetCorridorEdge() const {
    // Return the edge corresponding to the corridor that contains this facet.
    return mCorridorEdge;
  };
  
  tmCrease* GetBottomCrease() const;
  tmCrease* GetLeftCrease() const;
  tmCrease* GetRightCrease() const;

  const tmArray<tmFacet*>& GetTailFacets() const {
    // Return the list of facets that are immediate tail facets relative to
    // this one in the facet ordering graph.
    return mTailFacets;
   };
  
  const tmArray<tmFacet*>& GetHeadFacets() const {
    // Return the list of facets that are immediate source facets relative to
    // this one in the facet ordering graph.
    return mHeadFacets;
  };
  
  const std::size_t GetOrder() const {
    // Return the facet order value. Comparing this number with the number from
    // any other facet will determine which facet is on top in the folded form.
    return mOrder;
  };
    
  // which side of a facet faces upward
  enum Color {
    NOT_ORIENTED = 0,
    WHITE_UP = 1,
    COLOR_UP = 2
  };
  
  const Color GetColor() const {
    // Return which side of this facet is facing up in the folded form.
    return mColor;
  };
    
  bool IsWhiteUpFacet() const {
    // Return true if this facet is white up in the folded form.
    return mColor == WHITE_UP;
  };
    
  bool IsColorUpFacet() const {
    // Return true if this facet is color up in the folded form.
    return mColor == COLOR_UP;
  };
    
  bool IsUnorientedFacet() const {
    // Return true if this facet has not yet had its color set.
    return mColor == NOT_ORIENTED;
  };
    
  bool IsOrientedFacet() const {
    // Return true if this facet has had its color set to white up or
    // color up.
    return mColor == WHITE_UP || mColor == COLOR_UP;
  };
    
  const tmFacetOwner* GetFacetOwner() const {
    // Return the owner of this facet, which is always a tmPoly.
    return mFacetOwner;
  };
  
  // Source/sink status
  bool IsSourceFacet() const;
  bool IsSinkFacet() const;

  // Queries
  bool ConvexEncloses(const tmPoint& p);
  
private:
  // Built at construction
  tmPoint mCentroid;
  bool mIsWellFormed;
  tmArray<tmVertex*> mVertices;
  tmArray<tmCrease*> mCreases;
  
  // Rebuilt at cleanup
  tmEdge* mCorridorEdge;
  tmArray<tmFacet*> mTailFacets;
  tmArray<tmFacet*> mHeadFacets;
  std::size_t mOrder;
  Color mColor;
  
  // owner
  tmFacetOwner* mFacetOwner;

  // Constructors
  void InitFacet();
  tmFacet(tmTree* aTree);
  tmFacet(tmPoly* aPoly);
  
  // Facet queries related to incident crease type
  bool IsAxialFacet() const;
  bool IsGussetFacet() const;
  bool IsPseudohingeFacet() const;
  bool IsNotPseudohingeFacet() const;
  
  // Crease getters based on crease type
  tmCrease* GetAxialCrease() const;
  tmCrease* GetGussetCrease() const;
  void GetRidgeCreases(tmArray<tmCrease*>& ridgeCreases) const;
  
  // Crease getters based on crease position around facet
  tmCrease* GetOtherSideCrease(tmCrease* aCrease) const;
  void GetOtherCorridorCreases(tmCrease* aCrease, 
    tmArray<tmCrease*>& corridorCreases) const;
  
  // Facet-to-facet queries
  tmFacet* GetLeftFacet();
  tmFacet* GetRightFacet();
  tmFacet* GetLeftNonPseudohingeFacet();
  tmFacet* GetRightNonPseudohingeFacet();
  
  // Construction of contents
  void CalcContents();
  void ClearCleanupData();
  
  // Facet queries based on facet ordering graph
  bool HasTailFacet(tmFacet* aFacet) const;
  bool HasHeadFacet(tmFacet* aFacet) const;
  
  // Construction of facet ordering graph
  void ClearLinks();
  void AddTailFacet(tmFacet* tailFacet);
  void LinkTo(tmFacet* headFacet);
  static bool AreLinked(tmFacet* facet1, tmFacet* facet2);
  static void Link(tmFacet* facet1, tmFacet* facet2);
  static void Unlink(tmFacet* facet1, tmFacet* facet2);
  void CalcOrder(std::size_t& nextOrder);
    
  // Color utilities
  void CalcColor(const Color aColor);
  static Color OppositeColor(const Color aColor);

  // Stream I/O
  void Putv5Self(std::ostream& os);
  void Getv5Self(std::istream& is);
  
  // Class tag for stream I/O
  TM_DECLARE_TAG()
  
  // Friend classes
  friend class tmPart;
  friend class tmPart::StringT<tmFacet>;
  friend class tmPart::CreatorFnT<tmFacet>;
  friend class tmFacetOwner;
  friend class tmTree;
  friend class tmPoly;
  friend class tmCrease;
  friend class tmVertex;
  friend class tmRootNetwork;
  friend class tmCorridor;
};

#endif // _TMFACET_H_

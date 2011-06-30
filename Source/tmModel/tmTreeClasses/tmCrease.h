/*******************************************************************************
File:         tmCrease.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmCrease class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCREASE_H_
#define _TMCREASE_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmPart.h"
#include "tmDpptrTarget.h"
#include "tmDpptrArray.h"
#include "tmVertex.h"


/**********
class tmCrease
Class that represents a crease in a crease pattern.
**********/
class tmCrease : public tmPart, public tmDpptrTarget {
public:
  // Crease structural role
  enum Kind {
    AXIAL = 0,
    GUSSET = 1,
    RIDGE = 2,
    UNFOLDED_HINGE = 3,
    FOLDED_HINGE = 4,
    PSEUDOHINGE = 5
  };
  // Fold direction for creases
  enum Fold {
    FLAT = 0,
    MOUNTAIN = 1,
    VALLEY = 2,
    BORDER = 3
  };
  
  // Getters
  
  // Structural kind getters
  
  const Kind GetKind() const {
    // Return the kind of crease, i.e., its AGRH assignment.
    return mKind;
  };
    
  bool IsAxialCrease() const {
    // Return true if this is an axial crease.
    return mKind == AXIAL;
  };
    
  bool IsGussetCrease() const {
    // Return true if this is a gusset crease.
    return mKind == GUSSET;
  };
  
  bool IsRidgeCrease() const {
    // Return true if this is a ridge crease.
    return mKind == RIDGE;
  };
  
  bool IsAxialOrGussetCrease() const {
    // Return true if this is either an axial or gusset crease (as opposed
    // to a ridge or hinge).
    return mKind == AXIAL || mKind == GUSSET;
  };
  
  bool IsHingeCrease() const {
    // Return true if this is any form of hinge crease. 
    return mKind == UNFOLDED_HINGE || mKind == FOLDED_HINGE || 
      mKind == PSEUDOHINGE;
  };
    
  bool IsRegularHingeCrease() const {
    // Return true if this is a hinge crease but not a pseudohinge crease. 
    return mKind == UNFOLDED_HINGE || mKind == FOLDED_HINGE;
  };
    
  bool IsPseudohingeCrease() const {
    // return true if this is a pseudohinge crease, i.e., a hinge crease that
    // doesn't project to any tree node. 
    return mKind == PSEUDOHINGE;
  };
  
  bool IsMajorCrease() const {
    // Return true if this is an axial, ridge, or gusset crease. 
    return mKind == AXIAL || mKind == RIDGE || mKind == GUSSET;
  };
    
  bool IsMinorCrease() const {
    // Return true if this is a hinge crease.
    return IsHingeCrease();
  };
  
  bool IsFoldedCrease() const {
    // Return true if this crease is folded (versus unfolded).
    return mKind == AXIAL || mKind == GUSSET || mKind == RIDGE ||
      mKind == FOLDED_HINGE || mKind == PSEUDOHINGE;
  };
  
  const tmDpptrArray<tmVertex>& GetVertices() const {
    // Return the list of (2) vertices that are the endpoints of this crease.
    return mVertices;
  };
  
  const tmDpptr<tmFacet>& GetFwdFacet() const {
    // Return the facet incident to this crease in the forward (CCW) direction.
    return mFwdFacet;
  };
  
  const tmDpptr<tmFacet>& GetBkdFacet() const {
    // Return the facet incident to this crease in the backward (CW) direction.
    return mBkdFacet;
  };
  
  const tmCreaseOwner* GetCreaseOwner() const {
    // Return the owner of this crease, which is either a path or a poly.
    return mCreaseOwner;
  };
    
  // Rebuilt at cleanup
  
  const Fold GetFold() const {
    // Return the mountain/valley/flat/unfolded status of this crese.
    return mFold;
  };
  
  bool IsFlatCrease() const {
    // Return true if this crease is flat, i.e., unfolded. All creases start
    // out as flat.
    return mFold == FLAT;
  };
  
  bool IsMountainCrease() const {
    // Return true if this is a mountain crease.
    return mFold == MOUNTAIN;
  };
  
  bool IsValleyCrease() const {
    // Return true if this is a valley crease.
    return mFold == VALLEY;
  };
   
  const tmVertex* GetFrontVertex() const {
    // Return the front vertex of the two
    TMASSERT(mVertices.size() == 2);
    return mVertices.front();
  };
  
  const tmVertex* GetBackVertex() const {
    // Return the back vertex of the two
    TMASSERT(mVertices.size() == 2);
    return mVertices.back();
  };
  
  const tmPoint GetVector() const {
    // Return the vector from the front vertex to the back vertex in paper
    // coordinates.
    return mVertices.back()->mLoc - mVertices.front()->mLoc;
  };
  
  const tmFloat GetAngle() const {
    // Return the angle of this crease in degrees. The value lies within 
    // [-180, 180].
    return RADIAN * Angle(mVertices.back()->mLoc - mVertices.front()->mLoc);
  };
  
  const tmFloat GetPositiveAngle() const {
    // Return the angle of this crease, with direction defined so it is within 
    // [0, 180).
    tmFloat theAngle = GetAngle();
    if (theAngle >= 0) return theAngle;
    return theAngle + 180.;
  };

private:
  // Set construction
  Kind mKind;
  tmDpptrArray<tmVertex> mVertices;
  tmDpptr<tmFacet> mFwdFacet;
  tmDpptr<tmFacet> mBkdFacet;
  
  // Rebuilt at cleanup
  Fold mFold;
  
  // Scratch pad for facet ordering
  int mCCFlag;
  int mSTFlag;
  
  // owner
  tmCreaseOwner* mCreaseOwner;
  
  // Constructors
  void InitCrease();
  tmCrease(tmTree* aTree);
  tmCrease(tmTree* aTree, tmCreaseOwner* aCreaseOwner, tmVertex* aVertex1,
    tmVertex* aVertex2, Kind aKind);
    
  // Ownership
  tmPath* GetOwnerAsPath();
  tmPoly* GetOwnerAsPoly();

  // Incidence and adjacency
  tmVertex* GetOtherVertex(tmVertex* aVertex) const;
  tmVertex* GetHigherVertex() const;
  tmVertex* GetLowerVertex() const;
  tmFacet* GetLeftFacet() const;
  tmFacet* GetRightFacet() const;
  tmFacet* GetLeftNonPseudohingeFacet() const;
  tmFacet* GetRightNonPseudohingeFacet() const;
  tmFacet* GetOtherFacet(tmFacet* aFacet) const;
  tmVertex* GetCommonVertex(tmCrease* aCrease) const;
  bool IsIncidentTo(tmVertex* aVertex) const;
  bool HasCommonVertex(tmCrease* aCrease) const;
  bool IntersectsInterior(tmCrease* aCrease) const;
  bool IsBorderCrease() const;
  
  // Bend calculation
  void CalcBend();
  
  // Fold direction
  void CalcFold();
  
  // Data recalculated at cleanup
  void ClearCleanupData();
    
  // Stream I/O
  void Putv5Self(std::ostream& os);
  void Getv5Self(std::istream& is);
  void Putv4Self(std::ostream& os);
  void Getv4Self(std::istream& is);
  
  // Class tag for stream I/O
  TM_DECLARE_TAG()
  
  // Friend classes
  friend class tmPart;
  friend class tmPart::StringT<tmCrease>;
  friend class tmPart::CreatorFnT<tmCrease>;
  friend class tmPath;
  friend class tmVertex;
  friend class tmCreaseOwner;
  friend class tmFacetOwner;
  friend class tmTree;
  friend class tmPoly;
  friend class tmFacet;
  friend class tmRootNetwork;
  friend class tmCorridor;
};

#endif // _TMCREASE_H_

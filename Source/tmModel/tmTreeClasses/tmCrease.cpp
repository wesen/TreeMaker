/*******************************************************************************
File:         tmCrease.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmCrease class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmCrease.h"
#include "tmModel.h"

using namespace std;

/*
tmCrease represents a crease in the crease pattern. It has two vertices and
an assigned kind.
*/

/**********
class tmCrease
Class that represents a crease in a crease pattern.
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


#ifdef __MWERKS__
  #pragma mark --PRIVATE--
#endif


/*****
Common initialization
*****/
void tmCrease::InitCrease()
{
  // Register with tmTree
  mTree->mCreases.push_back(this);
  mIndex = size_t(mTree->mCreases.size());
  
  // Initialize member data with default values
  mKind = AXIAL;
  mFwdFacet = 0;
  mBkdFacet = 0;
  mFold = FLAT;
  mCCFlag = 0;
  mSTFlag = 0;
  
  // owner
  mCreaseOwner = 0;
}


/*****
Creator constructor
*****/
tmCrease::tmCrease(tmTree* aTree)  : 
  tmPart(aTree), tmDpptrTarget()
{
  InitCrease();
}


/*****
Full constructor
*****/
tmCrease::tmCrease(tmTree* aTree, tmCreaseOwner* aCreaseOwner, 
  tmVertex* aVertex1, tmVertex* aVertex2,  Kind aKind)  : 
  tmPart(aTree), tmDpptrTarget()
{
  InitCrease();
  
  // Register with Owner
  mCreaseOwner = aCreaseOwner;
  mCreaseOwner->mOwnedCreases.push_back(this);
  
  // Initialize type
  mKind = aKind;
  
  // Fill instances
  mVertices.push_back(aVertex1);
  mVertices.push_back(aVertex2);
  
  aVertex1->mCreases.push_back(this);
  aVertex2->mCreases.push_back(this);
  
  // Vertices must be distinct
  TMASSERT(aVertex1 != aVertex2);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Return the owner of this crease if it is a path and 0 otherwise.
This is not defined in tmCrease.h because it requires tmCreaseOwner.h; but
tmCreaseOwner.h requires tmCrease.h to define tmCrease::Kind.
*****/
tmPath* tmCrease::GetOwnerAsPath()
{
  return mCreaseOwner->CreaseOwnerAsPath();
}
  
/*****
Return the owner of this crease if it is a poly and 0 otherwise.
*****/
tmPoly* tmCrease::GetOwnerAsPoly()
{
  return mCreaseOwner->CreaseOwnerAsPoly();
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Return the vertex at the other end of this crease from aVertex, which MUST be
at one end or the other or an assertion will fail in debug builds.
*****/
tmVertex* tmCrease::GetOtherVertex(tmVertex* aVertex) const
{
  TMASSERT(mVertices.size() == 2);
  TMASSERT(mVertices.contains(aVertex));
  if (aVertex == mVertices.front()) return mVertices.back();
  else {
    TMASSERT(aVertex == mVertices.back());
    return mVertices.front();
  }
}


/*****
Return the higher-elevation vertex of this crease. It should be either a ridge
or a hinge crease. If it is a ridge crease that is axis-parallel, the result
will be unpredictable.
*****/
tmVertex* tmCrease::GetHigherVertex() const
{
  TMASSERT(!IsAxialOrGussetCrease());
  TMASSERT(mVertices.size() == 2);
  tmVertex* v1 = mVertices.front();
  tmVertex* v2 = mVertices.back();
  if (v1->mElevation < v2->mElevation) return v2;
  else return v1;
}
 
 
/*****
Return the lower-elevation vertex of this crease. It should be either a ridge
or a hinge crease. If it is a ridge crease that is axis-parallel, the result
will be unpredictable.
*****/
tmVertex* tmCrease::GetLowerVertex() const
{
  TMASSERT(!IsAxialOrGussetCrease());
  TMASSERT(mVertices.size() == 2);
  tmVertex* v1 = mVertices.front();
  tmVertex* v2 = mVertices.back();
  if (v1->mElevation < v2->mElevation) return v1;
  else return v2;
}


/*****
Return the facet on the other side of this crease from aFacet, which must be
one of the indicent facets. If this crease is a border crease, there is only
one incident facet and this routine will return 0, so users should check for
this possibility.
*****/
tmFacet* tmCrease::GetOtherFacet(tmFacet* aFacet) const
{
  if (aFacet == mFwdFacet) return mBkdFacet;
  else {
   TMASSERT(aFacet == mBkdFacet);
   return mFwdFacet;
  }
}


/*****
Return the facet to the left of this hinge or ridge crease.
*****/
tmFacet* tmCrease::GetLeftFacet() const
{
  TMASSERT(GetLowerVertex()->IsAxialOrGussetVertex());
  TMASSERT(!IsAxialOrGussetCrease());
  if (mFwdFacet->GetRightCrease() == this) return mFwdFacet;
  else {
    TMASSERT(mBkdFacet->GetRightCrease() == this);
    return mBkdFacet;
  }
}


/*****
Return the facet to the right of this hinge or ridge crease.
*****/
tmFacet* tmCrease::GetRightFacet() const
{
  TMASSERT(GetLowerVertex()->IsAxialOrGussetVertex());
  TMASSERT(!IsAxialOrGussetCrease());
  if (mFwdFacet->GetLeftCrease() == this) return mFwdFacet;
  else {
    TMASSERT(mBkdFacet->GetLeftCrease() == this);
    return mBkdFacet;
  }
}


/*****
Return the next non-pseudohinge facet to the left of this crease.
*****/
tmFacet* tmCrease::GetLeftNonPseudohingeFacet() const
{
  TMASSERT(GetLowerVertex()->IsAxialOrGussetVertex());
  TMASSERT(!IsAxialOrGussetCrease());
  tmFacet* theFacet = GetLeftFacet();
  for (; theFacet->IsPseudohingeFacet(); theFacet = theFacet->GetLeftFacet());
  return theFacet;
}


/*****
Return the next non-pseudohinge facet to the right of this crease.
*****/
tmFacet* tmCrease::GetRightNonPseudohingeFacet() const
{
  TMASSERT(GetLowerVertex()->IsAxialOrGussetVertex());
  TMASSERT(!IsAxialOrGussetCrease());
  tmFacet* theFacet = GetRightFacet();
  for (; theFacet->IsPseudohingeFacet(); theFacet = theFacet->GetRightFacet());
  return theFacet;
}


/*****
Return the vertex, if any, that this crease and the passed crease share. Return
NULL if they don't share a vertex. It is an error to pass the same crease as
the argument to this routine.
*****/
tmVertex* tmCrease::GetCommonVertex(tmCrease* aCrease) const
{
  TMASSERT(this != aCrease);
  TMASSERT(mVertices.size() == 2);
  TMASSERT(aCrease->mVertices.size() == 2);
  tmVertex* v1a = mVertices.front();
  tmVertex* v2a = aCrease->mVertices.front();
  if (v1a == v2a) return v1a;
  tmVertex* v2b = aCrease->mVertices.back();
  if (v1a == v2b) return v1a;
  tmVertex* v1b = mVertices.back();
  if (v1b == v2a || v1b == v2b) return v1b;
  return 0;
}


/*****
Return true if this crease is incident to the given vertex.
*****/
bool tmCrease::IsIncidentTo(tmVertex* aVertex) const
{
  return mVertices.contains(aVertex);
}


/*****
Return true if the given tmCrease starts or ends with the same tmVertex that
this tmCrease starts or ends with.
*****/
bool tmCrease::HasCommonVertex(tmCrease* aCrease) const
{
  return !!GetCommonVertex(aCrease);
}


/*****
Return true if these two tmCreases intersect in their interior, but don't share
an endpoint.
*****/
bool tmCrease::IntersectsInterior(tmCrease* aCrease) const
{
  if (HasCommonVertex(aCrease)) return false;
  const tmPoint& p = mVertices.front()->mLoc;
  const tmPoint rp = mVertices.back()->mLoc - p;
  const tmPoint& q = aCrease->mVertices.front()->mLoc;
  const tmPoint rq = aCrease->mVertices.back()->mLoc - q;
  tmFloat tp, tq;
  if (!GetLineIntersectionParms(p, rp, q, rq, tp, tq)) return false;
  if (tp <= 0 || tp >= 1 || tq <= 0 || tq >= 1) return false;
  return true;
}


/*****
Return true if this crease is a border crease, i.e., it is incident to only one
facet. Note that this only works after the facets have been fully constructed.
*****/
bool tmCrease::IsBorderCrease() const
{
  return (mFwdFacet == 0 || mBkdFacet == 0);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Calculate the bend status of this crease. This is only relevant for hinge
creases, so nothing will happen with other types of creases.
*****/
void tmCrease::CalcBend()
{
  if (!IsHingeCrease()) return;
  if (IsPseudohingeCrease()) return;
  
  // Now that we know it's a hinge crease, we can proceed. The bottom vertex of
  // the crease will be an axial or gusset vertex. Start by getting this
  // vertex. Note that the top vertex *could* be a gusset vertex; we pick the
  // vertex with the lowest elevation.
  tmVertex* vertex2 = mVertices.front();
  tmVertex* vertex2a = mVertices.back();
  if (vertex2->mElevation > vertex2a->mElevation) vertex2 = vertex2a;
  TMASSERT(vertex2->IsAxialOrGussetVertex());
  
  // Look at the axial (or gusset) creases to either side of this bottom
  // vertex, and find the two vertices at the other ends of those two creases.
  // The depth of the three vertices -- the first AG vertex, and the two
  // vertices on either side -- lets us determine whether the crease is folded
  // (and in which direction) and lets us set its bend status.
  tmCrease* crease1;
  tmCrease* crease3;
  vertex2->GetAxialOrGussetCreases(crease1, crease3);
  tmVertex* vertex1 = crease1->GetOtherVertex(vertex2);
  tmVertex* vertex3 = crease3->GetOtherVertex(vertex2);
  const tmFloat& depth1 = vertex1->mDepth;
  const tmFloat& depth2 = vertex2->mDepth;
  const tmFloat& depth3 = vertex3->mDepth;
  TMASSERT((depth1 != depth2) && (depth2 != depth3));
  if (((depth1 > depth2) && (depth2 < depth3)) || 
    ((depth1 < depth2) && (depth2 > depth3))) mKind = FOLDED_HINGE;
  else {
    mKind = UNFOLDED_HINGE;
    TMASSERT(((depth1 > depth2) && (depth2 > depth3)) || \
      ((depth1 < depth2) && (depth2 < depth3)));
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Calculate the fold direction of this crease, which depends on the relative order
of the two facets on either side and their color orientation.
*****/
void tmCrease::CalcFold()
{
  if (mFwdFacet == 0 || mBkdFacet == 0)
    mFold = BORDER;
  else if (mFwdFacet->mColor == mBkdFacet->mColor)
    mFold = FLAT;
  else if (mFwdFacet->mColor == tmFacet::COLOR_UP)
    if (mFwdFacet->mOrder > mBkdFacet->mOrder)
      mFold = MOUNTAIN;
    else
      mFold = VALLEY;
  else
    if (mFwdFacet->mOrder > mBkdFacet->mOrder)
      mFold = VALLEY;
    else
      mFold = MOUNTAIN;
} 


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Clear the data that is recalculated at cleanup
*****/
void tmCrease::ClearCleanupData()
{
  mFold = FLAT;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Put a tmCrease in version 5 format
*****/
void tmCrease::Putv5Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex);  
  PutPOD(os, int(mKind));
  mTree->PutPtrArray(os, mVertices);
  mTree->PutPtr(os, mFwdFacet);
  mTree->PutPtr(os, mBkdFacet);
  PutPOD(os, int(mFold));
  PutPOD(os, mCCFlag);
  PutPOD(os, mSTFlag);
  mTree->PutOwnerPtr(os, mCreaseOwner);
}


/*****
Get a tmCrease in version 5 format
*****/
void tmCrease::Getv5Self(istream& is)
{
  CheckTagStr<tmCrease>(is);
  GetPOD(is, mIndex);
  int kind;
  GetPOD(is, kind);
  mKind = Kind(kind);
  mTree->GetPtrArray(is, mVertices);
  mTree->GetPtr(is, mFwdFacet, true);
  mTree->GetPtr(is, mBkdFacet, true);
  int fold;
  GetPOD(is, fold);
  mFold = Fold(fold);
  GetPOD(is, mCCFlag);
  GetPOD(is, mSTFlag);
  mTree->GetOwnerPtr(is, mCreaseOwner);
}


/*****
Put a tmCrease in version 4 format
*****/
void tmCrease::Putv4Self(ostream& os)
{
  TMFAIL("tmCrease::Putv4Self");
}


/*****
Get a tmCrease in version 4 format
*****/
void tmCrease::Getv4Self(istream& is)
{
  CheckTagStr<tmCrease>(is);
  int kind;
  GetPOD(is, kind);
  mKind = tmCrease::Kind(kind);
  mTree->GetPtrArray(is, mVertices);
  mTree->GetOwnerPtr(is, mCreaseOwner);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmCrease, "crse")

/*******************************************************************************
File:         tmFacet.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmFacet class
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-24
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmFacet.h"
#include "tmModel.h"

using namespace std;

/*
tmFacet represents a facet in the crease pattern. It has a list of creases
and vertices around its border and an assigned color, which indicates
whether the white or colored side of the paper is facing up in the folded form.
*/

/**********
class tmFacet
Class that represents a facet in a crease pattern.
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


/*****
Return the crease on the bottom of this facet, which will be either an
axial or gusset crease.
*****/
tmCrease* tmFacet::GetBottomCrease() const
{
  TMASSERT(mIsWellFormed);
  TMASSERT(mCreases.size() >= 3);
  return mCreases.front();
}


/*****
Return the crease on the left side of this facet, which will be either a
hinge or a ridge crease that is adjacent to the bottom crease. Since creases
are ordered CCW starting with the bottom crease, the left crease will always
be the last crease in the array.
*****/
tmCrease* tmFacet::GetLeftCrease() const
{
  TMASSERT(mIsWellFormed);
  TMASSERT(mCreases.size() >= 3);
  return mCreases.back();
}


/*****
Return the crease on the right side of this facet, which will be either a
hinge or a ridge crease that is adjacent to the bottom crease. Since creases
are ordered CCW starting with the bottom crease, the right crease will always
be the 2nd crease in the array.
*****/
tmCrease* tmFacet::GetRightCrease() const
{
  TMASSERT(mIsWellFormed);
  TMASSERT(mCreases.size() >= 3);
  return mCreases[1];
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Return true if this facet has at least one tail facet but no head facets, i.e.,
it is a source in the facet ordering graph.
*****/
bool tmFacet::IsSourceFacet() const
{
  return mHeadFacets.not_empty() && mTailFacets.empty();
}


/*****
Return true if this facet has at least one head facet but no tail facets, i.e.,
it is a sink in the facet ordering graph.
*****/
bool tmFacet::IsSinkFacet() const
{
  return mTailFacets.not_empty() && mHeadFacets.empty();
}


/*****
Return true if this facet encloses the given point. Used by GUI to detect
clicks inside polygons.
*****/
bool tmFacet::ConvexEncloses(const tmPoint& p)
{
  size_t n = mVertices.size();
  vector<tmPoint> v(n);
  for (size_t i = 0; i < n; ++i) v[i] = mVertices[i]->mLoc;
  return ::ConvexEncloses(v, p);
}


#ifdef __MWERKS__
  #pragma mark -
  #pragma mark --PRIVATE--
#endif


/*****
Common initialization
*****/
void tmFacet::InitFacet()
{
  // Register with tmTree
  mTree->mFacets.push_back(this);
  mIndex = size_t(mTree->mFacets.size());
  
  // Initialize members with default values
  mCentroid = tmPoint(0., 0.);
  mIsWellFormed = false;
  mCorridorEdge = NULL;
  mOrder = size_t(-1);
  mColor = NOT_ORIENTED;
  
  // owner
  mFacetOwner = 0;
}


/*****
Creator constructor
*****/
tmFacet::tmFacet(tmTree* aTree) : 
  tmPart(aTree), tmDpptrTarget()
{
  InitFacet();
}


/*****
Constructor
*****/
tmFacet::tmFacet(tmPoly* aPoly) : 
  tmPart(aPoly->mTree), tmDpptrTarget()
{
  InitFacet();
  
  // Register with Owner
  TMASSERT(aPoly);
  mFacetOwner = aPoly;
  mFacetOwner->mOwnedFacets.push_back(this);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Return true if this facet is incident to an axial crease.
*****/
bool tmFacet::IsAxialFacet() const
{
  return !!GetAxialCrease();
}


/*****
Return true if this facet is incident to a gusset crease.
*****/
bool tmFacet::IsGussetFacet() const
{
  return !!GetGussetCrease();
}


/*****
Return true if this facet is incident to a pseudohinge crease. If it is, the
pseudohinge crease would be either the left or right crease.
*****/
bool tmFacet::IsPseudohingeFacet() const
{
  if (GetLeftCrease()->IsPseudohingeCrease()) return true;
  if (GetRightCrease()->IsPseudohingeCrease()) return true;
  return false;
}


/*****
Return true if this facet is NOT incident to a pseudohinge crease.
*****/
bool tmFacet::IsNotPseudohingeFacet() const
{
  if (GetLeftCrease()->IsPseudohingeCrease()) return false;
  if (GetRightCrease()->IsPseudohingeCrease()) return false;
  return true;
}


/*****
Return the axial crease (if any) incident to this facet. This, and related
routines that assume the facet is well-formed, can only be called for
well-formed facets and will throw an assertion error if they are not.
*****/
tmCrease* tmFacet::GetAxialCrease() const
{
  TMASSERT(mIsWellFormed);
  TMASSERT(mCreases.size() >= 3);
  tmCrease* theCrease = mCreases.front();
  if (theCrease->IsAxialCrease()) return theCrease;
  return 0;
}


/*****
Return the gusset crease (if any) incident to this facet.
*****/
tmCrease* tmFacet::GetGussetCrease() const
{
  TMASSERT(mIsWellFormed);
  TMASSERT(mCreases.size() >= 3);
  tmCrease* theCrease = mCreases.front();
  if (theCrease->IsGussetCrease()) return theCrease;
  return 0;
}


/*****
Return the ridge creases incident to this facet in the passed array.
There will be at least one. We can skip the first crease in mCreases, since
that's going to be axial or gusset. But we need to include all the others,
because either the left or right crease could be a ridge crease.
*****/
void tmFacet::GetRidgeCreases(tmArray<tmCrease*>& ridgeCreases) const
{
  TMASSERT(mIsWellFormed);
  TMASSERT(mCreases.size() >= 3);
  ridgeCreases.clear();
  for (size_t i = 1; i < mCreases.size(); ++i) {
    tmCrease* theCrease = mCreases[i];
    if (theCrease->IsRidgeCrease()) ridgeCreases.push_back(theCrease);
  }
}


/*****
Assuming that the passed crease is one of the side creases, return the other
side crease of this facet.
*****/
tmCrease* tmFacet::GetOtherSideCrease(tmCrease* aCrease) const
{
  tmCrease* leftCrease = GetLeftCrease();
  tmCrease* rightCrease = GetRightCrease();
  if (aCrease == leftCrease) return rightCrease;
  if (aCrease == rightCrease) return leftCrease;
  TMFAIL("In tmFacet::GetOtherSideCrease() argument was not a side crease");
  return 0;
}


/*****
Return all other creases in the same interior of the corridor as this facet,
excluding the passed crease. That would be all creases excluding the passed
crease and non-pseudohinge hinge creases, since pseudohinge creases, by
definition, lie entirely within a corridor.
*****/
void tmFacet::GetOtherCorridorCreases(tmCrease* aCrease, 
  tmArray<tmCrease*>& corridorCreases) const
{
  TMASSERT(mIsWellFormed);
  TMASSERT(mCreases.contains(aCrease));
  corridorCreases.clear();
  for (size_t i = 0; i < mCreases.size(); ++i) {
    tmCrease* theCrease = mCreases[i];
    if (theCrease == aCrease) continue;
    if (theCrease->IsRegularHingeCrease()) continue;
    corridorCreases.push_back(theCrease);
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Return the facet to the immediate left of this one.
*****/
tmFacet* tmFacet::GetLeftFacet()
{
  return GetLeftCrease()->GetLeftFacet();
}


/*****
Return the facet to the immediate right of this one.
*****/
tmFacet* tmFacet::GetRightFacet()
{
  return GetRightCrease()->GetRightFacet();
}


/*****
Return the next facet to the left of this one that is NOT a pseudohinge facet.
Note that we can have multiple side-by-side pairs of pseudohinges, so we have
too keep looking until we find one.
*****/
tmFacet* tmFacet::GetLeftNonPseudohingeFacet()
{
  TMASSERT(IsAxialFacet());
  tmFacet* theFacet = GetLeftFacet();
  for (; theFacet->IsPseudohingeFacet(); theFacet = theFacet->GetLeftFacet());
  return theFacet;
}


/*****
Return the next facet to the right of this one that is NOT a pseudohinge facet.
*****/
tmFacet* tmFacet::GetRightNonPseudohingeFacet()
{
  TMASSERT(IsAxialFacet());
  tmFacet* theFacet = GetRightFacet();
  for (; theFacet->IsPseudohingeFacet(); theFacet = theFacet->GetRightFacet());
  return theFacet;
}

  
#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Calculate the contents of this facet. This is also the place to insure that
facets have the proper structure. This is called at initial construction.
*****/
void tmFacet::CalcContents()
{
  // Calculate the centroid
  size_t numVertices = mVertices.size();
  mCentroid = tmPoint(0.0, 0.0);
  for (size_t i = 0; i < numVertices; ++i) mCentroid += mVertices[i]->mLoc;
  mCentroid /= numVertices;
  
  // Rotate the vertex and crease arrays so that the (sole) axial or gusset
  // crease is the first element in the array. This insures that we can
  // directly access left, right, and bottom creases of the facet without
  // searching. Note that in certain "sliver" situations, a facet may be
  // formed that doesn't have a bottom crease. This will prevent facet ordering,
  // but not construction of a crease pattern.
  mIsWellFormed = true;
  size_t irotate = 0;
  while (!mCreases.front()->IsAxialOrGussetCrease()) {
    mVertices.rotate_left();
    mCreases.rotate_left();
    ++irotate;
    if (irotate >= numVertices) {
      // Oh-oh, big problems; we didn't find an axial or gusset crease. This can
      // happen with vertical "sliver" facets. The crease pattern will look OK,
      // but we won't be able to calculate facet color or facet ordering.
      mIsWellFormed = false;
      break;
    }
  }
}


/*****
Clear the data that gets recalculated during every call to CleanupAfterEdit().
This is itself called by CleanupAfterEdit() before the bail-out-able phase.
*****/
void tmFacet::ClearCleanupData()
{
  mCorridorEdge = 0;
  mTailFacets.clear();
  mHeadFacets.clear();
  mOrder = size_t(-1);
  mColor = NOT_ORIENTED;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Return true if this facet has aFacet as an immediate tail in the facet ordering
graph.
*****/
bool tmFacet::HasTailFacet(tmFacet* aFacet) const
{
  return mTailFacets.contains(aFacet);
}


/*****
Return true if this facet has aFacet as an immediate head in the facet ordering
graph.
*****/
bool tmFacet::HasHeadFacet(tmFacet* aFacet) const
{
  return mHeadFacets.contains(aFacet);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Clear all heads and tails from this facet, removing it from the facet ordering
graph.
*****/
void tmFacet::ClearLinks()
{
  mTailFacets.clear();
  mHeadFacets.clear();
}


/*****
Add a link in the facet ordering graph with this facet as the tail and
headFacet as the head.
*****/
void tmFacet::LinkTo(tmFacet* headFacet)
{
  TMASSERT(headFacet);
  mHeadFacets.push_back(headFacet);
  headFacet->mTailFacets.push_back(this);
}


/*****
STATIC
Return true if these facets are already linked in either direction.
*****/
bool tmFacet::AreLinked(tmFacet* facet1, tmFacet* facet2)
{
  if (facet1->mHeadFacets.contains(facet2)) return true;
  if (facet2->mHeadFacets.contains(facet1)) return true;
  TMASSERT(!facet1->mTailFacets.contains(facet2));
  TMASSERT(!facet2->mTailFacets.contains(facet1));
  return false;
}


/*****
STATIC
Link the two facets by adding an edge to the facet ordering graph from the sink
to the source, whichever is which. An assertion failure will happen if it can't
be inferred which direction the link should go.
*****/
void tmFacet::Link(tmFacet* facet1, tmFacet* facet2)
{
  if (facet1->IsSinkFacet()) {
    TMASSERT(facet2->IsSourceFacet());
    facet1->LinkTo(facet2);
    return;
  }
  TMASSERT(facet1->IsSourceFacet());
  TMASSERT(facet2->IsSinkFacet());
  facet2->LinkTo(facet1);
}


/*****
STATIC
Remove the edge connecting these two facets.
*****/
void tmFacet::Unlink(tmFacet* facet1, tmFacet* facet2)
{
  if (facet1->mHeadFacets.contains(facet2)) {
    facet1->mHeadFacets.erase_remove(facet2);
    TMASSERT(facet2->mTailFacets.contains(facet1));
    facet2->mTailFacets.erase_remove(facet1);
    return;
  }
  TMASSERT(facet1->mTailFacets.contains(facet2));
  facet1->mTailFacets.erase_remove(facet2);
  TMASSERT(facet2->mHeadFacets.contains(facet1));
  facet2->mHeadFacets.erase_remove(facet1);
}


/*****
Calculate the order of this facet and its tail facets. nextOrder is the
next value to use. If any of the facets above it do not have their order set
yet, return. Otherwise, set the order to the next available value and call this
routine for each of its heads. Note that this will recurse to the depth of the
longest path in the facet order graph.
*****/
void tmFacet::CalcOrder(std::size_t& nextOrder)
{
  // Stop recursing if we reach a facet that has already had its order set.
  if (mOrder != size_t(-1)) return;
  
  // Stop recursing if we reach a facet that has a tail facet whose order has
  // not yet been set.
  for (size_t i = 0; i < mTailFacets.size(); ++i) {
    tmFacet* theFacet = mTailFacets[i];
    size_t theOrder = theFacet->mOrder;
    if (theOrder == size_t(-1)) return;
  }
  
  // Still here? Then set the order and move on to all the head facets.
  mOrder = nextOrder++;
  for (size_t i = 0; i < mHeadFacets.size(); ++i)
    mHeadFacets[i]->CalcOrder(nextOrder);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Propagate the color from this facet to adjacent not-yet-oriented facets.
Calling this routine for a single facet will propagate the color assignment
throughout the crease pattern as long as folded-ness has been set.
*****/
void tmFacet::CalcColor(const Color aColor)
{
  TMASSERT(mColor == NOT_ORIENTED);
  mColor = aColor;
  for (size_t i = 0; i < mCreases.size(); ++i) {
    tmCrease* theCrease = mCreases[i];
    tmFacet* otherFacet = theCrease->GetOtherFacet(this);
    if (!otherFacet || otherFacet->mColor != NOT_ORIENTED) continue;
    switch(theCrease->GetKind()) {
      case tmCrease::AXIAL:
      case tmCrease::GUSSET:
      case tmCrease::RIDGE:
      case tmCrease::FOLDED_HINGE:
      case tmCrease::PSEUDOHINGE:
        otherFacet->CalcColor(OppositeColor(aColor));
        break;
      case tmCrease::UNFOLDED_HINGE:
        otherFacet->CalcColor(aColor);
        break;
      default:
        TMFAIL("In tmFacet::CalcColor() crease type was not defined");
    }
  }
}


/*****
Return the opposite color from the passed one. aColor must be either WHITE_UP
or COLOR_UP.
*****/
tmFacet::Color tmFacet::OppositeColor(const Color aColor)
{
  if (aColor == WHITE_UP) return COLOR_UP;
  if (aColor == COLOR_UP) return WHITE_UP;
  TMFAIL("In tmFacet::OppositeColor() facet color was not yet defined");
  return NOT_ORIENTED;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Put a tmFacet in version 5 format
*****/
void tmFacet::Putv5Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex);  
  PutPOD(os, mCentroid);
  PutPOD(os, mIsWellFormed);
  mTree->PutPtrArray(os, mVertices);
  mTree->PutPtrArray(os, mCreases);
  mTree->PutPtr(os, mCorridorEdge);
  mTree->PutPtrArray(os, mHeadFacets);
  mTree->PutPtrArray(os, mTailFacets);
  PutPOD(os, mOrder);
  PutPOD(os, int(mColor));
  mTree->PutOwnerPtr(os, mFacetOwner);
}


/*****
Get a tmFacet in version 5 format
*****/
void tmFacet::Getv5Self(istream& is)
{
  CheckTagStr<tmFacet>(is);
  GetPOD(is, mIndex);
  GetPOD(is, mCentroid);
  GetPOD(is, mIsWellFormed);
  mTree->GetPtrArray(is, mVertices);
  mTree->GetPtrArray(is, mCreases);
  mTree->GetPtr(is, mCorridorEdge, true);
  mTree->GetPtrArray(is, mHeadFacets);
  mTree->GetPtrArray(is, mTailFacets);
  GetPOD(is, mOrder);
  int color;
  GetPOD(is, color);
  mColor = Color(color);
  mTree->GetOwnerPtr(is, mFacetOwner);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmFacet, "fact")

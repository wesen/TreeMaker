/*******************************************************************************
File:         tmVertex.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmVertex class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmVertex.h"
#include "tmModel.h"

using namespace std;

/**********
class tmVertex
Class that represents a vertex of a crease pattern.
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


/*****
Return whether this vertex has three or more major creases (axial or ridge)
incident upon it.
*****/
bool tmVertex::IsMajorVertex() const
{
  return GetNumMajorCreases() > 2;
}


/*****
Return whether this vertex has two or fewer major creases (axial or ridge)
incident upon it.
*****/
bool tmVertex::IsMinorVertex() const
{
  return GetNumMajorCreases() <= 2;
}


#ifdef __MWERKS__
  #pragma mark --PRIVATE--
#endif


/*****
Common initialization
*****/
void tmVertex::InitVertex()
{
  // Register with tmTree 
  mTree->mVertices.push_back(this);
  mIndex = mTree->mVertices.size();

  // Initialize member data
  mLoc = tmPoint(0., 0.);
  mElevation = 0.;
  mIsBorderVertex = false;
  mTreeNode = 0;
  mLeftPseudohingeMate = 0;
  mRightPseudohingeMate = 0;
  mDepth = DEPTH_NOT_SET;
  mDiscreteDepth = size_t(-1);
  
  // owner
  mVertexOwner = 0;
}


/*****
Creator constructor
*****/
tmVertex::tmVertex(tmTree* aTree) : 
  tmPart(aTree), tmDpptrTarget()
{
  InitVertex();
}


/*****
Constructor
*****/
tmVertex::tmVertex(tmTree* aTree, tmVertexOwner* aVertexOwner, tmPoint aLoc,
    tmFloat aElevation, bool aIsBorderVertex, tmNode* aTreeNode) : 
  tmPart(aTree), tmDpptrTarget()
{
  InitVertex();
  
  // Register with Owner  
  mVertexOwner = aVertexOwner;
  mVertexOwner->mOwnedVertices.push_back(this);
  
  // Initialize member data
  mLoc = aLoc;
  mElevation = aElevation;
  mIsBorderVertex = aIsBorderVertex;
  mTreeNode = aTreeNode;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Return the owner as a tmNode*. Return 0 if the owner is something else (tmPath).
*****/
tmNode* tmVertex::GetOwnerAsNode()
{
  return mVertexOwner->VertexOwnerAsNode();
}


/*****
Return the owner as a tmPath*. Return 0 if the owner is something else (tmNode).
*****/
tmPath* tmVertex::GetOwnerAsPath()
{
  return mVertexOwner->VertexOwnerAsPath();
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Return the number of major creases incident on this vertex.
*****/
size_t tmVertex::GetNumMajorCreases() const
{
  size_t n = 0;
  for (size_t i = 0; i < mCreases.size(); ++i)
    if (mCreases[i]->IsMajorCrease()) n++;
  return n;
}


/*****
Return the number of hinge creases incident on this vertex.
*****/
size_t tmVertex::GetNumHingeCreases() const
{
  size_t n = 0;
  for (size_t i = 0; i < mCreases.size(); ++i)
    if (mCreases[i]->IsHingeCrease()) n++;
  return n;
}


/*****
Return true if this vertex is incident to an axial crease, which puts it on the
border of a tree poly. Note that this (and the next two routines) should only
be called AFTER all creases within the poly have been constructed.
*****/
bool tmVertex::IsAxialVertex() const
{
  TMASSERT(mCreases.size() > 0);
  for (size_t i = 0; i < mCreases.size(); ++i)
    if (mCreases[i]->IsAxialCrease()) return true;
  return false;
}


/*****
Return true if this vertex is incident to either an axial or gusset crease,
rather than only ridges and hinges.
*****/
bool tmVertex::IsAxialOrGussetVertex() const
{
  TMASSERT(mCreases.size() > 0);
  for (size_t i = 0; i < mCreases.size(); ++i) {
    tmCrease* theCrease = mCreases[i];
    if (theCrease->IsAxialOrGussetCrease()) return true;
  }
  return false;
}


/*****
Return true if this vertex is incident to at least one hinge crease.
*****/
bool tmVertex::IsHingeVertex() const
{
  for (size_t i = 0; i < mCreases.size(); ++i)
    if (mCreases[i]->IsHingeCrease()) return true;
  return false;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Return the number of creases in the given crease list that are incident on this
vertex.
*****/
size_t tmVertex::GetDegree(const tmArray<tmCrease*>& creaseList) const
{
  size_t degree = 0;
  for (size_t i = 0; i < mCreases.size(); ++i)
    if (creaseList.contains(mCreases[i])) ++degree;
  return degree;
}


/*****
Return the first incident hinge crease if it is known that this vertex is
a hinge vertex. Of course, if it's a border hinge vertex, then the hinge
crease returned will be the only hinge crease that is incident to the vertex.
*****/
tmCrease* tmVertex::GetHingeCrease() const
{
  TMASSERT(IsHingeVertex());
  for (size_t i = 0; i < mCreases.size(); ++i) {
    tmCrease* theCrease = mCreases[i];
    if (theCrease->IsHingeCrease()) return theCrease;
  }
  TMFAIL("tmVertex::GetHingeCrease(): couldn't find incident hinge crease");
  return 0;
}


/*****
Return the two creases to either side of this axial or gusset vertex that are
the axial or gusset creases.
*****/
void tmVertex::GetAxialOrGussetCreases(tmCrease*& crease1, 
  tmCrease*& crease2) const
{
  TMASSERT(IsAxialOrGussetVertex());
  crease1 = crease2 = 0;
  for (size_t i = 0; i < mCreases.size(); ++i) {
    tmCrease* theCrease = mCreases[i];
    if (theCrease->IsAxialOrGussetCrease())
      if (!crease1)
        crease1 = theCrease;
      else {
        TMASSERT(!crease2);
        crease2 = theCrease;
        return;
      }
  }
  TMFAIL("tmVertex::GetAxialOrGussetCreases(): "\
    "couldn't find axial or gusset creases");
}


/*****
Return the zero, one or two hinge creases on either side of this vertex.
*****/
void tmVertex::GetHingeCreases(tmCrease*& crease1, tmCrease*& crease2) const
{
  crease1 = 0;
  crease2 = 0;
  for (size_t i = 0; i < mCreases.size(); ++i) {
    tmCrease* theCrease = mCreases[i];
    if (theCrease->IsHingeCrease()) {
      if (!crease1) crease1 = theCrease;
      else if (!crease2) {
        crease2 = theCrease;
        return;
      }
      else TMFAIL("tmVertex::GetHingeCreases()");
    }
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Swap the links at this vertex from running along axial creases to running across
the axial creases. The vertex should be an axial hinge vertex. This is used to
connect facet graphs within a single connected component of the local root
network, and to splice together connected components if different local root
networks.
*****/
void tmVertex::SwapLinks()
{
  TMASSERT(IsAxialVertex());
  TMASSERT(IsHingeVertex());
  tmCrease *hingeCrease1, *hingeCrease2;
  GetHingeCreases(hingeCrease1, hingeCrease2);
  TMASSERT(hingeCrease1);
  TMASSERT(hingeCrease2);
  tmFacet* facetA = hingeCrease1->GetLeftFacet();
  tmFacet* facetB = hingeCrease1->GetRightFacet();
  tmFacet* facetC = hingeCrease2->GetRightFacet();
  tmFacet* facetD = hingeCrease2->GetLeftFacet();
  tmFacet::Unlink(facetA, facetB);
  tmFacet::Unlink(facetC, facetD);
  facetA->LinkTo(facetC);
  facetD->LinkTo(facetB);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Clear the data that gets recalculated during cleanup
*****/
void tmVertex::ClearCleanupData()
{
  mDepth = DEPTH_NOT_SET;
  mDiscreteDepth = size_t(-1);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
STATIC
This defines the tolerance for deciding whether two vertices are at the same
location, i.e., whether to create a new vertex or use the existing vertex.
*****/
bool tmVertex::VerticesSameLoc(const tmPoint& p1, const tmPoint& p2)
{
  // We use a looser tolerance on vertex spacing than tmPart::DistTol(), which
  // helps cut down on spurious closely-spaced creases. Note that this only
  // applies to vertices within a single path; two closely-spaced paths can
  // have vertices more closely-spaced than this.
//   const tmFloat VERTEX_TOL = .001; // 20051207 this was too close
  const tmFloat VERTEX_TOL = .003;
  return fabs(Mag(p1 - p2)) < VERTEX_TOL;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Put a tmVertex in version 5 format
*****/
void tmVertex::Putv5Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex);  
  PutPOD(os, mLoc);
  PutPOD(os, mElevation);
  PutPOD(os, mIsBorderVertex);
  mTree->PutPtr(os, mTreeNode);
  mTree->PutPtr(os, mLeftPseudohingeMate);
  mTree->PutPtr(os, mRightPseudohingeMate);
  mTree->PutPtrArray(os, mCreases);
  PutPOD(os, mDepth);
  PutPOD(os, mDiscreteDepth);
  PutPOD(os, mCCFlag);
  PutPOD(os, mSTFlag);
  mTree->PutOwnerPtr(os, mVertexOwner);
}


/*****
Get a tmVertex in version 5 format
*****/
void tmVertex::Getv5Self(istream& is)
{
  CheckTagStr<tmVertex>(is);
  GetPOD(is, mIndex);
  GetPOD(is, mLoc);
  GetPOD(is, mElevation);
  GetPOD(is, mIsBorderVertex);
  mTree->GetPtr(is, mTreeNode, true);
  mTree->GetPtr(is, mLeftPseudohingeMate, true);
  mTree->GetPtr(is, mRightPseudohingeMate, true);
  mTree->GetPtrArray(is, mCreases); 
  GetPOD(is, mDepth);
  GetPOD(is, mDiscreteDepth);
  GetPOD(is, mCCFlag);
  GetPOD(is, mSTFlag);
  mTree->GetOwnerPtr(is, mVertexOwner);
}


/*****
Put a tmVertex in version 4 format
*****/
void tmVertex::Putv4Self(ostream& os)
{
  TMFAIL("tmVertex::Putv4Self");
}


/*****
Get a tmVertex in version 4 format
*****/
void tmVertex::Getv4Self(istream& is)
{
  CheckTagStr<tmVertex>(is);  
  GetPOD(is, mLoc); 
  mTree->GetPtrArray(is, mCreases); 
  mTree->GetOwnerPtr(is, mVertexOwner);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmVertex, "vrtx")

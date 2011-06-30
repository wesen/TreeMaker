/*******************************************************************************
File:         tmFacetOwner.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmFacetOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-24
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmFacetOwner.h"
#include "tmModel.h"

#ifdef TMDEBUG
  #include <fstream>
#endif

using namespace std;

/*
tmFacetOwner keeps a list of tmFacets that it owns in a list; when this object
is deleted, it automatically deletes all the objects in the list. Similarly,
when a tmFacet is deleted, it tells its owner, which removes it from the list.

Note that tmFacetOwner has two methods, BuildFacetsFromCreases() and
GetNextCreaseAndVertex(), which are closely analogous to tmPolyOwner's
BuildPolysFromPaths() and GetNextPathAndNode().
*/

/**********
class tmFacetOwner
Base class for an object that owns facets and is responsible for their deletion.
Subclasses: tmPoly
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


#ifdef __MWERKS__
  #pragma mark --PROTECTED--
#endif


/*****
Destructor deletes all owned facets
*****/
tmFacetOwner::~tmFacetOwner()
{
  mOwnedFacets.KillItems();
}


#ifdef __MWERKS__
  #pragma mark --PRIVATE--
#endif


/*****
Return true if this crease is eligible to start a facet going forward. We will
only start facets on non-axial creases; this guarantees that the facet will be
inside of the poly.
*****/
bool tmFacetOwner::CanStartFacetFwd(tmCrease* aCrease)
{
  if (aCrease->mFwdFacet) return false;
  if (!aCrease->IsAxialCrease()) return true;
  return AreCCW(aCrease->mVertices.front()->mLoc, 
    aCrease->mVertices.back()->mLoc, FacetOwnerAsPoly()->mCentroid);
}


/*****
Return true if this crease is eligible to start a facet going backward. As with
CanStartFacetBkd(), we will only start facets on non-axial creases.
*****/
bool tmFacetOwner::CanStartFacetBkd(tmCrease* aCrease)
{
  if (aCrease->mBkdFacet) return false;
  if (!aCrease->IsAxialCrease()) return true;
  return AreCW(aCrease->mVertices.front()->mLoc, 
    aCrease->mVertices.back()->mLoc, FacetOwnerAsPoly()->mCentroid);
}


/*****
Return false if the network contains pairs of creases that intersect in their
interiors, which implies that the embedding of the crease graph is nonplanar.
If this happens (due to numerical roundoff, usually), we can't construct facets
because our algorithm (like the polygon algorithm) assumes planarity.
*****/
bool tmFacetOwner::CalcHasPlanarCreases(tmArray<tmCrease*>& aCreaseList)
{
  for (size_t i = 1; i < aCreaseList.size(); ++i) {
    tmCrease* aCrease = aCreaseList[i];
    // Check for intersection against all preceding creases.
    for (size_t j = 0; j < i; ++j) {
      tmCrease* bCrease = aCreaseList[j];
      if (aCrease->IntersectsInterior(bCrease)) {
#ifdef TMDEBUG
        // This should never arise in practice, so write diagnostic data to a
        // file to aid debugging
        ofstream fout("BAD_HAS_PLANAR_CREASES.txt");
        fout << "crease A = " << aCrease->mIndex << endl;
        fout << "crease A, front vertex = " << 
          aCrease->mVertices.front()->mIndex << endl;
		fout << "(" << aCrease->mVertices.front()->mLoc.x << "," << aCrease->mVertices.front()->mLoc.y << ")" << endl;
        fout << "crease A, back vertex =  " << 
          aCrease->mVertices.back()->mIndex << endl;
		fout << "(" << aCrease->mVertices.back()->mLoc.x << "," << aCrease->mVertices.back()->mLoc.y << ")" << endl;
        fout << "crease B = " << 
          bCrease->mIndex << endl;
        fout << "crease B, front vertex = " << 
          bCrease->mVertices.front()->mIndex << endl;
		fout << "(" << bCrease->mVertices.front()->mLoc.x << "," << bCrease->mVertices.front()->mLoc.y << ")" << endl;
        fout << "crease B, back vertex =  " << 
          bCrease->mVertices.back()->mIndex << endl;
		fout << "(" << bCrease->mVertices.back()->mLoc.x << "," << bCrease->mVertices.back()->mLoc.y << ")" << endl;
        fout.close();
#endif // TMDEBUG
        return false;
      }
    }
  }
  return true;  
}


/*****
Find the next crease emanating from thisVertex after thisCrease, going
counterclockwise. Also return the tmVertex (nextVertex) at the opposite end of
the crease. 
*****/
void tmFacetOwner::GetNextCreaseAndVertex(tmCrease* thisCrease, 
  tmVertex* thisVertex,  tmCrease*& nextCrease, tmVertex*& nextVertex)
{
  // Get the angle of thisCrease.
  tmVertex* thatVertex = thisCrease->mVertices.front();
  if (thatVertex == thisVertex) thatVertex = thisCrease->mVertices.back();
  tmFloat thisAngle = Angle(thatVertex->mLoc - thisVertex->mLoc);
  
  // delta is the increment in angle from thisCrease to nextCrease. Start with
  // the largest possible value. Then go through each of the Creases emanating
  // from thisVertex and compare its angle to the angle of thisCrease. We'll
  // keep the smallest positive angle we find.
  tmFloat delta = TWO_PI;
  nextCrease = thisCrease;
  nextVertex = 0;
  for (size_t i = 0; i < thisVertex->mCreases.size(); ++i) {
    tmCrease* thatCrease = thisVertex->mCreases[i];
    if (thatCrease == thisCrease) continue;
    
    // Get the angle of thatCrease and the tmVertex at the other end of
    // thatCrease (thatVertex).
    thatVertex = thatCrease->mVertices.front();
    if (thatVertex == thisVertex) thatVertex = thatCrease->mVertices.back();
    tmFloat nextAngle = Angle(thatVertex->mLoc - thisVertex->mLoc);
    
      // Find the angular increment to the new crease. Constrain it to lie
      // between 0 and 2 PI.
    tmFloat newDelta = thisAngle - nextAngle;
    while (newDelta < 0) newDelta += TWO_PI;
    while (newDelta >= TWO_PI) newDelta -= TWO_PI;
    
    // If the angular increment is less than our current champion, we'll
    // replace the current values with those of the new tmCrease. When we're
    // done, we'll be left with the tmCrease that makes the smallest angle with
    // the current tmCrease.
    if (newDelta < delta) {
      delta = newDelta;
      nextCrease = thatCrease;
      nextVertex = thatVertex;
    }
  }
  
  // Make sure we found something
  TMASSERT(nextCrease != thisCrease);
  TMASSERT(nextVertex);
}


/*****
Starting with a list of creases, construct a complete network of
counterclockwise facets with their creases and vertices.
*****/
void tmFacetOwner::BuildFacetsFromCreases(tmArray<tmCrease*>& aCreaseList)
{
  // Nothing to build if there are no creases
  if (aCreaseList.empty()) return;
  
  // We shouldn't even try if there are intersecting creases, and the check
  // should happen outside of this routine.
  TMASSERT(CalcHasPlanarCreases(aCreaseList));

#ifdef TMDEBUG
  // Next, a check for validity of the vertex and crease information. Every
  // vertex should have at least 2 incident creases in our crease list.
  tmTree* theTree = aCreaseList.front()->mTree;
  for (size_t i = 0; i < theTree->mVertices.size(); ++i)
    TMASSERT(theTree->mVertices[i]->mCreases.size() >= 2);
#endif // TMDEBUG
  
  // Now build new facets. Each crease should point to exactly two facets via
  // the mFwdFacet and mBkdFacet members (border creases point to only 1 facet
  // from our set, although they may point to facets owned by other
  // tmFacetOwners (i.e., other polys)). We build facets by cycling through
  // creases and starting a facet if either mFwdFacet or mBkdFacet are unused
  // and are pointing the right direction.
  tmVertex *firstVertex, *thisVertex, *nextVertex;
  tmCrease *thisCrease, *nextCrease;

  for (size_t i = 0; i < aCreaseList.size(); ++i) {
    tmCrease* aCrease = aCreaseList[i];
    if (CanStartFacetFwd(aCrease)) {
      // build up a facet in the forward direction
      tmFacet* fwdFacet = new tmFacet(FacetOwnerAsPoly());
      aCrease->mFwdFacet = fwdFacet;
      firstVertex = aCrease->mVertices.front();
      thisCrease = aCrease;
      thisVertex = aCrease->mVertices.back();
      fwdFacet->mVertices.push_back(firstVertex);
      fwdFacet->mCreases.push_back(thisCrease);
      size_t tooMany = 0;
      do {
        GetNextCreaseAndVertex(thisCrease, thisVertex, nextCrease, nextVertex);
        TMASSERT(!fwdFacet->mCreases.contains(nextCrease));
        TMASSERT((!fwdFacet->mVertices.contains(nextVertex)) || \
          nextVertex == firstVertex);
        fwdFacet->mVertices.push_back(thisVertex);
        fwdFacet->mCreases.push_back(nextCrease);
        if (nextCrease->mVertices.front() == thisVertex) {
          TMASSERT(nextCrease->mFwdFacet == 0);
          nextCrease->mFwdFacet = fwdFacet;
        }
        else {
          TMASSERT(nextCrease->mBkdFacet == 0);
          nextCrease->mBkdFacet = fwdFacet;
        }
        thisCrease = nextCrease;
        thisVertex = nextVertex;
        tooMany++;
        TMASSERT(tooMany < 100);  // To avoid infinite loops
      } while (nextVertex != firstVertex);
      fwdFacet->CalcContents();
    }
    if (CanStartFacetBkd(aCrease)) {
      // build up a facet in the backward direction
      tmFacet* bkdFacet = new tmFacet(FacetOwnerAsPoly());
      aCrease->mBkdFacet = bkdFacet;
      firstVertex = aCrease->mVertices.back();
      thisCrease = aCrease;
      thisVertex = aCrease->mVertices.front();
      bkdFacet->mVertices.push_back(firstVertex);
      bkdFacet->mCreases.push_back(thisCrease);
      size_t tooMany = 0;
      do {
        GetNextCreaseAndVertex(thisCrease, thisVertex, nextCrease, nextVertex);
        TMASSERT(!bkdFacet->mCreases.contains(nextCrease));
        TMASSERT((!bkdFacet->mVertices.contains(nextVertex)) || \
          nextVertex == firstVertex);
        bkdFacet->mVertices.push_back(thisVertex);
        bkdFacet->mCreases.push_back(nextCrease);
        if (nextCrease->mVertices.front() == thisVertex) 
        {
          TMASSERT(nextCrease->mFwdFacet == 0);
          nextCrease->mFwdFacet = bkdFacet;
        }
        else {
          TMASSERT(nextCrease->mBkdFacet == 0);
          nextCrease->mBkdFacet = bkdFacet;
        }
        thisCrease = nextCrease;
        thisVertex = nextVertex;
        tooMany++;
        TMASSERT(tooMany < 100);
        
      } while (nextVertex != firstVertex);
      bkdFacet->CalcContents();
    }
  }
}

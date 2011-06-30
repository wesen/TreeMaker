/*******************************************************************************
File:         tmFacetOwner.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmFacetOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-24
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMFACETOWNER_H_
#define _TMFACETOWNER_H_

// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmDpptrArray.h"
#include "tmFacet.h"


/**********
class tmFacetOwner
Base class for an object that owns facets and is responsible for their deletion.
Subclasses: tmPoly
**********/
class tmFacetOwner {
public:
  const tmDpptrArray<tmFacet>& GetOwnedFacets() const {
    // Return read-only list of all owned facets.
    return mOwnedFacets;
  };
  
protected:
  virtual ~tmFacetOwner();
  virtual tmPoly* FacetOwnerAsPoly() = 0;
  
private:
  tmDpptrArray<tmFacet> mOwnedFacets;
  
  // Facet construction routines
  bool CanStartFacetFwd(tmCrease* aCrease);
  bool CanStartFacetBkd(tmCrease* aCrease);
  bool CalcHasPlanarCreases(tmArray<tmCrease*>& aCreaseList);
  void GetNextCreaseAndVertex(tmCrease* thisCrease, tmVertex* thisVertex,
    tmCrease*& nextCrease, tmVertex*& nextVertex);
  void BuildFacetsFromCreases(tmArray<tmCrease*>& aCreaseList);
    
  friend class tmTree;
  friend class tmFacet;
  friend class tmPoly;
};


#endif // _TMFACETOWNER_H_

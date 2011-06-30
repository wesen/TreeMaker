/*******************************************************************************
File:         tmCreaseOwner.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmCreaseOwner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCREASEOWNER_H_
#define _TMCREASEOWNER_H_

// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmDpptrArray.h"
#include "tmCrease.h"


/**********
class tmCreaseOwner
Base class for an object that owns creases and is responsible for their
deletion.
Subclasses: tmPath, tmPoly
**********/
class tmCreaseOwner {
public:
  const tmDpptrArray<tmCrease>& GetOwnedCreases() const {
    // Return read-only list of all owned creases.
    return mOwnedCreases;
  };
  
protected:
  virtual ~tmCreaseOwner();
  
private:
  tmDpptrArray<tmCrease> mOwnedCreases;

  virtual tmPath* CreaseOwnerAsPath() = 0;
  virtual tmPoly* CreaseOwnerAsPoly() = 0;

  tmCrease* GetOrMakeCrease(tmVertex* aVertex1, tmVertex* aVertex2, 
    tmCrease::Kind aKind);
  tmCrease* GetCrease(tmVertex* aVertex1, tmVertex* aVertex2);
  
  friend class tmTree;
  friend class tmPath;
  friend class tmPoly;
  friend class tmCrease;
};


#endif // _TMCREASEOWNER_H_

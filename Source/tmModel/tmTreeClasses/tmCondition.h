/*******************************************************************************
File:         tmCondition.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmCondition class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _CONDITION_H_
#define _CONDITION_H_

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
class  tmCondition
Abstract base class for objects that place conditions on a tree or its parts.
**********/
class tmCondition : public tmPart, public tmDpptrTarget {
public: 
  // Queries
  bool IsFeasibleCondition() const {
    return mIsFeasibleCondition;};
  virtual bool IsNodeCondition() const = 0;
  virtual bool IsEdgeCondition() const = 0;
  virtual bool IsPathCondition() const = 0;
  const tmConditionOwner* GetConditionOwner() const {
    return mConditionOwner;};    

  // Constraints  
  virtual void AddConstraints(tmScaleOptimizer* t) = 0;
  virtual void AddConstraints(tmEdgeOptimizer* t) = 0;
  virtual void AddConstraints(tmStrainOptimizer* t) = 0;

protected:
  // Feasibility is member, not computed, so can be quickly queried
  bool mIsFeasibleCondition;  

  // Subclasses and friend part classes need access to ctor & dtor
  tmCondition(tmTree* aTree);
  virtual ~tmCondition(); 

  // Subclasses must implement these
  virtual bool Uses(tmPart* aPart) const = 0;
  virtual bool IsValidCondition() const = 0;
  virtual void CalcFeasibility() = 0;
private:
  // owner
  tmConditionOwner* mConditionOwner;

  // Stream I/O
  virtual std::size_t GetNumLinesRest() = 0;
  virtual void PutRestv4(std::ostream& os) = 0;
  virtual void GetRestv4(std::istream& is) = 0; 

  // Friend classes     
  friend class tmTree;
  friend class tmDpptrArray<tmCondition>;
};


#endif // _CONDITION_H_

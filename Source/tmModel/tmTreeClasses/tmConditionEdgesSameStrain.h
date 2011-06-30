/*******************************************************************************
File:         tmConditionEdgesSameStrain.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionEdgesSameStrain class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONEDGESSAMESTRAIN_H_
#define _TMCONDITIONEDGESSAMESTRAIN_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmCondition.h"
#include "tmDpptr.h"


/**********
class tmConditionEdgesSameStrain
A condition for forcing two edges to have the same strain in a strain
optimization.
**********/
class tmConditionEdgesSameStrain : public tmCondition
{
public:
  // Getters
  tmEdge* GetEdge1() const {return mEdge1;};
  tmEdge* GetEdge2() const {return mEdge2;};

  // Setters
  void SetEdge1(tmEdge* aEdge);
  void SetEdge2(tmEdge* aEdge);

  // Queries
  bool IsNodeCondition() const {return false;};
  bool IsEdgeCondition() const {return true;};
  bool IsPathCondition() const {return false;};

private:
  tmDpptr<tmEdge> mEdge1;   // Edges to have same strain
  tmDpptr<tmEdge> mEdge2;

  // Constructors
  tmConditionEdgesSameStrain(tmTree* aTree);
  tmConditionEdgesSameStrain(tmTree* aTree, tmEdge* aEdge1, tmEdge* aEdge2);

  // Miscellaneous utilities
  bool Uses(tmPart* aPart) const;
  bool IsValidCondition() const;
  void CalcFeasibility();
  void AddConstraints(tmScaleOptimizer* t);
  void AddConstraints(tmEdgeOptimizer* t);
  void AddConstraints(tmStrainOptimizer* t);

  // Stream I/O
  std::size_t GetNumLinesRest();
  void PutRestv4(std::ostream& os);
  void GetRestv4(std::istream& is);

  // Class tag for stream I/O
  TM_DECLARE_TAG()

  // Friend classes
  friend class tmPart;
  friend class tmPart::StringT<tmConditionEdgesSameStrain>;
  friend class tmPart::CreatorFnT<tmConditionEdgesSameStrain>;
  friend class tmTree;
  friend class tmEdge;
};


#endif // _TMCONDITIONEDGESSAMESTRAIN_H_

/*******************************************************************************
File:         tmConditionNodesPaired.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodesPaired class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONNODESPAIRED_H_
#define _TMCONDITIONNODESPAIRED_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmCondition.h"
#include "tmDpptr.h"


/**********
class tmConditionNodesPaired
A condition for pairing 2 nodes about the symmetry line
**********/
class tmConditionNodesPaired : public tmCondition {
public:
  // Getters
  tmNode* GetNode1() const {return mNode1;};
  tmNode* GetNode2() const {return mNode2;};

  // Setters
  void SetNode1(tmNode* aNode);
  void SetNode2(tmNode* aNode);

  // Queries
  bool IsNodeCondition() const {return true;};
  bool IsEdgeCondition() const {return false;};
  bool IsPathCondition() const {return false;};

private:
  tmDpptr<tmNode> mNode1;
  tmDpptr<tmNode> mNode2;

  // Constructors
  tmConditionNodesPaired(tmTree* aTree);
  tmConditionNodesPaired(tmTree* aTree, tmNode* aNode1, tmNode* aNode2);

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
  friend class tmPart::StringT<tmConditionNodesPaired>;
  friend class tmPart::CreatorFnT<tmConditionNodesPaired>;
  friend class tmTree;
  friend class tmNode;
};

#endif // _TMCONDITIONNODESPAIRED_H_

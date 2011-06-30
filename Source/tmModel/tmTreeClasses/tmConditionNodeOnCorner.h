/*******************************************************************************
File:         tmConditionNodeOnCorner.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodeOnCorner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONNODEONCORNER_H_
#define _TMCONDITIONNODEONCORNER_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmCondition.h"
#include "tmDpptr.h"


/**********
class tmConditionNodeOnCorner
A condition for fixing a node to any corner of the paper
**********/
class tmConditionNodeOnCorner : public tmCondition {
public:
  // Getter
  tmNode* GetNode() const {return mNode;};

  // Setter
  void SetNode(tmNode* aNode);

  // Queries
  bool IsNodeCondition() const {return true;};
  bool IsEdgeCondition() const {return false;};
  bool IsPathCondition() const {return false;};

private:
  tmDpptr<tmNode> mNode;

  // Constructor
  tmConditionNodeOnCorner(tmTree* aTree);
  tmConditionNodeOnCorner(tmTree* aTree, tmNode* aNode);

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
  friend class tmPart::StringT<tmConditionNodeOnCorner>;
  friend class tmPart::CreatorFnT<tmConditionNodeOnCorner>;
  friend class tmTree;
};

#endif // _TMCONDITIONNODEONCORNER_H_

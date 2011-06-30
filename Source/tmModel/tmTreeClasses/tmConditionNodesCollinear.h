/*******************************************************************************
File:         tmConditionNodesCollinear.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodesCollinear class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONNODESCOLLINEAR_H_
#define _TMCONDITIONNODESCOLLINEAR_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmCondition.h"
#include "tmDpptr.h"


/**********
class tmConditionNodesCollinear
A condition for making three nodes collinear
**********/
class tmConditionNodesCollinear : public tmCondition {
public:
  // Getters
  tmNode* GetNode1() const {return mNode1;};
  tmNode* GetNode2() const {return mNode2;};
  tmNode* GetNode3() const {return mNode3;};

  // Setters
  void SetNode1(tmNode* aNode);
  void SetNode2(tmNode* aNode);
  void SetNode3(tmNode* aNode);

  // Queries
  bool IsNodeCondition() const {return true;};
  bool IsEdgeCondition() const {return false;};
  bool IsPathCondition() const {return false;};

private:
  tmDpptr<tmNode> mNode1;
  tmDpptr<tmNode> mNode2;
  tmDpptr<tmNode> mNode3;

  // Constructor
  tmConditionNodesCollinear(tmTree* aTree, tmNode* aNode1 = 0, 
    tmNode* aNode2 = 0, tmNode* aNode3 = 0);

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
  friend class tmPart::StringT<tmConditionNodesCollinear>;
  friend class tmPart::CreatorFnT<tmConditionNodesCollinear>;
  friend class tmTree;
  friend class tmNode;
};


#endif // _TMCONDITIONNODESCOLLINEAR_H_

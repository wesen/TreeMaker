/*******************************************************************************
File:         tmConditionNodeFixed.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodeFixed class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONNODEFIXED_H_
#define _TMCONDITIONNODEFIXED_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmCondition.h"
#include "tmDpptr.h"
#include "tmPoint.h"


/**********
class tmConditionNodeFixed
A condition for forcing one or both coordinates of a node to a fixed value.
**********/
class tmConditionNodeFixed : public tmCondition {
public:
  // Constructor
  tmConditionNodeFixed(tmTree* aTree, tmNode* aNode = 0, 
    bool aXFixed = false, const tmFloat& aXFixValue = 0, 
    bool aYFixed = false, const tmFloat& aYFixValue = 0);

  // Getters
  bool GetXFixed() const {return mXFixed;};
  bool GetYFixed() const {return mYFixed;};
  const tmFloat& GetXFixValue() const {return mXFixValue;};
  const tmFloat& GetYFixValue() const {return mYFixValue;};
  tmNode* GetNode() const {return mNode;};

  // Setters
  void SetXFixed(bool aXFixed);
  void SetYFixed(bool aYFixed);
  void SetXFixValue(const tmFloat& aXFixValue);
  void SetYFixValue(const tmFloat& aYFixValue);
  void SetNode(tmNode* aNode);

  // Queries
  bool IsNodeCondition() const {return true;};
  bool IsEdgeCondition() const {return false;};
  bool IsPathCondition() const {return false;};

private:
  // Member data
  tmDpptr<tmNode> mNode;
  bool mXFixed;
  tmFloat mXFixValue;
  bool mYFixed;
  tmFloat mYFixValue;

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
  friend class tmPart::StringT<tmConditionNodeFixed>;
  friend class tmPart::CreatorFnT<tmConditionNodeFixed>;
  friend class tmTree;
  friend class tmNode;
};

#endif // _TMCONDITIONNODEFIXED_H_

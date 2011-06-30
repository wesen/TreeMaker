/*******************************************************************************
File:         tmConditionPathActive.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionPathActive class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONPATHACTIVE_H_
#define _TMCONDITIONPATHACTIVE_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmCondition.h"
#include "tmDpptr.h"


/**********
class tmConditionPathActive
A condition for making a path active 
**********/
class tmConditionPathActive : public tmCondition {
public:
  // Getters
  tmPath* GetPath() const {return mPath;};
  tmNode* GetNode1() const {return mNode1;};
  tmNode* GetNode2() const {return mNode2;};  

  // Setters
  void SetPath(tmPath* aPath);
  void SetNodes(tmNode* aNode1, tmNode* aNode2);
  void SetNode1(tmNode* aNode1);
  void SetNode2(tmNode* aNode2);

  // Queries
  bool IsNodeCondition() const {return false;};
  bool IsEdgeCondition() const {return false;};
  bool IsPathCondition() const {return true;};

protected:  
  // Constructors
  tmConditionPathActive(tmTree* aTree);
  tmConditionPathActive(tmTree* aTree, tmPath* aPath);

  // Further implemented by subclasses
  bool Uses(tmPart* aPart) const;
  bool IsValidCondition() const;
  void CalcFeasibility();

private:
  tmDpptr<tmPath> mPath;
  tmDpptr<tmNode> mNode1;
  tmDpptr<tmNode> mNode2;

  // Miscellaneous utilities
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
  friend class tmPart::StringT<tmConditionPathActive>;
  friend class tmPart::CreatorFnT<tmConditionPathActive>;
  friend class tmTree;
  friend class tmPath;
  friend class tmConditionPathAngleFixed;
  friend class tmConditionPathAngleQuant;
};

#endif // _TMCONDITIONPATHACTIVE_H_

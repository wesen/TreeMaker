/*******************************************************************************
File:         tmConditionPathCombo.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionPathCombo class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONPATHCOMBO_H_
#define _TMCONDITIONPATHCOMBO_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmCondition.h"
#include "tmDpptr.h"


/**********
class tmConditionPathCombo
A condition for making a path active with or without angle constraints
**********/
class tmConditionPathCombo : public tmCondition {
public:
  // Constructors
  tmConditionPathCombo(tmTree* aTree);
  tmConditionPathCombo(tmTree* aTree, tmPath* aPath);

  // Getters
  tmPath* GetPath() const {return mPath;};
  tmNode* GetNode1() const {return mNode1;};
  tmNode* GetNode2() const {return mNode2;};
  bool IsActiveOnly() const {return !mIsAngleFixed && !mIsAngleQuant;};
  bool IsAngleFixed() const {return mIsAngleFixed;};
  tmFloat GetAngle() const {return mAngle;};
  bool IsAngleQuant() const {return mIsAngleQuant;};
  std::size_t GetQuant() const {return mQuant;};
  tmFloat GetQuantOffset() const {return mQuantOffset;};

  // Setters
  void SetPath(tmPath* aPath);
  void SetNodes(tmNode* aNode1, tmNode* aNode2);
  void SetNode1(tmNode* aNode1);
  void SetNode2(tmNode* aNode2);
  void SetAngleFixed(bool aAngleFixed);
  void SetAngle(const tmFloat& aAngle);
  void SetAngleQuant(bool aAngleQuant);
  void SetQuant(std::size_t aQuant);
  void SetQuantOffset(const tmFloat& aQuantOffset);

  // Queries
  bool IsNodeCondition() const {return false;};
  bool IsEdgeCondition() const {return false;};
  bool IsPathCondition() const {return true;};

private:
  tmDpptr<tmPath> mPath;
  tmDpptr<tmNode> mNode1;
  tmDpptr<tmNode> mNode2;
  bool mIsAngleFixed;
  tmFloat mAngle;
  bool mIsAngleQuant;
  std::size_t mQuant;
  tmFloat mQuantOffset; 

  // Miscellaneous utilities
  void InitConditionPathCombo();
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
  friend class tmPart::StringT<tmConditionPathCombo>;
  friend class tmPart::CreatorFnT<tmConditionPathCombo>;
  friend class tmTree;
  friend class tmPath;
};

#endif // _TMCONDITIONPATHCOMBO_H_

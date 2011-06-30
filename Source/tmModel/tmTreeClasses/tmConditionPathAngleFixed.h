/*******************************************************************************
File:         tmConditionPathAngleFixed.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionPathAngleFixed class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONPATHANGLEFIXED_H_
#define _TMCONDITIONPATHANGLEFIXED_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmConditionPathActive.h"
#include "tmDpptr.h"


/**********
struct ConditionPathAngleFixedInfo
Class that holds additional information needed to define the condition
**********/
struct ConditionPathAngleFixedInfo {
};


/**********
class tmConditionPathAngleFixed
A condition for making a path active at a fixed angle 
**********/
class tmConditionPathAngleFixed : public tmConditionPathActive {
public:
  // Getter
  const tmFloat& GetAngle() const {return mAngle;}; 

  // Setter
  void SetAngle(const tmFloat& aAngle);

private:
  tmFloat mAngle; 

  // Constructors
  tmConditionPathAngleFixed(tmTree* aTree);
  tmConditionPathAngleFixed(tmTree* aTree, tmPath* aPath, 
    const tmFloat& aAngle = 0);

  // Miscellaneous utilities
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
  friend class tmPart::StringT<tmConditionPathAngleFixed>;
  friend class tmPart::CreatorFnT<tmConditionPathAngleFixed>;
  friend class tmTree;
  friend class tmPath;
};

#endif // _TMCONDITIONPATHANGLEFIXED_H_

/*******************************************************************************
File:         tmConditionPathAngleQuant.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionPathAngleQuant class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONPATHANGLEQUANT_H_
#define _TMCONDITIONPATHANGLEQUANT_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmConditionPathActive.h"
#include "tmDpptr.h"


/**********
class tmConditionPathAngleQuant
A condition for making a path active with quantized angle
**********/
class tmConditionPathAngleQuant : public tmConditionPathActive {
public:
  // Getters
  std::size_t GetQuant() const {return mQuant;};
  const tmFloat& GetQuantOffset() const {return mQuantOffset;}; 

  // Setters
  void SetQuant(std::size_t aQuant);
  void SetQuantOffset(const tmFloat& aQuantOffset);

private:
  // Angle quantization is n * PI / mQuant + mQuantOffset
  std::size_t mQuant;
  tmFloat mQuantOffset; 

  // Constructors
  tmConditionPathAngleQuant(tmTree* aTree);
  tmConditionPathAngleQuant(tmTree* aTree, tmPath* aPath, 
    std::size_t aQuant = 8, const tmFloat& aQuantOffset = 0.0);

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
  friend class tmPart::StringT<tmConditionPathAngleQuant>;
  friend class tmPart::CreatorFnT<tmConditionPathAngleQuant>;
  friend class tmTree;
  friend class tmPath;
};

#endif // _TMCONDITIONPATHANGLEQUANT_H_

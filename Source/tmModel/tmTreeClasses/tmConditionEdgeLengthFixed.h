/*******************************************************************************
File:         tmConditionEdgeLengthFixed.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionEdgeLengthFixed class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONEDGELENGTHFIXED_H_
#define _TMCONDITIONEDGELENGTHFIXED_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmCondition.h"
#include "tmDpptr.h"


/**********
class tmConditionEdgeLengthFixed
A condition for fixing the length of an edge to its unstrained value
**********/
class tmConditionEdgeLengthFixed : public tmCondition {
public:
  // Getter
  tmEdge* GetEdge() const {return mEdge;};

  // Setter
  void SetEdge(tmEdge* aEdge);

  // Queries
  bool IsNodeCondition() const {return false;};
  bool IsEdgeCondition() const {return true;};
  bool IsPathCondition() const {return false;};

private:
  tmDpptr<tmEdge> mEdge;    // edge to fix  

  // Constructors
  tmConditionEdgeLengthFixed(tmTree* aTree);
  tmConditionEdgeLengthFixed(tmTree* aTree, tmEdge* aEdge);

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
  friend class tmPart::StringT<tmConditionEdgeLengthFixed>;
  friend class tmPart::CreatorFnT<tmConditionEdgeLengthFixed>;
  friend class tmTree;
  friend class tmEdge;
};


#endif // _TMCONDITIONEDGELENGTHFIXED_H_

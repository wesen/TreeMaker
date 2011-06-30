/*******************************************************************************
File:         tmConditionNodeCombo.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodeCombo class
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-22
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONDITIONNODECOMBO_H_
#define _TMCONDITIONNODECOMBO_H_

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
class tmConditionNodeCombo
A condition for placing several types of constraint on one or both coordinates
of a node.
**********/
class tmConditionNodeCombo : public tmCondition {
public:
  // Constructor
  tmConditionNodeCombo(tmTree* aTree, tmNode* aNode = 0,
    bool aToSymmetryLine = false,
    bool aToPaperEdge = false,
    bool aToPaperCorner = false,
    bool aXFixed = false, const tmFloat& aXFixValue = 0, 
    bool aYFixed = false, const tmFloat& aYFixValue = 0);

  // Getters
  bool GetToSymmetryLine() const {return mToSymmetryLine;};
  bool GetToPaperEdge() const {return mToPaperEdge;};
  bool GetToPaperCorner() const {return mToPaperCorner;};
  bool GetXFixed() const {return mXFixed;};
  const tmFloat& GetXFixValue() const {return mXFixValue;};
  bool GetYFixed() const {return mYFixed;};
  const tmFloat& GetYFixValue() const {return mYFixValue;};
  tmNode* GetNode() const {return mNode;};

  // Setters
  void SetToSymmetryLine(bool aToSymmetryLine);
  void SetToPaperEdge(bool aToPaperEdge);
  void SetToPaperCorner(bool aToPaperCorner);
  void SetXFixed(bool aXFixed);
  void SetXFixValue(const tmFloat& aXFixValue);
  void SetYFixed(bool aYFixed);
  void SetYFixValue(const tmFloat& aYFixValue);
  void SetNode(tmNode* aNode);

  // Queries
  bool IsNodeCondition() const {return true;};
  bool IsEdgeCondition() const {return false;};
  bool IsPathCondition() const {return false;};

private:
  // Member data
  bool mToSymmetryLine;
  bool mToPaperEdge;
  bool mToPaperCorner;
  bool mXFixed;
  tmFloat mXFixValue;
  bool mYFixed;
  tmFloat mYFixValue;
  tmDpptr<tmNode> mNode;

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
  friend class tmPart::StringT<tmConditionNodeCombo>;
  friend class tmPart::CreatorFnT<tmConditionNodeCombo>;
  friend class tmTree;
  friend class tmNode;
};

#endif // _TMCONDITIONNODECOMBO_H_

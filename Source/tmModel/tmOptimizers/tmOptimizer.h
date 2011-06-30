/*******************************************************************************
File:         tmOptimizer.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmOptimizer class for TreeMaker optimizers
Author:       Robert J. Lang
Modified by:  
Created:      2005-09-13
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMOPTIMIZER_H_
#define _TMOPTIMIZER_H_

// Standard libraries
#include <vector>
#include <sstream>

// TreeMaker model
#include "tmTreeCleaner.h"

// Forward declarations
class tmNLCO;

/**********
class tmOptimizer
Base class for TreeMaker optimizers. Provides two virtual methods that all
subclasses must support, used for transferring data between optimizer and tree.
This class inherits from class tmTreeCleaner, which will delay cleanup until
the optimizer goes completely out of scope or is destroyed, which means that
even if we've run the optimizer, we can Revert() and not lose any crease
patterns, etc.
**********/

class tmOptimizer : public tmTreeCleaner {
public:
  tmOptimizer(tmTree* aTree, tmNLCO* aNLCO);
  virtual ~tmOptimizer() {};
  tmNLCO* GetNLCO() { return mNLCO; };
  void Revert();
  virtual void Optimize();
  virtual void DataToTree() = 0;
  virtual void TreeToData() = 0;
protected:
  bool mInitialized;                    // true if we've been fully initialized
  tmNLCO* mNLCO;                        // object that performs NLCO
  std::vector<double> mCurrentStateVec;    // current state vector
  std::stringstream mInitialState;      // initial tree state (used for reversion)
};


#endif // _TMOPTIMIZER_H_

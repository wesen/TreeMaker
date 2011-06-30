/*******************************************************************************
File:         tmScaleOptimizer.h
Project:      TreeMaker 5.x
Purpose:      Header file for the tmScaleOptimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMSCALEOPTIMIZER_H_
#define _TMSCALEOPTIMIZER_H_

// TreeMaker classes
#include "tmTreeClasses.h"
#include "tmOptimizer.h"
#include "tmNLCO.h"

/**********
class tmScaleOptimizer
An optimizer for the overall scale of a tree.
**********/
class tmScaleOptimizer : public tmOptimizer
{
public:
  class EX_BAD_SCALE {};

  tmScaleOptimizer(tmTree* aTree, tmNLCO* aNLCO);
  void Initialize();
  std::size_t GetBaseOffset(tmNode* aNode);

  void DataToTree();
  void TreeToData();
private:
  std::size_t mNumVars;
  tmArray<tmNode*> mLeafNodes;
  friend class tmScaleOptimizerObjective;
};


/**********
class tmScaleOptimizerObjective
The objective function for the tmScaleOptimizer class.
**********/
class tmScaleOptimizerObjective : public tmDifferentiableFn
{
public:
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  tmScaleOptimizer* mScaleOptimizer;
  tmScaleOptimizerObjective(tmScaleOptimizer* aScaleOptimizer);
  tmScaleOptimizerObjective();
  tmScaleOptimizerObjective(const tmScaleOptimizerObjective&);
  friend class tmScaleOptimizer;
};


#endif // _TMSCALEOPTIMIZER_H_

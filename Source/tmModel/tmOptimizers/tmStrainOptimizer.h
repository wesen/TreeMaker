/*******************************************************************************
File:         tmStrainOptimizer.h
Project:      TreeMaker 5.x
Purpose:      Header file for the tmStrainOptimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMSTRAINOPTIMIZER_H_
#define _TMSTRAINOPTIMIZER_H_

// TreeMaker classes
#include "tmTreeClasses.h"
#include "tmOptimizer.h"
#include "tmNLCO.h"

/**********
class tmStrainOptimizer
An optimizer that minimizes the overall strain in a tree.
**********/
class tmStrainOptimizer : public tmOptimizer
{
public:
  class EX_NO_MOVING_NODES_OR_EDGES {};
  tmStrainOptimizer(tmTree* aTree, tmNLCO* aNLCO);
  void Initialize(tmDpptrArray<tmNode>& movingNodes, 
    tmDpptrArray<tmEdge>& stretchyEdges);
  bool IsFeasible() const;
  std::size_t GetBaseOffset(tmNode* aNode);
  std::size_t GetBaseOffset(tmEdge* aEdge);   
  void GetFixVarLengths(tmPath* aPath, double& lfix, std::size_t& ni,
    std::vector<std::size_t>& vi, std::vector<double>& vf);

  void DataToTree();
  void TreeToData();
private:
  tmArray<tmNode*> mMovingNodes;      // list of moving nodes
  tmArray<tmEdge*> mStretchyEdges;    // list of stretchy edges
  std::size_t edgeOffset;             // base index for edge strains
  std::size_t mNumVars;               // total number of variables
  std::vector<double> mStiffness;     // vector of stiffness coefficients

  friend class tmStrainOptimizerObjective;
};


/*****
class tmStrainOptimizerObjective
The objective function for the tmStrainOptimizer class.
*****/
class tmStrainOptimizerObjective : public tmDifferentiableFn
{
public:
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  tmStrainOptimizerObjective(tmStrainOptimizer* aStrainOptimizer);
  tmStrainOptimizer* mStrainOptimizer;
  tmStrainOptimizerObjective();
  tmStrainOptimizerObjective(const tmStrainOptimizerObjective&);
  friend class tmStrainOptimizer;
};

#endif // _TMSTRAINOPTIMIZER_H_

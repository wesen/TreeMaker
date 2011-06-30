/*******************************************************************************
File:         tmEdgeOptimizer.h
Project:      TreeMaker 5.x
Purpose:      Header file for the tmEdgeOptimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMEDGEOPTIMIZER_H_
#define _TMEDGEOPTIMIZER_H_

// TreeMaker classes
#include "tmTreeClasses.h"
#include "tmOptimizer.h"
#include "tmNLCO.h"

/**********
class tmEdgeOptimizer
An optimizer that calculates the maximal edge strain for a subset of edges.
**********/
class tmEdgeOptimizer : public tmOptimizer
{
public:
  class EX_NO_MOVING_NODES {};
  class EX_NO_MOVING_EDGES {};
  tmEdgeOptimizer(tmTree* aTree, tmNLCO* aNLCO);
  void Initialize(tmDpptrArray<tmNode>& movingNodes, 
    tmDpptrArray<tmEdge>& stretchyEdges);
  std::size_t GetBaseOffset(tmNode* aNode);
  std::size_t GetBaseOffset(tmEdge* aEdge);
  void GetFixVarLengths(tmPath* aPath, double& aFixLen, double& aVarLen);

  void DataToTree();
  void TreeToData();
private:
  std::size_t mNumVars;               // number of variables
  tmArray<tmNode*> mMovingNodes;      // list of moving nodes
  tmArray<tmEdge*> mStretchyEdges;    // list of stretchy edges
  
  friend class tmEdgeOptimizerObjective;
};



/**********
class tmEdgeOptimizerObjective
The objective function for the tmEdgeOptimizer class.
**********/
class tmEdgeOptimizerObjective : public tmDifferentiableFn
{
public:
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  tmEdgeOptimizer* mEdgeOptimizer;
  tmEdgeOptimizerObjective(tmEdgeOptimizer* aEdgeOptimizer);
  tmEdgeOptimizerObjective();
  tmEdgeOptimizerObjective(const tmEdgeOptimizerObjective&);
  friend class tmEdgeOptimizer;
};


#endif // _TMEDGEOPTIMIZER_H_

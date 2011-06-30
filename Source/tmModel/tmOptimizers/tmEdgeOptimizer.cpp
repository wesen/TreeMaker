/*******************************************************************************
File:         tmEdgeOptimizer.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for the tmEdgeOptimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-20
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmEdgeOptimizer.h"
#include "tmModel.h"

using namespace std;

/**********
class tmEdgeOptimizer
**********/

/*****
Constructor
*****/
tmEdgeOptimizer::tmEdgeOptimizer(tmTree* aTree, tmNLCO* aNLCO)
  : tmOptimizer(aTree, aNLCO)
{  
}


/*****
Initialize the calculation.
*****/
void tmEdgeOptimizer::Initialize(tmDpptrArray<tmNode>& movingNodes, 
    tmDpptrArray<tmEdge>& stretchyEdges)
{
  tmTree* theTree = GetTree(); // to have on hand
  
  // Filter the list to retain only unpinned nodes and unpinned edges
  theTree->FilterMovableParts(movingNodes, stretchyEdges);  
  
  // If there are no moving nodes or edges left, throw an exception.
  if (movingNodes.empty()) throw EX_NO_MOVING_NODES();
  if (stretchyEdges.empty()) throw EX_NO_MOVING_EDGES();
  
  // Copy list into our member variables so they'll be accessible to other
  // member fns.
  mMovingNodes = movingNodes;
  mStretchyEdges = stretchyEdges;

  // Set up our state vector
  size_t n = mMovingNodes.size();
  mNumVars = 1 + 2 * n;
  mNLCO->SetSize(mNumVars);
  mCurrentStateVec.resize(mNumVars);
  TreeToData();
  
  // Set the bounds on the optimization
  std::vector<double> bl(mNumVars, 0);      // lower bounds = 0 for all nodes
  std::vector<double> bu(mNumVars);        // upper bounds depend on paper size
  
  bl[0] = -0.999;            // min strain
  bu[0] = 10.0;            // max strain

  double w = theTree->GetPaperWidth();      // set other bounds from width and height
  double h = theTree->GetPaperHeight();

  for (size_t i = 0; i < n; ++i) {
    bu[2 * i + 1] = w;
    bu[2 * i + 2] = h;
  }
  mNLCO->SetBounds(bl, bu);
    
  // Create our objective function
  mNLCO->SetObjective(new tmEdgeOptimizerObjective(this));

  // Go through the leaf paths and add a constraint for each path that includes
  // one or more moving nodes or stretchy edges.
  {
    tmArrayIterator<tmPath*> iOwnedPaths(theTree->GetOwnedPaths());
    tmPath* aPath;
    while (iOwnedPaths.Next(&aPath)) {
      if (aPath->IsLeafPath()) {
        
        // If there's a path condition joining these nodes, then the
        // standard path condition is unnecessary. So we won't apply it.
        if (theTree->IsConditioned<tmConditionPathActive>(aPath)) 
          continue;
        
        // Get nodes at each end of the path and their indices
        tmNode* node1 = aPath->GetNodes().front();
        size_t ix = GetBaseOffset(node1);
        size_t iy = ix + 1;  
        bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);      
        
        tmNode* node2 = aPath->GetNodes().back();
        size_t jx = GetBaseOffset(node2);
        size_t jy = jx + 1;
        bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
        
        // Note that GetBaseOffset returned 0 if node1 or node2 aren't in
        // mMovingNodes; Thus, we can use the index as a check on whether it's
        // moving.
        
        // Get the fixed and variable lengths for this path and multiply them
        // by the scale of the tree. Note that we need constraints even if no
        // nodes are moving; the edge length might be fixed by fixed nodes.
        double lfix, lvar;
        GetFixVarLengths(aPath, lfix, lvar);
      
        if (iMovable && jMovable)   // both nodes moving
          mNLCO->AddNonlinearInequality(new StrainPathFn1(ix, iy, jx, jy, lfix, lvar));
        
        else if (iMovable)       // only tmNode 1 moving
          mNLCO->AddNonlinearInequality(new StrainPathFn2(ix, iy,
            node2->GetLocX(), node2->GetLocY(), lfix, lvar));
        
        else if (jMovable)       // only tmNode 2 moving
          mNLCO->AddNonlinearInequality(new StrainPathFn2(jx, jy,
            node1->GetLocX(), node1->GetLocY(), lfix, lvar));
        
        else {            // neither tmNode moving
        
          // make sure the path contains a stretchy edge (there's a nonzero
          // variable part); if it doesn't, there's no point in adding a
          // constraint.
          if (lvar == 0) continue;
          mNLCO->AddNonlinearInequality(new StrainPathFn3(node1->GetLocX(), node1->GetLocY(),
            node2->GetLocX(), node2->GetLocY(), lfix, lvar));
        }
      }
    }
  }
  
  // Go through all Conditions and add constraints for each.
  tmArrayIterator<tmCondition*> iConditions(theTree->GetConditions());
  tmCondition* aCondition;
  while (iConditions.Next(&aCondition)) aCondition->AddConstraints(this);
  
  // Ready to go. User should probably check whether the number of equalities
  // exceeds the number of variables.
  mInitialized = true;
}


/*****
Return the base index into the data vector for this tmNode.
Return tmArray<tmNode*>::BAD_OFFSET if it isn't a moving tmNode.
*****/

size_t tmEdgeOptimizer::GetBaseOffset(tmNode* aNode)
{
  size_t i = mMovingNodes.GetOffset(aNode);
  if (i == tmArray<tmNode*>::BAD_OFFSET) return tmArray<tmNode*>::BAD_OFFSET;
  else return 1 + 2 * i;
}


/*****
Return the base index into the data vector for this edge. Return
tmArray<tmEdge>::BAD_OFFSET if it isn't a moving edge.
*****/
size_t tmEdgeOptimizer::GetBaseOffset(tmEdge* aEdge)
{
  size_t i = mStretchyEdges.GetOffset(aEdge);
  if (i == tmArray<tmEdge>::BAD_OFFSET) return tmArray<tmEdge>::BAD_OFFSET;
  else return 0;
}


/*****
Return the fixed and variable parts of the length of this path
*****/
void tmEdgeOptimizer::GetFixVarLengths(tmPath* aPath, double& lfix, 
  double& lvar)
{
  lfix = 0;
  lvar = 0;
  tmEdge* aEdge;
  tmArrayIterator<tmEdge*> iPathEdges(aPath->GetEdges());
  while (iPathEdges.Next(&aEdge)) {
    double temp = aEdge->GetLength() * GetTree()->GetScale();
    if (mStretchyEdges.contains(aEdge)) {
      lfix += temp;
      lvar += temp;
    }
    else lfix += (1 + aEdge->GetStrain()) * temp;
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
OVERRIDE
Transcribe the current tree state into a state vector
*****/
void tmEdgeOptimizer::TreeToData()
{
  mCurrentStateVec[0] = 0;
  size_t n = 1;
  tmArrayIterator<tmNode*> iMovingNodes(mMovingNodes);
  tmNode* aNode;
  while (iMovingNodes.Next(&aNode)) {
    mCurrentStateVec[n++] = aNode->GetLocX();
    mCurrentStateVec[n++] = aNode->GetLocY();
  }
}


/*****
OVERRIDE
Transcribe the current state vector back to the tmTree.
*****/
void tmEdgeOptimizer::DataToTree()
{
  size_t n = 1;
  tmArrayIterator<tmNode*> iMovingNodes(mMovingNodes);
  tmNode* aNode;
  while (iMovingNodes.Next(&aNode)) {
    aNode->SetLocX(mCurrentStateVec[n++]);
    aNode->SetLocY(mCurrentStateVec[n++]);
  }
  
  tmArrayIterator<tmEdge*> iStretchyEdges(mStretchyEdges);
  tmEdge* aEdge;
  while (iStretchyEdges.Next(&aEdge)) aEdge->SetStrain(mCurrentStateVec[0]);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Constructor stores a pointer to the tmEdgeOptimizer so that it can call
the callback function.
*****/
tmEdgeOptimizerObjective::tmEdgeOptimizerObjective(
  tmEdgeOptimizer* aEdgeOptimizer)
  : mEdgeOptimizer(aEdgeOptimizer)
{
}


/*****
Return the value of the function; also record the current state and update UI
*****/
double tmEdgeOptimizerObjective::Func(const std::vector<double>& u)
{
  IncFuncCalls();
  mEdgeOptimizer->mCurrentStateVec = u;
  mEdgeOptimizer->GetNLCO()->ObjectiveUpdateUI();
  return -u[0];
}


/*****
Return the value of the gradient of the function
*****/
void tmEdgeOptimizerObjective::Grad(const std::vector<double>&, 
  std::vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[0] = -1;
}

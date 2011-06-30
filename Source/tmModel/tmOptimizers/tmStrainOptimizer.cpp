/*******************************************************************************
File:         tmStrainOptimizer.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for the tmStrainOptimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmStrainOptimizer.h"
#include "tmConstraintFns.h"
#include "tmNLCO.h"

using namespace std;

/*****
Constructor
*****/
tmStrainOptimizer::tmStrainOptimizer(tmTree* aTree, tmNLCO* aNLCO) 
  : tmOptimizer(aTree, aNLCO)
{  
}


/*****
Initialize the calculation.
*****/
void tmStrainOptimizer::Initialize(tmDpptrArray<tmNode>& movingNodes, 
  tmDpptrArray<tmEdge>& stretchyEdges)
{
  tmTree* theTree = GetTree(); // to have on hand
  
  // Include only leaf nodes
  theTree->FilterLeafNodes(movingNodes);

  // If there are no variable nodes or edges, then throw an exception.
  if (movingNodes.empty() && stretchyEdges.empty()) {
    throw EX_NO_MOVING_NODES_OR_EDGES();
  }
  
  // Copy arrays into member variables so they've be visible to all routines
  mMovingNodes = movingNodes;
  mStretchyEdges = stretchyEdges;
  
  // Set up our state vector
  size_t n = mMovingNodes.size();    // number of moving nodes
  edgeOffset = 2 * n;          // base index for moving edges (class variable)
  size_t ne = mStretchyEdges.size();  // number of stretchy edges
  mNumVars = edgeOffset + ne;      // total number of variables (class variable)
  mNLCO->SetSize(mNumVars);
  mCurrentStateVec.resize(mNumVars);
  TreeToData();
  
  // Set up vector of stiffness coefficients, which should be positive.
  mStiffness.resize(ne);
  for (size_t i = 0; i < ne; ++i) {
    mStiffness[i] = mStretchyEdges[i]->GetStiffness();
    if (mStiffness[i] <= 0.0) {
      TMFAIL("Attempt to use non-positive stiffness");
      mStiffness[i] = 1.0;
    }
  }
  
  // Set the bounds on the optimization
  std::vector<double> bl(mNumVars, 0);      // lower bounds = 0 for all nodes
  std::vector<double> bu(mNumVars);        // upper bounds
  
  double w = theTree->GetPaperWidth();    // set upper bounds from width and height
  double h = theTree->GetPaperHeight();

  for (size_t i = 0; i < n; ++i) {
    bu[2 * i] = w;
    bu[2 * i + 1] = h;
  }
  for (size_t i = edgeOffset; i < mNumVars; ++i) {
    bl[i] = -0.999;          // minimum edge strain
    bu[i] = 2.0;          // maximum edge strain
  }
  mNLCO->SetBounds(bl, bu);
    
  // Create our objective function
  mNLCO->SetObjective(new tmStrainOptimizerObjective(this));

  // Go through the leaf paths and add a constraint for each path that includes
  // one or more moving nodes or stretchy edges.
  tmArrayIterator<tmPath*> iOwnedPaths(theTree->GetOwnedPaths());
  tmPath* aPath;
  while (iOwnedPaths.Next(&aPath)) {
    if (aPath->IsLeafPath()) {
      
      // If there's a path condition joining these nodes, then the
      // standard path condition is unnecessary. So we won't apply it.      
      if (theTree->IsConditioned<tmConditionPathActive>(aPath)) 
        continue;
      
      // Get nodes at each end of the path, their indices, and whether they're 
      // movable.      
      tmNode* node1 = aPath->GetNodes().front();
      size_t ix = GetBaseOffset(node1);
      size_t iy = ix + 1;
      bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
      
      tmNode* node2 = aPath->GetNodes().back();
      size_t jx = GetBaseOffset(node2);
      size_t jy = jx + 1;
      bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
      
      // Get the fixed and variable lengths for this path.      
      double lfix;
      std::vector<size_t> vi;
      std::vector<double> vf;
      size_t ni;
      GetFixVarLengths(aPath, lfix, ni, vi, vf);
      
      if (iMovable && jMovable)   // both nodes moving
        mNLCO->AddNonlinearInequality(
          new MultiStrainPathFn1(ix, iy, jx, jy, lfix, ni, vi, vf));
      
      else if (iMovable)       // only tmNode 1 moving
        mNLCO->AddNonlinearInequality(
          new MultiStrainPathFn2(ix, iy, node2->GetLocX(), node2->GetLocY(),
          lfix, ni, vi, vf));
      
      else if (jMovable)       // only tmNode 2 moving
        mNLCO->AddNonlinearInequality(
          new MultiStrainPathFn2(jx, jy, node1->GetLocX(), node1->GetLocY(),
          lfix, ni, vi, vf));
      
      else {            // neither tmNode moving
      
        // make sure the path contains a stretchy edge (there's a nonzero
        // variable part); if it doesn't, there's no point in adding a
        // constraint.        
        if (ni == 0) continue;
        mNLCO->AddNonlinearInequality(
          new MultiStrainPathFn3(node1->GetLocX(), node1->GetLocY(),
          node2->GetLocX(), node2->GetLocY(), lfix, ni, vi, vf));
      }
    }
  }
  
  // Go through all Conditions and add Constraints for each.
  tmArrayIterator<tmCondition*> iConditions(theTree->GetConditions());
  tmCondition* aCondition;
  while (iConditions.Next(&aCondition)) aCondition->AddConstraints(this);
  
  // Ready to go. User might want to compare number of equality constraints
  // against the number of variables before proceeding.  
  mInitialized = true;
}


/*****
Return the base index into the data vector for this tmNode. 
Return BAD_OFFSET if it isn't a moving tmNode.
*****/
size_t tmStrainOptimizer::GetBaseOffset(tmNode* aNode)
{
  size_t i = mMovingNodes.GetOffset(aNode);
  if (i == tmArray<tmNode*>::BAD_OFFSET) return tmArray<tmNode*>::BAD_OFFSET;
  else return 2 * i;
}


/*****
Return the base index into the data vector for this edge. 
Return BAD_OFFSET if it isn't a moving edge.
*****/

size_t tmStrainOptimizer::GetBaseOffset(tmEdge* aEdge)
{
  size_t i = mStretchyEdges.GetOffset(aEdge);
  if (i == tmArray<tmEdge>::BAD_OFFSET) return tmArray<tmEdge>::BAD_OFFSET;
  else return edgeOffset + i;
}


/*****
Return the fixed and variable parts of the length of this path
*****/
void tmStrainOptimizer::GetFixVarLengths(tmPath* aPath, double& lfix, 
  size_t& ni,  vector<size_t>& vi, vector<double>& vf)
{
  lfix = 0;
  ni = 0;
  vi.resize(mStretchyEdges.size());  // initially size to hold all edges
  vf.resize(mStretchyEdges.size());
  
  tmEdge* aEdge;
  tmArrayIterator<tmEdge*> iPathEdges(aPath->GetEdges());
  while (iPathEdges.Next(&aEdge)) {
    size_t ii = GetBaseOffset(aEdge);
    if (ii != tmArray<tmEdge*>::BAD_OFFSET) {
      vi[ni] = ii;
      vf[ni] = aEdge->GetLength() * GetTree()->GetScale();
      ni++;
      lfix += aEdge->GetLength() * GetTree()->GetScale();
    }
    else lfix += aEdge->GetStrainedScaledLength();
  }
  vi.resize(ni);  // then shrink to fit
  vf.resize(ni);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
OVERRIDE
Transcribe the current tree state into the state vector mCurrentStateVec.
*****/
void tmStrainOptimizer::TreeToData()
{
  size_t n = 0;
  tmArrayIterator<tmNode*> iMovingNodes(mMovingNodes);
  tmNode* aNode;
  while (iMovingNodes.Next(&aNode)) {
    mCurrentStateVec[n++] = aNode->GetLocX();
    mCurrentStateVec[n++] = aNode->GetLocY();
  }
  tmArrayIterator<tmEdge*> iStretchyEdges(mStretchyEdges);
  tmEdge* aEdge;
  while (iStretchyEdges.Next(&aEdge)) mCurrentStateVec[n++] = aEdge->GetStrain();
}


/*****
OVERRIDE
Transcribe the current state vector mCurrentStateVec back into the tree.
*****/
void tmStrainOptimizer::DataToTree()
{
  size_t n = 0;
  tmArrayIterator<tmNode*> iMovingNodes(mMovingNodes);
  tmNode* aNode;
  while (iMovingNodes.Next(&aNode)) {
    aNode->SetLocX(mCurrentStateVec[n++]);
    aNode->SetLocY(mCurrentStateVec[n++]);
  }
  
  tmArrayIterator<tmEdge*> iStretchyEdges(mStretchyEdges);
  tmEdge* aEdge;
  while (iStretchyEdges.Next(&aEdge)) aEdge->SetStrain(mCurrentStateVec[n++]);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/**********
class tmStrainOptimizerObjective
**********/

/*****
Constructor
*****/
tmStrainOptimizerObjective::tmStrainOptimizerObjective(
  tmStrainOptimizer* aStrainOptimizer)
  : mStrainOptimizer(aStrainOptimizer)
{
}


/*****
Return the value of the function
*****/
double tmStrainOptimizerObjective::Func(const std::vector<double>& u)
{
  IncFuncCalls();
  mStrainOptimizer->mCurrentStateVec = u;
  mStrainOptimizer->GetNLCO()->ObjectiveUpdateUI();
  
  // Return mean square edge strain, weighted by stiffness
  double ut = 0;
  size_t nn = mStrainOptimizer->edgeOffset;
  size_t nt = mStrainOptimizer->mNumVars;
  for (size_t i = nn; i < nt; ++i) ut += 
    (mStrainOptimizer->mStiffness[i - nn]) * pow(u[i], 2);
  return ut;
}


/*****
Return the value of the gradient of the function
*****/
void tmStrainOptimizerObjective::Grad(const std::vector<double>& u, 
  std::vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0);
  size_t nn = mStrainOptimizer->edgeOffset;
  size_t nt = mStrainOptimizer->mNumVars;
  for (size_t i = nn; i < nt; ++i) du[i] = 2 * 
    (mStrainOptimizer->mStiffness[i - nn]) * u[i];
}

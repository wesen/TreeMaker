/*******************************************************************************
File:         tmScaleOptimizer.h
Project:      TreeMaker 5.x
Purpose:      Implementation file for the tmScaleOptimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-22
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmScaleOptimizer.h"
#include "tmModel.h"

using namespace std;

/**********
class tmScaleOptimizer
An optimizer for the overall scale of a tree.
**********/

/*****
Constructor
*****/
tmScaleOptimizer::tmScaleOptimizer(tmTree* aTree, tmNLCO* aNLCO)
  : tmOptimizer(aTree, aNLCO)
{
}


/*****
Initialize the calculation
*****/
void tmScaleOptimizer::Initialize()
{
  tmTree* theTree = GetTree(); // to have on hand
  
  // Make a list of all leaf nodes.
  theTree->GetLeafNodes(mLeafNodes);
  
  // Set up our state vector
  size_t n = mLeafNodes.size();
  mNumVars = 1 + 2 * n;
  mNLCO->SetSize(mNumVars);
  mCurrentStateVec.resize(mNumVars);
  TreeToData();
  
  // Set the bounds in the optimizer: one bound for each variable (incl. scale
  // & coords)
  std::vector<double> bl(mNumVars, 0);      // lower bounds = 0 for all variables
  std::vector<double> bu(mNumVars);        // upper bounds depend on paper size
  
  bu[0] = 2.0;              // max scale factor

  double w = theTree->GetPaperWidth();  // set other bounds from width and height
  double h = theTree->GetPaperHeight();

  for (size_t i = 0; i < n; ++i) {
    bu[2 * i + 1] = w;
    bu[2 * i + 2] = h;
  }
  mNLCO->SetBounds(bl, bu);
    
  // Create our objective function
  mNLCO->SetObjective(new tmScaleOptimizerObjective(this));
  
  // Add a constraint on the scale to keep it larger than 10% of its present
  // value.
  mNLCO->AddLinearInequality(new OneVarFn(0, -1.0, 0.1 * theTree->GetScale()));

  // Add a constraint for each leaf path.
  tmArrayIterator<tmPath*> iOwnedPaths(theTree->GetOwnedPaths());
  tmPath* aPath;
  while (iOwnedPaths.Next(&aPath)) {
    if (aPath->IsLeafPath()) {
    
      // If there's a path condition joining these nodes, then the
      // standard path condition is unnecessary. So we won't apply it.
      if (theTree->IsConditioned<tmConditionPathActive>(aPath)) 
        continue;
      
      // Get indices of the nodes at the end of the paths and add an inequality
      size_t ix = GetBaseOffset(aPath->GetNodes().front());
      size_t jx = GetBaseOffset(aPath->GetNodes().back());
      mNLCO->AddNonlinearInequality(new PathFn1(ix, ix + 1, jx, jx + 1, 
        aPath->GetMinTreeLength()));
    }
  }
  
  // Go through all Conditions and add constraints for each.
  tmArrayIterator<tmCondition*> iConditions(theTree->GetConditions());
  tmCondition* aCondition;
  while (iConditions.Next(&aCondition)) aCondition->AddConstraints(this);
  
  // Ready to go. User might want to compare number of equalities against
  // number of variables.
  mInitialized = true;
}


/*****
Return the base offset into the data vector for this tmNode, i.e., the offset
to the x coordinate of this node. y coordinate offset is x offset + 1. Return
tmArray<tmNode*>::BAD_OFFSET if it's not present.
*****/
size_t tmScaleOptimizer::GetBaseOffset(tmNode* aNode)
{
  size_t i = mLeafNodes.GetOffset(aNode);
  if (i == tmArray<tmNode*>::BAD_OFFSET) return tmArray<tmNode*>::BAD_OFFSET;
  else return 1 + 2 * i;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
OVERRIDE
Transcribe the current tree state into the state vector mCurrentStateVec
*****/
void tmScaleOptimizer::TreeToData()
{
  mCurrentStateVec[0] = GetTree()->GetScale();
  size_t n = mLeafNodes.size();
  for (size_t i = 0; i < n; ++i) {
    tmNode* aNode = mLeafNodes[i];
    mCurrentStateVec[2 * i + 1] = aNode->GetLocX();
    mCurrentStateVec[2 * i + 2] = aNode->GetLocY();
  }
}


/*****
OVERRIDE
Transcribe the current state vector mCurrentStateVec back into the tree. Note that
cleanup will be delayed until this object goes out of scope.
*****/
void tmScaleOptimizer::DataToTree()
{
  GetTree()->SetScale(mCurrentStateVec[0]);
  size_t n = mLeafNodes.size();
  for (size_t i = 0; i < n; ++i) {
    tmNode* aNode = mLeafNodes[i];
    aNode->SetLocX(mCurrentStateVec[2 * i + 1]);
    aNode->SetLocY(mCurrentStateVec[2 * i + 2]);
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmScaleOptimizerObjective
The objective function for the tmScaleOptimizer class.
**********/

/*****
Constructor stores a pointer to the tmScaleOptimizer so that it can show
progress and check for cancellation.
*****/
tmScaleOptimizerObjective::tmScaleOptimizerObjective(
  tmScaleOptimizer* aScaleOptimizer)
  : mScaleOptimizer(aScaleOptimizer)
{
}


/*****
Return the value of the function; also record current state and update UI
*****/
double tmScaleOptimizerObjective::Func(const std::vector<double>& u)
{
  IncFuncCalls();
  mScaleOptimizer->mCurrentStateVec = u;
  mScaleOptimizer->GetNLCO()->ObjectiveUpdateUI();
  return -u[0];
}


/*****
Return the value of the gradient of the function
*****/
void tmScaleOptimizerObjective::Grad(const std::vector<double>&, 
  std::vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[0] = -1;
}

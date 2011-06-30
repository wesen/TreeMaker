/*******************************************************************************
File:         tmConditionNodeSymmetric.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionNodeSymmetric class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionNodeSymmetric.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionNodeSymmetric
A condition for fixing a tmNode to the symmetry line 
**********/
    
/*****
Bare Constructor
*****/    
tmConditionNodeSymmetric::tmConditionNodeSymmetric(tmTree* aTree)
  : tmCondition(aTree)
{
}

    
/*****
Constructor for templated creator function
*****/
tmConditionNodeSymmetric::tmConditionNodeSymmetric(tmTree* aTree, tmNode* aNode)
  : tmCondition(aTree)
{
  mNode = aNode;
}


/*****
Set the node that is fixed to the symmetry line. Must be a leaf node.
*****/
void tmConditionNodeSymmetric::SetNode(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  tmTreeCleaner tc(mTree);
  mNode = aNode;
}

    
/*****
Return true if this uses the referenced tmPart.
*****/
bool tmConditionNodeSymmetric::Uses(tmPart* aPart) const
{
  tmNode* aNode = dynamic_cast<tmNode*>(aPart);
  return (aNode == mNode);
}


/*****
Return true if the referenced parts still exist
*****/
bool tmConditionNodeSymmetric::IsValidCondition() const
{
  return (mNode != 0) && (mNode->IsLeafNode());
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionNodeSymmetric::CalcFeasibility()
{
  TMASSERT(mNode);
  mIsFeasibleCondition = true;
  // symmetry conditions are defined to be infeasible if the symmetry line has
  // not been turned on.
  mIsFeasibleCondition &= mTree->HasSymmetry();
  if (!mIsFeasibleCondition) return;
  vector<double> vars(2, 0.);
  vars[0] = mNode->mLoc.x;
  vars[1] = mNode->mLoc.y;
  StickToLineFn fn(0, 1, mTree->mSymLoc, mTree->mSymAngle);
  mIsFeasibleCondition &= IsTiny(fn.Func(vars));
}


/*****
Add constraints for a tmScaleOptimizer
*****/
void tmConditionNodeSymmetric::AddConstraints(tmScaleOptimizer* t)
{
  if (!mTree->mHasSymmetry) return;
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1; 
    t->GetNLCO()->AddLinearEquality(
      new StickToLineFn(ix, iy, mTree->mSymLoc, mTree->mSymAngle));
  }
}


/*****
Add constraints for a tmEdgeOptimizer
*****/    
void tmConditionNodeSymmetric::AddConstraints(tmEdgeOptimizer* t)
{
  if (!mTree->mHasSymmetry) return; 
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    t->GetNLCO()->AddLinearEquality(
      new StickToLineFn(ix, iy, mTree->mSymLoc, mTree->mSymAngle));
  }
}


/*****
Add constraints for a tmStrainOptimizer
*****/    
void tmConditionNodeSymmetric::AddConstraints(tmStrainOptimizer* t)
{
  if (!mTree->mHasSymmetry) return;
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    t->GetNLCO()->AddLinearEquality(
      new StickToLineFn(ix, iy, mTree->mSymLoc, mTree->mSymAngle));
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/  
size_t tmConditionNodeSymmetric::GetNumLinesRest()
{
  return 1;
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionNodeSymmetric::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mNode);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionNodeSymmetric::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mNode);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionNodeSymmetric, "CNsn")

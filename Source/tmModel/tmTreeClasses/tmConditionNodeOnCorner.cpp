/*******************************************************************************
File:         tmConditionNodeOnCorner.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConditionNodeOnCorner class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionNodeOnCorner.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionNodeOnCorner
A condition for fixing a node to any corner of the paper
**********/
    
/*****
Bare constructor
*****/    
tmConditionNodeOnCorner::tmConditionNodeOnCorner(tmTree* aTree)
  : tmCondition(aTree)
{
}

    
/*****
Constructor
*****/    
tmConditionNodeOnCorner::tmConditionNodeOnCorner(tmTree* aTree, tmNode* aNode)
  : tmCondition(aTree)
{
  mNode = aNode;
  mNode->mIsConditionedNode = true;
}

    
/*****
Set the node that is forced to a corner; must be a leaf node.
*****/    
void tmConditionNodeOnCorner::SetNode(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  tmTreeCleaner tc(mTree);
  mNode = aNode;
}

    
/*****
Return true if this uses the referenced tmPart.
*****/  
bool tmConditionNodeOnCorner::Uses(tmPart* aPart) const
{
  tmNode* aNode = dynamic_cast<tmNode*>(aPart);
  return (aNode == mNode);
}


/*****
Return true if the referenced parts still exist
*****/    
bool tmConditionNodeOnCorner::IsValidCondition() const
{
  return (mNode != 0) && (mNode->IsLeafNode());
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionNodeOnCorner::CalcFeasibility()
{
  TMASSERT(mNode);
  mIsFeasibleCondition = true;
  vector<double> vars(2, 0.);
  vars[0] = mNode->mLoc.x;
  vars[1] = mNode->mLoc.y;
  CornerFn fn0(0, mTree->mPaperWidth);
  mIsFeasibleCondition &= IsTiny(fn0.Func(vars));
  if (!mIsFeasibleCondition) return;
  CornerFn fn1(1, mTree->mPaperHeight);
  mIsFeasibleCondition &= IsTiny(fn1.Func(vars));
}


/*****
Add constraints for a tmScaleOptimizer
*****/    
void tmConditionNodeOnCorner::AddConstraints(tmScaleOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    t->GetNLCO()->AddNonlinearEquality(new CornerFn(ix, mTree->mPaperWidth));
    t->GetNLCO()->AddNonlinearEquality(new CornerFn(iy, mTree->mPaperHeight));
  }
}


/*****
Add constraints for a tmEdgeOptimizer
*****/  
void tmConditionNodeOnCorner::AddConstraints(tmEdgeOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    t->GetNLCO()->AddNonlinearEquality(new CornerFn(ix, mTree->mPaperWidth));
    t->GetNLCO()->AddNonlinearEquality(new CornerFn(iy, mTree->mPaperHeight));
  }
}


/*****
Add constraints for a tmStrainOptimizer
*****/    
void tmConditionNodeOnCorner::AddConstraints(tmStrainOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    t->GetNLCO()->AddNonlinearEquality(new CornerFn(ix, mTree->mPaperWidth));
    t->GetNLCO()->AddNonlinearEquality(new CornerFn(iy, mTree->mPaperHeight));
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/    
size_t tmConditionNodeOnCorner::GetNumLinesRest()
{
  return 1;
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionNodeOnCorner::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mNode);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionNodeOnCorner::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mNode);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionNodeOnCorner, "CNkn")

/*******************************************************************************
File:         tmConditionNodeOnEdge.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionNodeOnEdge class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionNodeOnEdge.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionNodeOnEdge
A condition for fixing a node to any edge of the paper
**********/
    
/*****
Bare constructor
*****/    
tmConditionNodeOnEdge::tmConditionNodeOnEdge(tmTree* aTree)
  : tmCondition(aTree)
{
}

    
/*****
Constructor
*****/    
tmConditionNodeOnEdge::tmConditionNodeOnEdge(tmTree* aTree, tmNode* aNode)
  : tmCondition(aTree)
{
  mNode = aNode;
  mNode->mIsConditionedNode = true;
}

    
/*****
Set the node that is fixed to the edge. Must be a leaf node.
*****/    
void tmConditionNodeOnEdge::SetNode(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  tmTreeCleaner tc(mTree);
  mNode = aNode;
}

    
/*****
Return true if this uses the referenced part.
*****/    
bool tmConditionNodeOnEdge::Uses(tmPart* aPart) const
{
  tmNode* aNode = dynamic_cast<tmNode*>(aPart);
  return (aNode == mNode);
}


/*****
Return true if the referenced parts still exist
*****/  
bool tmConditionNodeOnEdge::IsValidCondition() const
{
  return (mNode != 0) && (mNode->IsLeafNode());
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionNodeOnEdge::CalcFeasibility()
{
  TMASSERT(mNode);
  mIsFeasibleCondition = true;
  vector<double> vars(2, 0.);
  vars[0] = mNode->mLoc.x;
  vars[1] = mNode->mLoc.y;
  StickToEdgeFn fn(0, 1, mTree->mPaperWidth, mTree->mPaperHeight);
  mIsFeasibleCondition &= IsTiny(fn.Func(vars));
}


/*****
Add constraints for a tmScaleOptimizer
*****/  
void tmConditionNodeOnEdge::AddConstraints(tmScaleOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    t->GetNLCO()->AddNonlinearEquality(
      new StickToEdgeFn(ix, iy, mTree->mPaperWidth, mTree->mPaperHeight));
  }
}


/*****
Add constraints for a tmEdgeOptimizer
*****/  
void tmConditionNodeOnEdge::AddConstraints(tmEdgeOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    t->GetNLCO()->AddNonlinearEquality(
      new StickToEdgeFn(ix, iy, mTree->mPaperWidth, mTree->mPaperHeight));
  }
}


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionNodeOnEdge::AddConstraints(tmStrainOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    t->GetNLCO()->AddNonlinearEquality(
      new StickToEdgeFn(ix, iy, mTree->mPaperWidth, mTree->mPaperHeight));
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/  
size_t tmConditionNodeOnEdge::GetNumLinesRest()
{
  return 1;
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionNodeOnEdge::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mNode);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionNodeOnEdge::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mNode);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionNodeOnEdge, "CNen")

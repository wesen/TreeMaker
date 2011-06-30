/*******************************************************************************
File:         tmConditionNodeFixed.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionNodeFixed class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionNodeFixed.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionNodeFixed
A condition for forcing one or both coordinates of a node to a fixed value.
**********/

/*****
Constructor
*****/
tmConditionNodeFixed::tmConditionNodeFixed(tmTree* aTree, tmNode* aNode,
  bool aXFixed, const tmFloat& aXFixValue, bool aYFixed, 
  const tmFloat& aYFixValue)
  : tmCondition(aTree), mNode(aNode), 
  mXFixed(aXFixed), mXFixValue(aXFixValue),
  mYFixed(aYFixed), mYFixValue(aYFixValue)
{
  if (mNode) mNode->mIsConditionedNode = true;
}


/*****
Set whether the x-coordinate is fixed.
*****/
void tmConditionNodeFixed::SetXFixed(bool aXFixed)
{
  if (aXFixed != mXFixed) {
    tmTreeCleaner tc(mTree);
    mXFixed = aXFixed;
  }
}


/*****
Set whether the y-coordinate is fixed.
*****/
void tmConditionNodeFixed::SetYFixed(bool aYFixed)
{
  if (aYFixed != mYFixed) {
    tmTreeCleaner tc(mTree);
    mYFixed = aYFixed;
  }
}


/*****
Set the value to which the x-coordinate is fixed.
*****/
void tmConditionNodeFixed::SetXFixValue(const tmFloat& aXFixValue)
{
  mXFixValue = aXFixValue;
  if (mXFixed) {
    tmTreeCleaner tc(mTree);
  }
}


/*****
Set the value to which the y-coordinate is fixed.
*****/
void tmConditionNodeFixed::SetYFixValue(const tmFloat& aYFixValue)
{
  mYFixValue = aYFixValue;
  if (mYFixed) {
    tmTreeCleaner tc(mTree);
  }
}


/*****
Set the node that is fixed. Must be a leaf node.
*****/
void tmConditionNodeFixed::SetNode(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  tmTreeCleaner tc(mTree);
  mNode = aNode;
}


/*****
Return true if this uses the referenced tmPart.
*****/
bool tmConditionNodeFixed::Uses(tmPart* aPart) const
{
  tmNode* aNode = dynamic_cast<tmNode*>(aPart);
  return (aNode == mNode);
}


/*****
Return true if the referenced parts still exist
*****/  
bool tmConditionNodeFixed::IsValidCondition() const
{
  return (mNode != 0) && (mNode->IsLeafNode());
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionNodeFixed::CalcFeasibility()
{
  TMASSERT(mNode);
  mIsFeasibleCondition = true;
  vector<double> vars(2, 0.);
  vars[0] = mNode->mLoc.x;
  vars[1] = mNode->mLoc.y;
  if (mXFixed) {
    OneVarFn fn(0, -1., mXFixValue);
    mIsFeasibleCondition &= IsTiny(fn.Func(vars));
    if (!mIsFeasibleCondition) return;
  }
  if (mYFixed) {
    OneVarFn fn(1, -1., mYFixValue);
    mIsFeasibleCondition &= IsTiny(fn.Func(vars));
  }
}


/*****
Add constraints for a tmScaleOptimizer
*****/  
void tmConditionNodeFixed::AddConstraints(tmScaleOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    if (mXFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(ix, -1., mXFixValue));
    if (mYFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(iy, -1., mYFixValue));
  }
}


/*****
Add constraints for a tmEdgeOptimizer
*****/  
void tmConditionNodeFixed::AddConstraints(tmEdgeOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    if (mXFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(ix, -1., mXFixValue));
    if (mYFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(iy, -1., mYFixValue));
  }
}


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionNodeFixed::AddConstraints(tmStrainOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    if (mXFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(ix, -1., mXFixValue));
    if (mYFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(iy, -1., mYFixValue));
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/  
size_t tmConditionNodeFixed::GetNumLinesRest()
{
  return 5;
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionNodeFixed::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mNode);
  PutPOD(os,mXFixed);
  PutPOD(os, mYFixed);
  PutPOD(os, mXFixValue);
  PutPOD(os, mYFixValue);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionNodeFixed::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mNode);
  GetPOD(is, mXFixed);
  GetPOD(is, mYFixed);
  GetPOD(is, mXFixValue);
  GetPOD(is, mYFixValue);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionNodeFixed, "CNfn")

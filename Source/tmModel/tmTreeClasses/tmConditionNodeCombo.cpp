/*******************************************************************************
File:         tmConditionNodeCombo.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionNodeCombo class
Author:       Robert J. Lang
Modified by:  
Created:      2004-04-22
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionNodeCombo.h"
#include "tmModel.h"

using namespace std;

/* Notes.
Class tmConditionNodeCombo is new in version 5; it combines the functionality
of tmConditionNodeFixed, tmConditionNodeSymmetric, tmConditionNodeOnEdge, and
tmConditionNodeOnCorner. It's a bit more convenient to use because one
typically switches among these different constraints on a single node, so it's
simpler to switch settings within the condition than to remove and create new
conditions.
*/

/**********
class tmConditionNodeCombo
A condition for placing several types of constraint on one or both coordinates
of a node.
**********/

/*****
Constructor
*****/
tmConditionNodeCombo::tmConditionNodeCombo(tmTree* aTree, tmNode* aNode,
  bool aToSymmetryLine, bool aToPaperEdge, bool aToPaperCorner,
  bool aXFixed, const tmFloat& aXFixValue, bool aYFixed, 
  const tmFloat& aYFixValue)
  : tmCondition(aTree),
  mToSymmetryLine(aToSymmetryLine),
  mToPaperEdge(aToPaperEdge),
  mToPaperCorner(aToPaperCorner),
  mXFixed(aXFixed), mXFixValue(aXFixValue),
  mYFixed(aYFixed), mYFixValue(aYFixValue),
  mNode(aNode)
{
  if (mNode) mNode->mIsConditionedNode = true;
}


/*****
Set whether the node should be fixed to the symmetry line.
*****/
void tmConditionNodeCombo::SetToSymmetryLine(bool aToSymmetryLine)
{
  mToSymmetryLine = aToSymmetryLine;
}


/*****
Set whether the node should be fixed to the edge of the paper
*****/
void tmConditionNodeCombo::SetToPaperEdge(bool aToPaperEdge)
{
  mToPaperEdge = aToPaperEdge;
}


/*****
Set whether the node should be fixed to one of the four corners
*****/
void tmConditionNodeCombo::SetToPaperCorner(bool aToPaperCorner)
{
  mToPaperCorner = aToPaperCorner;
}


/*****
Set whether the x-coordinate is fixed.
*****/
void tmConditionNodeCombo::SetXFixed(bool aXFixed)
{
  if (aXFixed != mXFixed) {
    tmTreeCleaner tc(mTree);
    mXFixed = aXFixed;
  }
}


/*****
Set the value to which the x-coordinate is fixed.
*****/
void tmConditionNodeCombo::SetXFixValue(const tmFloat& aXFixValue)
{
  mXFixValue = aXFixValue;
  if (mXFixed) {
    tmTreeCleaner tc(mTree);
  }
}


/*****
Set whether the y-coordinate is fixed.
*****/
void tmConditionNodeCombo::SetYFixed(bool aYFixed)
{
  if (aYFixed != mYFixed) {
    tmTreeCleaner tc(mTree);
    mYFixed = aYFixed;
  }
}


/*****
Set the value to which the y-coordinate is fixed.
*****/
void tmConditionNodeCombo::SetYFixValue(const tmFloat& aYFixValue)
{
  mYFixValue = aYFixValue;
  if (mYFixed) {
    tmTreeCleaner tc(mTree);
  }
}


/*****
Set the node that is fixed. Must be a leaf node.
*****/
void tmConditionNodeCombo::SetNode(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  if (aNode != mNode) {
    tmTreeCleaner tc(mTree);
    mNode = aNode;
  }
}


/*****
Return true if this uses the referenced tmPart.
*****/
bool tmConditionNodeCombo::Uses(tmPart* aPart) const
{
  tmNode* aNode = dynamic_cast<tmNode*>(aPart);
  return (aNode == mNode);
}


/*****
Return true if the referenced parts still exist
*****/  
bool tmConditionNodeCombo::IsValidCondition() const
{
  return (mNode != 0) && (mNode->IsLeafNode());
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionNodeCombo::CalcFeasibility()
{
  TMASSERT(mNode);
  mIsFeasibleCondition = true;
  vector<double> vars(2, 0.);
  vars[0] = mNode->mLoc.x;
  vars[1] = mNode->mLoc.y;

  if (mTree->HasSymmetry() && mToSymmetryLine) {
    StickToLineFn fn(0, 1, mTree->mSymLoc, mTree->mSymAngle);
    mIsFeasibleCondition &= IsTiny(fn.Func(vars));
    if (!mIsFeasibleCondition) return;
  }
  if (mToPaperEdge) {
    StickToEdgeFn fn(0, 1, mTree->mPaperWidth, mTree->mPaperHeight);
    mIsFeasibleCondition &= IsTiny(fn.Func(vars));
    if (!mIsFeasibleCondition) return;
  }
  if (mToPaperCorner) {
    CornerFn fn0(0, mTree->mPaperWidth);
    mIsFeasibleCondition &= IsTiny(fn0.Func(vars));
    if (!mIsFeasibleCondition) return;
    CornerFn fn1(1, mTree->mPaperWidth);
    mIsFeasibleCondition &= IsTiny(fn1.Func(vars));
    if (!mIsFeasibleCondition) return;
  }
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
void tmConditionNodeCombo::AddConstraints(tmScaleOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    if (mTree->HasSymmetry() && mToSymmetryLine) 
      t->GetNLCO()->AddLinearEquality(
        new StickToLineFn(ix, iy, mTree->mSymLoc, mTree->mSymAngle));
    if (mToPaperEdge) t->GetNLCO()->AddNonlinearEquality(
      new StickToEdgeFn(ix, iy, mTree->mPaperWidth, mTree->mPaperHeight));
    if (mToPaperCorner) {
      t->GetNLCO()->AddNonlinearEquality(
        new CornerFn(ix, mTree->mPaperWidth));
      t->GetNLCO()->AddNonlinearEquality(
        new CornerFn(iy, mTree->mPaperHeight));
    }
    if (mXFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(ix, -1., mXFixValue));
    if (mYFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(iy, -1., mYFixValue));
  }
}


/*****
Add constraints for a tmEdgeOptimizer
*****/  
void tmConditionNodeCombo::AddConstraints(tmEdgeOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    if (mTree->HasSymmetry() && mToSymmetryLine) 
      t->GetNLCO()->AddLinearEquality(
        new StickToLineFn(ix, iy, mTree->mSymLoc, mTree->mSymAngle));
    if (mToPaperEdge) t->GetNLCO()->AddNonlinearEquality(
      new StickToEdgeFn(ix, iy, mTree->mPaperWidth, mTree->mPaperHeight));
    if (mToPaperCorner) {
      t->GetNLCO()->AddNonlinearEquality(
        new CornerFn(ix, mTree->mPaperWidth));
      t->GetNLCO()->AddNonlinearEquality(
        new CornerFn(iy, mTree->mPaperHeight));
    }
    if (mXFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(ix, -1., mXFixValue));
    if (mYFixed) t->GetNLCO()->AddLinearEquality(
      new OneVarFn(iy, -1., mYFixValue));
  }
}


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionNodeCombo::AddConstraints(tmStrainOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode);
  if (ix != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    if (mTree->HasSymmetry() && mToSymmetryLine) 
      t->GetNLCO()->AddLinearEquality(
        new StickToLineFn(ix, iy, mTree->mSymLoc, mTree->mSymAngle));
    if (mToPaperEdge) t->GetNLCO()->AddNonlinearEquality(
      new StickToEdgeFn(ix, iy, mTree->mPaperWidth, mTree->mPaperHeight));
    if (mToPaperCorner) {
      t->GetNLCO()->AddNonlinearEquality(
        new CornerFn(ix, mTree->mPaperWidth));
      t->GetNLCO()->AddNonlinearEquality(
        new CornerFn(iy, mTree->mPaperHeight));
    }
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
size_t tmConditionNodeCombo::GetNumLinesRest()
{
  return 8;
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionNodeCombo::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mNode);
  PutPOD(os, mToSymmetryLine);
  PutPOD(os, mToPaperEdge);
  PutPOD(os, mToPaperCorner);
  PutPOD(os, mXFixed);
  PutPOD(os, mXFixValue);
  PutPOD(os, mYFixed);
  PutPOD(os, mYFixValue);
}


/*****
Get this tmCondition from the stream Note that version 4 didn't create this
kind of condition; we'll only encounter it in a version 5 document.
*****/  
void tmConditionNodeCombo::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mNode);
  GetPOD(is, mToSymmetryLine);
  GetPOD(is, mToPaperEdge);
  GetPOD(is, mToPaperCorner);
  GetPOD(is, mXFixed);
  GetPOD(is, mXFixValue);
  GetPOD(is, mYFixed);
  GetPOD(is, mYFixValue);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionNodeCombo, "CNxn")

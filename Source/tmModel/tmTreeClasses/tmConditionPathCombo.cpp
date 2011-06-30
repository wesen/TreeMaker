/*******************************************************************************
File:         tmConditionPathCombo.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionPathCombo class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-05
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionPathCombo.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionPathCombo
A condition for making a path active 
**********/
    
/*****
Member initialization
*****/
void tmConditionPathCombo::InitConditionPathCombo()
{
  mPath = 0;
  mNode1 = 0;
  mNode2 = 0;
  mIsAngleFixed = false;
  mAngle = 0.0;
  mIsAngleQuant = false;
  // Angle quantization is n * PI / mQuant + mQuantOffset
  mQuant = 8;
  mQuantOffset = 0.0;
}


/*****
Constructor
*****/    
tmConditionPathCombo::tmConditionPathCombo(tmTree* aTree)
  : tmCondition(aTree)
{
  InitConditionPathCombo();
}

    
/*****
Constructor from Path
*****/  
tmConditionPathCombo::tmConditionPathCombo(tmTree* aTree, tmPath* aPath)
  : tmCondition(aTree)
{
  InitConditionPathCombo();
  mPath = aPath;
  mNode1 = aPath->mNodes.front();
  mNode2 = aPath->mNodes.back();
}


/*****
Set the path this condition is attached to. It must be a leaf path.
*****/  
void tmConditionPathCombo::SetPath(tmPath* aPath)
{
  TMASSERT(aPath);
  TMASSERT(aPath->mIsLeafPath);
  if (mPath = aPath) return;
  tmTreeCleaner tc(mTree);
  mPath = aPath;
  mNode1 = aPath->mNodes.front();
  mNode2 = aPath->mNodes.back();
}


/*****
Set both nodes of a tmConditionPathCombo. They must both be leaf nodes.
*****/
void tmConditionPathCombo::SetNodes(tmNode* aNode1, tmNode* aNode2)
{
  TMASSERT(aNode1);
  TMASSERT(aNode1->IsLeafNode());
  TMASSERT(aNode2);
  TMASSERT(aNode2->IsLeafNode());
  TMASSERT(aNode1 != aNode2);
  if (mNode1 == aNode1 && mNode2 == aNode2) return;
  tmTreeCleaner tc(mTree);
  mNode1 = aNode1;
  mNode2 = aNode2;
  mPath = mTree->FindLeafPath(mNode1, mNode2);
}


/*****
Set the first node of a tmConditionPathCombo. Must be a leaf node.
*****/
void tmConditionPathCombo::SetNode1(tmNode* aNode1)
{
  TMASSERT(aNode1);
  TMASSERT(aNode1->IsLeafNode());
  if (mNode1 == aNode1) return;
  tmTreeCleaner tc(mTree);
  mNode1 = aNode1;
  mPath = mTree->FindLeafPath(mNode1, mNode2);
}


/*****
Set the second node of a tmConditionPathCombo. Must be a leaf node.
*****/
void tmConditionPathCombo::SetNode2(tmNode* aNode2)
{
  TMASSERT(aNode2);
  TMASSERT(aNode2->IsLeafNode());
  if (mNode2 == aNode2) return;
  tmTreeCleaner tc(mTree);
  mNode2 = aNode2;
  mPath = mTree->FindLeafPath(mNode1, mNode2);
}


/*****
Set the flag that says this path has its angle fixed. Note that if we turn on
angle fixed, we turn off angle quant.
*****/
void tmConditionPathCombo::SetAngleFixed(bool aAngleFixed)
{
  tmTreeCleaner tc(mTree);
  mIsAngleFixed = aAngleFixed;
  if (aAngleFixed) 
    mIsAngleQuant = false;
}


/*****
Set the angle that this path is fixed to.
*****/
void tmConditionPathCombo::SetAngle(const tmFloat& aAngle)
{
  tmTreeCleaner tc(mTree);
  mAngle = aAngle;
}


/*****
Set the flag that says this path has its angle quantized. Note that if we turn
on angle quant, we turn off angle fixed.
*****/
void tmConditionPathCombo::SetAngleQuant(bool aAngleQuant)
{
  tmTreeCleaner tc(mTree);
  mIsAngleQuant = aAngleQuant;
  if (aAngleQuant)
    mIsAngleFixed = false;
}


/*****
Set the quantization value for the angle of this path
*****/
void tmConditionPathCombo::SetQuant(size_t aQuant)
{
  tmTreeCleaner tc(mTree);
  mQuant = aQuant;
}


/*****
Set the quantization offset for the angle of this path
*****/
void tmConditionPathCombo::SetQuantOffset(const tmFloat& aQuantOffset)
{
  tmTreeCleaner tc(mTree);
  mQuantOffset = aQuantOffset;
}


/*****
Return true if this uses the referenced tmPart.
*****/  
bool tmConditionPathCombo::Uses(tmPart* aPart) const
{
  tmNode* aNode = dynamic_cast<tmNode*>(aPart);
  if (aNode) return ((aNode == mNode1) || (aNode == mNode2));
  tmPath* aPath = dynamic_cast<tmPath*>(aPart);
  if (aPath) return (aPath == mPath);
  return false;
}


/*****
Return true if the tmParts referenced by this tmCondition still exist
*****/  
bool tmConditionPathCombo::IsValidCondition() const
{
  return (mNode1 != 0) && (mNode2 != 0) && (mPath != 0) && mPath->IsLeafPath();
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionPathCombo::CalcFeasibility()
{
  TMASSERT(mPath);
  TMASSERT(mNode1);
  TMASSERT(mNode2);
  mIsFeasibleCondition = mPath->IsActivePath();
  if (!mIsFeasibleCondition) return;
  if (mIsAngleFixed) {
    vector<double> vars(4, 0.);
    vars[0] = mNode1->mLoc.x;
    vars[1] = mNode1->mLoc.y;
    vars[2] = mNode2->mLoc.x;
    vars[3] = mNode2->mLoc.y;
    PathAngleFn1 fn(0, 1, 2, 3, mAngle);
    mIsFeasibleCondition &= IsTiny(fn.Func(vars));
    if (!mIsFeasibleCondition) return;
  }
  if (mIsAngleQuant) {
    vector<double> vars(4, 0.);
    vars[0] = mNode1->mLoc.x;
    vars[1] = mNode1->mLoc.y;
    vars[2] = mNode2->mLoc.x;
    vars[3] = mNode2->mLoc.y;
    QuantizeAngleFn1 fn(0, 1, 2, 3, mQuant, mQuantOffset);
    mIsFeasibleCondition &= IsTiny(fn.Func(vars));
  }
}


/*****
Add constraints for a tmScaleOptimizer
*****/  
void tmConditionPathCombo::AddConstraints(tmScaleOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  if (ix == tmArray<tmNode*>::BAD_OFFSET || jx == tmArray<tmNode*>::BAD_OFFSET)
    return;

  // Add active path constraints
  t->GetNLCO()->AddNonlinearEquality(
    new PathFn1(ix, iy, jx, jy, mPath->mMinTreeLength));
  
  // Add additional constraints for angle fixed or quantized (can't have both
  // at once).
  TMASSERT(!(mIsAngleFixed && mIsAngleQuant));
  if (mIsAngleFixed) {
    t->GetNLCO()->AddLinearEquality(
      new PathAngleFn1(ix, iy, jx, jy, mAngle));
  }
  if (mIsAngleQuant) {
    t->GetNLCO()->AddNonlinearEquality(
      new QuantizeAngleFn1(ix, iy, jx, jy, mQuant, mQuantOffset));
  }
}


/*****
Add constraints for a tmEdgeOptimizer
*****/    
void tmConditionPathCombo::AddConstraints(tmEdgeOptimizer* t)
{
  // Add active path constraints. If either tmNode isn't moving, GetBaseOffset
  // returns -1.
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
  
  // Compute the fixed and variable parts for this path
  double lfix, lvar;
  t->GetFixVarLengths(mPath, lfix, lvar);
      
  if (iMovable && jMovable)   // both nodes moving
    t->GetNLCO()->AddNonlinearEquality(
      new StrainPathFn1(ix, iy, jx, jy, lfix, lvar));
        
  else if (iMovable)  // only node 1 moving
    t->GetNLCO()->AddNonlinearEquality(
      new StrainPathFn2(ix, iy,
      mNode2->mLoc.x, mNode2->mLoc.y, lfix, lvar));
        
  else if (jMovable)  // only node 2 moving
    t->GetNLCO()->AddNonlinearEquality(
      new StrainPathFn2(jx, jy,
      mNode1->mLoc.x, mNode1->mLoc.y, lfix, lvar));

  else {        // neither node moving
    // make sure the path contains a stretchy edge (there's a nonzero variable
    // part); if it doesn't, don't make a constraint         
    if (lvar != 0) t->GetNLCO()->AddNonlinearEquality(
      new StrainPathFn3(mNode1->mLoc.x, 
      mNode1->mLoc.y, mNode2->mLoc.x, mNode2->mLoc.y, lfix, lvar));
  }
  
  // Add additional constraints for angle fixed or quantized (can't have both
  // at once).
  TMASSERT(!(mIsAngleFixed && mIsAngleQuant));
  if (mIsAngleFixed) {
    if (iMovable && jMovable) // both nodes moving
      t->GetNLCO()->AddNonlinearEquality(
        new PathAngleFn1(ix, iy, jx, jy, mAngle));
      
    else if (iMovable)    // only node 1 moving
      t->GetNLCO()->AddNonlinearEquality(
        new PathAngleFn2(ix, iy,
        mNode2->mLoc.x, mNode2->mLoc.y, mAngle));
        
    else if (jMovable)    // only node 2 moving
      t->GetNLCO()->AddNonlinearEquality(
        new PathAngleFn2(jx, jy,
        mNode1->mLoc.x, mNode1->mLoc.y, mAngle));
  
  }
  if (mIsAngleQuant) {
    if (iMovable && jMovable)   // both nodes moving
      t->GetNLCO()->AddNonlinearEquality(
        new QuantizeAngleFn1(ix, iy, jx, jy, mQuant, mQuantOffset));
        
    else if (iMovable)      // only node 1 moving
      t->GetNLCO()->AddNonlinearEquality(
        new QuantizeAngleFn2(ix, iy,
        mNode2->mLoc.x, mNode2->mLoc.y, mQuant, mQuantOffset));
        
    else if (jMovable)      // only node 2 moving
      t->GetNLCO()->AddNonlinearEquality(
        new QuantizeAngleFn2(jx, jy,
        mNode1->mLoc.x, mNode1->mLoc.y, mQuant, mQuantOffset));
    }
}


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionPathCombo::AddConstraints(tmStrainOptimizer* t)
{
  // Add active path constraints. If either tmNode isn't moving, GetBaseOffset
  // returns -1.
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
        
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
        
  // Get the fixed and variable lengths for this path.
  double lfix;
  vector<size_t> vi;
  vector<double> vf;
  size_t ni;
  t->GetFixVarLengths(mPath, lfix, ni, vi, vf);
        
  if (iMovable && jMovable)   // both nodes moving
    t->GetNLCO()->AddNonlinearEquality(
      new MultiStrainPathFn1(ix, iy, jx, jy, lfix, ni, vi, vf));
        
  else if (iMovable)      // only node 1 moving
    t->GetNLCO()->AddNonlinearEquality(
      new MultiStrainPathFn2(ix, iy, mNode2->mLoc.x, mNode2->mLoc.y,
      lfix, ni, vi, vf));
        
  else if (jMovable)      // only node 2 moving
    t->GetNLCO()->AddNonlinearEquality(
      new MultiStrainPathFn2(jx, jy, mNode1->mLoc.x, mNode1->mLoc.y,
      lfix, ni, vi, vf));
        
  else {            // neither node moving
        
    // make sure the path contains a stretchy edge (there's a nonzero variable
    // part); if it doesn't, there's no point in adding a constraint.
    if (ni == 0) return;
    t->GetNLCO()->AddNonlinearEquality(
      new MultiStrainPathFn3(mNode1->mLoc.x, mNode1->mLoc.y,
      mNode2->mLoc.x, mNode2->mLoc.y, lfix, ni, vi, vf));
  }

  // Add additional constraints for angle fixed or quantized (can't have both
  // at once).
  TMASSERT(!(mIsAngleFixed && mIsAngleQuant));
  if (mIsAngleFixed) {
    if (iMovable && jMovable) // both nodes moving
      t->GetNLCO()->AddNonlinearEquality(
        new PathAngleFn1(ix, iy, jx, jy, mAngle));
      
    else if (iMovable)    // only node 1 moving
      t->GetNLCO()->AddNonlinearEquality(
        new PathAngleFn2(ix, iy,
        mNode2->mLoc.x, mNode2->mLoc.y, mAngle));
        
    else if (jMovable)    // only node 2 moving
      t->GetNLCO()->AddNonlinearEquality(
        new PathAngleFn2(jx, jy,
        mNode1->mLoc.x, mNode1->mLoc.y, mAngle));
  }
  if (mIsAngleQuant) {
    if (iMovable && jMovable)   // both nodes moving
      t->GetNLCO()->AddNonlinearEquality(
        new QuantizeAngleFn1(ix, iy, jx, jy, mQuant, mQuantOffset));
        
    else if (iMovable)      // only node 1 moving
      t->GetNLCO()->AddNonlinearEquality(
        new QuantizeAngleFn2(ix, iy,
        mNode2->mLoc.x, mNode2->mLoc.y, mQuant, mQuantOffset));
        
    else if (jMovable)      // only node 2 moving
      t->GetNLCO()->AddNonlinearEquality(
        new QuantizeAngleFn2(jx, jy,
        mNode1->mLoc.x, mNode1->mLoc.y, mQuant, mQuantOffset));
    }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/
size_t tmConditionPathCombo::GetNumLinesRest()
{
  return 7;
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionPathCombo::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mNode1);
  mTree->PutPtr(os, mNode2);
  mTree->PutPOD(os, mIsAngleFixed);
  mTree->PutPOD(os, mAngle);
  mTree->PutPOD(os, mIsAngleQuant);
  mTree->PutPOD(os, mQuant);
  mTree->PutPOD(os, mQuantOffset);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionPathCombo::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mNode1);
  mTree->GetPtr(is, mNode2);
  mPath = mTree->FindLeafPath(mNode1, mNode2);
  TMASSERT(mPath);
  mTree->GetPOD(is, mIsAngleFixed);
  mTree->GetPOD(is, mAngle);
  mTree->GetPOD(is, mIsAngleQuant);
  mTree->GetPOD(is, mQuant);
  mTree->GetPOD(is, mQuantOffset);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionPathCombo, "CNxp")

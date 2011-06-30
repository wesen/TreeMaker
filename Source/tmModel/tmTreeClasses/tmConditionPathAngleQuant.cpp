/*******************************************************************************
File:         tmConditionPathAngleQuant.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionPathAngleQuant class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionPathAngleQuant.h"
#include "tmModel.h"

using namespace std;

/*
tmConditionPathAngleQuant is a class for making a path active and quantized in angle
*/

/**********
class tmConditionPathAngleQuant
A condition for making a path active with quantized angle
**********/
    
/*****
Bare constructor
*****/    
tmConditionPathAngleQuant::tmConditionPathAngleQuant(tmTree* aTree)
  : tmConditionPathActive(aTree)
{
  mQuant = 0;
  mQuantOffset = 0;
}

    
/*****
Constructor from data
*****/  
tmConditionPathAngleQuant::tmConditionPathAngleQuant(tmTree* aTree, 
  tmPath* aPath, size_t aQuant, const tmFloat& aQuantOffset)
  : tmConditionPathActive(aTree, aPath)
{
  mQuant = aQuant;
  mQuantOffset = aQuantOffset;
}


/*****
Set the angle quantization.
*****/
void tmConditionPathAngleQuant::SetQuant(size_t aQuant)
{
  tmTreeCleaner tc(mTree);
  mQuant = aQuant;
}


/*****
Set the angle quantization offset.
*****/
void tmConditionPathAngleQuant::SetQuantOffset(const tmFloat& aQuantOffset)
{
  tmTreeCleaner tc(mTree);
  mQuantOffset = aQuantOffset;
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionPathAngleQuant::CalcFeasibility()
{
  TMASSERT(mNode1);
  TMASSERT(mNode2);
  tmConditionPathActive::CalcFeasibility();
  if (!mIsFeasibleCondition) return;
  vector<double> vars(4, 0.);
  vars[0] = mNode1->mLoc.x;
  vars[1] = mNode1->mLoc.y;
  vars[2] = mNode2->mLoc.x;
  vars[3] = mNode2->mLoc.y;
  QuantizeAngleFn1 fn(0, 1, 2, 3, mQuant, mQuantOffset);
  mIsFeasibleCondition &= IsTiny(fn.Func(vars));
}


/*****
Add constraints for a tmScaleOptimizer
*****/  
void tmConditionPathAngleQuant::AddConstraints(tmScaleOptimizer* t)
{
  // Add path active constraints
  tmConditionPathActive::AddConstraints(t);
  
  // Add path quantization constraint
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  if (ix != tmArray<tmNode*>::BAD_OFFSET && jx != tmArray<tmNode*>::BAD_OFFSET)
    t->GetNLCO()->AddNonlinearEquality(
      new QuantizeAngleFn1(ix, iy, jx, jy, mQuant, mQuantOffset));
}


/*****
Add constraints for a tmEdgeOptimizer
*****/  
void tmConditionPathAngleQuant::AddConstraints(tmEdgeOptimizer* t)
{
  // Add path active constraints
  tmConditionPathActive::AddConstraints(t);
  
  // Add path quantization constraint
  // If either node isn't moving, GetBaseOffset returns 0.
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
  
  // Add path angle constraints
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


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionPathAngleQuant::AddConstraints(tmStrainOptimizer* t)
{
  // Add path active constraints
  tmConditionPathActive::AddConstraints(t);
  
  // Add path quantization constraint
  // If either node isn't moving, GetBaseOffset returns 0.
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
  
  // Add path angle constraints
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


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of lines in the file format
*****/    
size_t tmConditionPathAngleQuant::GetNumLinesRest()
{
  return 2 + tmConditionPathActive::GetNumLinesRest();
}


/*****
Put this tmCondition to the file
*****/
    
void tmConditionPathAngleQuant::PutRestv4(ostream& os)
{
  tmConditionPathActive::PutRestv4(os); // put inherited data
  PutPOD(os, mQuant);
  PutPOD(os, mQuantOffset);
}


/*****
Get this tmCondition from the file
*****/  
void tmConditionPathAngleQuant::GetRestv4(istream& is)
{
  tmConditionPathActive::GetRestv4(is);
  GetPOD(is, mQuant);
  GetPOD(is, mQuantOffset);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionPathAngleQuant, "CNqp")

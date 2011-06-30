/*******************************************************************************
File:         tmConditionPathAngleFixed.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionPathAngleFixed class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-05
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionPathAngleFixed.h"
#include "tmModel.h"

using namespace std;

/*
tmConditionPathAngleFixed forces a path to be active and at a fixed angle.
*/

/**********
class tmConditionPathAngleFixed
A condition for making a path active at a fixed angle 
**********/
    
/*****
Bare Constructor
*****/    
tmConditionPathAngleFixed::tmConditionPathAngleFixed(tmTree* aTree)
  : tmConditionPathActive(aTree)
{
  mAngle = 0;
}

    
/*****
Constructor with values
*****/  
tmConditionPathAngleFixed::tmConditionPathAngleFixed(tmTree* aTree, 
  tmPath* aPath, const tmFloat& aAngle)
  : tmConditionPathActive(aTree, aPath)
{
  mAngle = aAngle;
}


/*****
Set the angle to which this path is quantized
*****/
void tmConditionPathAngleFixed::SetAngle(const tmFloat& aAngle)
{
  tmTreeCleaner tc(mTree);
  mAngle = aAngle;
}

    
/*****
Compute whether this condition is satisfied
*****/
void tmConditionPathAngleFixed::CalcFeasibility()
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
  PathAngleFn1 fn(0, 1, 2, 3, mAngle);
  mIsFeasibleCondition &= IsTiny(fn.Func(vars));
}


/*****
Add constraints for a tmScaleOptimizer
*****/  
void tmConditionPathAngleFixed::AddConstraints(tmScaleOptimizer* t)
{
  // Add constraints from superclass
  tmConditionPathActive::AddConstraints(t);
  
  // Add path angle constraint
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  if (ix != tmArray<tmNode*>::BAD_OFFSET && jx != tmArray<tmNode*>::BAD_OFFSET)
    t->GetNLCO()->AddLinearEquality(new PathAngleFn1(ix, iy, jx, jy, mAngle));
}


/*****
Add constraints for a tmEdgeOptimizer
*****/
    
void tmConditionPathAngleFixed::AddConstraints(tmEdgeOptimizer* t)
{
  // Add constraints from superclass
  tmConditionPathActive::AddConstraints(t);
  
  // Add path angle constraint
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
  
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


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionPathAngleFixed::AddConstraints(tmStrainOptimizer* t)
{
  // Add constraints from superclass
  tmConditionPathActive::AddConstraints(t);
  
  // Add path angle constraint
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
  
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


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream format
*****/  
size_t tmConditionPathAngleFixed::GetNumLinesRest()
{
  return 1 + tmConditionPathActive::GetNumLinesRest();
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionPathAngleFixed::PutRestv4(ostream& os)
{
  tmConditionPathActive::PutRestv4(os); // put inherited data
  PutPOD(os, mAngle);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionPathAngleFixed::GetRestv4(istream& is)
{
  tmConditionPathActive::GetRestv4(is); // get inherited data
  GetPOD(is, mAngle);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionPathAngleFixed, "CNfp")

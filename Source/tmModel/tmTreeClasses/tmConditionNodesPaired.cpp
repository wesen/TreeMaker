/*******************************************************************************
File:         tmConditionNodesPaired.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionNodesPaired class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionNodesPaired.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionNodesPaired
A condition for pairing 2 nodes about the symmetry line
**********/

/*****
Bare constructor
*****/    
tmConditionNodesPaired::tmConditionNodesPaired(tmTree* aTree)
  : tmCondition(aTree)
{
}

    
/*****
Constructor
*****/    
tmConditionNodesPaired::tmConditionNodesPaired(tmTree* aTree, tmNode* aNode1, 
  tmNode* aNode2)
  : tmCondition(aTree)
{
  mNode1 = aNode1;
  mNode2 = aNode2;
}

    
/*****
Set one of the two paired nodes. Must be a leaf node.
*****/    
void tmConditionNodesPaired::SetNode1(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  tmTreeCleaner tc(mTree);
  mNode1 = aNode;
}


/*****
Set the other of the two paired nodes. Must be a leaf node.
*****/    
void tmConditionNodesPaired::SetNode2(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  tmTreeCleaner tc(mTree);
  mNode2 = aNode;
}

    
/*****
Return true if this uses the referenced tmPart.
*****/    
bool tmConditionNodesPaired::Uses(tmPart* aPart) const
{
  tmNode* aNode = dynamic_cast<tmNode*>(aPart);
  return ((aNode == mNode1) || (aNode == mNode2));
}


/*****
Return true if the referenced parts still exist
*****/    
bool tmConditionNodesPaired::IsValidCondition() const
{
  return (mNode1 != 0) && (mNode1->IsLeafNode()) && 
    (mNode2 != 0) && (mNode2->IsLeafNode());
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionNodesPaired::CalcFeasibility()
{
  TMASSERT(mNode1);
  TMASSERT(mNode2);
  mIsFeasibleCondition = true;
  // symmetry conditions are defined to be infeasible if the symmetry line has
  // not been turned on.
  mIsFeasibleCondition &= mTree->HasSymmetry();
  if (!mIsFeasibleCondition) return;
  vector<double> vars(4, 0.);
  vars[0] = mNode1->mLoc.x;
  vars[1] = mNode1->mLoc.y;
  vars[2] = mNode2->mLoc.x;
  vars[3] = mNode2->mLoc.y;
  PairFn1A fna(0, 1, 2, 3, mTree->mSymLoc, mTree->mSymAngle);
  mIsFeasibleCondition &= IsTiny(fna.Func(vars));
  if (!mIsFeasibleCondition) return;
  PairFn1B fnb(0, 1, 2, 3, mTree->mSymLoc, mTree->mSymAngle);
  mIsFeasibleCondition &= IsTiny(fnb.Func(vars));
}


/*****
Add constraints for a tmScaleOptimizer
*****/    
void tmConditionNodesPaired::AddConstraints(tmScaleOptimizer* t)
{
  if (!mTree->mHasSymmetry) return; 
  size_t ix = t->GetBaseOffset(mNode1);
  size_t jx = t->GetBaseOffset(mNode2);
  if (ix != tmArray<tmNode*>::BAD_OFFSET && 
    jx != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    size_t jy = jx + 1; 
    t->GetNLCO()->AddLinearEquality(
      new PairFn1A(ix, iy, jx, jy, mTree->mSymLoc, mTree->mSymAngle));
    t->GetNLCO()->AddLinearEquality(
      new PairFn1B(ix, iy, jx, jy, mTree->mSymLoc, mTree->mSymAngle));
  }
}


/*****
Add constraints for a tmEdgeOptimizer
*****/    
void tmConditionNodesPaired::AddConstraints(tmEdgeOptimizer* t)
{
  if (!mTree->mHasSymmetry) return;
  
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
  
  if (iMovable && jMovable) { // both nodes moving
    t->GetNLCO()->AddLinearEquality(
      new PairFn1A(ix, iy, jx, jy, mTree->mSymLoc, mTree->mSymAngle));
    t->GetNLCO()->AddLinearEquality(
      new PairFn1B(ix, iy, jx, jy, mTree->mSymLoc, mTree->mSymAngle));
  }
  else if (iMovable) {    // only node 1 moving
    t->GetNLCO()->AddLinearEquality(
      new PairFn2A(ix, iy, mNode2->mLoc.x, mNode2->mLoc.y,
      mTree->mSymLoc, mTree->mSymAngle));
    t->GetNLCO()->AddLinearEquality(
      new PairFn2B(ix, iy, mNode2->mLoc.x, mNode2->mLoc.y,
      mTree->mSymLoc, mTree->mSymAngle));
  }
  else if (jMovable) {    // only node 2 moving
    t->GetNLCO()->AddLinearEquality(
      new PairFn2A(jx, jy, mNode1->mLoc.x, mNode1->mLoc.y,
      mTree->mSymLoc, mTree->mSymAngle));
    t->GetNLCO()->AddLinearEquality(
      new PairFn2B(jx, jy, mNode1->mLoc.x, mNode1->mLoc.y,
      mTree->mSymLoc, mTree->mSymAngle));
  }
}


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionNodesPaired::AddConstraints(tmStrainOptimizer* t)
{
  if (!mTree->mHasSymmetry) return;
  
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
  
  if (iMovable && jMovable) { // both nodes moving
    t->GetNLCO()->AddLinearEquality(
      new PairFn1A(ix, iy, jx, jy, mTree->mSymLoc, mTree->mSymAngle));
    t->GetNLCO()->AddLinearEquality(
      new PairFn1B(ix, iy, jx, jy, mTree->mSymLoc, mTree->mSymAngle));
  }
  else if (iMovable) {    // only node 1 moving
    t->GetNLCO()->AddLinearEquality(
      new PairFn2A(ix, iy, mNode2->mLoc.x, mNode2->mLoc.y,
      mTree->mSymLoc, mTree->mSymAngle));
    t->GetNLCO()->AddLinearEquality(
      new PairFn2B(ix, iy, mNode2->mLoc.x, mNode2->mLoc.y,
      mTree->mSymLoc, mTree->mSymAngle));
  }
  else if (jMovable) {    // only node 2 moving
    t->GetNLCO()->AddLinearEquality(
      new PairFn2A(jx, jy, mNode1->mLoc.x, mNode1->mLoc.y,
      mTree->mSymLoc, mTree->mSymAngle));
    t->GetNLCO()->AddLinearEquality(
      new PairFn2B(jx, jy, mNode1->mLoc.x, mNode1->mLoc.y,
      mTree->mSymLoc, mTree->mSymAngle));
  }
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/  
size_t tmConditionNodesPaired::GetNumLinesRest()
{
  return 2;
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionNodesPaired::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mNode1);
  mTree->PutPtr(os, mNode2);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionNodesPaired::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mNode1);
  mTree->GetPtr(is, mNode2);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionNodesPaired, "CNpn")

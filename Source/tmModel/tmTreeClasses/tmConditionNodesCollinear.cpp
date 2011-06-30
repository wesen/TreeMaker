/*******************************************************************************
File:         tmConditionNodesCollinear.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionNodesCollinear class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionNodesCollinear.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionNodesCollinear
A condition for making three nodes collinear
**********/
    
/*****
Constructor
*****/    
tmConditionNodesCollinear::tmConditionNodesCollinear(tmTree* aTree, 
  tmNode* aNode1, tmNode* aNode2, tmNode* aNode3)
  : tmCondition(aTree),
  mNode1(aNode1), mNode2(aNode2), mNode3(aNode3)
{
  if (mNode1) {
    TMASSERT(mNode2);
    TMASSERT(mNode3);
    mNode1->mIsConditionedNode = true;
    mNode2->mIsConditionedNode = true;
    mNode3->mIsConditionedNode = true;
  }
}

    
/*****
Set the first of 3 collinear nodes. Must be a leaf node.
*****/
void tmConditionNodesCollinear::SetNode1(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  tmTreeCleaner tc(mTree);
  mNode1 = aNode;
}


/*****
Set the second of 3 collinear nodes. Must be a leaf node.
*****/
void tmConditionNodesCollinear::SetNode2(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  tmTreeCleaner tc(mTree);
  mNode2 = aNode;
}


/*****
Set the third of 3 collinear nodes. Must be a leaf node.
*****/
void tmConditionNodesCollinear::SetNode3(tmNode* aNode)
{
  TMASSERT(aNode->IsLeafNode());
  tmTreeCleaner tc(mTree);
  mNode3 = aNode;
}


/*****
Return true if this uses the referenced tmPart.
*****/    
bool tmConditionNodesCollinear::Uses(tmPart* aPart) const
{
  tmNode* aNode = dynamic_cast<tmNode*>(aPart);
  return ((aNode == mNode1) || (aNode == mNode2) || (aNode == mNode3));
}


/*****
Return true if the referenced parts still exist
*****/
bool tmConditionNodesCollinear::IsValidCondition() const
{
  return (mNode1 != 0) && (mNode1->IsLeafNode()) &&
    (mNode2 != 0) && (mNode2->IsLeafNode()) &&
    (mNode3 != 0) && (mNode3->IsLeafNode());
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionNodesCollinear::CalcFeasibility()
{
  TMASSERT(mNode1);
  TMASSERT(mNode2);
  TMASSERT(mNode3);
  mIsFeasibleCondition = true;
  vector<double> vars(6, 0.);
  vars[0] = mNode1->mLoc.x;
  vars[1] = mNode1->mLoc.y;
  vars[2] = mNode2->mLoc.x;
  vars[3] = mNode2->mLoc.y;
  vars[4] = mNode3->mLoc.x;
  vars[5] = mNode3->mLoc.y;
  CollinearFn1 fn(0, 1, 2, 3, 4, 5);
  mIsFeasibleCondition &= IsTiny(fn.Func(vars));
}


/*****
Add constraints for a tmScaleOptimizer
*****/    
void tmConditionNodesCollinear::AddConstraints(tmScaleOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode1);
  size_t jx = t->GetBaseOffset(mNode2);
  size_t kx = t->GetBaseOffset(mNode3);
  if (ix != tmArray<tmNode*>::BAD_OFFSET && 
    jx != tmArray<tmNode*>::BAD_OFFSET && 
    kx != tmArray<tmNode*>::BAD_OFFSET) {
    size_t iy = ix + 1;
    size_t jy = jx + 1;
    size_t ky = kx + 1;
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn1(ix, iy, jx, jy, kx, ky));
  }
}


/*****
Add constraints for a tmEdgeOptimizer
*****/  
void tmConditionNodesCollinear::AddConstraints(tmEdgeOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t kx = t->GetBaseOffset(mNode3);
  size_t ky = kx + 1;
  bool kMovable = (kx != tmArray<tmNode*>::BAD_OFFSET);
  
  if (iMovable && jMovable && kMovable) // all three nodes moving
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn1(ix, iy, jx, jy, kx, ky));
    
  else if (iMovable && jMovable)      // nodes 1 and 2 moving
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn2(ix, iy, jx, jy, mNode3->mLoc.x, mNode3->mLoc.y));
      
  else if (iMovable && kMovable)      // nodes 1 and 3 moving
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn2(ix, iy, kx, ky, mNode2->mLoc.x, mNode2->mLoc.y));
      
  else if (jMovable && kMovable)      // nodes 2 and 3 moving
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn2(jx, jy, kx, ky, mNode1->mLoc.x, mNode1->mLoc.y));
      
  else if (iMovable)    // node 1 only
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn3(ix, iy, 
      mNode2->mLoc.x, mNode2->mLoc.y, mNode3->mLoc.x, mNode3->mLoc.y));
      
  else if (jMovable)    // node 2 only
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn3(jx, jy, 
      mNode1->mLoc.x, mNode1->mLoc.y, mNode3->mLoc.x, mNode3->mLoc.y));
      
  else if (kMovable)    // node 3 only
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn3(kx, ky,
      mNode1->mLoc.x, mNode1->mLoc.y, mNode2->mLoc.x, mNode2->mLoc.y));     
}


/*****
Add constraints for a tmStrainOptimizer
*****/    
void tmConditionNodesCollinear::AddConstraints(tmStrainOptimizer* t)
{
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  bool iMovable = (ix != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  bool jMovable = (jx != tmArray<tmNode*>::BAD_OFFSET);
  
  size_t kx = t->GetBaseOffset(mNode3);
  size_t ky = kx + 1;
  bool kMovable = (kx != tmArray<tmNode*>::BAD_OFFSET);
  
  if (iMovable && jMovable && kMovable) // all three nodes moving
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn1(ix, iy, jx, jy, kx, ky));
    
  else if (iMovable && jMovable)      // nodes 1 and 2 moving
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn2(ix, iy, jx, jy,
      mNode3->mLoc.x, mNode3->mLoc.y));
      
  else if (iMovable && kMovable)      // nodes 1 and 3 moving
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn2(ix, iy, kx, ky,
      mNode2->mLoc.x, mNode2->mLoc.y));
      
  else if (jMovable && kMovable)      // nodes 2 and 3 moving
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn2(jx, jy, kx, ky,
      mNode1->mLoc.x, mNode1->mLoc.y));
      
  else if (iMovable)    // node 1 only
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn3(ix, iy,
      mNode2->mLoc.x, mNode2->mLoc.y, mNode3->mLoc.x, mNode3->mLoc.y));
      
  else if (jMovable)    // node 2 only
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn3(jx, jy,
      mNode1->mLoc.x, mNode1->mLoc.y, mNode3->mLoc.x, mNode3->mLoc.y));
      
  else if (kMovable)    // node 3 only
    t->GetNLCO()->AddNonlinearEquality(
      new CollinearFn3(kx, ky,
      mNode1->mLoc.x, mNode1->mLoc.y, mNode2->mLoc.x, mNode2->mLoc.y));     
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/    
size_t tmConditionNodesCollinear::GetNumLinesRest()
{
  return 3;
}


/*****
Put this tmCondition to the stream
*****/    
void tmConditionNodesCollinear::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mNode1);
  mTree->PutPtr(os, mNode2);
  mTree->PutPtr(os, mNode3);
}


/*****
Get this tmCondition from the stream
*****/    
void tmConditionNodesCollinear::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mNode1);
  mTree->GetPtr(is, mNode2);
  mTree->GetPtr(is, mNode3);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionNodesCollinear, "CNcn")

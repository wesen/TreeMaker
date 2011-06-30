/*******************************************************************************
File:         tmConditionPathActive.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionPathActive class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-05
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionPathActive.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionPathActive
A condition for making a path active 
**********/
    
/*****
Constructor
*****/    
tmConditionPathActive::tmConditionPathActive(tmTree* aTree)
  : tmCondition(aTree)
{
}

    
/*****
Constructor from Path
*****/  
tmConditionPathActive::tmConditionPathActive(tmTree* aTree, tmPath* aPath)
  : tmCondition(aTree)
{
  mPath = aPath;
  mNode1 = aPath->mNodes.front();
  mNode2 = aPath->mNodes.back();
}


/*****
Set the path this condition is attached to. It must be a leaf path.
*****/  
void tmConditionPathActive::SetPath(tmPath* aPath)
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
Set both nodes of a tmConditionPathActive. They must both be leaf nodes.
*****/
void tmConditionPathActive::SetNodes(tmNode* aNode1, tmNode* aNode2)
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
Set the first node of a tmConditionPathActive. Must be a leaf node.
*****/
void tmConditionPathActive::SetNode1(tmNode* aNode1)
{
  TMASSERT(aNode1);
  TMASSERT(aNode1->IsLeafNode());
  if (mNode1 == aNode1) return;
  tmTreeCleaner tc(mTree);
  mNode1 = aNode1;
  mPath = mTree->FindLeafPath(mNode1, mNode2);
}


/*****
Set the second node of a tmConditionPathActive. Must be a leaf node.
*****/
void tmConditionPathActive::SetNode2(tmNode* aNode2)
{
  TMASSERT(aNode2);
  TMASSERT(aNode2->IsLeafNode());
  if (mNode2 == aNode2) return;
  tmTreeCleaner tc(mTree);
  mNode2 = aNode2;
  mPath = mTree->FindLeafPath(mNode1, mNode2);
}


/*****
Return true if this uses the referenced tmPart.
*****/  
bool tmConditionPathActive::Uses(tmPart* aPart) const
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
bool tmConditionPathActive::IsValidCondition() const
{
  return ((mPath != 0) && (mNode1 != 0) && (mNode2 != 0));
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionPathActive::CalcFeasibility()
{
  TMASSERT(mPath);
  TMASSERT(mNode1);
  TMASSERT(mNode2);
  mIsFeasibleCondition = mPath->IsActivePath();
}


/*****
Add constraints for a tmScaleOptimizer
*****/  
void tmConditionPathActive::AddConstraints(tmScaleOptimizer* t)
{
  // Add active path constraints
  size_t ix = t->GetBaseOffset(mNode1);
  size_t iy = ix + 1;
  size_t jx = t->GetBaseOffset(mNode2);
  size_t jy = jx + 1;
  if (ix != tmArray<tmNode*>::BAD_OFFSET && jx != tmArray<tmNode*>::BAD_OFFSET)
    t->GetNLCO()->AddNonlinearEquality(
      new PathFn1(ix, iy, jx, jy, mPath->mMinTreeLength));
}


/*****
Add constraints for a tmEdgeOptimizer
*****/    
void tmConditionPathActive::AddConstraints(tmEdgeOptimizer* t)
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
}


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionPathActive::AddConstraints(tmStrainOptimizer* t)
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
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/
size_t tmConditionPathActive::GetNumLinesRest()
{
  return 2;
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionPathActive::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mNode1);
  mTree->PutPtr(os, mNode2);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionPathActive::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mNode1);
  mTree->GetPtr(is, mNode2);
  mPath = mTree->FindLeafPath(mNode1, mNode2);
  TMASSERT(mPath);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionPathActive, "CNap")

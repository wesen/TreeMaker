/*******************************************************************************
File:         tmConditionEdgesSameStrain.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionEdgesSameStrain class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionEdgesSameStrain.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionEdgesSameStrain
A condition for forcing two edges to have the same strain in a strain
optimization.
**********/

/*****
Bare constructor
*****/  
tmConditionEdgesSameStrain::tmConditionEdgesSameStrain(tmTree* aTree)
  : tmCondition(aTree)
{
}

    
/*****
Constructor for templated creator function
*****/  
tmConditionEdgesSameStrain::tmConditionEdgesSameStrain(tmTree* aTree, 
  tmEdge* aEdge1, tmEdge* aEdge2)
  : tmCondition(aTree)
{
  mEdge1 = aEdge1;
  mEdge2 = aEdge2;
}


/*****
Set the first edge of a pair with same strain
*****/
void tmConditionEdgesSameStrain::SetEdge1(tmEdge* aEdge)
{
  tmTreeCleaner tc(mTree);
  mEdge1 = aEdge;
}


/*****
Set the second edge of a pair with same strain
*****/
void tmConditionEdgesSameStrain::SetEdge2(tmEdge* aEdge)
{
  tmTreeCleaner tc(mTree);
  mEdge2 = aEdge;
}

    
/*****
Return true if this uses the referenced tmPart.
*****/  
bool tmConditionEdgesSameStrain::Uses(tmPart* aPart) const
{
  tmEdge* aEdge = dynamic_cast<tmEdge*>(aPart);
  return ((aEdge == mEdge1) || (aEdge == mEdge2));
}


/*****
Return true if the referenced parts still exist
*****/  
bool tmConditionEdgesSameStrain::IsValidCondition() const
{
  return ((mEdge1 != 0) && (mEdge2 != 0));
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionEdgesSameStrain::CalcFeasibility()
{
  TMASSERT(mEdge1);
  TMASSERT(mEdge2);
  mIsFeasibleCondition = true;
  vector<double> vars(2, 0.);
  vars[0] = mEdge1->GetStrain();
  vars[1] = mEdge2->GetStrain();
  TwoVarFn fn(0, 1., 1, -1., 0.);
  mIsFeasibleCondition &= IsTiny(fn.Func(vars));
}


/*****
Add constraints for a tmScaleOptimizer
*****/  
void tmConditionEdgesSameStrain::AddConstraints(tmScaleOptimizer*)
{
  // There are no constraints with a tmScaleOptimizer because no edges are
  // variable.
}


/*****
Add constraints for a tmEdgeOptimizer
*****/  
void tmConditionEdgesSameStrain::AddConstraints(tmEdgeOptimizer*)
{
  // There are no constriants with a tmEdgeOptimizer since all strains are
  // equal anyhow
}


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionEdgesSameStrain::AddConstraints(tmStrainOptimizer* t)
{
  // If both edges are movable edges, add a constraint that equalizes their
  // strain
  size_t ie1 = t->GetBaseOffset(mEdge1);
  size_t ie2 = t->GetBaseOffset(mEdge2);
  if ((ie1 != tmArray<tmEdge*>::BAD_OFFSET) && 
    (ie2 != tmArray<tmEdge*>::BAD_OFFSET)) 
    t->GetNLCO()->AddLinearEquality(new TwoVarFn(ie1, 1., ie2, -1., 0.));
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/  
size_t tmConditionEdgesSameStrain::GetNumLinesRest()
{
  return 2;
}


/*****
Put this tmCondition to the stream
*****/  
void tmConditionEdgesSameStrain::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mEdge1);
  mTree->PutPtr(os, mEdge2);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionEdgesSameStrain::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mEdge1);
  mTree->GetPtr(is, mEdge2);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionEdgesSameStrain, "CNes")

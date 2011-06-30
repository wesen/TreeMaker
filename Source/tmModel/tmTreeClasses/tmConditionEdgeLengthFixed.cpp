/*******************************************************************************
File:         tmConditionEdgeLengthFixed.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConditionEdgeLengthFixed class
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-04
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConditionEdgeLengthFixed.h"
#include "tmModel.h"

using namespace std;

/**********
class tmConditionEdgeLengthFixed
A condition for fixing the length of an edge to its unstrained value
**********/

/*****
Bare constructor
*****/  
tmConditionEdgeLengthFixed::tmConditionEdgeLengthFixed(tmTree* aTree)
  : tmCondition(aTree)
{
}

    
/*****
Constructor for templated creator function
*****/  
tmConditionEdgeLengthFixed::tmConditionEdgeLengthFixed(tmTree* aTree, 
  tmEdge* aEdge)
  : tmCondition(aTree)
{
  mEdge = aEdge;
}


/*****
Set the edge whose length is fixed to its unstrained value
*****/
void tmConditionEdgeLengthFixed::SetEdge(tmEdge* aEdge)
{
  tmTreeCleaner tc(mTree);
  mEdge = aEdge;
}

    
/*****
Return true if this uses the referenced tmPart.
*****/    
bool tmConditionEdgeLengthFixed::Uses(tmPart* aPart) const
{
  tmEdge* aEdge = dynamic_cast<tmEdge*>(aPart);
  return (aEdge == mEdge);
}


/*****
Return true if the referenced parts still exist
*****/  
bool tmConditionEdgeLengthFixed::IsValidCondition() const
{
  return (mEdge != 0);
}


/*****
Compute whether this condition is satisfied
*****/
void tmConditionEdgeLengthFixed::CalcFeasibility()
{
  TMASSERT(mEdge);
  mIsFeasibleCondition = true;
  vector<double> vars(1, 0.);
  vars[0] = mEdge->GetStrain();
  OneVarFn fn(0, 1., 0.);
  mIsFeasibleCondition &= IsTiny(fn.Func(vars));
}


/*****
Add constraints for a tmScaleOptimizer
*****/    
void tmConditionEdgeLengthFixed::AddConstraints(tmScaleOptimizer*)
{
  // There are no constraints with a tmScaleOptimizer because no edges are
  // variable.
}


/*****
Add constraints for a tmEdgeOptimizer
*****/  
void tmConditionEdgeLengthFixed::AddConstraints(tmEdgeOptimizer* t)
{
  // Note that if a movable edge has this constraint, it will effectively
  // prevent any edges from being scaled since they all have common strain. 
  if (t->GetBaseOffset(mEdge) != tmArray<tmEdge*>::BAD_OFFSET) 
    t->GetNLCO()->AddLinearEquality(new OneVarFn(0, 1., 0.));
}


/*****
Add constraints for a tmStrainOptimizer
*****/  
void tmConditionEdgeLengthFixed::AddConstraints(tmStrainOptimizer* t)
{
  // If this is a movable edge, add a constraint that fixes strain value to
  // zero. 
  size_t ie = t->GetBaseOffset(mEdge);
  if (ie != tmArray<tmEdge*>::BAD_OFFSET)
    t->GetNLCO()->AddLinearEquality(new OneVarFn(ie, 1., 0.));
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Return the number of tokens in the stream
*****/  
size_t tmConditionEdgeLengthFixed::GetNumLinesRest()
{
  return 1;
}


/*****
Put this tmCondition to the stream
*****/    
void tmConditionEdgeLengthFixed::PutRestv4(ostream& os)
{
  mTree->PutPtr(os, mEdge);
}


/*****
Get this tmCondition from the stream
*****/  
void tmConditionEdgeLengthFixed::GetRestv4(istream& is)
{
  mTree->GetPtr(is, mEdge);
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmConditionEdgeLengthFixed, "CNfe")

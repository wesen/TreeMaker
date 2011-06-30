/*******************************************************************************
File:         tmCondition.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmCondition class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmCondition.h"
#include "tmModel.h"

/*
Class tmCondition is a base class for conditions imposed on the tree. This
includes conditions that bind nodes, impose symmetries, and anything else you
could think of. The tmCondition class allows conditions to be easily extended
to arbitrary complexity.

I make a definite distinction between a tmCondition and a constraint. A
constraint is a low-level mathematical equality or inequality that is used by
the mathematical optimization classes.  It operates on vector<tmFloat> objects.

By contrast, a tmCondition is a high-level condition imposed on the tree and
its structure. A tmCondition creates constraints; in fact, many of the
tmConditions create more than one constraint at a time and create different
types of constraints, depending on the type of optimization being carried out.

TreeMaker has many different types of tmCondition and can easily be extended to
add new ones. To add a new type of tmCondition, you must create the new class
as a subclass of class tmCondition overloading/overriding the four member
functions Tag(), TagStr(), GetTag(), and GetTagStr() in the usual way.
*/

/**********
class  tmCondition
Abstract base class for objects that place conditions on a tree or its parts.
**********/
  
/*****
Constructor
Subclass constructors should set the value of mConditionID to the class variable 
condition_ID that is defined in the subclass declaration.
*****/
tmCondition::tmCondition(tmTree* aTree)
  : tmPart(aTree), tmDpptrTarget()
{
  // Register with tree
  aTree->mConditions.push_back(this);
  
  // Register with owner
  mConditionOwner = aTree;
  mConditionOwner->mOwnedConditions.push_back(this);
  
}


/*****
Destructor is explicit only so I can set a breakpoint on destruction during
debugging
*****/
tmCondition::~tmCondition()
{
}


/*****
Returns true if the referenced tmPart* (which can be a tmNode*, tmEdge*, or
tmPath*) is used by the condition. Implemented by subclasses.
*****/
// bool tmCondition::Uses(tmPart*) const;


/*****
Returns true if the condition is still valid, i.e., if the tmParts it
references exist. NOTE: this is not the same as saying the condition is
*satisfied*. A condition can be valid even if its constraints are violated.
This just says it's still meaningful. Implemented by subclasses.
*****/
// bool tmCondition::IsValidCondition();


/*****
Compute whether the condition is feasible, i.e., the node and edge constraint
equations are satisfied, and set mIsFeasibleCondition accordingly. Implemented
by subclasses.
*****/
// void tmCondition::CalcFeasibility();


/*****
Returns true if this is a condition that is attached to a specific node.
Implemented by subclasses.
*****/
// bool tmCondition::IsNodeCondition();


/*****
Returns true if this is a condition that is attached to a specific edge.
Implemented by subclasses.
*****/
// bool tmCondition::IsEdgeCondition();


/*****
Returns true if this is a condition that is attached to a specific path.
Implemented by subclasses.
*****/
// bool tmCondition::IsPathCondition();


/*****
Subclasses create whatever constraints are necessary to implement this
tmCondition in a tmScaleOptimizer. Subclass conditions can call
tmScaleOptimizer::GetBaseOffset(aNode*) to get the indices of the tmNode
coordinates in the state vector. Any of the AddConstraints() functions should
create one or more new Constraint objects and pass them to the Optimizer class;
typically of the form
  t->AddConstraints(new ATypeOfConstraint(parm1, ...));
Implemented by subclasses. See any of the existing tmCondition types for an
example.
*****/
// void tmCondition::AddConstraints(tmScaleOptimizer*);


/*****
Subclasses create whatever constraints are necessary to implement this
condition in a tmEdgeOptimizer. Subclass conditions call
tmEdgeOptimizer::GetBaseOffset(aNode*) to get the indices of the tmNode
coordinates in the state vector and tmEdgeOptimizer::GetFixVarLengths(tmFloat,
tmFloat) to get the fixed and variable parts of the tmPath lengths. Implemented
by subclasses.
*****/
// void tmCondition::AddConstraints(tmEdgeOptimizer*);


/*****
Subclasses create whatever constraints are necessary to implement this
condition in a tmStrainOptimizer. Implemented by subclasses.
*****/
// void tmCondition::AddConstraints(tmStrainOptimizer*);


/*****
Subclasses should return the number of tokens put to or read from a stream or
read from a in the following two routines. Implemented by subclasses.
*****/
// size_t tmCondition::GetNumLinesRest();


/*****
Put the subclass-specific contents of this condition to a stream in version 4
format. Implemented by subclasses.
*****/
// void tmCondition::PutRestv4(ostream& os);


/*****
Get the subclass-specific contents of this condition from a stream in version 4
format. Implemented by subclasses.
*****/
// void tmCondition::GetRestv4(istream& is);

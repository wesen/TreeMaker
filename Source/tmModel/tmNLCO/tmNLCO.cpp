/*******************************************************************************
File:         tmNLCO.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmNLCO constrained optimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-13
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmNLCO.h"

using namespace std;

/**********
class tmNLCO
Abstract class for nonlinear constrained optimizer object used in TreeMaker.
Any constrained optimizer code must be adapted to this interface.
**********/

/*****
Constructor
*****/
tmNLCO::tmNLCO()
  : mSize(0), mUpdater(0), mObjective(0)
{
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Static variable initialization
*****/
tmNLCO::Algorithm tmNLCO::sAlgorithm =
#if defined(tmUSE_ALM)
  tmNLCO::ALM;
#elif defined(tmUSE_WNLIB)
  tmNLCO::WNLIB;
#elif defined(tmUSE_CFSQP)
  tmNLCO::CFSQP;
#elif defined(tmUSE_RFSQP)
  tmNLCO::RFSQP;
#else
  #error "You must turn on at least one optimizer in file tmNLCO.h!"
#endif


/*****
STATIC
Return the current type of nonlinear constrained optimizer being used
*****/
tmNLCO::Algorithm tmNLCO::GetAlgorithm()
{
  return sAlgorithm;
}


/*****
STATIC
Set the choice of nonlinear constrained optimizer, which will affect all future
optimizations.
*****/
void tmNLCO::SetAlgorithm(tmNLCO::Algorithm algorithm)
{
  sAlgorithm = algorithm;
}


/*****
STATIC
Create a nonlinear constrained optimizer of the current type. Client owns the
object and is responsible for its eventual deletion.
*****/
tmNLCO* tmNLCO::MakeNLCO()
{
  switch(sAlgorithm) {
#ifdef tmUSE_ALM
    case tmNLCO::ALM:
      return new tmNLCO_alm();
#endif
#ifdef tmUSE_WNLIB
    case tmNLCO::WNLIB:
      return new tmNLCO_wnlib();
#endif
#ifdef tmUSE_CFSQP
    case tmNLCO::CFSQP:
      return new tmNLCO_cfsqp();
#endif
#ifdef tmUSE_RFSQP
    case tmNLCO::RFSQP:
      return new tmNLCO_rfsqp();
#endif
    default:
      TMFAIL("In tmNLCO::MakeNLCO() algorithm specifier was undefined");
  }
  return 0;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Specify the object (descended from class tmNLCOUpdater) that will receive
calls to update the screen or otherwise display calculation progress.
*****/
void tmNLCO::SetUpdater(tmNLCOUpdater* updater)
{
  mUpdater = updater;
}


/*****
Set the objective function. Subclasses should override but call the base routine
*****/
void tmNLCO::SetObjective(tmDifferentiableFn* f)
{
  TMASSERT(mObjective == 0);
  mObjective = f;
}


/*****
Add any constraint. Subclasses should call this for each type of constraint.
*****/
void tmNLCO::AddConstraint(tmDifferentiableFn* f)
{
  mConstraints.push_back(f);
}


/*****
Set the dimensionality of the problem.
*****/
void tmNLCO::SetSize(size_t n)
{
  TMASSERT(mSize == 0);
  mSize = n;
}


/*****
Update during calls to the objective function. Since most libraries don't give
us access to the appropriate point within the optimization to call an updating
function, the default behavior is to update on every call to the objective
function. Subclasses that can update from within the guts of the optimization
can override ObjectiveUpdateUI() to turn off this behavior; but they need to
call mUpdater->UpdateUI() themselves at the appropriate times during the
optimization.
*****/
void tmNLCO::ObjectiveUpdateUI()
{
  if (mUpdater) mUpdater->UpdateUI();
}


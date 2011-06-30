/*******************************************************************************
File:         tmNLCO_cfsqp.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for CFSQP-based constrained optimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-09
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

/*
tmNLCO_cfsqp is a C++ object that encapsulates Lawrence/Zhou/Tits CFSQP C code
into a C++ object for performing nonlinear constrained optimization.

Compilation:
Include "tmNLCO_cfsqp.h" in your header files.
Include "tmNLCO_cfsqp.cpp" and "cfsqp.noprint.c" in the build, with 
"cfsqp.c" and "qld.c" (which get #included) at the same level.

Notes:
This object utilizes a modified version of CSFQP in which all calls to the 
console are suppressed via some macro hacking.

CFSQP is hard-coded to utilize arrays of double. The external interface of
tmNLCO_cfsqp utilizes vector<double> objects; there is some mapping back and
forth.

At the moment, I don't handle sequentially related constraint sets.

The private class variables have the same names as the input parameters to
CFSQP.

Basic assumptions:
Definitions of user constraints and merit functions are provided by
passing tmDifferentiableFn objects to the tmNLCO_cfsqp object as objective
functions, equality constraints and inequality constraints. Note that these
objects must have both function value and gradient methods defined that are
consistent with each other.

tmNLCO_cfsqp takes over ownership of constraint functions and deletes them when
the tmNLCO_cfsqp object goes out of scope.

Parameters passed to cfsqp() are private member variables that are set via 
calls through the object interface. Parameter values persist from one call
to the next.

Usage:
Create a tmNLCO_cfsqp object.
Set up the bounds.
Create a bunch of tmDifferentiableFn objects and pass them as objective
functions and constraints.
Call the routine "Minimize" with an initial value.  
*/

#include "tmNLCO.h"
#ifdef tmUSE_CFSQP

#ifndef __STDC__
  #define __STDC__
#endif

#include "CFSQP/include/cfsqpusr.h"

using namespace std;

/**********
class tmNLCO_cfsqp
Nonlinear constrained optimizer based on the CFSQP package
**********/

/*****
Constructor
Set the initial value of all of the parameters that control the optimization
to plausible values.
*****/
tmNLCO_cfsqp::tmNLCO_cfsqp()
  : mode(100), nb(0)
{
}


/*****
Destructor
Delete all owned objective and constraint functions
*****/
tmNLCO_cfsqp::~tmNLCO_cfsqp()
{
  for (size_t i = 0; i < mObjs.size(); ++i) delete mObjs[i];
  for (size_t i = 0; i < mNonlinIneqns.size(); ++i) delete mNonlinIneqns[i];
  for (size_t i = 0; i < mLinIneqns.size(); ++i) delete mLinIneqns[i];
  for (size_t i = 0; i < mNonlinEqns.size(); ++i) delete mNonlinEqns[i];
  for (size_t i = 0; i < mLinEqns.size(); ++i) delete mLinEqns[i];
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Set the objective function.
*****/
void tmNLCO_cfsqp::SetObjective(tmDifferentiableFn* f)
{
  tmNLCO::SetObjective(f);
  TMASSERT(mObjs.empty());
  mObjs.push_back(f);
}


/*****
Add a linear equality constraint to the problem.
*****/
void tmNLCO_cfsqp::AddLinearEquality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mLinEqns.push_back(f);
}


/*****
Add an nonlinear equality constraint to the problem.
*****/
void tmNLCO_cfsqp::AddNonlinearEquality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mNonlinEqns.push_back(f);
}


/*****
Add a linear inequality constraint to the problem.
*****/
void tmNLCO_cfsqp::AddLinearInequality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mLinIneqns.push_back(f);
}


/*****
Add an nonlinear inequality constraint to the problem.
*****/
void tmNLCO_cfsqp::AddNonlinearInequality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mNonlinIneqns.push_back(f);
}


/*****
Return the total number of equality constraints.
*****/
size_t tmNLCO_cfsqp::GetNumEqualities()
{
  return mLinEqns.size() + mNonlinEqns.size();
}


/*****
Return the total number of inequality constraints.
*****/
size_t tmNLCO_cfsqp::GetNumInequalities()
{
  return mLinIneqns.size() + mNonlinIneqns.size();
}


/*****
Set the lower and upper bounds on the variables. This should be called no more
than once.
*****/
void tmNLCO_cfsqp::SetBounds(const vector<double>& abl, const vector<double>& abu)
{
  TMASSERT(nb == 0);
  bl = abl;
  bu = abu;
  nb = 2 * mSize;
}

    
/*****
Call the cfsqp() core routine to find the optimum given the starting point
xx. The return value gives the status of the search (0 = normal termination)
and the new optimum is stored in x.
*****/
int tmNLCO_cfsqp::Minimize(vector<double>& x)
{  
  /* Note on mode setting. If the mode is CBA, then:
  C = 1: during the line search, the function that caused the previous value to
  be rejected is checked first, then all functions of the same type will be
  checked. "Recommended for most users."
  C = 2: Constraints will always be checked first at each trial point during
  the line search. "Useful when objective functions are not defined or are
  difficult to evaluate outside of the feasible region."
  B = 0: Algorithm FSQP-AL is selected, resulting in a decrease of the
  (modified) objective function at each iteration.
  B = 1: Algorithm FSQP-NL is selected, resulting in a decrease of the
  (modified) objective function within at most 4 iterations.
  A = 0: Minimize the maximum of one (or more) objective functions.
  A = 1: Minimize the maximum of the absolute value of one (or more) objective
  functions.
  
  Our constraints are easy to evaluate outside of the feasible region, so C = 1
  works fine for us. We're also strictly minimizing objective functions, so A =
  0 is also our choice. The choice of B is less clear.
  
  Some empirical testing with different modes found that mode = 100 can result
  in a lot of "bouncing around" before convergence that goes on for some
  minutes, while mode = 110 converges fairly quickly for scale optimization.
  
  However, for edge optimization, mode=100 seems to work better. So we'll have
  to set this depending on what we're doing using SetCFSQPMode(..).
  
  Note that mode is a member variable and the default (see ctor) is 100.
  */
  
  int iprint = 0;      // no printing
  int miter = 500;    // maximum number of iterations allowed by user
  int inform;        // return value
  double bigbnd = 1.0e10;  // plus infinity
  double eps = 1.0e-5;  // stopping criterion, stop when norm of dirn vector is smaller
  double epseqn = 1.0e-5;  // tolerance on violation of constraints
  double udelta = 0.0e0;  // perturbation size when computing gradients by finite difference

  // argments
  int nparam = int(mSize);    // number of variables
  int nf = int(mObjs.size());    // number of objective functions
  int nfsr = 0;          // number of sets of sequentially related objectives
  nineqn = int(mNonlinIneqns.size());    // number of nonlinear inequality constraints
  nineq = nineqn + int(mLinIneqns.size());// total number of inequality constraints
  neqn = int(mNonlinEqns.size());      // number of nonlinear equality constraints
  neq = neqn + int(mLinEqns.size());    // total number of equality constraints
  int ncsrl = 0;    // number of sets of linear sequentially related inequality constraints
  int ncsrn = 0;    // number of nonlinear sequentially related inequality constraints
  int mesh_pts = 0;  // array of integers giving the structure of sequential stuff

  // Allocate arrays that cfsqp() uses as scratch pads. Note that you must have
  // called or overrode SetBounds() to set the size of vectors bl and bu.
  TMASSERT(nb > 0);
  vector<double> f(size_t(nf + 1));
  vector<double> g(size_t(nineq + neq + 1));
  vector<double> lambda(size_t(nineq + neq + nf + nparam));
  
  // Call the core routine, passing object member variables as parameters. Note
  // that the functions we pass are the static member functions, which reference
  // back to the original object through the "client data" pointer argument.
  cfsqp(nparam, nf, nfsr, nineqn, nineq, neqn, neq, ncsrl, ncsrn, &mesh_pts,
         mode, iprint, miter, &inform, bigbnd, eps, epseqn, udelta, &bl[0], &bu[0], &x[0],
         &f[0], &g[0], &lambda[0], Obj, Constr, Gradobj, Gradcn, this);
         
  // compute the return value
  return inform;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Set the algorithm to be used by this optimizer. See discussion above for meaning.
*****/
void tmNLCO_cfsqp::SetCFSQPMode(int aMode)
{
  mode = aMode;
}


/*****
STATIC
Return the value of the jth objective function evaluated at x in fj
Note that j is a 1-based index
*****/
void tmNLCO_cfsqp::Obj(int, int j, double* x, double* fj, void* cd)
{
  tmNLCO_cfsqp* c = static_cast<tmNLCO_cfsqp*>(cd);
  static vector<double> xx;
  xx.resize(c->mSize);
  ArrayCpy(xx, x);
  *fj = c->mObjs[size_t(j - 1)]->Func(xx);
}


/*****
STATIC
Return the value of the jth constraint function evaluated at x in gj
Note that j is a 1-based index
*****/
void tmNLCO_cfsqp::Constr(int, int j, double* x, double* gj, void* cd)
{
  tmNLCO_cfsqp* c = static_cast<tmNLCO_cfsqp*>(cd);
  static vector<double> xx;
  xx.resize(c->mSize);
  ArrayCpy(xx, x);
  if (j <= c->nineqn) 
    *gj = c->mNonlinIneqns[size_t(j - 1)]->Func(xx);
  else if (j <= c->nineq) 
    *gj = c->mLinIneqns[size_t(j - c->nineqn - 1)]->Func(xx);
  else if (j <= c->nineq + c->neqn) 
    *gj = c->mNonlinEqns[size_t(j - c->nineq - 1)]->Func(xx);
  else 
    *gj = c->mLinEqns[size_t(j - c->nineq - c->neqn - 1)]->Func(xx);
}


/*****
STATIC
Return the value of the gradient of the jth objective function evaluated at x
in gradfj Note that j is a 1-based index
*****/
void tmNLCO_cfsqp::Gradobj(int, int j, double* x, double* gradfj, DummyFn,
  void* cd)
{
  tmNLCO_cfsqp* c = static_cast<tmNLCO_cfsqp*>(cd);
  static vector<double> xx;
  xx.resize(c->mSize);
  ArrayCpy(xx, x);
  static vector<double> gg;
  gg.resize(c->mSize);
  c->mObjs[size_t(j - 1)]->Grad(xx, gg);
  ArrayCpy(gradfj, gg);
}


/*****
STATIC
Return the value of the gradient of the jth constraint function evaluated at x
in gradgj Note that j is a 1-based index
*****/
void tmNLCO_cfsqp::Gradcn(int, int j, double* x, double* gradgj, DummyFn, 
  void* cd)
{
  tmNLCO_cfsqp* c = static_cast<tmNLCO_cfsqp*>(cd);
  static vector<double> xx;
  xx.resize(c->mSize);
  ArrayCpy(xx, x);
  static vector<double> gg;
  gg.resize(c->mSize);
  if (j <= c->nineqn) 
    c->mNonlinIneqns[size_t(j - 1)]->Grad(xx, gg);
  else if (j <= c->nineq) 
    c->mLinIneqns[size_t(j - c->nineqn - 1)]->Grad(xx, gg);
  else if (j <= c->nineq + c->neqn) 
    c->mNonlinEqns[size_t(j - c->nineq - 1)]->Grad(xx, gg);
  else 
    c->mLinEqns[size_t(j - c->nineq - c->neqn - 1)]->Grad(xx, gg);
  ArrayCpy(gradgj, gg);
}

#endif // tmUSE_CFSQP

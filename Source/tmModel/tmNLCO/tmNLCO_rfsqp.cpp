/*******************************************************************************
File:         tmNLCO_rfsqp.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for RFSQP-based constrained optimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-29
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

/*  tmNLCO_rfsqp is a C++ object that encapsulates Lawrence/Tits RFSQP C code
  into a C++ object for performing nonlinear constrained optimization.
  
  Compilation:
  Include "tmNLCO_rfsqp.h" in your header files.
  Include "tmNLCO_rfsqp.cpp" and "rfsqp_noprint.cpp" in the build, with
  all of the RFSQP distribution files (which get #included) at the same level.
  
  Notes:
  This object utilizes a modified version of RFSQP in which all calls to the 
  console are suppressed via some macro hacking.
  
  RFSQP is hard-coded to utilize arrays of double. The external interface of
  tmNLCO_rfsqp utilizes vector<double> objects; there is some mapping back and forth.
  
  At the moment, I don't handle sequentially related constraint sets.
  
  The private class variables have the same names as the input parameters to
  RFSQP.
  
  Basic assumptions:
  Definitions of user constraints and merit functions are provided by
  passing tmDifferentiableFn objects to the tmNLCO_rfsqp object as objective
  functions, equality constraints and inequality constraints. Note that these
  objects must have both function value and gradient methods defined that are
  consistent with each other.
  
  tmNLCO_rfsqp takes over ownership of constraint functions and deletes them when the
  tmNLCO_rfsqp object goes out of scope.
  
  Parameters passed to rfsqp() are private member variables that are set via 
  calls through the object interface. Parameter values persist from one call
  to the next.

  Usage:
  Create a tmNLCO_rfsqp object.
  Set up the bounds.
  Create a bunch of tmDifferentiableFn objects and pass them as objective
  functions and constraints.
  Call the routine "Minimize" with an initial value.  
*/

#include "tmNLCO.h"
#ifdef tmUSE_RFSQP
#include "tmNLCO_rfsqp.h"

using namespace std;

#include "RFSQP/include/user.h"

/**********
class tmNLCO_rfsqp
Nonlinear constrained optimizer based on the RFSQP package
**********/

/*****
Static member initialization
*****/
tmNLCO_rfsqp* tmNLCO_rfsqp::sThis = 0;


/*****
tmNLCO_rfsqp::tmNLCO_rfsqp()
Constructor
Set the initial value of all of the parameters that control the optimization
to plausible values.
*****/
tmNLCO_rfsqp::tmNLCO_rfsqp()
{
  // We can only have one object instantiated at a time because we communicate
  // through this static member variable
  TMASSERT(sThis == 0);
  sThis = this;
}


/*****
tmNLCO_rfsqp::~tmNLCO_rfsqp()
Destructor
Delete all owned objective and constraint functions
*****/
tmNLCO_rfsqp::~tmNLCO_rfsqp()
{
  for (size_t i = 0; i < mObjs.size(); ++i) delete mObjs[i];
  for (size_t i = 0; i < mNonlinIneqns.size(); ++i) delete mNonlinIneqns[i];
  for (size_t i = 0; i < mLinIneqns.size(); ++i) delete mLinIneqns[i];
  for (size_t i = 0; i < mNonlinEqns.size(); ++i) delete mNonlinEqns[i];
  for (size_t i = 0; i < mLinEqns.size(); ++i) delete mLinEqns[i];
  
  sThis = 0;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
void tmNLCO_rfsqp::SetObjective(tmDifferentiableFn* f)
Set the objective function.
*****/
void tmNLCO_rfsqp::SetObjective(tmDifferentiableFn* f)
{
  tmNLCO::SetObjective(f);
  TMASSERT(mObjs.empty());
  mObjs.push_back(f);
}


/*****
void tmNLCO_rfsqp::AddLinearEquality(tmDifferentiableFn* f)
Add a linear equality constraint to the problem.
*****/
void tmNLCO_rfsqp::AddLinearEquality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mLinEqns.push_back(f);
}


/*****
void tmNLCO_rfsqp::AddNonlinearEquality(tmDifferentiableFn* f)
Add an nonlinear equality constraint to the problem.
*****/
void tmNLCO_rfsqp::AddNonlinearEquality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mNonlinEqns.push_back(f);
}


/*****
void tmNLCO_rfsqp::AddLinearInequality(tmDifferentiableFn* f)
Add a linear inequality constraint to the problem.
*****/
void tmNLCO_rfsqp::AddLinearInequality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mLinIneqns.push_back(f);
}


/*****
void tmNLCO_rfsqp::AddNonlinearInequality(tmDifferentiableFn* f)
Add an nonlinear inequality constraint to the problem.
*****/
void tmNLCO_rfsqp::AddNonlinearInequality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mNonlinIneqns.push_back(f);
}


/*****
size_t tmNLCO_rfsqp::GetNumEqualities()
Return the total number of equality constraints.
*****/
size_t tmNLCO_rfsqp::GetNumEqualities()
{
  return mLinEqns.size() + mNonlinEqns.size();
}


/*****
size_t tmNLCO_rfsqp::GetNumInequalities()
Return the total number of inequality constraints.
*****/
size_t tmNLCO_rfsqp::GetNumInequalities()
{
  return mLinIneqns.size() + mNonlinIneqns.size();
}


/*****
void tmNLCO_rfsqp::SetBounds(const vector<double>& abl, const vector<double>& abu)
Set the lower and upper bounds on the variables. This should be called no more
than once.
*****/
void tmNLCO_rfsqp::SetBounds(const vector<double>& abl, const vector<double>& abu)
{
  TMASSERT(abl.size() == abu.size());
  bl = abl;
  bu = abu;
}

    
/*****
int tmNLCO_rfsqp::Minimize(vector<double>& x)
Call the rfsqp() core routine to find the optimum given the starting point
xx. The return value gives the status of the search (0 = normal termination)
and the new optimum is stored in x.
*****/
int tmNLCO_rfsqp::Minimize(vector<double>& x)
{  
  int iprint = 0;      // no printing
  int miter = 500;    // maximum number of iterations allowed by user
  int inform;        // return value
  double bigbnd = 1.0e10;  // plus infinity
  double eps = 1.0e-5;  // stop when norm of dirn vector is smaller than this
  double epseqn = 1.0e-5;  // tolerance on total violation of equality constraints

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

  // Allocate arrays that rfsqp() uses as scratch pads.
  vector<double> f(size_t(nf), 0.0);
  vector<double> g(size_t(nineq + neq), 0.0);
  vector<double> lambda(size_t(nineq + neq + nf + nparam), 0.0);
  
  //  You must have called or overrode SetBounds() to set the size of vectors bl and bu.
  TMASSERT(bl.size() == mSize);
  TMASSERT(bu.size() == mSize);
  TMASSERT(x.size() == mSize);
  
  // Call the core routine, passing object member variables as parameters. Note
  // that the functions we pass are the static member functions, which reference
  // back to the original object through the "client data" pointer argument.
  inform = rfsqp(nparam, nf, nineq, nineqn, nfsr, ncsrn, ncsrl, neq, neqn,
    &mesh_pts, iprint, miter, eps, epseqn, bigbnd, &x[0], &bl[0], &bu[0], &f[0], &g[0],
    &lambda[0], Obj, Constr, Gradobj, Gradcn);
         
  // compute the return value
  return inform;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
STATIC
void tmNLCO_rfsqp::Obj(int, int j, double* x, double* fj)
Return the value of the (j+1)th objective function evaluated at x in fj
Note that j is a 0-based offset
*****/
void tmNLCO_rfsqp::Obj(int, int j, double* x, double* fj)
{
  static vector<double> xx;
  xx.resize(sThis->mSize);
  ArrayCpy(xx, x);
  *fj = sThis->mObjs[size_t(j)]->Func(xx);
}


/*****
STATIC
void tmNLCO_rfsqp::Constr(int, int j, double* x, double* gj)
Return the value of the (j+1)th constraint function evaluated at x in gj
Note that j is a 0-based offset
*****/
void tmNLCO_rfsqp::Constr(int, int j, double* x, double* gj)
{
  static vector<double> xx;
  xx.resize(sThis->mSize);
  ArrayCpy(xx, x);
  if (j < sThis->nineqn)
    *gj = sThis->mNonlinIneqns[size_t(j)]->Func(xx);
  else if (j < sThis->nineq)
    *gj = sThis->mLinIneqns[size_t(j - sThis->nineqn)]->Func(xx);
  else if (j < sThis->nineq + sThis->neqn)
    *gj = sThis->mNonlinEqns[size_t(j - sThis->nineq)]->Func(xx);
  else
    *gj = sThis->mLinEqns[size_t(j - sThis->nineq - sThis->neqn)]->Func(xx);
}


/*****
STATIC
void tmNLCO_rfsqp::Gradobj(int, int j, double* x, double* gradfj)
Return the value of the gradient of the (j+1)th objective function evaluated at x in gradfj
Note that j is a 0-based offset
*****/
void tmNLCO_rfsqp::Gradobj(int, int j, double* x, double* gradfj)
{
  static vector<double> xx;
  xx.resize(sThis->mSize);
  ArrayCpy(xx, x);
  static vector<double> gg;
  gg.resize(sThis->mSize);
  sThis->mObjs[size_t(j)]->Grad(xx, gg);
  ArrayCpy(gradfj, gg);
}


/*****
STATIC
void tmNLCO_rfsqp::Gradcn(int, int j, double* x, double* gradgj)
Return the value of the gradient of the (j+1)th constraint function evaluated at x in gradgj
Note that j is a 0-based offset
*****/
void tmNLCO_rfsqp::Gradcn(int, int j, double* x, double* gradgj)
{
  static vector<double> xx;
  xx.resize(sThis->mSize);
  ArrayCpy(xx, x);
  static vector<double> gg;
  gg.resize(sThis->mSize);
  if (j < sThis->nineqn)
    sThis->mNonlinIneqns[size_t(j)]->Grad(xx, gg);
  else if (j < sThis->nineq)
    sThis->mLinIneqns[size_t(j - sThis->nineqn)]->Grad(xx, gg);
  else if (j < sThis->nineq + sThis->neqn)
    sThis->mNonlinEqns[size_t(j - sThis->nineq)]->Grad(xx, gg);
  else
    sThis->mLinEqns[size_t(j - sThis->nineq - sThis->neqn)]->Grad(xx, gg);
  ArrayCpy(gradgj, gg);
}

#endif // tmUSE_RFSQP

/*******************************************************************************
File:         tmNLCO_wnlib.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmNLCO_wnlib constrained optimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-05
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/


#include "tmNLCO.h"
#ifdef tmUSE_WNLIB
#include "tmNLCO_wnlib.h"
extern "C" { 
#include "wnnlp.h" 
#include "wnmem.h" 
}

using namespace std;

/**********
class tmNLCO_wnlib
Nonlinear constrained optimizer object based on Naylor/Chapman's wnlib library
**********/

/* Notes.
For our class to be exception-safe, we need to deallocate any memory that was
allocated during execution of the routine. Since wnlib wasn't written to be
exception- safe, we don't know where it may have allocated memory relative to
where an exception may be thrown. But what we can do is to have wnlib get all
its memory from a memory group that we create in the constructor; then if an
exception is thrown, all we need to do is make sure that the exception catcher
destroys the tmNLCO_wnlib object (which is typical behavior anyhow) and that
the memory group is deallocated in the constructor.

That creates a potential problem: if we instantiate two tmNLCO_wnlib objects,
each will push a memory group onto wnlib's memory group stack. Freeing a memory
group only frees the top group on the stack, so destroying one of two existing
tmNLCO_wnlib objects could result in one object freeing up the other object's
memory! Not good. To avoid this situation, we'll simply enforce the rule that
there can never be more than one tmNLCO_wnlib object instantiated at one time,
which we do with an assertion in the constructor.
*/

/*****
Constructor
*****/
tmNLCO_wnlib::tmNLCO_wnlib()
{
  objective = NULL;           // no objective function yet
  constraint_list = NULL;         // no constraints yet
  num_nonlinIneqns = 0;
  num_linIneqns = 0;
  num_nonlinEqns = 0;
  num_linEqns = 0;
  
  conj_iterations = 10;
  offset_iterations = 20;
  offset_adjust_rate = 1.;
  
  // Do an assertion check that the default memory group is the current group.
  // This assures that we don't have any other tmNLCO_wnlib objects
  // instantiated, which creates the potential for memory woes if their
  // destructors don't happen in the right order.
  wn_memgp dgp = wn_defaultgp();
  TMASSERT(dgp == wn_curgp());

  // Create a new wnlib memory group.
  wn_gpmake("no_free");
}


/*****
Destructor
*****/
tmNLCO_wnlib::~tmNLCO_wnlib()
{
  // Dispose of all tmDifferentiableFn objects we've taken ownership of.
  for (size_t i = 0; i < ownedDFs.size(); ++i) delete ownedDFs[i];
  
  // We don't need to worry about any memory allocated by wnlib because it
  // goes away when we release the memory group, which we'll do now.
  wn_gpfree();
}


/*****
Set the sole objective. We take ownership of the function f. This may only be
called once. You must have previously set the size.
*****/
void tmNLCO_wnlib::SetObjective(tmDifferentiableFn* f)
{
  tmNLCO::SetObjective(f);
  TMASSERT(mSize != 0);
  TMASSERT(objective == NULL);
  wn_make_nonlinear_constraint(&objective, int(mSize), WN_EQ_COMPARISON);
  for (size_t i = 0; i < mSize; ++i) objective->vars[i] = int(i);
  objective->pfunction = PFunction;
  objective->pgradient = PGradient;
  objective->client_data = f;
  ownedDFs.push_back(f);
}


/*****
Create a linear equality constraint. Since wnlib uses a parsimonious indexing
scheme for linear constraints, for simplicitly we'll treat all linear
constraints based on tmDifferentiableFn objects as if they were nonlinear
constraints. In which case, both linear and nonlinear equalities and
inequalities are built in pretty much the same way. In all cases, we take
ownership of the tmDifferentiableFn f.
*****/
void tmNLCO_wnlib::AddLinearEquality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  TMASSERT(mSize != 0);
  wn_nonlinear_constraint_type nlc;
  wn_make_nonlinear_constraint(&nlc, int(mSize), WN_EQ_COMPARISON);
  for (size_t i = 0; i < mSize; ++i) nlc->vars[i] = int(i);
  nlc->pfunction = PFunction;
  nlc->pgradient = PGradient;
  nlc->client_data = f;
  ownedDFs.push_back(f);
  wn_sllins(&constraint_list, nlc);
  num_linEqns++;
}


/*****
Create a nonlinear equality constraint.
*****/
void tmNLCO_wnlib::AddNonlinearEquality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  TMASSERT(mSize != 0);
  wn_nonlinear_constraint_type nlc;
  wn_make_nonlinear_constraint(&nlc, int(mSize), WN_EQ_COMPARISON);
  for (size_t i = 0; i < mSize; ++i) nlc->vars[i] = int(i);
  nlc->pfunction = PFunction;
  nlc->pgradient = PGradient;
  nlc->client_data = f;
  ownedDFs.push_back(f);
  wn_sllins(&constraint_list, nlc);
  num_nonlinEqns++;
}


/*****
Create a linear inequality constraint.
*****/
void tmNLCO_wnlib::AddLinearInequality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  TMASSERT(mSize != 0);
  wn_nonlinear_constraint_type nlc;
  wn_make_nonlinear_constraint(&nlc, int(mSize), WN_LT_COMPARISON);
  for (size_t i = 0; i < mSize; ++i) nlc->vars[i] = int(i);
  nlc->pfunction = PFunction;
  nlc->pgradient = PGradient;
  nlc->client_data = f;
  ownedDFs.push_back(f);
  wn_sllins(&constraint_list, nlc);
  num_linIneqns++;
}


/*****
Create a nonlinear inequality constraint.
*****/
void tmNLCO_wnlib::AddNonlinearInequality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  TMASSERT(mSize != 0);
  wn_nonlinear_constraint_type nlc;
  wn_make_nonlinear_constraint(&nlc, int(mSize), WN_LT_COMPARISON);
  for (size_t i = 0; i < mSize; ++i) nlc->vars[i] = int(i);
  nlc->pfunction = PFunction;
  nlc->pgradient = PGradient;
  nlc->client_data = f;
  ownedDFs.push_back(f);
  wn_sllins(&constraint_list, nlc);
  num_nonlinIneqns++;
}


/*****
Return the total number of equalities.
*****/
size_t tmNLCO_wnlib::GetNumEqualities()
{
  return num_linEqns + num_nonlinEqns;
}


/*****
Return the total number of inequalities. This does not include the inequalities
created by SetBounds.
*****/
size_t tmNLCO_wnlib::GetNumInequalities()
{
  return num_linIneqns + num_nonlinIneqns;
}


/*****
Set bounds on all of the variables. We will implement these bounds as linear
inequality constraints.
*****/
void tmNLCO_wnlib::SetBounds(const vector<double>& bl, const vector<double>& bu)
{
  TMASSERT(mSize != 0);
  for (size_t i = 0; i < mSize; ++i) {
    // lower bound on ith variable
    wn_linear_constraint_type lcl;
    wn_make_linear_constraint(&lcl, 1, bl[i], WN_GT_COMPARISON);
    lcl->vars[0] = int(i);
    lcl->weights[0] = 1.;
    wn_sllins(&constraint_list, lcl);
    // upper bound on ith variable
    wn_linear_constraint_type lcu;
    wn_make_linear_constraint(&lcu, 1, bu[i], WN_LT_COMPARISON);
    lcu->vars[0] = int(i);
    lcu->weights[0] = 1.;
    wn_sllins(&constraint_list, lcu);
  }
}


/*****
Perform the constrained minimization. The return value gives the state of
success:
WN_SUCCESS = 0 = successful completion, optimum found
WN_SUBOPTIMAL = 1 = termination before optimum found
WN_UNBOUNDED = -2 = function is unbounded
*****/
int tmNLCO_wnlib::Minimize(vector<double>& x)
{
  TMASSERT(mSize != 0);     // make sure we've set the size
  TMASSERT(objective != 0);     // and that we've set an objective
  wn_nlp_verbose = 0;       // don't print any status information
  
  int code;           // result code from wn_nlp_conj_method
  double val_min;         // minimum value of objective function
  double* solution_vect = &x[0];  // output result, written straight into x
  double* delta_vect = NULL;    // signifies nonlin cons have analytic gradients

  // Call the optimization routine, which will return a success code and the
  // solution in variable x.
  wn_nlp_conj_method(&code, &val_min, solution_vect, delta_vect, objective,
    constraint_list, int(mSize), conj_iterations, offset_iterations, 
    offset_adjust_rate);
  return code;
}


/*****
STATIC
Callback that invokes the Func(..) method for a tmDifferentiableFn. A pointer
to this function is provided in each wn_nonlinear_constraint_type_struct.
size = number of input variables
values = ptr to first element of input array
dfobj = ptr to tmDifferentiableFn object
*****/
double tmNLCO_wnlib::PFunction(int size, double* values, callbackptr dfobj)
{
  TMASSERT(size > 0);
  // x is a scratch pad that we reuse; static since PFunction isn't called
  // recursively.
  static vector<double> x;
  x.resize(size_t(size));
  for (size_t i = 0; i < size_t(size); ++i) x[i] = values[i];
  return ((tmDifferentiableFn*)(dfobj))->Func(x);
}


/*****
STATIC
Callback that invokes the Grad(..) method for a tmDifferentiableFn. A pointer
to this function is provided in each wn_nonlinear_constraint_type_struct.
grad = ptr to first element of output array
size = number of input variables
values = ptr to first element of input array
dfobj = ptr to tmDifferentiableFn object
*****/
void tmNLCO_wnlib::PGradient(double* grad, int size, double* values, 
  callbackptr dfobj)
{
  TMASSERT(size > 0);
  // x & g are scratch pads that we reuse; static since PGradient isn't called
  // recursively.
  static vector<double> x;
  x.resize(size_t(size));
  for (size_t i = 0; i < size_t(size); ++i) x[i] = values[i];
  static vector<double> g;
  g.resize(size_t(size));
  ((tmDifferentiableFn*)(dfobj))->Grad(x, g);
  for (size_t i = 0; i < size_t(size); ++i) grad[i] = g[i];
}

#endif // tmUSE_WNLIB

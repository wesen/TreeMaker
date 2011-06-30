/*******************************************************************************
File:         tmNLCO_alm.h
Project:      TreeMaker 5.x
Purpose:      Header file for NLCO using the ALM method
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-11
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmNLCO.h"
#ifdef tmUSE_ALM
#include "tmNLCO_alm.h"
#include "tmMatrix.h"

#include <cmath>
#include <limits>
#include <vector>
#include <sstream>

using namespace std;

// 1 = send information at each iteration to the console.
// 0 = be silent
#define DEBUG_SHOW_PROGRESS 1

// 0 = use summed absolute value for feasibility checking
// 1 = use worst-case for feasibility checking
#define USE_WORST_CASE_FEASIBILITY 1

// 0 = Require minimum change in objective value before returning
// 1 = As soon as we're merely reached feasibility, return.
#define RETURN_ON_FEASIBILITY 0

// A couple of useful inline functions.
template <class T1, class T2>
inline T1 MAX(const T1& t1, const T2& t2) {return (t1 < t2) ? t2 : t1; };

template <class T>
inline T SQR(const T& t) {return t * t;};

/**********
class tmNLCO_alm
Nonlinear constrained optimizer using the Augmented Lagrangian Multiplier method
**********/

/*****
Constructor
*****/
tmNLCO_alm::tmNLCO_alm()
  : mNumBnds(0), mWeight(0), mObjective(NULL)
{
}


/*****
Destructor. Destroy owned constraint functions.
*****/
tmNLCO_alm::~tmNLCO_alm()
{
  if (mObjective) delete mObjective;
  for (size_t i = 0; i < mEqns.size(); ++i) delete mEqns[i];
  for (size_t i = 0; i < mIneqns.size(); ++i) delete mIneqns[i];
}


/*****
Set the objective function to minimize. We take ownership. This must be called
exactly once.
*****/
void tmNLCO_alm::SetObjective(tmDifferentiableFn* f)
{
  tmNLCO::SetObjective(f);
  TMASSERT(mObjective == NULL);
  mObjective = f;
}


/*****
Add a linear equality constraint
*****/
void tmNLCO_alm::AddLinearEquality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mEqns.push_back(f);
}


/*****
Add a nonlinear equality constraint.
*****/
void tmNLCO_alm::AddNonlinearEquality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mEqns.push_back(f);
}


/*****
Add a linear inequality constraint.
*****/
void tmNLCO_alm::AddLinearInequality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mIneqns.push_back(f);
}


/*****
Add a nonlinear inequality constraint
*****/
void tmNLCO_alm::AddNonlinearInequality(tmDifferentiableFn* f)
{
  AddConstraint(f);
  mIneqns.push_back(f);
}


/*****
Return the number of equality constraints
*****/
size_t tmNLCO_alm::GetNumEqualities()
{
  return mEqns.size();
}


/*****
Return the number of inequality constraints
*****/
size_t tmNLCO_alm::GetNumInequalities()
{
  return mIneqns.size();
}


/*****
Record lower and upper bounds in member variables
*****/
void tmNLCO_alm::SetBounds(const vector<double>& bl, const vector<double>& bu)
{
  TMASSERT(bl.size() == bu.size());
  mbl = bl;
  mbu = bu;
  mNumBnds = bl.size();
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Override the default behavior of updating the screen every time the objective
function is called because in ALM, we do our own screen updating in the
optimization outer loop. So we don't do anything here.
*****/
void tmNLCO_alm::ObjectiveUpdateUI()
{
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Find the constrained minimum, starting with the value x and returning the
optimum in the variable x.
*****/
int tmNLCO_alm::Minimize(vector<double>& x)
{
  TMASSERT(mSize != 0);
  TMASSERT(x.size() == mSize);
  TMASSERT(mObjective != 0);
  
  const double WEIGHT_START = 10;      // initial penalty weight
  const double WEIGHT_RATIO = 10;      // growth rate of penalty
  const double WEIGHT_MAX = 1e8;      // maximum penalty weight
  const double TOL_FEAS = 1.0e-5;      // tolerance on feasibility
  const double TOL_F = 1.0e-5;      // tolerance on objective function value
  const double ITER_OUTER_MAX = 50;    // maximum number of outer iterations
  
  size_t ne = mEqns.size();
  size_t ni = mIneqns.size();
  
  // Initialize Lagrangian multipliers. Note: mNumBnds = 0 if we haven't set
  // bounds, = mSize if we have.
  mLagMul.assign(ne + ni + 2 * mNumBnds, 0.);
  
  // Set the maximum step size for line searches to be the space diagonal of
  // the mNumBnds-dimensional box defined by the upper and lower bounds.
  double sum = 0.0;
  for (size_t i = 0; i < mNumBnds; ++i) sum += SQR(mbu[i] - mbl[i]);
  mMaxStep = sqrt(sum);
  if (mMaxStep == 0) { // shouldn't happen
    TMASSERT(mMaxStep != 0);
    mMaxStep = 1;
  }
  
  size_t iter_outer = 1;
  mWeight = WEIGHT_START;
  double fval_old = 1.e30;
  while (iter_outer < ITER_OUTER_MAX) {
    size_t iter_inner = 0;
    double f_alm;
    MinimizeAugLag(x, iter_inner, f_alm);
  
#if USE_WORST_CASE_FEASIBILITY
    // Compute feasibility, using worst-case feasibility. At the same time,
    // we'll update the values of the Lagrangian multipliers in order to be
    // parsimonious with function calls.
    double feas = 0;
    // Go through equalities.
    for (size_t i = 0; i < ne; ++i) {
      double f = mEqns[i]->Func(x);
      feas = MAX(feas, fabs(f));
      double& lm = mLagMul[i];
      lm += 2 * mWeight * f;
    }
    // Go through inequalities.
    for (size_t i = 0; i < ni; ++i) {
      double f = mIneqns[i]->Func(x);
      if (f > 0) feas = MAX(feas, f);
      double& lm = mLagMul[i + ne];
      double mu = -0.5 * lm / mWeight;
      if (f < mu) lm = 0;
      else lm += 2 * mWeight * f;
    };
    // Go through lower bounds.
    for (size_t i = 0; i < mNumBnds; ++i) {
      double f = mbl[i] - x[i];
      if (f > 0) feas = MAX(feas, f);
      double& lm = mLagMul[i + ne + ni];
      double mu = -0.5 * lm / mWeight;
      if (f < mu) lm = 0;
      else lm += 2 * mWeight * f;
    }
    // go through upper bounds.
    for (size_t i = 0; i < mNumBnds; ++i) {
      double f = x[i] - mbu[i];
      if (f > 0) feas = MAX(feas, f);
      double& lm = mLagMul[i + ne + ni + mNumBnds];
      double mu = -0.5 * lm / mWeight;
      if (f < mu) lm = 0;
      else lm += 2 * mWeight * f;
    }
#else
    // Compute feasibility, using summed absolute value of feasibility. At the
    // same time, we'll update the values of the Lagrangian multipliers in
    // order to be parsimonious with function calls.
    double feas = 0;
    // Go through equalities.
    for (size_t i = 0; i < ne; ++i) {
      double f = mEqns[i]->Func(x);
      feas += fabs(f);
      double& lm = mLagMul[i];
      lm += 2 * mWeight * f;
    }
    // Go through inequalities.
    for (size_t i = 0; i < ni; ++i) {
      double f = mIneqns[i]->Func(x);
      if (f > 0) feas += f;
      double& lm = mLagMul[i + ne];
      double mu = -0.5 * lm / mWeight;
      if (f < mu) lm = 0;
      else lm += 2 * mWeight * f;
    };
    // Go through lower bounds.
    for (size_t i = 0; i < mNumBnds; ++i) {
      double f = mbl[i] - x[i];
      if (f > 0) feas += f;
      double& lm = mLagMul[i + ne + ni];
      double mu = -0.5 * lm / mWeight;
      if (f < mu) lm = 0;
      else lm += 2 * mWeight * f;
    }
    // go through upper bounds.
    for (size_t i = 0; i < mNumBnds; ++i) {
      double f = x[i] - mbu[i];
      if (f > 0) feas += f;
      double& lm = mLagMul[i + ne + ni + mNumBnds];
      double mu = -0.5 * lm / mWeight;
      if (f < mu) lm = 0;
      else lm += 2 * mWeight * f;
    }
#endif
    // Get the value of the objective function (NOT the same as f_alm).
    double fval = mObjective->Func(x);

#if DEBUG_SHOW_PROGRESS
    stringstream info;
    info << "i_outer=" << iter_outer << ", i_inner=" << iter_inner << 
      ", feas=" << feas << ", objective = " << fval << endl;
    info << "x[0] = " << x[0] << endl;
    TMLOG(info.str());
#endif // DEBUG_SHOW_PROGRESS

#if RETURN_ON_FEASIBILITY
    if (feas < TOL_FEAS) return 0;
#else
    if (feas < TOL_FEAS) {
      // We're feasible. But have we converged to a minimum? The first check is,
      // if all our Lagrangian multipliers are still zero then no constraints
      // are active and a single step of minimization should have done the trick.

// Unfortunately, the following code snippet fixes one problem but not
// another. If you make a 3-legged tree, strain one edge, then try to minimize
// strain, the program goes into an infinite loop. The following code snippet
// fixes that, but it disrupts convergence in other circumstances.

//       bool noActiveConstraints = true;
//       for (size_t j = 0; j < mLagMul.size(); ++j)
//         if (fabs(mLagMul[j]) != 0) {
//           noActiveConstraints = false;
//           break;
//         }
//       if (noActiveConstraints) return 0;
      
      // If we do have active constraints, the way to determine convergence to
      // a minimum is to see if two successive rounds gave the same value of
      // the objective function.
      if (fabs(fval - fval_old) < TOL_F) return 0;
      fval_old = fval;
    }
#endif
    
    // If we're still here, either we're infeasible or we haven't converged.
    // We've already updated the Lagrangian multipliers; now update the weight.
    mWeight *= WEIGHT_RATIO;
    if (mWeight > WEIGHT_MAX) mWeight = WEIGHT_MAX;

    iter_outer++;
    
    // ALM handles updating a bit differently from the generic approach. Since
    // we have access to our outer loop, we update the screen here rather than
    // calling the updating function from our objective.
    if (mUpdater) mUpdater->UpdateUI();
  }
  // If we got here, we never found a feasible solution.
  return ERROR_TOO_MANY_ITERATIONS;
}


/*****
Minimize the Augmented Lagrangian.
x == The starting point and the returned position of the minimum
iter_inner == the number of iterations that were performed
f_min == the returned minimum value of the function
*****/
void tmNLCO_alm::MinimizeAugLag(vector<double>& x, size_t &iter_inner, 
  double &f_min)
{
  TM_CHECK_NAN(x);

  size_t ITER_INNER_MAX = 200;  // maximum number of iterations
  const double EPS = numeric_limits<double>::epsilon();
  const double TOL_X = 4 * EPS;
  const double TOL_G = 1.0e-5;

  // Calculate starting function value and gradient.
  f_min = AugLagFn(x);
  TM_CHECK_NAN(f_min);
  vector<double> g(mSize);
  AugLagGrad(x, g);
  TM_CHECK_NAN(g);
  
  // Initialize the inverse Hessian matrix and the search direction.
  tmMatrix<double> hess_inv(mSize, mSize);
  vector<double> srch_dir(mSize);
  for (size_t i = 0; i < mSize; ++i) {
    for (size_t j = 0; j < mSize; ++j) hess_inv[i][j] = 0.0;
    hess_inv[i][i] = 1.0;
    srch_dir[i] = -g[i];
  }

  // Enter the main iteration loop.
  vector<double> x_new(mSize), dg(mSize), hdg(mSize);
  for (size_t iter = 1; iter <= ITER_INNER_MAX; ++iter) {
    iter_inner = iter;
    LineSearchAugLag(x, f_min, g, srch_dir, x_new, f_min);
    
    // Update the search direction and current point
    for (size_t i = 0; i < mSize; ++i) {
      srch_dir[i] = x_new[i] - x[i];
      x[i] = x_new[i];
    }
    
    // Construct a test for convergence of step size
    double xtest = 0.0;
    for (size_t i = 0; i < mSize; ++i) {
      double xtemp = fabs(srch_dir[i]) / MAX(fabs(x[i]), 1.0);
      if (xtemp > xtest) xtest = xtemp;
    }
    if (xtest < TOL_X) return;
    
    // Keep a copy of the old gradient and construct a new one at the
    // (new) current point.
    for (size_t i = 0; i < mSize; ++i) dg[i] = g[i];
    AugLagGrad(x, g);
    TM_CHECK_NAN(g);
    
    // Test for convergence on zero gradient
    double gtest = 0.0;
    double den = MAX(f_min, 1.0);
    for (size_t i = 0; i < mSize; ++i) {
      double gtemp = fabs(g[i]) * MAX(fabs(x[i]), 1.0) / den;
      if (gtemp > gtest) gtest = gtemp;
    }
    if (gtest < TOL_G) return;
    
    // Compute the difference between the previous and new gradient and
    // its product with the current inverse Hessian matrix.
    for (size_t i = 0; i < mSize; ++i) dg[i] = g[i] - dg[i];
    for (size_t i = 0; i < mSize; ++i) {
      hdg[i] = 0.0;
      for (size_t j = 0; j < mSize; ++j) hdg[i] += hess_inv[i][j] * dg[j];
    }
    
    // Calculate dot products used in denominators
    double fac(0.0), fae(0.0), sumdg(0.0), sumxi(0.0);
    for (size_t i = 0; i < mSize; ++i) {
      fac += dg[i] * srch_dir[i];
      fae += dg[i] * hdg[i];
      sumdg += SQR(dg[i]);
      sumxi += SQR(srch_dir[i]);
    }
    
    // Update the inverse Hessian matrix. But we can skip updating if it's
    // not sufficiently positive.
    if (fac > sqrt(EPS * sumdg * sumxi)) {
      fac = 1.0 / fac;
      TM_CHECK_NAN(fac);
      double fad = 1.0 / fae;
      TM_CHECK_NAN(fad);
      for (size_t i = 0; i < mSize; ++i) 
        dg[i] = fac * srch_dir[i] - fad * hdg[i];
      for (size_t i = 0; i < mSize; ++i) {
        for (size_t j = i; j < mSize; ++j) {
          hess_inv[i][j] += fac * srch_dir[i] * srch_dir[j] - 
            fad * hdg[i] * hdg[j] + fae * dg[i] * dg[j];
          hess_inv[j][i] = hess_inv[i][j];
        }
      }
    }
    
    // Finally, calculate the next search direction
    for (size_t i = 0; i < mSize; ++i) {
      srch_dir[i] = 0.0;
      for (size_t j = 0; j < mSize; ++j) 
        srch_dir[i] -= hess_inv[i][j] * g[j];
    }
  }
  // If we ended the loop without returning, we've exceeded the number of
  // iterations. Since our outer loop will try again, we can just keep going.
}


/*****
Perform a minimization of the Augmented Lagrangian along a line.
x_old = the previous location
f_old = the previous value of the function at x_old
g_old = the previous value of the gradient (at x_old)
srch_dir = direction of search
x_new = new location of minimum
f_new = new function value
*****/
void tmNLCO_alm::LineSearchAugLag(const vector<double>& x_old, 
  const double f_old, const vector<double>& g_old, 
  vector<double>& srch_dir, vector<double>& x_new, double &f_new)
{
  TM_CHECK_NAN(x_old);
  TM_CHECK_NAN(f_old);
  TM_CHECK_NAN(g_old);
  TM_CHECK_NAN(srch_dir);

  const double ALF = 1.0e-4;  // ensures sufficient decrease in function value
  const double TOL_X = numeric_limits<double>::epsilon();
  
  // scale the direction vector if the attempted step is too big.
  double dir_mag = 0.0;
  for (size_t i = 0; i < mSize; ++i) 
    dir_mag += SQR(srch_dir[i]);
  dir_mag = sqrt(dir_mag);
  if (dir_mag > mMaxStep)
    for (size_t i = 0; i < mSize; ++i) 
      srch_dir[i] *= mMaxStep / dir_mag;
  
  // Check the search direction against the old gradient. If we're going
  // uphill, then we don't want to search any more. NR's version of this
  // routine reports an error, but we'll just let the minimizer restart,
  // hopefully, with a better search direction.
  double slope = 0.0;
  for (size_t i = 0; i < mSize; ++i) 
    slope += g_old[i] * srch_dir[i];

  // TBD, better check?
  if (slope >= 0.0) return;

  // Compute the minimum step length
  double lmtest = 0.0;
  for (size_t i = 0; i < mSize; ++i) {
    double lmtemp = fabs(srch_dir[i]) / MAX(fabs(x_old[i]), 1.0);
    if (lmtest < lmtemp) lmtest = lmtemp;
  }
  double lm_min = TOL_X / lmtest;
  
  // lm is the step size relative to the full Newton step. We always start with
  // a full Newton step, because we get quadratic convergence when we're close
  // to a minimum.
  double lm = 1.0;
  double lm_2 = 0.0;
  double f_new_2 = 0.0;
  for (;;) {
    // Take a step along the search direction from our initial starting point.
    for (size_t i = 0; i < mSize; ++i) x_new[i] = x_old[i] + lm * srch_dir[i];
    f_new = AugLagFn(x_new);
    
    // If the step size is smaller than our minimum tolerance, we're done.
    // Copy the old value into the return value and return.
    if (lm < lm_min) {
      for (size_t i = 0; i < mSize; ++i) x_new[i] = x_old[i];
      return;
    };
    
    // If the function value has decreased sufficiently with this step,
    // we're done.
    double f_tobeat = f_old + ALF * lm * slope;
    if (f_new <= f_tobeat) return;
    
    // But if not, then we'll have to backtrack. Compute a backtrack step.
    double lm_tmp;
    if (lm == 1.0) lm_tmp = -slope / (2.0 * (f_new - f_old - slope));
    else {
      double rhs1 = f_new - f_old - lm * slope;
      double rhs2 = f_new_2 - f_old - lm_2 * slope;
      double lmsqr = SQR(lm);
      double lmsqr2 = SQR(lm_2);
      double lmd = lm - lm_2;
      double a = (rhs1 / lmsqr - rhs2 / lmsqr2) / lmd;
      double b = (-lm_2 * rhs1 / lmsqr + lm * rhs2 / lmsqr2) / lmd;
      if (a == 0.0) lm_tmp = -slope / (2.0 * b);
      else {
        double discr = SQR(b) - 3.0 * a * slope;
        if (discr < 0.0) lm_tmp = 0.5 * lm;
        else if (b <= 0.0) lm_tmp = (-b + sqrt(discr)) / (3.0 * a);
        else lm_tmp = -slope / (b + sqrt(discr));
      }
      if (lm_tmp > 0.5 * lm) lm_tmp = 0.5 * lm;
    }
    lm_2 = lm;
    f_new_2 = f_new;
    lm = MAX(lm_tmp, 0.1 * lm);
  }
}


/*****
The augmented Lagrangian, which gets minimized in the inner loop
*****/
double tmNLCO_alm::AugLagFn(const vector<double>& x)
{
  // A couple of useful numbers to have on hand
  size_t ne = mEqns.size();
  size_t ni = mIneqns.size();

  // Compute objective function value
  double fret = mObjective->Func(x);

  // Accumulate contributions from equality constraints
  for (size_t i = 0; i < ne; ++i) {
    tmDifferentiableFn* eqn = mEqns[i];
    const double& lm = mLagMul[i];
    double f = eqn->Func(x);
    fret += (lm + f * mWeight) * f;
  }
  // Contributions from inequality constraints
  for (size_t i = 0; i < ni; ++i) {
    tmDifferentiableFn* ineqn = mIneqns[i];
    const double& lm = mLagMul[i + ne];
    double f = ineqn->Func(x);
    double mu = -0.5 * lm / mWeight;
    fret += (f < mu) ? mu : (lm + f * mWeight) * f;
  }
  // Contributions from lower bounds
  for (size_t i = 0; i < mNumBnds; ++i) {
    const double& lm = mLagMul[i + ne + ni];
    double f = mbl[i] - x[i];
    double mu = -0.5 * lm / mWeight;
    fret += (f < mu) ? mu : (lm + f * mWeight) * f;
  }
  // Contributions from upper bounds
  for (size_t i = 0; i < mNumBnds; ++i) {
    const double& lm = mLagMul[i + ne + ni + mNumBnds];
    double f = x[i] - mbu[i];
    double mu = -0.5 * lm / mWeight;
    fret += (f < mu) ? mu : (lm + f * mWeight) * f;
  }
  return fret;
}


/*****
Gradient of the augmented Lagrangian
*****/
void tmNLCO_alm::AugLagGrad(const vector<double>& x, vector<double>& g)
{
  TM_CHECK_NAN(mLagMul);

  // A couple of useful numbers to have on hand
  size_t ne = mEqns.size();
  size_t ni = mIneqns.size();
  const double tol_lm = 4.0 * numeric_limits<double>::epsilon();

  // Create a scratch pad for individual constraint gradients
  static vector<double> gscr;
  gscr.resize(mSize);
  
  // compute gradient of objective
  mObjective->Grad(x, g);
  TM_CHECK_NAN(g);

  // Accumulate contributions from equality constraints.
  for (size_t i = 0; i < ne; ++i) {
    tmDifferentiableFn* eqn = mEqns[i];
    const double& lm = mLagMul[i];
    double f = eqn->Func(x);
    TM_CHECK_NAN(f);
    double gmul = lm + 2 * f * mWeight;
    if (fabs(gmul) > tol_lm) {
      eqn->Grad(x, gscr);
      TM_CHECK_NAN(gscr);
      for (size_t j = 0; j < mSize; ++j) 
        g[j] += gmul * gscr[j];
    }
  }
  // Contributions from inequality constraints
  for (size_t i = 0; i < ni; ++i) {
    tmDifferentiableFn* ineqn = mIneqns[i];
    const double& lm = mLagMul[i + ne];
    double f = ineqn->Func(x);
    TM_CHECK_NAN(f);
    double mu = -0.5 * lm / mWeight;
    if (f >= mu) {
      double gmul = lm + 2 * f * mWeight;
      if (fabs(gmul) > tol_lm) {
        ineqn->Grad(x, gscr);
          TM_CHECK_NAN(gscr);
        for (size_t j = 0; j < mSize; ++j) 
          g[j] += gmul * gscr[j];
      }
    }
  }
  // Contributions from lower bounds
  for (size_t i = 0; i < mNumBnds; ++i) {
    const double& lm = mLagMul[i + ne + ni];
    double f = mbl[i] - x[i];
    double mu = -0.5 * lm / mWeight;
    if (f >= mu) {
      double gmul = lm + 2 * f * mWeight;
      if (fabs(gmul) > tol_lm) {
        g[i] -= gmul;
      }
    }
  }
  // Contributions from upper bounds
  for (size_t i = 0; i < mNumBnds; ++i) {
    const double& lm = mLagMul[i + ne + ni + mNumBnds];
    double f = x[i] - mbu[i];
    double mu = -0.5 * lm / mWeight;
    if (f >= mu) {
      double gmul = lm + 2 * f * mWeight;
      if (fabs(gmul) > tol_lm) {
        g[i] += gmul;
      }
    }
  }
}

#endif // tmUSE_ALM

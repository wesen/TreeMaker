/*******************************************************************************
File:         tmNewtonRaphson.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker Newton-Raphson equation solver class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMNEWTONRAPHSON_H_
#define _TMNEWTONRAPHSON_H_

#include "tmHeader.h"
#include "tmMatrix.h"

/*
This class implements a Newton-Raphson  nonlinear equation solver. In your
subclass, you provide a virtual method that computes the gradient and the
function values to be zeroed. The method is based on the algorithms described
in Press, Flannery et al's Numerical Methods in C. My implementation differs
from PFTV's in several ways:
--PFTV require UserFn to supply the _negative_ of the function; in my 
implementation, you supply the function as well as its gradient.
--Run-time errors generation exceptions, which are member classes.
--My arrays are zero-based (PFTV uses 1-based indexing -- ick!)
*/

/**********
class tmNewtonRaphson<T>
**********/
template <class T>
class tmNewtonRaphson {
public:
  class EX_TOO_MANY_ITERATIONS {};
  class EX_SINGULAR_MATRIX {};
  virtual ~tmNewtonRaphson () {};
  void SolveEqns(std::size_t ntrials, std::vector<T>& x, const T& tolx, 
    const T& tolf);
  void LUDecomposition(tmMatrix<T>& a, T& d);
  void LUBackSubstitution(tmMatrix<T>& a, std::vector<T>& b);
  // Getters
  const std::vector<T>& GetRowScaling() {return mRowScaling;};
  const std::vector<std::size_t>& GetRowPermutation() {
    return mRowPermutation;};
  const std::vector<T>& GetFnVals() {return mFnVals;};
  const tmMatrix<T>& GetJacobian() {return mJacobian;};
protected:
  // Subclasses override
  virtual void UserFn(const std::vector<T>& x, tmMatrix<T>& a, 
    std::vector<T>& b);
private:
  std::vector<T> mRowScaling;          // row scaling for LU decomposition
  std::vector<std::size_t> mRowPermutation;  // row permutation for LU decomp'n
  std::vector<T> mFnVals;            // function values
  tmMatrix<T> mJacobian;            // function gradients (Jacobian matrix)
};
  

/**********
Template definitions
**********/
  
/*****
Given an initial guess x[0..n-1] for a root in n dimensions, take ntrials
Newton-Raphson steps to improve the root. Stop if the root converges in either
summed absolute variable increnets tolx or summed absolute function values
tolf. If the number of iterations exceeds ntrials, throw an exception.
*****/
template <class T>
void tmNewtonRaphson<T>::SolveEqns(std::size_t ntrials, std::vector<T>& x, 
  const T& tolx, const T& tolf)
{
  std::size_t n = x.size();
  mRowPermutation.resize(n);
  mFnVals.resize(n);
  mJacobian.resize(n, n);
  for (std::size_t k = 0; k < ntrials; ++k) {
    UserFn(x, mJacobian, mFnVals);
    T errf = T(0.);
    for (std::size_t i = 0; i < n; ++i) errf += fabs(mFnVals[i]);
    if (errf <= tolf) return;
    T permParity;
    LUDecomposition(mJacobian, permParity);
    LUBackSubstitution(mJacobian, mFnVals);
    T errx = T(0.);
    for (std::size_t i = 0; i < n; ++i) {
      errx += fabs(mFnVals[i]);
      x[i] -= mFnVals[i];
    }
    if (errx <= tolx) return;
  }
  throw EX_TOO_MANY_ITERATIONS();
}


/*****
This is the function that defines the equations to be solved. Users must
override. Users provide x vector in first parameter, Jacobian matrix in 2nd,
and function value in 3rd parameter.
*****/
template <class T>
void tmNewtonRaphson<T>::UserFn(const std::vector<T>&, tmMatrix<T>&, 
  std::vector<T>&)
{
  // "No user function provided for tmNewtonRaphson<T>"
  TMASSERT(false);
}


/*****
Perform the LU decomposition of a matrix in place. Given a matrix a, this
routine replaces it by the LU decomposition of a rowwise permutation of itself.
a is input and output, arranged as in equation (2.3.14) in PFTV;
mRowPermutation is an output vector that records the row permutation effected
by the partial pivoting; d is output as ±1 depending on whether the number of
row interchanges was even or odd, respectively. This routine is used in
combination with LUBackSubstitution to solve linear equations or invert a
matrix.
*****/
template <class T>
void tmNewtonRaphson<T>::LUDecomposition(tmMatrix<T>& a, T& d)
{
  std::size_t n = a.GetRows();
  TMASSERT(a.GetCols() == n);
  mRowScaling.resize(n);
  mRowPermutation.resize(n);
  const T TINY = T(1.0e-20);
  d = T(1.0);          // no row interchanges yet

  // loop over rows to get the implicit scaling information and check for
  // singularity
  T big, dum, sum, temp;
  for (std::size_t i = 0; i < n; ++i) {
    big = T(0.0);
    for (std::size_t j = 0; j < n; ++j)
      if ((temp = fabs(a[i][j])) > big) big = temp;
    if (big == 0.0) throw EX_SINGULAR_MATRIX();
    mRowScaling[i] = T(1.0) / big;  // save the scaling
  }
  
  // This is the loop over columns of Crout's method
  for (std::size_t j = 0; j < n; ++j) {
    std::size_t imax = size_t(-1);
    for (std::size_t i = 0; i < j; ++i) {
      sum = a[i][j];
      for (std::size_t k = 0; k < i; ++k) sum -= a[i][k] * a[k][j];
      a[i][j] = sum;
    }
    
    // initializes the search for the largest pivot element
    big = T(0.0);
    for (std::size_t i = j; i < n; ++i) {
      sum = a[i][j];
      for (std::size_t k = 0; k < j; ++k)
        sum -= a[i][k] * a[k][j];
      a[i][j] = sum;
      
      // is figure of merit for pivot better than the best so far?
      if ( (dum = mRowScaling[i] * fabs(sum)) >= big) {
        big = dum;
        imax = i;
      }
    }
    
    // Do we need to interchange rows?
    if (j != imax) {
      for (std::size_t k = 0; k < n; ++k) {
        dum = a[imax][k];
        a[imax][k] = a[j][k];
        a[j][k] = dum;
      }
      d = -d;        // update parity of d
      mRowScaling[imax] = mRowScaling[j];  // and interchange the scale factor.
    }
    mRowPermutation[j] = imax;
    
    // If the pivot element is zero, the matrix is singular (to the precision
    // of the algorithm). For some applications, it's desirable to substitute
    // TINY for zero.
    if (a[j][j] == 0.0) a[j][j] = TINY;
    if (j != n) {
      dum = T(1.0) / (a[j][j]);  // Divide by the pivot element
      for (std::size_t i = j + 1; i < n; ++i) a[i][j] *= dum;
    }
  }
}



/*****
Solves the set of n linear equations a*x==b. Here a is input as the LU
decomposition of the matrix, as determined by the routine LUDecomposition. b is
input as the right-hand side of the vector and returns with the solution vector
x. a and mRowPermutation are not modified by this routine and can be left in
place for successive calls with different right-hand-sides b.
*****/
template <class T>
void tmNewtonRaphson<T>::LUBackSubstitution(tmMatrix<T>& a, std::vector<T>& b)
{
  std::size_t n = a.GetRows();
  TMASSERT(a.GetCols() == n);
  TMASSERT(b.size() == n);
  TMASSERT(n >= 1);
    
  // When ii is valid, it is the index of the first nonvanishing element of b.
  // We now do the forward substitution, unscrambling the permutation as we go.
  std::size_t BAD_INDEX = std::size_t(-1);
  std::size_t ii = BAD_INDEX;
  T sum = T(0.);
  for (std::size_t i = 0; i < n; ++i) {
    std::size_t ip = mRowPermutation[i];
    sum = b[ip];
    b[ip] = b[i];
    if (ii != BAD_INDEX)
      for (std::size_t j = ii; j <= i - 1; ++j) sum -= a[i][j] * b[j];
    else if (sum) ii = i;
    b[i] = sum;
  }
  
  // Now do the back substitution. Note that we're iterating backward on i, so
  // since i is an unsigned long, our stopping criterion is wrap-around rather
  // than (i >= 0).
  for (std::size_t i = n - 1; i != BAD_INDEX; i--) {
    sum = b[i];
    for (std::size_t j = i + 1; j < n; ++j) sum -= a[i][j] * b[j];
    b[i] = sum / a[i][i];
  }
}


#endif // _TMNEWTONRAPHSON_H_

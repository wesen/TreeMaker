/*******************************************************************************
File:         tmNLCO_alm.h
Project:      TreeMaker 5.x
Purpose:      Header file for NLCO using the ALM method
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-10
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMNLCO_ALM_H_
#define _TMNLCO_ALM_H_

#include "tmNLCO.h"

/**********
class tmNLCO_alm
Nonlinear constrained optimizer using the Augmented Lagrangian Multiplier method
**********/
class tmNLCO_alm : public virtual tmNLCO {
public:
  // Reasons for abnormal termination
  enum {
    ERROR_TOO_MANY_ITERATIONS = 1
  };

  tmNLCO_alm();
  ~tmNLCO_alm();

  void SetObjective(tmDifferentiableFn* f);
  void AddLinearEquality(tmDifferentiableFn* f);
  void AddNonlinearEquality(tmDifferentiableFn* f);
  void AddLinearInequality(tmDifferentiableFn* f);
  void AddNonlinearInequality(tmDifferentiableFn* f);
  
  std::size_t GetNumEqualities();
  std::size_t GetNumInequalities();
  
  void SetBounds(const std::vector<double>& bl, const std::vector<double>& bu);
  
  int Minimize(std::vector<double>& x);
  
  void ObjectiveUpdateUI();
  
private:
  std::size_t mNumBnds;            // number of points we're optimizing
  std::vector<double> mbl;          // lower bound
  std::vector<double> mbu;          // upper bound
  std::vector<double> mLagMul;        // Lagrangian multipliers
  double mWeight;                // weighting factor
  tmDifferentiableFn* mObjective;        // objective function
  std::vector<tmDifferentiableFn*> mEqns;    // equality constraints
  std::vector<tmDifferentiableFn*> mIneqns;  // inequality constraints
  double mMaxStep;              // maximum step size in line searches
  
  void MinimizeAugLag(std::vector<double>& x, std::size_t &iter, double &f_min);
  void LineSearchAugLag(const std::vector<double>& x_old, const double f_old, 
    const std::vector<double>& g_old, std::vector<double>& srch_dir, 
    std::vector<double>& x_new, double &f_new);
  double AugLagFn(const std::vector<double>& x);
  void AugLagGrad(const std::vector<double>& x, std::vector<double>& gradx);

};

#endif // _TMNLCO_ALM_H_

/*******************************************************************************
File:         tmNLCO_rfsqp.h
Project:      TreeMaker 5.x
Purpose:      Header file for RFSQP-based constrained optimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-27
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMNLCO_RFSQP_H_
#define _TMNLCO_RFSQP_H_

#include "tmNLCO.h"

/**********
class tmNLCO_rfsqp
Nonlinear constrained optimizer based on the RFSQP package
**********/
class tmNLCO_rfsqp : public virtual tmNLCO {
public:
  enum {
    ERROR_QPSOLVER_FAILED = 1,
    ERROR_LINE_SEARCH_FAILED = 2,
    ERROR_MAX_ITERATIONS = 3,
    ERROR_INFEASIBLE_EQUALITY = 4,
    ERROR_INFEASIBLE_AFFINE = 5
  };
  tmNLCO_rfsqp();
  ~tmNLCO_rfsqp();
  
  void SetObjective(tmDifferentiableFn* f);
  void AddLinearEquality(tmDifferentiableFn* f);
  void AddNonlinearEquality(tmDifferentiableFn* f);
  void AddLinearInequality(tmDifferentiableFn* f);
  void AddNonlinearInequality(tmDifferentiableFn* f);
  
  std::size_t GetNumEqualities();
  std::size_t GetNumInequalities();
  
  void SetBounds(const std::vector<double>& bl, const std::vector<double>& bu);
  
  int Minimize(std::vector<double>& x);
private:
  int nineqn;      // number of nonlinear inequality constraints
  int nineq;      // total number of inequality constraints
  int neqn;      // number of nonlinear equality constraints
  int neq;      // total number of equality constraints

  std::vector<double> bl;    // array of lower bound on variables
  std::vector<double> bu;    // array of upper bound on variables  

  static void Obj(int n, int j, double* x, double* fj);
  static void Constr(int n, int j, double* x, double* gj);
  static void Gradobj(int n, int j, double* x, double* gradfj);
  static void Gradcn(int n, int j, double* x, double* gradgj);
  static tmNLCO_rfsqp* sThis;
    
  std::vector<tmDifferentiableFn*> mObjs;        // objectives
    std::vector<tmDifferentiableFn*> mNonlinIneqns;    // nonlinear inequalities
  std::vector<tmDifferentiableFn*> mLinIneqns;    // linear inequalities
  std::vector<tmDifferentiableFn*> mNonlinEqns;    // nonlinear equalities
  std::vector<tmDifferentiableFn*> mLinEqns;      // linear equalities
};

#endif // _TMNLCO_RFSQP_H_

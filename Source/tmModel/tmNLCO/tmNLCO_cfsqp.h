/*******************************************************************************
File:         tmNLCO_cfsqp.h
Project:      TreeMaker 5.x
Purpose:      Header file for CFSQP-based constrained optimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-03
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMNLCO_CFSQP_H_
#define _TMNLCO_CFSQP_H_

#include "tmNLCO.h"

/**********
class tmNLCO_cfsqp
Nonlinear constrained optimizer based on the CFSQP package
**********/
class tmNLCO_cfsqp : public virtual tmNLCO {
public:
  enum {
    ERROR_INFEASIBLE_LINEAR = 1,
    ERROR_INFEASIBLE_NONLINEAR = 2,
    ERROR_MAX_ITERATIONS = 3,
    ERROR_LINESEARCH_FAILED = 4,
    ERROR_QPSOLVER_FAILED_D0 = 5,
    ERROR_QPSOLVER_FAILED_D1 = 6,
    ERROR_INPUT_NOT_CONSISTENT = 7,
    ERROR_STOPPING_NOT_REACHED = 8,
    ERROR_PENALTY_TOO_LARGE = 9
  };
  tmNLCO_cfsqp();
  ~tmNLCO_cfsqp();
  
  void SetObjective(tmDifferentiableFn* f);
  void AddLinearEquality(tmDifferentiableFn* f);
  void AddNonlinearEquality(tmDifferentiableFn* f);
  void AddLinearInequality(tmDifferentiableFn* f);
  void AddNonlinearInequality(tmDifferentiableFn* f);
  
  std::size_t GetNumEqualities();
  std::size_t GetNumInequalities();
  
  void SetBounds(const std::vector<double>& bl, const std::vector<double>& bu);
  
  int Minimize(std::vector<double>& x);
  void SetCFSQPMode(int aMode);
private:
  int mode;      // chooses which algorithm and some convergence criteria
  int nineqn;      // number of nonlinear inequality constraints
  int nineq;      // total number of inequality constraints
  int neqn;      // number of nonlinear equality constraints
  int neq;      // total number of equality constraints

  std::size_t nb;        // number of bounds variables set
  std::vector<double> bl;    // array of lower bound on variables
  std::vector<double> bu;    // array of upper bound on variables  

  typedef void (*DummyFn)(int, int, double*, double*, void*);
  static void Obj(int n, int j, double* x, double* fj, void* cd);
  static void Constr(int n, int j, double* x, double* gj, void* cd);
  static void Gradobj(int n, int j, double* x, double* gradfj, DummyFn d, void* cd);
  static void Gradcn(int n, int j, double* x, double* gradgj, DummyFn d, void* cd);
    
  std::vector<tmDifferentiableFn*> mObjs;        // objectives
    std::vector<tmDifferentiableFn*> mNonlinIneqns;    // nonlinear inequalities
  std::vector<tmDifferentiableFn*> mLinIneqns;    // linear inequalities
  std::vector<tmDifferentiableFn*> mNonlinEqns;    // nonlinear equalities
  std::vector<tmDifferentiableFn*> mLinEqns;      // linear equalities
};

#endif // _TMNLCO_CFSQP_H_

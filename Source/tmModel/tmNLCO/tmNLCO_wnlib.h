/*******************************************************************************
File:         tmNLCO_wnlib.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmNLCO_wnlib constrained optimizer class
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-05
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMNLCO_WNLIB_H_
#define _TMNLCO_WNLIB_H_

#include "tmNLCO.h"

// fwd declarations (wnlib types)
typedef struct wn_sll_struct *wn_sll;
typedef struct wn_nonlinear_constraint_type_struct 
  *wn_nonlinear_constraint_type;
typedef void* callbackptr;


/**********
class tmNLCO_wnlib
Nonlinear constrained optimizer object based on Naylor/Chapman's wnlib library
**********/

class tmNLCO_wnlib : public virtual tmNLCO {
public:
  tmNLCO_wnlib();
  ~tmNLCO_wnlib();

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
  wn_nonlinear_constraint_type objective; // the sole objective function
  wn_sll constraint_list;     // singly linked list of constraints
  int conj_iterations;        // iterations in conjugate gradient algorithm
  int offset_iterations;      // iterations to adjust constraint offsets
  double offset_adjust_rate;  // aggressiveness in adjusting constraint offsets

  std::vector<tmDifferentiableFn*> ownedDFs;  // DF's we've taken ownership of
  std::size_t num_nonlinIneqns;   // number of nonlinear inequality constraints
  std::size_t num_linIneqns;
  std::size_t num_nonlinEqns;
  std::size_t num_linEqns;

  static double PFunction(int size, double* values, callbackptr dfobj);
  static void PGradient(double* grad, int size, double* values, 
    callbackptr dfobj);
};

#endif // _TMNLCO_WNLIB_H_

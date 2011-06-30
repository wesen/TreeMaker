/*******************************************************************************
File:         tmNLCO.h
Project:      TreeMaker 5.x
Purpose:      Header file that defines the TreeMaker optimizer
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-14
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMNLCO_H_
#define _TMNLCO_H_

#include "tmHeader.h"
#include <vector>

/*
This header file specifies which of the different optimizer classes are
available to be used throughout TreeMaker. It is possible to choose different
optimization algorithms at run-time, depending on what libraries are compiled
in. This header file lets us specify which optimizers to make available and
defines some enums for picking among the various optimizers.

You can define any or all of the following flags (and add new ones of your own)
to make the associated optimizer available. Of course, the related optimization
code must be compiled as well or a link error will result.
*/

//#define tmUSE_CFSQP
//#define tmUSE_RFSQP
#define tmUSE_ALM
//#define tmUSE_WNLIB

/*
The TreeMaker nonlinear optimization classes include support for profiling and
benchmarking different optimizers. #define TM_PROFILE_OPTIMIZERS = 1 to include
profiling support, 0 to disable it. See class definitions of tmNLCO and
tmDifferentiableFn for details, but briefly, turning on the flag means that
objective functions and constraints record how many times they were called
during the optimization.
*/

#if defined(TMDEBUG) || defined(TMPROFILE)
  #define TM_PROFILE_OPTIMIZERS 1
#else
  #define TM_PROFILE_OPTIMIZERS 0
#endif // defined(TMDEBUG) || defined(TMPROFILE)


/**********
class tmDifferentiableFn
Object that computes a scalar-valued function of a vector and the vector-valued
gradient.
**********/
class tmDifferentiableFn {
public:
  virtual ~tmDifferentiableFn() {};
  virtual double Func(const std::vector<double>& x) = 0;
  virtual void Grad(const std::vector<double>& x, 
    std::vector<double>& gradx) = 0;
#if TM_PROFILE_OPTIMIZERS
  tmDifferentiableFn() : mFuncCalls(0), mGradCalls(0) {};
  std::size_t GetNumFuncCalls() const {return mFuncCalls;};
  std::size_t GetNumGradCalls() const {return mGradCalls;};
  void ResetCalls() {mFuncCalls = 0; mGradCalls = 0;};
#else
  void ResetCalls() {};
#endif
protected:
#if TM_PROFILE_OPTIMIZERS
  void IncFuncCalls() {++mFuncCalls;};
  void IncGradCalls() {++mGradCalls;};
#else
  void IncFuncCalls() {};
  void IncGradCalls() {};
#endif
private:  
#if TM_PROFILE_OPTIMIZERS
  std::size_t mFuncCalls; // number of function calls since creation or reset
  std::size_t mGradCalls; // number of gradient calls since creation or reset
#endif // TM_PROFILE_OPTIMIZERS
};


/**********
class tmNLCOUpdater
Abstract class for object that will perform UI updating. If you pass one of
these objects to a tmNLCO via tmNLCO::SetUpdater(), the optimizer will
periodically call this object's UpdateUI() method, which can be used to
implement showing progress of the calculation.
**********/
class tmNLCOUpdater {
public:
  virtual void UpdateUI() = 0;
  virtual ~tmNLCOUpdater() {};
};


/**********
class tmNLCO
Abstract class for nonlinear constrained optimizer object used in TreeMaker.
Any constrained optimizer code must be adapted to this interface.
**********/
class tmNLCO {
public:
  // These enums are used for run-time selection of the choice of
  // optimization algorithm.
  enum Algorithm {
#ifdef tmUSE_CFSQP
    CFSQP,
#endif
#ifdef tmUSE_RFSQP
    RFSQP,
#endif
#ifdef tmUSE_ALM
    ALM,
#endif
#ifdef tmUSE_WNLIB
    WNLIB,
#endif
    NUM_ALGORITHMS };

  // Exception classes
  class EX_BAD_CONVERGENCE {
  public:
    int mReason;  // meaning is specific to optimizer implementation
    EX_BAD_CONVERGENCE(const int reason = -1) : mReason(reason) {};
    int GetReason() const {return mReason;};
  };
  // Ctor/dtor
  tmNLCO();
  virtual ~tmNLCO() {};
  
  // Setting the global NCLO type and obtaining an optimizer object
  static Algorithm GetAlgorithm();
  static void SetAlgorithm(Algorithm algorithm);
  static tmNLCO* MakeNLCO();
  
  void SetUpdater(tmNLCOUpdater* updater);
  virtual void SetSize(std::size_t);

  virtual std::size_t GetNumEqualities() = 0;
  virtual std::size_t GetNumInequalities() = 0;
  virtual std::size_t GetNumVariables() {return mSize;};
  
  tmDifferentiableFn* GetObjective() const {
    return mObjective;};
  const std::vector<tmDifferentiableFn*>& GetConstraints() const {
    return mConstraints;
  };
  
  // Setting up the problem
  virtual void SetObjective(tmDifferentiableFn* f);
  void AddConstraint(tmDifferentiableFn* f);
  virtual void AddLinearEquality(tmDifferentiableFn* f) = 0;
  virtual void AddNonlinearEquality(tmDifferentiableFn* f) = 0;
  virtual void AddLinearInequality(tmDifferentiableFn* f) = 0;
  virtual void AddNonlinearInequality(tmDifferentiableFn* f) = 0;
  virtual void SetBounds(const std::vector<double>& bl, 
    const std::vector<double>& bu) = 0;
    
  // Performing the optimization
  virtual int Minimize(std::vector<double>& x) = 0;
  
  // UI updating from the objective function
  virtual void ObjectiveUpdateUI();

protected:
  std::size_t mSize;        // dimensionality of the problem
  tmNLCOUpdater* mUpdater;  // object that shows progress

  // Utility for copying between dimensioned vectors and C arrays in which size
  // is always taken from the vector.
  static void ArrayCpy(std::vector<double>& dst, const double* const src) {
    for (std::size_t i = 0; i < dst.size(); ++i) dst[i] = src[i];};
  static void ArrayCpy(double* const dst, const std::vector<double>& src) {
    for (std::size_t i = 0; i < src.size(); ++i) dst[i] = src[i];};

private:
  static Algorithm sAlgorithm;                    // which algorithm to use
  tmDifferentiableFn* mObjective;                 // Objective to minimize
  std::vector<tmDifferentiableFn*> mConstraints;  // Ineq & eq constraints
};


/*****
Include header files for the supported nonlinear constrained optimizer classes.
*****/

#ifdef tmUSE_CFSQP
  #include "tmNLCO_cfsqp.h"
#endif

#ifdef tmUSE_RFSQP
  #include "tmNLCO_rfsqp.h"
#endif

#ifdef tmUSE_ALM
  #include "tmNLCO_alm.h"
#endif

#ifdef tmUSE_WNLIB
  #include "tmNLCO_wnlib.h"
#endif

#endif // _TMNLCO_H_

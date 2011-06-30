/*******************************************************************************
File:         tmNLCOTester.h
Project:      TreeMaker 5.x
Purpose:      Common header file for test code for nonlinear constrained optimizers
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-09
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMNLCOTESTER_H_
#define _TMNLCOTESTER_H_

/*
This header defines classes and template functions that can be instantiated for
any subclass of tmNLCO. It thus provides a common test suite we can use for
any candidate nonlinear constrained optimizer.
*/

#include <cmath>
#include <iostream>
#include <ctime>
#ifdef linux
// CLK_TCK is obsolete
#define CLK_TCK CLOCKS_PER_SEC
#endif
using namespace std;

#include "tmNLCO.h"

/**********
Utility routines for reporting output
**********/

/*****
Write the value of a vector to the console
*****/
void ReportVector(const char* name, const std::vector<double>& v);

/*****
Write the value of a vector in which consecutive values make up ordered pairs.
*****/
void ReportPairVector(const char* name, const std::vector<double>& v);

/*****
Write the number of function and gradient calls for a single function.
*****/
void ReportCalls(const char* name, tmDifferentiableFn* f);

/*****
Write the total number of function and gradient calls for a list of functions.
*****/
void ReportCalls(const char* name, 
  const std::vector<tmDifferentiableFn*>& flist);

/*****
Write the feasibility for the given lists of constraints
*****/
void ReportFeasibility(const std::vector<double>& x, 
  const std::vector<tmDifferentiableFn*>& eq_list,
  const std::vector<tmDifferentiableFn*>& ineq_list);


/***********************************************************
A really simple optimization with one linear constraint.
The objective is x[0]^2 + x[1]^2
The linear constraint is x[0] - x[1] == 1
The optimum solution is x[0]-->0.5, x[1]->-0.5.
***********************************************************/

/**********
class G1
Objective function
x[0]^2 + x[1]^2
**********/
class G1 : public tmDifferentiableFn {
public:
  double Func(const std::vector<double>& x) {
    IncFuncCalls();
    return std::pow(x[0], 2) + std::pow(x[1], 2);
  };
  void Grad(const std::vector<double>& x, std::vector<double>& gradx) {
    IncGradCalls();
    gradx[0] = 2 * x[0];
    gradx[1] = 2 * x[1];
  };
};


/**********
class G2
Linear equality constraint
x[0] - x[1] - 1 == 0
**********/
class G2 : public tmDifferentiableFn {
public:
  double Func(const std::vector<double>& x) {
    IncFuncCalls();
    return x[0] - x[1] - 1;
  };
  void Grad(const std::vector<double>&, std::vector<double>& gradx) {
    IncGradCalls();
    gradx[0] = 1.;
    gradx[1] = -1.;
  };
};


/*****
Routine for simple optimization with one constraint. Class T is the
subclass of tmNLCO being tested.
*****/
template <class T>
int Test_simple()
{
  cout << "Simple optimization:\n";
  cout << "Minimize x[0]^2 + x[1]^2 s.t. x[0] - x[1] == 1.\n";
  
  // Create and initialize the minimizer object
  T testMinimizer;
  testMinimizer.SetSize(2);
  
  // Set some bounds
  std::vector<double> bl, bu;
  bl.assign(2, -1.);
  bu.assign(2, 1.);
  testMinimizer.SetBounds(bl, bu);
  
  // Create objective functions and constraints
  G1* objective = new G1;
  testMinimizer.SetObjective(objective);
  G2* constraint = new G2;
  testMinimizer.AddLinearEquality(constraint);  
  
   // Set up initial solution in member variable, which is infeasible.
  std::vector<double> x(2);
  x[0] = 2.3;
  x[1] = 4.7;
  
  // Run the calculation
  clock_t start = clock();
  int code = testMinimizer.Minimize(x);
  clock_t stop = clock();
  
  // Display the result
  cout.setf(ios_base::fixed);
  cout.precision(6);
  cout << "1 objective, 1 equality constraint" << endl;
  ReportVector("x", x);
  cout << "Result code = " << code << endl;
  ReportCalls("objective", objective);
  ReportCalls("constraint", constraint);
  vector<tmDifferentiableFn*> total;
  total.push_back(objective);
  total.push_back(constraint);
  ReportCalls("total", total);
  cout << "time = " << (stop - start) / float(CLK_TCK) << " seconds" << endl;
  cout << endl;
  return 0;
}


/***********************************************************
Sample 1 from CFSQP distribution
Problem 32 from Hock/Schittkowski 1981
***********************************************************/

/**********
class F1
First objective function
**********/
class F1 : public tmDifferentiableFn {
public:
  double Func(const std::vector<double>& x) {
    IncFuncCalls();
    return std::pow((x[0] + 3.e0 * x[1] + x[2]), 2.e0) + 
      4.e0*std::pow((x[0] - x[1]), 2.e0);
  };
  void Grad(const std::vector<double>& x, std::vector<double>& gradx) {
    IncGradCalls();
    double fa = 2.e0 * (x[0] + 3.e0 * x[1] + x[2]);
    double fb = 8.e0 * (x[0] - x[1]);
    gradx[0] = fa + fb;
    gradx[1] = fa * 3.e0 - fb;
    gradx[2] = fa;
  };
};


/**********
class F2
First constraint (nonlinear inequality)
**********/
class F2 : public tmDifferentiableFn {
public:
  double Func(const std::vector<double>& x) {
    IncFuncCalls();
    return std::pow(x[0], 3.e0) - 6.e0 * x[1] - 4.e0 * x[2] + 3.e0;
  };
  void Grad(const std::vector<double>& x, std::vector<double>& gradx) {
    IncGradCalls();
    gradx[0] = 3.e0 * x[0] * x[0];
    gradx[1] = -6.e0;
    gradx[2] = -4.e0;
  };
};


/**********
class F3
Second constraint (linear equality)
**********/
class F3 : public tmDifferentiableFn {
public:
  double Func(const std::vector<double>& x) {
    IncFuncCalls();
    return 1.e0 - x[0] - x[1] - x[2];
  };
  void Grad(const std::vector<double>&, std::vector<double>& gradx) {
    IncGradCalls();
    gradx[0] = gradx[1] = gradx[2] = -1.e0;
  };
};


/*****
Routine for testing using a CFSQP sample problem
*****/
template <class T>
int Test_cfsqp_sample()
{
  cout << "Testing with CFSQP sample problem #1." << endl;
  
  // Create and initialize the minimizer object
  T testMinimizer;
  testMinimizer.SetSize(3);
  
  // Set up the bounds for the problem
  std::vector<double> bl(3);
  bl.assign(3, 0.);
  
  std::vector<double> bu(3);
  bu.assign(3, 1.0e10);
  
  testMinimizer.SetBounds(bl, bu);
  
  // Create objective functions and constraints
  F1* objective = new F1;
  std::vector<tmDifferentiableFn*> constraints;
  std::vector<tmDifferentiableFn*> eq_constraints;
  std::vector<tmDifferentiableFn*> ineq_constraints;
  F2* constraint1 = new F2;
  F3* constraint2 = new F3;
  testMinimizer.SetObjective(objective);
  testMinimizer.AddNonlinearInequality(constraint1);
  testMinimizer.AddLinearEquality(constraint2);  
  constraints.push_back(constraint1);
  ineq_constraints.push_back(constraint1);
  constraints.push_back(constraint2);
  eq_constraints.push_back(constraint2);
  
   // Set up initial solution in member variable
  std::vector<double> x(3);
  x[0] = 0.1;
  x[1] = 0.7;
  x[2] = 0.2;
  
  // Run the calculation
  clock_t start = clock();
  int code = testMinimizer.Minimize(x);
  clock_t stop = clock();
  
  // Display the result
  cout.setf(ios_base::fixed);
  cout.precision(6);
  cout << "1 objective, 1 equality constraint, 1 inequality constraint" << endl;
  ReportVector("x", x);
  cout << "Result code = " << code << endl;
  ReportCalls("objective", objective);
  ReportCalls("constraint", constraints);
  constraints.push_back(objective);
  ReportCalls("total", constraints);
  ReportFeasibility(x, eq_constraints, ineq_constraints);
  cout << "time = " << (stop - start) / float(CLK_TCK) << " seconds" << endl;
  cout << endl;
  return 0;
}


/***********************************************************
Packing circles problem
***********************************************************/

/*
This code uses the constrained optimizer class to solve a classic nonlinear
constrained optimization problem, which is finding the optimum packing of N
nonoverlapping circles such that their centers fall within a unit square (or
equivalently, packing N nonoverlapping circles wholly within a unit square).
The problem is equivalent to that of maximizing the circle radius r over
variables x[i=1..N] and y[i=1..N] such that
|U[i] - U[j]| >= 2 * r. For N circles, there are N(N+1)/2 constraints.

Circle packing is a special case of the origami design problem, so this problem
gives a good test of how well the optimizer will work on origami.

The variables are:
  
u[0] = circle radius r
u[1], u[2] = x and y coordinates of the N=1 circle
...
u[2*N-1], u[2*N] = x and y coordinates of the Nth circle
  
We define two function objects: the objective function and a constraint
function that stores some information internally.
  
We can keep track of progress by putting a callback into the objective function
or, as we do here, just updating a counter each time it's called.
*/

/**********
class H1
Objective function
**********/
class H1 : public tmDifferentiableFn {
public:
  double Func(const std::vector<double>& u) {
    IncFuncCalls();
//    if (!(mFuncCalls % 10)) cout << "iteration " << mFuncCalls << endl;
    return -u[0];
  };
  void Grad(const std::vector<double>&, std::vector<double>& du) {
    IncGradCalls();
    du.assign(du.size(), 0);
    du[0] = -1.0;
  };
};


/**********
class H2
Constraint (nonlinear inequality)
Constructor parameters i and j are zero-based indices of the circles we're
comparing
**********/
class H2 : public tmDifferentiableFn {
private:
  std::size_t ix;    // indicies of the affected variables
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
public:
  H2(std::size_t i, std::size_t j) {
    ix = 2 * i + 1;
    iy = 2 * i + 2;
    jx = 2 * j + 1;
    jy = 2 * j + 2;
  };
  double Func(const std::vector<double>& u) {
    IncFuncCalls();
    return 2 * u[0] - std::sqrt(std::pow(u[ix] - u[jx], 2) + 
      std::pow(u[iy] - u[jy], 2));
  };
  void Grad(const std::vector<double>& u, std::vector<double>& du) {
    IncGradCalls();
    du.assign(du.size(), 0);
    du[0] = 2.0;
    double temp = std::sqrt(std::pow(u[ix] - u[jx], 2) + 
      std::pow(u[iy] - u[jy], 2));
    du[ix] = (u[jx] - u[ix]) / temp;
    du[jx] = - du[ix];
    du[iy] = (u[jy] - u[iy]) / temp;
    du[jy] = -du[iy];
  };
};


/**********
class H3
Constraint (nonlinear equality) for mirror symmetry about the vertical midline
of the square. Both H3 and H4 are required. Constructor parameters i and j are
zero-based indices of the circles we're comparing
**********/
class H3 : public tmDifferentiableFn {
private:
  std::size_t ix;    // indicies of the affected variables
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
public:
  H3(std::size_t i, std::size_t j) {
    ix = 2 * i + 1;
    iy = 2 * i + 2;
    jx = 2 * j + 1;
    jy = 2 * j + 2;
  };
  double Func(const std::vector<double>& u) {
    IncFuncCalls();
    return u[ix] + u[jx] - 1.0;
  };
  void Grad(const std::vector<double>&, std::vector<double>& du) {
    IncGradCalls();
    du.assign(du.size(), 0);
    du[ix] = 1.0;
    du[jx] = 1.0;
  };
};


/**********
class H4
Constraint (nonlinear equality) for mirror symmetry about the vertical midline
of the square. Both H3 and H4 are required. Constructor parameters i and j are
zero-based indices of the circles we're comparing
**********/
class H4 : public tmDifferentiableFn {
private:
  std::size_t ix;    // indicies of the affected variables
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
public:
  H4(std::size_t i, std::size_t j) {
    ix = 2 * i + 1;
    iy = 2 * i + 2;
    jx = 2 * j + 1;
    jy = 2 * j + 2;
  };
  double Func(const std::vector<double>& u) {
    IncFuncCalls();
    return u[iy] - u[jy];
  };
  void Grad(const std::vector<double>&, std::vector<double>& du) {
    IncGradCalls();
    du.assign(du.size(), 0);
    du[iy] = 1.0;
    du[jy] = -1.0;
  };
};


/*****
Routine for testing circle-packing.
*****/
template <class T>
int Test_circle_packing(std::size_t num_circles, bool report_vectors = true, 
  bool use_symmetry = false)
{
  // Report which problem we're solving
  cout << "Circle packing for " << int(num_circles) << " circles" << 
    (use_symmetry ? " with symmetry:" : ":") << endl;
  
  std::size_t nn = 2 * num_circles + 1;  // dimensionality of problem
  
  // Create and initialize the minimizer object
  T testMinimizer;
  testMinimizer.SetSize(nn);
  
  // Set up the bounds for the problem.
  std::vector<double> bl(nn, 0.);
  bl[0] = .001;  // Bump up the bound on circle radius to force strict positivity.
  std::vector<double> bu(nn, 1.);
  testMinimizer.SetBounds(bl, bu);
  
   // Set up initial solution in member variable with arbitrary values. We make
   // the initial scale (x[0]) small enough that it'll be feasible. For the
   // other variables, we'll use a poor-man's version of a pseudo- random
   // distribution so every call gets the same initial distribution of values,
   // and we'll start with some infeasible points; coordinate values will be
   // distributed over the range [-0.25, 1.25).
  std::vector<double> x(nn);
  x[0] = 0;
  for (std::size_t i = 1; i < nn; ++i) 
    x[i] = std::fmod(double(i) * std::sqrt(1.0e9), 1.5) - 0.25;
  
  // Add the objective function
  H1* objective = new H1;
  testMinimizer.SetObjective(objective);
  
  // Add constraints for every pair of circles
  std::vector<tmDifferentiableFn*> constraints;
  std::vector<tmDifferentiableFn*> ineq_constraints;
  std::vector<tmDifferentiableFn*> eq_constraints;
  for (std::size_t i = 0; i < num_circles; ++i)
    for (std::size_t j = i + 1; j < num_circles; ++j) {
      H2* constraint = new H2(i, j);
      constraints.push_back(constraint);
      ineq_constraints.push_back(constraint);
      testMinimizer.AddNonlinearInequality(constraint);
    }
  
  // If we've selected symmetry, we'll place book symmetry constraints
  // on adjacent pairs of circles.
  if (use_symmetry) {
    std::size_t num_pairs = num_circles / 2;
    for (std::size_t i = 0; i < num_pairs; ++i) {
      H3* constraint1 = new H3(2 * i, 2 * i + 1);
      testMinimizer.AddNonlinearEquality(constraint1);
      constraints.push_back(constraint1);
      eq_constraints.push_back(constraint1);
      H4* constraint2 = new H4(2 * i, 2 * i + 1);
      constraints.push_back(constraint2);
      eq_constraints.push_back(constraint2);
      testMinimizer.AddNonlinearEquality(constraint2);
    }
  }
  // Note where we started
  std::vector<double> x_in = x;

  // Run the calculate
  clock_t start = clock();
  int code = testMinimizer.Minimize(x);
  clock_t stop = clock();
  
  // Display the result
  cout.setf(ios_base::fixed);
  cout.precision(6);
  cout << "1 objective, " << eq_constraints.size() << 
    " equality constraints , " <<  ineq_constraints.size() << 
    " inequality constraints" << endl;
  if (report_vectors) ReportPairVector("x_in", x_in);
  if (report_vectors) ReportPairVector("x_out", x);
  cout << "Result code = " << code << endl;
  cout << "Radius is: " << x[0] << endl;
  ReportCalls("objective", objective);
  ReportCalls("constraint", constraints);
  constraints.push_back(objective);
  ReportCalls("total", constraints);
  ReportFeasibility(x, eq_constraints, ineq_constraints);
  cout << "time = " << (stop - start) / float(CLK_TCK) << " seconds" << endl;
  cout << endl;
  return 0;
}


/*****
Run all tests on the given optimizer
*****/
template <class T>
int Test_all()
{
  cout.setf(ios_base::fixed);
  cout.precision(6);
  Test_simple<T>();
  Test_cfsqp_sample<T>();
  Test_circle_packing<T>(10, false, false);
  Test_circle_packing<T>(10, false, true);
  Test_circle_packing<T>(20, false, false);
  Test_circle_packing<T>(20, false, true);
  return 0;
}

#endif // _TMNLCOTESTER_H_

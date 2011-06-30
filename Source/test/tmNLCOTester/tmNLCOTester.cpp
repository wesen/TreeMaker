/*******************************************************************************
File:         tmNLCOTester.cpp
Project:      TreeMaker 5.x
Purpose:      Test application for the nonlinear constr. optimization algorithms
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-05
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmNLCO.h"
#include "tmNLCOTester.h"

using namespace std;

/**********
Utility routines for reporting output
**********/

/*****
Write the value of a vector to the console
*****/
void ReportVector(const char* name, const vector<double>& v)
{
  cout << name << " = (";
  for (size_t i = 0; i < v.size(); ++i) {
    cout << v[i];
    if (i != v.size() - 1) cout << ", ";
  }
  cout << ")\n";
}

/*****
Write the value of a vector in which consecutive values make up ordered pairs.
*****/
void ReportPairVector(const char* name, const vector<double>& v)
{
  size_t n = v.size() / 2;
  for (size_t i = 0; i < n; ++i)
    cout << name << "[" << i << "] = " << v[2*i] << ", " << v[2*i+1] << ")\n";
}

/*****
Write the number of function and gradient calls for a single function.
*****/
void ReportCalls(const char* name, tmDifferentiableFn* f)
{
#if TM_PROFILE_OPTIMIZERS 
  cout << name << " calls = (" << f->GetNumFuncCalls() << ", " <<
    f->GetNumGradCalls() << ")\n";
#endif
}


/*****
Write the total number of function and gradient calls for a list of functions.
*****/
void ReportCalls(const char* name, const std::vector<tmDifferentiableFn*>& flist)
{
#if TM_PROFILE_OPTIMIZERS 
  size_t numFunc = 0;
  size_t numGrad = 0;
  for (size_t i = 0; i < flist.size(); ++i) {
    numFunc += flist[i]->GetNumFuncCalls();
    numGrad += flist[i]->GetNumGradCalls();
  }
  cout << name << " calls = (" << numFunc << ", " << numGrad << ")\n";
#endif
}
  

/*****
Write the feasibility for the given lists of equality and inequality constraints
*****/
void ReportFeasibility(const vector<double>& x, 
  const vector<tmDifferentiableFn*>& eq_list,
  const vector<tmDifferentiableFn*>& ineq_list)
{
  // Record worst-case violation of equality constraint for this x
  double eq_bad = 0.0;
  for (size_t i = 0; i < eq_list.size(); ++i) {
    double fval = fabs(eq_list[i]->Func(x));
    if (eq_bad < fval) eq_bad = fval;
  }
  
  // Record worst-case violation of inequality constraint for this x
  double ineq_bad = 0.0;
  for (size_t i = 0; i < ineq_list.size(); ++i) {
    double fval = ineq_list[i]->Func(x);
    if (eq_bad < fval) ineq_bad = fval;
  }  
  cout << "equality violation = " << eq_bad << 
    ", inequality violation = " << ineq_bad << endl;
}


/*****
Main program. Run all of our test routines.
*****/

int main()
{
#ifdef tmUSE_CFSQP
  cout << "\n\n";
  cout << "***************************************\n";
  cout << "Testing class tmNLCO_cfsqp.\n";
  cout << "***************************************\n";
  cout << "\n\n";

  Test_all<tmNLCO_cfsqp>();
#endif // tmUSE_CFSQP

#ifdef tmUSE_RFSQP
  cout << "\n\n";
  cout << "***************************************\n";
  cout << "Testing class tmNLCO_rfsqp.\n";
  cout << "***************************************\n";
  cout << "\n\n";
  
  Test_all<tmNLCO_rfsqp>();
#endif // tmUSE_RFSQP
  
#ifdef tmUSE_ALM
  cout << "\n\n";
  cout << "***************************************\n";
  cout << "Testing class tmNLCO_alm.\n";
  cout << "***************************************\n";
  cout << "\n\n";
  
  Test_all<tmNLCO_alm>();
#endif // tmUSE_ALM
  
#ifdef tmUSE_WNLIB
  cout << "\n\n";
  cout << "***************************************\n";
  cout << "Testing class tmNLCO_wnlib.\n";
  cout << "***************************************\n";
  cout << "\n\n";
  
  Test_all<tmNLCO_wnlib>();
#endif // tmUSE_WNLIB
  
  return 0;
}


/*******************************************************************************
File:         tmNewtonRaphsonTester.cpp
Project:      TreeMaker 5.x
Purpose:      Test application for the tmNewtonRaphson<T> classes
Author:       Robert J. Lang
Modified by:  
Created:      2005-09-27
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

// Standard libraries
#include <iostream>
#include <cmath>

using namespace std;

// My libraries
#include "tmNewtonRaphson.h"

// stream output for vectors, also works for tmMatrix
template <class T>
ostream& operator<<(ostream& os, const vector<T>& v);

template <class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  os << "(";
  for (size_t i = 0; i < v.size(); ++i) {
    os << v[i];
    if (i < v.size() - 1) os << ", ";
  }
  os << ")";
  return os;
}


/*****
Main program to test tmMatrix<T> class
*****/
int main_Matrix()
{
  cout << "tmMatrix:" << endl;
  
  // Test different constructors.
  tmMatrix<double> foo(3, 4);
  cout << "Default constructor: foo = " << foo << endl;

  tmMatrix<long> foo1(2, 10, 5);
  cout << "Ctor w/ size, and initial value: foo1 = " << foo1 << endl;

  // test assigment
  for (size_t i = 0; i < foo.GetRows(); ++i) 
    for (size_t j = 0; j < foo.GetCols(); ++j) foo[i][j] = i + 0.1 * j;
  cout << "foo = " << foo << endl;
  cout << "foo[0] = " << foo[0] << endl;
  cout << "foo[0][0] = " << foo[0][0] << endl;
  foo[2][2] = 3.14159;
  cout << "modified matrix = " << foo << endl;

  // test resizing
  foo.resize(4, 5);          // increase size
  cout << "new matrix = " << foo << endl;
  foo.resize(2, 2);          // decrease size
  cout << "new matrix = " << foo << endl;
  cout << endl;
  return 0;
}


/*****
Main program to test out LU decomposition code
*****/
int main_Ludecomp(void)
{    
  // Test the LU decomposition objects
  cout << "LU decomposition:" << endl;
    
  // set up a 4x4 matrix system and solve it.
  tmMatrix<double> a(4, 4);
  a[0][0] = 1;
  a[0][1] = 1;
  a[0][2] = 1;
  a[0][3] = 1;
  a[1][0] = 1;
  a[1][1] = 2;
  a[1][2] = 2;
  a[1][3] = 2;
  a[2][0] = 1;
  a[2][1] = 2;
  a[2][2] = 3;
  a[2][3] = 3;
  a[3][0] = 1;
  a[3][1] = 2;
  a[3][2] = 3;
  a[3][3] = 4;
  cout << "tmMatrix to be LU'd is a = " << endl << a << endl;
    
  vector<double> x(4);
  x[0] = 1;
  x[1] = 0.1;
  x[2] = 0.01;
  x[3] = 0.001;
  cout << "Target solution is " << x << endl;

  vector<double> b(4);
  b[0] = a[0][0] * x[0] + a[0][1] * x[1] + a[0][2] * x[2] + a[0][3] * x[3];
  b[1] = a[1][0] * x[0] + a[1][1] * x[1] + a[1][2] * x[2] + a[1][3] * x[3];
  b[2] = a[2][0] * x[0] + a[2][1] * x[1] + a[2][2] * x[2] + a[2][3] * x[3];
  b[3] = a[3][0] * x[0] + a[3][1] * x[1] + a[3][2] * x[2] + a[3][3] * x[3];
    
  cout << "a * x = " << b << endl;
    
  double d;        // parity of permutation
    
  tmNewtonRaphson<double> nrsolver;  // allocate solver, which initializes private variables.
  nrsolver.LUDecomposition(a, d);    // perform LU decomposition on ap
    
  cout << "LU matrix is " << endl << a << endl;
  cout << "row scaling vector is " << nrsolver.GetRowScaling() << endl;
  cout << "permutation vector is " << nrsolver.GetRowPermutation() << endl;
    
  nrsolver.LUBackSubstitution(a, b);  // back substitute to find solution
    
  cout << "solution is " << b << endl;
  cout << endl;
  return 0;
}


/**********
class NRTester
A class to test tmNewtonRaphson
**********/
class NRTester : public tmNewtonRaphson<float>
{
  protected:  
    void UserFn(const vector<float>& x, tmMatrix<float>& a, vector<float>& b);
};

/*****
void NRTester::UserFn(const vector<float>& x, tmMatrix<float>& a, vector<float>& b)
Return the function and its gradient
*****/
void NRTester::UserFn(const vector<float>& x, tmMatrix<float>& a, vector<float>& b)
{
  // b should return the function values
  b[0] = x[0] + x[1] + x[2] - 6;
  b[1] = pow(x[0], 2) + pow(x[1], 2) + pow(x[2], 2) - 14;
  b[2] = pow(x[0], 3) + pow(x[1], 3) + pow(x[2], 3) - 36;

  // a should return the gradients of the function values
  a[0][0] = 1;
  a[0][1] = 1;
  a[0][2] = 1;
  a[1][0] = 2 * x[0];
  a[1][1] = 2 * x[1];
  a[1][2] = 2 * x[2];
  a[2][0] = 3 * pow(x[0], 2);
  a[2][1] = 3 * pow(x[1], 2);
  a[2][2] = 3 * pow(x[2], 2);
}
  

/*****
Main program to test tmNewtonRaphson<T> class
*****/
int main_NewtonRaphson()
{    
  cout << "Newton-Raphson:" << endl;
    
  NRTester nr;
  vector<float> x(3);
  x[0] = 1.5f;
  x[1] = 2.4f;
  x[2] = 3.3f;

  cout << "x start = " << x << endl;

  nr.SolveEqns(10, x, 0.0001f, 0.0001f);

  cout << "x end = " << x << endl;
  cout << endl;
  return 0;    
}


/*****
Main program
*****/
int main()
{
  cout << "Hello World\n\n";
  main_Matrix();
  main_Ludecomp();
  main_NewtonRaphson();
  return 0;
}

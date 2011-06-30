/*******************************************************************************
File:         tmConstraintFns.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmConstraintFns class
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-03
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmConstraintFns.h"

using namespace std;

/*****************************************************************************
This code implements all of the different types of constraint functions used in
optimization.
******************************************************************************/

/*****************************************************************************
class OneVarFn
The OneVarFn implements a function of one variable  of the form
 a * u[ix] + b
that is used to fix the position of a tmNode.
Member variables are:
ix is the index of a tmNode component
a is its coefficient
b is a constant part
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
OneVarFn::OneVarFn(size_t aix, double aa, double ab)
{
  ix = aix;
  a = aa;
  b = ab;
}


/*****
Func - return the value of the constraint
*****/
double OneVarFn::Func(const vector<double>& u)
{
  IncFuncCalls();
  IncFuncCalls();
  return a * u[ix] + b;
}


/*****
Grad - return the gradient of the constraint
*****/
void OneVarFn::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = a;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class TwoVarFn
The TwoVarFn implements a function of one variable  of the form
 a * u[ix] + b * u[iy] + c
that is used to force two strains to be equal.
Member variables are:
ix is the index of one tmEdge strain
iy is the index of the other tmEdge strain
a is the coefficient of ix
b is the coefficient of iy
c is a constant part
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
TwoVarFn::TwoVarFn(size_t aix, double aa, size_t aiy, double ab, double ac)
{
  ix = aix;
  iy = aiy;
  a = aa;
  b = ab;
  c = ac;
}


/*****
Func - return the value of the constraint
*****/
double TwoVarFn::Func(const vector<double>& u)
{
  IncFuncCalls();
  return a * u[ix] + b * u[iy] + c;
}


/*****
Grad - return the gradient of the constraint
*****/
void TwoVarFn::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = a;
  du[iy] = b;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class PathFn1
The PathFn1 implements a function of four variables that is used to constrain
the separation between two movable nodes to be larger than the scaled distance
between them as measured along the tree.
member variables:
ix is the index of the x component of the position of the first movable tmNode
iy is the index of the y component of the position of the first movable tmNode
jx is the index of the x component of the position of the second movable tmNode
jy is the index of the y component of the position of the second movable tmNode
lij is the unscaled path length
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
PathFn1::PathFn1(size_t aix, size_t aiy, size_t ajx, size_t ajy, double alij)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  lij = alij;
}


/*****
Func - return the value of the constraint
*****/
double PathFn1::Func(const vector<double>& u)
{
  IncFuncCalls();
  return u[0] * lij - sqrt(pow(u[ix] - u[jx], 2) + pow(u[iy] - u[jy], 2));
}


/*****
Grad - return the gradient of the constraint
*****/
void PathFn1::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[0] = lij;
  double temp = sqrt(pow(u[ix] - u[jx], 2) + pow(u[iy] - u[jy], 2));
  if (temp != 0) 
    temp = 1. / temp;  // setting temp to 0 is better than NaN
  du[ix] = temp * (u[jx] - u[ix]);
  du[jx] = -du[ix];
  du[iy] = temp * (u[jy] - u[iy]);
  du[jy] = -du[iy];
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class PathFn2
The PathFn1 implements a function of two variables that is used to constrain
the separation between a movable tmNode and a fixed tmNode to a fixed value.
member variables:
ix is the index of the x component of the position of the movable tmNode
iy is the index of the y component of the position of the movable tmNode
vx is the value of the x component of the position of the fixed tmNode
vy is the value of the y component of the position of the fixed tmNode
lij is the unscaled path length
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
PathFn2::PathFn2(size_t aix, size_t aiy, double avx, double avy, double alij)
{
  ix = aix;
  iy = aiy;
  vx = avx;
  vy = avy;
  lij = alij;
}


/*****
Func - return the value of the constraint
*****/
double PathFn2::Func(const vector<double>& u)
{
  IncFuncCalls();
  return u[0] * lij - sqrt(pow(u[ix] - vx, 2) + pow(u[iy] - vy, 2));
}


/*****
Grad - return the gradient of the constraint
*****/
void PathFn2::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[0] = lij;
  double temp = sqrt(pow(u[ix] - vx, 2) + pow(u[iy] - vy, 2));
  if (temp != 0) 
    temp = 1. / temp;  // setting temp to 0 is better than NaN
  du[ix] = temp * (vx - u[ix]);
  du[iy] = temp * (vy - u[iy]);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class PathAngleFn1
The PathAngleFn1 implements a function of two variables
that is used to constrain a path between two movable nodes to lie at a fixed angle.
member variables:
ix is the index of the x component of the position of the first movable tmNode
iy is the index of the y component of the position of the first movable tmNode
jx is the index of the x component of the position of the second movable tmNode
jy is the index of the y component of the position of the second movable tmNode
ca is the cosine of the path angle
sa is the sine of the path angle
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
PathAngleFn1::PathAngleFn1(size_t aix, size_t aiy, size_t ajx, size_t ajy, double aa)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  ca = cos(aa * DEGREES);
  sa = sin(aa * DEGREES);
}


/*****
Func - return the value of the constraint
*****/
double PathAngleFn1::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (u[ix] - u[jx]) * sa + (u[jy] - u[iy]) * ca;
}


/*****
Grad - return the gradient of the constraint
*****/
void PathAngleFn1::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = sa;
  du[jx] = -sa;
  du[iy] = -ca;
  du[jy] = ca;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class PathAngleFn2
The PathAngleFn2 implements a function of two variables
that is used to constrain a path between a movable and a fixed tmNode to 
lie at a fixed angle.
member variables:
ix is the index of the x component of the position of the movable tmNode
iy is the index of the y component of the position of the movable tmNode
vx is the value of the x component of the position of the fixed tmNode
vy is the value of the y component of the position of the fixed tmNode
ca is the cosine of the path angle
sa is the sine of the path angle
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
PathAngleFn2::PathAngleFn2(size_t aix, size_t aiy, double avx, double avy, double aa)
{
  ix = aix;
  iy = aiy;
  vx = avx;
  vy = avy;
  ca = cos(aa * DEGREES);
  sa = sin(aa * DEGREES);
}


/*****
Func - return the value of the constraint
*****/
double PathAngleFn2::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (u[ix] - vx) * sa + (vy - u[iy]) * ca;
}


/*****
Grad - return the gradient of the constraint
*****/
void PathAngleFn2::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = sa;
  du[iy] = -ca;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class StrainPathFn1
The StrainPathFn1 implements a function of four variables
that is used to constrain the separation between two movable nodes when some
of the edges are stretchable.
member variables:
ix is the index of the x component of the position of the first movable tmNode
iy is the index of the y component of the position of the first movable tmNode
jx is the index of the x component of the position of the second movable tmNode
jy is the index of the y component of the position of the second movable tmNode
lfix is the (scaled) fixed part of the path length
lvar is the (scaled) variable part of the path length
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
StrainPathFn1::StrainPathFn1(
  size_t aix, size_t aiy, size_t ajx, size_t ajy, double alfix, double alvar)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  lfix = alfix;
  lvar = alvar;
}


/*****
Func - return the value of the constraint
*****/
double StrainPathFn1::Func(const vector<double>& u)
{
  IncFuncCalls();
  return lfix + u[0] * lvar - sqrt(pow(u[ix] - u[jx], 2) + pow(u[iy] - u[jy], 2));
}


/*****
Grad - return the gradient of the constraint
*****/
void StrainPathFn1::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[0] = lvar;
  double temp = sqrt(pow(u[ix] - u[jx], 2) + pow(u[iy] - u[jy], 2));
  if (temp != 0) 
    temp = 1. / temp;  // setting temp to 0 is better than NaN
  du[ix] = temp * (u[jx] - u[ix]);
  du[jx] = - du[ix];
  du[iy] = temp * (u[jy] - u[iy]);
  du[jy] = - du[iy];
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class StrainPathFn2
The StrainPathFn2 implements a function of two variables
that is used to constrain the separation between one movable and one fixed tmNode
when some of the edges are stretchable.
member variables:
ix is the index of the x component of the position of the movable tmNode
iy is the index of the y component of the position of the movable tmNode
vx is the value of the x component of the position of the fixed tmNode
vy is the value of the y component of the position of the fixed tmNode
lfix is the (scaled) fixed part of the path length
lvar is the (scaled) variable part of the path length
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
StrainPathFn2::StrainPathFn2(
  size_t aix, size_t aiy, double avx, double avy, double alfix, double alvar)
{
  ix = aix;
  iy = aiy;
  vx = avx;
  vy = avy;
  lfix = alfix;
  lvar = alvar;
}


/*****
Func - return the value of the constraint
*****/
double StrainPathFn2::Func(const vector<double>& u)
{
  IncFuncCalls();
  return lfix + u[0] * lvar - sqrt(pow(u[ix] - vx, 2) + pow(u[iy] - vy, 2));
}


/*****
Grad - return the gradient of the constraint
*****/
void StrainPathFn2::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[0] = lvar;
  double temp = sqrt(pow(u[ix] - vx, 2) + pow(u[iy] - vy, 2));
  if (temp != 0) 
    temp = 1. / temp;  // setting temp to 0 is better than NaN
  du[ix] = temp * (vx - u[ix]);
  du[iy] = temp * (vy - u[iy]);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class StrainPathFn3
The StrainPathFn3 implements a function of two variables
that is used to constrain the separation between two fixed nodes
when some of the edges are stretchable.
member variables:
ux is the value of the x component of the position of the first fixed tmNode
uy is the value of the y component of the position of the first fixed tmNode
vx is the value of the x component of the position of the second fixed tmNode
vy is the value of the y component of the position of the second fixed tmNode
lfix is the (scaled) fixed part of the path length
lvar is the (scaled) variable part of the path length
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
StrainPathFn3::StrainPathFn3(
  double aux, double auy, double avx, double avy, double alfix, double alvar)
{
  ux = aux;
  uy = auy;
  vx = avx;
  vy = avy;
  lfix = alfix;
  lvar = alvar;
}


/*****
Func - return the value of the constraint
*****/
double StrainPathFn3::Func(const vector<double>& u)
{
  IncFuncCalls();
  return lfix + u[0] * lvar - sqrt(pow(ux - vx, 2) + pow(uy - vy, 2));
}


/*****
Grad - return the gradient of the constraint
*****/
void StrainPathFn3::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[0] = lvar;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class StickToEdgeFn
The StickToEdgeFn implements a function that forces a single tmNode
to lie on any edge of the paper.
member variables:
ix is the index of the x component of the position of the tmNode
iy is the index of the y component of the position of the tmNode
w is the width of the paper
h is the height of the paper
******************************************************************************/

const double StickToEdge_Weight = 10.0;

/*****
Constructor - record constants and index into data array
*****/
StickToEdgeFn::StickToEdgeFn(size_t aix, size_t aiy, double aw, double ah)
{
  ix = aix;
  iy = aiy;
  w = aw;
  h = ah;
}


/*****
Func - return the value of the constraint
*****/
double StickToEdgeFn::Func(const vector<double>& u)
{
  IncFuncCalls();
  return StickToEdge_Weight * u[ix] * (u[ix] - w) * u[iy] * (u[iy] - h);
}


/*****
Grad - return the gradient of the constraint
*****/
void StickToEdgeFn::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = StickToEdge_Weight * (2 * u[ix] - w) * u[iy] * (u[iy] - h);
  du[iy] = StickToEdge_Weight * (2 * u[iy] - h) * u[ix] * (u[ix] - w);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class StickToLineFn
The StickToLineFn implements a function that forces a single tmNode to lie
on a given line.
member variables:
ix is the index of the x component of the position of the tmNode
iy is the index of the y component of the position of the tmNode
p is a point that the line goes through
ca is the cosine of the angle of the line
sa is the sine of the angle of the line
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
StickToLineFn::StickToLineFn(size_t aix, size_t aiy, tmPoint ap, double aa)
{
  ix = aix;
  iy = aiy;
  p = ap;
  aa *= DEGREES;
  sa = sin(aa);
  ca = cos(aa);
}


/*****
Func - return the value of the constraint
*****/
double StickToLineFn::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (-u[ix] + p.x) * sa + (u[iy] - p.y) * ca;
}


/*****
Grad - return the gradient of the constraint
*****/
void StickToLineFn::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = -sa;
  du[iy] = +ca;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class PairFn1A
class PairFn1B
The two classes PairFn1A and PairFn1B implement constraints that
force two movable nodes to lie symmetrically about a line, which is defined by
a point on the line and an angle in degrees. You must create one of each
type of FnConstraint to fully implement the mirror constraint condition.
member variables:
ix is the index of the x component of the position of the first movable tmNode
iy is the index of the y component of the position of the first movable tmNode
jx is the index of the x component of the position of the second movable tmNode
jy is the index of the y component of the position of the second movable tmNode
p is a point the line of symmetry goes through
ca is the cosine of the angle of the line of symmetry
sa is the sine of the angle of the line of symmetry
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
PairFn1A::PairFn1A(size_t aix, size_t aiy, size_t ajx, size_t ajy, tmPoint ap, double aa)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  p = ap;
  aa *= DEGREES;
  sa = sin(aa);
  ca = cos(aa);
}


/*****
Func - return the value of the constraint
*****/
double PairFn1A::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (u[ix] - u[jx]) * ca + (u[iy] - u[jy]) * sa;
}


/*****
Grad - return the gradient of the constraint
*****/
void PairFn1A::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = ca;
  du[iy] = sa;
  du[jx] = -ca;
  du[jy] = -sa;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Constructor - record constants and index into data array
*****/
PairFn1B::PairFn1B(size_t aix, size_t aiy, size_t ajx, size_t ajy, tmPoint ap, double aa)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  p = ap;
  aa *= DEGREES;
  sa = sin(aa);
  ca = cos(aa);
}


/*****
Func - return the value of the constraint
*****/
double PairFn1B::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (-u[ix] - u[jx] + 2 * p.x) * sa + (u[iy] + u[jy] - 2 * p.y) * ca;
}


/*****
Grad - return the gradient of the constraint
*****/
void PairFn1B::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = -sa;
  du[iy] = ca;
  du[jx] = -sa;
  du[jy] = ca;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class PairFn2A
class PairFn2B
The two classes PairFn2A and PairFn2B implement constraints that
force a fixed and a movable tmNode to lie symmetrically about a line, which 
is defined by a point on the line and an angle in degrees. You must create one of each
type of FnConstraint to fully implement the mirror constraint condition.
member variables:
ix is the index of the x component of the position of the movable tmNode
iy is the index of the y component of the position of the movable tmNode
vx is the value of the x component of the position of the fixed tmNode
vy is the value of the y component of the position of the fixed tmNode
p is a point the line of symmetry goes through
ca is the cosine of the angle of the line of symmetry
sa is the sine of the angle of the line of symmetry
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
PairFn2A::PairFn2A(size_t aix, size_t aiy, double avx, double avy, tmPoint ap, double aa)
{
  ix = aix;
  iy = aiy;
  vx = avx;
  vy = avy;
  p = ap;
  aa *= DEGREES;
  sa = sin(aa);
  ca = cos(aa);
}


/*****
Func - return the value of the constraint
*****/
double PairFn2A::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (u[ix] - vx) * ca + (u[iy] - vy) * sa;
}


/*****
Grad - return the gradient of the constraint
*****/
void PairFn2A::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = ca;
  du[iy] = sa;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Constructor - record constants and index into data array
*****/
PairFn2B::PairFn2B(size_t aix, size_t aiy, double avx, double avy, tmPoint ap, double aa)
{
  ix = aix;
  iy = aiy;
  vx = avx;
  vy = avy;
  p = ap;
  aa *= DEGREES;
  sa = sin(aa);
  ca = cos(aa);
}


/*****
Func - return the value of the constraint
*****/
double PairFn2B::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (-u[ix] - vx + 2 * p.x) * sa + (u[iy] + vy - 2 * p.y) * ca;
}


/*****
Grad - return the gradient of the constraint
*****/
void PairFn2B::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = -sa;
  du[iy] = ca;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class CollinearFn1 implements a constraint that forces three movable nodes
to be collinear.
member variables are:
ix is the index of the x component of the position of the first movable tmNode
iy is the index of the y component of the position of the first movable tmNode
jx is the index of the x component of the position of the second movable tmNode
jy is the index of the y component of the position of the second movable tmNode
kx is the index of the x component of the position of the third movable tmNode
ky is the index of the y component of the position of the third movable tmNode
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
CollinearFn1::CollinearFn1(size_t aix, size_t aiy, size_t ajx, size_t ajy, size_t akx, size_t aky)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  kx = akx;
  ky = aky;
}


/*****
Func - return the value of the constraint
*****/
double CollinearFn1::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (u[jy] - u[iy]) * (u[kx] - u[jx]) - (u[ky] - u[jy]) * (u[jx] - u[ix]);
}


/*****
Grad - return the gradient of the constraint
*****/
void CollinearFn1::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = (u[ky] - u[jy]);
  du[kx] = (u[jy] - u[iy]);
  du[jx] = -(du[ix] + du[kx]);
  du[iy] = (u[jx] - u[kx]);
  du[ky] = (u[ix] - u[jx]);
  du[jy] = -(du[iy] + du[ky]);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class CollinearFn2 implements a constraint that forces two movable nodes
and one fixed tmNode to be collinear.
member variables are:
ix is the index of the x component of the position of the first movable tmNode
iy is the index of the y component of the position of the first movable tmNode
jx is the index of the x component of the position of the second movable tmNode
jy is the index of the y component of the position of the second movable tmNode
wx is the value of the x component of the position of the fixed tmNode
wy is the value of the y component of the position of the fixed tmNode
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
CollinearFn2::CollinearFn2(size_t aix, size_t aiy, size_t ajx, size_t ajy, double awx, double awy)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  wx = awx;
  wy = awy;
}


/*****
Func - return the value of the constraint
*****/
double CollinearFn2::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (u[jy] - u[iy]) * (wx - u[jx]) - (wy - u[jy]) * (u[jx] - u[ix]);
}


/*****
Grad - return the gradient of the constraint
*****/
void CollinearFn2::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = (wy - u[jy]);
  du[jx] = -du[ix];
  du[iy] = (u[jx] - wx);
  du[jy] = -du[iy];
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class CollinearFn3 implements a constraint that forces one movable tmNode
and two fixed nodes to be collinear.
member variables are:
ix is the index of the x component of the position of the movable tmNode
iy is the index of the y component of the position of the movable tmNode
vx is the value of the x component of the position of the first fixed tmNode
vy is the value of the y component of the position of the first fixed tmNode
wx is the value of the x component of the position of the second fixed tmNode
wy is the value of the y component of the position of the second fixed tmNode
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
CollinearFn3::CollinearFn3(size_t aix, size_t aiy, double avx, double avy, double awx, double awy)
{
  ix = aix;
  iy = aiy;
  vx = avx;
  vy = avy;
  wx = awx;
  wy = awy;
}


/*****
Func - return the value of the constraint
*****/
double CollinearFn3::Func(const vector<double>& u)
{
  IncFuncCalls();
  return (vy - u[iy]) * (wx - vx) - (wy - vy) * (vx - u[ix]);
}


/*****
Grad - return the gradient of the constraint
*****/
void CollinearFn3::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = (wy - vy);
  du[iy] = (vx - wx);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class BoundaryFn
This class implements a constraint that forces a tmNode to lie on the same
side of a given line as another point.
member variables are:
ix is the index of the x component of the position of the tmNode
iy is the index of the y component of the position of the tmNode
p1 is one point on the reference line
p2 is the other point that defines the reference line
q is the reference point not lying on the line from p1 to p2
t is a normalized vector perpendicular to the line pointing away from q
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
BoundaryFn::BoundaryFn(size_t aix, size_t aiy, tmPoint ap1, tmPoint ap2, tmPoint aq)
{
  ix = aix;
  iy = aiy;
  p1 = ap1;
  p2 = ap2;
  q = aq;

  // t is a vector perpendicular to the line and pointing away from q

  t = Normalize(RotateCCW90(ap2 - ap1));
  if (Inner(t, aq - ap1) > 0) t *= -1;
}


/*****
Func - return the value of the constraint
*****/
double BoundaryFn::Func(const vector<double>& u)
{
  IncFuncCalls();
  return ((u[ix] - p1.x) * t.x + (u[iy] - p1.y) * t.y);
}


/*****
Grad - return the gradient of the constraint
*****/
void BoundaryFn::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = t.x;
  du[iy] = t.y;
}


#ifdef __MWERKS__
#pragma mark -
#endif


#define NEW_QUANTIZATION 1

// I am in the process of defining a new quantization function that should be
// more robust and not favor shrinking the function into oblivion.

#if NEW_QUANTIZATION

/*****************************************************************************
class QuantizeAngleFn1
The QuantizeAngleFn1 implements an equality constraint that forces the
angle of a line between 2 nodes to be quantized in angle.
member variables are:
ix is the index of the x component of the position of the first tmNode
iy is the index of the y component of the position of the first tmNode
jx is the index of the x component of the position of the second tmNode
jy is the index of the y component of the position of the second tmNode
n is the number of divisions of 180¡ by which the angle is quantized
oa is the offset in angle quantization
da is the angle step
wt is a weighting constant for the constraint
The functional form of the constraint is
F = wt * ¸(all ak) ((u[ix] - u[jx]) sin(ak) - (u[iy] - u[jy]) cos(ak))
where wt = 3^(n - 1) * /¸(i=1,n-1) sin(i)
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
QuantizeAngleFn1::QuantizeAngleFn1(size_t aix, size_t aiy, size_t ajx, size_t ajy,
      size_t an, double aoffset)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  n = an;
  oa = aoffset * DEGREES;
  da = 180.0 * DEGREES / an;
  
  // Compute the weighting factor
  
  wt = pow(2., int(n - 1));
}


/*****
Func - return the value of the constraint
*****/
double QuantizeAngleFn1::Func(const vector<double>& u)
{
  IncFuncCalls();
  double r2 = pow(u[ix] - u[jx], 2) + pow(u[iy] - u[jy], 2);
  
  double f1 = pow(r2, -0.5 * n);
  
  double f2 = 1;
  for (size_t k = 0; k < n; ++k) {
    double ak = (k * da - oa);
    double fk = (u[ix] - u[jx]) * sin(ak) - (u[iy] - u[jy]) * cos(ak);
    f2 *= fk;
  }  
  
  return wt * f1 * f2;
}


/*****
Grad - return the gradient of the constraint
*****/
void QuantizeAngleFn1::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  
  // compute contributions to function value
  
  double r2 = pow(u[ix] - u[jx], 2) + pow(u[iy] - u[jy], 2);
  
  double f1 = pow(r2, -0.5 * n);  

  double f2 = 1;
  for (size_t k = 0; k < n; ++k) {
    double ak = (k * da - oa);
    double fk = (u[ix] - u[jx]) * sin(ak) - (u[iy] - u[jy]) * cos(ak);
    f2 *= fk;
  }
  
  // compute terms and accumulate them in the gradient

  // contributions from gradient of f1
  
  double f3 = - f1 * n * f2 / r2;
  
  double temp = f3 * (u[ix] - u[jx]);
  du[ix] += temp;
  du[jx] -= temp;
  temp = f3 * (u[iy] - u[jy]);
  du[iy] += temp;
  du[jy] -= temp;

  // contributions from gradient of f2
  
  for (size_t l = 0; l < n; l++) {
    double dl = 1;
    for (size_t k = 0; k < n; ++k) {
      if (k == l) continue;
      double ak = (k * da - oa);
      dl *= (u[ix] - u[jx]) * sin(ak) - (u[iy] - u[jy]) * cos(ak);
    }
    double al = (l * da - oa);
    temp = f1 * sin(al) * dl;
    du[ix] += temp;
    du[jx] -= temp;
    temp = f1 * -cos(al) * dl;
    du[iy] += temp;
    du[jy] -= temp;    
  }
    
  for (size_t i = 0; i < du.size(); ++i) du[i] *= wt;
}


#ifdef __MWERKS__
#pragma mark -
#endif


#else


/*****************************************************************************
class QuantizeAngleFn1
The QuantizeAngleFn1 implements an equality constraint that forces the
angle of a line between 2 nodes to be quantized in angle.
member variables are:
ix is the index of the x component of the position of the first tmNode
iy is the index of the y component of the position of the first tmNode
jx is the index of the x component of the position of the second tmNode
jy is the index of the y component of the position of the second tmNode
n is the number of divisions of 180¡ by which the angle is quantized
oa is the offset in angle quantization
da is the angle step
wt is the weighting function for the constraint
The functional form of the constraint is
F = wt * ¸(all ak) ((u[ix] - u[jx]) sin(ak) - (u[iy] - u[jy]) cos(ak))
where wt = 3^(n - 1) * /¸(i=1,n-1) sin(i)
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
QuantizeAngleFn1::QuantizeAngleFn1(size_t aix, size_t aiy, size_t ajx, size_t ajy,
      size_t an, double aoffset)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  n = an;
  oa = aoffset * DEGREES;
  da = 180.0 * DEGREES / an;
  
  // Compute the weighting factor that comes with angle
  
  wt = pow(3., int(n - 1));
  for (size_t k = 1; k < n; ++k) {
    double ak = (oa + k * da);
    wt /= sin(ak);
  }
}


/*****
Func - return the value of the constraint
*****/
double QuantizeAngleFn1::Func(const vector<double>& u)
{
  IncFuncCalls();
  double fret = 1;
  for (size_t k = 0; k < n; ++k) {
    double ak = (oa + k * da);
    double fk = (u[ix] - u[jx]) * sin(ak) - (u[iy] - u[jy]) * cos(ak);
    fret *= fk;
  }
  return wt * fret;
}


/*****
Grad - return the gradient of the constraint
*****/
void QuantizeAngleFn1::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  for (size_t k = 0; k < n; ++k) {
    double ak = (oa + k * da);
    double dk = 1;
    for (size_t l = 0; l < n; l++) {
      if (k == l) continue;
      double al = (oa + l * da);
      double fl = (u[ix] - u[jx]) * sin(al) - (u[iy] - u[jy]) * cos(al);
      dk *= fl;
    }
    double temp = sin(ak) * dk;
    du[ix] += temp;
    du[jx] -= temp;
    temp = -cos(ak) * dk;
    du[iy] += temp;
    du[jy] -= temp;    
  }
  du *= wt;
}


#endif


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class QuantizeAngleFn2
The QuantizeAngleFn2 implements an equality constraint that forces the
angle of a line between a fixed and a movable nodes to be quantized in angle.
member variables are:
ix is the index of the x component of the position of the movable tmNode
iy is the index of the y component of the position of the movable tmNode
vx is the index of the x component of the position of the fixed tmNode
vy is the index of the y component of the position of the fixed tmNode
n is the number of divisions of 180¡ by which the angle is quantized
oa is the offset in angle quantization
da is the angular step
wt is the weighting function for the constraint
The functional form of the constraint is
F = wt * ¸(all ak) ((u[ix] - vx) sin(ak) - (u[iy] - vy) cos(ak))
where wt = 3^(n - 1) * /¸(i=1,n-1) sin(i)
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
QuantizeAngleFn2::QuantizeAngleFn2(size_t aix, size_t aiy, double avx, double avy,
      size_t an, double aoffset)
{
  ix = aix;
  iy = aiy;
  vx = avx;
  vy = avy;
  n = an;
  oa = aoffset * DEGREES;
  da = 180.0 * DEGREES / an;
  
  // Compute the weighting factor that comes with angle
  
  wt = pow(3., int(an - 1));
  for (size_t k = 1; k < n; ++k) {
    double ak = (oa + da);
    wt /= sin(ak);
  }
}


/*****
Func - return the value of the constraint
*****/
double QuantizeAngleFn2::Func(const vector<double>& u)
{
  IncFuncCalls();
  double fret = 1;
  for (size_t k = 0; k < n; ++k) {
    double ak = (oa + k * da);
    double fk = (u[ix] - vx) * sin(ak) - (u[iy] - vy) * cos(ak);
    fret *= fk;
  }
  return wt * fret;
}


/*****
Grad - return the gradient of the constraint
*****/
void QuantizeAngleFn2::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  for (size_t k = 0; k < n; ++k) {
    double ak = (oa + k * da);
    double dk = 1;
    for (size_t l = 0; l < n; l++) {
      if (k == l) continue;
      double al = (oa + l * da);
      double fl = (u[ix] - vx) * sin(al) - (u[iy] - vy) * cos(al);
      dk *= fl;
    }
    double temp = sin(ak) * dk;
    du[ix] += temp;
    temp = -cos(ak) * dk;
    du[iy] += temp;
  }
  for (size_t i = 0; i < du.size(); ++i) du[i] *= wt;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class LocalizeFn
The LocalizeFn implements a function of two variables
that is used to constrain the coordinates of a tmNode to lie within a circle
of a given radius.
Member variables are:
ix is the index of the x component of the position of the tmNode
iy is the index of the y component of the position of the tmNode
vx is the value of the x component of the reference point
vy is the value of the y component of the reference point
r is the radius of the circle
******************************************************************************/

const double k_Localize_Weight = 5;

/*****
Constructor - record constants and index into data array
*****/
LocalizeFn::LocalizeFn(size_t aix, size_t aiy, double avx, double avy,
  double ar)
{
  ix = aix;
  iy = aiy;
  vx = avx;
  vy = avy;
  r = ar;
}


/*****
Func - return the value of the constraint
*****/
double LocalizeFn::Func(const vector<double>& u)
{
  IncFuncCalls();
  return k_Localize_Weight * sqrt(pow(u[ix] - vx, 2) + pow(u[iy] - vy, 2)) - r;
}


/*****
Grad - return the gradient of the constraint
*****/
void LocalizeFn::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  double temp = sqrt(pow(u[ix] - vx, 2) + pow(u[iy] - vy, 2));
  if (temp != 0) 
    temp = 1. / temp;  // setting temp to 0 is better than NaN
  temp = k_Localize_Weight * temp;
  du[ix] = temp * (u[ix] - vx);
  du[iy] = temp * (u[iy] - vy);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class MultiStrainPathFn1
The MultiStrainPathFn1 implements a function of several variables
that is used to constrain the separation between two movable nodes when some
of the edges are stretchable with multiple strains.
Member variables are:
ix is the index of the x component of the position of the first movable tmNode
iy is the index of the y component of the position of the first movable tmNode
jx is the index of the x component of the position of the second movable tmNode
jy is the index of the y component of the position of the second movable tmNode
lfix is the fixed part of the path length
ni is the number of variable parts of the path length
vi is a vector of indices of the strains corresponding to variable parts
vf is a vector of coefficients of the strains
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
MultiStrainPathFn1::MultiStrainPathFn1(size_t aix, size_t aiy, size_t ajx, size_t ajy,
  double alfix, size_t ani, vector<size_t>& avi, vector<double>& avf)
{
  ix = aix;
  iy = aiy;
  jx = ajx;
  jy = ajy;
  lfix = alfix;
  ni = ani;
  vi.resize(ni);
  vi = avi;
  vf.resize(ni);
  vf = avf;
}


/*****
Func - return the value of the constraint
*****/
double MultiStrainPathFn1::Func(const vector<double>& u)
{
  IncFuncCalls();
  double pathlen = lfix;
  for (size_t i = 0; i < ni; ++i) pathlen += u[vi[i]] * vf[i];  
  return pathlen - sqrt(pow(u[ix] - u[jx], 2) + pow(u[iy] - u[jy], 2));
}


/*****
Grad - return the gradient of the constraint
*****/
void MultiStrainPathFn1::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  for (size_t i = 0; i < ni; ++i) du[vi[i]] = vf[i];
  double temp = sqrt(pow(u[ix] - u[jx], 2) + pow(u[iy] - u[jy], 2));
  if (temp != 0) 
    temp = 1. / temp;  // setting temp to 0 is better than NaN
  du[ix] = temp * (u[jx] - u[ix]);
  du[jx] = - du[ix];
  du[iy] = temp * (u[jy] - u[iy]);
  du[jy] = - du[iy];
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class MultiStrainPathFn2
The MultiStrainPathFn2 implements a function of two variables
that is used to constrain the separation between one movable and one fixed tmNode
when some of the edges are stretchable with multiple strains.
Member variables are:
ix is the index of the x component of the position of the movable tmNode
iy is the index of the y component of the position of the movable tmNode
vx is the value of the x component of the position of the fixed tmNode
vy is the value of the y component of the position of the fixed tmNode
lfix is the fixed part of the path length
ni is the number of variable parts of the path length
vi is a vector of indices of the strains corresponding to variable parts
vf is a vector of coefficients of the strains
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
MultiStrainPathFn2::MultiStrainPathFn2(size_t aix, size_t aiy, double avx, double avy,
  double alfix, size_t ani, vector<size_t>& avi, vector<double>& avf)
{
  ix = aix;
  iy = aiy;
  vx = avx;
  vy = avy;
  lfix = alfix;
  ni = ani;
  vi.resize(ni);
  vi = avi;
  vf.resize(ni);
  vf = avf;
}


/*****
Func - return the value of the constraint
*****/
double MultiStrainPathFn2::Func(const vector<double>& u)
{
  IncFuncCalls();
  double pathlen = lfix;
  for (size_t i = 0; i < ni; ++i) pathlen += u[vi[i]] * vf[i];  
  return pathlen - sqrt(pow(u[ix] - vx, 2) + pow(u[iy] - vy, 2));
}


/*****
Grad - return the gradient of the constraint
*****/
void MultiStrainPathFn2::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  for (size_t i = 0; i < ni; ++i) du[vi[i]] = vf[i];
  double temp = sqrt(pow(u[ix] - vx, 2) + pow(u[iy] - vy, 2));
  if (temp != 0) 
    temp = 1. / temp;  // setting temp to 0 is better than NaN
  du[ix] = temp * (vx - u[ix]);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class MultiStrainPathFn3
The MultiStrainPathFn3 implements a function of two variables
that is used to constrain the separation between two fixed nodes
when some of the edges are stretchable with multiple strains.
Member variables are:
ux is the value of the x component of the position of the first fixed tmNode
uy is the value of the y component of the position of the first fixed tmNode
vx is the value of the x component of the position of the second fixed tmNode
vy is the value of the y component of the position of the second fixed tmNode
lfix is the fixed part of the path length
ni is the number of variable parts of the path length
vi is a vector of indices of the strains corresponding to variable parts
vf is a vector of coefficients of the strains
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
MultiStrainPathFn3::MultiStrainPathFn3( double aux, double auy, double avx, double avy,
  double alfix, size_t ani, vector<size_t>& avi, vector<double>& avf)
{
  ux = aux;
  uy = auy;
  vx = avx;
  vy = avy;
  lfix = alfix;
  ni = ani;
  vi.resize(ni);
  vi = avi;
  vf.resize(ni);
  vf = avf;
}


/*****
Func - return the value of the constraint
*****/
double MultiStrainPathFn3::Func(const vector<double>& u)
{
  IncFuncCalls();
  double pathlen = lfix;
  for (size_t i = 0; i < ni; ++i) pathlen += u[vi[i]] * vf[i];  
  return pathlen - sqrt(pow(ux - vx, 2) + pow(uy - vy, 2));
}


/*****
Grad - return the gradient of the constraint
*****/
void MultiStrainPathFn3::Grad(const vector<double>&, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  for (size_t i = 0; i < ni; ++i) du[vi[i]] = vf[i];
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****************************************************************************
class CornerFn
The class CornerFn implements constraints that force a tmNode to lie on the
corner of the paper. You create two CornerFns for a tmNode to constrain both
x and y coordinates.
member variables:
ix is the index of the x (or y) component of the position of the tmNode
w is the width (or height) of the paper.
******************************************************************************/

/*****
Constructor - record constants and index into data array
*****/
CornerFn::CornerFn(size_t aix, double aw)
{
  ix = aix;
  w = aw;
}


/*****
Func - return the value of the constraint
*****/
double CornerFn::Func(const vector<double>& u)
{
  IncFuncCalls();
  return u[ix] * (u[ix] - w);
}


/*****
Grad - return the gradient of the constraint
*****/
void CornerFn::Grad(const vector<double>& u, vector<double>& du)
{
  IncGradCalls();
  du.assign(du.size(), 0.);
  du[ix] = 2 * u[ix] - w;
}

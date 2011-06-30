/*******************************************************************************
File:         tmConstraintFns.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmConstraintFns class
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-03
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCONSTRAINTFNS_H_
#define _TMCONSTRAINTFNS_H_

// my libraries

#include "tmNLCO.h"
#include "tmPoint.h"

/**********
class OneVarFn 
Used to constrain a single variable, e.g., a coordinate of a single node.
**********/
class OneVarFn : public tmDifferentiableFn {
public:
  OneVarFn(std::size_t aix, double aa, double ab);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  double a;
  double b;
};


/**********
class TwoVarFn
Used to constrain two variables, e.g., a pair of strains
**********/
class TwoVarFn : public tmDifferentiableFn {
public:
  TwoVarFn(std::size_t aix, double aa, std::size_t aiy, double ab, double ac);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  double a;
  double b;
  double c;
};


/**********
class PathFn1
Used to constrain distances between two nodes when the scale is flexible.
**********/
class PathFn1 : public tmDifferentiableFn {
public:
  PathFn1(std::size_t aix, std::size_t aiy, std::size_t ajx, std::size_t ajy, double alij);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
  double lij;
};


/**********
class PathFn2
Used to fix the distance between a movable node and a fixed node when the 
scale is flexible.
**********/
class PathFn2 : public tmDifferentiableFn {
public:
  PathFn2(std::size_t aix, std::size_t aiy, double avx, double avy, double alij);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  double vx;
  double vy;
  double lij;
};


/**********
class PathAngleFn1
Used to fix the angle of a path between two movable nodes to a particular value.
**********/
class PathAngleFn1 : public tmDifferentiableFn {
public:
  PathAngleFn1(std::size_t aix, std::size_t aiy, std::size_t ajx, std::size_t ajy, double aa);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
  double ca;
  double sa;
};


/**********
class PathAngleFn2
Used to fix the angle of a path between a movable and a fixed node to a 
particular value.
**********/
class PathAngleFn2 : public tmDifferentiableFn {
public:
  PathAngleFn2(std::size_t aix, std::size_t aiy, double avx, double avy, double aa);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);  
private:
  std::size_t ix;
  std::size_t iy;
  double vx;
  double vy;
  double ca;
  double sa;
};


/**********
class StrainPathFn1
Used to constrain distances between two movable nodes when some 
of the edges may be stretchable.
**********/
class StrainPathFn1 : public tmDifferentiableFn {
public:
  StrainPathFn1(std::size_t aix, std::size_t aiy, std::size_t ajx, std::size_t ajy, double alfix, double alvar);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
  double lfix;
  double lvar;
};


/**********
class StrainPathFn2
Used to constrain distances between a movable node
and a fixed node when some of the edges may be stretchable.
**********/
class StrainPathFn2 : public tmDifferentiableFn {
public:
  StrainPathFn2(std::size_t aix, std::size_t aiy, double avx, double avy, double alfix, double alvar);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);  
private:
  std::size_t ix;
  std::size_t iy;
  double vx;
  double vy;
  double lfix;
  double lvar;
};


/**********
class StrainPathFn3
Used to constrain distances between two fixed nodes when some of the edges 
may be stretchable.
**********/
class StrainPathFn3 : public tmDifferentiableFn {
public:
  StrainPathFn3(double aux, double auy, double avx, double avy, double alfix, double alvar);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);    
private:
  double ux;
  double uy;
  double vx;
  double vy;
  double lfix;
  double lvar;
};


/**********
class StickToEdgeFn
Used to constrain a coordinate to lie on the edge of the paper.
**********/
class StickToEdgeFn : public tmDifferentiableFn {
public:
  StickToEdgeFn(std::size_t aix, std::size_t aiy, double aw, double ah);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  double w;
  double h;
};


/**********
class StickToLineFn
Used to constrain a node to lie on a line (e.du., a line of symmetry). 
The line is defined by a point on the line and the angle of the line.
**********/
class StickToLineFn : public tmDifferentiableFn {
public:
  StickToLineFn(std::size_t aix, std::size_t aiy, tmPoint ap, double aa);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  tmPoint p;
  double sa;
  double ca;
};


/*
class PairFn1A and PairFn1B are used to constrain two movable nodes to be 
mirror-symmetric about a line (e.g., a line of symmetry). The line is 
defined by a point on the line and the angle of the line. Note that you 
must create BOTH a PairFn1A and a PairFn1B to fully constrain the
two points.
*/

/**********
class PairFn1A
Used to constraint two movable nodes to be mirror-symmetric.
**********/
class PairFn1A : public tmDifferentiableFn {
public:
  PairFn1A(std::size_t aix, std::size_t aiy, std::size_t ajx, std::size_t ajy, tmPoint ap, double aa);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
  tmPoint p;
  double sa;
  double ca;
};


/**********
class PairFn1B
Used to constraint two movable nodes to be mirror-symmetric.
**********/
class PairFn1B : public tmDifferentiableFn {
public:
  PairFn1B(std::size_t aix, std::size_t aiy, std::size_t ajx, std::size_t ajy, tmPoint ap, double aa);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
  tmPoint p;
  double sa;
  double ca;
};


/*
class PairFn2A and PairFn2B are used to constrain a fixed and a movable node to be 
mirror-symmetric about a line (e.g., a line of symmetry). The line is 
defined by a point on the line and the angle of the line. Note that you
must create BOTH a PairFn2A and a PairFn2B to fully constrain the two points.
*/

/**********
class PairFn2A
Used to constrain a fixed and a movable node to be mirror-symmetric.
**********/
class PairFn2A : public tmDifferentiableFn {
public:
  PairFn2A(std::size_t aix, std::size_t aiy, double avx, double avy, tmPoint ap, double aa);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  double vx;
  double vy;
  tmPoint p;
  double sa;
  double ca;
};


/**********
class PairFn2B
Used to constrain a fixed and a movable node to be mirror-symmetric.
**********/
class PairFn2B : public tmDifferentiableFn {
public:
  PairFn2B(std::size_t aix, std::size_t aiy, double avx, double avy, tmPoint ap, double aa);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  double vx;
  double vy;
  tmPoint p;
  double sa;
  double ca;
};


/*
classes CollinearFn1, CollinearFn2, and CollinearFn3 are used to constrain three
nodes to lie on the same line.
class CollinearFn1 is used when 3 nodes are variable.
CollinearFn2 is used when 2 nodes are variable.
CollinearFn3 is used when 1 node is variable.
*/

/**********
class CollinearFn1
Used to constrain 3 nodes to lie on the same line when 3 nodes are variable.
**********/
class CollinearFn1 : public tmDifferentiableFn {
public:
  CollinearFn1(std::size_t aix, std::size_t aiy, std::size_t ajx, std::size_t ajy, std::size_t akx, std::size_t aky);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);  
private:
  std::size_t ix;
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
  std::size_t kx;
  std::size_t ky;
};


/**********
class CollinearFn2
Used to constrain 3 nodes to lie on the same line when 2 nodes are variable.
**********/
class CollinearFn2 : public tmDifferentiableFn {
public:
  CollinearFn2(std::size_t aix, std::size_t aiy, std::size_t ajx, std::size_t ajy, double awx, double awy);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);  
private:
  std::size_t ix;
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
  double wx;
  double wy;
};


/**********
class CollinearFn3
Used to constrain 3 nodes to lie on the same line when 1 node is variable.
**********/
class CollinearFn3 : public tmDifferentiableFn {
public:
  CollinearFn3(std::size_t aix, std::size_t aiy, double avx, double avy, double awx, double awy);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  double vx;
  double vy;
  double wx;
  double wy;
};


/**********
class BoundaryFn
Used to force a point to lie on the same side of the line through
points p1 and p2 as point q is on.
**********/
class BoundaryFn : public tmDifferentiableFn {
public:
  BoundaryFn(std::size_t aix, std::size_t aiy, tmPoint ap1, tmPoint ap2, tmPoint aq);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);  
private:
  std::size_t ix;
  std::size_t iy;
  tmPoint p1;
  tmPoint p2;
  tmPoint q;
  tmPoint t;
};


/**********
class QuantizeAngleFn1
Used to constrain the angle between two nodes to a quantized value.
**********/
class QuantizeAngleFn1 : public tmDifferentiableFn {
public:
  QuantizeAngleFn1(std::size_t aix, std::size_t aiy, std::size_t ajx, std::size_t ajy,
    std::size_t an, double aoffset);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
  std::size_t n;
  double da;
  double oa;
  double wt;
};


/**********
class QuantizeAngleFn2
Used to constrain the angle between a moving and a nonmoving node to a quantized value.
**********/
class QuantizeAngleFn2 : public tmDifferentiableFn {
public:
  QuantizeAngleFn2(std::size_t aix, std::size_t aiy, double avx, double avy,
    std::size_t an, double aoffset);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  double vx;
  double vy;
  std::size_t n;
  double da;
  double oa;
  double wt;
};


/**********
class LocalizeFn
Used to constrain the coordinates of a node to lie within a circle of a given radius.
**********/
class LocalizeFn : public tmDifferentiableFn {
public:
  LocalizeFn(std::size_t aix, std::size_t aiy, double avx, double avy, double ar);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  double vx;
  double vy;
  double r;
};


/**********
class MultiStrainPathFn1
Used to constrain distances between two movable nodes when some of the edges 
may be stretchable.
**********/
class MultiStrainPathFn1 : public tmDifferentiableFn {
public:
  MultiStrainPathFn1(std::size_t aix, std::size_t aiy, std::size_t ajx, std::size_t ajy, double alfix,
    std::size_t ani, std::vector<std::size_t>& avi, std::vector<double>& avf);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  std::size_t iy;
  std::size_t jx;
  std::size_t jy;
  double lfix;
  std::size_t ni;
  std::vector<std::size_t> vi;
  std::vector<double> vf;
};


/**********
class MultiStrainPathFn2
Used to constrain distances between a movable node and a fixed node when some 
of the edges may be stretchable.
**********/
class MultiStrainPathFn2 : public tmDifferentiableFn {
public:
  MultiStrainPathFn2(std::size_t aix, std::size_t aiy, double avx, double avy, double alfix,
    std::size_t ani, std::vector<std::size_t>& avi, std::vector<double>& avf);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);  
private:
  std::size_t ix;
  std::size_t iy;
  double vx;
  double vy;
  double lfix;
  std::size_t ni;
  std::vector<std::size_t> vi;
  std::vector<double> vf;
};


/**********
class MultiStrainPathFn3
Used to constrain distances between two fixed nodes when some of the edges 
may be stretchable.
**********/
class MultiStrainPathFn3 : public tmDifferentiableFn {
public:
  MultiStrainPathFn3(double aux, double auy, double avx, double avy, double alfix,
    std::size_t ani, std::vector<std::size_t>& avi, std::vector<double>& avf);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  double ux;
  double uy;
  double vx;
  double vy;
  double lfix;
  std::size_t ni;
  std::vector<std::size_t> vi;
  std::vector<double> vf;
};


/**********
class CornerFn
Used (in pairs) to constrain nodes to lie on any corner of the paper.
**********/
class CornerFn : public tmDifferentiableFn {
public:
  CornerFn(std::size_t aix, double aw);
  double Func(const std::vector<double>& u);
  void Grad(const std::vector<double>& u, std::vector<double>& du);
private:
  std::size_t ix;
  double w;
};

#endif // _TMCONSTRAINTFNS_H_

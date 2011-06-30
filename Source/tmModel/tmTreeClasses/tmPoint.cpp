/*******************************************************************************
File:         tmPoint.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for class tmPoint, floating-point cartesian points
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmPoint.h"
#include "tmPart.h"
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

/**********
class tmPoint
A Point class based on 2 tmFloat values
**********/

/*****
Rotate by an arbitrary angle [radians] in the counterclockwise direction.
*****/
tmPoint RotateCCW(const tmPoint& p, const tmFloat& a)
{
  const tmFloat c = cos(a);
  const tmFloat s = sin(a);
  return tmPoint(c * p.x - s * p.y, c * p.y + s * p.x);
}


/*****
Rotate by an arbitrary angle [radians] in the clockwise direction.
*****/
tmPoint RotateCW(const tmPoint& p, const tmFloat& a)
{
  const tmFloat c = cos(a);
  const tmFloat s = sin(a);
  return tmPoint(c * p.x + s * p.y, c * p.y - s * p.x);
}


/*****
Given one line defined by a point p and direction rp, and a second line defined
by a point q and direction rq, return the parameters tp and tq such that the
intersection of the two lines is given by p + tp * rp or q + tq * rq. The
direction vectors do not need to be normalized but they cannot be zero.
If the lines intersect, we place the values in tp and tq and return true. If
the two lines are parallel, we return false and leave tp and tq unchanged.
Note that if the lines are defined by two points, e.g., p1, p2, q1, q2,
you can find their intersection by calling 
GetLineIntersectionParm(p1, p2-p1, q1, q2-q1, tp and tq).
Furthermore, the line segments p1-p2 and q1-q2 intersect iff both tp and tq
lie strictly between 0 and 1 (to within roundoff error).
*****/
bool GetLineIntersectionParms(const tmPoint& p, const tmPoint& rp, 
  const tmPoint& q, const tmPoint& rq, tmFloat& tp, tmFloat& tq)
{
  const tmFloat EPS = sqrt(numeric_limits<tmFloat>::epsilon());
  tmPoint rrp = RotateCCW90(rp);
  const tmFloat rrpq = Inner(rrp, rq);
  if (fabs(rrpq) < EPS) return false;  // test for parallelity
  tmPoint rqp = RotateCCW90(q - p);
  tp = Inner(rqp, rq) / rrpq;
  tq = Inner(rqp, rp) / rrpq;
#ifdef TMDEBUG
  // Verify accuracy of the computed intersection point
  tmPoint ip = p + tp * rp;  // intersection pt on p-line
  tmPoint iq = q + tq * rq;  // intersection pt on q-line
  tmFloat dpq = Mag(ip - iq);  // distance; should be the same
  TMASSERT(dpq < EPS);
#endif // TMDEBUG
  return true;
}

  
/*****
Same as GetLineIntersectionParms(..), but only returns tp and saves a tiny bit
of calculation.
*****/
bool GetLineIntersectionParm(const tmPoint& p, const tmPoint& rp, 
  const tmPoint& q, const tmPoint& rq, tmFloat& tp)
{
  const tmFloat rrpq = Inner(RotateCCW90(rp), rq);
  if (rrpq == 0) return false;
  tp = Inner(RotateCCW90(q - p), rq) / rrpq;
  return true;
}

  
/*****
Intersection point of two lines, each defined by a point and a direction.
If the lines intersect, place the intersection point in pq and return true.
If the lines are parallel, return false and leave pq unchanged.
*****/    
bool GetLineIntersection(const tmPoint& p, const tmPoint& rp, 
  const tmPoint& q, const tmPoint& rq, tmPoint& pq)
{
  tmFloat rr;
  if (!GetLineIntersectionParm(p, rp, q, rq, rr)) return false;
  pq = p + rp * rr;
  return true;
}


/*****
Centroid of a triangle (center of mass)
*****/
tmPoint Centroid(const tmPoint& p1, const tmPoint& p2, const tmPoint& p3)
{
  return (p1 + p2 + p3) / 3.0;
}


/*****
Bisector intersection (center of inscribed circle) of a triangle.
Formula derived by me [RJL].
*****/
tmPoint Incenter(const tmPoint& p1, const tmPoint& p2, const tmPoint& p3)
{
  tmFloat l12 = Mag(p1 - p2);
  tmFloat l23 = Mag(p2 - p3);
  tmFloat l31 = Mag(p3 - p1);
  
  return ((l12 * p3) + (l23 * p1) + (l31 * p2)) / (l12 + l23 + l31);
}


/*****
Radius of inscribed circle of a triangle.
Formula given by Eric Weisstein's Mathworld.
*****/
tmFloat Inradius(const tmPoint& p1, const tmPoint& p2, const tmPoint& p3)
{
  tmFloat a = Mag(p1 - p2);
  tmFloat b = Mag(p2 - p3);
  tmFloat c = Mag(p3 - p1);
  
  return 0.5 * sqrt((b + c - a) * (c + a - b) * (a + b - c) / (a + b + c));
}


/*****
Return the angle of this vector. The angle lies in the range [-PI, PI).
*****/
//const tmFloat Angle(const tmPoint& p);


/*****
Return the angle of this vector. The angle lies in the range [amin,
amin+TWO_PI).
*****/
tmFloat Angle(const tmPoint& p, const tmFloat& amin)
{
  tmFloat a = Angle(p);
  const tmFloat amax = amin + TWO_PI;
  while (a < amin) a += TWO_PI;
  while (a >= amax) a -= TWO_PI;
  return a;
}


/*****
Return the change in angle from the segment p1-p2 to the segment p2-p3.
The angle lies in the range [-PI, PI).
If the 3 points are collinear, the angle is zero.
If the line veers to the left, the angle is positive.
If the line veers to the right, the angle is negative.
For a counterclockwise convex polygon, all consecutive triples of vertices 
will return nonnegative values.
*****/
tmFloat AngleChange(const tmPoint& p1, const tmPoint& p2, 
  const tmPoint& p3)
{
  tmFloat a = Angle(p3 - p2) - Angle(p2 - p1);
  if (a < -PI) return a + TWO_PI;
  if (a >= PI) return a - TWO_PI;
  return a;
}


/*****
Return a floating-point value, which is:
< 0 if the three points are ordered clockwise
== 0 if the three points are collinear
> 0 if the three points are ordered counterclockwise
*****/
tmFloat Orientation2D(const tmPoint& p1, const tmPoint& p2, 
  const tmPoint& p3)
{
  tmPoint p13 = p1 - p3;
  tmPoint p23 = p2 - p3;
  return (p13.x * p23.y - p13.y * p23.x);
}


/*****
Return true if the three points are ordered clockwise. Note that both
IsCW and IsCCW will return false if the points are collinear.
*****/
bool AreCW(const tmPoint& p1, const tmPoint& p2, const tmPoint& p3)
{
  return Orientation2D(p1, p2, p3) < 0;
}


/*****
return true if the three points are ordered counterclockwise. Note that both
IsCW and IsCCW will return false if the points are collinear.
*****/
bool AreCCW(const tmPoint& p1, const tmPoint& p2, const tmPoint& p3)
{
  return Orientation2D(p1, p2, p3) > 0;
}


/*****
return true if the three points are collinear.
*****/
bool AreCollinear(const tmPoint& p1, const tmPoint& p2, const tmPoint& p3)
{
  return Orientation2D(p1, p2, p3) == 0;
}


/*****
Return a unit vector at a given angle [radians]
*****/
tmPoint UnitVector(const tmFloat& a)
{
  return tmPoint(std::cos(a), std::sin(a));
}

  
/*****
True if two points (p1 and p2) lie on the same side of the line segment 
between q1 and q2
*****/
bool SameSide(const tmPoint& p1, const tmPoint& p2, 
  const tmPoint& q1, const tmPoint& q2)
{
  tmPoint dl = RotateCCW90(q2 - q1);
  return Inner(p1 - q1, dl) * Inner(p2 - q1, dl) > 0;
}


/*****
Given a point p on line segment p1-p2 and line segment q1-q2, this routine
constructs the line from p perpendicular to p1-p2 and finds the point q where
that line intersects the line through q1-q2. ProjectPtoQ returns true if q lies
between q1 and q2.
The revised version is a bit more numerically robust. Originally, q1 and q2
were supposed to be the projection of p1 and p2, respectively, but this new
version doesn't assume that and also uses DistTol() as the criterion for
between-ness.
*****/
bool ProjectPtoQ(const tmPoint& p1, const tmPoint& p2, const tmPoint& p, 
  const tmPoint& q1, const tmPoint& q2, tmPoint& q)
{
#if 0  // original version
  tmPoint dq = q2 - q1;
  tmPoint dp = p2 - p1;
  tmFloat u = Inner(dp, p - q1) / Inner(dp, dq);
  q = q1 + u * dq;
  return ((u >= -0.001) && (u <= 1.001));
#endif
#if 1  // revised version
  tmPoint rq = q2 - q1;
  tmFloat dq = Mag(rq);
  tmPoint up = Normalize(p2 - p1);
  tmFloat d = dq * Inner(up, p - q1) / Inner(up, rq);
  tmPoint uq = Normalize(rq);
  q = q1 + d * uq;
  return (d > -0.9 * tmPart::DistTol()) && (d < dq + 0.9 * tmPart::DistTol());
#endif
}


/*****
Given a point q and line segment p1-p2, this routine constructs the line from q
perpendicular to p1-p2 and finds the point p where that line intersects the
line through p1-p2. ProjectQtoP returns true if p lies between p1 and p2. Like
ProjectPtoQ, I've revised this to make it more robust against numerical
misalignments and to use DistTol() as the criterion for between-ness.
*****/
bool ProjectQtoP(const tmPoint& q, const tmPoint& p1, const tmPoint& p2, 
  tmPoint& p)
{
#if 0  // original version
  tmPoint dp = p2 - p1;
  tmFloat v = Inner(q - p1, dp) / Mag2(dp);
  p = p1 + v * dp;
  return ((v >= 0) && (v <= 1));
#endif
#if 1  // revised version
  tmPoint rp = p2 - p1;
  tmPoint up = Normalize(rp);
  tmFloat d = Inner(up, q - p1);
  tmFloat dp = Inner(up, rp);
  p = p1 + d * up;
  return (d > -0.9 * tmPart::DistTol()) && (d < dp + 0.9 * tmPart::DistTol());
#endif
}


/*****
Clip a line to lie within a rectangle. The line is parameterized as (p + t *
r). botLeft and topRight are the corners of the rectangle. q1 and q2 are
returned as the endpoints of the clipped segment. The point p MUST lie within
the paper, or BTWH. If the line does not pass through the rect or only touches
at one corner, return false; otherwise return true.
*****/
bool ClipLineToRect(const tmPoint& p, const tmPoint& rp, 
  const tmPoint& botLeft, const tmPoint& topRight,
  tmPoint& q1, tmPoint& q2)
{
  TMASSERT(Mag(rp) != 0);
  TMASSERT(p.x >= botLeft.x && p.x <= topRight.x && \
    p.y >= botLeft.y && p.y <= topRight.y);
  const tmFloat EPS = 1e-5;
  tmFloat t;
  std::vector<tmFloat> posParms, negParms;
  if (GetLineIntersectionParm(p, rp, botLeft, tmPoint(1, 0), t)) {
    if (t < EPS) negParms.push_back(t);
    if (t > -EPS) posParms.push_back(t);
  }
  if (GetLineIntersectionParm(p, rp, botLeft, tmPoint(0, 1), t)) {
    if (t < EPS) negParms.push_back(t);
    if (t > -EPS) posParms.push_back(t);
  }
  if (GetLineIntersectionParm(p, rp, topRight, tmPoint(1, 0), t)) {
    if (t < EPS) negParms.push_back(t);
    if (t > -EPS) posParms.push_back(t);
  }
  if (GetLineIntersectionParm(p, rp, topRight, tmPoint(0, 1), t)) {
    if (t < EPS) negParms.push_back(t);
    if (t > -EPS) posParms.push_back(t);
  }
  sort(negParms.begin(), negParms.end());
  sort(posParms.begin(), posParms.end());
  q1 = p + negParms.back() * rp;
  q2 = p + posParms.front() * rp;
  if (Mag(q1 - q2) < EPS) return false;
  else return true;
}


/*****
Return true if the polygon defined by the vector of tmPoints encloses the given
point. This is only defined for convex polygons. it works for both CW and CCW
polygons.
*****/
bool ConvexEncloses(const vector<tmPoint>& v, const tmPoint& p)
{
  size_t n = v.size();
  // Compute the centroid
  tmPoint ctrd(0, 0);
  for (size_t i = 0; i < n; ++i) ctrd += v[i];
  ctrd /= n;
  // check to see if the point lies on the same side of each segment as the
  // centroid.
  for (size_t i = 0; i < n; ++i) {
    tmPoint p1 = v[i];
    tmPoint p2 = v[(i + 1) % n];
    tmPoint q = RotateCCW90(p2 - p1);
    if (Inner(ctrd - p1, q) < 0) q *= -1;
    if (Inner(p - p1, q) < 0) return false;
  }
  return true;  
}

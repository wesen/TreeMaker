/*******************************************************************************
File:         tmPoint.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmPoint, floating-point cartesian points
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMPOINT_H_
#define _TMPOINT_H_

// Standard libraries
#include <cmath>
#include <vector>

// Common TreeMaker header
#include "tmHeader.h"

// Utility routines for min and max of a pair
template <class T>
T& min_val(T& t1, T& t2) {return t1 < t2 ? t1 : t2;};

template <class T>
T min_val(const T& t1, const T& t2) {return t1 < t2 ? t1 : t2;};

template <class T>
T& max_val(T& t1, T& t2) {return t1 > t2 ? t1 : t2;};

template <class T>
T max_val(const T& t1, const T& t2) {return t1 > t2 ? t1 : t2;};


/*****
Constants
*****/
const tmFloat PI = 3.1415926535897932385;
const tmFloat TWO_PI = 6.2831853071795864769;
const tmFloat RADIAN = 57.95779513082321;
const tmFloat DEGREES = 0.017453292519943296;


/**********
class tmPoint
A Point class based on floats
**********/

class tmPoint {
public: 
  tmFloat x;
  tmFloat y;
  
  tmPoint() : x(0), y(0) {};
  template <class Tx, class Ty>
  tmPoint(const Tx& ax, const Ty& ay) : x(tmFloat(ax)), y(tmFloat(ay)) {};
  
  const tmPoint operator()(const tmFloat& p) {
    return tmPoint(p, p);};
  
  tmPoint& operator+=(const tmPoint& fp) {
    x += fp.x; y += fp.y; return *this;};
  tmPoint& operator-=(const tmPoint& fp) {
    x -= fp.x; y -= fp.y; return *this;};
  tmPoint& operator*=(const tmPoint& fp) {
    x *= fp.x; y *= fp.y; return *this;};
  tmPoint& operator/=(const tmPoint& fp) {
    x /= fp.x; y /= fp.y; return *this;};
  
  tmPoint& operator=(const tmFloat& f) {
    x = f; y = f; return *this;};
  tmPoint& operator+=(const tmFloat& f) {
    x += f; y += f; return *this;};
  tmPoint& operator-=(const tmFloat& f) {
    x -= f; y -= f; return *this;};
  tmPoint& operator*=(const tmFloat& f) {
    x *= f; y *= f; return *this;};
  tmPoint& operator/=(const tmFloat& f) {
    x /= f; y /= f; return *this;};
  
  tmPoint operator+ (tmPoint fp) const {
    return tmPoint(x + fp.x, y + fp.y);};
  tmPoint operator- (tmPoint fp) const {
    return tmPoint(x - fp.x, y - fp.y);};
  tmPoint operator* (tmPoint fp) const {
    return tmPoint(x * fp.x, y * fp.y);};
  tmPoint operator/ (tmPoint fp) const {
    return tmPoint(x / fp.x, y / fp.y);};

  tmPoint operator+ (const tmFloat& f) const {
    return tmPoint(x + f, y + f);};
  tmPoint operator- (const tmFloat& f) const {
    return tmPoint(x - f, y - f);};
  tmPoint operator* (const tmFloat& f) const {
    return tmPoint(x * f, y * f);};
  tmPoint operator/ (const tmFloat& f) const {
    return tmPoint(x / f, y / f);};
  
  // Magnitude
  tmFloat Mag() const {
    return std::sqrt(std::pow(x, 2) + std::pow(y, 2));};
  
  // Normalize in place
  tmPoint& Normalize() {
    return *this /= Mag();};
};

// Arithmetic with scalars, scalar first
inline tmPoint operator+ (const tmFloat& f, const tmPoint& fp) {
  return fp + f;};
inline tmPoint operator- (const tmFloat& f, const tmPoint& fp) {
  return fp - f;};
inline tmPoint operator* (const tmFloat& f, const tmPoint& fp) {
  return fp * f;};
inline tmPoint operator/ (const tmFloat& f, const tmPoint& fp) {
  return fp / f;};
  
// Magnitude
inline tmFloat Mag(const tmPoint& p) {
  return std::sqrt(std::pow(p.x, 2) + std::pow(p.y, 2));};
  
// Magnitude squared
inline tmFloat Mag2(const tmPoint& p) {
  return (std::pow(p.x, 2) + std::pow(p.y, 2));};
  
// Inner product
inline tmFloat Inner(const tmPoint& p, const tmPoint& q) { 
    return p.x * q.x + p.y * q.y;};
  
// Normalize
inline tmPoint Normalize(const tmPoint& p) {
  return p / Mag(p);};
  
// 90-degree rotations [degrees]
inline tmPoint RotateCCW90(const tmPoint& p) {
  // Rotate 90 degrees counterclockwise
  return tmPoint(-p.y, p.x);};
inline tmPoint RotateCW90(const tmPoint& p) {
  // Rotate 90 degrees clockwise
  return tmPoint(p.y, -p.x);};
  
// Arbitrary rotation [radians]
tmPoint RotateCCW(const tmPoint& p, const tmFloat& a);
tmPoint RotateCW(const tmPoint& p, const tmFloat& a);
  
// Intersection of two lines, each defined by a point and a direction 
bool GetLineIntersectionParms(const tmPoint& p, const tmPoint& rp, 
  const tmPoint& q, const tmPoint& rq, tmFloat& tp, tmFloat& tq);
bool GetLineIntersectionParm(const tmPoint& p, const tmPoint& rp, 
  const tmPoint& q, const tmPoint& rq, tmFloat& tp);
bool GetLineIntersection(const tmPoint& p, const tmPoint& rp, 
  const tmPoint& q, const tmPoint& rq, tmPoint& pq);
  
// Centroid of a triangle
tmPoint Centroid(const tmPoint& p1, const tmPoint& p2, 
  const tmPoint& p3);
  
// Bisector intersection (center of inscribed circle) of a triangle
tmPoint Incenter(const tmPoint& p1, const tmPoint& p2, 
  const tmPoint& p3);
tmFloat Inradius(const tmPoint& p1, const tmPoint& p2, 
  const tmPoint& p3);

// Angle [radians]
inline tmFloat Angle(const tmPoint& p) {
  return std::atan2(p.y, p.x);};
tmFloat Angle(const tmPoint& p, const tmFloat& amin);
tmFloat AngleChange(const tmPoint& p1, const tmPoint& p2, 
  const tmPoint& p3);

// Rotational orientation of three points
tmFloat Orientation2D(const tmPoint& p1, const tmPoint& p2, 
  const tmPoint& p3);
bool AreCW(const tmPoint& p1, const tmPoint& p2, const tmPoint& p3);
bool AreCCW(const tmPoint& p1, const tmPoint& p2, const tmPoint& p3);
bool AreCollinear(const tmPoint& p1, const tmPoint& p2, const tmPoint& p3);
  
// Unit vector at given angle [radians]
tmPoint UnitVector(const tmFloat& a);
  
// Two points (p1 and p2) lie on the same side of the line segment between q1
// and q2
bool SameSide(const tmPoint& p1, const tmPoint& p2, 
  const tmPoint& q1, const tmPoint& q2);
  
// Strict parallelity 
inline bool AreParallel(const tmPoint& p, const tmPoint& q) {
  return Inner(p, RotateCCW90(q)) == 0;};
  
// Strict equality  
inline bool operator== (const tmPoint& p, const tmPoint& q) {
  return (p.x == q.x) && (p.y == q.y);};
  
// Strict inequality
inline bool operator!= (const tmPoint& p, const tmPoint& q) {
  return (p.x != q.x) || (p.y != q.y);};
  
// Projection between two line segments
bool ProjectPtoQ(const tmPoint& p1, const tmPoint& p2, const tmPoint& p, 
  const tmPoint& q1, const tmPoint& q2, tmPoint& q);
bool ProjectQtoP(const tmPoint& q, const tmPoint& p1, const tmPoint& p2, 
  tmPoint& p);

// Clip line to rectangle
bool ClipLineToRect(const tmPoint& p, const tmPoint& r, 
  const tmPoint& botLeft, const tmPoint& topRight,
  tmPoint& q1, tmPoint& q2);
  
// Polygon encloses a point
bool ConvexEncloses(const std::vector<tmPoint>& v, const tmPoint& p);

#endif // _TMPOINT_H_

/*******************************************************************************
File:         tmNLCO_wnlibStub.c
Project:      TreeMaker 5.x
Purpose:      Stub routines for unused wnlib routines
Author:       Robert J. Lang
Modified by:  
Created:      2004-03-03
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include <assert.h>

/**********
Stub routines
These routines are never called within TreeMaker, but some linkers can't figure
that out and complain at their absence.
**********/
double wn_machine_tolerance()
{
  assert(0);
  return 0;
}


void wn_scale_vect(double *vect,double scalar,int len)
{
  assert(0);
}


void wn_add_scaled_vect(double*x, double*y, double w, int t)
{
  assert(0);
}


void wn_multiply_vect_by_vect(double*x, double*y, int z)
{
  assert(0);
}


void wn_divide_vect_by_vect(double*x, double*y, int z)
{
  assert(0);
}

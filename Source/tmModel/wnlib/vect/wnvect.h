/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnvectH
#define wnvectH


void wn_print_vect(double vect[],int len);
void wn_enter_vect(double vect[],int len);
void wn_random_vect(double vect[],int len);
void wn_zero_vect(double vect[],int len);

void wn_make_vect(double **pvect,int len);
void wn_free_vect(double *vect,int len);

void wn_copy_vect(double to_vect[],double from_vect[],int len);

double wn_dot_vects(double *vect1,double *vect2,int len);
double wn_norm2_vect(double *vect,int len);
double wn_norm_vect(double *vect,int len);
void wn_unit_vect(double vect[],int len);
double wn_dist2_vect(double v1[],double v2[],int len);
double wn_dist_vect(double v1[],double v2[],int len);

void wn_add_scaled_vect(double *to_vect,double *from_vect,double scalar,
      int len);
void wn_add_vect_and_scaled_vect(double *out,
         double *v1,double *v2,double s2,
               int len);
void wn_add_scaled_vects
(
  double *out,
  double *v1,
  double s1,
  double *v2,
  double s2,
  int len
);
void wn_add_3_scaled_vects
(
  double *out,
  double *v1,
  double s1,
  double *v2,
  double s2,
  double *v3,
  double s3,
  int len
);
void wn_subtract_vect
(
  double *to_vect,
  double *from_vect,
  int len
);
void wn_scale_vect(double *vect,double scalar,int len);

void wn_multiply_vect_by_vect(double *v1, double *v2, int len);
void wn_divide_vect_by_vect(double *v1, double *v2, int len);

#endif


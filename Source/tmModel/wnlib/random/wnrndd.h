/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnrnddH
#define wnrnddH


#include "wnlib.h"


EXTERN bool wn_true_with_probability(double prob);

EXTERN int wn_random_int_between(int low,int high);
EXTERN double wn_random_double_between(double low,double high);
EXTERN double wn_flat_distribution(void);

EXTERN double wn_arbitrary_distribution
(
  double (*pinverse_integral_density_func)(double integrated_prob)
);
EXTERN double wn_normal_distribution(void);
EXTERN double wn_exponential_distribution(void);
EXTERN int wn_poisson_distribution(double mean);
EXTERN double wn_cauchy_distribution(void);


#endif


/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wncnjdnH
#define wncnjdnH


#include "wnlib.h"



/*
#define WN_CONJ_DIR_DBG_NONE        0
#define WN_CONJ_DIR_DBG_PASSES      1
#define WN_CONJ_DIR_DBG_LINESEARCH  2
#define WN_CONJ_DIR_DBG_ALL         3
*/

extern int wn_conj_direction_noise_debug;



EXTERN void wn_conj_direction_noise_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  double coord_mins[],
  double coord_maxes[],
  double initial_coord_x0s[],
  int initial_offset,
  int passed_num_vars,
  void (*passed_prepeat_function)(double *pmean,double *psdev,
          double vect[],int num_evals),
  int start_num_evals,
  int max_func_calls
);
EXTERN void wn_force_conj_direction_noise_stop(void);



#endif


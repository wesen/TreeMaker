/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/

#include "wnlib.h"

#include "wnrndd.h"



double wn_random_double_between(double low,double high)
{
  return((high-low)*wn_flat_distribution()+low);
}


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

#include "wnvect.h"



void wn_zero_vect(register double vect[],register int len)
{
  register int i;

  for(i=0;i<len;++i)
  {
    vect[i] = 0.0;
  }
}


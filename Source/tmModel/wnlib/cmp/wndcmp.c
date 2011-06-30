/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#include "wncmp.h"



int wn_doublecmp(double d1,double d2)
{
  if(d1 > d2)
  {
    return(1);
  }
  else if(d1 < d2)
  {
    return(-1);
  }
  else 
  {
    return(0);
  }
}



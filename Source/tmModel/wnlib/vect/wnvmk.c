/**********************************************************************

wn_make_vect(&vect,len)
wn_free_vect(vect,len)

**********************************************************************/
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
#include "wnmem.h"
#include "wnvect.h"


void wn_make_vect(double **pvect,int len)
{
  *pvect = (double *)wn_alloc(len*sizeof(double));

  wn_zero_vect(*pvect,len);
}


/*ARGSUSED*/ void wn_free_vect(double *vect,int len)
{
  wn_free((ptr)vect);
}




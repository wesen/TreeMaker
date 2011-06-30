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
#include "wnmemb.h"

#include "wnvect.h"



void wn_copy_vect(double to_vect[],double from_vect[],int len)
{
  wn_memcpy((ptr)to_vect,(ptr)from_vect,len*sizeof(double));
}



/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#include <string.h>

#include "wnlib.h"
#include "wnmem.h"

#include "wncpy.h"



/*
extern void *memcpy(void *,void *,unsigned int);
*/



void wn_memcpy(ptr out,ptr in,int size)
{
  memcpy(out,in,size);
}





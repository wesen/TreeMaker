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
#include "wnmemb.h"



/*
extern void *memset(void *,int,unsigned int);
extern int memcmp(void *,void *,unsigned int);
*/


void wn_memset(ptr out,char c,int n)
{
  (void)memset(out,(int)c,n);
}


/* return true iff all bytes in the block of memory are set to c */
bool wn_memtest(ptr mem, char c, int n)
{
  int i;
  char *pmem = (char *) mem;

  for (i = 0;  i < n;  ++i)
  {
    if (pmem[i] != c)
    {
      return FALSE;
    }
  }

  return TRUE;
} /* wn_memtest */


void wn_memzero(ptr out,int n)
{
  wn_memset(out,'\0',n);
}

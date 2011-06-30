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

#include "wnsll.h"



bool wn_sllempty(wn_sll list)
{
  return(list == NULL);
}


int wn_sllcount(register wn_sll list)
{
  register int ret;

  ret = 0;

  for(;list != NULL;list = list->next)
  {
    ++ret;
  }

  return(ret);
}




/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#include <stdio.h>

#include "wnlib.h"

#include "wnvect.h"



void wn_print_vect(double vect[],int len)
{
  int i;

  printf("[ ");

  for(i=0;i<len;i++)
  {
    printf("%lg ",vect[i]);
  }

  printf("]\n");
}



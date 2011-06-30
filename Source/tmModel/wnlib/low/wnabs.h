/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnabsH
#define wnabsH


#define wn_abs(_x)         (  ((_x)<0) ? (-(_x)) : (_x)  ) 

#define wn_sign(_x)        (  ((_x)>0) ? 1 : ( ((_x)==0) ? 0:(-1) )  ) 


#endif

/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnswapH
#define wnswapH


#define wn_swap(_a,_b,_type)\
{\
  _type _tmp;\
  \
  _tmp = (_a);\
  (_a) = (_b);\
  (_b) = _tmp;\
}


#endif

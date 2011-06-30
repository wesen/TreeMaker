/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnconsH
#define wnconsH



#ifdef local
#undef local
#endif
#define local static

#ifdef bool
#undef bool
#endif
#define bool int

#ifdef TRUE
#undef TRUE
#endif
#define TRUE    (1)

#ifdef FALSE
#undef FALSE
#endif
#define FALSE   (0)


typedef void *ptr;


#ifndef NULL
#  ifdef __cplusplus
#    define NULL (0)
#  else
#    define NULL ((void *)0)
#  endif
#endif



#endif



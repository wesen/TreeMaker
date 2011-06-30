/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnpolyH
#define wnpolyH



#define wn_eval_poly1(_x,_a0,_a1)  ((_a0)+(_x)*(_a1))
#define wn_eval_poly2(_x,_a0,_a1,_a2) ((_a0)+(_x)*wn_eval_poly1(_x,_a1,_a2))
#define wn_eval_poly3(_x,_a0,_a1,_a2,_a3) ((_a0)+(_x)*wn_eval_poly2(_x,_a1,_a2,_a3))
#define wn_eval_poly4(_x,_a0,_a1,_a2,_a3,_a4) ((_a0)+(_x)*wn_eval_poly3(_x,_a1,_a2,_a3,_a4))


double wn_eval_poly(double x,double coefs[],int len);

void wn_differentiate_poly(double out[],double in[],int len);
void wn_integrate_poly(double out[],double in[],int len);

void wn_mult_polys(double out[],double in1[],int len1,double in2[],int len2);




#endif


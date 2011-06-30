/**********************************************************************

wn_make_mat(&mat,len_i,len_j)
wn_free_mat(mat,len_i,len_j)

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

#include "wnmat.h"



void wn_make_mat(double ***pmat,int len_i,int len_j)
{
  int i;

  *pmat = (double **)wn_alloc(len_i*sizeof(double *));

  for(i=0;i<len_i;i++)
  {
    wn_make_vect(&((*pmat)[i]),len_j);
  }
}


void wn_free_mat(double **mat,int len_i,int len_j)
{
  int i;

  for(i=0;i<len_i;i++)
  {
    wn_free_vect(mat[i],len_j);
  }

  wn_free(mat);
}





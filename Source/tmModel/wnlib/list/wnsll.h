/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnsllH
#define wnsllH

#include "wnlib.h"


typedef struct wn_sll_struct *wn_sll;

struct wn_sll_struct
{
  ptr contents;
  wn_sll next;
};


void wn_mksll(wn_sll *pel);
void wn_sllins(wn_sll *plist,ptr e);
void wn_slleins(wn_sll *plist,wn_sll el);

void wn_freesll(wn_sll el);
void wn_freesll_list(wn_sll el);
bool wn_slldel(wn_sll *plist,ptr e);
void wn_slledel(wn_sll *pel);
void wn_slleunlink(wn_sll *pel);

void wn_sllrev(wn_sll *plist);

void wn_sllcpy(wn_sll *pout,wn_sll in);

void wn_sllins_sll(wn_sll *pout,wn_sll in);
void wn_sllcat(wn_sll *pout,wn_sll in);

void wn_sllend(wn_sll **ppend,wn_sll *plist);
void wn_slllast(wn_sll *plast,wn_sll list);

void wn_sllindex(ptr *pcontents,wn_sll list,int index);
void wn_slleindex(wn_sll *pel,wn_sll list,int index);
bool wn_sllget_index(int *pindex,wn_sll list,ptr e);

bool wn_sllempty(wn_sll list);
int wn_sllcount(wn_sll list);

void wn_randomize_sll(wn_sll *plist);


#endif


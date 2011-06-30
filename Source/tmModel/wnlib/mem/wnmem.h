/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnmemH
#define wnmemH


#include "wnlib.h"



#define WN_NO_FREE           0x1
#define WN_GENERAL_FREE      0x2


typedef struct wn_mem_block_struct *wn_mem_block;
typedef struct wn_memgp_struct *wn_memgp;

struct wn_memgp_struct
{
  int begin_magic;

  wn_memgp next, *plast;
  wn_memgp children, sub_next, *psub_last;

  wn_mem_block current_block;
  ptr block_ptr, block_end_ptr;  /* used for allocating memory.  In no_free
  **        ** groups, 8-byte aligned allocation is
  **        ** from block_ptr, which goes from start of
  **        ** block upward, other memory allocation is
  **        ** from block_end_ptr, which goes from top
  **        ** of block downward.  In general_free groups,
  **        ** all allocation is through block_ptr. */
  int block_mem_left;
  int block_size;
  long int mem_used;

  ptr pad_list, free_list;

  void (*pverify_group)(wn_memgp group),
       (*pfree_piece)(ptr p,wn_memgp group);
  ptr  (*palloc_piece)(int size,wn_memgp group);

  char *label;

  int end_magic;
};

struct wn_mem_block_struct
{
  wn_mem_block next;
  int size, leftover;
};
#define WN_MEM_BLOCK_MEMORY(mem_block) ((ptr) ((mem_block)+1))


#define GROUP_STACK_SIZE    100

typedef struct wn_gpstack_struct *wn_gpstack;
struct wn_gpstack_struct      /* kludge for now */
{
  int stack_ptr;
  wn_memgp *group_stack;
  wn_memgp current_group;
};


EXTERN void wn_print_gp_stack(void);

EXTERN wn_memgp wn_curgp(void);
EXTERN wn_memgp wn_defaultgp(void);
EXTERN void wn_gppush(wn_memgp group);
EXTERN void wn_gppop(void);
EXTERN void wn_gpmake(const char parms[]);
EXTERN void wn_gpmakef(int flags, int block_size);
EXTERN void wn_gpfree(void);

EXTERN ptr wn_zalloc(int size);
EXTERN ptr wn_alloc(int size);
EXTERN void wn_free(ptr p);
EXTERN void wn_realloc(ptr *pp,int old_size,int new_size);
EXTERN void wn_zrealloc(ptr *pp,int old_size,int new_size);

EXTERN void wn_get_current_gpstack(wn_gpstack *pstack);
EXTERN void wn_set_current_gpstack(wn_gpstack stack);

EXTERN ptr wn_system_alloc(int size);
EXTERN void wn_system_free(ptr mem,int size);

EXTERN void wn_gplabel(const char *label);
#define WN_GET_GPLABEL(group) ((group)->label ? (group)->label : "")
EXTERN void wn_gperrfpush(void (*pfunc)(int size));
EXTERN void wn_stack_fill(void);
EXTERN void wn_gp_trap_address(ptr address);
EXTERN void wn_allmem_verify(void);
EXTERN long int wn_mem_used(void);
EXTERN void wn_print_mem_used(void);
EXTERN long int wn_group_mem_used(wn_memgp group);
EXTERN long unsigned wn_total_memory_in_group(wn_memgp group);
EXTERN long unsigned wn_heapsize(void);
EXTERN void wn_print_group_mem_used(wn_memgp group);
EXTERN void wn_print_all_groups_mem_used(void);
EXTERN bool wn_mem_in_group(ptr p,wn_memgp group);

/*   note, if you call either of these 2 routines, do so before commencing any
** memory allocation. */
EXTERN void wn_gp_fill(void);
EXTERN void wn_gp_pad(void);

EXTERN void wn_memory_fragmentation_report(void);
EXTERN long int wn_amount_of_free_memory_in_group(wn_memgp group);
EXTERN void wn_print_composition_of_big_blocks_free_list(wn_memgp group);

#if defined(__cplusplus)
  extern "C" {
#endif
  /* note the compiler will insist the arg here is unsigned */

  /*   you'll have to ifdef the declaration of your substitute function
  ** if you want it to work on both 32 bit and sparc64 -ugh */
  
/*[RJL] put in the case for CW */
#ifdef __MWERKS__
    typedef void *(*wn_system_memory_alloc_func_type)(unsigned long);
#elif WN_SIZEOF_PTR == 8
/*
# if WN_SIZEOF_PTR == 8
*/
/*[\RJL]*/
    typedef void *(*wn_system_memory_alloc_func_type)(unsigned long int);
# else
    typedef void *(*wn_system_memory_alloc_func_type)(unsigned int);
# endif
  typedef void  (*wn_system_memory_free_func_type)(ptr);
#if defined(__cplusplus)
  }
#endif

/* note the routines passed to these must be of type extern "C" */
EXTERN void wn_set_system_memory_alloc_func(wn_system_memory_alloc_func_type);
EXTERN void wn_set_system_memory_free_func(wn_system_memory_free_func_type);

/* note this is only to be called at the end of the program, after
** all memory allocation is concluded */
EXTERN void wn_abandon_memory_leaks(bool abandon_default_group);

#endif

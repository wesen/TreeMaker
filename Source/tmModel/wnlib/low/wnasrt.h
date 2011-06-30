/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor, Bill Chapman

****************************************************************************/
#ifndef wnasrtH
#define wnasrtH


#include <stdio.h>
#include "wnlib.h"


#if defined(linux)
# define wn_assert_get_string(expr)  __STRING(expr)
#else
# if defined(__STDC__)
#   define wn_assert_get_string(expr)  #expr
# else
#   define wn_assert_get_string(expr)  "expr"
# endif
#endif


#if defined(linux)
# if defined(__cplusplus) ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#   define   wn_assert_function_name  __PRETTY_FUNCTION__
# else
#   if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#     define wn_assert_function_name  __func__
#   else
#     define wn_assert_function_name  NULL
#   endif
# endif
#else
# define     wn_assert_function_name  NULL
#endif


#define wn_assert(_cond) \
  ((void) ((_cond) ? (void)0 : wn_assert_routine_func_exp(__FILE__, \
  /**/  __LINE__, wn_assert_function_name, wn_assert_get_string(_cond))))

#define wn_assert_notreached()  wn_assert_notreached_routine_func(__FILE__, \
/**/          __LINE__, wn_assert_function_name)

#define wn_assert_warn(_cond) \
  ((void) ((_cond) ? (void) 0 : wn_assert_warn_routine_func_exp(__FILE__, \
  /**/  __LINE__, wn_assert_function_name, wn_assert_get_string(_cond))))

#define wn_assert_warn_notreached() \
  wn_assert_warn_notreached_r_func(__FILE__, __LINE__, \
  /**/            wn_assert_function_name)

#if defined(__cplusplus)
  extern "C" {
#endif
  void wn_set_assert_print
  (
    void (*passert_print)(const char string[])
  );
  void wn_default_assert_print(const char string[]);

  void wn_set_assert_crash
  (
    void (*passert_crash)(void)
  );
  void wn_default_assert_crash(void);

  void wn_assert_routine(const char file_name[],int line_num);

  void wn_assert_notreached_routine(const char file_name[],
  /**/              int line_num);

  void wn_assert_warn_routine(const char file_name[], int line_num);

  void wn_assert_warn_notreached_r(const char file_name[],int line_num);


  void wn_assert_routine_func_exp(const char file_name[], int line_num,
  /**/      const char *func_name, const char *exp_string);

  void wn_assert_notreached_routine_func(const char *file_name,
  /**/        int line_num, const char *func_name);

  void wn_assert_warn_routine_func_exp(const char file_name[],
  /**/  int line_num, const char *func_name, const char *exp_string);

  void wn_assert_warn_notreached_r_func(const char file_name[],
  /**/        int line_num, const char *func_name);

  void wn_get_assert_override_string(char *output_buffer,
  /**/        const char *file_name, int line_num);
#if defined(__cplusplus)
  }
#endif

#endif

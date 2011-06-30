/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wnportH
#define wnportH



#define WN_IHUGE (1000000000)

#define WN_FHUGE (1.0e+50)
#define WN_FTINY (1.0/WN_FHUGE)


#ifdef _LANGUAGE_C_PLUS_PLUS
#  ifndef __cplusplus
#    define __cplusplus
#  endif
#endif


/*
 * EXTERN is used to allow header files from ANSI C modules to be used by
 * modules compiled under C++
 */
#ifndef EXTERN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
#endif

/*
 * This is used to bracket code that may or may be in C or C++, forcing it
 * to have C linkage. For example, you may have a header file containing a
 * typedef for a pointer to a function. Put EXTERN_BEGIN and EXTERN_END
 * around the typedef to force it to be a pointer to a "C" function, whether
 * it is included in a C file or a C++ file.
 */
#ifdef __cplusplus
#define EXTERN_BEGIN extern "C" {
#define EXTERN_END };
#else
#define EXTERN_BEGIN
#define EXTERN_END
#endif


#if defined(sparc64)
# define WN_SIZEOF_PTR  8
# define WN_SIZEOF_LONG  8
# define WN_SIZEOF_INT  4
#else
# define WN_SIZEOF_PTR  4
# define WN_SIZEOF_LONG  4
# define WN_SIZEOF_INT  4
#endif

#endif /* wnportH */

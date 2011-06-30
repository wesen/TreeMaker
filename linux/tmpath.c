#include "tmVersion.h"
#include <stdio.h>
#include <string.h>


/*
  Tiny helper application for installing TreeMaker 5 for Linux
 */

#ifndef INSTALL_PREFIX
#define INSTALL_PREFIX "/usr/local"
#endif

int main(int argc, char **argv) {
  if (argc > 1) {
    if (! strcmp (argv [1], "-a"))
      puts (TM_APP_NAME_STR);
    else
      if (! strcmp (argv [1], "-nl"))
	puts (TM_APP_LONG_VERSION_NAME_STR);
      else
	if (! strcmp (argv [1], "-nvb"))
	  puts (TM_APP_BUILD_NAME_STR);
	else
	  if (! strcmp (argv [1], "-nab"))
	    puts (TM_APP_ABOUT_NAME_STR);
	  else
	    if (! strcmp (argv [1], "-b")) {
	      fputs (TM_APP_BUILD_NAME_STR, stdout);
#ifdef __WXDEBUG__
	      puts (" debug");
#elif defined (TMDEBUG)
	      puts (" devel");
#elif defined (TMPROFILE)
	      puts (" profile");
#else
	      putchar ('\n');
#endif
	    }
	    else
	      if (! strcmp (argv [1], "-p")) {
		puts (INSTALL_PREFIX);
	      }
  } else
    puts (TM_APP_NAME_STR);
  return 0;
}

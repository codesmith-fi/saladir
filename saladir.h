/* saladir.h

*/

#ifndef SALADIR_H_DEFINED
#define SALADIR_H_DEFINED

/* try guessing the target system */
#if defined(linux)
#define LINUX 
#define _TS_LINUX_
#define USECURSES
#define _HAS_VSNPRINTF_
#elif defined(__GO32__)
#define _TS_DOS_
#else
# Error: Please define your target system in "saladir.h"
#endif


/* debug for memory allocations */
#ifdef MSS
#include <mss.h>
#endif

/* lowlevel & system includes */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <stdarg.h>

#endif


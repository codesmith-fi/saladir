/* types.h
**
*/

#ifndef _TYPES_H_DEFINED
#define _TYPES_H_DEFINED

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!																					!
! NOTE: DEFINE LONT64B if your system has 64 bits wide longint	!
!																					!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#undef LONG64B

typedef int             sint;		/* standard int */

typedef double          real;

typedef unsigned char   int8u;
typedef signed char     int8s;	/* should probably not be used */
typedef unsigned short  int16u;
typedef signed short    int16s;

#ifdef LONG64B
typedef unsigned int    int32u;
typedef signed int      int32s;
#else
typedef unsigned long   int32u;
typedef signed long     int32s;
#endif

/* largest possible integer number */
typedef unsigned long   hugeint;


typedef int _FH_;

#endif

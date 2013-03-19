/* path.h */


#ifndef _PATHFIND_H_DEFINED
#define _PATHFIND_H_DEFINED

#include "saladir.h"
#include "types.h"

/* type which defines a simple direction */
typedef int8u Tdirection;

/* coordinate type */
typedef struct _coord
{
      int16s x;
      int16s y;
      int16u d;  /* distance, needed only while path evaluation */
} Tcoordinate;

/* one node for pathlist */
typedef struct _pathnode
{
      Tcoordinate c;
      struct _pathnode *next;
} Tpathnode;

typedef Tpathnode *Tpathlist;

/* one node fir direction */
typedef struct _dirnode
{
      Tdirection d;
      struct _dirnode *next;
} Tdirnode;

/* the final path will only be built of directions */
typedef Tdirnode *Tdirlist;


#endif


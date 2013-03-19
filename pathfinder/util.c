/* utils */

#include <stdlib.h>
#include <math.h>
#include "list.h"

/* get a direct line distance between two points in 2D-space */
unsigned int distance(Coord *p1, Coord *p2)
{
   unsigned int dist = 0;
   int dx, dy;
   float tmp;

   dx=p2->x - p1->x;
   dy=p2->y - p1->y;
   tmp = sqrt( (float)(dx*dx + dy*dy))*2.f;
//   tmp = (float)((dx*dx + dy*dy)/2);
   return (unsigned int)tmp;
//   dist = (unsigned int)(sqrt(dx*dx + dy*dy)*1.5);	
 
   return dist;
}

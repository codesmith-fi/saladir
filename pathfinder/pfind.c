/*


  */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "list.h"
#include "util.h"
#include "output.h"

#define SIZEX 40
#define SIZEY 20

#define T_WALL '#'
#define T_GRND '.'
#define T_START '*'
#define T_END	'X'

#define ABS(x)	(((x) >= 0) ? (x) : -(x))

char pathgrid[SIZEY+1][SIZEX+1];
char mapgrid[SIZEY+1][SIZEX+1];
unsigned char dirgrid[SIZEY+1][SIZEX+1];

int move_dx[]= { 0,-1, 0, 1,-1, 0, 1,-1, 0, 1 };
int move_dy[]= { 0, 1, 1, 1, 0, 0, 0,-1,-1,-1 };
int directions[]= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
int dir_invers[]= { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

void initmap(void)
{
   unsigned int x, y;
	
   for(y=0; y<SIZEY; y++) {
      for(x=0; x<SIZEX; x++) {
	 mapgrid[y][x]=T_GRND;
	 dirgrid[y][x]=0;
      }
   }

   for(x=0; x<SIZEX; x++) {
      mapgrid[0][x]=T_WALL;
      mapgrid[SIZEY-1][x]=T_WALL;
   }
   for(y=0; y<SIZEY; y++) {
      mapgrid[y][0]=T_WALL;
      mapgrid[y][SIZEX-1]=T_WALL;
   }


   /* draw a simple blockage */
   y=SIZEY/2 + 4;
   for(x=8; x<SIZEX-8; x++)
      mapgrid[y][x]=T_WALL;

   y=SIZEY/2 - 6;
   for(x=15; x<SIZEX-15; x++)
      mapgrid[y][x]=T_WALL;

   x=SIZEX/2;
   for(y=4; y<SIZEY-4; y++)
      mapgrid[y][x]=T_WALL;
}

void showmap(void)
{
   unsigned int x, y;

   my_gotoxy(0,0);

   for(y=0; y<SIZEY; y++) {
      for(x=0; x<SIZEX; x++) {
	 my_putch(mapgrid[y][x]);
//	 else
//	    my_putch('0'+dirgrid[y][x]);
      }
      my_printf("\n");
   }
   my_refresh();
}


void my_delay(int ms)
{
   clock_t start, finish;

   start = clock();
   finish = start + (ms * (CLOCKS_PER_SEC / 1000));

   while(start < finish)
      start = clock();
}

int main(void)
{
   Llist free1;
   Llist *active;

   clock_t s_tm, f_tm;

   Coord start, end, now, next;
   int curdist=0, i, cx=0, cy=0;
   int pathexists = 0;

   srand(time(0));

   start.x = 1 + (rand() % (SIZEX-2)); // SIZEX/2+4;
   start.y = 1 + (rand() % (SIZEY-2)); // SIZEY/2+6;
   start.d = 0;

   end.x = 1 + (rand() % (SIZEX-2));
   end.y = 1 + (rand() % (SIZEY-2));
   end.d = 0;


   if(console_init() < 0) {
      return 5;
   }


   /* init both freelists */
   list_init(&free1);

   initmap();
   dirgrid[start.y][start.x] = 5;

   mapgrid[start.y][start.x] = T_START;
   mapgrid[end.y][end.x] = T_END;

   showmap();

   /* put start position to the list */
   active = &free1;
   list_insert(active, &start);
   now = start;

   curdist = 0;	

   s_tm = clock();
	
   while( list_getbest(active, &end, &now)) {    
      for(i=1; i<10; i++) {
	 if(i==5) continue;

	 next = now;

	 /* diagonal movements should be lengthier */
//	 if(i==1 || i==3 || i==7 || i==9)
//	    next.d = now.d + 2;
//	 else
	    next.d = now.d+1;
	 cy = now.y + move_dy[i];
	 cx = now.x + move_dx[i];

	 if(mapgrid[cy][cx] != T_WALL && dirgrid[cy][cx] == 0) {
	    dirgrid[cy][cx] = dir_invers[i];
	    next.x += move_dx[i];
	    next.y += move_dy[i];
	    list_insert(active, &next);
	 }
      }
      showmap();      
      my_gotoxy(now.x, now.y);
      my_putch('?');
      list_dispmap(active);
      my_refresh();
      my_delay(50);

      if(now.x == end.x && now.y == end.y) {
	 pathexists++;
	 break;
      }
   }

   showmap();

   /* find the path by backtracking the direction map */
   while(!(now.x == start.x && now.y == start.y) && pathexists) {
      my_gotoxy(now.x, now.y);
      my_putch('*');

      cx = now.x;
      cy = now.y;
      now.x += move_dx[dirgrid[cy][cx]];
      now.y += move_dy[dirgrid[cy][cx]];
      my_refresh();
      my_delay(50);
   }



   f_tm=clock();
   my_gotoxy(0, 22);

   if(pathexists) {
/*
      my_printf("Search took %d moves, and %2.8f seconds.\n",
	  now.d,
	  (float)((f_tm - s_tm)/CLOCKS_PER_SEC));
*/
      my_printf("Path exits, reached in %d moves.", now.d);
   }
   else {
      my_printf("No path exits!");
   }

   my_getch();
   console_close();

   list_clear(&free1);

   return 0;
}

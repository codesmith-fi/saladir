/**************************************************************************
 * outworld.cc --                                                         *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 10.07.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 19.07.1998                                         *
 *                                                                        *
 **************************************************************************
 *                                                                        *
 * (C) 1997, 1998 by Erno Tuomainen.                                      *
 * All rights reserved.                                                   *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 *                                                                        *
 **************************************************************************
 * eventually intended to be a random outworld generator                  *
 **************************************************************************/

#include "saladir.h"

#include "output.h"
#include "types.h"
#include "caves.h"
#include "generate.h"
#include "tables.h"

#include "outworld.h"

bool outworld_dolakes(level_type *level, int8u percentage);

bool outworld_create(level_type *level, int16u size_x, int16u size_y)
{
   bool result=true;

   /* allocate and initialize level with TYPE_GRASS */
   initlevel(level, TYPE_GRASS, size_x, size_y);

   /* generate water, lakes */
//   outworld_dolakes(level, 10);

   srand(time(0));
   diamond_square(level, 0.47f);

   return result;
}


/******************************************************************
 *
 * Diamond - Square - Algorithm 
 *
 ******************************************************************/

#define DSIZE		256
#define DARRAYSIZE	(DSIZE+1)

float dterrain[(DARRAYSIZE+1) * (DARRAYSIZE+1)];

/* return random value from -1 .. 1 */
float smallrand(void)
{
   return ((float)rand())/(RAND_MAX/2) - 1;
}

float randnum (float min, float max)
{
   float x;
    
   x = (float)(rand() & 0x7fff) /
      (float)0x7fff;

   return (x * (max - min) + min);
} 

/*
 * fractRand is a useful interface to randnum.
 */
#define fractRand(v) randnum (-v, v)

void table_init(void)
{
   int x, y;


   for(y=0; y<DARRAYSIZE; y++) {
      for(x=0; x<DARRAYSIZE; x++) {
	 dterrain[y * DARRAYSIZE + x]=0;
      }
   }
	

}

void dsq_2_map(level_type *level)
{
   float val, max=0, min=999.0, avg=0;
   int x, y, count=0;

   float level_ground, level_hill, level_mount, level_water;

   for(y=0; y<DARRAYSIZE; y++) {
      for(x=0; x<DARRAYSIZE; x++) {
	 val = dterrain[y * DARRAYSIZE + x];

	 if(val>max)
	    max=val;
	 if(val<min)
	    min=val;
	 avg+=val;
	 count++;
      }
   }
   avg = avg / count;

   if(min < 0) {
      min = -1 * min;

      for(y=0; y<DARRAYSIZE; y++) {
	 for(x=0; x<DARRAYSIZE; x++) {
	    dterrain[y * DARRAYSIZE + x]+=min;
	 }
      }
      max+=min;
      avg+=min;
      min=0;
   }

/*
  level_water = 0.55f * level_ground;
  level_ground = 0.70f * avg;
  level_hill = 1.40f * level_ground;
  level_mount = 1.80f * level_ground;
*/	
   level_water = 0.20f * max;
   level_ground = 0.70f * avg ;
   level_hill = 1.30f * avg;
   level_mount = 0.80f * max;

   my_printf("Max = %3.4f\nMin = %3.4f\nAvg = %3.4f\n",
	  max,min,avg);

   my_printf("   Water level: %4.2f - %4.2f\n", min, level_ground);
   my_printf("  Ground level: %4.2f - %4.2f\n", level_ground, level_hill);
   my_printf("    Hill level: %4.2f - %4.2f\n", level_hill, level_mount);
   my_printf("Mountain level: %4.2f - %4.2f\n", level_mount, max);

   my_printf("Creating the map... (%d * %d)\n", level->sizex, level->sizey);

   getch();

   for(y=0; y<DARRAYSIZE; y++) {
      if(y>(level->sizey-1)) break;
      for(x=0; x<DARRAYSIZE; x++) {

	 if(x>(level->sizex-1)) break;
	 val = dterrain[(y * DARRAYSIZE) + x];

	 if(val < level_water)
	    set_terrain(level, x, y, TYPE_SEA);
	 else if(val < level_ground)
	    set_terrain(level, x, y, TYPE_WATER);
	 else if(val < level_hill)
	    set_terrain(level, x, y, TYPE_GRASS);
	 else if(val < level_mount)
	    set_terrain(level, x, y, TYPE_HILLS);
	 else 
	    set_terrain(level, x, y, TYPE_MOUNTAIN);

//			printf("%c", '0'+(int)dterrain[y][x]);
      }
      my_printf("\n");
   }

   my_getch();

}

float avg_square(int delta, int x, int y)
{
   float avg;

   avg = ( dterrain[(y-delta)*DARRAYSIZE + (x-delta)] +
	   dterrain[(y-delta)*DARRAYSIZE + (x+delta)] +
	   dterrain[(y+delta)*DARRAYSIZE + (x+delta)] +
	   dterrain[(y+delta)*DARRAYSIZE + (x-delta)] ) * 0.25f;

   return avg;

}
float avg_diamond(int delta, int x, int y)
{
   float avg;

   if(!x && y>0) {
      avg = (dterrain[(y-delta) * DARRAYSIZE + x] +
	     dterrain[(y+delta) * DARRAYSIZE + x] +
	     dterrain[y*DARRAYSIZE + (x+delta)] +
	     dterrain[y*DARRAYSIZE + (x+DSIZE-delta)]) * 0.25f;
   }
   else if(x==DSIZE && y>0) {
      avg = (dterrain[(y-delta) * DARRAYSIZE + x] +
	     dterrain[(y+delta) * DARRAYSIZE + x] +
	     dterrain[y*DARRAYSIZE + (delta)] +
	     dterrain[y*DARRAYSIZE + (x-delta)]) * 0.25f;
   }
   else if(x>0 && !y) {
      avg = (dterrain[(y+DSIZE-delta) * DARRAYSIZE + x] +
	     dterrain[(y+delta) * DARRAYSIZE + x] +
	     dterrain[y*DARRAYSIZE + (x-delta)] +
	     dterrain[y*DARRAYSIZE + (x+delta)]) * 0.25f;
   }
   else if(x>0 && y==DSIZE) {
      avg = (dterrain[(y-delta)*DARRAYSIZE + x] +
	     dterrain[delta * DARRAYSIZE + x] +
	     dterrain[y * DARRAYSIZE + (x-delta)] +
	     dterrain[y * DARRAYSIZE + (x+delta)]) * 0.25f;
   }
   else {
      avg = (dterrain[(y-delta)*DARRAYSIZE + x] +
	     dterrain[(y+delta)*DARRAYSIZE + x] +
	     dterrain[y*DARRAYSIZE + (x+delta)] +
	     dterrain[y*DARRAYSIZE + (x-delta)]) * 0.25f;
   }

   return avg;
}

void diamond_square(level_type *level, float h)
{
   int delta;
   int x, y;
   float scale, ratio, rr;

   float heightScale;

   heightScale = 100;

   table_init();

   ratio = (float) pow (2.0,-h);
   scale = heightScale * ratio;

   /* init the starting values */
   dterrain[0] = 
      dterrain[DSIZE*DARRAYSIZE] = 
      dterrain[DSIZE] = 
      dterrain[DSIZE * DARRAYSIZE + DSIZE] = 0.0f;

   delta = DSIZE / 2;
   x=y=0;

   while(delta) {
      // generate squares
      for(y=delta; y<DSIZE; y+=delta) {
	 for(x=delta; x<DSIZE; x+=delta) {
	    dterrain[y * DARRAYSIZE + x]= scale * fractRand (.5f) +
	       avg_square(delta, x, y);
	    x+=delta;
	 }
	 y+=delta;
      }

      // generate diamonds
      for(y=delta; y<DSIZE; y+=delta) {
	 for(x=delta; x<DSIZE; x+=delta) {

	    rr = scale * fractRand (.5f);

	    dterrain[DARRAYSIZE * (y-delta) + x]=
	       rr + avg_diamond(delta, x, y-delta);
	    dterrain[DARRAYSIZE * (y+delta) + x]=
	       rr + avg_diamond(delta, x, y+delta); 
	    dterrain[DARRAYSIZE * y + (x-delta)]=
	       rr + avg_diamond(delta, x-delta, y);
	    dterrain[DARRAYSIZE * y + (x+delta)]=
	       rr + avg_diamond(delta, x+delta, y); 
	    x+=delta;
	 }
	 y+=delta;
      }
      delta >>= 1;
      scale *= ratio;
//		delta = 0;
   }

   dsq_2_map(level);
}






/******************************************************************
 *
 * Some old routines
 *
 ******************************************************************/



void set_at_random(level_type *level, int16u type)
{
   int16u sx, sy;
   int16u maxtries = 1000;

   while(maxtries--) {
      sx=1+RANDU(level->sizex-2);
      sy=1+RANDU(level->sizey-2);

      if(!ispassable(level, sx, sy) || level->loc[sy][sx].type == type)
	 continue;

      set_terrain(level, sx, sy, type);
      break;
   }
}


bool advnext_point(level_type *level, int8u mode, int16u *x, int16u *y)
{
   switch(mode) {
      case DIR_RIGHT:
	 if(*x == level->sizex-1) {
	    if(*y == level->sizey-1) {
	       return true;
	    }
	    *x=0;
	    (*y)++;
	 }
	 else
	    (*x)++;
	 
	 break;
      case DIR_LEFT:
	 break;
      default:
	 break;
   }

   return false;
}

bool outworld_dolakes(level_type *level, int8u percentage)
{
   int32u tiles_left;
   int8u seeds_left;
   int16u sx, sy, i;

   bool prev_set, now_clear;
   real amt;

   amt = ((real)level->sizex * level->sizey) / 100 * percentage;
   tiles_left = (int32u)amt;

   msg.vnewmsg(C_GREEN, "Generated %ld tiles of water.", tiles_left);

   /* how may "lake seeds" to be generated */
   seeds_left = throwdice(3, 5, 0);

   msg.vnewmsg(C_GREEN, "Generated %ld water seeds.", seeds_left);


   if(tiles_left <= seeds_left)
      return false;

   tiles_left -= seeds_left;

   /* seed lake spots */
   while(seeds_left--) {
      set_at_random(level, TYPE_WATER);
   }

   sx=sy=0;
   prev_set=false;
   now_clear= false;
   
   while(tiles_left) {
/*
      if(sx>=level->sizex-1) {
	 my_printf("%d", sy);
	 my_refresh();
	 my_getch();
      }
*/


      if(level->loc[sy][sx].type == TYPE_WATER)
	 prev_set=true;
      else
	 prev_set=false;

      if(advnext_point(level, DIR_RIGHT, &sx, &sy)) {
	 sx=sy=0;
      }

      if(level->loc[sy][sx].type == TYPE_GRASS)
	 now_clear=true;
      else
	 now_clear=false;
      
      if(prev_set && now_clear) {
	 if( RANDU(100) < 60 ) {
	    set_terrain(level, sx, sy, TYPE_WATER);
	    tiles_left--;
	    prev_set=now_clear=false;

	    my_gotoxy(1,1);
	    my_printf("%4ld", tiles_left);
	    my_refresh();

	 }
      }      

      
   }

   return true;

   /* now start iterating lakes from their seedpoints */
   while(1) {
      my_gotoxy(1,1);
      my_printf("%ld", tiles_left);
      my_refresh();

      sx=1+RANDU(level->sizex-2);
      sy=1+RANDU(level->sizey-2);
      
      if(level->loc[sy][sx].type == TYPE_WATER) {
	 for(i=1; i<10; i++) {
	    if(level->loc[sy+move_dy[i]][sx+move_dx[i]].type == TYPE_GRASS) {
	       level->loc[sy+move_dy[i]][sx+move_dx[i]].type=TYPE_WATER;
	       tiles_left--;
	       break;
	    }

	 }

      }      

      if(!tiles_left)
	 break;
   }


   return true;
}

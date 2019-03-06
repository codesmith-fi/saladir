/**************************************************************************
 * generate.cc --                                                         *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1888                                         *
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
 **************************************************************************/

#include "saladir.h"

#include "output.h"
#include "types.h"
#include "caves.h"
#include "shops.h"
#include "process.h"
#include "doors.h"
#include "file.h"
#include "scrolls.h"
#include "traps.h"
#include "imanip.h"
#include "generate.h"
#include "pathfunc.h"

/* passage carving doesn't carve locations with sval of this */
#define GENERATE_DONOTCARVE	99

struct seed_type
{
     int16u seedid;
     int16u x;
     int16u y;
     struct seed_type *next;
};

bool createroom(level_type *level, int16s *rx1, int16s *ry1, 
		int16s *sizex, int16s *sizey);
void carvepassage(level_type *, int16u, int16u, int16u, int16u);
void carvepassage2(level_type *, int16u, int16u, int16u, 
		   int16u, bool, int8u dir);
bool checkregion(level_type *, int16u, int16u, int16u, int16u);

/* returns false if ROOMFLOOR or WALL insider room candidant */
bool checkregion_inv(level_type *level, int16u x1, int16u y1, 
		     int16u rsizex, int16u rsizey);

void makeroom(level_type *level, int16s x1, int16s y1, 
	      int16s dir, int16u recd);
void makehall(level_type *level, int16s x1, int16s y1, 
	      int16s dir, int16u recd);
void makehall_only(level_type *level, int16s x1, int16s y1, int16s dir);
void roomylevel2(level_type *level);
bool generatelair(level_type *level);
void generate_easymaze(level_type *level);

void generate_town(level_type *level);
bool generate_townroom(level_type *level, int8u roomtype);

void set_alignment(item_def *item);


int16s delta_x[]= {0, 1, 0, -1, 0};
int16s delta_y[]= {-1, 0, 1, 0, 0};
int16s dirturn[]= {1, 2, 3, 0, 0};

/* Createlevel
**
** This will be called when a new level is needed
*/
bool createlevel(level_type *level)
{
   int16s sx, sy;
   int16u i, num, rnum;
   int16u dtype;

   // set level size first to the minimum sizes
   sx=MINSIZEX;
   sy=MINSIZEY;

   sx=MINSIZEX + RANDU(MAXSIZEX-MINSIZEX);
   sy=MINSIZEY + RANDU(MAXSIZEY-MINSIZEY);

   level->danglev=0;

   dtype=player.levptr->dtype;

   if(dtype==DTYPE_RANDOM ) {
      dtype=RANDU(NUM_DUNGEONTYPES);
   }

   if(dtype==DTYPE_MAZE) {
      // ensure that level x and y are not even (for maze type levels)
      if(!(sx % 2)) sx++;
      if(!(sy % 2)) sy++;
      initlevel(level, TYPE_WALLIP, sx, sy);
      recurse_maze(level, 1, 1);
   } else if( dtype==DTYPE_ROOMY ) {
      initlevel(level, TYPE_WALLIP, sx, sy);
      roomylevel(level);
   } else if( dtype==DTYPE_ROOMY2 ) {
      initlevel(level, TYPE_WALLIP, sx, sy);
      roomylevel2(level);
   } else if(dtype==DTYPE_MAZE2) {
      generate_easymaze(level);
   } else if(dtype==DTYPE_TOWN) {
      generate_town(level);
   } else {
      // if all else fails, we create a ROOMY level
      initlevel(level, TYPE_WALLIP, sx, sy);
      roomylevel(level);
   }

   if(dtype==DTYPE_WILDHUNT)
      return true;

   monster_addspecial(level);

   if(dtype==DTYPE_TOWN) {
      shop_init(level);

      sy=5;
      for(int16s sx=4; sx<(level)->sizex-5; sx++) {
	 if(ispassable(level, sx, sy) &&
	    level->loc[sy][sx].type!=TYPE_ROOMFLOOR ) {
	    player.pos_x=sx;
	    player.pos_y=sy;
	    player.lreg_x=sx-4;
	    player.lreg_y=sy-4;
	    player.spos_x=5;
	    player.spos_y=5;

	    if(player.pos_x<5)
	       player.spos_x=player.pos_x+1;
	    if(player.pos_y<5)
	       player.spos_y=player.pos_y+1;
	 }
      }
      return true;
	
   }
   
   createstairs(level);

   num=3+RANDU(10);	// how many items to create
   for(i=0; i<num; i++) {
      sx=RANDU(level->sizex);
      sy=RANDU(level->sizey);
      while(!ispassable(level, sx, sy)) {
	 sx=RANDU(level->sizex);
	 sy=RANDU(level->sizey);
      }
      rnum=RANDU(100);

      if(rnum < 15)
	 level_createitem(level, sx, sy, IS_SCROLL, -1, 1, -1);
      else if(rnum < 30)
	 level_createitem(level, sx, sy, IS_WEAPON1H, -1, 1, -1);
      else if(rnum < 70)
	 level_createitem(level, sx, sy, IS_ARMOR, -1, 1, -1);
      else 
	 level_createitem(level, sx, sy, ISMG_MISCITEM, -1, 1, -1);

   }

/*
  level_createitem(level, player.pos_x, player.pos_y, 
  ISMG_MISCITEM, MISCI_RING, 1, -1);
  level_createitem(level, player.pos_x, player.pos_y, 
  ISMG_MISCITEM, MISCI_RING, 1, -1);
*/

//   trap_create(level, player.pos_x-1, player.pos_y, -1);
//   level_createitem(level, player.pos_x, player.pos_y, 
//		    ISMG_MISCITEM, MISCI_CHEST, 1, -1);
   // add gold and coins
   for(i=0; i<RANDU(5); i++) {
      sx=RANDU(level->sizex);
      sy=RANDU(level->sizey);

      while(!ispassable(level, sx, sy)) {
	 sx=RANDU(level->sizex);
	 sy=RANDU(level->sizey);
      }
      level_createitem(level, sx, sy, 
		       IS_MONEY, -1, RANDU(128), -1);
   }

   // create monsters but not to players location
   for(i=0; i<5+RANDU(6); i++) {
      while(1) {
	 sx=2+RANDU(level->sizex-4);
	 sy=2+RANDU(level->sizey-4);

	 if(ispassable(level, sx, sy) ) {
	    if(sx==player.pos_x && sy==player.pos_y)
	       continue;
	    else
	       break;
	 }
      }
      monster_add(level, sx, sy, 0);
   }

   /* generate lairs and dungeon shops */
   generatelair(level);

   /* init shops if needed */
   shop_init(level);

   return true;
}

/* initlevel
**
** This will be called at the beginning of new level generation
** it will clear the level with "mode".
*/
bool initlevel(level_type *level, int8u mode, int16u sizex, int16u sizey) {

   int16u tx, ty;
   player.lreg_x=player.lreg_y=0;

   freelevel(level);

   level->sizex=sizex;
   level->sizey=sizey;

   level->roomcount=0;

   level->monsters=NULL;
//   level->items=NULL;	// no items in level at first

   inv_init(&level->inv, NULL);

   // allocate memory for new level
   // clear the maze
   for(ty=0; ty<level->sizey; ty++) {
      level->loc[ty] = new cave_type[level->sizex+1]; // allocate row
      if(!level->loc[ty])
	 ERROROUT("---initlevel--- OUT OF MEMORY!");
      for(tx=0; tx<level->sizex; tx++) {
	 mem_clear(&level->loc[ty][tx], sizeof(cave_type));
	 level->loc[ty][tx].type=mode;		// clear row with TYPE_PATH
	 level->loc[ty][tx].flags=terrains[mode].flags;
      }
   }
   return true;
}

// frees all memory allocated by level *maze
int8u freelevel(level_type *level)
{
   removeitems(level);
   monster_removeall(level);

   /* clear the room array */
   for(int16u i=0; i<ROOM_MAXNUM; i++) {
      mem_clear(&level->rooms[i], sizeof(_roomdef));
   }

   for(int16u ty=0; ty<MAXSIZEY; ty++) {
      if(level->loc[ty])
	 delete [] level->loc[ty];
      level->loc[ty]=NULL;
   }

   level->sizex=level->sizey=0;

//   inv_init(&level->inv, NULL);
//   level->items=NULL;
//   inv_removeall(&level->inv, NULL);

   level->monsters=NULL;
   level->roomcount=0;

   return 1;
}


bool set_terrain(level_type *level, int16s x, int16s y, int8u type)
{
     level->loc[y][x].type=type;
     level->loc[y][x].flags=terrains[type].flags;

     return true;
}

void roomylevel2(level_type *level)
{
     makeroom(level, level->sizex/2, level->sizey/2, -1, 0);
}

void create_type_random(level_type *level, int8u type, int16u count)
{
   int16s sx, sy;
   int16u num=count;

   while(num>0) {
      num--;
      sx=1+RANDU(level->sizex-2);
      sy=1+RANDU(level->sizey-2);

      if(!ispassable(level, sx, sy))
	 continue;

      set_terrain(level, sx, sy, type);
   }
}

void generate_town(level_type *level)
{
   int16s sx, sy;

   // set level size first to the minimum sizes
   sx=MINSIZEX+RANDU(20);
   sy=40;
   if(sy<MINSIZEY) sy=MINSIZEY;

   initlevel(level, TYPE_GRASS, sx, sy);

   create_type_random(level, TYPE_TREE, 150);

   generate_townroom(level, ROOMTMPL_CASTLE1);

   if(generate_townroom(level, ROOMTMPL_SHOP1))
      level->rooms[level->roomcount-1].shoptype=SHOPTYPE_GENERAL;

   if(generate_townroom(level, ROOMTMPL_SHOP5))
      level->rooms[level->roomcount-1].shoptype=-1;

   generate_townroom(level, ROOMTMPL_FARM);
   generate_townroom(level, ROOMTMPL_HOUSE1);
   generate_townroom(level, ROOMTMPL_HOUSE1);
   generate_townroom(level, ROOMTMPL_HOUSE2);
   generate_townroom(level, ROOMTMPL_HOUSE1);

   createstairs(level);
}

void generate_wildhunt(level_type *level)
{
     int16s sx, sy;

     // set level size first to the minimum sizes
     sx=MINSIZEX+10;
     sy=MINSIZEY+10;

//     init_lineofsight(10);
     player.sight=10;
     save_tmpfile(level, player.dungeon, player.dungeonlev);

     player.wildx=player.pos_x;
     player.wildy=player.pos_y;

     initlevel(level, TYPE_GRASS, sx, sy);
     create_type_random(level, TYPE_TREE, 150);

     player.huntmode=true;

     player.pos_x=10;
     player.pos_y=10;
     player.spos_x=5;
     player.spos_y=5;
     player.lreg_x=player.pos_x-4;
     player.lreg_y=player.pos_y-4;
     if(player.pos_x<5)
	  player.spos_x=player.pos_x+1;
     if(player.pos_y<5)
	  player.spos_y=player.pos_y+1;

     my_clrscr();
     do_redraw(level);
}


void generate_easymaze(level_type *level) {

   int16s sx, sy, holes;
   int16s bx, by;

   /* random size */
   sx=MINSIZEX + RANDU(MAXSIZEX-MINSIZEX);
   sy=MINSIZEY + RANDU(MAXSIZEY-MINSIZEY);

   // ensure that level x and y are not even (for maze type levels)
   if(!(sx % 2)) sx++;
   if(!(sy % 2)) sy++;
   initlevel(level, TYPE_WALLIP, sx, sy);
   recurse_maze(level, 1, 1);

   holes=(sx*sy)/(30+RANDU(20));

   while(holes>0) {
      sx=1+RANDU(level->sizex-2);
      sy=1+RANDU(level->sizey-2);

      if(ispassable(level, sx, sy))
	 continue;

      set_terrain(level, sx, sy, TYPE_PASSAGE);

      holes--;
   }

   int16s cx, cy;

   holes=15+RANDU(20);

   while(holes) {
      sx=5+RANDU(10);
      sy=5+RANDU(5);

      by=1+RANDU(level->sizey - sy - 2 );
      bx=1+RANDU(level->sizex - sx - 2 );

      cx=sx;
      cy=sy;

      while(cy>0) {
	 cx=sx;
	 while(cx>0) {
	    set_terrain(level, bx+cx, by+cy, TYPE_PASSAGE);
	    cx--;
	 }
	 cy--;
      }
      holes--;
   }

}


bool generate_townroom(level_type *level, int8u roomtype)
{
   int16s x1, y1, wd, hg, maxr, i, j;

   Troomtemplate *rooms;
   char *tmplptr;

   rooms=towntemplates+roomtype;

   wd=rooms->sx;
   hg=rooms->sy;

   tmplptr=rooms->room;

   maxr=0;
   while(1) {
      x1=4+RANDU(level->sizex-8-wd);
      y1=4+RANDU(level->sizey-8-hg);

      if( checkregion_inv(level, x1-2, y1-2, wd+4, hg+4) ) {
	 break;
      }
      maxr++;
      if(maxr>=99)
	 return false;
   }

   /* if roomtable is already full */
   if(level->roomcount>=ROOM_MAXNUM)
      return false;

   /* update level roomlist */
   my_strcpy(level->rooms[level->roomcount].name, 
	     rooms->name, ROOM_NAMEMAX-1);
   level->rooms[level->roomcount].type=rooms->roomtype;
   level->rooms[level->roomcount].x1=x1;
   level->rooms[level->roomcount].y1=y1;
   level->rooms[level->roomcount].x2=x1+wd-1;
   level->rooms[level->roomcount].y2=y1+hg-1;

   for(j=0; j<hg; j++) {
      for(i=0; i<wd; i++) {

	 /* or replace by something else */
	 /* impassable walls */
	 if(*tmplptr=='#') {
	    set_terrain(level, x1+i, y1+j, TYPE_WALLIP);
	 }
	 else if(*tmplptr=='&') {
	    /* skip */

	 }
	 else if(*tmplptr=='=') {
	    set_terrain(level, x1+i, y1+j, TYPE_WATER);
	 }
	 else if(*tmplptr=='.') {
	    set_terrain(level, x1+i, y1+j, TYPE_ROOMFLOOR);
	 }
	 else if(*tmplptr==',') {
	    set_terrain(level, x1+i, y1+j, TYPE_ROOMFLOOR);
	 }
	 else if(*tmplptr=='|') {
	    set_terrain(level, x1+i, y1+j, TYPE_BRIDGEV);
	 }
	 else if(*tmplptr=='-') {
	    set_terrain(level, x1+i, y1+j, TYPE_BRIDGEH);
	 }
	 else if(*tmplptr=='+') {
	    /* generate a normal door */
	    door_create(level, x1+i, y1+j, false);
	 }
	 else if(*tmplptr=='T') {
	    set_terrain(level, x1+i, y1+j, TYPE_TREE);
	 }
	 /* exitpoint (door) */
	 else if(*tmplptr=='!') {
	    /* generate door and add its coordinates to the roomlist */
	    door_create(level, x1+i, y1+j, false);
	    level->rooms[level->roomcount].doorx=x1+i;
	    level->rooms[level->roomcount].doory=y1+j;
	 }
	 else {
	    my_printf("Lair generator, unknown type: %d (%c)\n", 
		      *tmplptr, *tmplptr);
	    my_getch();
	 }
	 tmplptr++;
      }
   }
   level->roomcount++;
   return true;
}

bool generatelair(level_type *level)
{
   int16s x1, y1, wd, hg, maxr, i, j;
   _monsterlist *habitant;
   Troomtemplate *rooms;
   char *tmplptr;

   int8u lastattr;

   rooms=lairtemplates+RANDU(num_lairs);

   wd=rooms->sx;
   hg=rooms->sy;

   tmplptr=rooms->room;

   maxr=0;
   while(1) {
      x1=4+RANDU(level->sizex-8-wd);
      y1=4+RANDU(level->sizey-8-hg);

      if( checkregion(level, x1-1, y1-1, wd+2, hg+2) ) {
	 break;
      }
      maxr++;
      if(maxr>=99)
	 return false;
   }

   /* if roomtable is already full */
   if(level->roomcount>=ROOM_MAXNUM)
      return false;

   /* update level roomlist */
   my_strcpy(level->rooms[level->roomcount].name, rooms->name, 
	     ROOM_NAMEMAX-1);
   level->rooms[level->roomcount].type=rooms->roomtype;
   level->rooms[level->roomcount].x1=x1;
   level->rooms[level->roomcount].y1=y1;
   level->rooms[level->roomcount].x2=x1+wd-1;
   level->rooms[level->roomcount].y2=y1+hg-1;

   if(rooms->roomtype == ROOM_SHOP) {
      level->rooms[level->roomcount].shoptype=-1;
   }

   lastattr=terrains[TYPE_ROOMFLOOR].flags;
   for(j=0; j<hg; j++) {
      for(i=0; i<wd; i++) {

	 /* create passable areas (floor) */
	 level->loc[y1+j][x1+i].type=TYPE_ROOMFLOOR;
	 level->loc[y1+j][x1+i].flags=lastattr;

	 /* or replace by something else */
	 /* impassable walls */
	 if(*tmplptr=='#') {
	    level->loc[y1+j][x1+i].sval=GENERATE_DONOTCARVE;
	    level->loc[y1+j][x1+i].type=TYPE_WALLIP;
	    level->loc[y1+j][x1+i].flags=terrains[TYPE_WALLIP].flags;
	 }
	 else if(*tmplptr=='&') {
	    level->loc[y1+j][x1+i].sval=GENERATE_DONOTCARVE;
	    level->loc[y1+j][x1+i].type=TYPE_WALLIP;
	    level->loc[y1+j][x1+i].flags=terrains[TYPE_WALLIP].flags;
	    level->loc[y1+j][x1+i].flags|=CAVE_NOLIT;
	 }
	 else if(*tmplptr==',') {
	    level->loc[y1+j][x1+i].flags|=CAVE_NOLIT;
	    lastattr=terrains[TYPE_ROOMFLOOR].flags|CAVE_NOLIT;
	 }
	 else if(*tmplptr=='.') {
	    level->loc[y1+j][x1+i].flags=terrains[TYPE_ROOMFLOOR].flags;
	    lastattr=terrains[TYPE_ROOMFLOOR].flags;
	 }
	 else if(*tmplptr=='?') {
	    /* add room owner */
	    habitant=monster_add(level, x1+i, y1+j, 0);
	 }
	 /* exitpoint (door) */
	 else if(*tmplptr=='!') {
	    door_create(level, x1+i, y1+j, false);
	    level->rooms[level->roomcount].doorx=x1+i;
	    level->rooms[level->roomcount].doory=y1+j;
	 }
	 else if(((*tmplptr)>'A'-1) && ((*tmplptr)<'Z'+1)
		 || ((*tmplptr)>'a'-1) && ((*tmplptr)<'z'+1) ) {
	    habitant=monster_add(level, x1+i, y1+j, *tmplptr);
	 }
	 else {
	    my_printf("Lair generator, unknown type: %d (%c)\n", 
		      *tmplptr, *tmplptr);
	    my_getch();
	 }
	 tmplptr++;
      }
   }

   makehall_only(level, level->rooms[level->roomcount].doorx, 
		 level->rooms[level->roomcount].doory, rooms->doordir);

   /* clear the sval */
   for(y1=0; y1<level->sizey; y1++) {
      for(x1=0; x1<level->sizex; x1++) {
	 level->loc[y1][x1].sval=0;
      }
   }

   level->roomcount++;

   return true;
}

/* create a random length hall from x1,y1 to direction x */
void makehall(level_type *level, int16s x1, int16s y1, int16s dir, int16u recd)
{
   int16s len, dir2;

   if(recd>1000)
      return;

   /* random length */
   len=4+RANDU(8);

   for(int i=0; i<len; i++) {
      /* move to dir */
      x1+=delta_x[dir];
      y1+=delta_y[dir];

      /* check boundaries */
      if(x1>=level->sizex-1 || y1>=level->sizey-1
	 || x1<1 || y1<1)
	 return;

      if(level->loc[y1][x1].type==TYPE_PASSAGE
	 || level->loc[y1][x1].type==TYPE_ROOMFLOOR)
	 return;

      if(dir==0 || dir==2) {
	 if(level->loc[y1][x1-1].flags & CAVE_PASSABLE)
	    return;
	 if(level->loc[y1][x1+1].flags & CAVE_PASSABLE)
	    return;
      }
      if(dir==1 || dir==3) {
	 if(level->loc[y1-1][x1].flags & CAVE_PASSABLE)
	    return;
	 if(level->loc[y1+1][x1].flags & CAVE_PASSABLE)
	    return;
      }

      /* place terrain */
      level->loc[y1][x1].type=TYPE_PASSAGE;
      level->loc[y1][x1].flags=terrains[TYPE_PASSAGE].flags;

      if(i==0)
	 door_create(level, x1, y1, true);

   }

   len=RANDU(100);
   if(len>50) {
      while(1) {
	 dir2=RANDU(4);
	 if(dir2!=dir)
	    break;
      }
      makehall(level, x1, y1, dir2, recd+1);
   }
   else if(len>4 && len<=50)
      makeroom(level, x1, y1, dir, recd+1);
   else
      return;
}

void makehall_only(level_type *level, int16s x1, int16s y1, int16s dir)
{
   int16s len;
   int16u MAXRETR=10000;

   len=10+RANDU(20);
   while(1 && MAXRETR>0) {
      /* move to dir */
      x1+=delta_x[dir];
      y1+=delta_y[dir];

      /* try to avoid infinite loops */
      MAXRETR--;

      /* check boundaries */
      if(x1>=level->sizex-2 || y1>=level->sizey-2
	 || x1<1 || y1<1) {
	 x1-=delta_x[dir];
	 y1-=delta_y[dir];
	 dir=RANDU(4);
//			   dir=dirturn[dir];
	 continue;
      }
      if(level->loc[y1][x1].flags & CAVE_PASSABLE) {
	 if(level->loc[y1][x1].sval==GENERATE_DONOTCARVE) {
//			   dir=dirturn[dir];
	    dir=RANDU(4);
	    continue;
	 }
	 /* create a door there */
	 door_create(level, x1-delta_x[dir], y1-delta_y[dir], true);

	 return;
      }
      /* place terrain */
      /* sval must be cleared after this routine */
      level->loc[y1][x1].sval=GENERATE_DONOTCARVE;
      level->loc[y1][x1].type=TYPE_PASSAGE;
      level->loc[y1][x1].flags|=terrains[TYPE_PASSAGE].flags;

      len--;
      if(len==0) {
	 len=10+RANDU(20);
	 dir=RANDU(4);
      }
   }

}


void makeroom(level_type *level, int16s x1, int16s y1, int16s dir, int16u recd)
{
   int16s hg, wd;
   int16s x2, y2;
   int8u newflags=0;

   /* if roomtable is already full */
   if(level->roomcount>=ROOM_MAXNUM)
      return;

   if(recd>1000)
      return;

   x2=x1;
   y2=y1;

//   newflags=terrains[TYPE_ROOMFLOOR].flags;
   /* create dark room */
   if(RANDU(100)>80) {
      newflags=CAVE_NOLIT;
   }

   while(1) {
      wd=ROOM_MINX+RANDU(ROOM_MAXX-ROOM_MINX);
      if(!(wd%2))
	 break;
   }
   while(1) {
      hg=ROOM_MINY+RANDU(ROOM_MAXY-ROOM_MINY);
      if(!(hg%2))
	 break;
   }
   if(dir==0) {	/* north */
      y2-=hg;
      x2-=wd/2;
   }
   else if(dir==1) { /* east */
      y2-=hg/2;
   }
   else if(dir==2) { /* south */
      x2-=wd/2;
   }
   else if(dir==3) { /* west */
      x2-=wd;
      y2-=hg/2;
   }

   if(x2==0)
      x2++;
   if(y2==0)
      y2++;

   if( checkregion(level, x2, y2, wd, hg) ) {
      /* update level roomlist */
      my_strcpy(level->rooms[level->roomcount].name, "an ordinary room", ROOM_NAMEMAX-1);
      level->rooms[level->roomcount].type=ROOM_NORMAL1;
      level->rooms[level->roomcount].x1=x2;
      level->rooms[level->roomcount].y1=y2;
      level->rooms[level->roomcount].x2=x2+wd-1;
      level->rooms[level->roomcount].y2=y2+hg-1;
      level->roomcount++;

      for(int j=0; j<hg; j++) {
	 for(int i=0; i<wd; i++) {
	    level->loc[y2+j][x2+i].type=TYPE_ROOMFLOOR;
	    level->loc[y2+j][x2+i].flags=terrains[TYPE_ROOMFLOOR].flags | newflags;
//	    level->loc[y2+j][x2+i].flags=newflags;
//	    if(j==0 || i==0 || j==hg-1 || i==wd-1) {
//	       level->loc[j+y2][i+x2].type=TYPE_WALLIP;
//	       level->loc[j+y2][i+x2].flags=terrains[TYPE_WALLIP].flags | newflags;
//	    }
	 }
      }

      for(dir=0; dir<4; dir++) {
	 x1=x2;
	 y1=y2;
	 if(dir==0) {
//		   	x1+=wd/2;
	    x1+=1+RANDU(wd-2);
	 }
	 else if(dir==1) {
	    x1+=wd-1;
	    y1+=1+RANDU(hg-2);
//		      y1+=hg/2;
	 }
	 else if(dir==2) {
	    x1+=1+RANDU(wd-2);
//		   	x1+=wd/2;
	    y1+=hg-1;
	 }
	 else if(dir==3) {
	    y1+=1+RANDU(hg-2);
//		      y1+=hg/2;
	 }
	 makehall(level, x1, y1, dir, recd+1);
      }
   }
   else {
      dir=RANDU(4);

      makehall(level, x1, y1, dir, recd+1);
   }
}


void roomylevel(level_type *level)
{
   int16u i;
//   int16u ocx=0, ocy=0, ncx=0, ncy=0;

//   int16u tlx, tly, brx, bry;
/*
  for(i=0; i<8; i++) {
  if(ncx && ncy) {
  ocx=ncx;
  ocy=ncy;
  }
  createroom(level, &ncx, &ncy);
//      createroom(level, &tlx, &tly, &brx, &bry);

if(ocx && ocy && ncx && ncy)
carvepassage2(level, ocx, ocy, ncx, ncy, true);
}
*/

   int16s x1=0, y1=0, sx=0, sy=0;
   int16s ox1=0, oy1=0, osx=0, osy=0;	/* old values */

   int16s cx, cy, difx, dify;
   int16s ocx, ocy;
   int16s pasy, pasx;
   int16s doory, doorx;
   int8u dir;

   for(i=0; i<4+RANDU(ROOM_MAXNUM-4); i++) {
      if(x1 && y1 && sx && sy) {
	 ox1=x1;
	 oy1=y1;
	 osx=sx;
	 osy=sy;
      }
      /* plant a room */
      if(!createroom(level, &x1, &y1, &sx, &sy))
	 continue;

//      my_printf("old room at %d:%d, %d,%d\n", ox1, oy1, osx, osy);
//      my_printf("created room at %d:%d, %d,%d\n", x1, y1, sx, sy);
//		my_getch();
      if(x1 && y1 && ox1 && oy1) {

	 /* get center coords for rooms */
	 cx=x1+(sx/2);
	 cy=y1+(sy/2);
	 ocx=ox1+(osx/2);
	 ocy=oy1+(osy/2);

	 difx=ocx-cx;
	 dify=ocy-cy;

	 if(abs(difx) > abs(dify)) {
	    if(cx > ocx) {
	       doorx=x1;
	       doory=y1+sy/2;
	       pasx=x1-1;
	       pasy=y1+sy/2;
	       dir=3;
	    }
	    else {
	       doorx=x1+sx-1;
	       doory=y1+sy/2;
	       pasx=x1+sx;
	       pasy=y1+sy/2;
	       dir=1;
	    }

	 }
	 else {
	    if(cy > ocy) {
	       doorx=x1+sx/2;
	       doory=y1;
	       pasx=x1+sx/2;
	       pasy=y1-1;
	       dir=0;

	    }
	    else {
	       doorx=x1+sx/2;
	       doory=y1+sy-1;
	       pasx=x1+sx/2;
	       pasy=y1+sy;
	       dir=2;
	    }
	 }
	 door_create(level, doorx, doory, true);
	 carvepassage2(level, pasx, pasy, ocx, ocy, true, dir);
      }
   }
}

void swapint16u(int16u *num1, int16u *num2)
{
   int16u aux;

   aux=*num1;
   *num1=*num2;
   *num2=aux;
}

void carvepassage2(level_type *level, int16u sx, int16u sy, 
		   int16u tx, int16u ty, bool stopopen, int8u dir)
{
   int16u x_now, y_now;
   int8u doorc;

   /* this shouldn't be possible */
   if(sx==tx && sy==ty) return;

   y_now=sy;
   x_now=sx;
   doorc=0;
   while(1) {
      if(x_now>=level->sizex || y_now>=level->sizey || 
	 x_now<=0 || y_now <= 0) {
	 my_printf("OHHOH! Carving over level boundaries at %d, %d.", 
		   x_now, y_now);
	 my_getch();
      }

      /* stop carving if digging on open passage */
      if(ispassable(level, x_now, y_now) && stopopen)
	 return;

      set_terrain(level, x_now, y_now, TYPE_PASSAGE);

      if(x_now==tx && y_now==ty) break;

      if(dir==0 || dir==2) {
	 if(level->loc[y_now][x_now-1].flags & CAVE_PASSABLE)
	    return;
	 if(level->loc[y_now][x_now+1].flags & CAVE_PASSABLE)
	    return;
      }
      if(dir==1 || dir==3) {
	 if(level->loc[y_now-1][x_now].flags & CAVE_PASSABLE)
	    return;
	 if(level->loc[y_now+1][x_now].flags & CAVE_PASSABLE)
	    return;
      }


      if(x_now<tx) {
	 x_now++;
	 dir=1;
      }
      else if(x_now>tx) {
	 x_now--;
	 dir=3;
      }
      else if(y_now<ty) {
	 y_now++;
	 dir=2;
      }
      else if(y_now>ty) {
	 y_now--;
	 dir=0;
      }
   }
}

void carvepassage(level_type *level, int16u sx, int16u sy, int16u tx, int16u ty)
{
   int8u prop_lr=0, prop_ud=0;

   int16u x_now, y_now;
   int8u dir, rnum;

   x_now=sx;	/* initialize starting coords */
   y_now=sy;

   if(sx==tx && sy==ty) return;

   rnum=RANDU(100);
   while(1) {
      if(x_now==tx)
	 prop_lr=50; 		/* equal probability left and right */
      else if(x_now>tx) prop_lr=95;	/* we need to move left more */
      else prop_lr=5;		/* we need to move right more */

      if(y_now==ty)
	 prop_ud=50;			/* up and down equal */
      else if(y_now>ty) prop_ud=95;	/* more downwards */
      else prop_ud=5;		/* more upwards */

      if(RANDU(100)<2)
	 rnum=RANDU(100);

      if(RANDU(100)>30) {
	 if(rnum >= prop_lr) dir=1; // right
	 else dir=2;						// left
      }
      else {
	 if(rnum < prop_ud) dir=4;	// down
	 else dir=3;						// up
      }
/*
  if(x_now <= 1 || x_now >= level->sizex-1) {
  dir=4;
  if(prop_ud>50) dir=3;
  }
  if(y_now <= 1 || y_now >= level->sizey-1) {
  dir=1;
  if(prop_lr>50) dir=2;
  }
*/
      switch(dir)
      {
	 case 1:	x_now++;
	    break;
	 case 2:	x_now--;
	    break;
	 case 3:	y_now++;
	    break;
	 case 4:	y_now--;
	    break;

      }
      if(x_now <= 1 || x_now >= level->sizex-1) break;
      if(y_now <= 1 || y_now >= level->sizey-1) break;

      level->loc[y_now][x_now].type=TYPE_PATH;
      level->loc[y_now][x_now].flags=terrains[TYPE_PATH].flags | CAVE_SEEN;

      if(x_now > tx+ROOM_MINX/2 && x_now < tx-ROOM_MINX/2 &&
         y_now > ty+ROOM_MINY/2 && y_now < ty-ROOM_MINY/2) break;
   }
}

/* Plant a room in to the level!
**
** It returns the topleft and bottomright coordinates for the room
*/
//void createroom(level_type *level, int16u *ncx, int16u *ncy)

bool createroom(level_type *level, int16s *rx1, int16s *ry1, int16s *sizex, int16s *sizey)
{
   int16s x1, y1, i, j;
   int16s rsizex, rsizey;
   int16s maxtries=100;

   int8u newflags=0;

//   bool darkroom=false;

//   newflags=terrains[TYPE_ROOMFLOOR].flags;

   /* create dark room */
   if(RANDU(100)>80) {
      newflags=CAVE_NOLIT;
//      darkroom=true;
   }

   /* random size */
   rsizex=ROOM_MINX+RANDU(ROOM_MAXX-ROOM_MINX);
   rsizey=ROOM_MINY+RANDU(ROOM_MAXY-ROOM_MINY);

   /* random upper left corner */
   while(1) {
      x1=3+RANDU(level->sizex - rsizex - 6);
      y1=3+RANDU(level->sizey - rsizey - 6);

      if(checkregion(level, x1-2, y1-2, rsizex+4, rsizey+4))
	 break;

      if(!maxtries--) {
	 *rx1=*ry1=0;
	 *sizex=*sizey=0;
	 return false;
      }
   }

   /* if roomtable is already full */
   if(level->roomcount>=ROOM_MAXNUM) {
      *rx1=*ry1=0;
      *sizex=*sizey=0;
      return false;
   }

   /* update level roomlist */
   my_strcpy(level->rooms[level->roomcount].name, "an ordinary room", 
	     ROOM_NAMEMAX-1);
   level->rooms[level->roomcount].type=ROOM_NORMAL1;
   level->rooms[level->roomcount].x1=x1+1;
   level->rooms[level->roomcount].y1=y1+1;
   level->rooms[level->roomcount].x2=x1+rsizex-2;
   level->rooms[level->roomcount].y2=y1+rsizey-2;
   level->roomcount++;

   for(j=0; j<rsizey; j++) {
      for(i=0; i<rsizex; i++) {
	 level->loc[j+y1][i+x1].type=TYPE_ROOMFLOOR;
	 level->loc[j+y1][i+x1].flags=terrains[TYPE_ROOMFLOOR].flags | newflags;
	 if(j==0 || i==0 || j==rsizey-1 || i==rsizex-1) {
	    level->loc[j+y1][i+x1].type=TYPE_WALLIP;
	    level->loc[j+y1][i+x1].flags=terrains[TYPE_WALLIP].flags; // | newflags;
	 }

      }
   }
   /* return center coords of the new room */
//	*tlx=x1;
//	*tly=y1;
//	*brx=x1+rsizex;
//	*bry=y1+rsizey;

   *rx1=x1;
   *ry1=y1;
   *sizex=rsizex;
   *sizey=rsizey;

   return true;
//	*ncx=x1+(rsizex/2);
//	*ncy=y1+(rsizey/2);
}

bool checkregion_inv(level_type *level, int16u x1, int16u y1, int16u rsizex, int16u rsizey)
{
   int8u i, j;

   if(x1==0 || y1==0)
      return false;

   for(j=0; j<rsizey; j++) {
      for(i=0; i<rsizex; i++) {
	 /* check boundaries */
	 if((j+y1)>=level->sizey || (i+x1)>=level->sizex)
	    return false;

	 if( level->loc[j+y1][i+x1].type==TYPE_WALLIP) {

	    return false;
	 }
	 if( level->loc[j+y1][i+x1].type==TYPE_ROOMFLOOR) {

	    return false;
	 }
      }
   }

   return true;
}

bool checkregion(level_type *level, int16u x1, int16u y1, int16u rsizex, int16u rsizey)
{
   int8u i, j;

   if(x1==0 || y1==0)
      return false;

   for(j=0; j<rsizey; j++) {
      for(i=0; i<rsizex; i++) {
	 /* check boundaries */
	 if((j+y1)>=level->sizey || (i+x1)>=level->sizex)
	    return false;

	 if(level->loc[j+y1][i+x1].type!=TYPE_WALLIP) {
	    return false;
	 }
      }
   }

   return true;
}

/* This function builds a maze by recursively calling itself
** there is only one solution for this maze
*/
void recurse_maze(level_type *maze, int16u x, int16u y)
{
   int p = 0, d[4];
   maze->loc[y][x].type = TYPE_PATH;
   maze->loc[y][x].flags= CAVE_PASSABLE;

   if ((x > 1) && (maze->loc[y][x- 2].type == TYPE_WALLIP))
      d[p++] = 1;
   if ((y > 1) && (maze->loc[y- 2][x].type == TYPE_WALLIP))
      d[p++] = 2;
   if ((x < (maze->sizex - 2)) && (maze->loc[y][x + 2].type == TYPE_WALLIP))
      d[p++] = 3;
   if ((y < (maze->sizey - 2)) && (maze->loc[y + 2][x].type == TYPE_WALLIP))
      d[p++] = 4;
   if (!p) return;
   p = d[rand() % p];

   switch (p)
   {
      case 1:	maze->loc[y][x - 1].type = TYPE_PATH;
	 maze->loc[y][x - 1].flags= CAVE_PASSABLE;
	 recurse_maze(maze, x - 2, y);
	 break;
      case 2:	maze->loc[y - 1][x].type = TYPE_PATH;
	 maze->loc[y - 1][x].flags= CAVE_PASSABLE;
	 recurse_maze(maze, x, y - 2);
	 break;
      case 3:	maze->loc[y][x + 1].type = TYPE_PATH;
	 maze->loc[y][x + 1].flags = CAVE_PASSABLE;
	 recurse_maze(maze, x + 2, y);
	 break;
      case 4:	maze->loc[y + 1][x].type = TYPE_PATH;
	 maze->loc[y + 1][x].flags= CAVE_PASSABLE;
	 recurse_maze(maze, x, y + 2);
	 break;
   }
   recurse_maze(maze, x, y);

}

void listitems(void)
{
   item_def *itmptr;
   itmptr=weapons;

   my_printf("These items are listed\n\nWeapons:\n");

   while(itmptr->name) {
      my_printf("%s", itmptr->name);
      if(itmptr->sname[0]) {
	 my_printf(" named \"%s\"", itmptr->sname);
      }
      my_printf("\n");
      itmptr++;
   }
}


bool random_scrollname(char *str, int16s maxlen)
{
   sint numsyll=0, cur, num, num2;
   
   if(!str)
      return false;
   
   maxlen--;
   if(maxlen<=0)
      return false;
   
   while(syllables[numsyll]) 
      numsyll++;
   
   if(!numsyll) {
      my_strcpy(str, "error", maxlen);
      return 0;
   }

   str[0]=0;
   
   num=num2=1+RANDU(3)+RANDU(3);

   while(maxlen>0) {
      cur=RANDU(numsyll);

      maxlen-=strlen(syllables[cur]);
      if(maxlen<=0 || num==0)
	 return true;

      if(RANDU(100)<10 && maxlen>0 && strlen(str)>0 ) {
	 maxlen--;
	 strcat(str, " ");
      }
      
      strcat(str, syllables[cur]);
      num--;
   }
   return true;
}

void inititems()
{
   item_def *ptr;
   _Tvaluable *vptr;
   _npcracedef *rptr;
   _monsterdef *mptr;
   Troomtemplate *lairptr;
   _matlist *matptr;
   Tclassdef *clptr;
   Tscroll *scptr;
   _dungeondef *dungptr;

   num_shopkeepers=0;
   mptr=shopkeeper_list;
   while(mptr->desc[0]!=0) {
      num_shopkeepers++;
      mptr++;
   }

   // init money
   num_valuables=0;
   vptr=valuables;
   while(vptr->name[0]!=0) {
      num_valuables++;
      vptr++;
   }

   // init specials
   num_specials=0;
   ptr=SPECIAL_ITEMS;
   while(ptr->name[0]!=0) {
      num_specials++;
      ptr++;
   }

   // init weapons
   num_weapons=0;
   ptr=weapons;
   while(ptr->name[0]!=0) {
      num_weapons++;
      ptr++;
   }

   // init weapons
   num_artifacts=0;
   ptr=artifacts;
   while(ptr->name[0]!=0) {
      num_artifacts++;
      ptr++;
   }

   num_armors=0;
   ptr=armor;
   while(ptr->name[0]!=0) {
      num_armors++;
      ptr++;
   }

   num_miscitems=0;
   ptr=miscitems;
   while(ptr->name[0]!=0) {
      num_miscitems++;
      ptr++;
   }


   num_npcraces=0;
   rptr=npc_races;
   while(rptr->name) {
      num_npcraces++;
      rptr++;
   }

   num_classes=0;
   clptr=classes;
   while(clptr->name) {
      num_classes++;
      clptr++;
   }

   /* don't count the first scroll */
   num_scrolls=0;
   scptr=list_scroll+1;
   while(scptr->name) {
      num_scrolls++;

      /* initialize scrollnames */
      random_scrollname(scptr->uname, sizeof(scptr->uname));

      scptr++;
   }

   num_materials=0;
   matptr=materials;

   while(matptr->name) {
      num_materials++;
      matptr++;
   }

   num_dungeons=0;
   dungptr=dungeonlist;
   while(dungptr->name) {
      num_dungeons++;
      dungptr++;
   }

   num_lairs=0;
   lairptr=lairtemplates;
   while(lairptr->sx && lairptr->sy) {
      num_lairs++;
      lairptr++;
   }

   num_rooms=0;
   lairptr=towntemplates;

   while(lairptr->sx && lairptr->sy) {
      num_rooms++;
      lairptr++;
   }

   if(CONFIGVARS.DEBUGMODE) {
      my_clrscr();
      my_printf("%d weapons\n%d artifacts\n%d armor\n%d scrolls\n", 
		num_weapons, num_artifacts, num_armors, num_scrolls);
      my_printf("%d classes\n%d races\n%d valuables\n",
		num_classes, num_npcraces, num_valuables);
      my_printf("%d lair templates\n%d general room templates\n", 
		num_lairs, num_rooms);
//		my_printf("%d armor materials\n%d item materials\n\n(more)",
//      	num_armormaterials, num_stdmaterials);
      my_printf("%d materials\n", num_materials);
      my_printf("%d shopkeeper templates\n%d dungeon specials\n", 
		num_shopkeepers, num_specials);
      showmore(false, false);
   }
}

/* count the number of characters around location
** no diagonal positions
**
*/
int8u countchars(level_type *level, int8u ctype, int16u x, int16u y )
{
   int8u count=0;

   if(level->loc[y-1][x].type == ctype) count++;
   if(level->loc[y+1][x].type == ctype) count++;
   if(level->loc[y][x-1].type == ctype) count++;
   if(level->loc[y][x+1].type == ctype) count++;
   if(level->loc[y-1][x-1].type == ctype) count++;
   if(level->loc[y+1][x+1].type == ctype) count++;
   if(level->loc[y+1][x-1].type == ctype) count++;
   if(level->loc[y-1][x+1].type == ctype) count++;

   return count;
}

/* check cave grid for passability */
bool ispassable(level_type *level, int16s x, int16s y)
{
   if(level->loc[y][x].flags & CAVE_PASSABLE)
      return true;

   return false;
}

bool haslight(level_type *level, int16s x, int16s y)
{
   bool res;

   res=false;

   /* this is permanent lighting flag */
   if(level->loc[y][x].flags & CAVE_NOLIT)
      res=false;

   /* is there TEMPORARY LIGHT like torches...*/
   if( (level->loc[y][x].flags & CAVE_TMPLIGHT) || 
       (level->loc[y][x].flags & CAVE_MONSTERLIGHT) )
      res=true;

   return res;
}

/*
** This creates up and down stairs in the level
*/
void createstairs(level_type *level)
{
   int16u tx, ty, px, py;

//   _dungeondef *dptr;

   bool doup1, doup2, dodown1, dodown2, doout;
   bool READY=false;

   /* do we need up stair (first) */
   px=py=0;

   doup1=false;
   doup2=false;
   dodown2=false;
   dodown1=false;
   doout=false;
   if(player.levptr->linkfrom1 ) doup1=true;
   if(player.levptr->linkto1 ) dodown1=true;
   if(player.levptr->linkfrom2 ) doup2=true;
   if(player.levptr->linkto2 ) dodown2=true;

   if( (player.levptr->outx && player.levptr->outy) ) doout=true;

   if(player.levptr->dtype==DTYPE_TOWN && player.dungeonlev==1) {
      doup1=false;
      doup2=false;
      doout=false;
   }

   /* up stair 1 */
   if(doup1) {
      READY=false;
      while(!READY) {
	 tx=RANDU(level->sizex);
	 ty=RANDU(level->sizey);
//      if(level->loc[ty][tx].type == TYPE_PATH) {
	 if(level->loc[ty][tx].flags & CAVE_PASSABLE) {
	    level->loc[ty][tx].type = TYPE_STAIRUP;
	    level->loc[ty][tx].flags |= terrains[TYPE_STAIRUP].flags;
	    level->loc[ty][tx].doorfl = STAIRUP1;
	    READY=true;
	 }
      }
      if(player.lastdir==STAIRDOWN1) {
	 px=tx;
	 py=ty;
      }
   }
   /* down stair */
   if(dodown1) {
      READY=false;
      while(!READY) {
	 tx=RANDU(level->sizex);
	 ty=RANDU(level->sizey);

	 if(player.levptr->dtype==DTYPE_TOWN && player.dungeonlev==1) {
	    if(level->loc[ty][tx].type==TYPE_ROOMFLOOR) {
	       level->loc[ty][tx].type = TYPE_STAIRDOWN;
	       level->loc[ty][tx].flags |= terrains[TYPE_STAIRUP].flags;
	       level->loc[ty][tx].doorfl = STAIRDOWN1;
	       READY=true;
	    }
	 }
	 else {
	    if(level->loc[ty][tx].flags & CAVE_PASSABLE) {
	       level->loc[ty][tx].type = TYPE_STAIRDOWN;
	       level->loc[ty][tx].flags |= terrains[TYPE_STAIRUP].flags;
	       level->loc[ty][tx].doorfl = STAIRDOWN1;
	       READY=true;
	    }
	 }
      }
      if(player.lastdir==STAIRUP1) {
	 px=tx;
	 py=ty;
      }
   }

   if(doup2) {
      READY=false;
      while(!READY) {
	 tx=RANDU(level->sizex);
	 ty=RANDU(level->sizey);
	 if(level->loc[ty][tx].flags & CAVE_PASSABLE) {
//      if(level->loc[ty][tx].type == TYPE_PATH) {
	    level->loc[ty][tx].type = TYPE_STAIRUP;
	    level->loc[ty][tx].flags |= terrains[TYPE_STAIRUP].flags;
//         level->loc[ty][tx].flags = CAVE_PASSABLE;
	    level->loc[ty][tx].doorfl = STAIRUP2;
	    READY=true;
	 }
      }
      if(player.lastdir==STAIRDOWN2) {
	 px=tx;
	 py=ty;
      }
   }
   /* down stair */
   if(dodown2) {
      READY=false;
      while(!READY) {
	 tx=RANDU(level->sizex);
	 ty=RANDU(level->sizey);
	 if(player.levptr->dtype==DTYPE_TOWN && player.dungeonlev==1) {
	    if(level->loc[ty][tx].type==TYPE_ROOMFLOOR) {
	       READY=true;
	       level->loc[ty][tx].type = TYPE_STAIRDOWN;
	       level->loc[ty][tx].flags |= terrains[TYPE_STAIRUP].flags;
	       level->loc[ty][tx].doorfl = STAIRDOWN2;
	    }
	 }
	 else {
	    if(level->loc[ty][tx].flags & CAVE_PASSABLE) {
	       level->loc[ty][tx].type = TYPE_STAIRDOWN;
	       level->loc[ty][tx].flags |= terrains[TYPE_STAIRUP].flags;
	       level->loc[ty][tx].doorfl = STAIRDOWN2;
	       READY=true;
	    }

	 }
      }
      if(player.lastdir==STAIRUP2) {
	 px=tx;
	 py=ty;
      }
   }

   if(doout) {
      READY=false;
      while(!READY) {
	 tx=RANDU(level->sizex);
	 ty=RANDU(level->sizey);
	 if(level->loc[ty][tx].flags & CAVE_PASSABLE) {
//      if(level->loc[ty][tx].type == TYPE_PATH) {
	    level->loc[ty][tx].type = TYPE_STAIRUP;
	    level->loc[ty][tx].flags |= terrains[TYPE_STAIRUP].flags;
//         level->loc[ty][tx].flags = CAVE_PASSABLE;
	    level->loc[ty][tx].doorfl = STAIROUT;
	    READY=true;
	 }
      }
      if(player.lastdir==STAIROUT) {
	 px=tx;
	 py=ty;
      }
   }

   player.pos_x=px;
   player.pos_y=py;
   player.lreg_x=px-4;
   player.lreg_y=py-4;
   player.spos_x=5;
   player.spos_y=5;

   if(player.pos_x<5)
      player.spos_x=player.pos_x+1;
   if(player.pos_y<5)
      player.spos_y=player.pos_y+1;

}

/*
** Removes item from level item list, remthis points to the item to be
** removed
*
*  DOES NOT CLEAR CONTAINERS!!!!!!!!!!!!!!!!!!!!!!!!!!
*
*/
#ifdef _OLDINVENTORY
item_info *level_createitem(level_type *level, int16u x, int16u y, int8u type)
{
   item_info *ptr;

   ptr=level_inititem(level);
   if(ptr) {
      switch(type) {
	 case IS_WEAPON1H:
	 case IS_WEAPON2H:
	    add_weapon(&ptr->i);
	    break;
	 case IS_ARMOR:
	    add_armor(&ptr->i);
	    break;

	 default: add_weapon(&ptr->i);
	    break;
      }
      ptr->x=x;
      ptr->y=y;

   }
   else
      return NULL;

   return ptr;
}

item_info *add_specialitem(level_type *level, int16s x, int16s y, int16s type)
{
   item_info *iptr;
   item_def *mptr;

   iptr=level_inititem(level);

   if(!iptr)
      return NULL;

   if(type>=0 && type<num_specials)
      mptr=SPECIAL_ITEMS+type;
   else
      mptr=SPECIAL_ITEMS+RANDU(num_specials);

   iptr->count=1;
   iptr->x=x;
   iptr->y=y;
   iptr->i=*mptr;

   if(iptr->i.status & ITEM_NOTPASSABLE)
      level->loc[y][x].flags &= ( 0xffff ^ CAVE_PASSABLE );

   return iptr;  
}

item_info *add_miscitem(level_type *level, int16s x, int16s y, int16s type)
{
   item_info *iptr;
   item_def *mptr;

   iptr=level_inititem(level);

   if(!iptr)
      return NULL;

   if(type>=0 && type<num_miscitems)
      mptr=miscitems+type;
   else
      mptr=miscitems+RANDU(num_miscitems);

   iptr->count=1;
   iptr->x=x;
   iptr->y=y;
   iptr->i=*mptr;

   if(iptr->i.type == IS_RING)
      set_material(&iptr->i, -1);

   if(iptr->i.type == IS_CONTAINER)
      container_init(&iptr->i);

   return iptr;

}

item_info *add_fooditem(level_type *level, int16s x, int16s y)
{
   item_info *iptr;
   item_def *mptr;

   iptr=level_inititem(level);

   if(!iptr)
      return NULL;

   while(1) {
      mptr=miscitems+RANDU(num_miscitems);

      if(mptr->type==IS_FOOD)
	 break;
   }

   iptr->count=1;
   iptr->x=x;
   iptr->y=y;
   iptr->i=*mptr;

   return iptr;
}
/*
 * Create a scroll to the level at coordinates x, y.
 *
 * If scrolltype < 0 then create a random scroll, else create a scroll
 * type as requested. (ie. SCROLL_TELEPORT)
 * item.h contains all definitions for scrolls as well as other items too
 *
 */
item_info *add_scroll(level_type *level, int16s x, int16s y, int16s scrolltype)
{
   item_info *iptr;
   int16u item;

   if(!level)
      return NULL;

   iptr=level_inititem(level);

   if(!iptr)
      return NULL;

   /* set count and coords */
   iptr->count=1;
   iptr->x=x;
   iptr->y=y;

   /* set item data from scroll template (items.cc)*/
   iptr->i=templ_scroll;

   /* item group contains scroll type number (see items.cc, items.h) */
   if(scrolltype < 0)
      item=RANDU(num_scrolls);
   else
      item=(int16u)scrolltype;

   /* check for self written scroll type */
   if(item==SCROLL_SELFWRITTEN)
      item++;

   iptr->i.group=item;

   iptr->i.pmod1=list_scroll[item].group;
   iptr->i.pmod2=list_scroll[item].spell;
   iptr->i.pmod3=list_scroll[item].skill;

   /* scroll label in "sname" */
   my_strcpy(iptr->i.sname, list_scroll[item].uname, sizeof(iptr->i.sname));

   /* scroll realname in "rname" */
   my_strcpy(iptr->i.rname, list_scroll[item].name, sizeof(iptr->i.rname));

   /* automagically identify known scrolls */
   if(list_scroll[item].flags & SCFLAG_IDENTIFIED)
      iptr->i.status |= ITEM_IDENTIFIED;

   set_alignment(&iptr->i);
   
   return iptr;
}

int16s level_add_valuables(level_type *level, int16u x, int16u y, int16s count)
{
   int16u num;

   item_info *ptr;

   ptr=level_inititem(level);

   if(!ptr)
      return -1;

   num=RANDU(num_valuables);

   if(count<=0)
      count=1+RANDU(128);

   ptr->count=count;
   ptr->x=x;
   ptr->y=y;

   my_strcpy(ptr->i.name, valuables[num].name, ITEM_NAMEMAX);

   ptr->i.type=IS_MONEY;
   ptr->i.group=num;
   ptr->i.align=NEUTRAL;
   ptr->i.material=valuables[num].material;
   ptr->i.weight=valuables[num].weight;

   return 0;
}

/*
** Create a random weapon
**
*/
void add_weapon(item_def *ptr)
{
   int16u newitem;
   item_def *itmptr;

   newitem=RANDU(num_weapons);

   itmptr=weapons+newitem;

   mem_copy(ptr, itmptr, sizeof(item_def) );

   ptr->icond=0;	// set condition as new

   set_material(ptr, -1);
}

void add_armor(item_def *ptr)
{
   int16u newitem;
   item_def *itmptr;

   newitem=RANDU(num_armors);

   itmptr=armor+newitem;

   mem_copy(ptr, itmptr, sizeof(item_def) );

   ptr->icond=0;	// set condition as new

   set_material(ptr, -1);
}

#endif

int32s get_randomitem(int16u group)
{
   item_def *igptr;
   int32u igmax;
   int32s result = -1;

   if(group == IS_FOOD || group==IS_LIGHT || group == IS_CONTAINER) {
      igptr = miscitems;
      igmax = num_miscitems;
   }
   else
      return -1;

   while(1) {
      result = RANDU(igmax);

      if((igptr+result)->type == group)
	 return result;
   }

   return result;
}

/* This function removes ALL items from the level itemlist
**
**
*/
void removeitems(level_type *level)
{
   inv_removeall(&level->inv, NULL);
   inv_init(&level->inv, NULL);

}

void showitems(level_type *level)
{
}

/*
** Removes monster from level monster list, remthis points to the monster to be
** removed
**
*/
void level_removemonster(level_type *level, _monsterlist *remthis)
{
   _monsterlist *last;
   _monsterlist *ptr;
   bool found=false;

   ptr=last=level->monsters;

   // return if no items in level
   if(!ptr) return;
   // if trying to remove NULL
   if(!remthis) return;

   while(!found) {
      if(remthis==ptr) {
	 found=true;
	 break;
      }
      if(!ptr->next) break;
      last=ptr;
      ptr=ptr->next;

   }

   // remove the requested node at this location
   if(found) {
      monster_cleanmeup(ptr);

/*
      inv_removeall(&ptr->inv);
      skill_removeall(&ptr->skills);
      cond_removeall(&ptr->conditions);
*/

      if(level->monsters==ptr)
	 level->monsters=ptr->next;
      else
	 last->next=ptr->next;

      delete ptr;
   }
}

/* init monster struct with random data */
bool monster_initrandom(_monsterlist *newptr, int8u type, bool classinit)
{
   _npcracedef *stdmon;
   int16u randrace, randclass;
   int16u i, j;
//   int16s lev;

   if(!newptr)
      return false;

   i=j=0;
   if(type) {
      stdmon=npc_races;
      while(stdmon->name) {
	 if(stdmon->out==type) {
	    j=1;
	    break;
	 }
	 i++;
	 stdmon++;
      }
   }

   if(j==1) {
      randrace=i;
      stdmon=npc_races + i;
//		randrace=0;
   }
   else {
      randrace=RANDU(num_npcraces);
      stdmon=npc_races + randrace;
   }
   randclass=1+RANDU(num_classes-1);

   newptr->m.name[0]=0;
   newptr->m.longdesc=0;
   my_strcpy(newptr->m.desc, stdmon->name, CDESC_MAX);
   newptr->m.weight=stdmon->weight;

   /* set alignment */
   if(stdmon->align==RANDALIGN)
      newptr->m.align=RANDU(LAWFUL);
   else
      newptr->m.align=stdmon->align;
   newptr->m.race=randrace;
   
   newptr->m.mclass=0;
   /* class if monster is not animal */
   if(!(stdmon->behave & BEHV_ANIMAL)) {
      newptr->m.mclass=randclass;

      random_name(newptr->m.name, CNAME_MAX);
      newptr->m.name[0]=toupper(newptr->m.name[0]);

   }
   else {
      skill_addnew(&newptr->skills, SKILLGRP_WEAPON, SKILL_HAND, 5+RANDU(20));
   }

   newptr->base_hp=npc_races[newptr->m.race].hp_base;
   newptr->sp_max=npc_races[newptr->m.race].sp_base;

   newptr->m.attitude=stdmon->attitude;
   newptr->m.behave=stdmon->behave;
   newptr->m.status=stdmon->status;

//   newptr->ac=stdmon->ac;

   /* this is now obsolete because of new statpack (see below) */
   /* done just for safety */
   newptr->m.STR=stdmon->STR;
   newptr->m.WIS=stdmon->WIS;
   newptr->m.DEX=stdmon->DEX;
   newptr->m.CON=stdmon->CON;
   newptr->m.CHA=stdmon->CHA;
   newptr->m.INT=stdmon->INT;
   newptr->m.TGH=stdmon->TGH;
   newptr->m.LUC=stdmon->LUC;
   newptr->m.SPD=stdmon->SPD;

   /* init monster gender */
   newptr->m.gender=SEX_MALE;
   /* generate sex for the creature */
   if(RANDU(100) > 50)
      newptr->m.gender=SEX_MALE;
   else
      newptr->m.gender=SEX_FEMALE;

   /* initialize stats */
   for(i=0; i<STAT_ARRAYSIZE; i++) {
      newptr->stat[i].initial=0;
      newptr->stat[i].temp=0;
      newptr->stat[i].perm=0;
      newptr->stat[i].max=99;
   }


   /* decide some level for the monster */

//   newptr->m.level=lev;

   int16s lev=0;
   lev=RANDU(4) - 2 + player.level;
   if(lev<1)
      lev=1;

   newptr->m.level=lev;

//   newptr->exp=expneeded[lev] + 1 + RANDU(200);
//   newptr->m.level=0;

   return true;
}

/* add a new (EMPTY) node to level monsterlist */
_monsterlist *monster_initnode(level_type *level)
{
   _monsterlist *newptr;

   newptr=new _monsterlist;

   // bail out if memory is out
   if(!newptr)
      return NULL;

   /* first clear all data */
   mem_clear(newptr, sizeof(_monsterlist));

   if(!level->monsters) {
      level->monsters=newptr;
      newptr->next=NULL;
   }
   else {
      newptr->next=level->monsters;
      level->monsters=newptr;
   }

   /* init monster inventory */
   inv_init(&newptr->inv, newptr->equip);   

   /* init monster pathlist */
   path_init(&newptr->path);
   
   /* set monster id number */
   newptr->id=GD.monid;
   GD.monid++;

   newptr->conditions=NULL;
   newptr->skills=NULL;
   newptr->target=NULL;
   newptr->exp=0;

   newptr->tactic=TACTIC_NORMAL;

   newptr->roomnum=-1;
   newptr->inroom=-1;

   return newptr;
}

void monster_postgeneration(level_type *level, _monsterlist *mptr)
{
   int16u i;

   for(i=0; i<HPSLOT_MAX; i++) {
      if(npc_races[mptr->m.race].bodyparts[i]==-1)
	 mptr->equip[eqslot_from_hpslot[i]].status = EQSTAT_NOLIMB;
   }

   mptr->base_hp=npc_races[mptr->m.race].hp_base;
   mptr->sp_max=npc_races[mptr->m.race].sp_base;

   mptr->exp=expneeded[mptr->m.level]+1;
   mptr->m.level=0;

   monster_checklevelraise(level, mptr, true);

   if(classes[mptr->m.mclass].initfunc!=NULL) {
      classes[mptr->m.mclass].initfunc(&mptr->skills, &mptr->inv,
					 mptr->stat, mptr->hpp);      
   }

   monster_useitems(level, mptr);

   calculate_itembonus(mptr);

}

/* Create a monster
**
**
*/
_monsterlist *monster_add(level_type *level, int16u x, int16u y, int8u type)
{
   _monsterlist *newptr;

   // create a new node
   newptr=monster_initnode(level);

   // bail out if memory is out
   if(!newptr)
      ERROROUT("-- createmonster ---\n\tOut of memory");

   monster_initrandom(newptr, type, true);

   monster_postgeneration(level, newptr);

   newptr->time=0;

   /* no target for monster */
   newptr->target=NULL;

   newptr->x=x;
   newptr->y=y;
   newptr->lastdir=0;

   return newptr;
}

/* go through the special NPC's and generate what is needed */
_monsterlist *monster_addspecial(level_type *level)
{
   _monsterlist *newptr;
   _monsterdef *monptr;
   _Tappearlist *aptr;
   int16u monnum=0;

   aptr=appearlist;
   monptr=npc_list;
   newptr=NULL;

   while( (aptr->DUNGEON!=0) && (aptr->LEVEL!=0) ) {
      if(dungeonlist[player.dungeon].index==aptr->DUNGEON && 
	 player.levptr->index==aptr->LEVEL) {
	 /* add monster */

	 /* create a new node */
	 newptr=monster_initnode(level);
	 // bail out if memory is out
	 if(!newptr)
	    ERROROUT("-- createmonster ---\n\tOut of memory");

	 /* just in case, init a random monster */
	 monster_initrandom(newptr, 0, false);

	 newptr->m=*monptr;
	 newptr->npc = newptr->m.special;

	 monster_postgeneration(level, newptr);

	 /* status flags to zero, no specials when born */
	 /* allow monster to move immendiately */
	 newptr->time=0;

	 /* no target for monster */
	 newptr->target=NULL;

	 if(aptr->X==0 || aptr->Y==0) {
	    while(1) {
	       newptr->x=5+RANDU(level->sizex-7);
	       newptr->y=5+RANDU(level->sizey-7);
	       if(ispassable(level, newptr->x, newptr->y))
		  break;
	    }
	 }
	 else {
	    newptr->x=aptr->X;
	    newptr->y=aptr->Y;
	 }
	 newptr->lastdir=0;

      }
      monptr++;
      aptr++;
      monnum++;
   }

   return newptr;
}

/* Remove all monsters contained in the level 'level' and free memory
**
*/
void monster_removeall(level_type *level)
{
   _monsterlist *ptr, *oldptr;

   /* leave if monsterlist is empty */
   if(!level->monsters) return;

   ptr=level->monsters;  
   while(ptr) {
/*
      inv_removeall(&ptr->inv);
      skill_removeall(&ptr->skills);
      cond_removeall(&ptr->conditions);
*/
      monster_cleanmeup(ptr);

      oldptr=ptr;
      ptr=ptr->next;
      delete oldptr;
   }
}

/* clean up one monster and all it's memory */
void monster_cleanmeup(_monsterlist *monster)
{
   /* remove inventory */
   inv_removeall(&monster->inv, monster->equip);

   /* remove skills */
   skill_removeall(&monster->skills);

   /* remove all special conditions */
   cond_removeall(&monster->conditions);
   
   /* clear walk path */
   path_clear(&monster->path);
}

void monster_listall(level_type *level)
{
   _monsterlist *ptr;

   ptr=level->monsters;
   while(ptr) {
      printf("%d:%d\t%s\n", ptr->x, ptr->y, monster_takename(&ptr->m));

      ptr=ptr->next;
   }
}

char *generate_scrollname(void)
{
   int16u num, j, snum=0;

   int numsyll=0;

   while(syllables[numsyll]) numsyll++;

   num=2+RANDU(6);

   j=0;
   for(int i=0; i<num; i++) {
      my_printf("%s", syllables[RANDU(numsyll)] );
      if(snum==(1+RANDU(2)) ) {
	 if(RANDU(100) < 5) 
	    my_printf("'");
	 else 
	    my_printf(" ");
	 snum=0;
      }
      snum++;
   }
   my_printf("\n");

   return NULL;
}

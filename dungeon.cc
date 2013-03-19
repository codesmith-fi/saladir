/**************************************************************************
 * dungeon.cc --                                                          *
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
#include "caves.h"
#include "gametime.h"
#include "generate.h"
#include "player.h"
#include "monster.h"
#include "weather.h"
#include "tables.h"
#include "doors.h"
#include "utility.h"
#include "search.h"
#include "status.h"
#include "traps.h"
#include "inventor.h"
#include "dungeon.h"

#include <sys/time.h>

int16u los_clearmask;
int16u los_psight;
int16u los_torchsight;
bool det_ar_natten;

/* opaque table for Line-of-Sight routine */
char opaque[102][102];

inline void disp_trap(register int16u x, register int16u y);

/*
void inline calc_point_old(level_type *level, int x, int y, int i, int j, int mid)
{
   int16s x1=0, y1=0, x2=0, y2=0, sii, sij; 
   int16u tmps;

//   if (x + i < 0 || x + i >= level->sizex || y + j < 0 || 
//       y + j >= level->sizey) {
//      return;
//   }

   int16s y_j, x_i;

   tmps=i*i+j*j;

   x1=(-1 * SIGN(i));
   y1=(-1 * SIGN(j));

   if(abs(j)>abs(i)) {
      x2=0;
      y2=(-1*SIGN(j));
   } else if(abs(j) < abs(i)) {
      x2=(-1*SIGN(i));
      y2=0;
   } else {
      x2=x1;
      y2=y1;
   }

   y_j=y+j;
   x_i=x+i;

   if((x_i) < 0 || (x_i) > level->sizex-1) i=0;
   if((y_j) < 0 || (y_j) > level->sizey-1) j=0;

   if((player.spos_x+(x_i - player.pos_x)) < 1 ||
      (player.spos_x+(x_i - player.pos_x)) > MAPWIN_SIZEX) return;
   if((player.spos_y+(y_j - player.pos_y)) < 1 ||
      (player.spos_y+(y_j - player.pos_y)) > MAPWIN_SIZEY) return;

   if((opaque[i+x1+mid][j+y1+mid] + opaque[i+x2+mid][j+y2+mid] >=2)) {
      level->loc[y_j][x_i].flags &= los_clearmask;

      my_gotoxy(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		MAPWIN_RELY+player.spos_y+(y_j - player.pos_y));

      output_type(level->loc[y_j][x_i].type, 
		  level->loc[y_j][x_i].flags);

      opaque[i+mid][j+mid] = 1;
   }
   else if(tmps <= los_psight) {     
      if(tmps <= los_torchsight)
	 level->loc[y_j][x_i].flags |= (CAVE_TMPLIGHT|CAVE_SEEN|CAVE_LOSTMP);
      else {
	 level->loc[y_j][x_i].flags &= los_clearmask;

	 if(level->loc[y_j][x_i].flags & CAVE_MONSTERLIGHT) 
	    level->loc[y_j][x_i].flags |= CAVE_TMPLIGHT;


	 if(level->loc[y_j][x_i].flags & CAVE_TMPLIGHT) 
	    level->loc[y_j][x_i].flags |= (CAVE_SEEN|CAVE_LOSTMP|CAVE_TMPLIGHT);

      }
      
      if( level->loc[y_j][x_i].flags & CAVE_PASSABLE )
	 opaque[i+mid][j+mid] = 0;
      else
	 opaque[i+mid][j+mid] = 1;

      my_gotoxy(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		MAPWIN_RELY+player.spos_y+(y_j - player.pos_y));
      output_type(level->loc[y_j][x_i].type, 
		  level->loc[y_j][x_i].flags);
   }
   else {
      level->loc[y_j][x_i].flags &= (los_clearmask | CAVE_MONSTERLIGHT);

      my_gotoxy(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		MAPWIN_RELY+player.spos_y+(y_j - player.pos_y));

      output_type(level->loc[y_j][x_i].type, 
		  level->loc[y_j][x_i].flags);
   }
}
*/

inline void calc_point(level_type *level, int x, int y, int i, int j, int mid)
{
   int16s x1=0, y1=0, x2=0, y2=0; 
   int16u tmps;

   int16s y_j, x_i;

   /* for radius check */
   tmps=i*i+j*j;

   x1=-1 * SIGN(i);
   y1=-1 * SIGN(j);

   if(ABS(j)>ABS(i)) {
      x2=0;
      y2=-1*SIGN(j);
   } else if(ABS(j) < ABS(i)) {
      x2=-1*SIGN(i);
      y2=0;
   } else {
      x2=x1;
      y2=y1;
   }

   y_j=y+j;
   x_i=x+i;

   if((x_i) < 0 || (x_i) > level->sizex-1) i=0;
   if((y_j) < 0 || (y_j) > level->sizey-1) j=0;

   if((player.spos_x+(x_i - player.pos_x)) < 1 ||
      (player.spos_x+(x_i - player.pos_x)) > MAPWIN_SIZEX) return;
   if((player.spos_y+(y_j - player.pos_y)) < 1 ||
      (player.spos_y+(y_j - player.pos_y)) > MAPWIN_SIZEY) return;

   if((opaque[i+x1+mid][j+y1+mid] + opaque[i+x2+mid][j+y2+mid] >=2)) {
      /* not visible grid */      
      level->loc[y_j][x_i].flags &= los_clearmask;
      opaque[i+mid][j+mid] = 1;

      if(det_ar_natten)
	 my_setcolor(CH_DGRAY);
      else
	 my_setcolor(terrains[level->loc[y_j][x_i].type].color);

      /* if location is seen, print it with darker color */
      if( level->loc[y_j][x_i].flags & CAVE_SEEN) {
	 my_mvaddch(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		    MAPWIN_RELY+player.spos_y+(y_j - player.pos_y),
		    terrains[level->loc[y_j][x_i].type].out);

      }
      else {
	 my_mvaddch(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		    MAPWIN_RELY+player.spos_y+(y_j - player.pos_y),
		    ' ');
      }
   }
   /* if the area is in players sight */
   else if(tmps < los_psight) {     
      if( level->loc[y_j][x_i].flags & CAVE_PASSABLE )
	 opaque[i+mid][j+mid] = 0;
      else
	 opaque[i+mid][j+mid] = 1;

      /* if the area is in the torch radius or lit by the monster*/
      if(tmps < los_torchsight ||
	 level->loc[y_j][x_i].flags & CAVE_MONSTERLIGHT) {

	 if(level->loc[y_j][x_i].flags & CAVE_NOLIT)
	    level->loc[y_j][x_i].flags |= CAVE_LOSTMP;
	 else
	    level->loc[y_j][x_i].flags |= (CAVE_SEEN|CAVE_LOSTMP);

	 /* display in bright yellow? */
//	 if(CONFIGVARS.showlight)
//	    my_setcolor(CH_YELLOW);
//	 else
	 if(level->loc[y_j][x_i].flags & CAVE_TRAPIDENT) {
	    my_gotoxy(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		      MAPWIN_RELY+player.spos_y+(y_j - player.pos_y));
	    disp_trap(x_i, y_j);
	 }
	 else {
	    my_setcolor(terrains[level->loc[y_j][x_i].type].color);
	    my_mvaddch(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		       MAPWIN_RELY+player.spos_y+(y_j - player.pos_y),
		       terrains[level->loc[y_j][x_i].type].out);
	 }
      }
      /* if not lit by torch or monster*/
      else {
	 level->loc[y_j][x_i].flags &= los_clearmask;
//	 level->loc[y_j][x_i].flags |= CAVE_PLRSEEGRID;

	 if(det_ar_natten)
	    my_setcolor(CH_DGRAY);
	 else
	    my_setcolor(terrains[level->loc[y_j][x_i].type].color);

	 if( level->loc[y_j][x_i].flags & CAVE_SEEN)
	    my_mvaddch(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		       MAPWIN_RELY+player.spos_y+(y_j - player.pos_y),
		       terrains[level->loc[y_j][x_i].type].out);
	 else
	    my_mvaddch(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		       MAPWIN_RELY+player.spos_y+(y_j - player.pos_y),
		       ' ');

      }
           
   }
   /* if not in the sight of the player */
   else {
      level->loc[y_j][x_i].flags &= los_clearmask;

      if(det_ar_natten)
	 my_setcolor(CH_DGRAY);
      else
	 my_setcolor(terrains[level->loc[y_j][x_i].type].color);

      if( level->loc[y_j][x_i].flags & CAVE_SEEN)
	 my_mvaddch(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		    MAPWIN_RELY+player.spos_y+(y_j - player.pos_y),
		    terrains[level->loc[y_j][x_i].type].out);
      else
	 my_mvaddch(MAPWIN_RELX+player.spos_x+(x_i - player.pos_x), 
		    MAPWIN_RELY+player.spos_y+(y_j - player.pos_y),
		    ' ');

   }
}

/* see with LoS at location pos_x, pos_y */
void player_los_old(level_type *level, int16s pos_x, int16s pos_y)
{
   int i, j, dist, mid;
   int px, py;

   int16u ltmp;

//   px=player.pos_x;
//   py=player.pos_y;
   py=pos_y;
   px=pos_x;

   ltmp=player.light;

   det_ar_natten=worldtime.is_night();

   /* night is always present in the dungeons */
   if(player.dungeon!=0 && player.levptr->dtype!=DTYPE_TOWN)
      det_ar_natten=true;

   if(player.light >= player.sight)
      player.light-=2;

//   if(worldtime.is_night() ||
//       (player.dungeon!=0 && player.levptr->dtype!=DTYPE_TOWN))
   if(det_ar_natten)
      los_clearmask= 0xffff ^ ( CAVE_TMPLIGHT | CAVE_LOSTMP);
   else {
      los_clearmask= 0xffff ^ ( CAVE_LOSTMP);

      /* when it's day, player light radius is the sight radius */
      player.light=player.sight;
   }

   los_psight=player.sight*player.sight;  
   los_torchsight=player.light*player.light;

   mid=50;

   opaque[mid][mid]=0;

   if(player.light) {
      if(level->loc[py][px].flags & CAVE_NOLIT)
	 level->loc[py][px].flags |= (CAVE_TMPLIGHT|CAVE_LOSTMP);
      else
	 level->loc[py][px].flags |= (CAVE_TMPLIGHT|CAVE_SEEN|CAVE_LOSTMP);
   }

   my_setcolor(CH_RED);
   for(dist = 1; dist<=player.sight+2; dist++) {
      for(i=-dist; i<=dist; i++) {
	 for(j=-dist; j<=dist; j+=(2*dist)) {
	    calc_point(level, px, py, i, j, mid);

	 }
      }
      for(j=-dist+1; j <dist; j++) {
	 for(i=-dist; i<=dist; i+=(2*dist) ) {
	    calc_point(level, px, py, i, j, mid);
	 }
      }
   }

   /* restore light */
   player.light=ltmp;

}

inline void calc_point_monster(level_type *level, int x, int y, int i, int j, int mid)
{
   register int x1=0, y1=0, x2=0, y2=0; 
   int16u tmps;

   if (x + i < 0 || x + i >= level->sizex || y + j < 0 || 
       y + j >= level->sizey) {
      return;
   }

   int16s y_j, x_i;

   /* for radius check */
   tmps=i*i+j*j;

   x1=(-1 * SIGN(i));
   y1=(-1 * SIGN(j));

   if(ABS(j)>ABS(i)) {
      x2=0;
      y2=(-1*SIGN(j));
   } else if(ABS(j) < ABS(i)) {
      x2=(-1*SIGN(i));
      y2=0;
   } else {
      x2=x1;
      y2=y1;
   }

   y_j=y+j;
   x_i=x+i;

   if((x_i) < 0 || (x_i) > level->sizex-1) i=0;
   if((y_j) < 0 || (y_j) > level->sizey-1) j=0;

   if((opaque[i+x1+mid][j+y1+mid] + opaque[i+x2+mid][j+y2+mid] >=2)) {
      /* not visible grid */      
      /* clear flags */
      //    level->loc[y_j][x_i].flags &= los_clearmask;

      opaque[i+mid][j+mid] = 1;
   }
   else if(tmps <= los_torchsight) {     
      if( (level->loc[y_j][x_i].flags & CAVE_PASSABLE) ) {
	 level->loc[y_j][x_i].flags |= CAVE_MONSTERLIGHT;
	 opaque[i+mid][j+mid] = 0;
      }
      else
	 opaque[i+mid][j+mid] = 1;
   }
//   else {
//      level->loc[y_j][x_i].flags &= los_clearmask;
//   }
}

void monster_torchlos(level_type *level, _monsterlist *mptr)
{
   int i, j, dist, mid;
   int px, py;

   py=mptr->y;
   px=mptr->x;

   los_clearmask= 0xffff ^ CAVE_MONSTERLIGHT;
   los_torchsight=mptr->light*mptr->light;

   mid=50;
   opaque[mid][mid]=0;
   level->loc[py][px].flags |= CAVE_MONSTERLIGHT;

   for(dist = 1; dist<=mptr->light; dist++) {
      for(i=-dist; i<=dist; i++) {
         for(j=-dist; j<=dist; j+=(2*dist)) {
	    calc_point_monster(level, px, py, i, j, mid);
         }
      }
      for(j=-dist+1; j <dist; j++) {
	 for(i=-dist; i<=dist; i+=(2*dist) ) {
	    calc_point_monster(level, px, py, i, j, mid);
         }
      }
   }
}


// This routine will be called during line of sight check
// it will print all visible items on screen
inline bool output_items(level_type *level, int16u x, int16u y)
{
   Tinvpointer ptr;

   ptr=level->inv.first;

   while(ptr) {
      if(x==ptr->x && y==ptr->y) {	 
	 if(ptr->i.material==MAT_FABRIC || ptr->i.type==IS_SPECIAL)
	    my_setcolor( ptr->i.pmod4 );
	 else
	    my_setcolor( materials[ptr->i.material].color);

	 if(ptr->i.type==IS_SPECIAL)
	    my_putch( ptr->i.pmod3 );
	 else
	    my_putch( gategories[ptr->i.type].out );

	 return true;
      }
      ptr=ptr->next;
   }
   return false;
}

bool output_monsters(level_type *level, int16u x, int16u y)
{
   _monsterlist *ptr;

   ptr=level->monsters;

   while(ptr) {
      if(x==ptr->x && y==ptr->y) {
	 my_setcolor( npc_races[ptr->m.race].color );
	 my_putch( npc_races[ptr->m.race].out);
	 return true;
      }
      ptr=ptr->next;
   }
   return false;
}


/* NEW ! FAST ! */
bool new_output_items(level_type *level)
{
   register Tinvpointer ptr;
   register int16s px, py;
   bool item=false;

   px=player.spos_x+MAPWIN_RELX;
   py=player.spos_y+MAPWIN_RELY;

   ptr=level->inv.first;

   while(ptr) {
      if( level->loc[ptr->y][ptr->x].flags & CAVE_LOSTMP) {
//	 if(ptr->i.material!=MAT_FABRIC)
//	 else
//	    my_setcolor( ptr->i.pmod4 );

	 if(ptr->i.type==IS_SPECIAL) {
	    my_setcolor( item_outfaces[ptr->i.group].color );
	    my_mvaddch(px+(ptr->x-player.pos_x), py+(ptr->y - player.pos_y),
		       item_outfaces[ptr->i.group].out );	    
	 }
	 else {
	    my_setcolor( materials[ptr->i.material].color);
	    my_mvaddch(px+(ptr->x-player.pos_x), py+(ptr->y - player.pos_y),
		       gategories[ptr->i.type].out );
	 }
//	 my_gotoxy(px+(ptr->x-player.pos_x), py+(ptr->y - player.pos_y));
//	 my_putch( gategories[ptr->i.type].out );
	 item=true;
      }
      ptr=ptr->next;
   }

   return item;
}

/* NEW ! FAST ! */
bool new_output_monsters(level_type *level)
{
   register _monsterlist *ptr;
   register int16s px, py;
   bool monster=false;


   px=player.spos_x+MAPWIN_RELX;
   py=player.spos_y+MAPWIN_RELY;

   ptr=level->monsters;

   while(ptr) {
      if( level->loc[ptr->y][ptr->x].flags & CAVE_LOSTMP) {
	 my_setcolor( npc_races[ptr->m.race].color );

	 my_mvaddch(px+(ptr->x-player.pos_x), 
		    py+(ptr->y - player.pos_y),
		    npc_races[ptr->m.race].out);
	 monster=true;
      } 
/*
      else if(level->loc[ptr->y][ptr->x].flags & CAVE_PLRSEEGRID) {
	 my_setcolor( CH_DGRAY );
	 my_mvaddch(px+(ptr->x-player.pos_x), 
		    py+(ptr->y - player.pos_y),
		    '?');
      }
*/
      ptr=ptr->next;
   }
   return monster;
}


#define MAX_LOS      20
#define MAX_LOS_AREA 40   // MAX_LOS*2

void player_los(level_type *level)
{
   player_los_old(level, player.pos_x, player.pos_y);

   player.iteminsight=new_output_items(level);

   player.monsterinsight=new_output_monsters(level);

   /* now display items and monsters at visible coordinates */
}

inline void disp_trap(register int16u x, register int16u y)
{
   if(isitadoor(c_level, x, y)) {
      my_setcolor(CH_RED);
      my_addch(terrains[c_level->loc[y][x].type].out);
   }
   else {
      my_setcolor(list_traps[c_level->loc[y][x].trap].color);
      my_addch(list_traps[c_level->loc[y][x].trap].out);
   }

}


void do_redraw(level_type *level)
{
   GAME_NOTIFYFLAGS|=GAME_SHOWALLSTATS;

   my_clrscr();
   msg.update(true);
 
   printmaze(level, player.lreg_x, player.lreg_y);
 
   showplayer(level);
   showstatus(false);

   GAME_NOTIFYFLAGS = 0;

}

int16s printmaze(level_type *level, int16s wx, int16s wy)
{
   int16s res=0;
   int16u tx=0, ty=0;
   int16u clearmask;

   player.lreg_x=wx;
   player.lreg_y=wy;

   clearmask=0xffff ^ ( CAVE_LOSTMP | CAVE_TMPLIGHT | CAVE_MONSTERLIGHT);
//   if(( player.dungeon==0 || player.levptr->dtype==DTYPE_TOWN) ) {
      /* clear all temp flags */
   for(ty=0; ty<level->sizey; ty++) {
      for(tx=0; tx<level->sizex; tx++) {
	 level->loc[ty][tx].flags &= clearmask;
	 if(!worldtime.is_night() &&
	    ( player.dungeon==0 || player.levptr->dtype==DTYPE_TOWN) )
	    level->loc[ty][tx].flags |= CAVE_TMPLIGHT;
      }
   }
//   }

   if(wx > (level->sizex - MAPWIN_SIZEX)) {
      wx=res=level->sizex - MAPWIN_SIZEX;
      player.spos_x=player.pos_x-wx+1;
      player.lreg_x=wx;
   }
   if(wy > (level->sizey - MAPWIN_SIZEY)) {
      wy=res=level->sizey - MAPWIN_SIZEY;
      player.spos_y=player.pos_y-wy+1;
      player.lreg_y=wy;
   }
   if(wy < 0) {
      player.spos_y=player.pos_y+1;
      wy=player.lreg_y=0;
   }
   if(wx < 0) {
      player.spos_x=player.pos_x+1;
      wx=player.lreg_x=0;
   }

//   hidecursor();
   for(ty=0; ty<MAPWIN_SIZEY; ty++) {
      if( (ty+wy) >= level->sizey )
	 break;
      my_gotoxy(MAPWIN_X, MAPWIN_Y+ty);

      for(tx=0; tx<MAPWIN_SIZEX; tx++) {
	 if( (tx+wx) >= level->sizex )
	    break;
	 output_type(level->loc[ty+wy][tx+wx].type, 
		     level->loc[ty+wy][tx+wx].flags);
      }
   }
//   showcursor();

   return res;
}

inline void output_type(int16u ctype, int16u cflags)
{
   if( (cflags & CAVE_SEEN)  ) {
      if( cflags & CAVE_LOSTMP) {
	 if((cflags & CAVE_TMPLIGHT) || (cflags & CAVE_MONSTERLIGHT)) {
	    my_setcolor(terrains[ctype].color);
	    if(CONFIGVARS.inversesight) 
	       attrset(A_REVERSE);
	    my_putch(terrains[ctype].out);
//	    my_cputch(terrains[ctype].out, terrains[ctype].color);
	 }
	 else {
	    if( cflags & CAVE_NOLIT ) {
//	       my_cputch( terrains[0].out, terrains[0].color );
	       my_putch(' ');
	    }
	    else {
	       my_setcolor(CH_DGRAY);
	       my_putch(terrains[ctype].out);
//	       my_cputch(terrains[ctype].out, CH_DGRAY);
	    }
	 }
      
      }
      else {
	 if(!(cflags & CAVE_NOLIT ) ) {
	    if(cflags & CAVE_TMPLIGHT )
	       my_cputch(terrains[ctype].out, terrains[ctype].color);
	    else
	       my_cputch(terrains[ctype].out, CH_DGRAY);
	 }
	 else 
	    my_cputch(terrains[0].out, terrains[0].color);
      }
   }
   else {
      my_cputch( terrains[0].out, terrains[0].color );
   }


}

/* counts all directions for passable directions */
int8u sur_countall(level_type *level, int16u x, int16u y)
{
     int8u count=0;

     if(level->loc[y-1][x].flags & CAVE_PASSABLE) count++;
     if(level->loc[y+1][x].flags & CAVE_PASSABLE) count++;
     if(level->loc[y][x-1].flags & CAVE_PASSABLE) count++;
     if(level->loc[y][x+1].flags & CAVE_PASSABLE) count++;
     if(level->loc[y-1][x-1].flags & CAVE_PASSABLE) count++;
     if(level->loc[y+1][x+1].flags & CAVE_PASSABLE) count++;
     if(level->loc[y+1][x-1].flags & CAVE_PASSABLE) count++;
     if(level->loc[y-1][x+1].flags & CAVE_PASSABLE) count++;

     return count;
}

// counts passable horizontal and vertical directions
// around given location
int8u sur_countpass(level_type *level, int16u x, int16u y)
{
   int8u count=0;

   if(level->loc[y-1][x].flags & CAVE_PASSABLE) count++;
   if(level->loc[y+1][x].flags & CAVE_PASSABLE) count++;
   if(level->loc[y][x-1].flags & CAVE_PASSABLE) count++;
   if(level->loc[y][x+1].flags & CAVE_PASSABLE) count++;

   if(CONFIGVARS.door_disturb) {
      if(level->loc[y-1][x].type == TYPE_DOORCLOS ||
	 level->loc[y-1][x].type == TYPE_DOOROPEN )
	 count++;
      if(level->loc[y+1][x].type == TYPE_DOORCLOS ||
	 level->loc[y+1][x].type == TYPE_DOOROPEN )
	 count++;
      if(level->loc[y][x-1].type == TYPE_DOORCLOS ||
	 level->loc[y][x-1].type == TYPE_DOOROPEN )
	 count++;
      if(level->loc[y][x+1].type == TYPE_DOORCLOS ||
	 level->loc[y][x+1].type == TYPE_DOOROPEN )
	 count++;
   }

   return count;
}

// counts passable diagonal directions
// around given location
int8u sur_countpass_dia(level_type *level, int16u x, int16u y)
{
   int8u count=0;

   if(level->loc[y-1][x-1].flags & CAVE_PASSABLE) count++;
   if(level->loc[y+1][x+1].flags & CAVE_PASSABLE) count++;
   if(level->loc[y+1][x-1].flags & CAVE_PASSABLE) count++;
   if(level->loc[y-1][x+1].flags & CAVE_PASSABLE) count++;

   if(CONFIGVARS.door_disturb) {
      if(level->loc[y-1][x-1].type == TYPE_DOORCLOS ||
	 level->loc[y-1][x-1].type == TYPE_DOOROPEN )
	 count++;
      if(level->loc[y+1][x+1].type == TYPE_DOORCLOS ||
	 level->loc[y+1][x+1].type == TYPE_DOOROPEN )
	 count++;
      if(level->loc[y+1][x-1].type == TYPE_DOORCLOS ||
	 level->loc[y+1][x-1].type == TYPE_DOOROPEN )
	 count++;
      if(level->loc[y-1][x+1].type == TYPE_DOORCLOS ||
	 level->loc[y-1][x+1].type == TYPE_DOOROPEN )
	 count++;
   }

   return count;
}


// print the player on screen
void showplayer(level_type *level)
{
   player_los(level);

   my_setcolor(C_WHITE);
   my_gotoxy(1, 3);
   my_printf("%-15s", 
	     terrains[level->loc[player.pos_y][player.pos_x].type].desc);

   my_gotoxy(MAPWIN_RELX+player.spos_x, MAPWIN_RELY+player.spos_y);
   my_cputch('@', player.color);
   my_gotoxy(MAPWIN_RELX+player.spos_x, MAPWIN_RELY+player.spos_y);  

   my_refresh();

}

void show_at(level_type *level, int16s x, int16s y) 
{
   player_los_old(level, x, y);
   player.iteminsight=new_output_items(level);
   player.monsterinsight=new_output_monsters(level);

   my_setcolor(C_WHITE);
   my_gotoxy(1, 3);
   my_printf("%-15s", 
	     terrains[level->loc[player.pos_y][player.pos_x].type].desc);

   my_gotoxy(MAPWIN_RELX+player.spos_x, MAPWIN_RELY+player.spos_y);
   my_cputch('@', player.color);
   my_gotoxy(MAPWIN_RELX+player.spos_x, MAPWIN_RELY+player.spos_y);  

   my_refresh();

}

void noticestuff(level_type *level)
{
   Tinvpointer ptr=NULL;
   int32u a;

   bool light;

   light=haslight(level, player.pos_x, player.pos_y);

   if(!light)
      if(player.light==0)
	 light=false;
      else
	 light=true;

   if(!light) {
      if(RANDU(100)>80) {
	 msg.newmsg("It's really dark here.", C_WHITE);
      }
   }

   if(level->loc[player.pos_y][player.pos_x].type==TYPE_STAIRUP) {
      if(light)
	 msg.newmsg("You see stairs leading up!", C_GREEN);
      else
	 msg.newmsg("You feel a staircase here.", C_GREEN);
   }
   else
      if( level->loc[player.pos_y][player.pos_x].type==TYPE_STAIRDOWN) {
	 if(light)
	    msg.newmsg("You see stairs leading down!", C_GREEN);
	 else
	    msg.newmsg("You feel a staircase here.", C_GREEN);
      }

   a=inv_countitems(&level->inv, -1, player.pos_x, player.pos_y, false);
   if(light) {
      if( a>1 )
	 msg.newmsg("Several items are here!", C_WHITE);
      else if( a==1) {
	 /* do autopickup if required */
	 ptr=giveiteminfo(level, player.pos_x, player.pos_y);
	 item_printinfo(&ptr->i, ptr->i.weight, ptr->count, "You see here");

	 if(CONFIGVARS.autopickup) {
	    player.repeatwalk=false;
	    player_autopickup(level);
	 }
      }

   }
   else {
      if(a) {
	 msg.newmsg("Feels like some items are here.", C_WHITE);
      }
   }

   /* activate traps */
   if(handletrap(level, player.pos_x, player.pos_y, NULL))
      player.repeatwalk=false;

}

void repeatmove(int8u dir, level_type *level)
{
   int8u numdir=0, oldnumdir, movres;
   bool END_REPEAT=false;

   int16u px, py, repeats=0;
   player.repeatwalk=true;

   if(CONFIGVARS.foodwarn && player.nutr==FOOD_FAINTING) {
      msg.newmsg("Not now, you're starving!", C_RED);
      return;
   }

   if(CONFIGVARS.monster_disturb && player.monsterinsight) {
      msg.newmsg("Not now, there're possible hostile monsters around",
		 C_RED);
      return;
   }

   numdir=sur_countpass(level, player.pos_x+move_dx[dir], player.pos_y+move_dy[dir]);

   movres=moveplayer(dir, level, true);
   if(movres) {
      game_passturn(level, CONFIGVARS.repeatupdate, true);
   }
   else
      return;

   while(!END_REPEAT) {

      px=player.pos_x; //+move_dx[dir];
      py=player.pos_y; //+move_dy[dir];

      /* get new surrounding conditions */
      oldnumdir=numdir;
      numdir=sur_countpass(level, px, py);

      /* if surrounding conditions changed */
      if(numdir!=oldnumdir) 
	 break;

      /* if repeat walk somehow got disabled */
      if(!player.repeatwalk) 
	 break;

      /* if walked into an item */
      if(CONFIGVARS.item_disturb)
	 if(inv_countitems(&level->inv, -1, px, py, false)) 
	    break;

      /* if walked into a stair */
      if(CONFIGVARS.stair_disturb)
	 if(level->loc[py][px].type==TYPE_STAIRUP || 
	    level->loc[py][px].type==TYPE_STAIRDOWN ) 
	    break;

      /* if monster in vision range */
      if(CONFIGVARS.monster_disturb && player.monsterinsight) {
	 break;
      }

      /* if repeats exeeded */
      if(repeats>CONFIGVARS.repeatcount) 
	 break;

      /* if still alive, move on */
      movres=moveplayer(dir, level, true);

      game_passturn(level, CONFIGVARS.repeatupdate, true);

      px=player.pos_x;
      py=player.pos_y;

      /* walk corridors */
      if(movres) {
//	 END_REPEAT=true;
	 if((dir==DIR_LEFT) || (dir==DIR_RIGHT) || 
	    (dir==DIR_UP) || (dir==DIR_DOWN)) {
	    if(sur_countpass(level, px, py) == 2) {
	       if( (level->loc[py-1][px].flags & CAVE_PASSABLE) && 
		   dir!=DIR_DOWN) dir=DIR_UP;
	       else if( (level->loc[py][px-1].flags & CAVE_PASSABLE) && 
			dir!=DIR_RIGHT) dir=DIR_LEFT;
	       else if( (level->loc[py+1][px].flags & CAVE_PASSABLE) && 
			dir!=DIR_UP) dir=DIR_DOWN;
	       else if( (level->loc[py][px+1].flags & CAVE_PASSABLE) && 
			dir!=DIR_LEFT) dir=DIR_RIGHT;
	       END_REPEAT=false;
	    }
	    else
	       END_REPEAT=false;
	 }
//	 END_REPEAT=false;
      }
      else
	 break;

//      if((dir==DIR_LEFT) || (dir==DIR_RIGHT) || 
//	 (dir==DIR_UP) || (dir==DIR_DOWN))


      repeats++;

   }
   player.repeatwalk=false;
}


int16s checkterraineffects(level_type *level, int16s x, int16s y)
{
   /* basic movement time */
   int16s timeadd=TIME_MOVEAROUND,
      rmod;

   real rtime=0;

   /* get the time effect from terrainslist */
   rmod=terrains[level->loc[y][x].type].timemod;

   if(level->loc[y][x].flags & CAVE_WATER) {
      msg.newmsg("You can't swim.", C_WHITE);
      return 0;
   }
   
   if(level->loc[y][x].flags & CAVE_MOUNTAIN) {
      msg.newmsg("Impossible, look how high the mountain is.", C_WHITE);
      return 0;
   }

   if(!(level->loc[y][x].flags & CAVE_PASSABLE)) {
      if(!editmode)
	 return -1;
   }


   if(rmod!=100) {
      if(rmod<1 || rmod>500)
	 rmod=100;

      rtime=(TIME_MOVEAROUND / 100) * rmod;

      timeadd=(int16s)rtime;

      if(RANDU(100)<50) {
	 if(rmod>=400) {
	    msg.newmsg("This is almost impossible walk on.", C_YELLOW);
	 }
	 else if(rmod>=150) {
	    msg.newmsg("This is really slow to walk on.", C_YELLOW);
	 }
	 else if( rmod > 100) {
	    msg.newmsg("This terrain slows you down.", C_YELLOW);
	 }

	 if(rmod<20)
	    msg.newmsg("Feels like flying.", C_YELLOW);
	 else if(rmod<50)
	    msg.newmsg("This terrain is really easy to walk on.", C_YELLOW);
	 else if(rmod<100)
	    msg.newmsg("It's easy to walk on this..", C_YELLOW);
      }
   }
   else
      timeadd=TIME_MOVEAROUND;


   return timeadd;
}

/*
** handles player moves
** value for 'dir is from 0 to 9. Direction of move is actually
** taken from tables move_dx[] and move_dy[]
*/
int8u moveplayer(int8u dir, level_type *level, bool autowalk)
{
   int16u ox, oy, sx, sy;
   int16s tmp1, nx, ny, temptime;
   bool pushres;
   ox=player.pos_x;
   oy=player.pos_y;

   nx=player.pos_x;
   ny=player.pos_y;
   sx=player.spos_x;
   sy=player.spos_y;

   _monsterlist *mptr;

   if(cond_check(player.conditions, CONDIT_CONFUSED) > 0)
      dir=1+RANDU(9);

   nx+=move_dx[dir];
   sx+=move_dx[dir];
   ny+=move_dy[dir];
   sy+=move_dy[dir];

   if(nx>=level->sizex || nx<0 || ny>=level->sizey || ny<0) {
      msg.newmsg("Are you going to the void?", C_RED);
      msg.newmsg("I wouldn't mind but it would cause some nasty pagefaults!",
		 C_RED);
      msg.newmsg("So leave it!", C_RED);
      return 0;
   }

   if(player.levptr->dtype==DTYPE_TOWN ) {
      if(nx <= 2 || nx>level->sizex-3 || ny <=2 || ny>level->sizey-3) {
	 sprintf(tempstr, "Do you want to leave %s", player.levptr->name);
	 if(confirm_yn(tempstr, false, true)) {
	    player.lastdir=STAIROUT;
	    player_go_up(&c_level);
	 }
	 return 0;
      }
   }

   if(player.huntmode) {
      if(nx <= 2 || nx>level->sizex-3 || ny <=2 || ny>level->sizey-3) {
	 if(confirm_yn("Leave this encounter", false, true)) {
	    player_go_outworld(&c_level);
	 }
	 return 0;
      }
   }

   mptr=istheremonster(level, nx, ny);

   if(mptr) {
      player.repeatwalk=false;
      
      if(!autowalk)
	 player_meleeattack(level, mptr);

      return 0;
   }
   else if(cond_check(player.conditions, CONDIT_BADLEGS) > 0) {
//   else if(player.hpp[HPSLOT_LEGS].cur<=0) {
      msg.newmsg("You can't walk now.", C_RED);
      msg.newmsg("Your legs are badly injured!", C_RED);
      player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), 
				  800, player.tactic);
      return 0;
   }

   temptime=checkterraineffects(level, nx, ny);

   /* issue automatic searching skill */
   if(player.searchmode) {
      search_surroundings(level, NULL, true);
   }
    
   if(temptime>0 || editmode) {
//	 return 1;

      /* substract moving time */
      player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), temptime, player.tactic);

      if(player.searchmode) 
	 player.timetaken -= TIME_AUTOSEARCH;

      if(player.hitwall > -1) player.hitwall-=1;
      if(nx<0 || ny<0 || ny==level->sizey || nx==level->sizex)
	 return 0;
      player.pos_x=nx;
      player.pos_y=ny;

//      my_gotoxy(player.spos_x+MAPWIN_RELX, player.spos_y+MAPWIN_RELY);
//      output_type(level->loc[oy][ox].type, level->loc[oy][ox].flags, false, false);

      player.spos_x=sx;
      player.spos_y=sy;

      if(level->sizey > MAPWIN_SIZEY) {
	 if(player.spos_y > MAPWIN_SIZEY-4 && player.lreg_y < level->sizey-MAPWIN_SIZEY)
	 {
	    player.spos_y=5;
	    tmp1=printmaze(level, player.lreg_x, player.pos_y-4);
	 }
	 else if(player.spos_y < 5 && player.lreg_y>0 )
	 {
	    player.spos_y=MAPWIN_SIZEY-4;
	    printmaze(level, player.lreg_x, player.pos_y-MAPWIN_SIZEY+5);
	 }
      }
      if(level->sizex > MAPWIN_SIZEX) {
	 if(player.spos_x > MAPWIN_SIZEX-4)
	 {
	    player.spos_x=5;
	    tmp1=printmaze(level, player.pos_x-4, player.lreg_y);
	 }
	 else if(player.spos_x < 5 && player.lreg_x)
	 {
	    player.spos_x=MAPWIN_SIZEX-4;
	    printmaze(level, player.pos_x-MAPWIN_SIZEX+5, player.lreg_y);
	 }
      }

      noticestuff(level);
   }
   else {
      int8u dstat=DOORSTAT_NODOOR;
      
      if(autowalk)
	 return 0;

      /* automatic doors ? */
      if(CONFIGVARS.autodoor) {
	 if(isitadoor(level, nx, ny)) {
	    if(confirm_yn("There's a door, open it", true, true))
	       dstat=door_open(level, nx, ny, NULL);
	 }
      }

      if(dstat==DOORSTAT_NODOOR) {
	 pushres=false;
	 if(CONFIGVARS.autopush) 
	    pushres=player_push(level, dir);

	 if(!pushres && !player.repeatwalk && temptime<0)
	    player_hitwall(level);
      }
      else {
	 player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), 
				  TIME_OPENDOOR, player.tactic);

      }
      return 1;
   }

   return 1;	// successible move
}

// returns the distance between 2 points
int16u distance(int16s x1, int16s y1, int16s x2, int16s y2)
{
     return (int16u)sqrt( (x2-x1)*(x2-x1) + (y2-y1) * (y2-y1) );
}

int16s show_statcolor(int8u wy, char *txt, _statpack *pstat)
{
   int16s result;

   result=get_stat(pstat);

   my_gotoxy(1, wy);
   my_setcolor(C_WHITE);

   my_printf("%s ", txt);
   my_printf("%2d", result);

   if(pstat->temp > 0) {
      my_setcolor(C_GREEN);
      my_printf(" (%+2d)", pstat->temp);
   }
   else if(pstat->temp < 0) {
      my_setcolor(C_RED);
      my_printf(" (%2d)", pstat->temp);
   }

   if(pstat->perm > 0) {
      my_setcolor(CH_GREEN);
      my_printf(" (%+2d)", pstat->perm);
   }
   else if(pstat->perm < 0) {
      my_setcolor(CH_RED);
      my_printf(" (%2d)", pstat->perm);
   }

   return result;
}

//print status lines
void showstatus(bool showmove)
{
//     _dungeonleveldef *levptr;

   my_setcolor(C_WHITE);

   /* movement count on every turn */
   my_gotoxy(1, 5);
   my_printf("Moves: %-6d", player.movecount);

   /* does money amount need a redraw ? */
   if(GAME_NOTIFYFLAGS & GAME_MONEYCHG) {
      my_gotoxy(1, 7);
      my_printf("Wealth:");
      my_gotoxy(2, 8);
      my_printf("%6.1f", 
		((real)player.inv.copper) / valuables[MONEY_GOLD].value);
   }

   /* does experience need a redraw ? */
   if(GAME_NOTIFYFLAGS & GAME_EXPERCHG) {
      my_gotoxy(1,9);
      my_printf("Exp: %d", player.exp);
   }

   if(GAME_NOTIFYFLAGS & GAME_ATTRIBCHG) {
      show_statcolor(11, "Str:", &player.stat[STAT_STR]);
      show_statcolor(12, "Tgh:", &player.stat[STAT_TGH]);
      show_statcolor(13, "Dex:", &player.stat[STAT_DEX]);
      show_statcolor(14, "Con:", &player.stat[STAT_CON]);
      show_statcolor(15, "Cha:", &player.stat[STAT_CHA]);
      show_statcolor(16, "Wis:", &player.stat[STAT_WIS]);
      show_statcolor(17, "Int:", &player.stat[STAT_INT]);

      show_statcolor(18, "Spd:", &player.stat[STAT_SPD]);
      show_statcolor(19, "Lck:", &player.stat[STAT_LUC]);
   }

   if(GAME_NOTIFYFLAGS & GAME_HPSPCHG) {
      my_gotoxy(1, 20);
      my_printf("Hp: %3d/%-3d", player.hp, player.hp_max);
      my_gotoxy(1, 21);
      my_printf("Sp: %3d/%-3d", player.sp, player.sp_max);
   }

   my_setcolor(C_WHITE);
   my_gotoxy(1, STATUSROW);

   if(GAME_NOTIFYFLAGS & GAME_LEVELCHG) {
//    my_printf("%-20s A level %d %s ", player.name, player.level, 
//		classes[player.pclass].name);
      my_printf("%s %s (lvl %d)", player.name, player.title, player.level);

      if(player.huntmode)
	 sprintf(tempstr, "Wilderness");
      else {
	 sprintf(tempstr, "%s Lvl:%d %s", dungeonlist[player.dungeon].name, 
		 player.levptr->index, player.levptr->name);
      }

      my_gotoxy(SCREEN_COLS-strlen(tempstr), STATUSROW);
      my_printf(tempstr);
   }


   if(GAME_NOTIFYFLAGS & GAME_ALIGNCHG) {
      my_gotoxy(SCREEN_COLS-2, STATUSROW+1);
      if(player.align == CHAOTIC) 
	 my_printf("C-");
      else if(player.align < CHAOTIC_E) 
	 my_printf("C ");
      else if(player.align == NEUTRAL) 
	 my_printf("N=");
      else if(player.align < NEUTRAL_E) 
	 my_printf("N ");
      else if(player.align < LAWFUL_E) 
	 my_printf("L ");
      else if(player.align == LAWFUL) 
	 my_printf("L+");
   }

#ifdef _NOT_USED_CODE_
      clearline(STATUSROW+2);
   
      my_gotoxy(1, STATUSROW+2);
      if(player.nutr == FOOD_FAINTED) {
	 my_setcolor(CHB_RED);
	 my_printf("Fainted! ");
      }   
      else if(player.nutr <= FOOD_FAINTING) {
	 my_setcolor(CH_RED);
	 my_printf("Fainting ");
      }
      else if(player.nutr <= FOOD_STARVING) {
	 my_setcolor(C_RED);
	 my_printf("Starving ");
      }
      else if(player.nutr <= FOOD_HUNGRY) {
	 my_setcolor(C_YELLOW);
	 my_printf("Hungry ");
      }
      else if(player.nutr <= FOOD_FULL) {
      }
      else if(player.nutr <= FOOD_SATIATED) {
	 my_setcolor(C_GREEN);
	 my_printf("Satiated ");
      }
      else {
	 my_setcolor(CH_GREEN);
	 my_printf("Bloated ");
      }

      /* confuzion */
      if(player.conf > 0) {
	 my_setcolor(CH_BLUE);
	 my_printf("Confuzed ");
      }        

      /* check weights */
      if( player.burden == INV_OVERLOAD ) {
	 my_setcolor(CH_RED);
	 my_printf("Overloaded ");
      }
      else if( player.burden == INV_STRAIN) {
	 my_setcolor(C_RED);
 	 my_printf("Strained ");
      }
      else if( player.burden == INV_BURDEN) {
	 my_setcolor(C_RED);
	 my_printf("Burdened ");
      }
   }
#endif

   if(GAME_NOTIFYFLAGS & GAME_CONDCHG) {
      cond_statshow(player.conditions, STATUSROW+2);
   }

   /* hack for level editing mode - REMOVE */
   if(editmode) {
      my_gotoxy(1, STATUSROW+2);
      my_setcolor(CHB_RED);
      my_printf("Edit: ");
      my_setcolor(C_WHITE);
      my_printf("%2d   = %-20s [%3d][%3d]", editerrain, terrains[editerrain].desc, player.pos_x, player.pos_y);
      my_setcolor(terrains[editerrain].color);
      my_gotoxy(10, STATUSROW+2);
      my_putch(terrains[editerrain].out);
   }
}

/*
** Check if location x,y is occupied by monster
** returns pointer to the monsterlist entry of that monster
*/
_monsterlist *istheremonster(level_type *level, int16u x, int16u y)
{
   _monsterlist *ptr;
   ptr=level->monsters;

   if(!ptr) return NULL;

   while(1) {
      if(ptr->x==x && ptr->y==y)
	 return ptr;

      ptr=ptr->next;
      if(!ptr) break;
   }

   return NULL;
}

/*
** return pointer to inventory node if item exists in coordinates x,y
**
*/
Tinvpointer giveiteminfo(level_type *level, int16u x, int16u y)
{
   Tinvpointer ptr;

   ptr = level->inv.first;

   while(ptr) {
      if(ptr->x==x && ptr->y==y)
	 return ptr;

      ptr=ptr->next;
   }
   return NULL;
}

Tinvpointer largestitem(level_type *level, int16u x, int16u y)
{
   Tinvpointer ptr, heaviest=NULL;

   ptr=level->inv.first;

   while(ptr) {
      if(ptr->x==x && ptr->y==y) {
	 if(!heaviest)
	    heaviest=ptr;
	 else {
	    if(item_calcweight(&ptr->i) > item_calcweight(&heaviest->i))
	       heaviest=ptr;
	 }
      }
      ptr=ptr->next;
   }
   return heaviest;
}


void crush_items(level_type *level, int16s x, int16s y, Tinvpointer nothis)
{
   Tinvpointer crush;
   bool breakone=false;

   crush=level->inv.first;
   while(crush) {
      if(nothis!=crush && crush->x==x && crush->y==y) {
	 if(!breakone) {
	    if(distance(player.pos_x, player.pos_y, crush->x, 
			crush->y) < player.sight)
	       msg.add("You hear a nasty crushing sound.", C_WHITE);
	 }
	 breakone=true;
	 crush->i.icond=COND_BROKEN;
      }
      crush=crush->next;
   }
}

int8u age_food(item_def *item)
{
   if(item->type!=IS_FOOD)
      return 0;

   /* food must be removed if ages enough */
   if(item->icond==COND_BROKEN+1)
      return 1;

   return 0;
}

/****************************
	age (make worse) weapons

   luck is holders luck value

   first a rand value 1-100 is generated, this value is then compared
   to the materials BREAK PROPABILITY value, if rand is lower than
   the material value then item condition might be lowered...

   A second throw against item holders luck is then issued
   if throw (1..100) <= luck, item remains the same

   when COND_BROKEN is reached, function returns 1
      if item breaks completely function returns 2 (beyond repair)

   materials with BREAK PROBABILITY of 0 WILL *NOT* get worse

*/
int8s age_weapon(item_def *item, int16s luck, bool player)
{
   int16u prob;
   int8u shard=0;

   if(!item)
      return 0;

   /* if broken weapons breaks once more */
   if(item->icond==COND_BROKEN)
      return 2;

   /* some items don't get older */
   if(item->turnsleft<0)
      return 0;

   if(luck < 10 && luck>=0)
      item->turnsleft-=1+RANDU(10-luck);
   else
      item->turnsleft--;

   /* if prob is smaller than material->breakprob (1-100)
      then make item condition worse */

   if( item->turnsleft<=0 && item->icond!=COND_BROKEN) {

      item->icond++;

      item->turnsleft=materials[item->material].durability;

      /* substract damage modifiers */
      item->meldam_mod--;
      item->misdam_mod--;

      /* if MATSTAT_CANSHARD is set there is
	 a chance that it breaks immendiately */
      if( materials[item->material].status & MATSTAT_CANSHARD ) {
	 prob=50 + (luck - 10);

	 if((1+RANDU(100)) > prob) {
	    item->icond=COND_BROKEN;
	    shard++;
	 }
      }

      if(player && item->icond<COND_BROKEN && !shard) {
	 sprintf(tempstr, "Your %s %s seems worse.", materials[item->material].name, item->name);
	 msg.newmsg(tempstr, CH_BLUE);
      }

      /* if stat reaches BROKEN condition then it must be removed
	 by the caller of this routine */
      if(item->icond>=COND_BROKEN) {
	 if(player) {
	    if(shard)
	       sprintf(tempstr, "Your %s %s breaks into million pieces.", materials[item->material].name, item->name);
	    else
	       sprintf(tempstr, "Your %s %s breaks up.", materials[item->material].name, item->name);
	    msg.newmsg(tempstr, CH_RED);
	 }
	 item->melee_ds=0;
	 item->melee_dt=0;
	 item->missi_ds=0;
	 item->missi_dt=0;
	 item->meldam_mod=0;
	 item->misdam_mod=0;

	 item->icond=COND_BROKEN;
	 if(shard)
	    return 2;
	 return 1;
      }
   }

   return 0;
}

void level_handleitems(level_type *level)
{

   Tinvpointer iptr, temptr;

   iptr = level->inv.first;

   while(iptr) {

      temptr=iptr;
      iptr=iptr->next;
/*
  if(age_food(&temptr->i))
  level_removeitem(level, temptr, 1);
*/
   }
}

/* 
 * Tries to move an item 'item' to direction given in 'dir'
 * if 'checkmonster' is true then does not allow items to move
 * if grid blocked by monster (items which are not walkable, like large
 * boulders with ITEM_NOTPASSABLE bit set)
 *
 * Returns 0 on successful move, >0 if not
 *
 */
int8u move_item(level_type *level, Tinvpointer item, int8u dir, 
	       bool checkmonster)
{
   _monsterlist *mptr=NULL;
   int16s ix, iy;
   int8u res;

   if(!item || !level || dir>9) 
      return false;

//   ix=item->x;
//   iy=item->y;

   if(item->i.status & ITEM_NOTPASSABLE) {
      level->loc[item->y][item->x].flags |= CAVE_PASSABLE;
   }

   /* move the item */
   ix=item->x + move_dx[dir];
   iy=item->y + move_dy[dir];

   if(checkmonster && (item->i.status & ITEM_NOTPASSABLE)) {
      mptr=istheremonster(level, ix, iy);
   }

   /* if the new location is passalbe, actually update the coords */
   if(mptr==NULL && (level->loc[iy][ix].flags & CAVE_PASSABLE)) {

      item->x=ix;
      item->y=iy;
      res=0;
   }
   else if(mptr)
      res=BLOCKED_MONSTER;
   else
      res=BLOCKED_WALL;

   /* if item is not walkable, mark the level location not walkable */
   if(item->i.status & ITEM_NOTPASSABLE) {
      /* mask the passable flag out from current level location */
      level->loc[item->y][item->x].flags &= ( 0xffff^CAVE_PASSABLE );
   }

   return res;
}




/*
** This routine does one turn of game...
*/

/*
 * When player makes a move, it takes a certain amount of time, this time
 * is then divided into a number of time slots. The length of one time slot
 * is taken from player speed attribute.
 *
 */

#define TIMEMOD_OUTSIDE 10

void game_passturn(level_type *level, bool playervis, bool foodsub)
{
   int32s ptime, spdadd, difft;

#ifdef _TIME_MEASUREMENT_
   timeval tt1, tt2;
#endif

#ifdef _TIME_MEASUREMENT_
   gettimeofday(&tt1, NULL);
#endif
  
   if(player.timetaken<BASE_TIMENEED)
      player.movecount++;

   if(player.nutr<=FOOD_FAINTING) {
      if(CONFIGVARS.foodwarn) {
	 msg.newmsg("You're starving!", CH_RED);
      }
   }

   /* if move didn't take any time, ... */
   if(player.timetaken>=BASE_TIMENEED) {
      game_noticeevents(level);

      msg.notice();

      if(playervis)
	 showplayer(level);

      return;
   }

   if(CONFIGVARS.lightmonster) {
      int16s a, x1, x2, y1, y2;

      a=(1+player.sight)*2;

      x1=player.pos_x - a;
      x2=player.pos_x + a;
      y1=player.pos_y - a;
      y2=player.pos_y + a;
      if(x1 < 0) x1=0;
      if(x2 >= level->sizex) x2=level->sizex-1;
      if(y1 < 0) y1=0;
      if(y2 >= level->sizey) y2=level->sizey-1;

      while(y1<y2) {
	 a=x1;
	 while(a<x2) {
	    level->loc[y1][a].flags&=(0xffff ^ CAVE_MONSTERLIGHT);
	    a++;
	 }
	 y1++;
      }
   }

   player_handleitems();
   player_inroom(level);

   spdadd=get_stat(&player.stat[STAT_SPD]);
   ptime=get_stat(&player.stat[STAT_SPD]);

   if(player.dungeon==0 && !player.huntmode) {
      spdadd = spdadd / TIMEMOD_OUTSIDE;
   }

   /* calculate the number of used "time slots" taken by player move */
   difft = ((int32s)BASE_TIMENEED - player.timetaken) / spdadd;

   /* advance world time and calendar */
   passedtime.tick(ptime * difft);
   worldtime.tick(ptime * difft);
   weather.passtime((ptime * difft), worldtime);

   /* regenerate player hitpoints and handle other stuff */
   player_handlestatus(level, difft);

   /* 
    * if dungeon==0 then player is in wilderness and there're are no
    * monsters at the moment.
    *
    */

/*
  if(player.dungeon!=0) {
  while(player.timetaken<BASE_TIMENEED) {

  handle_monsters(level, difft);

  player.timetaken+=spdadd;
  }
  }
*/
   handle_monsters(level, difft);

   player.timetaken+=(difft*spdadd);


   game_noticeevents(level);

   if(playervis)
      showplayer(level);

   msg.notice();

   my_gotoxy(MAPWIN_RELX+player.spos_x, MAPWIN_RELY+player.spos_y);


   player.timetaken=BASE_TIMENEED;  // clear turntime before next turn

#ifdef _TIME_MEASUREMENT_
   gettimeofday(&tt2, NULL);
   my_setcolor(CH_RED);
   my_gotoxy(1, SCREEN_LINES-1);
   my_printf("Turn took %ld sec %ld microsec.", tt2.tv_sec - tt1.tv_sec, 
	     tt2.tv_usec - tt1.tv_usec);
#endif

}

/* return true if location visible for player */
bool player_has_los(level_type *level, int16s x, int16s y)
{
   if(level->loc[y][x].flags & CAVE_LOSTMP)
      return true;

   return false;
}


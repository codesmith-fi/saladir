/* traps.cc 
 */


#include "saladir.h"

#include "output.h"
#include "types.h"
#include "doors.h"
#include "tables.h"
#include "variable.h"
#include "generate.h"
#include "damage.h"
#include "dungeon.h"
#include "monster.h"
#include "inventor.h"
#include "traps.h"

Ttrap list_traps[]=
{
   { "disarmed", '^', C_WHITE, 0 },
   { "boulder", '~', C_RED, TRAPF_DOORTRAP },
   { "bomb", '^', C_RED,  TRAPF_DOORTRAP|TRAPF_CAVETRAP },
   { "electric shock", '^', CH_BLUE,
     TRAPF_CAVETRAP|TRAPF_NOREMOVE },


   { NULL, 0 }
};


/*
 * get a horizontal or vertical direction from x1,y1 to x2,y2
 * if "onlymajor" is set, then get only one of these directions,
 * "LEFT" "RIGHT" "UP" or "DOWN".
 */
int8u get_dir_to(level_type *level, int16s x1, int16s y1, int16s x2, int16s y2)
{
   int8u i;

   x2-=x1;
   y2-=y1;

   for(i=0; i<10; i++) {
      if(x2==move_dx[i] && y2==move_dy[i])
	 break;
   }

   if(i==DIR_UPLEFT) {
      if(ispassable(level, x1-1, y1))
	 return DIR_LEFT;
      else if(ispassable(level, x1, y1-1))
	 return DIR_UP;
   }
   else if(i==DIR_UPRIGHT) {
      if(ispassable(level, x1+1, y1))
	 return DIR_RIGHT;
      else if(ispassable(level, x1, y1-1))
	 return DIR_UP;
   }
   else if(i==DIR_DNRIGHT) {
      if(ispassable(level, x1+1, y1))
	 return DIR_RIGHT;
      else if(ispassable(level, x1, y1+1))
	 return DIR_DOWN;
   }
   else if(i==DIR_DNLEFT) {
      if(ispassable(level, x1-1, y1))
	 return DIR_LEFT;
      else if(ispassable(level, x1, y1+1))
	 return DIR_DOWN;
   }

   return i;  /* 5 */
}

/* remove a trap */
void trap_remove(level_type *level, int16s dx, int16s dy)
{
   level->loc[dy][dx].trap=TRAP_NOTRAP;
   level->loc[dy][dx].trap_st=0;
}

bool trap_eshock(level_type *level, int16s tx, int16s ty)
{
   _monsterlist *mptr;

   mptr=istheremonster(level, tx, ty);

   if(player_has_los(level, tx, ty)) {
      if(tx==player.pos_x && ty==player.pos_y) {
	 my_strcpy(player_killer, "Got zapped by an electric trap.", 
		   sizeof(player_killer));
	 msg.add("Zap...", CH_RED);
	 damage_issue(level, NULL, NULL, ELEMENT_ELEC, 
		      level->loc[ty][tx].trap_st, RANDU(HPSLOT_MAX+1),
		      NULL);
      }
      else if(mptr) {
	 monster_sprintf(nametemp, mptr, true, true );
	 msg.vadd(C_WHITE, "%s got zapped.",
		     nametemp);
	 damage_issue(level, mptr, NULL, ELEMENT_ELEC, 
		      level->loc[ty][tx].trap_st, RANDU(HPSLOT_MAX+1),
		      NULL);	 
      }

      for(int8u i=0; i<8; i++) {
	 my_setcolor(RANDU(16));
	 my_mvaddch((MAPWIN_RELX+player.spos_x)+(tx-player.pos_x), 
		    MAPWIN_RELY+player.spos_y+(ty-player.pos_y),
		    '*');
	 my_refresh();
	 my_delay(200);
      }

   }

   return true;
}

bool trap_boulder(level_type *level, int16s dx, int16s dy, 
		  int16s tx, int16s ty)
{
   _monsterlist *mptr;
   int8u dir, force;
   Tinvpointer boulder;

//   bool somevis=false;
   bool moveres, hitsome;

   /* this is a door trap only */
   if(!isitadoor(level, dx, dy))
      return false;

   /* open the door */
   door_closed2open(level, dx, dy);

   dir=get_dir_to(level, dx, dy, tx, ty);

   /* create a boulder */
//   boulder=add_specialitem(level, dx, dy, 
//			   SPECIAL_BOULDER);
   boulder = level_createitem( level, dx, dy, 
			    IS_SPECIAL, SPECIAL_BOULDER, 1, -1);
   showplayer(level);

   if(!boulder) {
      return false;
   }
   else if(player_has_los(level, dx, dy)) {
      msg.add("A large boulder drops as the door opens!", C_WHITE);
   }

   /* do until hits a wall */

   force=5+RANDU(10);

   moveres=0;
   while(!moveres && force>0) {    // ispassable(level, tx, ty)) {
      hitsome=false;
      /* move the boulder */
      moveres=move_item(level, boulder, dir, false);
      force--;

      showplayer(level);

      tx=boulder->x;
      ty=boulder->y;

      if(player_has_los(level, tx, ty)) {

	 /* crush items */
	 crush_items(level, tx, ty, boulder);

	 if(tx==player.pos_x && ty==player.pos_y) {
	    my_strcpy(player_killer, "a nasty rolling boulder.", 
		      sizeof(player_killer));
	    msg.newmsg("You're crushed by the boulder.", CH_RED);
	    damage_issue(level, NULL, NULL, ELEMENT_NOTHING, 
			 level->loc[dy][dx].trap_st, -1,
			 NULL);
	    hitsome=true;
	 }
	 else {
	    mptr=istheremonster(level, tx, ty);
	 
	    if(mptr) {
	       monster_sprintf(nametemp, mptr, true, true );
	       msg.vnewmsg(C_WHITE, "%s is crushed by rolling boulder.", 
			   nametemp);
	       damage_issue(level, mptr, NULL, ELEMENT_NOTHING, 
			    level->loc[dy][dx].trap_st, -1,
			    NULL);
	       hitsome=true;
	    }
	 }

	 if(hitsome) {
	    new_output_items(level);
	    my_refresh();
	    msg.notice();
	    showmore(false, true);
	    msg.update(true);
	 }
	 else 
	    my_delay(200);
      }

   }


   return true;
}


bool trap_bomb(level_type *level, int16s dx, int16s dy)
{
   _monsterlist *mptr;

   int16u j, i, ci=0;
   int16s bx, by;
   bool somevis;
  

   const char bomb_seq[]="-+x*X*x+-";
   const int16u bomb_col[]={ C_YELLOW, C_RED, CH_YELLOW, 
			     CH_RED, CH_RED, 0xffff };

   /* destroy some dungeon first */
   for(i=1; i<10; i++) {
      bx=dx + move_dx[i];
      by=dy + move_dy[i];
      if(bx>0 && bx<level->sizex && by>0 && by<level->sizey) {
	 set_terrain(level, bx, by, TYPE_PASSAGE);
      }
   }

   /* calculate new los */
   showplayer(level);

   for(j=0; j<strlen(bomb_seq); j++) {
      somevis=false;

      for(i=1; i<10; i++) {
	 bx=dx + move_dx[i];
	 by=dy + move_dy[i];

	 /* make some damage for monsters and level structures */
	 if(!j) {

	    if(bx==player.pos_x && by==player.pos_y) {
	       my_strcpy(player_killer, "Roasted by a fire trap.", 
			 sizeof(player_killer));
	       msg.newmsg("You're burned by the soaring flames..", CH_RED);
	       damage_issue(level, NULL, NULL, ELEMENT_FIRE, 
			    level->loc[dy][dx].trap_st, -1,
			    NULL);
	    }
	    
	    mptr=istheremonster(level, bx, by);

	    if(mptr) {
	       monster_sprintf(nametemp, mptr, true, true );
	       msg.vnewdist(level, mptr->x, mptr->y, C_RED, C_RED,
			    NULL, 
			    "%s is burned by the flames.", 
			    nametemp);

	       damage_issue(level, mptr, NULL, ELEMENT_FIRE, 
			    level->loc[dy][dx].trap_st, -1,
			    NULL);
	    }

	 }

	 /* display it if player sees */
	 if(player_has_los(level, bx, by)) {
	    somevis=true;
	    my_setcolor(bomb_col[ci]);
	    my_mvaddch((MAPWIN_RELX+player.spos_x)+(bx-player.pos_x), 
		       MAPWIN_RELY+player.spos_y+(by-player.pos_y),
		       bomb_seq[j]);
	 }


      }
      if(somevis) {
	 msg.notice();
	 my_refresh();
	 my_delay(70);
      }

      ci++;
      if(bomb_col[ci]==0xffff)
	 ci=0;
   }

   return true;
}

/* handle trapped door, return true if the trap removed the door */
bool handletrap(level_type *level, int16u dx, int16u dy, 
		_monsterlist *monster)
{
   bool tres, door=false;
   int16s tx, ty, tid=0;

   tid=level->loc[dy][dx].trap;

   if(isitadoor(level, dx, dy))
      door=true;
   
   if(tid==TRAP_NOTRAP)
      return false;

   if(monster) {
      tx=monster->x;
      ty=monster->y;
      monster_sprintf(nametemp, monster, true, true );
      msg.vnewdist(level, monster->x, monster->y, C_RED, C_RED,
		   NULL, 
		   "%s triggers a trap.", 
		   nametemp);
   }
   else {
      tx=player.pos_x;
      ty=player.pos_y;
      showplayer(level);
      msg.add("You trigger a trap...", C_RED);      
   }


   switch(tid) {
      case TRAP_NOTRAP:
	 tres=false;
	 break;
      case TRAP_BOULDER:
	 tres=trap_boulder(level, dx, dy, tx, ty);
	 break;
      case TRAP_BOMB:
	 tres=trap_bomb(level, dx, dy);
	 break;
      case TRAP_ESHOCK:
	 tres=trap_eshock(level, dx, dy);
	 break;
      default:
	 tres=false;
   }

   /* remove if the trap is in a door or flagged as removable on
    * blow up 
    */
   if(!(list_traps[tid].flags & TRAPF_NOREMOVE) || door )
      trap_remove(level, dx, dy);
   else
      trap_makevisible(level, dx, dy);

   return tres;
}

void trap_create(level_type *level, int16u x, int16u y, int16s type)
{
   int16u trap, trapf;

   /* check for a legal trap */
   if( (type<=0) || (type>TRAP_MAXNUM) )
      trap=1+RANDU(TRAP_MAXNUM);
   else
      trap = type;
   
   if(isitadoor(level, x, y)) {
      level->loc[y][x].doorfl|=DOOR_TRAPPED;
      trapf=TRAPF_DOORTRAP;
   }
   else
      trapf=TRAPF_CAVETRAP;
   
   /* search a legal trap, depending on a location of level */
   while(!(list_traps[trap].flags & trapf)) {
      trap=1+RANDU(TRAP_MAXNUM);
   }

   /* create a random trap */
   level->loc[y][x].trap=trap;
   level->loc[y][x].trap_st=throwdice(4, 4, 1);
}

void trap_makevisible(level_type *level, int16u x, int16u y)
{
/*
   if(level->loc[y][x].trap == TRAP_NOTRAP)
      return;

   if(!isitadoor(level, x, y))
      set_terrain(level, x, y, TYPE_FIRSTTRAP+level->loc[y][x].trap-1);
*/

   level->loc[y][x].flags |= CAVE_TRAPIDENT;
}

/**************************************************************************
 * search.cc --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 26.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 26.04.1888                                         *
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
/* Searching related functions and definitions                            *
 **************************************************************************/

#include "saladir.h"

#include "output.h"
#include "tables.h"
#include "doors.h"
#include "creature.h"
#include "monster.h"
#include "skills.h"
#include "utility.h"
#include "traps.h"
#include "search.h"


/*
 * Search surroundings 
 *
 */

int16u search_location(level_type *level, int16s x, int16s y)
{
   if(x<1 || x>=level->sizex || y<1 || y>=level->sizey)
      return FOUND_NOTHING;

   if(is_secretdoor(level, x, y))
      return FOUND_SECRETDOOR;

   if(level->loc[y][x].trap!=TRAP_NOTRAP) {
      if((level->loc[y][x].flags & CAVE_TRAPIDENT))
	 return FOUND_NOTHING;
	 
      if(isitadoor(level, x, y)) {
	 return FOUND_DOORTRAP;
      }
      else {
	 return FOUND_CAVETRAP;
      }
   }

   return FOUND_NOTHING;
      
}

bool search_surroundings(level_type *level, _monsterlist *monster, 
			 bool automatic)
{
   int16u sres, skill;
   int16s sx, sy;
   int8u i;

   if(!monster)
      skill=skill_check(player.skills, SKILLGRP_GENERIC, SKILL_SEARCHING);
   else
      skill=skill_check(monster->skills, SKILLGRP_GENERIC, SKILL_SEARCHING);

   /* if automatic searching, no success if no skill
    * if manual searching, allow some sort of a success
    */
   if(!skill && automatic)
      return false;

   if(!skill)
      skill=5;
   
   if(monster) {
      if(player_has_los(level, monster->x, monster->y))
	 return false;

      monster->time-=calc_time(get_stat(&monster->stat[STAT_SPD]), 
			       TIME_SEARCH, monster->tactic);

      monster_sprintf(nametemp, monster, true, true);
      if(RANDU(100)<20) {
	 msg.vnewdist(level, monster->x, monster->y, 
		      C_WHITE, C_WHITE,
		      NULL,
		      "%s seems to be searching for something...",
		      nametemp);
      }
      sx=monster->x;
      sy=monster->y;
   }
   else {
      if(!automatic) {
	 msg.newmsg("You search your surroundings...", C_WHITE);
	 player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), 
				     TIME_SEARCH, player.tactic);

      }
      else {
	 if(RANDU(100) < 20)
	    msg.newmsg("You're searching!", C_WHITE);
      }
      sx=player.pos_x;
      sy=player.pos_y;
   }

   sres=0;
   for(i=1; i<10; i++) {
      
      sres=search_location(level, sx+move_dx[i], sy+move_dy[i]);
      if(sres!=FOUND_NOTHING)
	 break;
   }

   if(RANDU(100)>skill) {
      sres=FOUND_NOTHING;
   }

   if(automatic) {
      if( sres!=FOUND_NOTHING && !monster ) {
	 msg.addwait("You notice that there's something hidden around here.", 
		 CH_WHITE);
      }
      return true;
   }

   switch(sres) {
      case FOUND_SECRETDOOR:
	 if(monster) {
	    msg.vnewdist(level, monster->x, monster->y, 
			 C_WHITE, C_WHITE,
			 NULL,
			 "%s found a secret door.",
			 nametemp);
	 }
	 else 
	    msg.addwait("a secret door!", C_WHITE);

	 door_secret2normal(level, sx+move_dx[i], sy+move_dy[i]);
	 break;
      case FOUND_DOORTRAP:
	 if(monster) {
	    msg.vnewdist(level, monster->x, monster->y, 
			 C_WHITE, C_WHITE,
			 NULL,
			 "%s found a trapped door.",
			 nametemp);
 	 }
	 else 
	    msg.addwait("this door is trapped!", C_WHITE);

	 trap_makevisible(level, sx+move_dx[i], sy+move_dy[i]);
	 break;
      case FOUND_CAVETRAP:
	 if(monster) {
	    msg.vnewdist(level, monster->x, monster->y, 
			 C_WHITE, C_WHITE,
			 NULL,
			 "%s found a trap.",
			 nametemp);
 	 }
	 else 
	    msg.addwait("a TRAP!", C_WHITE);

	 trap_makevisible(level, sx+move_dx[i], sy+move_dy[i]);

	 break;
	 
      default:
	 if(!monster)
	    msg.newmsg("nothing found.", C_WHITE);
	 return false;
	 break;	
   }

   if(monster)
      skill_modify_raise(&monster->skills, SKILLGRP_GENERIC, SKILL_SEARCHING,
			 1, 5);
   else
      skill_modify_raise(&player.skills, SKILLGRP_GENERIC, SKILL_SEARCHING,
			 1, 5);
		      
   return true;
}


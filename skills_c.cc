/**************************************************************************
 * skills_c.cc --                                                         *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 31.05.1888                                         *
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
 * a skill module, contains the code for actual skills. skills.cc contains*
 * general skill handling routines                                        *
 **************************************************************************/

#include "saladir.h"

#include "output.h"
#include "types.h"
#include "defines.h"
#include "caves.h"
#include "tables.h"
#include "traps.h"
#include "skills.h"

/*
 * Test skill against success or failure.
 * This routine was included here because later on some special conditions
 * might affect these conditions. Ie. a blessed player could get extra
 * try, cursed player might get worse results. Lucky player has a better
 * chance and so on.
 *
 * Returns true if skill throw was a success. False if not success.
 *
 */
bool skill_testsuccess(_monsterlist *mptr, int16u group, int16u type)
{
   int16s value, dc, luck=0;
   Tskillpointer list;

   if(mptr)
      list=mptr->skills;
   else
      list=player.skills;

   /* get skill value */
   value=skill_check(list, group, type) + luck;

   if(value<=0)
      return false;
   
   dc=throwdice(1, 100, 0);

   if(dc<=value)
      return true;

   return false;
}


bool skill_disarmtrap(level_type *level, _monsterlist *monster)
{
   int8u dir;
   int16u val, tid=0;

   /* not monster support yet */
   if(monster)
      return false;

   for(dir=1; dir<10; dir++) {
      if(level->loc[player.pos_y+move_dy[dir]][player.pos_x+move_dx[dir]].flags & CAVE_TRAPIDENT)
	 break;
   }

   if(dir>9) {
      msg.newmsg("You don't know any traps around here.", C_WHITE);
      return false;
   }

   tid=level->loc[player.pos_y+move_dy[dir]][player.pos_x+move_dx[dir]].trap;

   if(!tid || tid>TRAP_MAXNUM) {
      msg.newmsg("The trap needs no disarming.", C_WHITE);
      return false;
   }

   msg.vadd(C_WHITE, "You try to disarm the %s trap...", list_traps[tid].name);

//   val=skill_check(player.skills, SKILLGRP_GENERIC, SKILL_DISARMTRAP);
   my_gotoxy(MAPWIN_RELX+player.spos_x+move_dx[dir], 
	     MAPWIN_RELY+player.spos_y+move_dy[dir]);
   my_setcolor(CHB_RED);
   my_putch('X');
   my_refresh();
   
   my_delay(1000);

   if(skill_testsuccess(NULL, SKILLGRP_GENERIC, SKILL_DISARMTRAP)) {
      msg.newmsg("Success!", CH_GREEN);
      trap_remove(level, player.pos_x+move_dx[dir], player.pos_y+move_dy[dir]);

      skill_modify_raise(&player.skills, SKILLGRP_GENERIC, SKILL_DISARMTRAP,
			 1, 5);
   }
   else {
      if(throwdice(1,100,0) < 50) {
	 msg.newmsg("Oh no, the trap goes on!", CH_RED);
	 /* if fails to disarm, launch the trap */
	 handletrap(level, player.pos_x+move_dx[dir], 
		    player.pos_y+move_dy[dir],
		    NULL);
      }
      else {
	 msg.newmsg("You fail, fortunately nothing bad happens!", C_RED);

      }
   }
   return true;
}

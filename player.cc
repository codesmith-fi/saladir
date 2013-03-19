/**************************************************************************
 * player.cc --                                                           *
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
#include "memory.h"

#include "inventor.h"
#include "tables.h"
#include "variable.h"
#include "scrolls.h"
#include "utility.h"
#include "raiselev.h"
#include "shops.h"
#include "process.h"
#include "doors.h"
#include "weapons.h"
#include "damage.h"
#include "magic.h"
#include "file.h"
#include "search.h"
#include "defines.h"
#include "status.h"
#include "traps.h"
#include "skills.h"
#include "skills_c.h"
#include "outworld.h"
#include "pathfunc.h"
#include "imanip.h"
#include "player.h"

/* local prototypes */
void eat_passturns(level_type *, item_def *);

#define ITEM_UNPAYMSGNUM 5
char *item_unpaymsg[]=
{
     "But you have not paid for that!",
     "That's not yours yet.",
     "Maybe you should pay for it first?",
     "The shopkeeper would not like that!",
     "You don't want make the shopkeeper angry!",
     NULL
};

void player_showstatus(void)
{
  int16u i;

  my_clrscr();
  GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

  my_setcolor(C_GREEN);
  drawline(1, '-');
  my_cputs(1, "=[ Your status ]=");
  my_gotoxy(1, 2);

  my_setcolor(CH_WHITE);
  my_printf("Limb status:\n");
  my_setcolor(C_WHITE);
  for(i=0; i<HPSLOT_MAX; i++) {
    my_setcolor(C_YELLOW);
    my_printf("%-15s", bodyparts[i]);
    my_setcolor(C_WHITE);
    my_printf(" Hp [%3d/%3d] Ac [%3d] ",
	      player.hpp[i].cur, 
	      player.hpp[i].max,
	      player.hpp[i].ac);

    my_printf(" F%03d P%03d C%03d E%03d W%03d A%03d\n%15s ",
	      player.hpp[i].res.fire,
	      player.hpp[i].res.poison,
	      player.hpp[i].res.cold,
	      player.hpp[i].res.elec,
	      player.hpp[i].res.water,
	      player.hpp[i].res.acid, "");

    if(player.equip[eqslot_from_hpslot[i]].status==EQSTAT_OK) {
      my_setcolor(C_GREEN);
      my_printf("Condition OK!\n");
    }
    else {
      my_setcolor(C_RED);
      my_printf("Not usable!\n");
    }
    my_setcolor(C_WHITE);

    /*
      my_printf("%15s Fire=%3d Poison=%3d Cold=%3d "
      "Elec=%3d Water=%3d Acid=%3d\n", "", 
      player.hpp[i].res.fire,
      player.hpp[i].res.poison,
      player.hpp[i].res.cold,
      player.hpp[i].res.elec,
      player.hpp[i].res.water,
      player.hpp[i].res.acid);
    */
  }
   
  showmore(false, false);
}

void player_handlestatus(level_type *level, int32u slots)
{
   static int32u oldpackweight=0;

   int16s stat;
   bool statchg, hpregen;
   int16u i=0, j=0;

   int32u carry;

   int16u slotheal[6]={1, 2, 3, 4, 0, 1};

   /************************/
   /* regenerate hitpoints */
   /************************/
   /* collect the slots which need healing */
   j=0;
   for(i=0; i<HPSLOT_MAX; i++) {
      if(player.hpp[i].cur < player.hpp[i].max)
	 slotheal[j++]=i;
   }
   /* j is bigger than 0 if some slot needs healing */
   /* slots to be healed are indexed in array slotheal[], maxindex is j-1 */

//   player.regentime+=get_stat(&player.stat[STAT_SPD]);
   player.regentime+=slots * get_stat(&player.stat[STAT_SPD]);

   stat=get_stat( &player.stat[STAT_CON] );

   hpregen=false;
   if( !j ) {
      player.regentime=0;
   }
   else if(player.regentime >= CON_HITP[stat]) {
      hpregen=true;
   }
   else {
      stat=skill_check(player.skills, SKILLGRP_GENERIC, SKILL_HEALING);
      stat=stat/4;

      if(throwdice(1, 100, 0) < stat) {
	 hpregen=true;
      }
   }

   if(hpregen) {
      player.regentime=0;
      //CON_HITP[stat];

      /* when regenerated, show hpsp area */
      GAME_NOTIFYFLAGS|=GAME_HPSPCHG;
      statchg=false;

      /* regen every hpslot randomly */
      i=RANDU(j);

      if(player.hpp[slotheal[i]].cur < player.hpp[slotheal[i]].max)
	 player.hpp[slotheal[i]].cur++;

      player_checkstat(level, false, true);
   }


   /************************/
   /*         food         */
   /************************/
   /* substract food, each slot takes one food */
   int32u oldnutr = player.nutr;
   
   if(!CONFIGVARS.cheat)
      player.nutr-=slots;

   if(oldnutr >= FOOD_FAINTING && player.nutr < FOOD_FAINTING) {
      msg.addwait("You need food badly!", C_RED);
      cond_add(&player.conditions, CONDIT_FAINTING, 1);
   }
   
   if(oldnutr >= FOOD_STARVING && player.nutr < FOOD_STARVING) {
      cond_add(&player.conditions, CONDIT_STARVING, 1);
      msg.addwait("You're getting really hungry!", C_RED);
   }

   if(oldnutr >= FOOD_HUNGRY && player.nutr < FOOD_HUNGRY) {
      cond_add(&player.conditions, CONDIT_HUNGRY, 1);
      msg.addwait("You're getting hungry!", C_RED);
   }

   if(oldnutr >= FOOD_FULL && player.nutr < FOOD_FULL) {
      cond_delete_group(&player.conditions, CONDGRP_FOOD);
   }

   if(oldnutr >= FOOD_BLOATED && player.nutr < FOOD_BLOATED) {
      cond_add(&player.conditions, CONDIT_BLOATED, 1);
   }

   if(oldnutr >= FOOD_SATIATED && player.nutr < FOOD_SATIATED) {
      cond_add(&player.conditions, CONDIT_SATIATED, 1);
   }

   if(player.nutr <= FOOD_FAINTED) {
      player.alive=false;
      my_strcpy(player_killer, "Starved to death", sizeof(player_killer));
      msg.add("You died by starving to death.", CH_RED);
      cond_add(&player.conditions, CONDIT_DEAD, 1);
      cond_add(&player.conditions, CONDIT_FAINTED, 1);


      GAME_NOTIFYFLAGS|=GAME_SHOWALLSTATS;
      return;
   }

   /* handle all conditions, like confusion, poison etc. */
   cond_handle(NULL, slots);

   if(oldpackweight!=player.inv.weight) {
      oldpackweight=player.inv.weight;

      carry = calc_carryweight(player.stat, player.weight);

      /* remove all conditions related to carry weight */
      cond_delete_group(&player.conditions, CONDGRP_PW);

      /* set new conditions if necessary */
      if(player.inv.weight > (carry *  WGH_OVERLOAD / 100)) {
	 cond_add(&player.conditions, CONDIT_OVERLOADED, 1);
      }
      else if(player.inv.weight > (carry * WGH_STRAIN / 100)) {
	 cond_add(&player.conditions, CONDIT_STRAINED, 1);
      }
      else if(player.inv.weight > (carry * WGH_BURDEN / 100)) {
	 cond_add(&player.conditions, CONDIT_BURDENED, 1);
      }
   }
}

void player_showinv(void)
{
   Titemlistptr multi;

   multi = inv_listitems_multi(&player.inv, "Items in your backpack", 
			       -1, true, true, -1, -1);
   if(multi) {
      inv_freemultilist(&multi);
   }


}

bool player_push(level_type *level, int8u dir)
{
   int8u moveres;
   _monsterlist *mptr;
   Tinvpointer iptr;
   bool automode=false;

   if(!dir) {
      dir=dir_askdir("Push", false);
   }
   else 
      automode=true;

   if(dir==5 || dir>9 || dir<1) {
      if(!automode)
	 msg.newmsg("Not a good idea.", C_WHITE);
      return false;
   }

   /* is there a monster? */
   mptr=istheremonster(level, player.pos_x+move_dx[dir], 
		       player.pos_y+move_dy[dir]);
   if(mptr) {
      monster_sprintf(nametemp, mptr, true, true);
      msg.vnewmsg(C_WHITE, "%s doesnt really want to be pushed around!", 
		  nametemp);
      return false;
   }

   /* push the largest item always */
   iptr=largestitem(level, player.pos_x+move_dx[dir], 
		    player.pos_y+move_dy[dir]);
   if(iptr) {
      moveres=move_item(level, iptr, dir, true);

      if(!moveres) {
	 item_printinfo(&iptr->i, iptr->i.weight, iptr->count, "You push");

	 /* large items crush */
	 if(iptr->i.weight > 300000) {
	    crush_items(level, iptr->x, iptr->y, iptr);
	 }
      }
      else {
	 msg.vnewmsg(C_WHITE, "You push the %s, but it won't move.",
		    iptr->i.name);
	 if(moveres==BLOCKED_MONSTER) {
	    msg.newmsg("You hear some noises on the other side, maybe"
		       " that's the reason.", C_WHITE);
	 }
      }
      /* idea!
       * Kun työntää ison lohkareen esineiden päälle -> ruskis 
       * pyörea lohkare voisi lahtea pyorimaan?!? 
       */
      return true;
   }

   return false;
}


void player_quickskill(level_type *level, int16u slot)
{
   if(slot >= NUM_QUICKSKILLS) {
      msg.newmsg("error! Trying to access quickskill array over boundaries!",
		 CHB_RED);
      return;
   }

   if(player.qskills[slot].select == 0) {
      msg.vnewmsg(C_WHITE, "Quickskill number %d not selected yet.", slot);
      return;
   }

   player_useskill(level, player.qskills[slot].group, 
		   player.qskills[slot].type);

}

bool player_useskill(level_type *level, int16u group, int16s skill)
{
   int16u value, sucval=0;
   int16s bonus=0;
   int16u skill2;

   /***************************************************/
   /***************************************************/
   /* ERI SKILLEILLE ERI AJANKULUTUS!!!!!!!!!!!!!!    */
   /***************************************************/
   /***************************************************/

   player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), 800, TACTIC_NORMAL);

   if(group>SKILLGRP_MAGIC) {
      group=0;
      skill=skill_listselect2(player.skills, &group, 
			      "Select a skill to apply.");
   }
   else if(skill < 0) {
      skill=skill_listselect2(player.skills, &group, 
			      "Select a skill to apply.");
   }

   game_noticeevents(level);

   if(group > SKILLGRP_MAGIC ) {
      msg.newmsg("Trying to use skill from illegal group.", CHB_RED);
      return false;
   }

   if(skill < 0) {
      return false;
   }

   value=skill_check(player.skills, group, skill);

   if(value == 0) 
      return false;

   /* do magic skills */
   if(group==SKILLGRP_MAGIC) {
      sprintf(tempstr, "You concentrate for ");

      /* check which group of spell it is */
      if(list_spells[skill].flags & SPF_ALTERATION) {
	 skill2=SKILL_ALTERATION;
	 strcat(tempstr, "alteration");
      }
      else if(list_spells[skill].flags & SPF_MYSTICISM) {
	 skill2=SKILL_MYSTICISM;
	 strcat(tempstr, "mysticism");
      }
      else if(list_spells[skill].flags & SPF_DESTRUCTION) {
	 skill2=SKILL_DESTRUCTION;
	 strcat(tempstr, "destruction");
      }
      else if(list_spells[skill].flags & SPF_OBSERVATION) {
	 skill2=SKILL_OBSERVATION;
	 strcat(tempstr, "observation");
      }
      else {
	 sprintf(tempstr, "Error: Spell %s has no area of effect!",
		 list_spells[skill].name);
	 msg.newmsg(tempstr, CHB_RED);
	 return false;
      }

      sucval=skill_check(player.skills, 
			 SKILLGRP_GENERIC, skill2);

      sprintf(nametemp, " spell %s...", list_spells[skill].name);
      strcat(tempstr, nametemp);

      msg.newmsg(tempstr, C_GREEN);
      /* give 5% chance for success if no skill in that area */
      if(!sucval)
	 sucval=5;

      bonus=get_stat(&player.stat[STAT_LUC]) - STATMAX_LUCK/2;
      if(bonus>0)
	 sucval+=bonus*2;

      if( throwdice(1, 100, 0) <= sucval) {
	 msg.newmsg("Success!", C_WHITE);
      }
      else {
	 msg.newmsg("Failure!", C_WHITE);
//	 player.sp-=throwdice(1, list_spells[skill].sp, 0);
	 player.sp-=1;
	 if(player.sp < 0)
	    player.sp=0;

	 GAME_NOTIFYFLAGS|=GAME_HPSPCHG;

	 return false;
      }
      if(!spell_zap(level, skill, value, false)) {
	 msg.newmsg("Spell failed.", C_WHITE);
	 return false;
      }

      skill_modify_raise(&player.skills, SKILLGRP_MAGIC, skill, 
			 1, 1);      
      skill_modify_raise(&player.skills, SKILLGRP_GENERIC, skill2, 
			 1, 1);      
   }
   else if(group==SKILLGRP_GENERIC) {
      switch(skill) {
	 case SKILL_SEARCHING:
	    search_surroundings(level, NULL, false);
	    break;
	 case SKILL_DISARMTRAP:
	    skill_disarmtrap(level, NULL);
	    break;
      }

   }
   else {

      msg.newmsg("Function not yet implemented.", C_RED);
   }
   return true;
}

bool player_autopickup(level_type *level)
{
   Tinvpointer lptr;
//   Tinvpointer invptr;
   int32u carrycap;
   int32u count;
   int16u i, j;

   count=inv_countitems(&level->inv, -1, player.pos_x, player.pos_y, false);

   /* don't get multiple items */
   if(count>1) {
      
      return false;
   }
  
   if(strlen(CONFIGVARS.pickuptypes)==0)
      return false;

   /* get level item pointer */
   lptr=giveiteminfo(level, player.pos_x, player.pos_y);

   /* check capasity first */
   carrycap=calc_carryweight(player.stat, player.weight);

   if((player.inv.weight + lptr->count * item_calcweight(&lptr->i)) 
      > carrycap ) {
      msg.newmsg("You tried to take an item, but it weights too much.", 
		 C_WHITE);
      return false;
   }

   /* don't get UNPAID items */
   if(lptr->i.status & ITEM_UNPAID)
      return false;

   if(my_stricmp(CONFIGVARS.pickuptypes, "all")!=0) {

      /* check if the item exists in the pickup list */
      for(i=0, j=0; i<strlen(CONFIGVARS.pickuptypes); i++) {
	 if(CONFIGVARS.pickuptypes[i] == gategories[lptr->i.type].out )
	    j=1;
      }

      if(j==0)
	 return false;
   }

   /* init new item for player */
   lptr = inv_transfer2inv(level, &level->inv, NULL, &player.inv, 
			lptr, lptr->count, 0, 0);
   if(lptr) {
      item_printinfo(&lptr->i, lptr->i.weight, lptr->count, "You took");
   }
   else {
      msg.newmsg("Autopickup: Can't get item, no memory!", CHB_RED);
      return false;
   }

   return true;
}

bool player_inroom(level_type *level)
{
   int16u rc;

   /* maybe we could check if the player is walking on an ROOMFLOOR type
      and then only do this?
   */

   for(rc=0; rc<level->roomcount; rc++) {
      if(player.pos_x >= level->rooms[rc].x1 && 
	 player.pos_x <= level->rooms[rc].x2) {
	 if(player.pos_y >= level->rooms[rc].y1 && 
	    player.pos_y <= level->rooms[rc].y2) {

	    if(player.inroom==-1) {
	       /* mark the room as visited one */
	       if(!(level->rooms[rc].flags & ROOM_IS_VISITED)) {
		  level->rooms[rc].flags|=ROOM_IS_VISITED;
//		  sprintf(tempstr, "This looks like %s.", 
//			  level->rooms[rc].name);
//		  msg.newmsg(tempstr, C_WHITE);
		  msg.vnewmsg(C_WHITE, "This looks like %s.", 
			      level->rooms[rc].name);
	       }
	    }
	    else {
	       /* ungreet */
	    }
	    /* greet */

	    player.inroom=rc;
	    return true;
	 } // end if2
      } // end if1
   } // end for

   player.inroom=-1;
   return false;
}

void player_readscroll(void)
{
   Tinvpointer scroll;
   int32u expgain;
   bool sres=false;
   level_type *level;

   level=c_level;

   scroll=inv_listitems(&player.inv, "Read which scroll", IS_SCROLL, 
			true, -1, -1);

   game_noticeevents(level);

   if(scroll) {
      if(scroll->i.type != IS_SCROLL) {
	 msg.newmsg("It's not a scroll.", C_WHITE);
	 return;
      }

      if(!(scroll->i.status & ITEM_UNPAID))
	 sres = scroll_readit(level, &scroll->i);
      else
	 msg.newmsg("You can't quite make sense of it, the writing "
		    "is oddly blurred here. ", C_MAGENTA);
   }
   else {
      msg.newmsg("Nothing read.", C_WHITE);
      return;
   }

   if(sres) {
      game_noticeevents(level);

      /* relabel unknown scroll */
      if(!(scroll->i.status & ITEM_IDENTIFIED)) {
	 if( !(list_scroll[scroll->i.group].flags & SCFLAG_AUTOIDENTIFY) &&
	     !(list_scroll[scroll->i.group].flags & SCFLAG_NAMED) ) 
	 {      
	    list_scroll[scroll->i.group].flags |= SCFLAG_TRIED;

	    msg.add("What do you want to call it?", C_WHITE);
	    my_gets(nametemp, ITEM_NAMEMAX);
	    if(strlen(nametemp)>0) {
	       if(!my_stricmp(nametemp, list_scroll[scroll->i.group].name)){
		  scroll->i.status |= ITEM_IDENTIFIED;
		  list_scroll[scroll->i.group].flags |= SCFLAG_IDENTIFIED;

		  expgain = RANDU(200) + RANDU(200);

		  
//		  sprintf(tempstr, 
//			  "You get %ld experience for correctly identifying "
//			  "the scroll of %s!", 
//			  expgain, list_scroll[scroll->i.group].name);
//		  msg.newmsg(tempstr, CH_YELLOW);
		  msg.vnewmsg(C_YELLOW, 
			      "You get %ld experience for correctly "
			      "identifying the scroll of %s!", 
			      expgain, list_scroll[scroll->i.group].name);

		  gain_experience(level, NULL, expgain);

	       }
	       else {
		  
		  list_scroll[scroll->i.group].flags |= SCFLAG_NAMED;

		  msg.vnewmsg(C_WHITE, 
			   "You just relabeled a scroll called \"%s\" "
			   "to \"%s\"",
			   list_scroll[scroll->i.group].uname, nametemp);

		  my_strcpy(list_scroll[scroll->i.group].cname, nametemp, 
			    ITEM_NAMEMAX);
	       }
	    }
	    else {
	       msg.newmsg("Cancelled!", C_RED);
	    }

	 }
      }

      if( scroll->i.group != SCROLL_BLANK) {
	 item_clearall(&scroll->i);
	 inv_removeitem(&player.inv, player.equip, scroll, 1);
      }
      
   }
}

void player_eat(void)
{
//   Tinvpointer ptr=NULL;
   Tinvpointer eatit=NULL;
   level_type *level;
   Tinventory *sinv;
//   bool dideat;
//   int16s res;
//   int32u count;

   level=c_level;

   /* check if player can eat first */
   if(player.nutr > FOOD_SATIATED) {
      msg.newmsg("You're too full to eat now.", C_WHITE);
      return;
   }

   sinv=inv_selectsource(&player.inv, &level->inv, player.pos_x, player.pos_y,
		    IS_FOOD);
   if(!sinv) {
      msg.newmsg("Sorry, you've no food on the ground or in your backpack!", 
		 C_WHITE);
      return;
   }

   if(sinv == &level->inv) {
      if(inv_countitems(sinv, -1, player.pos_x, player.pos_y, false)==1) {
	 eatit=giveiteminfo(level, player.pos_x, player.pos_y);

	 if(eatit->i.group==FOOD_CORPSE)
	    sprintf(tempstr, "Do you really want to eat this dead %s", 
		    eatit->i.name);
	 else
	    sprintf(tempstr, "Do you want to eat this %s", eatit->i.name);

	 if(!confirm_yn(tempstr, true, true))
	    eatit = NULL;
      }
      else {
	 eatit=inv_listitems(sinv, "What do you want to eat?", IS_FOOD, 
			     true, player.pos_x, player.pos_y);
      }
   }
   else {
      /* browse the inventory normally */
      eatit=inv_listitems(sinv, "What do you want to eat?", IS_FOOD, 
			  true, -1, -1);
   }

   /* if eatit has a pointer, then we're gonna eat the bloody pointer :) */
   if(eatit) {
      if(eatit->slot>=0) {
	 msg.newmsg("Please, unequip it first.", C_WHITE);
	 return;
      }

      if(eatit->i.type == IS_FOOD) {
	 /* eat */

	 if(eatit->i.status & ITEM_UNPAID) {
	    msg.newmsg(item_unpaymsg[RANDU(ITEM_UNPAYMSGNUM)], C_MAGENTA);
	    return;
	 }

	 game_noticeevents(level);
	 eat_passturns(level, &eatit->i);
	 if(eatit->i.pmod3) 
	    eat_specialmonster(level, eatit->i.pmod3);
	 item_clearall(&eatit->i);
	 inv_removeitem(sinv, NULL, eatit, 1);
      }
      else
	 msg.vnewmsg(CH_RED, "You can't eat that, for gods sake "
		     "it's made out of %s.",
		     materials[eatit->i.material].name);
   }
}

/* Some funny messages to display while eat is going on */
#define NUM_EATMSG 10
char *eatmsg[]=
{
     "munch...",
     "Crunsk...",
     "rousk...",
     "yam...",
     "gmbh...",
     "njam...",
     "mmmm...",
     "burp!...",
     "hjam...",
     "mjousk..",
     NULL
};

void eat_addnutr(item_def *item, int32u weight)
{
//   int16u timea;
   real nutrv;

   /* food nutrition based on weight and multiplier pmod1 */
   nutrv = (real)weight * 0.5;
   
   nutrv = nutrv / 100 * item->pmod1;

   player.nutr += (int32u)nutrv;

   if(player.nutr >= FOOD_MAXNUTR)
      player.nutr=FOOD_MAXNUTR;

   if(player.nutr <= FOOD_FAINTING)
      cond_add(&player.conditions, CONDIT_FAINTING, 1);
   else if(player.nutr <= FOOD_STARVING)
      cond_add(&player.conditions, CONDIT_STARVING, 1);
   else if(player.nutr <= FOOD_HUNGRY)
      cond_add(&player.conditions, CONDIT_HUNGRY, 1);
   else if(player.nutr <= FOOD_FULL)
      cond_delete_group(&player.conditions, CONDGRP_FOOD);
   else if(player.nutr <= FOOD_SATIATED)
      cond_add(&player.conditions, CONDIT_SATIATED, 1);
   else
      cond_add(&player.conditions, CONDIT_BLOATED, 1);
   

}

/* Food notes:
 *
 * Basic food gathering without any skill could be 5% of the corpse
 * size, rations/other "real" food items of course 100% all time even
 * without the skill.
 *
 * When "food gathering" skill increases:
 *
 * Skill
 * 0  .. 19    5% of the corpse
 * 20 .. 39   10%
 * 40 .. 59   15%
 * 60 .. 79   20%
 * 80 .. 89   25%
 * 90 .. 100  30%
 *
 * Skill increasing:
 *
 */

void eat_passturns(level_type *level, item_def *item)
{
   int16u movecount, skill;
   real suc=0.03;
   int32u weight=0;

   if(item->group==FOOD_CORPSE) {

      /* test for food gathering skill */
      skill=skill_check(player.skills, SKILLGRP_GENERIC, SKILL_FOODGATHER);

      suc += 0.003 * skill;

      /* now test for skill levels */
      weight = (int32u) ((real)item->weight * suc);

      /* modify skill marks */
      skill_modify_raise(&player.skills, SKILLGRP_GENERIC, SKILL_FOODGATHER, 
			 1, 1);
   }
   else
      weight = item->weight;

   movecount=1+weight/WEIGHT_KILO;

   eat_addnutr(item, weight);

   for(int i=0; i<movecount; i++) {
      player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), TIME_EATKILO, player.tactic);

      msg.setdelay(100);
      msg.newmsg(eatmsg[ RANDU(NUM_EATMSG) ], C_WHITE);
      msg.setdelay(0);
      game_passturn(level, true, false);
   }

   msg.vnewmsg(C_WHITE, "That was worth %4.2fkg of food.", 
	       (real)weight/WEIGHT_KILO);

}

/* DROP item (or sell if drop in a shop
 *
 * ** FIX **
 * THERE'S A BUG here if the actual DROP fails but the shopkeeper agrees
 * to buy the item. This is because shopkeeper will first buy the item and
 * then this routine tries to drop it -> if drop fails, the player has
 * got the money from shopkeeper and the item he/she just sold! 
 * ** FIX **
 */
bool player_dropitem_inner(Tinvpointer dropit)
{

//   Tinvpointer dropit;
//   Tinvpointer newitem;
   level_type *level;
//   int32u oldw;

   int32u count;
   bool sellmode=false;

   level=c_level;
//   oldw=player.inv.weight;

//   dropit=inv_listitems(&player.inv, "Drop what?", -1, 
//			  true, -1, -1);

   if(dropit) {
      if(dropit->slot >= 0) {
	 msg.vnewmsg(C_YELLOW, "Item %s %s is in use, unequip it first!", 
		     materials[dropit->i.material].name, dropit->i.name );
	 return true;
      }

      count=dropit->count;
      if(dropit->count > 1 && !CONFIGVARS.droppiles) {
	 game_noticeevents(level);

	 msg.vadd(C_GREEN, "How many %s's to drop [0..%ld, ENTER for all]?", 
		  dropit->i.name, dropit->count);
	 my_gets(tempstr, 6);

	 if(!strlen(tempstr) || !my_stricmp(tempstr, "all") )
	    count=dropit->count;
	 else
	    count=atoi(tempstr);

	 if(count==0) {
	    msg.newmsg("Nothing dropped.", C_WHITE);
	    return true;
	 }
	 if(count > dropit->count) {
	    msg.vnewmsg(C_WHITE, "But you only have %ld %s's.", 
		       dropit->count, dropit->i.name);
	    
	    count=dropit->count;
	 }
      }

      msg.update(true);
      
      dropit = inv_transfer2inv(level, &player.inv, player.equip, &level->inv,
		       dropit, count, player.pos_x, player.pos_y);

      if(dropit) {
	 if(dropit->i.status & ITEM_UNPAID)
	    player.bill -= count * dropit->i.price;

	 player.timetaken-=calc_time(get_stat( &player.stat[STAT_SPD]), 
				     TIME_DROPITEM, player.tactic);

	 if(player.inroom>=0 && !(dropit->i.status & ITEM_UNPAID)) {
	    if( level->rooms[player.inroom].type == ROOM_SHOP) {
	       game_noticeevents(level);
	       if(shopkeeper_buy(level, level->rooms[player.inroom].owner, 
				 dropit))
		  sellmode=true;
	    }
	 }

	 if(sellmode) {
	    item_printinfo(&dropit->i, dropit->i.weight, count, 
			   "You just sold");
	 }
	 else
	    item_printinfo(&dropit->i, dropit->i.weight, count, "You drop");
      }
      else {
	 msg.newmsg("You kept the item.", 
		    C_WHITE);
      }

      return true;
   }

   return false;
}

void player_dropitem(void)
{
   Titemlistptr multilist;
   int16u i;

   multilist = inv_listitems_multi(&player.inv, "Select items to drop!", -1, 
				   true, true, -1, -1);
   if(multilist) {
      i = 0;
      /* go through the selection list */
      while(multilist[i].ptr != NULL) {
	 if( !player_dropitem_inner( multilist[i].ptr )) {
	    break;
	 }
	 i++;
      }
      /* release the multilist */
      inv_freemultilist(&multilist);
   }
}

int32s player_getitem_inner(level_type *level, Tinvpointer lptr)
{
   int32u count;
   int32u carrycap;

// check that player really did selected something
   if(!lptr) {
      return -1;
   }

   count=1;
   if(lptr->count > 1) {
      game_noticeevents(level);

      if(lptr->i.type == IS_MONEY && CONFIGVARS.getallmoney) {
	 count=lptr->count;
      } else {
	 msg.vadd(C_GREEN, "How many %s's to get [0..%ld, ENTER for all]?", 
		  lptr->i.name, lptr->count);
	 my_gets(tempstr, 6);

	 if(!strlen(tempstr) || !my_stricmp(tempstr, "all") )
	    count=lptr->count;
	 else
	    count=atoi(tempstr);
      }
   }

   if(count==0) {
      msg.vnewmsg(C_WHITE, "Item %s %s was not taken.",
		  materials[lptr->i.material].name, lptr->i.name);
      return 0;
   }

   if(count > lptr->count) {
      msg.vnewmsg(C_WHITE, "There are only %ld %s's!", 
		  lptr->count, lptr->i.name);

      count=lptr->count;
   }

   carrycap=calc_carryweight(player.stat, player.weight);

   if((player.inv.weight + count * item_calcweight(&lptr->i)) > carrycap ) {

      if(count==1) {
	 msg.vnewmsg(C_WHITE, "The %s %s weights too much for you.", 
		     materials[lptr->i.material].name, lptr->i.name);
      }
      else
	 msg.vnewmsg(C_WHITE, "Those %d %s %s's weight too much for you.", 
		     count, materials[lptr->i.material].name, lptr->i.name);
      return 0;
   }

   lptr = inv_transfer2inv(level, &level->inv, NULL, &player.inv, 
			   lptr, lptr->count, 0, 0);
   if(lptr) {
      if(lptr->i.status & ITEM_UNPAID)
	 player.bill += count * lptr->i.price;

      item_printinfo(&lptr->i, lptr->i.weight, count, "You took");
   }
   else {
      msg.newmsg("Error: Can't get item, item transfer failed!", CHB_RED);
      return -1;
   }

   return count;
}

void player_getitem(void)
{
   Titemlistptr multilist;
   level_type *level = c_level;
   Tinvpointer lptr;
   int32u i;

   i=inv_countitems(&level->inv, -1, player.pos_x, player.pos_y, false);

   if(!i) {
      msg.newmsg("You fill your pockets with dust...", C_WHITE);
      return;
   }

   /* if there're multiple items, call up a inventory lister */
   /* otherwise get the item directly */
   if(i > 1 ) {
      multilist = inv_listitems_multi(&level->inv, "Select items to get!", -1, 
				      true, true, player.pos_x, player.pos_y);
      if(multilist) {
	 i = 0;

	 /* go through the selection list */
	 while(multilist[i].ptr != NULL) {
	    if( player_getitem_inner( level, multilist[i].ptr ) < 0) {
	       break;
	    }
	    i++;
	 }
	 /* release the multilist */
	 inv_freemultilist(&multilist);
      }

   }
   else {
      lptr=giveiteminfo(level, player.pos_x, player.pos_y);
      if(lptr)
	 player_getitem_inner( level, lptr );
   }
}

/* 
 * this will be called every turn, handles items in inventory
 * Like rotting food etc...
 *   calculates also the weight of inventory and equipped items
 */
void player_handleitems(void)
{
   Tinvpointer startptr;

   real oldgold=0;
   int16s decaytime;

   decaytime=get_stat(&player.stat[STAT_SPD]);

   startptr=player.inv.first;

//   oldgold=player.goldamount;
//   player.goldamount=0;

   while(startptr) {
      /* count money, but not equipped money */
      if(startptr->i.type == IS_MONEY && startptr->slot<0) {
	 /* clear status bits if money item */
	 /* pay-routine would break if this is not done */
	 startptr->i.status = 0;
//	 player.goldamount += 
//	    valuables[startptr->i.group].value * startptr->count;
      }

      startptr=startptr->next;
   }

   /* money needs a redraw */
/*
   if(oldgold!=player.goldamount) {
      GAME_NOTIFYFLAGS |= GAME_MONEYCHG;
   }
*/

}

bool item_isweapon(item_def *item)
{

   if(item->type == IS_WEAPON1H)
      return true;
   if(item->type == IS_WEAPON2H)
      return true;
   if(item->type == IS_MISWEAPON)
      return true;
   if(item->type == IS_MISSILE)
      return true;

   return false;


}

bool item_isarmor(item_def *item)
{
   if(item->type == IS_SHIELD)
      return true;
   if(item->type == IS_ARMOR)
      return true;

   return false;
}

/*
** This prints out some info about the item, based on the
** idenfiy status...
*/
void item_printinfo(item_def *iptr, int32u weight, int32u count, char *acttxt)
{
   int32u gcount=0;
   int32u scount=0;
   int32u ccount=0;

   i_stattxt[0]=0;
   i_identxt[0]=0;
   i_piletxt[0]=0;
   i_manytxt[0]=0;
   i_hugetmp[0]=0;
   i_pricetxt[0]=0;

   /* if it's a container, modify the weight */
   if(iptr->inv != NULL) {
      weight += iptr->inv->weight;
   }

   if(iptr->status & ITEM_UNPAID) {
      shopkeeper_coppervalue(iptr->price, &gcount, &scount, &ccount);
      i_identxt[0]=0;

      strcat(i_pricetxt, "(");
      if(gcount) {
	 sprintf(i_identxt, "%ldg", gcount);
	 strcat(i_pricetxt, i_identxt);
      }
      if(scount) {
	 if(gcount)
	    strcat(i_pricetxt, ",");
	 sprintf(i_identxt, "%lds", scount);
	 strcat(i_pricetxt, i_identxt);
      }
      if(ccount) {
	 if(gcount || scount)
	    strcat(i_pricetxt, ",");
	 sprintf(i_identxt, "%ldc", ccount);
	 strcat(i_pricetxt, i_identxt);
      }
      strcat(i_pricetxt, ")");
      i_identxt[0]=0;
   }
   
   if((iptr->status & ITEM_IDENTIFIED) ) {
      if((iptr->status & ITEM_BLESSED) ) {
	 sprintf(i_stattxt, "blessed ");
	 my_setcolor(CH_GREEN);
      }
      if((iptr->status & ITEM_CURSED) ) {
	 sprintf(i_stattxt, "cursed ");
	 my_setcolor(CH_RED);
      }
      /* show modifiers */
      if(iptr->melee_dt >0 || iptr->melee_ds || iptr->meldam_mod >0) {
	 sprintf(tempstr, "[%dd%d,%+2d] ",
		 iptr->melee_dt, iptr->melee_ds, iptr->meldam_mod);
	 strcat(i_identxt, tempstr);
      }

      if(iptr->missi_dt >0 || iptr->missi_ds || iptr->misdam_mod >0) {
	 sprintf(tempstr, "{%dd%d,%+2d} ",
		 iptr->missi_dt, iptr->missi_ds, iptr->misdam_mod);
	 strcat(i_identxt, tempstr);
      }

      if(iptr->ac>0) {
	 sprintf(tempstr, "(AC%+2d)", iptr->ac);
	 strcat(i_identxt, tempstr);
      }

      if(!acttxt) {
	 sprintf(tempstr, "%s %4.2fkg",
		 i_pricetxt, (real)(count*weight)/WEIGHT_KILO);
	 strcat(i_identxt, tempstr);
      }
      else {
	 strcat(i_identxt, i_pricetxt);
      }
   }
   else {
      if(!acttxt) {
	 sprintf(i_identxt, 
		 "%s %4.2fkg", i_pricetxt, (real)(count*weight)/WEIGHT_KILO);
      }
      else
	 sprintf(i_identxt, "%s", i_pricetxt);
   }

   if(count>1) {
      sprintf(i_piletxt, "a pile of %ld ", count);
      sprintf(i_manytxt, "s");
   } else
      sprintf(i_piletxt, "a ");

   if( (iptr->type==IS_FOOD) ) {
      if((iptr->status & ITEM_IDENTIFIED) ) {
	 sprintf(tempstr, "%s%s%s%s (%s)", 
		 i_piletxt, i_stattxt, iptr->name, 
		 i_manytxt, food_condition[iptr->icond]);
      }
      else {
	 sprintf(tempstr, "%s%s%s%s", 
		 i_piletxt, i_stattxt, iptr->name, i_manytxt);
      }
   }
   else if( (iptr->type==IS_SCROLL) ) {
      if(list_scroll[iptr->group].flags & SCFLAG_IDENTIFIED)
	 iptr->status|= ITEM_IDENTIFIED;

      if((iptr->status & ITEM_IDENTIFIED) ) {
	 sprintf(tempstr, "%s%s%s%s of %s (\"%s\")", 
		 i_piletxt, i_stattxt, 
		 iptr->name, i_manytxt,
		 iptr->rname,
		 iptr->sname);
      }
      else {
	 sprintf(tempstr, "%s%s %s%s labeled \"%s\"", 
		 i_piletxt, materials[iptr->material].name, 
		 iptr->name, i_manytxt,
		 iptr->sname);

	 if(list_scroll[iptr->group].flags & SCFLAG_NAMED) {
	    strcat(tempstr, " (\"");
	    strcat(tempstr, list_scroll[iptr->group].cname);
	    strcat(tempstr, "\")");
	 }
	 else if(list_scroll[iptr->group].flags & SCFLAG_TRIED)
 	    strcat(tempstr, " {tried}");
      }
   }
   else if( (iptr->type==IS_MONEY) ) {
      sprintf(tempstr, "%s%s%s%s", i_piletxt, i_stattxt,
	      iptr->name, i_manytxt);
   }
   else if( (iptr->type==IS_SPECIAL) ) {
	 sprintf(tempstr, "%s %s%s%s", 
		 i_piletxt, i_stattxt, 
		 iptr->name, i_manytxt);
   }
   else {
      if(iptr->material>=0) {
	 sprintf(tempstr, "%s%s %s%s %s%s", 
		 i_piletxt, condition[iptr->icond], i_stattxt, 
		 materials[iptr->material].name, iptr->name, i_manytxt);
      }
      else {
	 sprintf(tempstr, "%s%s %s%s%s", 
		 i_piletxt, condition[iptr->icond], i_stattxt, 
		 iptr->name, i_manytxt);
      }
   }

   if(!acttxt) {
      my_printf(tempstr);
      my_gotoxy(SCREEN_COLS-strlen(i_identxt), my_gety());
      my_printf(i_identxt);
   }
   else {
      sprintf(i_hugetmp, "%s %s%s!", acttxt, tempstr, i_identxt);
      msg.newmsg(i_hugetmp, C_WHITE);
   }
}

char *wallhitmsg[]=
{
     "Bonk!",
     "Kilk!",
     "Wham!",
     "Zabam!",
     "Ouch.",
     NULL
};

void player_hitwall(level_type *level)
{
     player.hitwall++;

     if(wallhitmsg[player.hitwall]==NULL) player.hitwall=0;

     msg.newmsg(wallhitmsg[player.hitwall], 1+RANDU(15));
}

/* skills = pointer to skill list
	diff = difficulty (1..10)

*/
int16u calculate_meleehit(item_def *iptr,
	_monsterlist *attack, _monsterlist *target, int8u bodypart)
{
   Tskillpointer askills, tskills;
   _Tequipslot *tequip;

   item_def *defwpn1, *defwpn2;
   bool useleft, useright;

   int16s wpnbonus;
   int16s aluck, tluck, i, tdv=0;
   int16s defskill, attskill;
   int16u trace;

   int8u ttactic, atactic;

   /* JOS KŽDESSŽ OLEVA ESINE ON JOTAIN MUUTA KUIN ASE
      NIIN EI SKILLEJŽ PIDŽ KASVATTAA!!!
      Nyt kasvaa handskilli aina silloin */

   /* attacker is player if 'attack'==NULL */
   if(attack==NULL) {
      aluck=get_stat( &player.stat[STAT_LUC] );
      askills=player.skills;
      atactic=player.tactic;
   }
   else {
      aluck=get_stat( &attack->stat[STAT_LUC] );
      askills=attack->skills;
      atactic=attack->tactic;
   }

   /* if target is NULL, then it's player */

   if(target==NULL) {
      tequip=player.equip;
      tskills=player.skills;
      tluck=get_stat( &player.stat[STAT_LUC] );
      decide_meleeweapon(player.equip, &useleft, &useright);
      trace=player.prace;
      ttactic=player.tactic;
//      tdv=calculate_slot_pv(bodypart, player.inv.equip );
   }
   else {
      tequip=target->equip;
      tskills=target->skills;
      tluck=get_stat( &target->stat[STAT_LUC] );
      decide_meleeweapon(target->equip, &useleft, &useright);
      trace=target->m.race;
      ttactic=target->tactic;
//      tdv=calculate_slot_pv(bodypart, target->inv.equip );
   }

   /* determine attacker weapon bonuses */
   i=SKILL_HAND;
   wpnbonus=0;

   if(iptr) {
      if(iptr->type==IS_WEAPON1H || iptr->type==IS_WEAPON2H
	 || iptr->type==IS_MISWEAPON ) {
	 i=iptr->group;

	 /* material bonus to hit */
	 wpnbonus=materials[iptr->material].hit;
      }
      else i=-1;
   }

   /* determine defender weapon skill */
   /* if no weapon in hand, then it must be determined
      from "dodging" etc? */
   if(useright) {
      defwpn1=&tequip[EQUIP_RHAND].item->i;
      defwpn2=NULL;
   }
   else if(useleft) {
      defwpn1=&tequip[EQUIP_LHAND].item->i;
   }
   else
      defwpn1=NULL;

   /* if defender skill missing THEN NO bonuses */
   defskill=0;
   if(defwpn1) {
      if(defwpn1->type==IS_WEAPON1H || defwpn1->type==IS_WEAPON2H) {
	 defskill=skill_check(tskills, SKILLGRP_WEAPON, defwpn1->group);
      }
   }

   attskill=0;
   if(i>=0)
      attskill=skill_check(askills, SKILLGRP_WEAPON, i);

   /* add luck modifiers from target and destination */
   attskill+=(aluck - 10)-(tluck - 10)+wpnbonus;

   /* substract half of defender weapn skill */
   attskill-=(defskill/2);

   /* every difficulty point substracts 2 points from hit */
   attskill-=(2 * npc_races[trace].bodyparts[bodypart] );

   /* tactic effects */
   attskill+=tacticeffects[atactic].hit;

   /* defender tactic effects */
   attskill-=tacticeffects[ttactic].dv;

   /* defender bodypart DV */
   attskill-=tdv;

   /* attacker has always some probability to hit */
   /* I use 5% */
   if(attskill < MIN_SKILLSCORE)
      attskill=MIN_SKILLSCORE;

//	sprintf(tempstr, "%d %d Ds: %d. As: %d.", atactic, ttactic, defskill, attskill);
//	msg.newmsg(tempstr, CH_WHITE);

   return attskill;


}

void melee_learnskills(Tskillpointer *skills, item_def *iptr, int8u amount)
{
   if(!iptr) {
      skill_modify_raise(skills, SKILLGRP_WEAPON, SKILL_HAND, amount, 
			 MIN_SKILLSCORE);
   }
   else {
      if(iptr->type==IS_WEAPON1H || iptr->type==IS_WEAPON2H
	 || iptr->type==IS_MISWEAPON ) {
	 skill_modify_raise(skills, SKILLGRP_WEAPON, iptr->group, amount, 
			    MIN_SKILLSCORE);
      }
   }
}

/* inner routine for player melee attack

	here decide what bodypart to hit, if hit at all etc
*/
bool player_meleeinner(level_type *level, _Tequipslot *eqptr,
		       int16s damage, _monsterlist *mptr)
{
   int16u crit;

   /* array for target bodyparts */
   int8s bparts[HPSLOT_MAX+1];

   int8u tslot, i, j;
   int16s hitresult, inttotal;
   real hittotal;

   if(eqptr) {

      /* check if the item broke */
      if(age_weapon(&eqptr->item->i, 
		    get_stat( &player.stat[STAT_LUC] ), true )==2 ){

	 if(eqptr->item->i.type==IS_LIGHT)
	    player.light=1;

	 inv_removeitem(&player.inv, player.equip, eqptr->item, -1);

/*
	 if(eqptr->item->i.type == IS_WEAPON2H) {
	    inv_removeequipped(&player.inv, eqptr->item->slot);

	    mem_clear(&player.inv.equip[EQUIP_RHAND], sizeof(_Tequipslot));
	    mem_clear(&player.inv.equip[EQUIP_LHAND], sizeof(_Tequipslot));
	 }
	 else {
	    inv_removeequipped(&player.inv, eqptr->item->slot);
	    mem_clear(eqptr, sizeof(_Tequipslot));
	 }
*/

	 return false;
      }
   }

   /* collect target bodyparts which the race has */
   for(j=0, i=0; i<HPSLOT_MAX; i++) {
      if(npc_races[mptr->m.race].bodyparts[i]>=0) {
	 bparts[j++]=i;
      }
   }

/*
  for(i=0; i<j; i++) {
  sprintf(tempstr, "%s", bodyparts[bparts[i]]);
  msg.newmsg(tempstr, C_GREEN);
  }
*/
   /* get a random target bodypart */
   tslot=bparts[RANDU(j)];

//   inttotal=calculate_meleehit(iptr, player.skills, get_stat( &player.stat[STAT_LUC]), npc_races[mptr->m.race].bodyparts[tslot], player.tactic );

   if(eqptr)
      inttotal=calculate_meleehit(&eqptr->item->i, NULL, mptr, tslot);
   else
      inttotal=calculate_meleehit(NULL, NULL, mptr, tslot);

   hittotal=(real)inttotal;

   /* 1..100 */
   hitresult=1+RANDU(100);

   damage+=tacticeffects[player.tactic].dam;
   if(damage<0)
      damage=0;

   if(hitresult > inttotal) {
      damage=0;
      msg.newmsg("You miss.", C_WHITE);
   }
   /* creat hit */
   else {
      /* normal hits give 1 learning mark */
      i=1;

#ifdef PASKAKOODIA
      if(hitresult > (hittotal*0.95) ) {
	 msg.newmsg("That was a great hit.", C_WHITE);
	 damage+=1+RANDU(6);

	 /* great hits give 2 marks */
	 i=2;
      }
      else if(hitresult == inttotal ) {
	 msg.newmsg("That was a PERFECT hit.", C_WHITE);
	 damage+=damage;

	 /* criticals give 4 marks */
	 i=4;
      }
#endif

      crit=skill_check(player.skills, SKILLGRP_GENERIC, SKILL_FINDWEAKNESS);
      crit=crit/4;

      if(throwdice(1,100,0) <= crit) {
	 i=4;
	 msg.newmsg("That was a critical hit(2xDAM)!", CH_WHITE);
	 damage+=damage;
      }

      /* now increase melee weaponskills learning counter */
      if(eqptr) {
	 melee_learnskills(&player.skills, &eqptr->item->i, i);
      }
      else
	 melee_learnskills(&player.skills, NULL, i);
   }

   if(damage==0) return false;

//   textify_damage(level, damage, mptr);

//   if(monster_takedamage(level, mptr, NULL, damage, tslot )<=0) {
   if(damage_issue(level, mptr, NULL, ELEMENT_NOTHING, 
		   damage, tslot, NULL)<=0) {
      player_killedmonster(mptr);

      return true;
   }

   return false;
}

void player_killedmonster(_monsterlist *mptr)
{
   int32u expgain;

   expgain = npc_races[mptr->m.race].exp * mptr->m.level;

   player.num_kills++;

   msg.vnewmsg(C_GREEN, "(%ld exp)!", expgain);

   gain_experience(NULL, NULL, expgain);   

   quest_checkkill(&player.quests, mptr);

   GAME_NOTIFYFLAGS|=GAME_EXPERCHG;
}




bool decide_meleeweapon(_Tequipslot *equip, bool *useleft, bool *useright)
{
   item_def *rweap, *lweap;

   *useleft = false;
   *useright = false;
   rweap=lweap=NULL;

   if(equip[EQUIP_RHAND].item) {
      rweap=&equip[EQUIP_RHAND].item->i;
   }

   if(equip[EQUIP_LHAND].item) {
      lweap=&equip[EQUIP_LHAND].item->i;
   }

   /* no weapons in hand then use hand damage */
   if(!rweap && !lweap) {
      return false;
   }
   
   if(rweap) {
      if( rweap->type==IS_WEAPON2H) {
	 *useright = true;
	 return true;
      }

      if( rweap->type==IS_WEAPON1H) {
	 *useright = true;
      }

   }

   if(lweap) {
      if( lweap->type==IS_WEAPON2H) {
	 *useleft = true;
	 return true;
      }

      if( lweap->type==IS_WEAPON1H) {
	 *useleft = true;
      }

   }

   return true;
}

void player_meleeattack(level_type *level, _monsterlist *mptr)
{
   bool doattack=false;

   item_def *iptr;

   int16u damage=0, dmod=0;
   int16s timel;
   int8u dices=0, dicet=0;

   bool lefthand, righthand;

   if( !(mptr->m.status & MST_ATTACKMODE) && 
       haslight(level, mptr->x, mptr->y) ) {
      monster_sprintf(nametemp, mptr, false, false );
      sprintf(tempstr, "Really attack %s", nametemp);

      doattack=confirm_yn(tempstr, false, true);

   }
   else doattack=true;

   if(doattack) {
      /* decide weapon to use in attack */
      decide_meleeweapon(player.equip, &lefthand, &righthand);

      if(lefthand) {
	 /* subtract time taken by attack */
	 player.timetaken-=calc_time(get_stat( &player.stat[STAT_SPD]), 
				     TIME_MELEEATTACK, player.tactic);

	 iptr=&player.equip[EQUIP_LHAND].item->i;

	 dmod=iptr->meldam_mod;
	 dices=iptr->melee_ds;
	 dicet=iptr->melee_dt;

	 damage=throwdice(dicet, dices, dmod);

	 if(player_meleeinner(level, &player.equip[EQUIP_LHAND], 
			      damage, mptr))
	    return;
      }

      if(righthand) {
	 timel=player.timetaken-calc_time(get_stat(&player.stat[STAT_SPD]), 
					  TIME_MELEEATTACK, player.tactic);

	 if(timel<=0)
	    return;

	 player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), 
				     TIME_MELEEATTACK, player.tactic);

	 iptr=&player.equip[EQUIP_RHAND].item->i;

	 dmod=iptr->meldam_mod;
	 dices=iptr->melee_ds;
	 dicet=iptr->melee_dt;

	 damage=throwdice(dicet, dices, dmod);
	 if(player_meleeinner(level, &player.equip[EQUIP_RHAND], 
			      damage, mptr))
	    return;

      }

      /* do hand damage if no items */
      if(!righthand && !lefthand ) {
	 player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), 
				     TIME_MELEEATTACK, player.tactic);
	 dmod=0;
	 dices=2;
	 dicet=2;

	 damage=throwdice(dicet, dices, dmod);
	 msg.newmsg("Hand attack!", C_RED);

	 if(player_meleeinner(level, NULL, damage, mptr))
	    return;
      }

   }
}

void player_rangedattack(void)
{
   level_type *level;
   int16s t_lx, t_ly, t_sx, t_sy;
   int16u outchar=0;
   bool misres;

   int16u agrp, askill, skillval;
   int16u itype, igroup;

   level=c_level;

   if(!get_rangedskill(NULL, &agrp, &askill))
      return;

   if(!ranged_gettarget(level, &t_sx, &t_sy, &t_lx, &t_ly)) {
      msg.newmsg("No target selected.", C_WHITE);
      return;
   }

   showplayer(level);

   itype=player.equip[EQUIP_MISSILE].item->i.type;
   igroup=player.equip[EQUIP_MISSILE].item->i.group;

   skillval=skill_check(player.skills, agrp, askill);

   /* give atleast some chance to hit the target */
   if(!skillval) 
	skillval = 1 + RANDU(6);

   /* OBS! 
    *
    * Etäisyysvaikutus ja atribuuttien vaikutus osuma
    * tarkkuuteen ja ampumaetäisyyteen
    *
    */

   msg.vnewmsg(C_WHITE, "Skill value is %d!", skillval);

   if(player.equip[EQUIP_MISSILE].item->i.type==IS_MISSILE) {
      outchar=0;
      my_setcolor(CH_WHITE);
   }
   else {
      outchar=gategories[player.equip[EQUIP_MISSILE].item->i.type].out;
      my_setcolor( materials[player.equip[EQUIP_MISSILE].item->i.material].color);
   }
   hidecursor();
//   ranged_line(level, 0, 60, true,
//	       MAPWIN_RELX+player.spos_x, MAPWIN_RELY+player.spos_y,
//	       t_sx, t_sy, NULL);

   misres=ranged_line(level, outchar, 60, true,
	       player.pos_x, player.pos_y,
	       t_lx, t_ly, ranged_checkhit_player, NULL, skillval);

   showcursor();

   /* if we didn't hit anything, we need to remove the item here! */
   if(!misres) {
      msg.newmsg("You missed everything.", C_WHITE);

      inv_transfer2inv(level, &player.inv, player.equip,
		       &level->inv, player.equip[EQUIP_MISSILE].item, 1,
		       t_lx, t_ly);
   }

   /* equip a new item from reserve if possible */

   if(!ready_newmissile(&player.inv, player.equip, itype, igroup))
      msg.newmsg("You've run out of missiles.", C_WHITE);

   player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), 
			       TIME_MISSILEATTACK, player.tactic);

}


/* chat with monster */
void player_chat(void)
{
   int16u nx, ny;
   int8u dir=0;
   _monsterlist *mptr;
   level_type *level;

   nx=player.pos_x;
   ny=player.pos_y;
   level=c_level;
   dir=dir_askdir("Talk to", true);

   if(dir>9) return;
   nx+=move_dx[dir];
   ny+=move_dy[dir];

   if(nx==player.pos_x && ny==player.pos_y)
      msg.newmsg("You chat deeply with yourself!", C_WHITE);
   else {
      mptr=istheremonster(level, nx, ny);
      if(mptr) {
	 monster_talk(mptr);
      }
      else
	 msg.newmsg("You mumble by yourself...", C_WHITE);
   }
}

void player_opendoor(void)
{
   level_type *level;
   int16u nx, ny;
   int8u dir;

   nx=player.pos_x;
   ny=player.pos_y;
   level=c_level;

   dir=dir_askdir("Open door -", true);
   if(dir>9) return;
   nx+=move_dx[dir];
   ny+=move_dy[dir];

   if(nx==player.pos_x && ny==player.pos_y) {
      msg.newmsg("You have lost the key...", C_WHITE);
   }
   else if(door_open(level, nx, ny, NULL)==DOORSTAT_SUCCESS) {
      player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]), TIME_OPENDOOR, player.tactic);


   }

}

void player_closedoor(void)
{
   level_type *level;

   int16u nx, ny;
   int8u dir;

   nx=player.pos_x;
   ny=player.pos_y;

   level=c_level;

   dir=dir_askdir("Close door -", true);
   if(dir>9) return;
   nx+=move_dx[dir];
   ny+=move_dy[dir];

   if(nx==player.pos_x && ny==player.pos_y) {
      msg.newmsg("Close your mouth.", C_WHITE);
   }
   else if(door_close(level, nx, ny, NULL)==DOORSTAT_SUCCESS) {
      player.timetaken-=calc_time(get_stat(&player.stat[STAT_SPD]),
				  TIME_CLOSEDOOR, player.tactic);

   }
}


/* allows player to select a location with cursors */
void player_locationselect(level_type *level)
{
   int16s tx, ty, ltx, lty;
   int16u keycode, dir;

   int32u items;
   _monsterlist *mptr=NULL;
   Tinvpointer iptr=NULL;

   bool monsterthere=false;

   ltx=tx=player.spos_x;
   ltx=ty=player.spos_y;

   clearline(MSGLINE);
   my_gotoxy(1, MSGLINE);
   my_setcolor(C_WHITE);
   msg.newmsg("Use movement keys for location, quit with ESC or ENTER.", C_WHITE);
   clearline(MSGLINE+1);

   keycode='5';
   while(1) {
      dir=5;

      if(keycode==KEY_ESC || keycode==PADENTER || keycode==KEY_ENTER || keycode==10)
	 break;
      if(keycode==KEY_UP) dir=8;
      if(keycode==KEY_DOWN) dir=2;
      if(keycode==KEY_LEFT) dir=4;
      if(keycode==KEY_RIGHT) dir=6;
      if(keycode>='1' && keycode <='9') {
	 dir=keycode-'0';
      }

      if(keycode==' ' && monsterthere ) {
	 if(mptr) {
	    monster_showdesc(mptr);
	    do_redraw(level);
	    my_setcolor(C_WHITE);
	    msg.newmsg("Use movement keys for location, quit with ESC or ENTER.", C_WHITE);
	 }
	 else
	    msg.newmsg("No info.", C_WHITE);
      }
      else if(dir) {
	 msg.update(true);

	 /* last coordinates */
	 ltx=tx;
	 lty=ty;

	 /* make new coordinates */
	 tx+=move_dx[dir];
	 ty+=move_dy[dir];

	 /* do not allow beyond map window */
	 if(tx<1) tx=1;
	 if(ty<1) ty=1;
	 if(tx>MAPWIN_SIZEX) tx=MAPWIN_SIZEX;
	 if(ty>MAPWIN_SIZEY) ty=MAPWIN_SIZEY;

	 /* check if that location is visible */

	 if(!(level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].flags & CAVE_LOSTMP)) {
	    tx=ltx;
	    ty=lty;
	 }
	 showplayer(level);
//	 show_at(level, player.lreg_x+tx-1, player.lreg_y+ty-1);

	 my_gotoxy(MAPWIN_RELX+tx, MAPWIN_RELY+ty);
	 my_cputch('*', CH_RED);
	 my_gotoxy(1, MSGLINE+1);
	 mptr=istheremonster(level, player.lreg_x+tx-1, player.lreg_y+ty-1);

	 items=inv_countitems(&level->inv, -1, player.lreg_x+tx-1, player.lreg_y+ty-1, false);
	 monsterthere=false;
	 if(mptr) {
	    monster_sprintf(nametemp, mptr, true, true );
	    msg.vnewmsg(C_RED, "%s. (More info with SPACE)", nametemp);
	    monsterthere=true;
	 }
	 else if(items) {
	    if( items==1 ) {
	       iptr=giveiteminfo(level, player.lreg_x+tx-1, player.lreg_y+ty-1);
	       item_printinfo(&iptr->i, iptr->i.weight, iptr->count, "It's");
	    }
	    else {
	       msg.newmsg("A pile of items.", C_WHITE);
	    }
	 }
	 else {
//	    sprintf(tempstr, "%s.", terrains[level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].type].desc);
	    msg.vnewmsg(C_WHITE, "%s.", 
		  terrains[level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].type].desc);
	 }

	 if(level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].trap != TRAP_NOTRAP &&
	    level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].flags & CAVE_TRAPIDENT ) {
	    
	    msg.vnewmsg(CH_RED, "A %s trap.", 
		list_traps[ level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].trap]);
	 }
#ifndef SALADIR_RELEASE
/*
	    if(level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].flags & CAVE_NOLIT)
	       msg.newmsg("UNLIT!!", CH_RED);
	    if(level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].flags & CAVE_MONSTERLIGHT)
	       msg.newmsg("monsterlight!!", CH_RED);
	    if(level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].flags & CAVE_LOSTMP)
	       msg.newmsg("LOS can see!!", CH_RED);
	    if(level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].flags & CAVE_TMPLIGHT ||
	       level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].flags & CAVE_MONSTERLIGHT)
	       msg.newmsg("Has tmplight!", CH_YELLOW);
*/
#endif


      }
      msg.notice();
      my_gotoxy(MAPWIN_RELX+tx, MAPWIN_RELY+ty);
      keycode=my_getch();
   }
}

void player_lookdir(void)
{
   int16u nx, ny;
   int8u dir;
   _monsterlist *mptr;
   level_type *level;

   level=c_level;

   player_locationselect(level);
   return;
   nx=player.pos_x;
   ny=player.pos_y;
   dir=dir_askdir("Look at", true);
//   dir_giveposition(dir, &nx, &ny);
   if(dir>9) return;
   nx+=move_dx[dir];
   ny+=move_dy[dir];

   if(nx==player.pos_x && ny==player.pos_y)
      msg.newmsg("What an ugly adventurer here!", C_WHITE);
   else {
      mptr=istheremonster(level, nx, ny);
      if(mptr) {
	 monster_showdesc(mptr);
      }
      else
	 msg.newmsg("Nothing there!", C_WHITE);

      GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
      //do_redraw(level);
   }

}

void player_equip_show(void)
{
   sint i;

   my_clrscr();
   my_setcolor(CH_WHITE);
   my_cputs(1, "Equipment in use");
   my_setcolor(CH_DGRAY);
   drawline(2, '=');
   my_gotoxy(1, 3);

   for(i=0; i<MAX_EQUIP; i++) {
      my_setcolor(C_GREEN);

      if(i==0)
	 my_printf("Helmet, crown & jewelry\n");
      if(i==4)
	 my_printf("Weapons & Tools\n");
      if(i==8)
	 my_printf("Clothes, Protection & Armour\n");

      if(player.equip[i].status!=EQSTAT_OK) {
	 my_setcolor(CH_RED);
	 my_printf(" !%c) %-10s  broken or unusable!", 'A'+i, equip_slotdesc[i]);
      }
      else if(!player.equip[i].reserv) {
	 my_setcolor(CH_RED);
	 my_printf("  %c", 'A'+i);
	 my_setcolor(C_YELLOW);
	 my_printf(") %-10s ", equip_slotdesc[i]);
	 my_setcolor(C_WHITE);
	 my_printf(": ");
	 if(!player.equip[i].item) {
	    my_printf("no item");
	 }
	 else
	    item_printinfo(&player.equip[i].item->i, 
			   player.equip[i].item->i.weight, 1, NULL);
      }
      else {
	 my_setcolor(CH_DGRAY);
	 my_printf("  %c) %-10s  reserved by another item!", 
		   'A'+i, equip_slotdesc[i] );
      }

      my_printf("\n");
   }
   my_setcolor(CH_DGRAY);
   drawline(SCREEN_LINES-2, '=');
   my_setcolor(CH_WHITE);
   my_cputs(SCREEN_LINES-1, "Select slot to access      [SPACE] to return");
  
}

int16s equip_checkfit(int16u slot, item_def *item)
{
   if(slot==EQUIP_MISSILE)
      return -1;

   if(slot==EQUIP_TOOL)
      return -1;

   if(slot==EQUIP_LRING || slot==EQUIP_RRING) {
      if(item->type==IS_RING)
	 return -1;
   }

   if(item->type == IS_MISSILE ) {
      if(slot==EQUIP_MISSILE || slot==EQUIP_RHAND || slot==EQUIP_LHAND)
	 return -1;      
   }
   else if(item->type == IS_ARMOR ) {
      if(slot==item->group)
	 return -1;
   }
   else if(item->type == IS_WEAPON1H || item->type == IS_MISWEAPON ) {
      if(slot==EQUIP_LHAND || slot==EQUIP_RHAND)
	 return -1;
   }
   else if(item->type == IS_WEAPON2H) {
      if( slot==EQUIP_LHAND && player.equip[EQUIP_RHAND].in_use==0) {
	 if(player.equip[EQUIP_RHAND].status==EQSTAT_OK)
	    return EQUIP_RHAND;
	 else
	    return -9;
      }
      if(slot==EQUIP_RHAND && player.equip[EQUIP_LHAND].in_use==0) {
	 if( player.equip[EQUIP_LHAND].status==EQSTAT_OK)
	    return EQUIP_LHAND;
	 else
	    return -9;
      }
      return -10;
   }

   /* any item can be used in hand */
   if(slot==EQUIP_LHAND || slot==EQUIP_RHAND)
      return -1;

   return -10;
}

int16s equipitem(int16u slot)
{
   Tinvpointer equipit, invnode;
   int16s res, eres;

   if(player.equip[slot].reserv)
      return -4;

   if(!player.equip[slot].item) {

      res=0;
      equipit=inv_listitems(&player.inv, "What do you want to equip?", -1, 
			false, -1, -1);
      if(equipit) {
	 if((equipit->i.status & ITEM_UNPAID))
	    return -7;

	 if(equipit->slot >= 0)
	    return -9;

	 eres=equip_checkfit(slot, &equipit->i);
	 
	 if(eres>0)
	    player.equip[eres].reserv=slot;

	 if(eres == -9)
	    return -6;

	 if(eres >= -1) {

	    /* split a piles */
	    invnode = inv_splitpile(&player.inv, equipit);

	    if(!invnode)
	       return -3;

	    /* mark item equipped to disable piling */
//	    invnode->i.status |= ITEM_EQUIPPED;

	    /* identify item when equipped */
	    if(item_isweapon(&invnode->i) ||
	       item_isarmor(&invnode->i))
	       invnode->i.status |= ITEM_IDENTIFIED;

	    invnode->slot = slot;

	    player.equip[slot].in_use=1;
	    player.equip[slot].item=invnode;

	    /* if equipping light, make it visible */
	    if(invnode->i.type==IS_LIGHT) {
	       if(invnode->i.group>=LIGHT_MAXNUM)
		  invnode->i.group=LIGHT_MAXNUM-1;
	       player.light=invnode->i.pmod1;
	    }


	    item_printinfo(&equipit->i, equipit->i.weight, 1, "Equipped");

//	    inv_removeitem(&player.inv, equipit, 1);
	    return 0;
	 }
	 else return -5;

      }
      else {
	 if(res==0) {
	    /* no items to equip */
	    return -1;
	 }
	 return -2;
      }
   } else {
      equipit=player.equip[slot].item;

//      equipit=inv_inititem(&player.inv);
//      if(equipit) {

      if(player.equip[slot].item->i.status & ITEM_CURSED) {
	 return -8;
      }

//      if(player.inv.equip[slot].item->i.type==IS_WEAPON2H) {
//	 if(slot==EQUIP_RHAND)
//	    player.inv.equip[EQUIP_LHAND].reserv=0;
//	 if(slot==EQUIP_LHAND)
//	    player.inv.equip[EQUIP_RHAND].reserv=0;

//      }

      if(player.equip[slot].item->i.type==IS_LIGHT) {
	 player.light=1;
      }

      /* mark item unequipped to enable piling again */
      equipit->slot=-1;

      /* free equip slot */
//      player.inv.equip[slot].in_use=0;
//      player.inv.equip[slot].item=NULL;

      inv_clearequipslot(&player.inv, player.equip, slot);

      item_printinfo(&equipit->i, equipit->i.weight, 1, "Removed");

      /* clear the memory of the slot */
//      mem_clear(&player.equip[slot].item, sizeof(item_def));

      return 1;
//   }
//   else
//      return -3;
   }

}

void player_equip(void)
{
   int16u key;
   int16s result;

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

   /* show equip slots */
   player_equip_show();

   key=1;
   while(key!=32) {
      key=my_getch();

      if(key>='A' && key <='A'+MAX_EQUIP-1)
	 key-='A';
      else if(key>='a' && key <='a'+MAX_EQUIP-1)
	 key-='a';
      else {
	 drawline(SCREEN_LINES, ' ');
	 continue;
      }

      if(key<MAX_EQUIP)
	 if(player.equip[key].status!=EQSTAT_OK) {
	    my_cputs(SCREEN_LINES, "That slot is unusable!");
	    continue;
	 }

      result=equipitem(key);

      player_equip_show();

      if(result==-1) {
	 my_setcolor(C_RED);
	 my_cputs(SCREEN_LINES, "You have no items to equip!");
      }
      else if(result==-2) {
	 my_setcolor(C_YELLOW);
	 my_cputs(SCREEN_LINES, "Nothing selected.");
      }
      else if(result==-3) {
	 my_setcolor(CHB_RED);
	 my_cputs(SCREEN_LINES, "Out of memory!");
      }
      else if(result==-4) {
	 my_setcolor(CH_YELLOW);
	 my_cputs(SCREEN_LINES, "That slot is reserved!");
      }
      else if(result==-5) {
	 my_setcolor(CH_YELLOW);
	 my_cputs(SCREEN_LINES, "Item doesn't fit there!");
      }
      else if(result==-6) {
	 my_setcolor(CH_RED);
	 my_cputs(SCREEN_LINES, "Can't use two handed weapons now!");
      }
      else if(result==-7) {
	 my_setcolor(CH_RED);
	 my_cputs(SCREEN_LINES, item_unpaymsg[RANDU(ITEM_UNPAYMSGNUM)]);
      }
      else if(result==-8) {
	 my_setcolor(CH_RED);
	 my_cputs(SCREEN_LINES, "This item is cursed, can't remove!");
      }
      else if(result==-9) {
	 my_setcolor(CH_RED);
	 my_cputs(SCREEN_LINES, "Item is already in use!");
      }
      else if(result==0) {
	 my_setcolor(C_GREEN);
	 sprintf(tempstr, "Equipped %s.", player.equip[key].item->i.name);
	 my_cputs(SCREEN_LINES, tempstr);
      }
      else if(result==1) {
	 my_setcolor(C_GREEN);
	 my_cputs(SCREEN_LINES, "Item removed.");
      }
      else {
	 my_setcolor(CHB_RED);
	 my_cputs(SCREEN_LINES, "Error! Unknown result code!");
      }
   }

   /* calculate effect from items */
   calculate_itembonus(NULL);
}


/*
** Calculates new position based on direction
** Used by many routines, like chat
*/
int8u dir_askdir(char *prompt, bool self)
{
   int8u dir=0;
   int16u key;

   if(self)
      msg.vadd(C_WHITE, "%s which direction [84261397.]?", prompt);
   else
      msg.vadd(C_WHITE, "%s which direction [84261397]?", prompt);
   key=my_getch();

   switch(key) {
      case '1':
      case '2':
      case '3':
      case '4':
      case '6':
      case '7':
      case '8':
      case '9':
	 dir=key-'0';
	 break;
      case '5':
      case '.':
	 dir=5;
	 break;
      case KEY_UP:
	 dir=8;
	 break;
      case KEY_DOWN:
	 dir=2;
	 break;
      case KEY_LEFT:
	 dir=4;
	 break;
      case KEY_RIGHT:
	 dir=6;
	 break;
      default:
	 dir=10;
	 msg.newmsg("Not a valid direction!", C_WHITE);
	 break;
   }

   msg.update(true);
   return dir;
}

void player_checkstat(level_type *level, bool lower, bool showmsg)
{
   real rhp;

   bool alarmi=false;
   player.hp=calculate_totalhp(player.hpp);

   /* check for hpslot healing */
   /* and hp warning */
   for(int8u i=0; i<HPSLOT_MAX; i++) {
      /* warn for low hitpoints */
      if(CONFIGVARS.health_alarm>0 && lower && showmsg
	 && (player.equip[eqslot_from_hpslot[i]].status==EQSTAT_OK)) {
	 rhp=(real)player.hpp[i].max;

	 rhp=(rhp/100.0) * CONFIGVARS.health_alarm;

	 if(player.hpp[i].cur <= (int16s)rhp) {
	    if(!alarmi) {
	       alarmi=true;
	       msg.newmsg("HPALARM!", CHB_RED);
	    }
	    msg.vnewmsg(C_RED, "Your %s is in bad condition!", bodyparts[i]);
	 }
      }

      if( (player.hpp[i].cur > 0) && 
	  (player.equip[eqslot_from_hpslot[i]].status!=EQSTAT_OK) ) {

	 /* put the slot back to work */
	 player.equip[eqslot_from_hpslot[i]].status=EQSTAT_OK;

	 if(i==HPSLOT_LEGS)
	    cond_delete(&player.conditions, CONDIT_BADLEGS);
	 else if(i==HPSLOT_LEFTHAND) 
	    cond_delete(&player.conditions, CONDIT_BADLARM);
	 else if(i==HPSLOT_RIGHTHAND) 
	    cond_delete(&player.conditions, CONDIT_BADRARM);

	 msg.vnewmsg(C_GREEN, "Your %s looks better!", bodyparts[i]);
      }
   }

   /* cheat, no die possible */
   if(CONFIGVARS.cheat)
      return;

   /* check for the hpslot damage */
   damage_checkbodyparts(level, NULL);

   if(player.hp <= 0) {
      msg.vnewmsg(C_RED, "You die... (%s).", player_killer);
      player.alive=false;
      cond_add(&player.conditions, CONDIT_DEAD, 1);
      return;
   }

}

/* returns the ARMOR ABSORB value for certain bodypart */
/* bodyparts

	HPSLOT_HEAD			0	helmet, cloak
	HPSLOT_LEFTHAND	1	arm armor, shirt, cloak, gloves, lring
	HPSLOT_RIGHTHAND	2  arm armor, shirt, cloak, gloves, lring
	HPSLOT_BODY			3  body armor, shirt, cloak
	HPSLOT_LEGS			4  leg armor, pants, boots
*/
#ifdef paskakoodia
int16s player_getmeleehit(level_type *level, char *message,
			  int16u damage, int8u bodypart, _monsterlist *monster)
{
   msg.newmsg("Calling old getmeleehit()!", CHB_RED);
   return player.hp;

   int16s absorb;
   bool armorprot=false;

   if(!monster)
      return player.hp;

   if(bodypart>=HPSLOT_MAX)
      bodypart=RANDU(HPSLOT_MAX);

//   absorb=calculate_slotac(bodypart, player.inv.equip );

   absorb=player.hpp[bodypart].ac;

   if(absorb>=damage) {
      msg.newmsg("Your armor protects you.", C_WHITE);
      damage=0;;
      return player.hp;
   }
   else {
      armorprot=true;
      damage-=absorb;
   }

   if(damage>0) {
      monster_sprintf(nametemp, monster, false, true);
      sprintf(player_killer, "Killed by %s.", nametemp);

      return player_takehit(level, damage, bodypart, 
			    message);
   }
   return player.hp;
}
#endif

void player_go_outworld(level_type **level)
{
   _dungeondef *dptr;

   level_type *newlevel;


   //   outworld_create(*level, MAXSIZEX, MAXSIZEY);

   /* try to load the wilderness map */
   newlevel=load_tmplevel(0, 0);
   if(newlevel!=NULL) {
      freelevel(*level);
      delete *level;

      *level=newlevel;
   }
   else {
      if(!chdir_todatadir()) {
	 msg.newmsg("Data directory unavailable", CHB_RED);
	 return;
      }

      // if wilderness hasn't been generated to D00L00.TMP file yet
      if(!edit_loadmap(*level, "outworld.map")) {
	 msg.newmsg("Couldn't enter outworld! ERROR!!!", CHB_RED);
	 return;
      }
   }

   DELTA=2;
   player.sight=4;

//   init_lineofsight(4);

   if(player.huntmode) {
      player.huntmode=false;
      player.pos_x=player.wildx;
      player.pos_y=player.wildy;
   }
   else {
      player.pos_x=player.levptr->outx;
      player.pos_y=player.levptr->outy;
   }
   player.spos_x=5;
   player.spos_y=5;
   player.lreg_x=player.pos_x-4;
   player.lreg_y=player.pos_y-4;
   if(player.pos_x<5)
      player.spos_x=player.pos_x+1;
   if(player.pos_y<5)
      player.spos_y=player.pos_y+1;

   /* enable weather notifications */
   worldtime.set_events(true);

   msg.newmsg("Entering outside...", C_GREEN);

   if(dungeonlist[0].desc)
      msg.newmsg(dungeonlist[0].desc, C_WHITE);

   /* 0 is the outworld "dungeon */
   player.dungeon=0;
   player.dungeonlev=0;
   player.levptr=dungeonlist[player.dungeon].levels;

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

   /* mark all known dungeon locations now */
   dptr=dungeonlist;
   while(dptr->name) {
//       if((dptr->flags & DUNGEON_KNOWN))
      if(dptr->x && dptr->y && dptr->out) {
	 (*level)->loc[dptr->y][dptr->x].type=dptr->out;
	 (*level)->loc[dptr->y][dptr->x].flags=terrains[dptr->out].flags;
      }
      dptr++;
   }
}

void searchstaircase(level_type *level, int8u stair, int8u ldir)
{
   int16u px=0, py=0, stop=0;
   int16u k=0, j=0;

   for(k=0; (k<level->sizey) && !stop; k++) {
      for(j=0; (j < level->sizex) && !stop; j++) {
	 if( (level->loc[k][j].type==stair) ) {
	    px=j;
	    py=k;

	    if(level->loc[k][j].doorfl==ldir) {
	       stop=1;
	    }
	 }
      }
   }

   if(px==0 && py==0) {
      msg.newmsg("Error! No staircases found?!?!", CHB_RED);
      while(1) {
	 px=RANDU(level->sizex);
	 py=RANDU(level->sizey);
	 if(level->loc[k][j].flags & CAVE_PASSABLE)
	    break;
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

/* marks all dungeons with index 'index' visited */
void visit_dungeon(int16u index)
{
   _dungeondef *dptr;

   bool visit=false;

   dptr=dungeonlist;
   while(dptr->name) {
      if(dptr->index == index && !(dptr->flags & DUNGEON_VISITED)) {
	 dptr->flags|=DUNGEON_VISITED;
	 visit=true;
      }
      dptr++;
   }  

   if(visit)
      player.num_places++;
}

void player_go_down(level_type **level)
{
   _dungeondef *dptr;
   level_type *newlevel;
   bool isdung;

   int16u i=0;
   int8u sdir=0;

//   init_lineofsight(10);

   /* save the current level first */
   if(!chdir_totempdir()) {
      msg.newmsg("Unable to access temporary directory!", CHB_RED);     
      return;
   }

   save_tmpfile(*level, dungeonlist[player.dungeon].index, player.dungeonlev);

   isdung=false;
   /* if player is currently in wilderness */
   if(player.dungeon==0) {
      /* search the dungeon player is entering (with coords) */
      dptr=dungeonlist;
      i=0;
      while(dptr->name) {
	 if(player.pos_x==dptr->x && player.pos_y==dptr->y) {

//	    player.lastdir=STAIROUT;
	      player.lastdir=dungeonlist[i].staircase;
//	    player.dungeon=dungeonlist[i].index;
	    player.dungeon=i;

	    player.levptr=dungeonlist[i].levels;

	    visit_dungeon(dungeonlist[i].index);

	    player.dungeonlev=player.levptr->index;

	    isdung=true;

	    msg.vnewmsg(C_GREEN, "Entering %s!", dptr->name);
	    if(dptr->desc)
	       msg.newmsg(dptr->desc, C_WHITE);

	    break;
	 }
	 i++;
	 dptr++;
      }
      if(!isdung) {
	 msg.newmsg("Want do start digging a new dungeon?", C_WHITE);
	 return;
      }
   }
   else {
      msg.newmsg("You entered down stairs...", C_YELLOW);

      if(player.lastdir==STAIRDOWN1) {
	 player.levptr=player.levptr->linkto1;
	 sdir=STAIRUP1;
      }
      if(player.lastdir==STAIRDOWN2) {
	 sdir=STAIRUP2;
	 player.levptr=player.levptr->linkto2;
      }

      player.dungeonlev=player.levptr->index;
   }

   player.sight=10;
   DELTA=6;

   /* check and set the visited flag for the level */
   if(!(player.levptr->flags & LEVEL_VISITED)) {
      player.num_levels++;
      player.levptr->flags |= LEVEL_VISITED;
   }

   /* if player is in dungeon */
   player.pos_x=player.pos_y=1;
   player.spos_x=2;
   player.spos_y=2;

   /* newlevel==NULL if no level loaded */

   /* try loading level from disk */

   newlevel=load_tmplevel(dungeonlist[player.dungeon].index, player.dungeonlev);
   if(newlevel!=NULL) {
      freelevel(*level);
      delete *level;

      *level=newlevel;
   }
   else {
      createlevel(*level);
   }

   worldtime.set_events(false);

   if(player.levptr->dtype==DTYPE_TOWN) {

      worldtime.set_events(true);

//      init_lineofsight(15);
      player.sight=15;

      for(int16s a=3; a<(*level)->sizey-4; a++) {
	 for(int16s b=3; b<(*level)->sizex-4; b++) {
	    if(ispassable(*level, b, a) &&
	       (*level)->loc[a][b].type!=TYPE_ROOMFLOOR ) {
	       player.pos_x=b;
	       player.pos_y=a;
	       player.lreg_x=b-4;
	       player.lreg_y=a-4;
	       player.spos_x=5;
	       player.spos_y=5;

	       if(player.pos_x<5)
		  player.spos_x=player.pos_x+1;
	       if(player.pos_y<5)
		  player.spos_y=player.pos_y+1;
	    }
	 }
      }
   }
   else
      searchstaircase(*level, TYPE_STAIRUP, sdir );

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
}


void player_go_up(level_type **level)
{
   level_type *newlevel;
   int8u sdir=0;

   /* save the current level first */
   if(!chdir_totempdir()) {
      msg.newmsg("Unable to access temporary directory!", CHB_RED);     
      return;
   }

   save_tmpfile(*level, dungeonlist[player.dungeon].index, player.dungeonlev);

   if(player.lastdir==STAIROUT && 
      player.levptr->outx && player.levptr->outy ) {
      player_go_outworld(level);
      return;
   }

   worldtime.set_events(false);

   msg.newmsg("You entered up stairs...", C_YELLOW);

   if(player.lastdir==STAIRUP1) {
      player.levptr=player.levptr->linkfrom1;
      sdir=STAIRDOWN1;
   }
   if(player.lastdir==STAIRUP2) {
      player.levptr=player.levptr->linkfrom2;
      sdir=STAIRDOWN2;
   }

   player.dungeonlev=player.levptr->index;
//   player.dungeonlev--;


   /* check and set the visited flag for the level */
   if(!(player.levptr->flags & LEVEL_VISITED)) {
      player.num_levels++;
      player.levptr->flags |= LEVEL_VISITED;
   }

   player.pos_x=player.pos_y=1;
   player.spos_x=2;
   player.spos_y=2;

   newlevel=load_tmplevel(dungeonlist[player.dungeon].index, player.dungeonlev);
   if(newlevel!=NULL) {
//   	msg.newmsg("LOADED OK!", CHB_GREEN);
      freelevel(*level);
      delete *level;

      *level=newlevel;
   }
   else {
      createlevel(*level);
   }

   searchstaircase(*level, TYPE_STAIRDOWN, sdir );

//	createlevel(*level);

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
}

void player_paybill(void)
{
   _monsterlist *mptr;
   level_type *level;
   Tinvpointer pinv;
   int8u i;
   int16s px, py;

   int32u gc, sc, cc;

   level=c_level;

   if(!player.bill) {
      msg.newmsg("You don't have any debts.", C_WHITE);
      return;
   }

   if(player.inroom<0 ) {
      msg.newmsg("You're not in a shop.", C_WHITE);
      return;
   }
   if( level->rooms[player.inroom].type != ROOM_SHOP) {
      msg.newmsg("You're not in a shop.", C_WHITE);
      return;
   }

   if( level->rooms[player.inroom].owner == NULL) {
      msg.newmsg("There's no one to pay your bill to.", C_WHITE);      
      return;
   }

   for(i=1; i<10; i++) {
      px=player.pos_x + move_dx[i];
      py=player.pos_y + move_dy[i];
      mptr=istheremonster(level, px, py);
      if(mptr) {
	 if(mptr->m.status & MST_SHOPKEEPER) {
	    break;
	 }
	 else
	    mptr=NULL;
      }
   }

   if(!mptr) {
      msg.newmsg("Shopkeeper says: \"Would you please come closer first!\".", C_YELLOW);
      return;
   }

   shopkeeper_coppervalue(player.bill, &gc, &sc, &cc);

   sprintf(tempstr, 
	   "Do you want to pay your bill (%ldg, %lds and %ldc) to %s", 
	   gc, sc, cc, mptr->m.name);
   if(!confirm_yn(tempstr, false, true))
      return;

   /* transfer the money */
   if( inv_paymoney(&player.inv, &mptr->inv, player.equip, 
		    player.bill, false) ) {
      msg.newmsg("You've paid your bill!", C_RED);
   }
   else {
      msg.newmsg("But you don't have enough money!", C_MAGENTA);
      return;
   }
   /* mark items as normal */
   pinv=player.inv.first;
   while(pinv) {
      if(pinv->i.status & ITEM_UNPAID) {
	 pinv->i.status ^= ITEM_UNPAID;
      }
      pinv=pinv->next;
   }

   player.bill=0;
}

const char *tacticnames[]=
{
   "Coward",
   "Very defensive",
   "Defensive",
   "Normal",
   "Aggressive",
   "Very aggressive",
   "Berzerk",
   NULL
};


void player_changetactics(void)
{
   player.tactic++;

   if(player.tactic==TACTIC_BERZERK+1)
      player.tactic=TACTIC_COWARD;

   msg.update(true);
   msg.vadd(C_WHITE, 
	    "Tactic: %s (Hit:%+d, Dodge:%+d, DAM:%+d, AC:%+d, SPD:%+d%%)!",
	    tacticnames[player.tactic],
	    tacticeffects[player.tactic].hit,
	    tacticeffects[player.tactic].dv,
	    tacticeffects[player.tactic].dam,
	    tacticeffects[player.tactic].pv,
	    100-tacticeffects[player.tactic].spd);
//   msg.newmsg(tempstr, C_WHITE);

}

void player_init()
{
   sint i;

   my_strcpy(player.name, "Moonlite", NAMEMAX);
   my_strcpy(player.title, "the adventurer", TITLEMAX);

   player.pos_x=player.pos_y=1;
   player.spos_x=2;
   player.spos_y=2;

   player.lastdir=STAIROUT;

   player.num_places=0;
   player.num_levels=0;
   player.num_kills=0;

   player.dungeon=5;
   player.levptr=dungeonlist[player.dungeon].levels;
   player.dungeonlev=player.levptr->index;

   player.light=0;
   player.status=0;

   inv_init(&player.inv, player.equip);
   
   player.skills=NULL;
   player.nutr=FOOD_SATIATED;

   player.lreg_x=player.lreg_y=0;

   player.color=CH_RED;

   player.align=NEUTRAL;

   player.pclass=1;
   player.prace=RACE_HUMAN;

   /* initialize player stats */
   for(i=0; i<STAT_ARRAYSIZE; i++) {
      player.stat[i].initial=50;
      player.stat[i].temp=0;
      player.stat[i].perm=0;
      player.stat[i].max=99;
      player.stat[i].min=0;
   }

   calculate_hp(player.hpp, npc_races[player.prace].hp_base, RACE_HUMAN);
   player.hp_max=player.hp=calculate_totalhp(player.hpp);

   player.stat[STAT_SPD].initial=BASE_SPEED + DEX_SPEED[ get_stat( &player.stat[STAT_DEX] ) ];
//   player.stat[STAT_SPD].initial=BASE_SPEED;
   player.stat[STAT_SPD].max=STATMAX_SPEED;

   player.stat[STAT_LUC].initial=5+RANDU(11);
   player.stat[STAT_LUC].max=STATMAX_LUCK;

   /* clear quick skills */
   for(i=0; i<NUM_QUICKSKILLS; i++) {
      player.qskills[i].select=0;
   }

//   player.ac=0;
//	player.hp=20;
//	player.hp_max=20;
   player.sp=20;
   player.sp_max=20;

   player.level=1;
   player.alive=true;
   player.repeatwalk=false;
   player.hitwall=-1;
   player.huntmode=false;

   player.timetaken=BASE_TIMENEED;
//   init_lineofsight(8);
   player.sight=8;
   player.searchmode=false;
   player.regentime=0;

   player.weight=75000;

   player.tactic=TACTIC_NORMAL;
   player.inroom=-1;

   cond_init(&player.conditions);

   quest_init(&player.quests);

   calculate_itembonus(NULL);

   path_init(&player.path);

}

void freeall_player(void)
{
   inv_removeall(&player.inv, player.equip);
   skill_removeall(&player.skills);
   cond_removeall(&player.conditions);
   quest_removeall(&player.quests);
   path_clear(&player.path);

   mem_clear(&player, sizeof(playerinfo));

}


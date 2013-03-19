/**************************************************************************
 * status.cc --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 12.05.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 12.05.1998                                         *
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
/* Handles all creature (player, monster) condition and status stuff      *
 **************************************************************************/

#include "saladir.h"
#include "output.h"

#include "defines.h"

#include "dungeon.h"
#include "monster.h"
#include "status.h"

/* local prototypes */
bool handle_confusion(_monsterlist *monster, Tcondpointer cond, int32u slots);

/* a list of condition descriptions */
const Tcond_desc cond_list[]=
{
   { "Bloated",    NULL, 0, CONDGRP_FOOD, CH_GREEN },
   { "Satiated",   NULL, 0, CONDGRP_FOOD, C_GREEN },
   { "Hungry",     NULL, 0, CONDGRP_FOOD, CH_YELLOW },
   { "Starving",   NULL, 0, CONDGRP_FOOD, C_RED },
   { "Fainting",   NULL, 0, CONDGRP_FOOD, CH_RED },
   { "Fainted",    NULL, 0, CONDGRP_FOOD, CH_RED },
   { "Burdened",   NULL, 0, CONDGRP_PW, C_YELLOW },
   { "Strained",   NULL, 0, CONDGRP_PW, C_RED },
   { "Overloaded", NULL, 0, CONDGRP_PW, CH_RED },  
   { "Confused",   NULL, 0, -1, C_MAGENTA },
   { "Blessed",    NULL, 0, -1, CH_GREEN },
   { "Cursed",     NULL, 0, -1, C_RED },
   { "Stun",       NULL, 0, -1, C_RED },
   { "Lucky",      NULL, 0, -1, CH_GREEN },
   { "Bleeding",   NULL, 0, -1, CH_RED },
   { "Poisoned",   NULL, 0, -1, CH_RED },
   { "Paralysed",  NULL, 0, -1, CH_RED },
   { "Legs_BAD",   NULL, 0, -1, CH_RED },
   { "LArm_BAD",   NULL, 0, -1, CH_RED },
   { "RArm_BAD",   NULL, 0, -1, CH_RED },
   { "Dead!",      NULL, 0, -1, CHB_RED }
};

void cond_init(Tcondpointer *list)
{
   *list=NULL;
}

Tcondpointer cond_add(Tcondpointer *list, int16u cond, int16s value)
{
   Tcondpointer newnode;
   bool condfound=false;

   /* remove all conditions with same group as the new condition */
   if(cond_list[cond].group >= 0)
      cond_delete_group(list, cond_list[cond].group);

   newnode=*list;
   while(newnode) {
      if(newnode->cond.type == cond) {
	 condfound=true;
	 break;
      }
      newnode=newnode->next;
   }

   /* if condition already exists, do not add new */
   if(!condfound) {
      newnode=NULL;

      /* create a node */
      newnode=new Tcondnode;

      if(!newnode)
	 return NULL;

      mem_clear(newnode, sizeof(Tcondnode));
      newnode->next=NULL;

      if(*list!=NULL) {
	 newnode->next=*list;
      }
      
      *list=newnode;
   }

   /* init condition */
   newnode->cond.type=cond;
   newnode->cond.val+=value;
   newnode->cond.time=0;

   if(list==&player.conditions)
      GAME_NOTIFYFLAGS |= GAME_CONDCHG;

   return newnode;
}

void cond_delete_group(Tcondpointer *list, int16u group)
{
   Tcondpointer cptr, dptr, prev;

   dptr=cptr=*list;

   if(list==&player.conditions)
      GAME_NOTIFYFLAGS |= GAME_CONDCHG;

   while(cptr) {
      prev=dptr;
      dptr=cptr;
      cptr=cptr->next;

      if(cond_list[dptr->cond.type].group==group) {

	 /* if the first node */
	 if(dptr==*list)
	    *list=(*list)->next;
	 /* if not the first node */
	 else
	    prev->next=dptr->next;

	 /* free node */
	 delete dptr;
	 dptr=prev;
      }
   }
}

void cond_removeall(Tcondpointer *list)
{
   Tcondpointer del;

   while(*list) {
      del=*list;

//      my_printf("Deleted cond %s!\n", cond_list[(*list)->cond.type].name);
      *list=(*list)->next;

      delete del;
   }

   cond_init(list);

}


void cond_delete(Tcondpointer *list, int16u cond)
{
   Tcondpointer cptr, dptr, prev;

   if(list==&player.conditions)
      GAME_NOTIFYFLAGS |= GAME_CONDCHG;

   dptr=cptr=*list;
   while(cptr) {
      prev=dptr;
      dptr=cptr;
      cptr=cptr->next;

      if(dptr->cond.type==cond) {

	 if(*list==dptr)
	    *list=(*list)->next;
	 else {
	    prev->next=dptr->next;
	 }
	 delete dptr;
	 break;
      }
   }
}


/* Test if a condition exists, return it's value or 0 */
int16u cond_check(Tcondpointer list, int16u cond)
{
   register Tcondpointer cptr;
   int16u val=0;

   cptr=list;
   
   while(cptr) {
      if(cptr->cond.type == cond) {
	 val=cptr->cond.val;
	 break;
      }
      cptr=cptr->next;
   }

   return val;
}

/* display all set conditions in the status row */
void cond_statshow(Tcondpointer list, int16u y)
{
   Tcondpointer cptr;

   clearline(y);
   my_gotoxy(1, y);

   cptr=list;
   while(cptr) {

      if(cptr->cond.type > CONDIT_MAX) {
	 msg.vnewmsg(CH_RED, 
		     "Error: Illegal condition %d (max=%d) in condition list.",
		     cptr->cond.type, CONDIT_MAX);
      }
      else {
	 /* stop if going over screen border */
	 if(my_getx() >= 
	    ( SCREEN_COLS - strlen(cond_list[cptr->cond.type].name))) {
	    return;
	 }

	 my_setcolor(cond_list[cptr->cond.type].color);
	 my_printf("%s ", cond_list[cptr->cond.type].name);
      }
      cptr=cptr->next;
   }
}

/* handle conditions */
/*
void cond_handle(Tcondpointer *list, int32u slots)
{
   Tcondpointer cptr, dptr, prev;  
   bool cres;
   
*/
void cond_handle(_monsterlist *monster, int32u slots)
{
   Tcondpointer cptr, dptr, prev;  
   Tcondpointer *list;
   bool cres;

   if(monster)
      list=&monster->conditions;
   else
      list=&player.conditions;

   dptr=cptr=*list;

   while(cptr) {
      cres=false;
      prev=dptr;
      dptr=cptr;
      cptr=cptr->next;

      switch(dptr->cond.type) {
	 case CONDIT_CONFUSED:
	    cres=handle_confusion(monster, dptr, slots);
	    break;
	 default:
	    break;

      }    

      /* if set, then the condition should be deleted! */
      if(cres) {
	 GAME_NOTIFYFLAGS |= GAME_CONDCHG;

	 /* if the first node */
	 if(dptr==*list)
	    *list=(*list)->next;
	 /* if not the first node */
	 else
	    prev->next=dptr->next;

	 /* free node */
	 delete dptr;
	 dptr=prev;
      }
   }
}

bool handle_confusion(_monsterlist *monster, Tcondpointer cond, int32u slots)
{
   if(monster) {
      if(RANDU(100) < 20 && player_has_los(c_level, monster->x, monster->y)) {
	 monster_sprintf(nametemp, monster, true, true);
	 msg.vnewmsg(C_WHITE, "%s looks strangely distorted.", nametemp);
      }
   }
   else
      msg.newmsg("Confusion handling for you!", C_RED);

   cond->cond.val-=slots;

   if(cond->cond.val <= 0 ) {
      if(!monster)
	 msg.newmsg("You feel more stable now.", CH_GREEN);
      else {
	 monster_sprintf(nametemp, monster, true, true);
	 msg.vnewmsg(C_WHITE, "%s looks much more stable now.", nametemp);
      }
      return true;
   }

   return false;
}


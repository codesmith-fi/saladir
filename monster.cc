/**************************************************************************
 * monster.cc --                                                          *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1998                                         *
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

#include "types.h"
#include "defines.h"
#include "creature.h"
#include "caves.h"
#include "classes.h"
#include "utility.h"
#include "damage.h"
#include "talk.h"
#include "NPCtalk.h"
#include "inventor.h"
#include "shops.h"
#include "raiselev.h"
#include "weapons.h"
#include "doors.h"
#include "variable.h"
#include "status.h"
#include "traps.h"
#include "monster.h"
#include "pathfunc.h"

void billgates_chat(_monsterlist *);
void billgates_act(_monsterlist *, level_type *);
void thomas_act(_monsterlist *, level_type *);
void thomas_chat(_monsterlist *);
void sparhawk_act(_monsterlist *, level_type *);

int16u monster_rangedattack(level_type *level, _monsterlist *mptr);

char ERRORSTRING[]="THIS SHOULD NOT BE VISIBLE!";

char *monster_takename(_monsterdef *monster)
{
   if(monster->status & MST_KNOWN) {
      if(strlen(monster->name)>0)
	 return monster->name;
      else
	 return ERRORSTRING;
   }

   if(strlen(monster->desc)>0)
      return monster->desc;

   return ERRORSTRING;
}

/* sprintf for monster data */
/* generates a monster name to the destination string... */
/* "the female snake" or "Bill Gates", etc... */
void monster_sprintf(char *dest, _monsterlist *mptr, 
		     bool upcase, bool nameonly)
{
   if(!dest)
      return;

   if(!(c_level->loc[mptr->y][mptr->x].flags & CAVE_LOSTMP)) {
      strcpy(dest, "something");
      if(upcase)
	 dest[0]=toupper(dest[0]);
      return;

   }


   if(npc_races[mptr->m.race].behave & BEHV_ANIMAL) {
      sprintf(dest, "the %s", mptr->m.desc);
   }
   else if(mptr->m.status & MST_KNOWN) {
      if(strlen(mptr->m.name)>0) {
	 if(nameonly)
	    sprintf(dest, "%s", mptr->m.name);
	 else {
	    if(!mptr->m.special && !mptr->m.status & MST_SHOPKEEPER) {
	       sprintf(dest, "%s, the %s %s %s", 
		       mptr->m.name, gendertext[mptr->m.gender], 
		       mptr->m.desc, classes[mptr->m.mclass].name);
	    }
	    else {
	       sprintf(dest, "%s, the %s %s", 
		       mptr->m.name, gendertext[mptr->m.gender], 
		       mptr->m.desc);
	    }
	 }
      }
      else {
	 if(!mptr->m.special && !mptr->m.status & MST_SHOPKEEPER) {
	    sprintf(dest, "the %s %s %s", 
		    gendertext[mptr->m.gender], 
		    mptr->m.desc, classes[mptr->m.mclass].name);
	 }
	 else {
	    sprintf(dest, "the %s %s", 
		    gendertext[mptr->m.gender], mptr->m.desc);
	 }
      }
   }
   else {
      if(!mptr->m.special &&  !mptr->m.status & MST_SHOPKEEPER) {
	 sprintf(dest, "the %s %s %s", 
		 gendertext[mptr->m.gender], mptr->m.desc, 
		 classes[mptr->m.mclass].name);
      }
      else {
	 sprintf(dest, "the %s %s", 
		 gendertext[mptr->m.gender], mptr->m.desc);
      }
   }

   if(upcase)
      dest[0]=toupper(dest[0]);
}

void monster_regenerate(level_type *level, _monsterlist *mptr, 
			int16s ctime, int16u slots )
{
//     int32u rgval=0;
   int16u stat;
   int16u i, j;

   int16u slotheal[HPSLOT_MAX+1]={ 0 };

   bool hpregen;

   /* collect the slots which need healing */
   j=0;
   for(i=0; i<HPSLOT_MAX; i++) {
      if(mptr->hpp[i].cur < mptr->hpp[i].max)
	 slotheal[j++]=i;
   }

   /* add regeneration time */
   mptr->hpregen+=ctime*slots;

   stat=get_stat( &mptr->stat[STAT_CON] );

   /* j is >0 if regen needed */
   hpregen=false;
   if( !j ) {
      mptr->hpregen=0;
   }
   else if( mptr->hpregen >= CON_HITP[stat]) {
      hpregen=true;
   }
   else {
      stat=skill_check(mptr->skills, SKILLGRP_GENERIC, SKILL_HEALING);
      stat=stat/4;
      if(throwdice(1, 100, 0) < stat) {
	 hpregen=true;
      }
   }

   if(hpregen) {
      mptr->hpregen=0;

      /* take a random slot to regen */
      i=RANDU(j);

      /* regenerate */
      if(mptr->hpp[slotheal[i]].cur < mptr->hpp[slotheal[i]].max)
	 mptr->hpp[slotheal[i]].cur++;

      monster_checkstat(mptr, level);
   }

   /* monster needs atleast half of it's max health to get out from
      fleemode */

   return;
}

int16u monster_checkroom(level_type *level, _monsterlist *mptr)
{
   int16u rc;

   for(rc=0; rc<level->roomcount; rc++) {
      if(mptr->x >= level->rooms[rc].x1 && mptr->x <= level->rooms[rc].x2) {
	 if(mptr->y >= level->rooms[rc].y1 && mptr->y <= level->rooms[rc].y2) {
/*
  if(mptr->inroom==-1) {

  }
  else {

  }
*/
	    mptr->inroom=rc;
            return true;

         }
      }
   }

   mptr->inroom=-1;

   return false;
}


void handle_monsters(level_type *level, int16u slots)
{
   _monsterlist *ptr, *dptr;
   int16u tslots;

   ptr=level->monsters;

   while(ptr) {
      /* dead monsters don't do much */
      if(ptr->hp > 0) {

	 tslots=slots;
	 while(tslots) {
	    monster_checkturn(ptr, level);
	    tslots--;
	 }

	 monster_checkroom(level, ptr);
	 monster_regenerate(level, ptr, get_stat(&player.stat[STAT_SPD]), 
			    slots);
	 cond_handle(ptr, slots);

	 ptr->light=4;
	 if(ptr->light>0 && CONFIGVARS.lightmonster) 
	    monster_torchlos(level, ptr);

	 /* store distance to the player now */
//	 ptr->dist_2_player=distance(ptr->x, ptr->y, 
//				     player.pos_x, player.pos_y);
	 handletrap(level, ptr->x, ptr->y, ptr);
	 
      }

      dptr=ptr;
      /* point ptr to next monster in level */
      ptr=ptr->next;

      /* check monster status, hp and stuff and die if necessary */
      monster_checkstat(dptr, level);
   }
}

/* 
 * Get angry to someone, if "target" is NULL then get angry to player
 *
 * Must take care for ie. removing the pathnodeslist if one exists!
 *
 */
void monster_getangry(level_type *level, _monsterlist *monster, 
		      _monsterlist *target)
{
   if(!monster)
      return;

   /* target must NOT be the monster who is getting angry :) */
   if(monster == target) {
      msg.addwait("Error: monster_getangry() trying to make monster angry with itself!", CH_RED);
      return;
   }

   /* shopkeepers will start hating player */
   if(monster->m.status & MST_SHOPKEEPER) {
      if(!target)
	 monster->m.status |= MST_HATEPLAYER;
      else
	 target->m.status |= MST_KEEPERHATES;
   }

   if(!(monster->m.status & MST_ATTACKMODE) || monster->target!=target) {
      monster_sprintf(nametemp, monster, true, true);
      if(target) {
	 monster_sprintf(nametemp2, target, false, true);
	 if(player_has_los(level, target->x, target->y) && 
	    player_has_los(level, monster->x, monster->y)) {
	    msg.vnewmsg(C_RED, "%s gets angry with %s", nametemp, nametemp2);
	 }
	 else
	    msg.newmsg("You hear a distant cry.", C_WHITE);
      }
      else {
	 msg.vnewmsg(C_RED, "%s gets angry with you!", nametemp);
      }
   }

   /* make it angry */    
   monster->m.status|=MST_ATTACKMODE;
   monster->target=target;

   /* for now, use TACTIC_AGGR */
   monster->tactic=TACTIC_AGGR;

   /* forget other special modes */
   monster->targetx=0;
   monster->targety=0;
   if(monster->m.status & MST_PURSUEITEM)
      monster->m.status ^= MST_PURSUEITEM;

   /* when attacked, the monster WILL forget it's path, so if the monster
    * being attacked is a shopkeeper and it's outside its shop then it must
    * somehow get back to the shop..
    * the path must be calculated with path_findroute() ...
    * (for now, shopeeker will recalculate a way back to the shop 
    * when the distance between the shopeeper and his shop (door) is 
    * over SHOPKEEPER_MAXDOORDIST
    *
    */

   path_clear(&monster->path);

}



/*
 * this routine handles monster seeing 
 *
 */
bool monster_noticestuff(_monsterlist *mptr, level_type *level)
{
   Tinvpointer itemptr;
   _monsterlist *nptr;
   int16s align, auxdist;

   /* don't change attack parameters if monster is
      already in attack mode chasing something */
   if(mptr->m.status & MST_ATTACKMODE) {
      return false;
   }

   /* 
    * must develop a *faster* way to check visibility from a monster to
    * a monster or from a monster to an item!
    * drawing lines between 2 targets is *slow*, atleast when there're
    * 10's of items and 10's of monsters in the level!
    */


   /* notice items on level */
   /* maybe this could be done ie. once in 10 turns!?!? */
   if(!(mptr->movecount % (10+RANDU(4)))) {
      if(mptr->targetx==0 && mptr->targety==0 && !(npc_races[mptr->m.race].behave & BEHV_ANIMAL) && !(mptr->m.status & MST_SHOPKEEPER)) {
	 itemptr=level->inv.first;
	 while(itemptr) {

	    if( cansee(level, mptr->x, mptr->y, itemptr->x, itemptr->y, 10, true) ) {
	       mptr->targetx=itemptr->x;
	       mptr->targety=itemptr->y;
	       mptr->m.status|=MST_PURSUEITEM;
	       break;
	    }
	    itemptr=itemptr->next;
	 }
      }
   }

   /* neutral characters don't attack anyone spontaneously */
   if(mptr->m.align>=NEUTRAL_S && mptr->m.align<=NEUTRAL_E)
      return false;

   /* friendly monsters doesn't attack spontaneously */
   if(mptr->m.behave & BEHV_FRIENDLY)
      return false;

   /* if monster sees the player */
   auxdist=cansee(level, mptr->x, mptr->y, player.pos_x, player.pos_y, 10, true);
   if( auxdist > 0) {

      /* at this point, special monsters do not get angry spontaneously */
      if(mptr->m.special!=0 )
	 return false;

      align=ABS(mptr->m.align - player.align) / (LAWFUL/100);
      align+=mptr->m.attitude;

      /* in here I must check the conditions for getting angry to player*/
      if( RANDU(100) < align) {
	 monster_getangry(level, mptr, NULL);
         return true;
      }
   }

   nptr=level->monsters;

   /* check for monsters too */
   while(nptr) {
      auxdist=cansee(level, mptr->x, mptr->y, nptr->x, nptr->y, 10, true);
      if(auxdist>0) {
	 /* at this point, special monsters do not attack each other */
	 if((mptr->m.special!=0) && (nptr->m.special!=0)) {
	    nptr=nptr->next;
	    continue;
	 }
	 /* if attacker has same race as target */
	 if(mptr->m.race == nptr->m.race) {
	    nptr=nptr->next;
	    continue;
	 }
	 align=ABS(mptr->m.align - nptr->m.align) / (LAWFUL/100);
	 align+=mptr->m.attitude;

	 if( RANDU(100) < align) {

	    monster_getangry(level, mptr, nptr);
	    return true;
	 }
      }
      nptr=nptr->next;
   }

   return false;
}

void monster_checkturn(_monsterlist *mptr, level_type *level)
{
   int16s mtime=0;

   if(!mptr)
      return;

   mptr->time+=get_stat(&mptr->stat[STAT_SPD]);

   /* act if time passes over BASE_TIMENEED */
   if(mptr->time>=BASE_TIMENEED) {
      
      monster_noticestuff(mptr, level);

      /* here do special monsters acting, they are good actors :-) */
      if(mptr->m.special!=0)
	 act_specialmonster(mptr, level);

      mtime=monster_rangedattack(level, mptr);

      if(!mtime) {
	 if(mptr->m.status & MST_SHOPKEEPER)
	    mtime=shopkeeper_move(level, mptr);
	 else
	    mtime=move_monster(mptr, level);

      }
      mptr->time-=mtime;
      mptr->movecount++;
   }

}

void monster_shouldflee(level_type *level, _monsterlist *mptr)
{
   bool shouldflee=false;
   real rhp;

   rhp=(real)mptr->hpp[HPSLOT_HEAD].max;
   rhp=rhp*0.60;

   /* flee if head too weak */
   if(mptr->hpp[HPSLOT_HEAD].cur <= rhp)
      shouldflee=true;

   rhp=(real)mptr->hpp[HPSLOT_BODY].max;
   rhp=rhp*0.40;

   /* flee if body too weak */
   if(mptr->hpp[HPSLOT_BODY].cur <= rhp)
      shouldflee=true;

   if(shouldflee) {
      if(!(mptr->m.status & MST_FLEEMODE)) {

	 if(player_has_los(level, mptr->x, mptr->y) )
	    monster_sprintf(nametemp, mptr, true, true );
	 else
	    my_strcpy(nametemp, "It", sizeof(nametemp)-1);

	 mptr->tactic=TACTIC_VERYDEF;

	 mptr->m.status|=MST_FLEEMODE;
	 sprintf(tempstr, "%s flees.", nametemp);
	 msg.add_dist(level, mptr->x, mptr->y, tempstr, C_YELLOW, NULL, C_CYAN);
      }
   }
}

void monster_checkbody(_monsterlist *mptr)
{
   /* check for hpslot healing */
   /* and hp warning */
   for(int8u i=0; i<HPSLOT_MAX; i++) {
      /* warn for low hitpoints */

      if( (mptr->hpp[i].cur > 0)
	  && (mptr->equip[eqslot_from_hpslot[i]].status!=EQSTAT_OK) ) {

	 /* put the slot back to work */
	 mptr->equip[eqslot_from_hpslot[i]].status=EQSTAT_OK;

	 if(i==HPSLOT_LEGS)
	    cond_delete(&mptr->conditions, CONDIT_BADLEGS);
	 else if(i==HPSLOT_LEFTHAND) 
	    cond_delete(&mptr->conditions, CONDIT_BADLARM);
	 else if(i==HPSLOT_RIGHTHAND) 
	    cond_delete(&mptr->conditions, CONDIT_BADRARM);

	 /* Should we message the monster somehow now that he's
	    able to use certain bodyparts */
      }
   }

   /* let flying creatures fly when both hands(vanes) have hp's */
   if(npc_races[mptr->m.race].behave & BEHV_FLYING) {
      if((mptr->hpp[HPSLOT_LEFTHAND].cur>0) && (mptr->hpp[HPSLOT_RIGHTHAND].cur>0)) {
	 if( (mptr->m.status & MST_CANTMOVE) )
	    mptr->m.status ^= MST_CANTMOVE;
      }
   }
   else {
      if( (mptr->hpp[HPSLOT_LEGS].cur>0) && (mptr->m.status & MST_CANTMOVE))
	 mptr->m.status ^= MST_CANTMOVE;
   }
}

void monster_checkstat(_monsterlist *mptr, level_type *level)
{
   real rhp;

   bool getoutflee;

   mptr->hp=calculate_totalhp(mptr->hpp);

   monster_sprintf(nametemp, mptr, true, true );

   getoutflee=false;

   rhp=(real)mptr->hpp[HPSLOT_HEAD].max;
   rhp=rhp * 0.60;

   /* to get out of flee mode, both head and body must be healthy enough */
   if(mptr->hpp[HPSLOT_HEAD].cur > rhp) {

      rhp=(real)mptr->hpp[HPSLOT_BODY].max;
      rhp=rhp * 0.50;

      if(mptr->hpp[HPSLOT_BODY].cur > rhp)
	 getoutflee=true;
   }

   /* get out of the flee mode */
   /* should probably watch head and body damage instead */
   if(getoutflee && (mptr->m.status & MST_FLEEMODE)) {
      mptr->m.status^=MST_FLEEMODE;

      sprintf(tempstr, "%s looks healthier.", nametemp);
      msg.add_dist(level, mptr->x, mptr->y, tempstr, C_YELLOW, NULL, C_CYAN);

   }

   /* check for healing bodyparts and activate them */
   monster_checkbody(mptr);

   /* check for damaging bodyparts and disable them if needed */
   damage_checkbodyparts(level, mptr);

   /* here dies the monster */
   if(mptr->hp <= 0) {
      if(player_has_los(level, mptr->x, mptr->y) )
	 monster_sprintf(nametemp, mptr, true, false );
      else
	 my_strcpy(nametemp, "Something", sizeof(nametemp)-1);

      sprintf(tempstr, "%s dies!", nametemp);

      msg.add_dist(level, mptr->x, mptr->y, tempstr, C_RED, 
		   "Someone died!", C_RED);

//      generate_corpse(level, &mptr->m, mptr->x, mptr->y);
      generate_corpse(level, mptr);

      /* drop everything from monster inventory */
      inv_dropall(&mptr->inv, mptr->equip, level, mptr->x, mptr->y);

      if(mptr->m.status & MST_SHOPKEEPER)
	 shopkeeper_clean(level, mptr);

      quest_ownerdied(&player.quests, mptr);

      level_removemonster(level, mptr);
      monster_cleantargets(level, mptr);
   }
}

bool monster_meleeinner(level_type *level, _monsterlist *monster,
			_Tequipslot *eqptr, _monsterlist *target )
{
   int16s damage=0, thp=0;
   int16s hitresult, inttotal;
   real hittotal;
   int16u trace, tlevel, i, j;

   /* array for target bodyparts */
   int8s bparts[HPSLOT_MAX+1];
   char hitbonustxt[40], actiontxt[40];

   int8u tslot;

   if(eqptr) {
      /* check if the item broke */
      damage = throwdice( eqptr->item->i.melee_dt,
			  eqptr->item->i.melee_ds,
			  eqptr->item->i.meldam_mod);
   }
   else {
      damage = throwdice( npc_races[monster->m.race].dam_dt, 
			  npc_races[monster->m.race].dam_ds, 
			  npc_races[monster->m.race].dam_mod );
   }

   /* targetting monster or player */
   if(!target) {
      trace=player.prace;
      tlevel=player.level;
      my_strcpy(nametemp2, "you", sizeof(nametemp2));
   }
   else {
      trace=target->m.race;
      tlevel=target->m.level;
      monster_sprintf(nametemp2, target, false, true );
   }

   /* collect target bodyparts which the race has */
   for(j=0, i=0; i<HPSLOT_MAX; i++) {
      if(npc_races[trace].bodyparts[i]>=0) {
	 bparts[j++]=i;
      }
   }

   /* get a random target bodypart */
   tslot=bparts[RANDU(j)];

   if(eqptr)
      inttotal=calculate_meleehit(&eqptr->item->i, NULL, target, tslot);
   else
      inttotal=calculate_meleehit(NULL, NULL, target, tslot);

   hittotal=(real)inttotal;

   /* throw the dice */
   hitresult=throwdice(1, 100, 0);

   /* add tactic effect damage */
   damage+=tacticeffects[monster->tactic].dam;
   if(damage<0)
      damage=0;

   hitbonustxt[0]=0;   

   /* get attacker name */
   if( player_has_los(level, monster->x, monster->y) )
      monster_sprintf(nametemp, monster, true, true);
   else
      sprintf(nametemp, "It");
 
   if(hitresult <= hittotal) {
      if(hitresult > (hittotal*0.95) ) {
	 my_strcpy(hitbonustxt, " hard", sizeof(hitbonustxt)-1);
	 damage+=throwdice(2, 3, 0);
      }
      else if(hitresult == inttotal ) {
	 my_strcpy(hitbonustxt, " critically", sizeof(hitbonustxt)-1);
	 damage+=damage;
      }
   }
   else {
//      dam=0;
      if(!target) {
	 sprintf(tempstr, "%s misses your %s.", nametemp, bodyparts[tslot]);
	 msg.newmsg(tempstr, C_WHITE);
      }
      else {
	 sprintf(tempstr, "%s misses %s.", nametemp, nametemp2);
	 msg.add_dist(level, monster->x, monster->y, tempstr, C_WHITE, 
		      NULL, C_WHITE);
      }
      return false;
   }

   if(npc_races[monster->m.race].behave & BEHV_ANIMAL) {
      if(npc_races[monster->m.race].attacktypes & ATTACK_BITE)
	 sprintf(actiontxt, "bites");
      else if(npc_races[monster->m.race].attacktypes & ATTACK_KICK)
	 sprintf(actiontxt, "kicks");
      else
	 sprintf(actiontxt, "hits");
   }
   else
      sprintf(actiontxt, "hits");

   if(!damage) {
      if(!target) {
	 sprintf(tempstr, "%s %s your %s with no damage.", 
		 nametemp, actiontxt, bodyparts[tslot]);
	 msg.newmsg(tempstr, C_WHITE);
      }
      else {
	 sprintf(tempstr, "%s %s the %s of %s with no damage.", 
		 nametemp, actiontxt, bodyparts[tslot], nametemp2);
	 msg.add_dist(level, monster->x, monster->y, tempstr, C_WHITE, 
		      NULL, C_WHITE);
      }
      return false;
   }  
   else {
/*
      if(!target)
	 sprintf(tempstr, "%s %s your %s%s.", 
		 nametemp, actiontxt, bodyparts[tslot], hitbonustxt);
      else
	 sprintf(tempstr, "%s %s %s%s.", 
		 nametemp, actiontxt, 
		 nametemp2, hitbonustxt);
*/
      if(!target)
	 sprintf(tempstr, "%s %s you%s.", 
		 nametemp, actiontxt, hitbonustxt);
      else
	 sprintf(tempstr, "%s %s %s%s.", 
		 nametemp, actiontxt, 
		 nametemp2, hitbonustxt);
   }

   if(!target) {
//      msg.newmsg(tempstr, C_RED);
//      thp=player_getmeleehit(level, tempstr, damage, tslot, monster);
      thp=damage_issue(level, NULL, monster,
		       ELEMENT_NOTHING, damage, tslot, 
		       tempstr);
   }
   else {
      if(!player_has_los(level, monster->x, monster->y) )
	 my_strcpy(tempstr, "Something is fighting here.", sizeof(tempstr)-1);

      if((1+RANDU(100)) > 80)
	 msg.add_dist(level, monster->x, monster->y, tempstr, C_RED, 
		      "You hear angry noises!", C_WHITE);


      thp=damage_issue(level, target, monster, ELEMENT_NOTHING, damage, tslot, 
		       tempstr);

//      else
//	 msg.add_dist(level, monster->x, monster->y, tempstr, C_RED, 
//		      NULL, C_WHITE);

//      thp=monster_takedamage(level, target, monster, 
//			     damage, tslot);
   }

   if(thp<=0) {
      monster_gainedexperience(level, monster, target);
   }

   return true;
}

/* this function is called always when a monster kills something */
void monster_gainedexperience(level_type *level,
			      _monsterlist *monster, _monsterlist *dies) 
{
   int32u expgain;
   int16u trace, tlevel;

   if(dies) {
      trace=dies->m.race;
      tlevel=dies->m.level;
   }
   else {
      trace=player.prace;
      tlevel=player.level;
   }

   expgain=npc_races[trace].exp * tlevel;

   gain_experience(level, monster, expgain);

   monster->target=NULL;

   /* clear attack and flee modes */
   if( (monster->m.status & MST_ATTACKMODE) )
      monster->m.status^=MST_ATTACKMODE;
   if( (monster->m.status & MST_FLEEMODE) )
      monster->m.status^=MST_FLEEMODE;

}


void monster_meleeattack(level_type *level, _monsterlist *monster,
			 _monsterlist *target)
{
//   item_def *iptr;
   int16s timel;

   bool lefthand, righthand;

   /* decide weapon to use in attack */
   decide_meleeweapon(monster->equip, &lefthand, &righthand);

   if(lefthand) {
      /* subtract time taken by attack */
      monster->time-=calc_time(get_stat( &monster->stat[STAT_SPD]), 
				  TIME_MELEEATTACK, monster->tactic);

//      iptr=&monster->inv.equip[EQUIP_LHAND].item->i;

      if(monster_meleeinner(level, monster, 
			    &monster->equip[EQUIP_LHAND], target))
	 return;
   }

   if(righthand) {
      timel=monster->time-calc_time(get_stat(&monster->stat[STAT_SPD]), 
				       TIME_MELEEATTACK, monster->tactic);

      if(timel<=0)
	 return;

      monster->time-=calc_time(get_stat(&monster->stat[STAT_SPD]), 
				  TIME_MELEEATTACK, monster->tactic);

//      iptr=&monster->inv.equip[EQUIP_RHAND].item->i;

      if(monster_meleeinner(level, monster, &monster->equip[EQUIP_RHAND], 
			    target))
	 return;
	 
   }

   /* do hand damage if no items */
   if(!righthand && !lefthand ) {
      monster->time-=calc_time(get_stat(&monster->stat[STAT_SPD]), 
				  TIME_MELEEATTACK, monster->tactic);

      if(monster_meleeinner(level, monster, NULL, 
			    target))
	 return;
   }

}


int16u monster_rangedattack(level_type *level, _monsterlist *mptr)
{
   bool misres=true;
   int16u timetaken=0;
   int16u itype, igroup;
   int16u askill, agrp;   
   int16u skillval;

   /* if not in attack mode, return */
   if(!(mptr->m.status & MST_ATTACKMODE))
      return 0;

   if(!get_rangedskill(mptr, &agrp, &askill))
      return 0;

   itype=mptr->equip[EQUIP_MISSILE].item->i.type;
   igroup=mptr->equip[EQUIP_MISSILE].item->i.group;
   
   if(mptr->target==NULL) {
      skillval=skill_check(mptr->skills, agrp, askill);

      if( cansee(level, mptr->x, mptr->y, 
		 player.pos_x, player.pos_y, 10, true) ) {

	 hidecursor();  
	 misres=ranged_line(level, 0, 60, true,
			    mptr->x, mptr->y,
			    player.pos_x, player.pos_y, 
			    ranged_checkhit_player, mptr, skillval);
	 showcursor();

	 timetaken=calc_time(get_stat(&mptr->stat[STAT_SPD]), 
			     TIME_MISSILEATTACK, mptr->tactic);
      }
   }
   else {
      

      timetaken=0;
   }

   /* PUTOAA NYT AINA PELAAJAN KOHDALLE!!! */

   if(!misres) {
      inv_transfer2inv(level, &mptr->inv, mptr->equip,
		   &level->inv, mptr->equip[EQUIP_MISSILE].item, 
		   1, player.pos_x, player.pos_y);
   }

   /* equip a new item from reserve if possible */
   ready_newmissile(&mptr->inv, mptr->equip, itype, igroup);
   
   return timetaken;
}





/* Clean up monster targets
**
** if monster is targetting a monster which does not exists in our
** list it must be removed or the results are not nice...
**
** This is called always when a monster dies
*/
void monster_cleantargets(level_type *level, _monsterlist *remove)
{
   _monsterlist *mptr;

//   cmptr=level->monsters;
   mptr=level->monsters;

   /* empty list, nothing to clean up */
   if(!mptr) return;

   /* do the whole list until NULL comes */
   while(mptr) {
      /* if monster is targetting itself, clear target */
      if(mptr->target==mptr) {
	 mptr->target=NULL;
	 if( (mptr->m.status & MST_ATTACKMODE) )
	    mptr->m.status^=MST_ATTACKMODE;
	 if( (mptr->m.status & MST_FLEEMODE) )
	    mptr->m.status^=MST_FLEEMODE;
      }
      /* if monster is targetting the removed monster, clean */
      else if(mptr->target==remove) {
	 mptr->target=NULL;
	 if( (mptr->m.status & MST_ATTACKMODE) )
	    mptr->m.status^=MST_ATTACKMODE;
	 if( (mptr->m.status & MST_FLEEMODE) )
	    mptr->m.status^=MST_FLEEMODE;

      }
      /* go to the next node */
      mptr=mptr->next;
   }
   /* check monsterlist if there is node for targetted monster */
/*
  if(mptr->target) {
  targetfound=false;
  cmptr=level->monsters;
  while(cmptr) {
  if(cmptr==mptr->target ) {
  targetfound=true;
  break;
  }
  cmptr=cmptr->next;
  }

  // clear the target for current monster because it doesn't exist
  if(!targetfound) {
  mptr->target=NULL;
  if( (mptr->m.status & MST_ATTACKMODE) )
  mptr->m.status^=MST_ATTACKMODE;
  if( (mptr->m.status & MST_FLEEMODE) )
  mptr->m.status^=MST_FLEEMODE;
  }
  }
*/

}

void drawborder(int16u x1, int16u y1, int16u x2, int16u y2)
{
     int16u j;
     for(j=y1; j<y2; j++) {
	  my_gotoxy(x1, j);
	  my_printf("|");
	  drawline_limit(j, x1+1, x2, ' ');
     }
     my_gotoxy(x1, j);
     my_printf("+");
     drawline_limit(j, x1+1, x2-1, '-');
}

#define CONDITION_MAXNUM 10
const char *condition_txt[]=
{
     "is very near to death.",
     "is in VERY bad condition.",
     "is in very bad condition.",
     "is in pretty bad condition.",
     "has some serious looking wounds.",
     "has has some wounds.",
     "has got some bruises and scars.",
     "is looking healthy.",
     "is in good shape.",
     "is in exellent condition",	/* full hp */
     NULL,
};

void monster_showdesc(_monsterlist *mptr)
{
   char *dptr;

   if(!mptr)
      return;

   int16s i;
   real rg;

   my_clrscr();

   monster_sprintf(nametemp, mptr, false, false);
   my_setcolor(CH_WHITE);
   my_cputs(1, nametemp);
   my_setcolor(C_YELLOW);
   drawline(2, '-');

   my_setcolor(C_WHITE);
   if(mptr->m.longdesc) {
      dptr=desclist[mptr->m.longdesc];
      my_wordwraptext( desclist[mptr->m.longdesc], 4, SCREEN_LINES-1, 1, SCREEN_COLS-1);
      showmore(false, false);
   }
   my_setcolor(C_WHITE);
   dptr=npc_races[mptr->m.race].desc;
   if(dptr) {
      my_wordwraptext( dptr, 4, SCREEN_LINES-1, 1, SCREEN_COLS-1);
   }
   else {
      my_printf("\nNo race description.\n");
   }
   my_printf("\n");

   if(npc_races[mptr->m.race].behave & BEHV_ANIMAL) {
      my_printf("It's an animal");
      if(npc_races[mptr->m.race].behave & BEHV_CANUSEITEM)
	 my_printf(" but it's known to use some items. ");
      else my_printf(". ");
   }
   if(npc_races[mptr->m.race].behave & BEHV_FLYING)
      my_printf("It can fly.");
   my_printf("\n");
   if(npc_races[mptr->m.race].attacktypes & ATTACK_BITE)
      my_printf("It can bite. ");
   if(npc_races[mptr->m.race].attacktypes & ATTACK_KICK)
      my_printf("It can kick. ");
   if((npc_races[mptr->m.race].attacktypes & ATTACK_HIT) || npc_races[mptr->m.race].attacktypes==0)
      my_printf("It can hit. ");
   my_printf("\n");
   
   bool some1=false;
   zprintf("As you examine %s closer, you notice that ", 
	     gender_art2[mptr->m.gender]);

   if(mptr->equip[EQUIP_RHAND].item) {
      some1=true;
      zprintf("%s has readied a %s in %s right hand", 
	      gender_art1[mptr->m.gender], 
	      mptr->equip[EQUIP_RHAND].item->i.name,
	      gender_art3[mptr->m.gender]);
   }
   if(mptr->equip[EQUIP_LHAND].item) {
      if(some1)
	 zprintf(" and ");
      zprintf("in %s left hand %s has a %s", 
		gender_art3[mptr->m.gender], 
		gender_art1[mptr->m.gender],
		mptr->equip[EQUIP_LHAND].item->i.name);
   }
   if(mptr->equip[EQUIP_MISSILE].item) {
      if(some1)
	 zprintf(" and ");
      zprintf("%s seems to have some missiles readied (%s)", 
		gender_art1[mptr->m.gender], 
		mptr->equip[EQUIP_MISSILE].item->i.name);
   }

   if(!some1)
      zprintf("%s is not using anything particular.\n", 
		gender_art1[mptr->m.gender]);
   else
      zprintf(".\n");
//   my_printf("\n");

   real tmp;

   tmp=(real)mptr->hp_max;
   tmp=tmp/CONDITION_MAXNUM;

   rg=(real)mptr->hp;
   rg=rg/tmp;
   i=(int16s)rg;

   if(i<0)
      i=0;
   if(i>=CONDITION_MAXNUM)
      i=CONDITION_MAXNUM-1;

   my_printf("%s %s\n", nametemp, condition_txt[i]);

   if(CONFIGVARS.cheat) {
      my_setcolor(CH_BLUE);
      my_printf("\nCheat values here: (monster id=%ld)\n", mptr->id);
      my_setcolor(C_WHITE);
      my_printf("It can target:");
      if(npc_races[mptr->m.race].targetflags & TARGET_HEAD)
	 my_printf(" %s", bodyparts[HPSLOT_HEAD]);
      if(npc_races[mptr->m.race].targetflags & TARGET_BODY)
	 my_printf(" %s", bodyparts[HPSLOT_BODY]);
      if(npc_races[mptr->m.race].targetflags & TARGET_LHAND)
	 my_printf(" %s", bodyparts[HPSLOT_LEFTHAND]);
      if(npc_races[mptr->m.race].targetflags & TARGET_RHAND)
	 my_printf(" %s", bodyparts[HPSLOT_RIGHTHAND]);
      if(npc_races[mptr->m.race].targetflags & TARGET_LEGS)
	 my_printf(" %s", bodyparts[HPSLOT_LEGS]);

      my_printf("\nAlignment=%d\tLevel=%d (exp=%ld)\n", 
		mptr->m.align, mptr->m.level, mptr->exp);
      my_printf("It's hitpoints are %d/%d\n", mptr->hp, mptr->hp_max);
      for(i=0; i<HPSLOT_MAX; i++) {
	 if(npc_races[mptr->m.race].bodyparts[i]>=0) {
	    if(npc_races[mptr->m.race].behave & BEHV_FLYING)
	       my_printf("%s [%d/%d] ", bodyparts_flying[i], 
			 mptr->hpp[i].cur, mptr->hpp[i].max);
	    else
	       my_printf("%s [%d/%d] ", bodyparts[i], 
			 mptr->hpp[i].cur, mptr->hpp[i].max);
	 }
      }
      my_printf("\n");
      if(mptr->m.status & MST_FLEEMODE)
	 my_printf(", It's fleeing");
      if(mptr->m.status & MST_ATTACKMODE) {
	 my_printf(", It's attacking");
	 if(mptr->target==NULL)
	    my_printf(" YOU");
	 else
	    my_printf(" %s", mptr->target->m.desc);
      }
      my_printf("\n");
      my_printf("STR=%d DEX=%d CON=%d WIS=%d INT=%d CHA=%d TGH=%d SPD=%d LCK=%d\n",
		get_stat(&mptr->stat[STAT_STR]),
		get_stat(&mptr->stat[STAT_DEX]),
		get_stat(&mptr->stat[STAT_CON]),
		get_stat(&mptr->stat[STAT_WIS]),
		get_stat(&mptr->stat[STAT_INT]),
		get_stat(&mptr->stat[STAT_CHA]),
		get_stat(&mptr->stat[STAT_TGH]),
		get_stat(&mptr->stat[STAT_SPD]),
		get_stat(&mptr->stat[STAT_LUC]) );
      rg=(real) CON_HITP[ get_stat(&mptr->stat[STAT_CON])];
      rg=rg/TIME_MOVEAROUND;
      my_printf("Regen at %d, current %d. It take appr. %4.2f turns per Hp.\n", CON_HITP[ get_stat(&mptr->stat[STAT_CON])],
		mptr->hpregen, rg);
      rg=((real)mptr->hp_max) * 0.20;
      my_printf("It flees when hitpoints fall below %4.2f\n", rg);

   }
   showmore(false, false);
   return;
}

/*
** This nasty routine generates a corpse to location mx, my
**
*/
void generate_corpse(level_type *level, _monsterlist *mons)
{
   Tinvpointer ptr;
   item_def *idef;
   int16u prob = 100;

   if(!mons) return;
   
   /* decide whether to generate a corpse or not */
   if(mons->m.weight < (4 * WEIGHT_KILO))
      prob = 5;
   else
      if(mons->m.weight < (10 * WEIGHT_KILO))
	 prob = 20;
   else
      if(mons->m.weight < (30 * WEIGHT_KILO))
	 prob = 40;
   else
      prob = 31+RANDU(70);

   if(throwdice(1, 100, 0) > prob )
      return;

   idef = (item_def *)malloc(sizeof(item_def));

   if(!idef)
      return;

   mem_clear(idef, sizeof(item_def));

   /* generate a corpse to the given coords */

   idef->type=IS_FOOD;
   idef->group=FOOD_CORPSE;

   if( (strlen(mons->m.desc)+7) <= ITEM_NAMEMAX)
      sprintf(idef->name, "%s corpse", mons->m.desc);
   else
      sprintf(idef->name, "corpse");

   /* pmod2 will have the monster race */
   idef->pmod2=mons->m.race;
   idef->pmod4=0;

   idef->icond=COND_NEW;

   /* I want to retain the special monster type for nice effects :-) */
   if(mons->m.special) idef->pmod3=mons->m.special;

   /* food items have their nutrition modifier in items 'pmod' field */
   idef->pmod1=90;
   idef->weight=mons->m.weight;

   ptr=inv_createitem(&level->inv, idef, 1);

   /* set coordinates */
   if(ptr) {
      ptr->x=mons->x;
      ptr->y=mons->y;
   }
   else
      msg.vnewmsg(CHB_RED, "Error: cannot generate a %s.", idef->name);
 
   // free temp item
   free(idef);

}

bool cansee_line(level_type *level, int16s x1, int16s y1, int16s x2, int16s y2, bool checklight)
{
   int16s d, x, y, ax, ay, sx, sy, dx, dy;

   sx=sy=0;

   dx = x2-x1;
   dy = y2-y1;

   sx = SIGN(dx);
   sy = SIGN(dy);
   ax = ABS(dx)<<1;
   ay = ABS(dy)<<1;

   x = x1;
   y = y1;
   if (ax>ay) {		/* x dominant */
      d = ay-(ax>>1);
      for (;;) {
	 if(x>=level->sizex || x<0 || y<0 || y>=level->sizey)
	    return false;

	 /* check the room light IF checklight is set */
	 if(checklight) {
	    if(!haslight(level, x, y))
	       return false;
	 }

	 if(!(level->loc[y][x].flags & CAVE_PASSABLE) )
	    return false;
	 if (x==x2) return true;
	 if (d>=0) {
	    y += sy;
	    d -= ax;
	 }
	 x += sx;
	 d += ay;
      }
   }
   else {			/* y dominant */
      d = ax-(ay>>1);
      for (;;) {
	 if(x>=level->sizex || x<0 || y<0 || y>=level->sizey)
	    return false;

	 /* check the room light IF checklight is set */
	 if(checklight) {
	    if(!haslight(level, x, y))
	       return false;
	 }

	 if(!(level->loc[y][x].flags & CAVE_PASSABLE))
	    return false;
	 if (y==y2) return true;
	 if (d>=0) {
	    x += sx;
	    d -= ay;
	 }
	 y += sy;
	 d += ax;
      }
   }
}

/*
** Can location mx,my see location px,py?
*/
int16s cansee(level_type *level, int16s mx, int16s my, int16s px, int16s py, int16u dist, bool checklight)
{
     int16s auxd=0;

     if(mx < 0 || my < 0 || px < 0 || py < 0 ||
	mx >= level->sizex || px >= level->sizex ||
	my >= level->sizey || py >= level->sizey ) {
	  msg.newmsg("ERROR!! Something is over level boundaries!", CHB_RED);
	  return false;
     }

     /* get distance to the destination */
     auxd=distance(mx, my, px, py);

     /* if distance low enough */
     if(auxd < dist) {
	  if(cansee_line(level, mx, my, px, py, checklight) )
	       return auxd;
     }
     return 0;
}

int8u monster_moveTOtarget(level_type *level, int16s *mx, int16s *my, int16s tx, int16s ty, bool getpast)
{
     int8u dir1=0;
     int8u *prioptr;
     _monsterlist *mptr;
     int16s newx, newy;
     prioptr=move_dirpriority_attack[0];

     if(tx>*mx && ty>*my) {
	  prioptr=move_dirpriority_attack[0];
     }
     if(tx<*mx && ty<*my) {
	  prioptr=move_dirpriority_attack[1];
     }
     if(tx<*mx && ty>*my) {
	  prioptr=move_dirpriority_attack[2];
     }
     if(tx>*mx && ty<*my) {
	  prioptr=move_dirpriority_attack[3];
     }
     if(tx==*mx && ty<*my) {
	  prioptr=move_dirpriority_attack[4];
     }
     if(tx==*mx && ty>*my) {
	  prioptr=move_dirpriority_attack[5];
     }
     if(tx<*mx && ty==*my) {
	  prioptr=move_dirpriority_attack[6];
     }
     if(tx>*mx && ty==*my) {
	  prioptr=move_dirpriority_attack[7];
     }

     // priodir array now has all directions in priority order
     // all we now need to do is to move to FIRST possible direction
     // in that table, easy...
     for(int i=0; i<8; i++) {
	  dir1=prioptr[i];

	  newy=*my+move_dy[dir1];
	  newx=*mx+move_dx[dir1];

	  if(getpast) {
	       mptr=istheremonster(level, newx, newy);
	       if(mptr) {
		    dir1=5;
		    continue;
	       }
	  }
	  if( ispassable(level, newx, newy ) ||
	      (level->loc[newy][newx].type == TYPE_DOORCLOS) ) {
	       *mx=newx;
	       *my=newy;
	       break;
	  }

     }
     return dir1;
}


int8u monster_moveFROMtarget(level_type *level, int16s *mx, int16s *my, int16s tx, int16s ty)
{
     int8u dir1=0;

     int8u *prioptr;

     prioptr=move_dirpriority_flee[0];

     if(tx>*mx && ty>*my) {
	  prioptr=move_dirpriority_flee[0];
     }
     if(tx<*mx && ty<*my) {
	  prioptr=move_dirpriority_flee[1];
     }
     if(tx<*mx && ty>*my) {
	  prioptr=move_dirpriority_flee[2];
     }
     if(tx>*mx && ty<*my) {
	  prioptr=move_dirpriority_flee[3];
     }
     if(tx==*mx && ty<*my) {
	  prioptr=move_dirpriority_flee[4];
     }
     if(tx==*mx && ty>*my) {
	  prioptr=move_dirpriority_flee[5];
     }
     if(tx<*mx && ty==*my) {
	  prioptr=move_dirpriority_flee[6];
     }
     if(tx>*mx && ty==*my) {
	  prioptr=move_dirpriority_flee[7];
     }

     // priodir array now has all directions in priority order
     // all we now need to do is to move to FIRST possible direction
     // in that table, easy...
     for(int i=0; i<8; i++) {
	  dir1=prioptr[i];

	  if( (level->loc[*my+move_dy[dir1]][*mx+move_dx[dir1]].type == TYPE_DOORCLOS) ) {
	       *mx+=move_dx[dir1];
	       *my+=move_dy[dir1];
	       break;
	  }

	  if( ispassable(level, *mx+move_dx[dir1], *my+move_dy[dir1])  ) {
	       *mx+=move_dx[dir1];
	       *my+=move_dy[dir1];
	       break;
	  }

     }
     return dir1;
}

int8u monster_moveAROUNDtarget(level_type *level, int16s *mx, int16s *my, int16s tx, int16s ty)
{
     int8u dir1=0;

     int8u *prioptr;
     int16s newx, newy;
     _monsterlist *mptr;

     prioptr=move_dirpriority_circulate[0];

     if(tx>*mx && ty>*my) {
	  prioptr=move_dirpriority_circulate[0];
     }
     if(tx<*mx && ty<*my) {
	  prioptr=move_dirpriority_circulate[1];
     }
     if(tx<*mx && ty>*my) {
	  prioptr=move_dirpriority_circulate[2];
     }
     if(tx>*mx && ty<*my) {
	  prioptr=move_dirpriority_circulate[3];
     }
     if(tx==*mx && ty<*my) {
	  prioptr=move_dirpriority_circulate[4];
     }
     if(tx==*mx && ty>*my) {
	  prioptr=move_dirpriority_circulate[5];
     }
     if(tx<*mx && ty==*my) {
	  prioptr=move_dirpriority_circulate[6];
     }
     if(tx>*mx && ty==*my) {
	  prioptr=move_dirpriority_circulate[7];
     }

     // priodir array now has all directions in priority order
     // all we now need to do is to move to FIRST possible direction
     // in that table, easy...
     for(int i=0; i<8; i++) {
	  dir1=prioptr[i];

	  newy=*my+move_dy[dir1];
	  newx=*mx+move_dx[dir1];

	  mptr=istheremonster(level, newx, newy);

	  if(mptr || (player.pos_x==newx && player.pos_y==newy)) {
	       dir1=5;
	       continue;
	  }

	  if( (level->loc[newy][newx].type == TYPE_DOORCLOS) ||
	      ispassable(level, newx, newy) ) {
	       *mx=newx;
	       *my=newy;
	       break;
	  }

     }
     return dir1;
}

/* try to give a rating for some item */
/* bigger result is better */
int16u monster_rateitem(item_def *item)
{
   int16u value;
   _matlist *matptr;

   value=0;

   if(!item)
     return 0;

   /*
    *  get pointer to the material list,
    * this is because there're two lists, one for armor items
    * and the other for the rest of items
    */
   matptr=materials;

   /* money is good to keep :) */
   if(item->type==IS_MONEY)
     return 1000;

   matptr+=item->material;

   /* at this time, rate only by appearing propability */
   value+=matptr->appearprob;
   value+=matptr->dam;
   value+=matptr->hit;
   value+=matptr->speed;
//   value-=matptr->breakprob;

   return value;
}

/*
 * Routine to find a best item type of "type" contained in inventory
 * "inv".
 *
 */
Tinvpointer inv_findbestitem(Tinventory *inv, int16u type, int16s group)
{
   Tinvpointer invptr, best;
   bool better;

   invptr=inv->first;
   best=NULL;

   while(invptr) {
      better=false;
      if(invptr->i.type == type) {

	 if(group>=0) {
	    if(invptr->i.group == (int16u)group)
	       return invptr;

	 }
	 else {

	    if(best==NULL)
	       better=true;
	    else {
	       if(monster_rateitem( &invptr->i) > monster_rateitem(&best->i))
		  better=true;
	    }
	    if(better)
	       best=invptr;
	 }
      }

      invptr=invptr->next;
   }

   return best;
}

#define MONEQ_OK          0x00
#define MONEQ_CURSEDITEM  0x01
#define MONEQ_NOTEQUIPPED 0x02

bool monster_unequip(level_type *level, _monsterlist *monster, 
		     Tinvpointer removeitem)
{
   if(removeitem->slot < 0)
      return MONEQ_NOTEQUIPPED;

   if(removeitem->i.status & ITEM_CURSED)
      return MONEQ_CURSEDITEM;

//   if(cansee(level, player.pos_x, player.pos_y, monster->x, monster->y,
//	     player.sight, true)) {
   if(player_has_los(level, monster->x, monster->y)) {

      monster_sprintf(nametemp, monster, true, true);
      sprintf(nametemp2, "%s unequipped", nametemp);
      item_printinfo(&removeitem->i, removeitem->i.weight, removeitem->count,
		     nametemp2);
   }
//   msg.newmsg(tempstr, CH_CYAN);

   /* remove equipped item from equipslot */
   inv_clearequipslot(&monster->inv, monster->equip, removeitem->slot);


   return MONEQ_OK;

}

bool monster_equip(level_type *level, _monsterlist *monster, 
		   Tinvpointer useitem)
{
   int16u stat1=0, stat2=0;
   int16u useslot=EQUIP_RHAND;

   useitem=inv_splitpile(&monster->inv, useitem);

   /* put twhohanded weapons always on RIGHT hand */
   if( useitem->i.type == IS_WEAPON2H ||
      useitem->i.type == IS_MISWEAPON) {

      useslot=EQUIP_RHAND;

      if(monster->equip[EQUIP_RHAND].item)
	 stat1=monster_unequip(level, monster, 
			       monster->equip[EQUIP_RHAND].item);
      if(monster->equip[EQUIP_LHAND].item)
	 stat2=monster_unequip(level, monster, 
			       monster->equip[EQUIP_LHAND].item);
     
   }
   else if( useitem->i.type == IS_WEAPON1H) {
      useslot=EQUIP_RHAND;

      if(monster->equip[useslot].item) {
	 useslot=EQUIP_LHAND;
      }

      /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
      /* THIS NEED TO BE FIXED !!!!!!!!!!!!!!! */
      if(monster->equip[useslot].item) {
	 return false;
      }      


   }

   if(stat1 == MONEQ_OK && stat2==MONEQ_OK) {
      /* equip the item */
      monster->equip[useslot].item = useitem;
      useitem->slot=useslot;

      if(player_has_los(level, monster->x, monster->y)) {
//      if(cansee(level, player.pos_x, player.pos_y, monster->x, monster->y,
//		player.sight, true)) {

	 monster_sprintf(nametemp, monster, true, true);
	 sprintf(nametemp2, "%s just equipped", nametemp);
	 item_printinfo(&useitem->i, useitem->i.weight, 
			useitem->count,
			nametemp2);
      }
      return true;
   }
   
   return false;

}


bool monster_useitems(level_type *level, _monsterlist *monster)
{
   Tinvpointer usenode=NULL;
   Tinvpointer bow=NULL;
   
   bow=inv_findbestitem(&monster->inv, IS_MISWEAPON, -1);

   /* first, find a best melee weapon */
   usenode = inv_findbestitem(&monster->inv, IS_WEAPON2H, -1);

   if(!usenode) 
      usenode = inv_findbestitem(&monster->inv, IS_WEAPON1H, -1);
   
   bool mistat=false;

   /* for now, give priority for ranged weapons */
   if(bow) {
      if(bow->slot < 0) {
	 /* ready missiles for the bow */
	 if(bow->i.group == WPN_BOW)
	    mistat=ready_newmissile(&monster->inv, monster->equip, 
				    IS_MISSILE, WPN_MISSILE_1);
	 else if(bow->i.group == WPN_CROSSBOW)
	    mistat=ready_newmissile(&monster->inv, monster->equip, 
				    IS_MISSILE, WPN_MISSILE_2);

	 /* equip bow if there're missiles also */
	 if(mistat) {
	    usenode=bow;
	 }

      }
   }

   if(!usenode) 
      return false;

   /* is the item already used */
   if(usenode->slot >= 0) {
      return true;
   }

//   sprintf(tempstr, "Somebody used: %s!", usenode->i.name);
//   msg.newmsg(tempstr, CH_YELLOW);

   monster_equip(level, monster, usenode);

   calculate_itembonus(monster);

   return true;
}


bool monster_get(level_type *level, _monsterlist *monster, Tinvpointer itemptr)
{
   Tinvpointer invptr;

   if(!itemptr)
      return false;

   /* animals doesn't take items */
   if(npc_races[monster->m.race].behave & BEHV_ANIMAL)
      return false;

   /* and monsters don't take shop items (unpaid!) */
   if((itemptr->i.status & ITEM_UNPAID))
      return false;

   /* copy the item and other info */
   invptr=inv_transfer2inv(level, &level->inv, NULL, &monster->inv, itemptr,
			   itemptr->count, -1, -1);
   if(!invptr)
      return false;

   /* print a short message about the action */
   if(player_has_los(level, monster->x, monster->y)) {
      monster_sprintf(nametemp, monster, true, true );
      sprintf(tempstr, "%s just took something.", nametemp);
      msg.add_dist(level, monster->x, monster->y, 
		   tempstr, C_WHITE, NULL, C_CYAN);
   }

   /* after getting an item, monster should decide what to do what it */
   /* scan all items with this function */
   monster_useitems(level, monster);

   return true;
}

/*
** A very intelligent monster move routine :-)
**
** 1.7.-97 HEY! Now monsters can do nicer target follow and flee.
**              it's not perfect but works nicely for now.
**
** This routine returns the amount of timeunits taken by monster move
*/
int16s move_monster(_monsterlist *monster, level_type *level)
{
   _monsterlist *mptr;
   Tinvpointer iptr;
   int8u dir=0, stat;
   int16s ny, nx, tx, ty;
   int16s timetaken;

   timetaken=0;

   /* NULL monsters are unable to move... (REALLY?!) should I create
      a null monster to the list... whaahaahaa! :)*/
   if(!monster) 
      return 0;

   nx=monster->x;
   ny=monster->y;

   /* if monster is in attackmode, let him attack his target (STUPID!)*/
   /* with this stupid code, hostile monsters just need to move into */
   /* players position... :-) */

   /* get coordinates for the target */

   if(monster->target) {
      tx=monster->target->x;
      ty=monster->target->y;
   }
   else if(monster->targetx && monster->targety) {
      /* pursue item located in level */
      tx=monster->targetx;
      ty=monster->targety;
   }
   else {
      /* if there is no target, then attack player */
      tx=player.pos_x;
      ty=player.pos_y;
   }
   
   /* confusion walk */
   if(cond_check(monster->conditions, CONDIT_CONFUSED) > 0) {
      dir=monster->lastdir=1+RANDU(9);
      tx=nx - move_dx[monster->lastdir];
      ty=ny - move_dy[monster->lastdir];
   }

   /* first check the FLEEMODE */
   if((monster->m.status & MST_FLEEMODE)) {
      monster_moveFROMtarget(level, &nx, &ny, tx, ty);
   }
   else if((monster->m.status & MST_ATTACKMODE)) {
      if(!monster->target)
	 monster_moveTOtarget(level, &nx, &ny, tx, ty, true);
      else
	 monster_moveTOtarget(level, &nx, &ny, tx, ty, false);
   }
   else if((monster->m.status & MST_PURSUEITEM)) {
      monster_moveTOtarget(level, &nx, &ny, tx, ty, true);
   }
   /* otherwise move randomly :-) */
   else {
      /* if the monster has a path */
      if(monster->path!=NULL) {
	 path_movealong(&monster->path, &nx, &ny);
      }
      else
      /* if monster is in corridor mode, move from last position */
      if((monster->lastdir>0) && (monster->lastdir<10)) {
	 tx=nx - move_dx[monster->lastdir];
	 ty=ny - move_dy[monster->lastdir];
	 monster->lastdir=monster_moveFROMtarget(level, &nx, &ny, tx, ty);

	 /* end corridor mode when over 2 possible directions to walk */
	 if(sur_countall(level, nx, ny)>4)
	    monster->lastdir=0;
      }
      else {
	 dir=1+RANDU(9);
	 tx=monster->x + move_dx[dir];
	 ty=monster->y + move_dy[dir];

	 monster->lastdir=monster_moveFROMtarget(level, &nx, &ny, tx, ty);

	 if(nx < 1) nx=1;
	 if(ny < 1) ny=1;
	 if(nx > level->sizex-2) nx=level->sizex-2;
	 if(ny > level->sizey-2) ny=level->sizey-2;


	 if( (sur_countall(level, nx, ny)>4) )
	    monster->lastdir=0;
      }
   }

/*
   if((nx>=level->sizex) || (ny>=level->sizey) || (ny<0) || (nx<0)) {
      sprintf(tempstr, 
	      "MONSTER %s IS BEYOND LEVEL LIMITS at %d:%d!", 
	      monster->m.desc,
	      nx, ny);
      msg.newmsg(tempstr, CHB_RED);
      monster->lastdir=0;
      return 800;
   }
*/

   /* open closed doors */
   if( level->loc[ny][nx].type == TYPE_DOORCLOS ) {
      if( npc_races[monster->m.race].behave & BEHV_ANIMAL ) {
	 if(!(npc_races[monster->m.race].behave & BEHV_CANUSEITEM)) {
	    monster->lastdir=0;
	    return 0;
	 }
      }
      stat=door_open(level, nx, ny, monster);

      return calc_time(get_stat(&monster->stat[STAT_SPD]), 
		       TIME_CLOSEDOOR, monster->tactic);
   }

   if( ny==player.pos_y && nx==player.pos_x) {
      if( (monster->m.status & MST_ATTACKMODE) && monster->target==NULL) {
	 timetaken=calc_time(get_stat(&monster->stat[STAT_SPD]), 
			     TIME_MELEEATTACK, monster->tactic);

//	 monster_meleeattack_player(level, monster);
	 monster_meleeattack(level, monster, NULL);


	 /* return after attack */
	 return timetaken;
      }
      else {
	 timetaken=calc_time(get_stat(&monster->stat[STAT_SPD]), 
			     TIME_MOVEAROUND, monster->tactic);

	 /* forget corridor mode */
	 monster->lastdir=0;

	 nx=monster->x;
	 ny=monster->y;

	 if(monster_moveAROUNDtarget(level, &nx, &ny, 
				     player.pos_x, player.pos_y)==5) {
	    monster->targetx=0;
	    monster->targety=0;

	    if(monster->m.status & MST_PURSUEITEM)
	       monster->m.status^=MST_PURSUEITEM;

	    /* make the monster a bit more angry */
	    monster->m.attitude++;

	    if(player_has_los(level, monster->x, monster->y)) {
	       monster_sprintf(nametemp, monster, false, true );
	       sprintf(tempstr, 
		       "You are standing on %s's way!", nametemp);
	    }
	    else
	       sprintf(tempstr, "Someone tries to get past of you.");

	    msg.newmsg(tempstr, C_WHITE);

	 }
	 /* take a move from players coord */

	 /* forget target */

      }
   }
   else {
      mptr=NULL;
      if( (nx!=monster->x) || (ny!=monster->y)) {
	 mptr=istheremonster(level, nx, ny);
      }

      if(mptr) {
	 if((monster->m.status & MST_ATTACKMODE) && 
	    (mptr==monster->target)) {
	    /* do the attack */
//	    monster_meleeattack_monster(level, monster, mptr);
	    monster_meleeattack(level, monster, mptr);

	    /* timeunits taken by monster attack */
	    timetaken=calc_time(get_stat(&monster->stat[STAT_SPD]), 
				TIME_MELEEATTACK, monster->tactic);
	 }
	 else {
	    /* angrify monster */
	    timetaken=calc_time(get_stat(&monster->stat[STAT_SPD]), 
				TIME_MOVEAROUND, monster->tactic);
	    monster->m.attitude++;
	 }

	 monster->lastdir=0;

	 /* return after attack */
	 return timetaken;

      }
   }

   /* set new coordinates */
   if( ispassable(level, nx, ny) ) {
/*
      if(nx>=level->sizex-2 || ny>=level->sizey-2 || ny<2 || nx<2) {
	 monster->lastdir=0;
	 return 0;
      }
*/
//      else {

	 /* monster is able to move if LEGS HP > 0 or it doesn't have
	    legs, legless monsters move with their body! */
	 if(!( monster->m.status & MST_CANTMOVE) ) {
	    monster->x=nx;
	    monster->y=ny;
	 }
	 else {
	    if(RANDU(100) < 20) {
	       monster_sprintf(nametemp, monster, true, true );
	       sprintf(tempstr, 
		       "%s seems unable to move!", nametemp);
	       msg.add_dist(level, monster->x, monster->y, 
			    tempstr, C_WHITE, NULL, CHB_CYAN);
	    }
	 }
//      }
      /* timeunits taken by monster move */
      timetaken=calc_time(get_stat(&monster->stat[STAT_SPD]), 
			  TIME_MOVEAROUND, monster->tactic);

      if((monster->m.status & MST_PURSUEITEM) && 
	 monster->targetx && monster->targety) {
	 if(monster->targetx==monster->x && 
	    monster->targety==monster->y) {

	    /**********************************************/
	    /* SOMEHOW SHOULD DECIDE TO GET OR NOT TO GET */
	    /**********************************************/

	    iptr=giveiteminfo(level, monster->x, monster->y);
	    while(iptr) {
	       /* take an item */
	       if(!monster_get(level, monster, iptr))
		  break;

	       /* check if there's another item to take */
	       iptr=giveiteminfo(level, monster->x, monster->y);

	    }

	    monster->m.status^=MST_PURSUEITEM;
	    monster->targetx=monster->targety=0;

	    timetaken=calc_time( 
	       get_stat(&monster->stat[STAT_SPD]), 
	       TIME_PICKUP, monster->tactic);
	 }
      }
   }
   else {
      monster->lastdir=0;
   }

   return timetaken;
}

void act_specialmonster(_monsterlist *monster, level_type *level)
{
     switch(monster->m.special) {
     case NPC_THOMAS:
	  thomas_act(monster, level);
	  break;
     case NPC_BILLGATES:
	  billgates_act(monster, level);
	  break;
     case NPC_SPARHAWK:
	  sparhawk_act(monster, level);
	  break;
     default: break;
     }
}

#define RESP_NOTHUMANCOUNT 4
char *resp_nothumanly[]=
{
     "%s grunts.",
     "%s squawks.",
     "%s yodls.",
     "%s yarns.",
     NULL
};

#define RESP_HUMANLYCOUNT 4
char *resp_humanly[]=
{
     "We meet again, %s.",
     "Oh, the world is so small, %s.",
     "Nice to see you again, %s!",
     "Where've you been, %s?",
     NULL
};

void monster_talk(_monsterlist *monster)
{
   int8u bbc=0;

   if(npc_races[monster->m.race].behave & BEHV_ANIMAL) {
      monster_sprintf(nametemp, monster, true, true );
      msg.vnewmsg(C_WHITE, resp_nothumanly[RANDU(RESP_NOTHUMANCOUNT)], 
		  nametemp);
      return;
   }

   /* if it's attacking the player, it wont't talk much */
   if(monster->m.status & MST_ATTACKMODE && monster->target==NULL) {
      msg.newmsg("No response.", C_WHITE);
      return;
   }

   my_clrscr();
   my_enablescroll();

   monster_sprintf(nametemp, monster, true, true );
   my_printf("%s, you are now talking with %s.\n\n", player.name, nametemp);

   if(monster->m.status & MST_KNOWN) 
	zprintf("%s\n", NPCchatinfo[monster->m.special].knowgreet);
   else
	zprintf("%s\n", NPCchatinfo[monster->m.special].firstgreet);

   while(1) {
      my_setcolor(CH_RED);
      my_printf("> ");
      my_setcolor(CH_WHITE);
      my_gets(tempstr, sizeof(tempstr)-1);

      if( my_stricmp(tempstr, "bye")==0)
	 break;
      if( strlen(tempstr)==0 ) {
	 bbc++;
	 if(bbc==2)
	    break;

	 my_setcolor(CH_CYAN);
	 my_printf("To end talking, type \"bye\" or press another enter.\n");
	 
      }
      else {
	 bbc=0;
	 my_setcolor(C_WHITE);
	 wordtalk(monster, tempstr);
	 my_printf("\n");
      }
   }

   my_disablescroll();
   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

   return;

   if(monster->m.special)
      monster_chatspecials(monster);
   else {
      if(npc_races[monster->m.race].behave & BEHV_ANIMAL) {
	 monster_sprintf(nametemp, monster, true, true );
	 sprintf(tempstr, 
 		 resp_nothumanly[RANDU(RESP_NOTHUMANCOUNT)], nametemp);
      }
      else {
	 if(!(monster->m.status & MST_KNOWN)) {
	    if(strlen(monster->m.name)>0) {
	       monster->m.status|=MST_KNOWN; // monster has been met now
	       sprintf(tempstr, "Hi, my name is %s", monster->m.name);
	    }
	    else {
	       random_language(tempstr, 30+RANDU(40));
	       strcat(tempstr, "?");
	    }
	 }
	 else {
	    sprintf(tempstr, resp_humanly[RANDU(RESP_HUMANLYCOUNT)], player.name);
	 }
      }
      msg.newmsg(tempstr, C_WHITE);
   }

}

void monster_chatspecials(_monsterlist *mptr)
{
     switch(mptr->m.special) {
     case NPC_BILLGATES:
	  billgates_chat(mptr);
	  break;
     case NPC_THOMAS:
	  thomas_chat(mptr);
	  break;
     default:
	  msg.newmsg("He hasn't much to talk about", C_WHITE);
     }
}

void eat_specialmonster(level_type *level, int16u type)
{
     switch(type) {
     case NPC_BILLGATES: eat_billgates(level);
	  break;
     default:
	  break;

     }
}

/* TŽSSŽ ON JOTAIN BUGIA? */

void eat_billgates(level_type *level)
{
     msg.setdelay(300);
     msg.add("...as you finish your meal your stomach", C_WHITE);
     msg.add("starts to vibrate violently", C_WHITE);
     msg.add("..oh no..", C_WHITE);
     msg.add("...this really...", C_GREEN);
     msg.add("...wasn't a good idea...", CH_GREEN);
     msg.add("...........", C_RED);
     msg.add("You transform into Windows 95(R)...", C_RED);
     msg.add("...you try to keep operating...", C_RED);
     msg.setdelay(500);
     msg.add("...but then...", C_RED);
     msg.setdelay(0);

     cond_add(&player.conditions, CONDIT_DEAD, 1);

     my_strcpy(player_killer, "eating a very unstable meal", 
	       sizeof(player_killer));
     msg.add("... you crash.", CH_RED);
     player.alive=false;

//     player_takehit(level, 10000, -1, "*** YOU CRASH ***");

}

/***************************************************
 ** SPECIAL NPC ROUTINES HERE                     **
 **                                               **
 **                                               **
 **                                               **
 **                                               **
 ***************************************************/

/***************************************************
 * HERE IS BILL GATES, A VERY INTELLIGENT NPC      *
 ***************************************************/

#define BILLGATES_NUMMSG   8
char *msg_billgates[]=
{
     "Bill Gates throws you a fact about Microsoft Corporation",
     "Bill Gates smiles to you! UGh!",
     "Bill Gates tries to sell you some Microsoft stocks!",
     "Bill is trying to sell you Windows 95",
     "Bill Gates tries to recruit you into Microsoft!",
     "Bill Gates is hideously blinking an eye to you!",
     "Bill Gates says: linux is bad for your health!",
     "Bill Gates weeps...buuhhuu",
     NULL
};

void billgates_act(_monsterlist *monster, level_type *level)
{
     if(RANDU(100) < 10)
	  msg.add_dist(level, monster->x, monster->y, 
		       msg_billgates[ RANDU(BILLGATES_NUMMSG) ], 
		       1+RANDU(15), NULL, 0);
}

void billgates_chat(_monsterlist *mptr)
{
     if(!(mptr->m.status & MST_KNOWN)) {
	  msg.newmsg("Hello, my name is Bill Gates!", C_GREEN);
	  mptr->m.status|=MST_KNOWN; // monster has been met now
     }
     else {
	  sprintf(tempstr, "Hello again %s!", player.name);
	  msg.newmsg(tempstr, C_GREEN);
     }
}

/***************************************************/
/* HERE IS SPARHAWK ROUTINES                       */
/***************************************************/
#define SPARHAWK_NUMMSG 4
char *msg_sparhawk[]=
{
     "\"You really should equip some weapons!\", Sparhawk says.",
     "\"Armor is good for protection!\", Sparhawk tells you.",
     "Sparhawk informs: \"There's a small dungeon near this city!\"",
     "\"Go and check out the local stores.\", Sparhawk informs you.",
     NULL
};
void sparhawk_act(_monsterlist *monster, level_type *level)
{
     if(RANDU(100) < 10)
	  msg.add_dist(level, monster->x, monster->y, 
		       msg_sparhawk[RANDU(SPARHAWK_NUMMSG)], CH_GREEN, NULL, 0);
     
}

/***************************************************/
/* HERE IS THOMAS BISKUPS ROUTINES                 */
/***************************************************/
#define THOMAS_NUMMSG   4
char *msg_thomas[]=
{
     "\"You really should pay a visit to my world!\", Thomas says.",
     "\"Why are you wandering here?\", Thomas asks.",
     "Thomas says: \"Obviously you are in the wrong world!\"",
     "Have you played ADOM?",
     NULL
};

void thomas_act(_monsterlist *monster, level_type *level)
{
     if(RANDU(100) < 10)
	  msg.add_dist(level, monster->x, monster->y, 
		       msg_thomas[RANDU(THOMAS_NUMMSG)], CH_GREEN, NULL, 0);
}

void thomas_chat(_monsterlist *mptr)
{
     if(!(mptr->m.status & MST_KNOWN)) {
	  msg.add("\"Hey, is it really you?\", Thomas suddenly asks.", CH_WHITE);
	  sprintf(tempstr, "You probably can't recognize me. My name is %s!", mptr->m.name);
	  msg.add(tempstr, CH_WHITE);
	  msg.add("After you last time saw me I have met a terrible misfortune,", CH_WHITE);
	  msg.add("my own world turned against me and now I am corrupted like hell and", CH_WHITE);
	  msg.add("stuck here in this world!", CH_WHITE);
	  msg.add("I really would like to get rid of these bloody corruptions and", CH_WHITE);
	  msg.add("finally get back to my world Ankardia!", CH_WHITE);
	  mptr->m.status|=MST_KNOWN;
	  msg.add("Are you willing to help me with this (Y/n)?", C_GREEN);
//	  msg.notice();
	  if(!confirm_yn(NULL, true, false)) {
	       msg.add("You bastard!", C_RED);
	       msg.add("Thomas hits you with DJGPP C++ compiler!", C_RED);
	       msg.add("You are badly optimized", C_RED);
	       msg.add("You feel slower...", C_GREEN);
	       change_stat_perm(&player.stat[STAT_DEX], -RANDU(20), true);

	  }
	  else
	       msg.add("Thanks, quest will be here later!", CH_GREEN);
     }
     else {
	  sprintf(tempstr, "Any news for me %s!", player.name);
	  msg.add(tempstr, C_GREEN);
     }

}


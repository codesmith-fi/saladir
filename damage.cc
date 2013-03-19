/**************************************************************************
 * damage.cc --                                                           *
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
#include "defines.h"
#include "player.h"
#include "monster.h"
#include "inventor.h"
#include "dungeon.h"
#include "utility.h"
#include "damage.h"

/* local protos */
void damage_issue_inner(_monsterlist *target, 
			int8u element, int16s damage, int16s bodypart);
void damage_msg_player(int16s damage, int16s bodypart);
void damage_msg_monster(_monsterlist *target, int16s damage, int16s bodypart);


void damage_checkbodyparts(level_type *level, _monsterlist *mptr)
{
   _statpack *statptr;
   _hpslot *hpptr;
   _Tequipslot *equip;
   Tinventory *inv;
   int16s *stdhp;
   int16u race;
   int16s px, py;
   int8u slot;
   
   /* set pointers regarding to who we are handling */
   if(!mptr) {
      equip=player.equip;
      statptr=player.stat;
      hpptr=player.hpp;
      race=player.prace;
      inv=&player.inv;
      stdhp=&player.hp;
      px=player.pos_x;
      py=player.pos_y;
      
   }
   else {
      /* take the name of the monster to 'nametemp' */
      monster_sprintf(nametemp, mptr, true, true );
      equip=mptr->equip;
      statptr=mptr->stat;
      hpptr=mptr->hpp;
      race=mptr->m.race;
      inv=&mptr->inv;
      stdhp=&mptr->hp;
      px=mptr->x;
      py=mptr->y;
   }
   
   /* when head falls below or to zero, the creature SHOULD die! */
   if(hpptr[HPSLOT_HEAD].cur <= 0) {
      *stdhp=0;
      if(!mptr)
	 cond_add(&player.conditions, CONDIT_DEAD, 1);
      else
	 cond_add(&mptr->conditions, CONDIT_DEAD, 1);
   }
   
   /* when body falls below or to zero, the creature SHOULD die! */
   if(hpptr[HPSLOT_BODY].cur <= 0) {
      *stdhp=0;
      if(!mptr)
	 cond_add(&player.conditions, CONDIT_DEAD, 1);
      else
	 cond_add(&mptr->conditions, CONDIT_DEAD, 1);
   }
   
   /* lefthand and righthand should be disabled and weapons dropped */
   if( (hpptr[HPSLOT_LEFTHAND].cur <= 0) ) {
      hpptr[HPSLOT_LEFTHAND].cur=0;
      
      if( (equip[EQUIP_LHAND].status==EQSTAT_OK) ) {
	 
	 /* here must drop item from left hand */
	 /* and disable that hand */
	 equip[EQUIP_LHAND].status=EQSTAT_BROKEN;
	 
	 /* if damaged hand is reserved by another item drop the item
	    from the hand which reserved this slot */
	 if(equip[EQUIP_LHAND].reserv)
	    slot=equip[EQUIP_LHAND].reserv;
	 else
	    slot=EQUIP_LHAND;
	 
	 if(!mptr) {
	    msg.newmsg("Your left hand is disabled!", C_RED);
	    cond_add(&player.conditions, CONDIT_BADLARM, 1);

	    if(equip[slot].item) {
	       sprintf(tempstr, "Your %s fell down.", 
		       equip[slot].item->i.name);

	       /* drop the item */
//	       if(equip_dropfromslot(level, inv, slot, px, py))
//		  msg.newmsg(tempstr, C_RED);
	       if(inv_transfer2inv(level, inv, equip, &level->inv,
			       equip[slot].item, 1, px, py))
		  msg.newmsg(tempstr, C_RED);
	       
	    }
	 }
	 else {
	    cond_add(&mptr->conditions, CONDIT_BADLARM, 1);

	    if(npc_races[mptr->m.race].behave & BEHV_FLYING) {

	       sprintf(tempstr, "%s drops to the ground.", nametemp);
	       msg.add_dist(level, px, py, tempstr, C_WHITE, 
			    "You hear a \"splat.\".", C_WHITE);
	       mptr->m.status|=MST_CANTMOVE;
	    }
	    if(equip[slot].item) {
	       sprintf(tempstr, "%s lost %s %s.", nametemp, 
		       gender_art3[mptr->m.gender], equip[slot].item->i.name);
	       /* drop the item */
	       if(inv_transfer2inv(level, inv, equip, &level->inv,
			       equip[slot].item, 1, px, py))
		  msg.add_dist(level, px, py, tempstr, C_RED, NULL, CHB_CYAN);
	    }
	 }
      }
   }

   if( (hpptr[HPSLOT_RIGHTHAND].cur <= 0) ) {
      hpptr[HPSLOT_RIGHTHAND].cur=0;
      
      if( (equip[EQUIP_RHAND].status==EQSTAT_OK) ) {
	 
	 /* here must drop item from left hand */
	 /* and disable that hand */
	 equip[EQUIP_RHAND].status=EQSTAT_BROKEN;
	 
	 /* if damaged hand is reserved by another item drop the item
	    from the hand which reserved this slot */
	 if(equip[EQUIP_RHAND].reserv)
	    slot=equip[EQUIP_RHAND].reserv;
	 else
	    slot=EQUIP_RHAND;
	 
	 if(!mptr) {
	    cond_add(&player.conditions, CONDIT_BADRARM, 1);
	    msg.newmsg("Your right hand is disabled!", C_RED);
	    if(equip[slot].item) {
	       sprintf(tempstr, "Your %s fell down.", 
		       equip[slot].item->i.name);
	    
	       /* drop it */
	       if(inv_transfer2inv(level, inv, equip, &level->inv,
			       equip[slot].item, 1, px, py))
		  msg.newmsg(tempstr, C_RED);
	    }
	 }
	 else {
	    cond_add(&mptr->conditions, CONDIT_BADRARM, 1);

	    if(npc_races[mptr->m.race].behave & BEHV_FLYING) {	    
	       sprintf(tempstr, "%s drops to the ground.", nametemp);
	       msg.add_dist(level, px, py, tempstr, C_WHITE, 
			    "You hear a distant \"splat.\".", C_WHITE);
	       mptr->m.status|=MST_CANTMOVE;
	    }

	    if(equip[slot].item) {
	    
	       sprintf(tempstr, "%s lost %s %s.", nametemp,
		       gender_art3[mptr->m.gender], equip[slot].item->i.name);
	    
	       /* drop the item */
	       if(inv_transfer2inv(level, inv, equip, &level->inv,
			       equip[slot].item, 1, px, py))
		  msg.add_dist(level, px, py, tempstr, C_RED, NULL, CHB_CYAN);
	    }
	 }
      }
   }   

   /* legs should stop working, no walking, fall to the ground etc. */
   if(hpptr[HPSLOT_LEGS].cur <= 0 ) {
      hpptr[HPSLOT_LEGS].cur=0;

      if( npc_races[race].bodyparts[HPSLOT_LEGS] < 0)
	 return;

      if( (equip[EQUIP_LEGS].status==EQSTAT_OK) ) {
	 
	 equip[EQUIP_LEGS].status=EQSTAT_BROKEN;
	 
         if(!mptr) {
	    cond_add(&player.conditions, CONDIT_BADLEGS, 1);
	    msg.newmsg("Your legs are disabled!", C_RED);
	    msg.newmsg("You fall to the ground.", CH_RED);
	 }
	 else {
	    cond_add(&mptr->conditions, CONDIT_BADLEGS, 1);

	    /* check for flying creatures */
	    if(! (npc_races[mptr->m.race].behave & BEHV_FLYING)) {
	       
	       sprintf(tempstr, "%s falls to the ground.", nametemp);
	       msg.add_dist(level, px, py, tempstr, C_RED, NULL, CHB_CYAN);
	       
               mptr->m.status|=MST_CANTMOVE;
	    }
	 }
      }
   }
}

int16s damage_issue(level_type *level, 
		    _monsterlist *target, _monsterlist *attacker,
		    int8u element, int16s damage, int16s bodypart,
		    char *message)
{  
   int16u i, st=0, ed=0;
   bool bodydam=false;
   int16s idam;

   if(bodypart<0 || bodypart>=HPSLOT_MAX) {
      st=0;
      ed=HPSLOT_MAX;
      bodydam=true;
   }
   else {
      bodydam=false;
      st=bodypart;
      ed=bodypart+1;
      
   }

   /* show the hit message */
   if(message!=NULL) {
      if(!target)
	 msg.newmsg(message, C_RED);
      else if(player_has_los(level, target->x, target->y))
	 msg.newmsg(message, C_RED);
   }


   if(!target) 
      damage_msg_player(damage, bodypart);
   else {
      damage_msg_monster(target, damage, bodypart);      
   }
	 
   /* issue damage */
   for(i=st; i<ed; i++) {
      if(bodydam) {
	 idam=(int16s)(hp_bpmod[i] * (real)damage);	 
      }
      else
	 idam=damage;

      damage_issue_inner(target, element, idam, i);
   }

   /* return remaining hitpoints */
   if(target) {
      target->hp=calculate_totalhp(target->hpp);

      damage_checkbodyparts(level, target);

      if(target->hp>0) {
	 /* lets make the monster really angry */
	 if(attacker==NULL) {
	    /* make angry only if sees the player */
	    if(player_has_los(level, target->x, target->y))
	       monster_getangry(level, target, NULL);
	 }
	 else
	    monster_getangry(level, target, attacker);

	 monster_shouldflee(level, target);
      }
      
      return target->hp;
   }
   else {
      GAME_NOTIFYFLAGS|=GAME_HPSPCHG;
      player_checkstat(level, true, true);
      return player.hp;
   }
}

void damage_issue_inner(_monsterlist *target, int8u element, int16s damage, 
			int16s bodypart)
{
   _statpack *statptr;
   _hpslot *hpptr;
   Tinventory *inv;
   int16u race;
   int16s dprot=0;

   if(target) {
      statptr=target->stat;
      hpptr=target->hpp;
      race=target->m.race;
      inv=&target->inv;
   }
   else {
      statptr=player.stat;
      hpptr=player.hpp;
      race=player.prace;
      inv=&player.inv;

   }

   /* if target has no such bodypart, cancel out */
   if(npc_races[race].bodyparts[bodypart] < 0)
      return;

   /* for damage protection */
   switch(element) {
      /* normal weapon hit damage */
      case ELEMENT_NOTHING:
	 dprot=0;
	 break;
      case ELEMENT_FIRE:
	 dprot=(int16s)(((real)damage/100.0) * hpptr[bodypart].res.fire);
	 break;
      case ELEMENT_POISON:
	 dprot=(int16s)(((real)damage/100.0) * hpptr[bodypart].res.poison);
	 break;
      case ELEMENT_COLD:
	 dprot=(int16s)(((real)damage/100.0) * hpptr[bodypart].res.cold);
	 break;
      case ELEMENT_ELEC:
	 dprot=(int16s)(((real)damage/100.0) * hpptr[bodypart].res.elec);
	 break;
      case ELEMENT_WATER:
	 dprot=(int16s)(((real)damage/100.0) * hpptr[bodypart].res.water);
	 break;
      case ELEMENT_ACID:
	 dprot=(int16s)(((real)damage/100.0) * hpptr[bodypart].res.acid);
	 break;

      default:
	 break;
   }

   /* armor protection */
   if(hpptr[bodypart].ac > damage) {
      damage=0;
   }
   else {
      damage-=hpptr[bodypart].ac;
   }

   /* resistance protection */
   if(dprot > damage) {
      damage=0;
   }
   else {
      damage-=dprot;
   }

//   msg.vnewmsg(C_GREEN, "AC=%d, resprot=%d.", hpptr[bodypart].ac, dprot);
//   msg.vnewmsg(C_GREEN, "final=%d.", damage);

   /* substract damage */
   hpptr[bodypart].cur-=damage;

   if(hpptr[bodypart].cur<0)
      hpptr[bodypart].cur=0;

}

/* show damage message, for player */
void damage_msg_player(int16s damage, int16s bodypart)
{
   real rhp=0;

   if(bodypart<0 || bodypart>=HPSLOT_MAX) {
      msg.newmsg("Your whole body takes damage.", C_RED);
      return;
   }

   rhp=player.hpp[bodypart].cur;

   if(damage>rhp)
      msg.vnewmsg(CH_RED, "Your %s %s very badly injured!", 
		  bodyparts[bodypart], bodypart_art[bodypart] );
   else if(damage > (rhp*0.8))
      msg.vnewmsg(C_RED, "Your %s %s severely injured!",
		  bodyparts[bodypart], bodypart_art[bodypart] );
   else if(damage > (rhp*0.5))
      msg.vnewmsg(CH_YELLOW, "Your %s %s moderately injured!",
		  bodyparts[bodypart], bodypart_art[bodypart] );
   else if(damage > (rhp*0.2))
      msg.vnewmsg(C_YELLOW, "Your %s %s slightly injured!",
		  bodyparts[bodypart], bodypart_art[bodypart] );
   else if(damage > (rhp*0.05))
      msg.vnewmsg(C_WHITE, "Your %s %s is just scratched.", 
		  bodyparts[bodypart], bodypart_art[bodypart] );
}

/* for monster */
void damage_msg_monster(_monsterlist *mptr, int16s damage, int16s bodypart)
{
   char **bpptr;
   real rhp;
   int8u gindex;

   if(bodypart<0 || bodypart>=HPSLOT_MAX) {
      if(player_has_los(c_level, mptr->x, mptr->y)) {
	 sprintf(tempstr, "%s whole body seems to be damaged.",
		     gender_art3[mptr->m.gender]);
	 tempstr[0]=toupper(tempstr[0]);
	 msg.newmsg(tempstr, C_RED);
      }
      
      return;
   }

   rhp=mptr->hpp[bodypart].cur;
   
   if(npc_races[mptr->m.race].behave & BEHV_ANIMAL)
      gindex=SEX_NEUTRAL;
   else
      gindex=mptr->m.gender;

   if(npc_races[mptr->m.race].behave & BEHV_FLYING) 
      bpptr=bodyparts_flying;
   else
      bpptr=bodyparts;

   if(damage>rhp) 
      sprintf(tempstr, "%s %s %s very badly injured!", 
	      gender_art3[gindex], bpptr[bodypart], bodypart_art[bodypart]);
   else if(damage > (rhp*0.8))
      sprintf(tempstr, "%s %s %s severely injured!", 
	      gender_art3[gindex], bpptr[bodypart], bodypart_art[bodypart]);
   else if(damage > (rhp*0.5))
      sprintf(tempstr, "%s %s %s moderately injured!", 
	      gender_art3[gindex], bpptr[bodypart], bodypart_art[bodypart]);
   else if(damage > (rhp*0.2))
      sprintf(tempstr, "%s %s %s slightly injured!",
	      gender_art3[gindex], bpptr[bodypart], bodypart_art[bodypart]);
   else if(damage > (rhp*0.1))
      sprintf(tempstr, "%s %s %s minimally damaged!", 
	      gender_art3[gindex], bpptr[bodypart], bodypart_art[bodypart]);
   else
      sprintf(tempstr, "%s %s %s not damaged!", 
	      gender_art3[gindex], bpptr[bodypart], bodypart_art[bodypart]);

   tempstr[0]=toupper(tempstr[0]);
   msg.add_dist(c_level, mptr->x, mptr->y, tempstr, C_RED, NULL, C_RED);

}

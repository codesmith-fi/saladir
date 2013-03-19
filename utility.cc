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


#include "utility.h"

void game_noticeevents(level_type *level)
{
   if((GAME_NOTIFYFLAGS & GAME_DO_REDRAW))
      do_redraw(level);
   else
      showstatus(false);

   GAME_NOTIFYFLAGS=0;
}

/* initialize stat node */
void set_stat_initial(_statpack *pstat, int16s value, int16s max, int16s min)
{
   pstat->max=max;
   pstat->min=min;
   pstat->temp=0;

//	pstat->initial+=amount;
   pstat->initial=value;

   if(pstat->initial > max)
      pstat->initial = max;
   if(pstat->initial < min)
      pstat->initial = min;
}

/* CHANGE initial stat */
void change_stat_initial(_statpack *pstat, int8s amount, bool player)
{
   if(player)
      GAME_NOTIFYFLAGS|=GAME_ATTRIBCHG;

   pstat->initial+=amount;

   if(pstat->initial > pstat->max)
      pstat->initial = pstat->max;
   if(pstat->initial < pstat->min)
      pstat->initial = pstat->min;

}

void change_stat_temp(_statpack *pstat, int8s amount, bool player)
{
   if(player)
      GAME_NOTIFYFLAGS|=GAME_ATTRIBCHG;

   pstat->temp+=amount;

   if(pstat->temp > STATMAX_GEN)
      pstat->temp = STATMAX_GEN;

   if(pstat->temp < -STATMAX_GEN)
      pstat->temp = -STATMAX_GEN;

}

void change_stat_perm(_statpack *pstat, int8s amount, bool player)
{
   if(player)
      GAME_NOTIFYFLAGS|=GAME_ATTRIBCHG;

   pstat->perm+=amount;

   if(pstat->perm > STATMAX_GEN)
      pstat->perm = STATMAX_GEN;

   if(pstat->perm < -STATMAX_GEN)
      pstat->perm = -STATMAX_GEN;

}

int16s get_stat(_statpack *pstat)
{
   int16s result=0;

   result=pstat->initial + pstat->temp + pstat->perm;

   if(result < pstat->min)
      result = pstat->min;
   else if( result > pstat->max)
      result = pstat->max;

//	if(result >=100) result=99;

   return result;
}

int16s calc_time(int16s speed, int16s time, int8u tactic)
{
   real tm;

   tm=(real)tacticeffects[tactic].spd;
   tm=(tm*time)/100;

   time=(int16u)tm;

   if(speed<100) {
      tm=time;
      tm+=time-(time/100 * speed);

      return (int16s)tm;
   }
   if(speed>100) {
      tm=time;
      tm-=time/100 * (speed-100);

      return (int16s)tm;
   }

   return time;
}

/*****************************************************
 calculate initial hitpoints from base give in basehp
 ***************************************************/
/* now takes account the fact of "missing" bodyparts */
/* as some races DON'T have some bodyparts */

void calculate_hp(_hpslot *hpack, int16s basehp, int16u race)
{
   real val;

   if(basehp==0)
      basehp=12+RANDU(13);

   /* EVERY MONSTER HAS A HEAD AND BODY */

   hpack[HPSLOT_BODY].ini=basehp;
   hpack[HPSLOT_BODY].cur=basehp;
   hpack[HPSLOT_BODY].max=basehp;

   val=basehp * HP_HEADMOD;
   hpack[HPSLOT_HEAD].ini=(int16s)val;
   hpack[HPSLOT_HEAD].cur=(int16s)val;
   hpack[HPSLOT_HEAD].max=(int16s)val;

   /* HANDS AND LEGS ARE OPTIONAL */
   if(npc_races[race].bodyparts[HPSLOT_LEFTHAND]>=0) {
      val=basehp * HP_HANDMOD;
   }
   else
      val=0;

   hpack[HPSLOT_LEFTHAND].ini=(int16s)val;
   hpack[HPSLOT_LEFTHAND].cur=(int16s)val;
   hpack[HPSLOT_LEFTHAND].max=(int16s)val;

   if(npc_races[race].bodyparts[HPSLOT_RIGHTHAND]>=0) {
      val=basehp * HP_HANDMOD;
   }
   else
      val=0;

   hpack[HPSLOT_RIGHTHAND].ini=(int16s)val;
   hpack[HPSLOT_RIGHTHAND].cur=(int16s)val;
   hpack[HPSLOT_RIGHTHAND].max=(int16s)val;

   if(npc_races[race].bodyparts[HPSLOT_LEGS]>=0) {
      val=basehp * HP_LEGSMOD;
   }
   else
      val=0;

   hpack[HPSLOT_LEGS].ini=(int16s)val;
   hpack[HPSLOT_LEGS].cur=(int16s)val;
   hpack[HPSLOT_LEGS].max=(int16s)val;

}

/* just sum all hpslots together */
int16s calculate_totalhp(_hpslot *hpack)
{
     int16s total=0;

     for(int i=0; i<HPSLOT_MAX; i++) {
	total+=hpack[i].cur;
     }

     return total;
}

/* calculate carrying limit */
int32u calc_carryweight(_statpack *stats, int32u weight)
{
   real tmp, carry;

   if(!stats)
      return 0;

   /* carrying capasity = weight * (1+(stat_STR - 50)/100) */

   tmp=(real)get_stat(&stats[STAT_STR]) - 50;
   tmp+=(real)(get_stat(&stats[STAT_TGH]) - 50)/2;
   tmp=(tmp / 100) + 1.0;

   carry=(real)weight;

   return (int32u)(carry * tmp);
}

void additembonus(_hpslot *hpslot, _Tequipslot *eqslot)
{
   if(!hpslot || !eqslot)
      return;

   if(eqslot->item == NULL)
      return;

   /* armor class (damage protection) */
   hpslot->ac += eqslot->item->i.ac;

   /* 
    * defence value, substracted directly from attack 
    * skill on melee attacks 
    */
   hpslot->dv += eqslot->item->i.dv;

   if(eqslot->item->i.status & ITEM_ENCHANTED) {
      hpslot->res.fire+=eqslot->item->i.ench.res.fire;
      hpslot->res.poison+=eqslot->item->i.ench.res.poison;
      hpslot->res.cold+=eqslot->item->i.ench.res.cold;
      hpslot->res.magic+=eqslot->item->i.ench.res.magic;
      hpslot->res.elec+=eqslot->item->i.ench.res.elec;
      hpslot->res.water+=eqslot->item->i.ench.res.water;
      hpslot->res.acid+=eqslot->item->i.ench.res.acid;
   }

}

/* 
 * calculate ALL bonuses caused by equipped items for a creature
 * or a player (if monster==NULL) 
 */
void calculate_itembonus(_monsterlist *monster)
{
   _hpslot *hpp;
   _statpack *stat;
   _Tequipslot *equip;
   Tinventory *inv;   
   int16u i, prace;

   /* first get the pointers depending on creature */
   if(monster) {
      equip=monster->equip;
      hpp=monster->hpp;
      inv=&monster->inv;
      stat=monster->stat;
      prace=monster->m.race;
   }
   else {
      equip=player.equip;
      hpp=player.hpp;
      inv=&player.inv;
      stat=player.stat;
      prace=player.prace;
   }

   /* first clear all bonuses from active bodyparts (which have hpslot) */
   for(i=0; i<HPSLOT_MAX; i++) {
      hpp[i].ac=0;

      mem_clear(&hpp[i].res, sizeof(Tresistpack));
   }

   for(i=0; i<STAT_ARRAYSIZE; i++) {
      stat[i].temp=0;
   }

   /* add stats modifiers */
   for(i=0; i<MAX_EQUIP; i++) {
      if(equip[i].item) {
	 /* check if the enchantment pack is valid */
	 if(equip[i].item->i.status & ITEM_ENCHANTED) {
	    /* add enchantment bonus */
	    stat[STAT_STR].temp+=equip[i].item->i.ench.STR;
	    stat[STAT_WIS].temp+=equip[i].item->i.ench.WIS;
	    stat[STAT_DEX].temp+=equip[i].item->i.ench.DEX;
	    stat[STAT_CON].temp+=equip[i].item->i.ench.CON;
	    stat[STAT_CHA].temp+=equip[i].item->i.ench.CHA;
	    stat[STAT_INT].temp+=equip[i].item->i.ench.INT;
	    stat[STAT_TGH].temp+=equip[i].item->i.ench.TGH;
	    stat[STAT_LUC].temp+=equip[i].item->i.ench.LUC;
	    stat[STAT_SPD].temp+=equip[i].item->i.ench.SPD;
	 }
      }    
   }

   /* recalc speed */
   stat[STAT_SPD].initial=npc_races[prace].SPD + 
      DEX_SPEED[ get_stat( &stat[STAT_DEX] ) ];
   
   /* items which affect every bodypart */
   for(i=0; i<HPSLOT_MAX; i++) {
      /* set race ac */
      hpp[i].ac=npc_races[prace].ac;

      /* set race resistances */
      hpp[i].res=npc_races[prace].res;

      additembonus(&hpp[i], &equip[EQUIP_NECK]);
      additembonus(&hpp[i], &equip[EQUIP_LRING]);
      additembonus(&hpp[i], &equip[EQUIP_RRING]);
      additembonus(&hpp[i], &equip[EQUIP_CLOAK]);

      /* weapons are considered like this */
      additembonus(&hpp[i], &equip[EQUIP_LHAND]);
      additembonus(&hpp[i], &equip[EQUIP_RHAND]);
      additembonus(&hpp[i], &equip[EQUIP_TOOL]);
   }

   /* items which affect only head */
   additembonus(&hpp[HPSLOT_HEAD], &equip[EQUIP_HEAD]);

   /* items which affect only legs */
   additembonus(&hpp[HPSLOT_LEGS], &equip[EQUIP_PANTS]);
   additembonus(&hpp[HPSLOT_LEGS], &equip[EQUIP_BOOTS]);
   additembonus(&hpp[HPSLOT_LEGS], &equip[EQUIP_LEGS]);

   /* items which affect only body */
   additembonus(&hpp[HPSLOT_BODY], &equip[EQUIP_BODY]);
   additembonus(&hpp[HPSLOT_BODY], &equip[EQUIP_SHIRT]);

   /* items which affect only left hand */
   additembonus(&hpp[HPSLOT_LEFTHAND], &equip[EQUIP_LARM]);
   additembonus(&hpp[HPSLOT_LEFTHAND], &equip[EQUIP_GLOVES]);

   /* items which affect only left hand */
   additembonus(&hpp[HPSLOT_RIGHTHAND], &equip[EQUIP_RARM]);
   additembonus(&hpp[HPSLOT_RIGHTHAND], &equip[EQUIP_GLOVES]);

}

int16s calculate_slotac(int8u bodypart, _Tequipslot *eqslots )
{
   int16s value;

   if(bodypart>HPSLOT_MAX)
      return 0;

   value=0;
   /* calculate absorb for head */
   if(bodypart==HPSLOT_HEAD) {
      if(eqslots[EQUIP_HEAD].item)
	 value+=eqslots[EQUIP_HEAD].item->i.ac;
      if(eqslots[EQUIP_CLOAK].item)
	 value+=eqslots[EQUIP_CLOAK].item->i.ac;
   }
   else if(bodypart==HPSLOT_LEFTHAND) {
      if(eqslots[EQUIP_LARM].item)
	 value+=eqslots[EQUIP_LARM].item->i.ac;
      if(eqslots[EQUIP_GLOVES].item)
	 value+=eqslots[EQUIP_GLOVES].item->i.ac;
      if(eqslots[EQUIP_SHIRT].item)
	 value+=eqslots[EQUIP_SHIRT].item->i.ac;
      if(eqslots[EQUIP_CLOAK].item)
	 value+=eqslots[EQUIP_CLOAK].item->i.ac;
      if(eqslots[EQUIP_LRING].item)
	 value+=eqslots[EQUIP_LRING].item->i.ac;

      /* shield should be tied to some skill */
      /* including other handheld armor */
      if(eqslots[EQUIP_LHAND].item)
	 value+=eqslots[EQUIP_LHAND].item->i.ac;
   }
   else if(bodypart==HPSLOT_RIGHTHAND) {
      if(eqslots[EQUIP_RARM].item)
	 value+=eqslots[EQUIP_RARM].item->i.ac;
      if(eqslots[EQUIP_GLOVES].item)
	 value+=eqslots[EQUIP_GLOVES].item->i.ac;
      if(eqslots[EQUIP_SHIRT].item)
	 value+=eqslots[EQUIP_SHIRT].item->i.ac;
      if(eqslots[EQUIP_CLOAK].item)
	 value+=eqslots[EQUIP_CLOAK].item->i.ac;
      if(eqslots[EQUIP_RRING].item)
	 value+=eqslots[EQUIP_RRING].item->i.ac;

      /* shield should be tied to some skill */
      /* including other handheld armor */
      if(eqslots[EQUIP_RHAND].item)
	 value+=eqslots[EQUIP_RHAND].item->i.ac;
   }
   else if(bodypart==HPSLOT_BODY) {
      if(eqslots[EQUIP_BODY].item)
	 value+=eqslots[EQUIP_BODY].item->i.ac;
      if(eqslots[EQUIP_SHIRT].item)
	 value+=eqslots[EQUIP_SHIRT].item->i.ac;
      if(eqslots[EQUIP_CLOAK].item)
	 value+=eqslots[EQUIP_CLOAK].item->i.ac;
   }
   else if(bodypart==HPSLOT_LEGS) {
      if(eqslots[EQUIP_LEGS].item)
	 value+=eqslots[EQUIP_LEGS].item->i.ac;
      if(eqslots[EQUIP_PANTS].item)
	 value+=eqslots[EQUIP_PANTS].item->i.ac;
      if(eqslots[EQUIP_BOOTS].item)
	 value+=eqslots[EQUIP_BOOTS].item->i.ac;
   }
//   sprintf(tempstr, "Your armor absorbs %d points", value);
//	msg.newmsg(tempstr, C_YELLOW);
   return value;
}

int16s calculate_slot_pv(int8u bodypart, _Tequipslot *eqslots )
{
   int16s value;

   if(bodypart>HPSLOT_MAX)
      return 0;

   value=0;
   if(bodypart==HPSLOT_HEAD) {
      if(eqslots[EQUIP_HEAD].item)
	 value+=eqslots[EQUIP_HEAD].item->i.dv;
      if(eqslots[EQUIP_CLOAK].item)
	 value+=eqslots[EQUIP_CLOAK].item->i.dv;
   }
   else if(bodypart==HPSLOT_LEFTHAND) {
      if(eqslots[EQUIP_LARM].item)
	 value+=eqslots[EQUIP_LARM].item->i.dv;
      if(eqslots[EQUIP_GLOVES].item)
	 value+=eqslots[EQUIP_GLOVES].item->i.dv;
      if(eqslots[EQUIP_SHIRT].item)
	 value+=eqslots[EQUIP_SHIRT].item->i.dv;
      if(eqslots[EQUIP_CLOAK].item)
	 value+=eqslots[EQUIP_CLOAK].item->i.dv;
      if(eqslots[EQUIP_LRING].item)
	 value+=eqslots[EQUIP_LRING].item->i.dv;

      /* shield should be tied to some skill */
      /* including other handheld armor */
      if(eqslots[EQUIP_LHAND].item)
	 value+=eqslots[EQUIP_LHAND].item->i.dv;
   }
   else if(bodypart==HPSLOT_RIGHTHAND) {
      if(eqslots[EQUIP_RARM].item)
	 value+=eqslots[EQUIP_RARM].item->i.dv;
      if(eqslots[EQUIP_GLOVES].item)
	 value+=eqslots[EQUIP_GLOVES].item->i.dv;
      if(eqslots[EQUIP_SHIRT].item)
	 value+=eqslots[EQUIP_SHIRT].item->i.dv;
      if(eqslots[EQUIP_CLOAK].item)
	 value+=eqslots[EQUIP_CLOAK].item->i.dv;
      if(eqslots[EQUIP_RRING].item)
	 value+=eqslots[EQUIP_RRING].item->i.dv;

      /* shield should be tied to some skill */
      /* including other handheld armor */
      if(eqslots[EQUIP_RHAND].item)
	 value+=eqslots[EQUIP_RHAND].item->i.dv;
   }
   else if(bodypart==HPSLOT_BODY) {
      if(eqslots[EQUIP_BODY].item)
	 value+=eqslots[EQUIP_BODY].item->i.dv;
      if(eqslots[EQUIP_SHIRT].item)
	 value+=eqslots[EQUIP_SHIRT].item->i.dv;
      if(eqslots[EQUIP_CLOAK].item)
	 value+=eqslots[EQUIP_CLOAK].item->i.dv;
   }
   else if(bodypart==HPSLOT_LEGS) {
      if(eqslots[EQUIP_LEGS].item)
	 value+=eqslots[EQUIP_LEGS].item->i.dv;
      if(eqslots[EQUIP_PANTS].item)
	 value+=eqslots[EQUIP_PANTS].item->i.dv;
      if(eqslots[EQUIP_BOOTS].item)
	 value+=eqslots[EQUIP_BOOTS].item->i.dv;
   }
//   sprintf(tempstr, "Your armor absorbs %d points", value);
//	msg.newmsg(tempstr, C_YELLOW);
   return value;
}

void change_alignment(int16s *align, char dir, int16s amount)
{
   int16s nal;

   nal=*align;

   if(align==&player.align)
      GAME_NOTIFYFLAGS|=GAME_ALIGNCHG;

   if(dir=='c' || dir=='C') {
      nal-=amount;
      if(nal < CHAOTIC)
	 nal=CHAOTIC;
   }
   else if(dir=='N' || dir=='n') {
      if(nal < NEUTRAL) {
	 nal+=amount;
	 if(nal > NEUTRAL) 
	    nal=NEUTRAL;
      }
      else if(nal > NEUTRAL) {
	 nal-=amount;
	 if(nal < NEUTRAL) 
	    nal=NEUTRAL;
      }

   }
   else if(dir=='l' || dir=='L') {
      nal+=amount;
      if(nal > LAWFUL)
	 nal=LAWFUL;
   }

   *align=nal;

}

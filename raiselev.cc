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

#include "raiselev.h"

int32u expneeded[MAXNUM_LEVELS+1]= { 0 };

void init_exptable(void)
{
   real exp2, exp3;

//   my_clrscr();

   expneeded[0]=150;
   for(int i=1; i<MAXNUM_LEVELS; i++) {

      exp2=(real)expneeded[i-1];
      exp2=exp2*1.60;
      expneeded[i]=(int32u)exp2;

      if(i > 10) {
	 exp3=(real)i / 50 + 1.0;
	 exp2=(real)expneeded[8];
	 exp2=exp2*exp3;
	 expneeded[i]=expneeded[i-1]+(int32u)exp2;
      }

//      my_printf("%d=%ld\t", i, expneeded[i]);
   }

   expneeded[0]=0;
   expneeded[1]=0;

//   showmore(false,false);
//   my_endwin();
//   exit(1);

}

void gain_experience(level_type *level, _monsterlist *mptr, int32u gain)
{
   if(mptr) {
      if(!level) {
	 msg.addwait("Error: Level pointer NULL when calling gain_experience().", CH_RED);
	 return;
      }
      mptr->exp+=gain;
      monster_checklevelraise(level, mptr, false);      
   }
   else {
      GAME_NOTIFYFLAGS |= GAME_EXPERCHG;
      player.exp+=gain;
      player_checklevelraise();
   }
}


void calculate_raisestats(void)
{
   int16s hpgain=0;
   int8u dicet, dices;

   dicet=npc_races[player.prace].hp_plev_dt;
   dices=npc_races[player.prace].hp_plev_ds;

   hpgain=throwdice(dicet, dices, 0);

   calculate_hp(player.hpp, player.hpp[HPSLOT_BODY].max+hpgain, player.prace);

   player.hp_max=player.hp=calculate_totalhp(player.hpp);

   player.level++;

}

bool player_checklevelraise(void)
{
//   init_exptable();

   sint raise=0;

   if(player.level > MAXNUM_LEVELS) {
      player.level=MAXNUM_LEVELS;
      return false;
   }

   for(int i=player.level+1; i<MAXNUM_LEVELS; i++) {
      if(player.exp >= expneeded[i] ) {
	 raise++;
      }
      else
	 break;
   }

   if(!raise)
      return false;

   GAME_NOTIFYFLAGS |= GAME_SHOWALLSTATS;

   while(raise>0) {
      msg.newmsg("LEVEL!", CH_GREEN);
      calculate_raisestats();
      raise--;
   }

   msg.vaddwait(CH_GREEN, "Welcome to level %d, %s!", 
	       player.level, player.name);
   
   return true;
}


bool monster_checklevelraise(level_type *level, _monsterlist *monster,
			     bool initmode)
{
//   init_exptable();

   sint raise=0;

   if(monster->m.level > MAXNUM_LEVELS) {
      monster->m.level=MAXNUM_LEVELS;
      return false;
   }

   for(int i=monster->m.level+1; i<MAXNUM_LEVELS; i++) {
      if(expneeded[i]<=monster->exp) {
	 raise++;
      }
      else
	 break;
   }

   if(!raise)
      return false;

   while(raise>0) {
      if(!initmode) {
	 monster_sprintf(nametemp, monster, true, true);
	 sprintf(tempstr, "%s suddenly looks more powerful.", nametemp);
	 msg.add_dist(level, monster->x, monster->y, tempstr, C_GREEN,
		      NULL, C_GREEN);
      }
      monster_advancelevels(monster);
      monster->m.level++;

      raise--;
   }

   return true;
}

void player_checkexpneeded(void)
{
   int32u cur;
//   init_exptable();

   cur=expneeded[player.level+1]-player.exp;

   sprintf(tempstr, "Level %d needs %ld exp points, you need %ld points!",
	   player.level+1, expneeded[player.level+1], cur );

   msg.newmsg(tempstr, C_GREEN);
}

/* advance creature ONE level up */
void monster_advancelevels(_monsterlist *mptr)
{
   int16u race;

   race=mptr->m.race;

   /* init stats */
   set_stat_initial(&mptr->stat[STAT_STR], mptr->m.STR, 
		    STATMAX_GEN, STATMIN_GEN);
   set_stat_initial(&mptr->stat[STAT_WIS], mptr->m.WIS, 
		    STATMAX_GEN, STATMIN_GEN);
   set_stat_initial(&mptr->stat[STAT_DEX], mptr->m.DEX, 
		    STATMAX_GEN, STATMIN_GEN);
   set_stat_initial(&mptr->stat[STAT_CON], mptr->m.CON, 
		    STATMAX_GEN, STATMIN_GEN);
   set_stat_initial(&mptr->stat[STAT_INT], mptr->m.INT, 
		    STATMAX_GEN, STATMIN_GEN);
   set_stat_initial(&mptr->stat[STAT_CHA], mptr->m.CHA, 
		    STATMAX_GEN, STATMIN_GEN);
   set_stat_initial(&mptr->stat[STAT_TGH], mptr->m.TGH, 
		    STATMAX_GEN, STATMIN_GEN);
   set_stat_initial(&mptr->stat[STAT_LUC], mptr->m.LUC, STATMAX_LUCK, 0);
   set_stat_initial(&mptr->stat[STAT_SPD], mptr->m.SPD, STATMAX_SPEED, 1);

   change_stat_initial(&mptr->stat[STAT_SPD], DEX_SPEED[mptr->m.DEX], false);


   /* get base stats from racelist */
//   mptr->hp_max=npc_races[mptr->m.race].hp_base;
//   mptr->sp_max=npc_races[mptr->m.race].sp_base;

   /* calculate hitpoints and spellpoints */
   mptr->base_hp+=throwdice(npc_races[mptr->m.race].hp_plev_dt, 
			   npc_races[mptr->m.race].hp_plev_ds, 0);
   mptr->sp_max+=throwdice(npc_races[mptr->m.race].sp_plev_dt, 
			   npc_races[mptr->m.race].sp_plev_ds, 0);
   mptr->sp=mptr->sp_max;

   Tskillpointer sptr;

   /* now increase skills */
   sptr=mptr->skills;
   while(sptr) {
      skill_modify_raise(&mptr->skills, sptr->s.group, sptr->s.type, 
			 throwdice(2, 100, 0), false);
      sptr=sptr->next;
   }
   
   calculate_hp(mptr->hpp, mptr->base_hp, mptr->m.race);

   mptr->hp_max=mptr->hp=calculate_totalhp(mptr->hpp);

//   mptr->m.SPD+=DEX_SPEED[mptr->m.DEX];

}


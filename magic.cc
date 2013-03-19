/**************************************************************************
 * magic.cc --                                                            *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the winter of 1998                 *
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
#include "magic.h"
#include "scrolls.h"
#include "player.h"
#include "monster.h"
#include "utility.h"
#include "inventor.h"
#include "tables.h"

bool spell_teleport(level_type *level, _monsterlist *caster, 
		    Ttarget *target, int16u skill);
bool spell_curse(level_type *level, _monsterlist *caster, 
		 Ttarget *target, int16u skill);
bool spell_uncurse(level_type *level, _monsterlist *caster, 
		 Ttarget *target, int16u skill);
bool spell_bless(level_type *level, _monsterlist *caster, 
		 Ttarget *target, int16u skill);
bool spell_confuze(level_type *level, Ttarget *target, int16u skill);
bool spell_identify(level_type *level, Ttarget *target, int16u skill);
bool spell_magicmap(level_type *level, _monsterlist *caster, int16u skill);


/*
 * If "caster"==NULL then caster is the player, else it's a monster
 *
 * "target" is a struct of type Ttarget, one field must be set to determine
 * the target of a spell.
 *
 * Every spell has the similar format of targetting
 *
 */
void player_gettarget(level_type *level, Ttarget *target, Tspell *spell)
{
   int16s tx, ty, ltx, lty;
   int16u keycode, dir;

   int32u items;
   _monsterlist *mptr=NULL;
   Tinvpointer iptr=NULL;

   bool monsterthere=false;

   ltx=tx=player.spos_x;
   ltx=ty=player.spos_y;

   mem_clear(target, sizeof(Ttarget));


   msg.add("Use movement keys for target, quit with ESC or ENTER.",
	   C_WHITE);

   while(1) {
      keycode=my_getch();

      dir=0;

      if(keycode==KEY_ESC || keycode==PADENTER 
	 || keycode==KEY_ENTER || keycode==10)
	 break;

      if(keycode==KEY_UP) dir=8;
      if(keycode==KEY_DOWN) dir=2;
      if(keycode==KEY_LEFT) dir=4;
      if(keycode==KEY_RIGHT) dir=6;
      if(keycode>='1' && keycode <='9') {
	 dir=keycode-'0';
      }
      if(keycode==' ') {
	 return;
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

	 if( !player_has_los(level, player.lreg_x+tx-1, player.lreg_y+ty-1)) {
	    tx=ltx;
	    ty=lty;
	 }

/*
	 if( !(check_visible(level, player.pos_x, player.pos_y, 
			     lreg_x+tx-1, lreg_y+ty-1)) ) {
	    tx=ltx;
	    ty=lty;
	 }
	 else {
	    if( distance( player.pos_x, player.pos_y, 
			  lreg_x+tx-1, lreg_y+ty-1 ) > player.sight) {
	       tx=ltx;
	       ty=lty;
	    }
	 }
*/

	 showplayer(level);

	 my_gotoxy(MAPWIN_RELX+tx, MAPWIN_RELY+ty);
	 my_cputch('*', CH_RED);
	 my_gotoxy(1, MSGLINE+1);
	 mptr=istheremonster(level, player.lreg_x+tx-1, player.lreg_y+ty-1);

	 items=inv_countitems(&level->inv, -1, player.lreg_x+tx-1, player.lreg_y+ty-1, false);
	 monsterthere=false;
	 if(mptr) {
	    /* set target to monster */
	    mem_clear(target, sizeof(Ttarget));

	    if(spell->other >= 0) {
	       target->x=mptr->x;
	       target->y=mptr->y;

	       monster_sprintf(nametemp, mptr, true, false );
	       sprintf(tempstr, "Target = %s.", nametemp);
	       msg.newmsg(tempstr, C_RED);
	       monsterthere=true;
	    }
	    else
	       msg.newmsg("Monster targetting not allowed.", C_RED);
	 }
	 else if(items) {
	    /* set target to item coords */
	    mem_clear(target, sizeof(Ttarget));
	    if( spell->levitem >= 0 ) {
	       target->x=player.lreg_x+tx-1;
	       target->y=player.lreg_y+ty-1;

	       if( items==1 ) {
		  iptr=giveiteminfo(level, player.lreg_x+tx-1, 
				    player.lreg_y+ty-1);
		  item_printinfo(&iptr->i, iptr->i.weight, iptr->count, 
				 "Target =");
	       }
	       else {
		  msg.newmsg("Target = a pile of items.", C_WHITE);
	       }
	    }
	    else
	       msg.newmsg("Item targetting not allowed.", C_RED);
	 }
	 else if((player.pos_x==(player.lreg_x+tx-1)) && 
		 (player.pos_y==(player.lreg_y+ty-1))) {
	    mem_clear(target, sizeof(Ttarget));
	    if(spell->self >= 0) {

	       target->x=player.lreg_x+tx-1;
	       target->y=player.lreg_y+ty-1;
	       msg.newmsg("Target = yourself.", C_WHITE);
	    }
	    else
	       msg.newmsg("Self targetting not allowed.", C_RED);
	 }
	 else {
	    sprintf(tempstr, "%s (no target).", 
		    terrains[level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].type].desc);
	    msg.newmsg(tempstr, C_WHITE);
	 }
      }

      msg.notice();
      my_gotoxy(MAPWIN_RELX+tx, MAPWIN_RELY+ty);
   }
}

/*******************************************************/
/*******************************************************/
/* SPELLS HERE                                         */
/*******************************************************/
/*******************************************************/

Tspell list_spells[]=
{
   { "heal", SPF_ALTERATION,
     4, 1, -1, -1, -1, -1
   },
   { "teleport", SPF_MYSTICISM,
     4, 1, 1, 1, -1, 1
   },
   { "identify", SPF_OBSERVATION,
     4, -1, -1, -1, 1, -1
   },
   { "bless", SPF_MYSTICISM,
     4, -1, -1, -1, 1, -1
   },
   { "curse", SPF_MYSTICISM,
     4, -1, -1, -1, 1, -1
   },
   { "remove curse", SPF_MYSTICISM,
     4, -1, -1, -1, 1, -1
   },
   { "confuze", SPF_MYSTICISM,
     4, 1, 1, -1, -1, 1
   },
   { "magic map", SPF_OBSERVATION,
     4, -1, -1, -1, -1, -1
   },

   { NULL, 0 }
};

/* players spell casting routine */
bool spell_zap(level_type *level, int16u spell, int16u skill, bool scroll)
{
   Ttarget t;
   int16s res;
   bool noneedtarget=false;

//   do_redraw(level);

   mem_clear(&t, sizeof(Ttarget));

//   player_gettarget(level, &t, list_spells[spell].flags );

   char chstr[10];

   if( (list_spells[spell].self < 0) &&
       (list_spells[spell].other < 0) &&
       (list_spells[spell].levitem < 0) &&
       (list_spells[spell].direction < 0) &&
       (list_spells[spell].invitem < 0 ))
      noneedtarget=true;

   if(!noneedtarget) {
      my_strcpy(tempstr, "Which type of target,", sizeof(tempstr));
      chstr[0]=0;
      if((skill > list_spells[spell].self && list_spells[spell].self > 0) ||
	 (skill > list_spells[spell].other && list_spells[spell].other > 0) ||
	 (skill > list_spells[spell].levitem && list_spells[spell].levitem > 0)) {
	 strcat(tempstr, " [T]arget");
	 strcat(chstr, "t");
      }
      if(skill > list_spells[spell].direction && 
	 list_spells[spell].direction > 0) {
	 strcat(tempstr, " [D]ir");
	 strcat(chstr, "d");
      }
      if(skill > list_spells[spell].invitem && 
	 list_spells[spell].invitem > 0) {
	 strcat(tempstr, " [I]nv");
	 strcat(chstr, "i");
      }

      if(strlen(chstr)>1) {
	 strcat(tempstr, "? ");

	 char data;
	 bool DONE=false;
	 while(!DONE) {
	    msg.add(tempstr, C_WHITE);

	    data=(char)my_getch();
	    data=tolower(data);

	    for(int16u i=0; i<strlen(chstr); i++) {
	       if(data == chstr[i]) {
		  chstr[0]=(char)data;
		  DONE=true;
	       }
	    }

	    if(!DONE) {
	       msg.update(true);
	       msg.add("Not a valid choice!", C_RED);
	    }
	 }      
      }

      if(chstr[0]=='d') {
	 t.direction = dir_askdir("Cast into", true);

	 if(t.direction > 9)
	    t.direction=0;
	 else {
	    t.x = player.pos_x + move_dx[t.direction];
	    t.y = player.pos_y + move_dy[t.direction];
	 }
      }
      else if(chstr[0]=='i') {
	 t.invitem=inv_listitems(&player.inv, "Target to which item?", 
				 -1, true, -1, -1);
      }
      else {
	 if((skill > list_spells[spell].self && list_spells[spell].self > 0) &&
	    (skill < list_spells[spell].other || list_spells[spell].other < 0) &&
	    (skill<list_spells[spell].levitem || list_spells[spell].levitem<0)) {
	    t.x=player.pos_x;
	    t.y=player.pos_y;
	 }
	 else
	    player_gettarget(level, &t, &list_spells[spell] );
      }


      if(t.invitem==NULL && (!t.x && !t.y) && !t.direction) {
	 msg.newmsg("No target selected.", C_YELLOW);
	 return false;
      }   
   }

   if(!scroll) {
      if((player.sp < list_spells[spell].sp)) {
	 msg.newmsg("You don't have enough mana.", C_RED);
	 return false;
      }

      /* subsctract the sp amount needed */
      player.sp -= list_spells[spell].sp;
      GAME_NOTIFYFLAGS|=GAME_HPSPCHG;

   }

   bool rescode;
     
   switch(spell) {
      case SPELL_TELEPORT:
	 rescode=spell_teleport(level, NULL, &t, skill);
	 break;
      case SPELL_CURSE:
	 rescode=spell_curse(level, NULL, &t, skill);
	 break;
      case SPELL_UNCURSE:
	 rescode=spell_uncurse(level, NULL, &t, skill);
	 break;
      case SPELL_BLESS:
	 rescode=spell_bless(level, NULL, &t, skill);
	 break;
      case SPELL_CONFUZE:
	 rescode=spell_confuze(level, &t, skill);
	 break;
      case SPELL_IDENTIFY:
	 rescode=spell_identify(level, &t, skill);
	 break;
      case SPELL_MAGICMAP:
	 rescode=spell_magicmap(level, NULL, skill);
	 break;
      default:
	 rescode=false;
	 sprintf(tempstr, "Error! Trying to zap an unknown spell %d.", spell);
	 msg.newmsg(tempstr, CH_RED);
	 break;
   }

   if(!rescode)
      msg.newmsg("Spell failed!", C_RED);

   return true;
}

void set_randomcoord(level_type *level, int16s *tx, int16s *ty) {

   while(1) {
      *tx=RANDU(level->sizex);
      *ty=RANDU(level->sizey);	    
      if(level->loc[*ty][*tx].flags & CAVE_PASSABLE)
	 break;
   }
}

/*
 * Curse, will turn item immendiately cursed!
 *
 */

bool spell_curse(level_type *level, _monsterlist *caster, 
		 Ttarget *target, int16u skill) 
{
   /* curse the item */
   if(target->invitem) {
      if(target->invitem->i.status & ITEM_BLESSED)
	 target->invitem->i.status ^= ITEM_BLESSED;

      target->invitem->i.status |= ITEM_CURSED;

      if(caster) {
	 monster_sprintf(nametemp, caster, true, true);
	 sprintf(tempstr, 
		 "The backpack of %s seems to shake violently.", 
		 nametemp);
	 msg.add_dist(level, caster->x, caster->y, tempstr, CH_DGRAY,
		      NULL, C_CYAN);
      }
      else {
	 if(target->invitem->count > 1)
	    my_strcpy(nametemp2, "s", sizeof(nametemp2));
	 else
	    nametemp2[0]=0;

	 sprintf(tempstr, "Your %s%s vibrates violently for a moment.", 
		 target->invitem->i.name, nametemp2);

	 msg.newmsg(tempstr, CH_DGRAY);
      }

   }
   
   return true;
}

/*
 * Uncurse, which actually is "Neutralise", removes cursed or blessed
 *  status 
 */
bool spell_uncurse(level_type *level, _monsterlist *caster, 
		 Ttarget *target, int16u skill) 
{
   /* uncurse/unbless the item */
   if(target->invitem) {
      if(target->invitem->i.status & ITEM_CURSED)
	 target->invitem->i.status ^= ITEM_CURSED;
      else if(target->invitem->i.status & ITEM_BLESSED)
	 target->invitem->i.status ^= ITEM_BLESSED;

      if(caster) {
	 monster_sprintf(nametemp, caster, true, true);
	 sprintf(tempstr, 
		 "The backpack of %s glows light blue for a moment.", 
		 nametemp);
	 msg.add_dist(level, caster->x, caster->y, tempstr, CH_BLUE,
		      NULL, C_CYAN);
      }
      else {
	 if(target->invitem->count > 1)
	    my_strcpy(nametemp2, "s", sizeof(nametemp2));
	 else
	    nametemp2[0]=0;

	 sprintf(tempstr, "Your %s%s glows light blue for a moment.", 
		 target->invitem->i.name, nametemp2);

	 msg.newmsg(tempstr, CH_BLUE);
      }

   }
   
   return true;

}

/*
 * Bless, will turn item towards blessed status
 *
 */
bool spell_bless(level_type *level, _monsterlist *caster, 
		 Ttarget *target, int16u skill) 
{
   /* bless the item, if cursed first remove cursing. */
   if(target->invitem) {
      if(target->invitem->i.status & ITEM_CURSED) {
	 target->invitem->i.status ^= ITEM_CURSED;
      }
      else 
	 target->invitem->i.status |= ITEM_BLESSED;

      if(caster) {
	 monster_sprintf(nametemp, caster, true, true);
	 sprintf(tempstr, 
		 "The backpack of %s glows bright white for a moment.", 
		 nametemp);

	 msg.add_dist(level, caster->x, caster->y, tempstr, CH_WHITE,
		      NULL, C_CYAN);

      }
      else {

	 if(target->invitem->count > 1)
	    my_strcpy(nametemp2, "s", sizeof(nametemp2));
	 else
	    nametemp2[0]=0;

	 sprintf(tempstr, "Your %s%s glows bright white for a moment.", 
		 target->invitem->i.name, nametemp2);
   
	 msg.newmsg(tempstr, CH_WHITE);
      }

   }
   
   return true;

}

/*
 * Teleport spell
 *
 */
bool spell_teleport(level_type *level, _monsterlist *caster,
		    Ttarget *target, int16u skill)
{
   int16s tx, ty;

   Tinvpointer iptr;
   _monsterlist *mptr=NULL;

   /* find a new coordinate from the level */
   while(1) {
      tx=RANDU(level->sizex);
      ty=RANDU(level->sizey);

      if(level->loc[ty][tx].flags & CAVE_PASSABLE)
	 break;
   }
/*
  if(caster) {
  monster_sprintf(nametemp, caster, true, true);
  sprintf(tempstr, "%s disappears.", nametemp);

  msg.add_dist(level, caster->x, caster->y, tempstr, C_CYAN,
  "You hear a distant \"Swoosh!\".", C_CYAN);
  caster->x=tx;
  caster->y=ty;

  monster_sprintf(nametemp, caster, true, true);
  sprintf(tempstr, "%s suddenly appears.", nametemp);

  msg.add_dist(level, caster->x, caster->y, tempstr, C_CYAN,
  "You hear a distant \"Zap!\".", C_CYAN);

  showplayer(level);
  return true;
  }
*/

   if(target->x==player.pos_x && target->y==player.pos_y) {
      msg.newmsg("You're displaced by strange forces.", C_GREEN);
      GAME_NOTIFYFLAGS |= GAME_DO_REDRAW;
      player.pos_x=tx;
      player.pos_y=ty;

      player.spos_x=5;
      player.spos_y=5;
      player.lreg_x=player.pos_x-4;
      player.lreg_y=player.pos_y-4;
      if(player.pos_x<5)
	 player.spos_x=player.pos_x+1;
      if(player.pos_y<5)
	 player.spos_y=player.pos_y+1;
  
      game_noticeevents(level);
//      printmaze(level, lreg_x, lreg_y);
   }
   else {
      mptr=istheremonster(level, target->x, target->y);

      if(mptr) {
	 set_randomcoord(level, &tx, &ty);

	 monster_sprintf(nametemp, mptr, true, true);	    
	 sprintf(tempstr, "%s disappears.", nametemp);

	 msg.add_dist(level, mptr->x, mptr->y, tempstr, C_CYAN,
		      "You hear a distant \"swoosh\"-sound.", C_CYAN);

	 mptr->x=tx;
	 mptr->y=ty;
	    
	 sprintf(tempstr, "%s appears in your sight.", nametemp);
	 msg.add_dist(level, mptr->x, mptr->y, tempstr, C_CYAN,
		      "You hear a distant \"Zap!\".", C_CYAN);

	 showplayer(level);
      }
      else {
	 iptr=level->inv.first;
	 while(iptr) {
	    if(iptr->x == target->x && iptr->y==target->y) {

	       set_randomcoord(level, &tx, &ty);

	       if(iptr->count > 1)
		  sprintf(tempstr, "A pile of %ss disappears.", iptr->i.name);
	       else
		  sprintf(tempstr, "A %s disappears.", iptr->i.name);

	       msg.add_dist(level, iptr->x, iptr->y, tempstr, C_CYAN,
			    "You hear a distant \"swoosh\"-sound.", C_CYAN);

	       iptr->x=tx;
	       iptr->y=ty;


	       if(iptr->count > 1)
		  sprintf(tempstr, "A pile of %ss appears in your sight.", 
			  iptr->i.name);
	       else
		  sprintf(tempstr, "A %s appears in your sight.", 
			  iptr->i.name);

	       msg.add_dist(level, iptr->x, iptr->y, tempstr, C_CYAN,
			    "You hear a distant \"Zap!\".", C_CYAN);

	       showplayer(level);
	    }
	    iptr=iptr->next;
	 }
      }
   }

   return true;
}

bool spell_confuze(level_type *level, Ttarget *target, int16u skill)
{
   _monsterlist *mptr;

   mptr=istheremonster(level, target->x, target->y);

   if(target->x == player.pos_x && target->y == player.pos_y) {

      msg.newmsg("The world spins around you.", C_CYAN);
//      player.conf += skill * 5;
      cond_add(&player.conditions, CONDIT_CONFUSED, skill * 5);

   }
   else if(mptr) {
      monster_sprintf(nametemp, mptr, true, true);
      sprintf(tempstr, "%s looks quite confuzed.", nametemp);
      
      msg.add_dist(level, mptr->x, mptr->y, tempstr, C_CYAN,
		   NULL, C_CYAN);

      cond_add(&mptr->conditions, CONDIT_CONFUSED, skill * 5);
//      mptr->conf += skill * 5;
   }
   else
      return false;

   return true;
}

bool spell_identify(level_type *level, Ttarget *target, int16u skill)
{
   if(target->invitem) {
      if(target->invitem->i.status & ITEM_IDENTIFIED)
	 return false;

      /* identify scroll */
      if(target->invitem->i.type == IS_SCROLL) {
	 list_scroll[target->invitem->i.group].flags |= SCFLAG_IDENTIFIED;
      }
      

      target->invitem->i.status |= ITEM_IDENTIFIED;

      item_printinfo(&target->invitem->i, target->invitem->i.weight,
		     target->invitem->count, "This item is actually");
   }
   else
      return false;


   return true;
}

bool spell_magicmap(level_type *level, _monsterlist *caster, int16u skill)
{
   int16s x, y;

   if(caster)
      return false;

   for(y=1; y<level->sizey-1; y++) {
      for(x=1; x<level->sizex-1; x++) {
	 if(sur_countall(level, x, y) > 0 && 
	    !(level->loc[y][x].flags & CAVE_NOLIT) )
	    level->loc[y][x].flags|=CAVE_SEEN;
      }
   }

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
   msg.newmsg("Suddenly you can remember the locations in this level.", 
	      C_WHITE);

   return true;
}

/*
 * Heal self spell
 *
 */
/*
bool spell_heal_self_minor(level_type *level, _monsterlist *caster, 
			   int16u skill)
{
   if(caster) {

      return true;
   }

   
  
   return true;
}
*/

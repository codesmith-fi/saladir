/**************************************************************************
 * weapons.cc --                                                          *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 22.04.1888                                         *
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
#include "player.h"
#include "monster.h"
#include "options.h"
#include "inventor.h"
#include "weapons.h"
#include "tables.h"
#include "dungeon.h"
#include "damage.h"
#include "inventor.h"
#include "file.h"
#include "textview.h"
#include "weapons.h"


char text_targetting[] = 
"Target with movement keys [n,t,SPACE,q/ESC or?].";


/****************************************************************
 * Function to determine a char to print the line with.         *
 ****************************************************************/
int16u decidelinechar(int16s ox, int16s oy, int16s nx, int16s ny)
{
   int16u outchar='-';

   if( (nx>ox && ny>oy) || (nx<ox && ny<oy))
      outchar='\\';
   if( (nx<ox && ny>oy) || (nx>ox && ny<oy))
      outchar='/';

   if(ny==oy)
      outchar='-';
   if(nx==ox)
      outchar='|';

   return outchar;
}

/*
 * Return 'true' if the item was removed from the inventory.
 *   meaning that the item hit something.
 *
 */
bool ranged_line(level_type *level, int16u out, int16u delay, bool single,
		 int16s x1, int16s y1, int16s x2, int16s y2,
		 bool (*hitfunc)(level_type *, _monsterlist *, 
				 int16s, int16s, bool, int16u),
		 _monsterlist *monster, int16u skill)
{
   int16s d, x, y, ax, ay, sx, sy, dx, dy, old_x, old_y;
   int16u outchar;
   bool vistat;

   sx=sy=0;

   dx = x2-x1;
   dy = y2-y1;
   sx = SIGN(dx);
   sy = SIGN(dy);
   ax = ABS(dx)<<1;
   ay = ABS(dy)<<1;

   /* get a correct line character for output */

   if(!out)
      outchar=decidelinechar(x1, y1, x2, y2);
   else
      outchar=out;

   x = x1;
   y = y1;
   if (ax>ay) {		/* x dominant */
      d = ay-(ax>>1);
      for (;;) {

//	 if( check_visible(level, player.pos_x, player.pos_y, x, y)) 
	 if(level->loc[y][x].flags & CAVE_LOSTMP)
	 {
	    vistat=true;
	    old_x=player.spos_x+MAPWIN_RELX+(x - player.pos_x);
	    old_y=player.spos_y+MAPWIN_RELY+(y - player.pos_y);

	    if(old_x > 0 && old_x < (MAPWIN_RELX+MAPWIN_SIZEX) &&
	       old_y > 0 && old_y < (MAPWIN_RELY+MAPWIN_SIZEY) ) {

	       my_gotoxy(old_x, old_y);

	       my_putch(outchar);
	       my_refresh();

	       if(delay>0)
		  my_delay(delay);
	       if(single)
		  showplayer(level);
	    }
	 }
	 else
	    vistat=false;

	 if(hitfunc!=NULL) {
	    if(hitfunc(level, monster, x, y, vistat, skill))
	       return true;
	 }

	 if (x==x2) return false;

	 old_y=y;
	 old_x=x;

	 if (d>=0) {
	    y += sy;
	    d -= ax;
	 }
	 x += sx;
	 d += ay;

	 if(!ispassable(level, x, y))
	    return false;

	 if(!out)
	    outchar=decidelinechar(old_x, old_y, x, y);
      }

   }
   else {	      	/* y dominant */
      d = ax-(ay>>1);
      for (;;) {

	 if(level->loc[y][x].flags & CAVE_LOSTMP) {
	    vistat=true;

	    old_x=player.spos_x+MAPWIN_RELX+(x - player.pos_x);
	    old_y=player.spos_y+MAPWIN_RELY+(y - player.pos_y);

	    if(old_x > 0 && old_x < (MAPWIN_RELX+MAPWIN_SIZEX) &&
	       old_y > 0 && old_y < (MAPWIN_RELY+MAPWIN_SIZEY) ) {

	       my_gotoxy(old_x, old_y);
	       my_putch(outchar);
	       my_refresh();

	       if(delay>0)
		  my_delay(delay);
	       if(single)
		  showplayer(level);
	    }
	 }
	 else
	    vistat=false;

	 if(hitfunc!=NULL) {
	    if(hitfunc(level, monster, x, y, vistat, skill))
	       return true;
	 }

	 if (y==y2) return false;

	 old_y=y;
	 old_x=x;

	 if (d>=0) {
	    x += sx;
	    d -= ay;
	 }
	 y += sy;
	 d += ax;

	 if(!ispassable(level, x, y))
	    return false;

	 if(!out)
	    outchar=decidelinechar(old_x, old_y, x, y);
      }
   }

}

int16s target_nearest(level_type *level, int16s *cx, int16s *cy, 
		      int16s lastidx)
{
   _monsterlist *mptr, *nearest=NULL;

   int16s i;

   mptr=level->monsters;
   i=0;
   while(mptr) {
      if(i>lastidx) {
/*
	 if(!nearest) {
	    if(level->loc[mptr->y][mptr->x].flags & CAVE_LOSTMP)
	       nearest=mptr;
	 }
	 else if(level->loc[mptr->y][mptr->x].flags & CAVE_LOSTMP) {
//	    ndist=distance(nearest->x, nearest->y, player.pos_x, player.pos_y);
//	    cdist=distance(mptr->x, mptr->y, player.pos_x, player.pos_y);
	 
//	    if(cdist < cdist)
	    nearest=mptr;

	 }
*/
	 if(level->loc[mptr->y][mptr->x].flags & CAVE_LOSTMP) {
	    nearest=mptr;
	    break;
	 }

      }	 
      i++;
      mptr=mptr->next;
   }

   if(nearest) {
      *cx=nearest->x;
      *cy=nearest->y;
      return i;
   }

   return -1;
}


bool ranged_gettarget(level_type *level, int16s *scr_x, int16s *scr_y,
		      int16s *lev_x, int16s *lev_y)
{
   int16s tx, ty, ltx, lty;
   int16s lasttidx=-1;
   int16u keycode, dir;
   int32u items;
   _monsterlist *mptr=NULL;
   Tinvpointer iptr=NULL;

   bool monsterthere=false;

   /* get first target in sight */
   lasttidx=target_nearest(level, &tx, &ty, lasttidx);

   if(lasttidx>=0) {
      ltx=tx;
      lty=ty;
   }
   else {
      ltx=tx=player.pos_x;
      ltx=ty=player.pos_y;
   }

   *scr_x=MAPWIN_RELX+player.spos_x+tx-player.pos_x;
   *scr_y=MAPWIN_RELY+player.spos_y+ty-player.pos_y;
   *lev_x=tx;
   *lev_y=ty;
   
   msg.add(text_targetting, C_WHITE);

   keycode='5';
   while(1) {
      dir=5;
      if(keycode==KEY_ESC || keycode=='q' || keycode=='Q')
	 return false;
      else if(keycode=='n' || keycode=='N') {
	 lasttidx=target_nearest(level, &tx, &ty, lasttidx);
	 if(lasttidx<0)
	    lasttidx=target_nearest(level, &tx, &ty, lasttidx);
      }
      else if(keycode==KEY_UP) 
	 dir=8;
      else if(keycode==KEY_DOWN) 
	 dir=2;
      else if(keycode==KEY_LEFT) 
	 dir=4;
      else if(keycode==KEY_RIGHT) 
	 dir=6;
      else if(keycode>='1' && keycode <='9') {
	 dir=keycode-'0';
      }
      else if(keycode=='t' || keycode=='T' || keycode==' ' || 
	      keycode==PADENTER || keycode==KEY_ENTER || keycode==10 ) {
	 return true;
      }
      else if(keycode=='?' || keycode=='h' || keycode=='H') {
	   if( chdir_tohelpdir() ) {
		viewfile("missile.txt");
		do_redraw(level);
	   }
	   else
		msg.add("Can't display missile/ranged help", CH_RED);
	   msg.add(text_targetting, C_WHITE);
      }

      if(dir) {
	 /* last coordinates */
	 ltx=tx;
	 lty=ty;

	 /* make new coordinates */
	 tx+=move_dx[dir];
	 ty+=move_dy[dir];

	 /* do not allow beyond map window */
	 if((player.spos_x+tx-player.pos_x)<1) tx=ltx;
	 if((player.spos_y+ty-player.pos_y)<1) ty=lty;
	 if((player.spos_x+tx-player.pos_x)>MAPWIN_SIZEX) tx=ltx;
	 if((player.spos_y+ty-player.pos_y)>MAPWIN_SIZEY) ty=lty;

	 /* allow targetting only on visible areas */
	 if(!player_has_los(level, tx, ty)) {
	    tx=ltx;
	    ty=lty;
	 }


	 *scr_x=MAPWIN_RELX+player.spos_x+tx-player.pos_x;
	 *scr_y=MAPWIN_RELY+player.spos_y+ty-player.pos_y;
	 *lev_x=tx;
	 *lev_y=ty;

	 showplayer(level);

	 if(CONFIGVARS.targetline) {
	    ranged_line(level, 0, 0, false, 
			player.pos_x, 
			player.pos_y, 
			tx, ty, NULL, NULL, 0);
	 }
	 my_gotoxy(MAPWIN_RELX+player.spos_x+(tx-player.pos_x), 
		   MAPWIN_RELY+player.spos_y+(ty-player.pos_y));
	 my_cputch('X', CH_RED);
	 my_gotoxy(1, MSGLINE+1);
	 mptr=istheremonster(level, tx, ty);

	 items=inv_countitems(&level->inv, -1, tx, ty, false);
	 monsterthere=false;
	 if(mptr) {
	    monster_sprintf(nametemp, mptr, true, false );
	    msg.vnewmsg(C_RED, "Target = %s.", nametemp);
	    monsterthere=true;
	 }
	 else if(items) {
	    /* set target to item coords */
	    if( items==1 ) {
	       iptr=giveiteminfo(level, tx, ty);
	       item_printinfo(&iptr->i, iptr->i.weight, iptr->count, 
			      "Target =");
	    }
	    else {
	       msg.newmsg("Target = a pile of items.", C_WHITE);
	    }
	 }
	 else if((player.pos_x==tx) && 
		 (player.pos_y==ty)) {
	    msg.newmsg("Target = yourself.", C_WHITE);
	 }
	 else {
	    msg.vnewmsg(C_WHITE, "%s (no target).", 
			terrains[level->loc[ty][tx].type].desc);
	 }
      }

      msg.notice();
      keycode=my_getch();
      msg.update(true);
      my_gotoxy(MAPWIN_RELX+player.spos_x+(tx-player.pos_x), 
		MAPWIN_RELY+player.spos_y+(ty-player.pos_y));

   }
   msg.update(true);
}

bool OOOranged_gettarget(level_type *level, int16s *scr_x, int16s *scr_y,
		      int16s *lev_x, int16s *lev_y)
{
   int16s tx, ty, ltx, lty;
   int16u keycode, dir;

   int32u items;
   _monsterlist *mptr=NULL;
   Tinvpointer iptr=NULL;

   bool monsterthere=false;

/*
   if(target_nearest(level, &tx, &ty)) {

   }
   else {
      ltx=tx=player.spos_x;
      ltx=ty=player.spos_y;
   }
*/

   ltx=tx=player.spos_x;
   ltx=ty=player.spos_y;
   *scr_x=MAPWIN_RELX+tx;
   *scr_y=MAPWIN_RELY+ty;
   *lev_x=player.lreg_x+tx-1;
   *lev_y=player.lreg_y+ty-1;

   
   msg.add("Use movement keys for target, quit with ESC or ENTER.",
	   C_WHITE);

   while(1) {
      keycode=my_getch();

      dir=0;

      if(keycode==KEY_ESC || keycode=='q' || keycode=='Q')
	 return false;

      if(keycode==KEY_UP) dir=8;
      if(keycode==KEY_DOWN) dir=2;
      if(keycode==KEY_LEFT) dir=4;
      if(keycode==KEY_RIGHT) dir=6;
      if(keycode>='1' && keycode <='9') {
	 dir=keycode-'0';
      }
      if(keycode==' ' || keycode==PADENTER || keycode==KEY_ENTER 
	 || keycode==10 ) {
	 return true;
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

	 /* allow targetting only on visible areas */
	 if(!(level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].flags & CAVE_LOSTMP)) {
	    tx=ltx;
	    ty=lty;
	 }

	 *scr_x=MAPWIN_RELX+tx;
	 *scr_y=MAPWIN_RELY+ty;
	 *lev_x=player.lreg_x+tx-1;
	 *lev_y=player.lreg_y+ty-1;

	 showplayer(level);

	 if(CONFIGVARS.targetline) {
	    ranged_line(level, 0, 0, false, 
			player.pos_x, 
			player.pos_y, 
			player.lreg_x+tx-1, 
		        player.lreg_y+ty-1, NULL, NULL, 0);
	 }
	 my_gotoxy(MAPWIN_RELX+tx, MAPWIN_RELY+ty);
	 my_cputch('X', CH_RED);
	 my_gotoxy(1, MSGLINE+1);
	 mptr=istheremonster(level, player.lreg_x+tx-1, player.lreg_y+ty-1);

	 items=inv_countitems(&level->inv, -1, player.lreg_x+tx-1, player.lreg_y+ty-1, false);
	 monsterthere=false;
	 if(mptr) {
	    monster_sprintf(nametemp, mptr, true, false );
	    msg.vnewmsg(C_RED, "Target = %s.", nametemp);
	    monsterthere=true;
	 }
	 else if(items) {
	    /* set target to item coords */
	    if( items==1 ) {
	       iptr=giveiteminfo(level, player.lreg_x+tx-1, player.lreg_y+ty-1);
	       item_printinfo(&iptr->i, iptr->i.weight, iptr->count, 
			      "Target =");
	    }
	    else {
	       msg.newmsg("Target = a pile of items.", C_WHITE);
	    }
	 }
	 else if((player.pos_x==(player.lreg_x+tx-1)) && 
		 (player.pos_y==(player.lreg_y+ty-1))) {
	    msg.newmsg("Target = yourself.", C_WHITE);
	 }
	 else {
	    msg.vnewmsg(C_WHITE, "%s (no target).", 
	       	terrains[level->loc[player.lreg_y+ty-1][player.lreg_x+tx-1].type].desc);
	 }
      }

      msg.notice();
      my_gotoxy(MAPWIN_RELX+tx, MAPWIN_RELY+ty);
   }
   msg.update(true);
}

/*
 * Function which is called when a player throws a missile item,
 * ie. with a bow or with hands. This function is called for every
 * grid location the missile passes.
 *
 * The item used for throw (equip[EQUIP_MISSILE]) should NOT be removed
 * from the player's inventory
 * until the missile throwing sequence has ended. 
 * This function needs that item.
 *
 * Remove the throw item at the end of this (hit) function.
 *
 * Return 'true' if the item hit something and was removed.
 *               in this case the ranged line function will END.
 * Return 'false' if not.
 *               in this case the ranged line will continue.
 *
 */
bool ranged_checkhit_player(level_type *level, _monsterlist *monster,
			    int16s x, int16s y, bool visible, int16u skill)
{
   _monsterlist *mptr;
   Tinventory *inv;
   int16s *bonptr;
   Tskillpointer *skills;
   _Tequipslot *equip;
   int16s damage;
   bool playerhere=false;
   int16u bodyp=0;
   int16u trace=0, i, j;

   /* an array for target bodyparts */
   int8s bparts[HPSLOT_MAX+1];

   if(x==player.pos_x && y==player.pos_y && monster)
      playerhere=true;

   if(monster)
      if(x==monster->x && y==monster->y)
	 return false;
   
   /* is there a monster? */
   mptr=istheremonster(level, x, y);

   /* no, no action required */
   if(!mptr && !playerhere)
      return false;

   /* yes, we did */

   if(mptr)
      trace=mptr->m.race;
   else if(playerhere)
      trace=player.prace;


   /* collect target bodyparts which the race has */
   for(j=0, i=0; i<HPSLOT_MAX; i++) {
      if(npc_races[trace].bodyparts[i]>=0) {
	 bparts[j++]=i;
      }
   }

   /* now get a random target bodypart */
   bodyp=bparts[RANDU(j)];
		    
   /* if the player is acting */
   if(!monster) {
      skills=&player.skills;
      equip=player.equip;
      inv=&player.inv;
      bonptr=&player.attackbonus;

      if(mptr) {
	 monster_sprintf(nametemp, mptr, false, true);
	 sprintf(tempstr, "Your %s hits %s!", 
		 equip[EQUIP_MISSILE].item->i.name, 
		 nametemp);
      }
      else if(playerhere) {
	 sprintf(tempstr, "You shoot your %s!", 
		 bodyparts[bodyp]);
      }
   }
   else {
      equip=monster->equip;
      skills=&monster->skills;
      inv=&monster->inv;
      bonptr=&monster->attackbonus;

      if(mptr) {
	 monster_sprintf(nametemp, mptr, false, true);
	 sprintf(tempstr, "%s hits %s!", 
		 equip[EQUIP_MISSILE].item->i.name, 
		 nametemp);
      }
      else if(playerhere) {
	 sprintf(tempstr, "%s hits your %s!", 
		 equip[EQUIP_MISSILE].item->i.name, 
		 bodyparts[bodyp]);
	 
      }
      tempstr[0]=toupper(tempstr[0]);
   }
   
   /* first check against the skill if we hit */
   if(throwdice(1, 100, 0) > skill) {
      if(!mptr) 
	 msg.vadd(C_WHITE, "An %s misses your %s.", 
		  equip[EQUIP_MISSILE].item->i.name, 
		  bodyparts[bodyp]);

      return false;
   }
   /* we hit! */
   damage=throwdice(equip[EQUIP_MISSILE].item->i.missi_dt,
		    equip[EQUIP_MISSILE].item->i.missi_ds, 
		    equip[EQUIP_MISSILE].item->i.missi_ds + *bonptr);

   /* issue damage to target */
   if(mptr) {
      if(visible)
	 msg.add(tempstr, C_WHITE);

//      if(monster_takedamage(level, mptr, monster, damage, bodyp )<=0) {
      if(damage_issue(level, mptr, monster, ELEMENT_NOTHING, damage, 
		      bodyp, NULL) <= 0) {
	 if(monster)
	    monster_gainedexperience(level, monster, mptr);
	 else
	    player_killedmonster(mptr);
      }
           
   }
   else if(monster) {      
      if(damage_issue(level, NULL, monster, ELEMENT_NOTHING, damage, 
		      bodyp, NULL) <= 0) {
	 monster_gainedexperience(level, monster, NULL);
      }
//      if(player_getmeleehit(level, tempstr, damage, bodyp, monster) <= 0)
//	 monster_gainedexperience(level, monster, NULL);
   }

   /* drop the item to the ground near target */
   inv_transfer2inv(level, inv, equip, &level->inv, 
		    equip[EQUIP_MISSILE].item, 1, x, y);

   return true;
}

/*
 * Function equips an item of given type and group 
 * (ie, IS_MISSILE, WPN_MISSILE_1) into the missile equip slot
 *
 */
bool ready_newmissile(Tinventory *inv, _Tequipslot *equip, 
		      int16u type, int16u group)
{
   Tinvpointer iptr, invnode;

   if(!inv)
      return false;

   iptr=inv->first;

   while(iptr) {
      if(type==iptr->i.type && group==iptr->i.group) {

	 /* split a piles */
	 invnode = inv_splitpile(inv, iptr);
	 invnode->slot = EQUIP_MISSILE;

	 equip[EQUIP_MISSILE].in_use=1;
	 equip[EQUIP_MISSILE].item=invnode;

	 return true;
      }

      iptr=iptr->next;
   }

   return false;
}

/* 
 * Return a skill value for current ranged weapon in use
 *
 */
bool get_rangedskill(_monsterlist *monster, int16u *group, int16u *skill)
{
   Tskillpointer sptr;
   Tinventory *inv;
   _Tequipslot *equip;
   int16s *bonptr;

   int16s mistype=-1;
   int16s wpntype=-1;
   int16s bonus;

   if(monster) {
      equip=monster->equip;
      bonptr=&monster->attackbonus;
      sptr=monster->skills;
      inv=&monster->inv;
   }
   else {
      equip=player.equip;
      bonptr=&player.attackbonus;
      sptr=player.skills;
      inv=&player.inv;
   }

   bonus=0;

   /* first check if there is something to throw/shoot */
   if(!equip[EQUIP_MISSILE].item) {
      if(!monster)
	 msg.newmsg("You don't have any missiles readied!", C_WHITE);

      return false; /* nothing */
   }

   /* do we have a missile weapon equipped, get it's type to wpntype */
   if(equip[EQUIP_RHAND].item)
      if(equip[EQUIP_RHAND].item->i.type == IS_MISWEAPON) {
	 wpntype=equip[EQUIP_RHAND].item->i.group;
	 bonus=equip[EQUIP_RHAND].item->i.misdam_mod;
      }

   if(equip[EQUIP_LHAND].item)
      if(equip[EQUIP_LHAND].item->i.type == IS_MISWEAPON) {
	 wpntype=equip[EQUIP_LHAND].item->i.group;
	 bonus=equip[EQUIP_LHAND].item->i.misdam_mod;
      }

   /* then check if the item is an arrow or a bolt (IS_MISSILE)*/
   if(equip[EQUIP_MISSILE].item->i.type == IS_MISSILE) 
      mistype=equip[EQUIP_MISSILE].item->i.group;

   /* if bow then needs a missile type 1 */
   if(wpntype==WPN_BOW && mistype==WPN_MISSILE_1) {
      *skill=SKILL_BOW;
      *group=SKILLGRP_WEAPON;

      *bonptr=bonus;

      return true;

   }
   /* if crossbow, needs a missile type 2 */
   else if(wpntype==WPN_CROSSBOW && mistype==WPN_MISSILE_2) {
      *skill=SKILL_CROSSBOW;
      *group=SKILLGRP_WEAPON;

      *bonptr=bonus;

      return true;

   }
   /* else, throw the missile */
   else {
      *skill=SKILL_THROW;
      *group=SKILLGRP_WEAPON;

      *bonptr=-2;

      return true;

   }

   return false;

}

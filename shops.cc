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

#include "shops.h"
#include "pathfind.h"
#include "pathfunc.h"
#include "generate.h"

#define SHOPNAME_ARMOURNUM 3
#define SHOPNAME_WEAPONNUM 4
#define SHOPNAME_GENNUM 5
#define SHOPNAME_FOODNUM 5

char *shopname_armour[]=
{
     "Armour Emporium",
     "Protectionware",
     "Fine Armour",
     NULL
};

char *shopname_weapon[]=
{
     "Weapon Emporium",
     "Blades'n Bows",
     "Sharp Blades",
     "Blades'n Daggers",
     NULL
};

char *shopname_general[]=
{
     "Shop O'Goodies",
     "General Store",
     "Used Goodies",
     "Worthy Equipment",
     "Abandoned Wares",
     NULL
};

char *shopname_food[]=
{
     "Food Emporium",
     "Tasty Snackbite",
     "Hack'n Snack",
     "Rations of Life",
     "Eat or Die",
     NULL
};

bool shop_addshopkeeper(level_type *level, int8u roomnum)
{
     _monsterlist *newptr;
     _monsterdef *keeptr;

     int16u rrace;

     /* create a new node */
     newptr=monster_initnode(level);

     // bail out if memory is out
     if(!newptr) {
	  msg.newmsg("Could not create shopkeeper!", CHB_RED);
	  return false;
     }

     /* get a random shopkeeper template */
     keeptr=shopkeeper_list+RANDU(num_shopkeepers);

     /* copy data */
     newptr->m=*keeptr;

     /* get a random race */
     while(1) {
	  rrace=RANDU(num_npcraces);

	  /* no chaotic or animal shopkeepers! :) */
	  if(npc_races[rrace].align!=CHAOTIC && !(npc_races[rrace].behave & BEHV_ANIMAL))
	       break;
     }

     newptr->m.race=rrace;

     random_name(newptr->m.name, CNAME_MAX-1);
     newptr->m.name[0]=toupper(newptr->m.name[0]);

     /* set movement limits */
     newptr->rev_x1=level->rooms[roomnum].x1;
     newptr->rev_x2=level->rooms[roomnum].x2;
     newptr->rev_y1=level->rooms[roomnum].y1;
     newptr->rev_y2=level->rooms[roomnum].y2;

     newptr->sindex=0;

//   level->rooms[roomnum].sellp=100;
//   level->rooms[roomnum].buyp=75;

     level->rooms[roomnum].owner=newptr;
     newptr->roomnum=roomnum;

     /* set initial coordinates to the room in case */
//     newptr->x=1+level->rooms[roomnum].x1+RANDU(level->rooms[roomnum].x2-level->rooms[roomnum].x1-1);
//     newptr->y=1+level->rooms[roomnum].y1+RANDU(level->rooms[roomnum].y2-level->rooms[roomnum].y1-1);

     newptr->x=level->rooms[roomnum].x1+2;
     newptr->y=level->rooms[roomnum].y1+2;

     /* act as shopkeeper */
     newptr->m.status=MST_SHOPKEEPER;

     /* advance to level */
     newptr->exp=expneeded[newptr->m.level]+1;
     newptr->m.level=0;

     newptr->base_hp=npc_races[newptr->m.race].hp_base;
     newptr->sp_max=npc_races[newptr->m.race].sp_base;

     monster_checklevelraise(level, newptr, true);

     /* init class */
     if(classes[newptr->m.mclass].initfunc != NULL) {
	classes[newptr->m.mclass].initfunc(&newptr->skills, &newptr->inv,
					  newptr->stat, newptr->hpp);
     }

     /* add initial money purse */
     inv_additem(&newptr->inv, IS_MONEY, MONEY_GOLD,
		 4000+RANDU(4000), -1);

     /* use items */
     monster_useitems(level, newptr);

     return true;
}

/* initializes all shops in level! */
/* shoptype must be set before, when the room is created */
void shop_init(level_type *level)
{
     int16u i;

     /* scan the roomlist for shops */
     for(i=0; i<level->roomcount; i++) {
	  if(level->rooms[i].type==ROOM_SHOP) {
	       shop_addshopkeeper(level, i);

	       shop_equip(level, i);
	  }
     }
}

/* equip shops with items */
void shop_equip(level_type *level, int8u roomnum)
{
   Tinvpointer iptr=NULL;
   int32s ritem;
   int16u itype;

   real price, pr2;

   int8u shoptype;

   if(!level)
      return;

   if(level->rooms[roomnum].shoptype==-1) {
      shoptype=RANDU(100);

      if(shoptype<20)
	 shoptype=SHOPTYPE_ARMOUR;
      else if(shoptype<40)
	 shoptype=SHOPTYPE_WEAPON;
      else if(shoptype<70)
	 shoptype=SHOPTYPE_FOOD;
      else
	 shoptype=SHOPTYPE_GENERAL;

      level->rooms[roomnum].shoptype=shoptype;
   }

   shoptype=level->rooms[roomnum].shoptype;

   if(shoptype==SHOPTYPE_ARMOUR) {
      level->rooms[roomnum].shopname=RANDU(SHOPNAME_ARMOURNUM);
      level->rooms[roomnum].sellp=110;
      level->rooms[roomnum].buyp=70;
   }
   else if(shoptype==SHOPTYPE_WEAPON) {
      level->rooms[roomnum].shopname=RANDU(SHOPNAME_WEAPONNUM);
      level->rooms[roomnum].sellp=100;
      level->rooms[roomnum].buyp=75;
   }
   else if(shoptype==SHOPTYPE_FOOD) {
      level->rooms[roomnum].shopname=RANDU(SHOPNAME_FOODNUM);
      level->rooms[roomnum].sellp=100;
      level->rooms[roomnum].buyp=30;
   }
   else {
      level->rooms[roomnum].shopname=RANDU(SHOPNAME_GENNUM);

      level->rooms[roomnum].sellp=100;
      level->rooms[roomnum].buyp=50;
   }
   level->rooms[roomnum].flags=0;

   for(int16s j=level->rooms[roomnum].y1; j<level->rooms[roomnum].y2; j++) {
      for(int16s i=level->rooms[roomnum].x1; i<level->rooms[roomnum].x2; i++) {
	 if(ispassable(level, i, j)) {
	    /* food shop */
	    if(shoptype==SHOPTYPE_FOOD) {
	       ritem = get_randomitem(IS_FOOD);
	       if(ritem>=0 && ritem<num_miscitems) {
		  iptr=level_createitem(level, i, j, ISMG_MISCITEM, 
					(int16s)ritem, 1, -1);
	       }
	    }
	    else if(shoptype==SHOPTYPE_WEAPON) {
	       itype=RANDU(100);
	       if(itype < 90)
		  iptr=level_createitem(level, i, j, IS_WEAPON1H, -1, 1, -1);
	    }
	    else if(shoptype==SHOPTYPE_ARMOUR) {
	       iptr=level_createitem(level, i, j, IS_ARMOR, -1, 1, -1);
	    }
	    /* general shop items */
	    else {
	       itype=RANDU(100);	       
	       if(itype<10)
		  iptr=level_createitem(level, i, j, IS_SCROLL, -1, 1, -1);
	       else if(itype<25)
		  iptr=level_createitem(level, i, j, ISMG_MISCITEM, -1, 1, -1);
	       else if(itype < 60)
		  iptr=level_createitem(level, i, j, IS_WEAPON1H, -1, 1, -1);
	       else if(itype < 90)
		  iptr=level_createitem(level, i, j, IS_ARMOR, -1, 1, -1);
	    }

	    if(iptr) {
	       if(iptr->i.type != IS_FOOD) {
		  pr2=(real)level->rooms[roomnum].sellp;
		  pr2=pr2/100;
		  price=(real)iptr->i.price;
		  price=price * materials[iptr->i.material].vmod * pr2;
		  iptr->i.price=(int32u)price;
		  /* make the item unpaid */
	       }
	       iptr->i.status|=ITEM_UNPAID;
	    }
	 }
      }
   }
}

/* calculate price from COPPER
  print a string: ie. "3g-2s-13c" or "12g"
  */
void shopkeeper_coppervalue(int32u copper, int32u *pgold, 
			    int32u *psilver, int32u *pcopper)
{
//     real rmod;
     int32u c1, c2;

     c1=0;
     c2=0;

/*
     rmod=(real)((1.0)/valuables[MONEY_COPPER].value);	// 8

     c1=(int32u)(copper/rmod);

     copper-=(int32u)(c1*rmod);

     if(copper > 0) {
	  rmod=(real)(1.0/valuables[MONEY_SILVER].value);	// 4
	  c2=(int32u)(copper/rmod);

	  copper-=(int32u)(c2*rmod);
     }
*/

     c1=copper/valuables[MONEY_GOLD].value;
     copper-=c1*valuables[MONEY_GOLD].value;

     if(copper > 0) {
//	  rmod=(real)(1.0/valuables[MONEY_SILVER].value);	// 4
	  c2=( copper/valuables[MONEY_SILVER].value );
	  copper-=c2*valuables[MONEY_SILVER].value;
     }
    
     *pgold=c1;
     *psilver=c2;
     *pcopper=copper;
}

/* clean the room owners, shopkeepers mainly */
/* go trough the room array and remove owner pointer 'remove' */
void shopkeeper_clean(level_type *level, _monsterlist *remove)
{
   Tinvpointer iptr;
   int32u mask;

   mask=0xffffffff ^ ITEM_UNPAID;

   /* mark all items inside the owners room PAID */
   iptr=level->inv.first;
   while(iptr) {
      if( (iptr->x > level->rooms[remove->roomnum].x1 &&
	 iptr->x < level->rooms[remove->roomnum].x2) &&
	 (iptr->y > level->rooms[remove->roomnum].y1 &&
	 iptr->y < level->rooms[remove->roomnum].y2) )
	 iptr->i.status &= mask;

      iptr=iptr->next;
   }

   /* remove room owner */
   level->rooms[remove->roomnum].owner=NULL;
}

#define BILLMSG_NUM 6
char *keep_billmsg[]=
{
     "curses: \"There're so much thieves around these days.\".",
     "says: \"I hope you're not going to steal my stuff.\".",
     "warns: \"Watch your fingers...\".",
     "says: \"I'm here, just in case...\".",
     "notes: \"You can't believe how sharp my new sword is!\".",
     "curses: \"The local police just can't keep the thieves away.\".",
     NULL
};

#define NOBILLMSG_NUM 7
char *keep_nobillmsg[]=
{
     "mumbles: \"These days you just can't be too carefull!\".",
     "curses: \"There're times when I want to sell this shop.\".",
     "says: \"Adventurers life is so easy..\".",
     "says: \"Adventurers need not to worry about thieves.\".",
     "curses: \"There just are no honest people around these days.\".",
     "asks: \"Have you seen the sword stolen from me a while ago?\".",
     "curses: \"Someone stole my wand of thief genociding.\".",
     NULL
};

#define NOSEEMSG_NUM 3
char *keep_noseemsg[]=
{
     "You hear someone cursing shoplifters.",
     "Someone curses.",
     "You hear from distance: \"Curse, curse and #&%#!\".",
     NULL
};

#define GETOUTMSG_NUM 4
char *keep_throwmsg[]=
{
     "%s yells: \"GET OUT OF MY SHOP YOU LOWLIFE SCUM!\".",
     "%s screams: \"...AND STAY AWAY FROM MY SHOP!\".",
     "%s yells: \"...AND DON'T COME BACK THIEF!\".",
     "%s swears: \"#&#! NEXT TIME YOU'RE HISTORY!\".",
     NULL,
};

bool shopkeeper_buy(level_type *level, _monsterlist *keeper, Tinvpointer item)
{
     real copper, mod, matmod;
     int32u gc, sc, cc;
     bool notaccept;

     if(!keeper) {
	  msg.newmsg("There's nobody to pay you for that!\n", C_WHITE);
	  return false;
     }
/*
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
	  msg.newmsg("Shopkeeper says: \"Please come closer so I can see the stuff you're selling!\".", C_YELLOW);
	  return false;
     }
*/

     if(item->i.type == IS_MONEY) {
	  sprintf(tempstr, 
		  "%s laughs: \"Like you want to sell your money right?\".", 
		  keeper->m.name);
	  msg.newmsg(tempstr, C_MAGENTA);
	  return false;
     }
     if(item->i.type == IS_FOOD && item->i.group == FOOD_CORPSE) {
	  sprintf(tempstr, 
		  "%s grins: \"Yuck! I don't have so sick needs...\".", 
		  keeper->m.name);
	  msg.newmsg(tempstr, C_MAGENTA);
	  sprintf(tempstr, "%s vomits.", keeper->m.name);
	  msg.newmsg(tempstr, C_WHITE);
	  return false;
     }

     notaccept=false;

     if(level->rooms[keeper->roomnum].shoptype==SHOPTYPE_ARMOUR) {
	if( !item_isarmor(&item->i))
	   notaccept=true;
     }

     if( level->rooms[keeper->roomnum].shoptype==SHOPTYPE_WEAPON) {
	if( !item_isweapon(&item->i) )
	   notaccept=true;
     }
     if( level->rooms[keeper->roomnum].shoptype==SHOPTYPE_FOOD) {
	if( item->i.type != IS_FOOD )
	   notaccept=true;
     }
     if(level->rooms[keeper->roomnum].shoptype==SHOPTYPE_GENERAL)
	  notaccept=false;

     if(notaccept) {
	  sprintf(tempstr, 
		  "\"Take your stuff elsewhere. We don't buy that.\", %s says.", 
		  keeper->m.name);
	  msg.newmsg(tempstr, C_MAGENTA);
	  return false;
     }

     if(item->i.type!=IS_FOOD)
	  matmod=materials[item->i.material].vmod;
     else matmod=1.0;

     copper=(real)item->i.price * item->count;
     mod=(real)level->rooms[keeper->roomnum].buyp;
     mod=mod/100;
     copper=copper * mod * matmod;

     shopkeeper_coppervalue((int32u)copper, &gc, &sc, &cc);
     sprintf(nametemp, "%ldg, %lds, %ldc", gc, sc, cc);

     if(!inv_paymoney(&keeper->inv, &player.inv, keeper->equip, 
		      (int32u)copper, true) ) {
	  if(item->count==1) {
	       sprintf(tempstr, 
		       "\"Sorry, I can't afford to pay for that\", %s says.", 
		       keeper->m.name);
	  }
	  else {
	       sprintf(tempstr, 
		       "\"Sorry, I can't afford to pay for those\", %s says.",
		       keeper->m.name);
	  }
	  msg.newmsg(tempstr, C_MAGENTA);
	  return false;
     }

     if(item->count==1) {
	  sprintf( tempstr, 
		   "%s asks: \"Do you want to sell that %s for %s?\"", 
		   keeper->m.name, item->i.name, nametemp);
     }
     else {
	  sprintf( tempstr, 
		   "%s asks: \"Do you want to sell those %s's for %s?\"", 
		   keeper->m.name, item->i.name, nametemp );
     }
     if(confirm_yn(tempstr, false, true)) {
	  if( inv_paymoney(&keeper->inv, &player.inv, keeper->equip, (int32u)copper, false) )
	       msg.newmsg("Sold!", C_GREEN);

	  /* item is no more players */
	  item->i.status |= ITEM_UNPAID;

	  /* calculate the shop sellout price */
	  mod=(real)level->rooms[keeper->roomnum].sellp;
	  mod=mod/100;
	  copper=(real)item->i.price;
	  copper=copper * matmod * mod;
	  item->i.price=(int32u)copper;

	  return true;
     }
     else return false;
}

/*
 * Shopkeeper routine which tries to drop unpaid items to the floor
 * Drops only ONE item and returns.
 */
bool shopkeeper_drop(level_type *level, _monsterlist *keeper)
{
//   item_info *iptr;
   Tinvpointer invptr;

   invptr=keeper->inv.first;

   if(!invptr)
      return false;

   /* drop the first UNPAID item */
   while(invptr) {
      if(invptr->i.status & ITEM_UNPAID) {
	 if(inv_transfer2inv(level, &keeper->inv, keeper->equip, &level->inv,
			     invptr, -1, keeper->x, keeper->y)) {

	    sprintf(tempstr, 
		    "%s just put something new for sale", 
		    keeper->m.name);
	    msg.add_dist(level, keeper->x, keeper->y, 
			 tempstr, C_GREEN, NULL, C_CYAN);

	    return true;
	 }
      }
      invptr=invptr->next;
   }

   return false;
}

bool shopkeeper_get(level_type *level, _monsterlist *monster, inv_info *itemptr)
{
   if(inv_transfer2inv(level, &level->inv, NULL, &monster->inv,
		       itemptr, -1, 0, 0)) {
      if(haslight(level, monster->x, monster->y)) {
	 monster_sprintf(nametemp, monster, true, true );
	 sprintf(tempstr, "%s repositions some items.", nametemp);

	 msg.add_dist(level, monster->x, monster->y, tempstr, C_WHITE, 
		      keep_noseemsg[0], C_WHITE);
      }
      return true;
   }

   return false;
}

int16s shopkeeper_move(level_type *level, _monsterlist *keeper)
{
   inv_info *iptr;

   int16s timetaken;
   int16s drx, dry, nx, ny, tx, ty;
   int16s gx1=-1, gy1=-1, gx2=-1, gy2=-1, gx3=-1, gy3=-1, gx4=-1, gy4=-1;
   int8u dir;
   int16s player_distance;

   drx=level->rooms[keeper->roomnum].doorx;
   dry=level->rooms[keeper->roomnum].doory;


   player_distance=distance(drx, dry, player.pos_x, player.pos_y);

   shopkeeper_greet(level, keeper);

   /* current coords */
   nx=keeper->x;
   ny=keeper->y;

   if(level->loc[dry-1][drx-1].type == TYPE_ROOMFLOOR) {
      gx1=drx-1;
      gy1=dry-1;
   }
   else if(level->loc[dry+1][drx-1].type == TYPE_ROOMFLOOR) {
      gx2=drx-1;
      gy2=dry+1;
   }
   else if(level->loc[dry-1][drx+1].type == TYPE_ROOMFLOOR) {
      gx3=drx+1;
      gy3=dry-1;
   }
   else if(level->loc[dry+1][drx+1].type == TYPE_ROOMFLOOR) {
      gx4=drx+1;
      gy4=dry+1;
   }

   /* if attacking then move after the target */
   if((keeper->m.status & MST_ATTACKMODE) || keeper->path!=NULL) {

      /* evaluate path back to the door */
      if(distance(drx, dry, nx, ny) > SHOPKEEPER_MAXDOORDIST
	 && keeper->path==NULL) {
	 keeper->path = path_findroute(level, keeper->x, keeper->y,
				       drx, dry);
	 if(keeper->m.status & MST_ATTACKMODE) 
	    keeper->m.status ^= MST_ATTACKMODE;

      }

      if(drx == keeper->x && dry == keeper->y) {
	 path_clear(&keeper->path);
      }

      keeper->targetx=0;
      keeper->targety=0;

      return move_monster(keeper, level);


      /* move if the door is not reached */
/*
      if(distance(drx, dry, nx, ny) < 5) {
	 keeper->targetx=0;
	 keeper->targety=0;
	 timetaken=move_monster(keeper, level);
	 return timetaken;
      }
      else {
	 keeper->m.status ^= MST_ATTACKMODE;
	 keeper->targetx=drx;
	 keeper->targety=dry;
      }
*/
   }

   /* check for items in the door area */
   if(keeper->targetx==0 && keeper->targety==0) {
      for(dir=1; dir<10; dir++) {
	 tx=drx+move_dx[dir];
	 ty=dry+move_dy[dir];

	 if(level->loc[ty][tx].type == TYPE_ROOMFLOOR) {
	    iptr=giveiteminfo(level, tx, ty);
	    if(iptr) {
	       keeper->targetx=iptr->x;
	       keeper->targety=iptr->y;
	       break;
	    }

	 }

      }
   }
   else {
      if(keeper->x==keeper->targetx && keeper->y==keeper->targety) {
	 keeper->targetx=0;
	 keeper->targety=0;

	 iptr=giveiteminfo(level, keeper->x, keeper->y);
	 if(iptr)
	    shopkeeper_get(level, keeper, iptr);

      }
   }

   if(keeper->targetx>0 && keeper->targety>0 && player_distance>1) {     

//      iptr=giveiteminfo(level, keeper->targetx, keeper->targety);
//      if(iptr) {
      monster_moveTOtarget(level, &nx, &ny, 
			   keeper->targetx, keeper->targety, false);
//      }
//      else {
//	 keeper->targetx=drx;
//	 keeper->targety=dry;
//      }

   }
   /* move to block the door area if player has taken something */
   else if((player.bill > 0) &&
	   (level->rooms[keeper->roomnum].flags & ROOM_PLAYERHERE) ) {

      if(RANDU(100) < 3) {
//	 sprintf(tempstr, "%s %s", 
//		 keeper->m.name, keep_billmsg[RANDU(BILLMSG_NUM)]);
//	 msg.newmsg(tempstr, C_WHITE);
      }

      if(nx!=drx || ny!=dry)
	 monster_moveTOtarget(level, &nx, &ny, drx, dry, true);
   }
   else {
      /* if player is closing the shop door, move near the door area */
      if( player_distance < 8) {
	 if(gx1>0 && gy1>0) {
	    if(nx!=gx1 || ny!=gy1)
	       monster_moveTOtarget(level, &nx, &ny, gx1, gy1, true);
	 }
	 else if(gx2>0 && gy2>0) {
	    if(nx!=gx2 || ny!=gy2)
	       monster_moveTOtarget(level, &nx, &ny, gx2, gy2, true);
	 } 
	 else if(gx3>0 && gy3>0) {
	    if(nx!=gx3 || ny!=gy3)
	       monster_moveTOtarget(level, &nx, &ny, gx3, gy3, true);
	 }
	 else if(gx4>0 && gy4>0) {
	    if(nx!=gx4 || ny!=gy4)
	       monster_moveTOtarget(level, &nx, &ny, gx4, gy4, true);
	 }
	 if( level->rooms[keeper->roomnum].flags & ROOM_PLAYERHERE) {

	    if(RANDU(100) < 3) {
//	       sprintf(tempstr, "%s %s", 
//		       keeper->m.name, 
//		       keep_nobillmsg[RANDU(NOBILLMSG_NUM)]);
//	       msg.newmsg(tempstr, C_WHITE);
	    }
	 }
      }
      else {
	 dir=1+RANDU(9);
	 tx=keeper->x + move_dx[dir];
	 ty=keeper->y + move_dy[dir];

	 /* drop items, but not next to the door */
	 if(distance(drx, dry, nx, ny) > 1) 
	    shopkeeper_drop(level, keeper);

	 if(level->loc[ty][tx].type != TYPE_ROOMFLOOR) {
	    nx=keeper->x;
	    ny=keeper->y;
	 }
	 else {
	    monster_moveTOtarget(level, &nx, &ny, tx, ty, true);

	    if(RANDU(500) < 3) {
//	       msg.newmsg(keep_noseemsg[RANDU(NOSEEMSG_NUM)], C_WHITE);
	    }
	 }

      }
   }

   timetaken = calc_time( get_stat(&keeper->stat[STAT_SPD]), 
			  TIME_MOVEAROUND, keeper->tactic );

   if(nx!=player.pos_x || ny!=player.pos_y) {
      keeper->x=nx;
      keeper->y=ny;
   }

   return timetaken;

}

bool inside_room(level_type *level, int16u roomnum, int16s x, int16s y)
{

   if( (x > level->rooms[roomnum].x1 && x < level->rooms[roomnum].x2) &&
       (y > level->rooms[roomnum].y1 && y < level->rooms[roomnum].y2))
      return true;

   return false;
}

void keeper_greet(level_type *level, _monsterlist *owner, _monsterlist *creat)
{
   char *shopname;
   int8u shoptype, nameindex;

   bool greet;

   /* do not greet myself :-) */
   if(creat==owner)
      return;

   shoptype=level->rooms[owner->roomnum].shoptype;
   nameindex=level->rooms[owner->roomnum].shopname;

   /* determine the shop type */
   if(shoptype==SHOPTYPE_GENERAL) {
      shopname=shopname_general[nameindex];
   }
   else if(shoptype==SHOPTYPE_ARMOUR) {
      shopname=shopname_armour[nameindex];
   }
   else if(shoptype==SHOPTYPE_WEAPON) {
      shopname=shopname_weapon[nameindex];
   }
   else if(shoptype==SHOPTYPE_FOOD) {
      shopname=shopname_food[nameindex];
   }
   else
      shopname=shopname_general[0];

   if(creat) {
      /* ungreet if goes to door */
      if(creat->m.status & MST_INSIDESHOP) {
	 if(inside_room(level, owner->roomnum, creat->x, creat->y))
	    return;
//	 if(level->loc[creat->y][creat->x].type == TYPE_ROOMFLOOR)
//	    return;

//	 if(creat->x != level->rooms[owner->roomnum].doorx ||
//	    creat->y != level->rooms[owner->roomnum].doory)
//	    return;
	 creat->m.status^=MST_INSIDESHOP;
	 greet=false;
      }
      else {
	 /* do not greet until walks on the floor */
	 if(!inside_room(level, owner->roomnum, creat->x, creat->y))
	    return;

	 creat->m.status|=MST_INSIDESHOP;
	 greet=true;
      }
   }
   else {
      /* player ungreet ? */
      if( level->rooms[owner->roomnum].flags & ROOM_PLAYERHERE ) {
//	 msg.vnewmsg(C_RED, "Playerhere! Room=%d! ", owner->roomnum);
	 /* do not ungreet if player is still inside a shop */
//	 if(level->loc[player.pos_y][player.pos_x].type == TYPE_ROOMFLOOR)
//	    return;
	 if(inside_room(level, owner->roomnum, player.pos_x, player.pos_y))
	    return;

	 level->rooms[owner->roomnum].flags^=ROOM_PLAYERHERE;
	 greet=false;

      }
      else {
	 /* no greet until walks on the floor */
	 if(!inside_room(level, owner->roomnum, player.pos_x, player.pos_y))
	    return;

//	 if(level->loc[player.pos_y][player.pos_x].type!=TYPE_ROOMFLOOR)
//	    return;

	 level->rooms[owner->roomnum].flags|=ROOM_PLAYERHERE;
	 greet=true;
      }

   }

   if(greet) {
      if(creat) {
	 monster_sprintf(nametemp, creat, false, true);
	 if(creat->m.status & MST_KEEPERHATES) {
	    sprintf(tempstr, "%s yells to %s: \"Get out of here...NOW!\".", 
		    owner->m.name, nametemp);
	    msg.add_dist(level, owner->x, owner->y, tempstr, CH_MAGENTA, 
			 "You hear from distance: \"Get out of here...NOW!\".",
			 C_MAGENTA);
	    owner->m.status|=MST_ATTACKMODE;
	    owner->target=creat;
	 }
	 else {
	    sprintf(tempstr, 
		    "%s welcomes %s: \"Welcome to %s's %s!\".", 
		    owner->m.name, nametemp, owner->m.name, shopname);
	    sprintf(i_hugetmp, 
		    "You hear from distance: \"Welcome to %s's %s!\".",
		    owner->m.name, shopname);
	    msg.add_dist(level, owner->x, owner->y, 
			 tempstr, C_WHITE, i_hugetmp, C_WHITE);
	 }
      }
      else {
	 if(owner->m.status & MST_HATEPLAYER) {
	    owner->m.status|=MST_ATTACKMODE;
	    owner->target=NULL;
	    sprintf(tempstr, 
		    "%s yells: \"Get OUT of here, thief!\".", 
		    owner->m.name);
	    msg.newmsg(tempstr, CH_MAGENTA);
	    return;
	 }
	 else {
	    if(!(owner->m.status & MST_KNOWN)) {
	       // make shopkeeper known!
	       owner->m.status|=MST_KNOWN;
	       sprintf(tempstr, "Welcome to %s's %s, stranger!", 
		       owner->m.name, shopname);
	    }
	    else {
	       // if hates player, attack !
	       sprintf(tempstr, 
		       "Hello again %s, welcome to %s's %s!", 
		       player.name, owner->m.name, shopname);
	    }
	    msg.newmsg(tempstr, CH_GREEN);
	 }
      }

   }
   else {
      if(creat) {
	 if(creat->bill>0) {

	    if(owner->m.status & MST_GUARDDOOR)
	       owner->m.status^=MST_GUARDDOOR;

	    creat->m.status |= MST_KEEPERHATES;

	    sprintf(tempstr, 
		    "%s screams: \"THIEEEEEEEF! WHERE'S THE SHERIFF!?!\".", 
		    owner->m.name);
	    msg.add_dist(level, owner->x, owner->y, 
			 tempstr, C_GREEN, 
			 "You hear someone calling for sheriff!", C_WHITE);
	    creat->bill=0;
	    inv_payall(&creat->inv);

	    return;
	 }

	 if(!(creat->m.status & MST_KEEPERHATES)) {
	    sprintf(tempstr, 
		    "%s says: \"Come again soon, stranger!\".", owner->m.name);
	    msg.add_dist(level, owner->x, owner->y, tempstr, C_GREEN, 
			 "You hear from distance: \"Come again soon, stranger!\".", C_WHITE);
	 }
      }
      else {
	 /* if bill>0 then it's a successfull steal */ 
	 if(player.bill>0) {
	    
	    /* mark owner to hate player */
	    owner->m.status |= MST_HATEPLAYER;
	   
	    if(owner->m.status & MST_GUARDDOOR)
	       owner->m.status^=MST_GUARDDOOR;

	    // clear the bill, succesfull steal
	    player.bill=0;

	    /* handle all player items and remove unpaid mark */
	    inv_payall(&player.inv);

	    sprintf(tempstr, 
		    "%s screams: \"THIEEEEEEEF! WHERE'S THE SHERIFF!?!\".", 
		    owner->m.name);
	    msg.newmsg(tempstr, CH_RED);
	    return;
	 }

	 /* friendly bye-msg if doesn't hate player */
	 if(!(owner->m.status & MST_HATEPLAYER)) {
	    sprintf(tempstr, "%s says: \"Come again soon, %s!\".", 
		    owner->m.name, player.name);
	    msg.newmsg(tempstr, C_WHITE);
	 }
	 /* if keeper is in attack mode 
	  * yell an unfriendly phrase when target leaves shop
	  */
	 else {
	    sprintf(nametemp, keep_throwmsg[RANDU(GETOUTMSG_NUM)], 
		    "Someone");
	    sprintf(tempstr, keep_throwmsg[RANDU(GETOUTMSG_NUM)], 
		    owner->m.name);

	    msg.add_dist(level, owner->x, owner->y, tempstr, CH_MAGENTA,
			 nametemp, C_MAGENTA);
	 }
//	 owner->m.status^=MST_ATTACKMODE;
      }

   }
}

void shopkeeper_greet(level_type *level, _monsterlist *owner)
{
   _monsterlist *creat;

   if(!owner) {
      return;
   }
   /* greet every monster who enters the shop */
   creat=level->monsters;
   while(creat) {
      if(creat->inroom==owner->roomnum && creat!=owner)
	 keeper_greet(level, owner, creat);

      creat=creat->next;
   }

//   if(owner->roomnum==player.inroom) {
      keeper_greet(level, owner, NULL);
//   }
   return;
}


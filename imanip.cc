/**************************************************************************
 * imanip.cc --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 18.10.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 18.10.1998                                         *
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
 * Item manipulation functions                                            *
 **************************************************************************/ 

#include "saladir.h"
#include "output.h"

#include "inventor.h"
#include "utility.h"
#include "scrolls.h"
#include "imanip.h"

void item_setrandomcondition(item_def *item)
{
   int16u pluck;

   if(!item)
      return;

   if(item->status & ITEM_ARTIFACT)
      return;

   if(item->type == IS_SPECIAL || item->type == IS_MONEY)
      return;

   item->icond = COND_USED;

   pluck = get_stat(&player.stat[STAT_LUC]) * (RANDU(100/STATMAX_LUCK));

   for(int i=0; i<5; i++) {
      if(RANDU(100) < pluck) {
	 if(item->icond > 0)
	    item->icond--;
      }
      else if(item->icond < COND_BROKEN)
	 item->icond++;
      else
	 item->icond = RANDU(COND_BROKEN);
   }

   if(item->icond==COND_BROKEN)
      item->icond--;
 
}

bool container_removeinventory(item_def *iptr)
{

   if(!iptr) {
      msg.newmsg("Error: NULL item pointer passed to container_clear().", 
		 CHB_RED);
      return false;
   }

   /* check if the item is a container */
   if(iptr->type != IS_CONTAINER) {
      return true;
   }

   if(iptr->inv == NULL) {
      msg.vnewmsg(CHB_RED, "Error: item %s is a container but the inventory "
		  "hasn't been initialized!", iptr->name);
      return false;
   }

   /* clear the item inventory */
   inv_removeall(iptr->inv, NULL);

   /* remove inventory from item */
   delete iptr->inv;

   return true;
}

bool container_init(item_def *iptr)
{
   /* first empty inventory if it already exists */
   if(iptr->inv)
      container_removeinventory(iptr);

   /* do not create inventory if not a container */
   if(iptr->type != IS_CONTAINER)
      return false;

   /* create new inventory */
   iptr->inv = new Tinventory;

   if(!iptr->inv) {
      return false;
   }

   inv_init(iptr->inv, NULL);

   return true;
}

/*
 * free resources reserved by some item
 *
 */
bool item_clearall(item_def *iptr)
{
   /* remove item inventory, if any */
   return container_removeinventory(iptr);
}

/*
 * Set item cursed/blessed status
 *
 */
void set_alignment(item_def *item)
{
   int16u prob, r;

   /* set cursed, uncursed status */
   prob=get_stat(&player.stat[STAT_LUC]) / 2;

   r=throwdice(1, 100, 0);

   if(r <= prob)
      item->status |= ITEM_CURSED;
   else if(r >= 100-prob)
      item->status |= ITEM_BLESSED;

   /* set item religious alignment
    * **hack** not used for now so set NEUTRAL
    */
   item->align = NEUTRAL;
}

/*
 * This sets the material for the item
 * 
 */
void set_material(item_def *item, int16s material)
{
   int8u mat;
   int16u prob;

   /* don't touch artifacts */
   if(item->status & ITEM_ARTIFACT || item->type == IS_SPECIAL)
      return;

   set_alignment(item);

   if(item->type == IS_FOOD || item->type == IS_CONTAINER ||
      item->type == IS_SCROLL )
      return;

   if(item->type == IS_LIGHT) {
      item->material=MAT_WOOD;
      return;
   }

   if(item->type == IS_POTION) {
      item->material=MAT_GLASS;
      return;
   }

   if(item->material==MAT_NOMATERIAL || item->material==MAT_FABRIC) {
      item->pmod4=1+RANDU(15);
      return;
   }

   if(material==-1) {
      /* find a material */
      while(1) {
	 mat=RANDU(num_materials);
//			matptr=materials+mat;

	 prob=throwdice(10, 100, get_stat(&player.stat[STAT_LUC]));

	 /* if rand value smaller than appearing probability, continue */
	 if( prob >= materials[mat].appearprob ) {

            /* accept only correct types of material */
	    if(materials[mat].status & MATSTAT_SPECIAL)
	       continue;

	    if(item->type==IS_MISWEAPON && item->group==WPN_BOW && 
	       (materials[mat].status & MATSTAT_NOTBOWS))
	       continue;
	    if(item->type==IS_MISSILE && 
	       materials[mat].status & MATSTAT_NOTMISSILE)
	       continue;

	    if((item->type==IS_WEAPON2H || item->type==IS_WEAPON1H) &&
	       (materials[mat].status & MATSTAT_NOTWEAPON))
	       continue;
	    if((item->type==IS_SHIELD || item->type == IS_BRACELET) &&
	       (materials[mat].status & MATSTAT_NOTWEAPON))
	       continue;

	    if(item->type==IS_RING && 
	       !(materials[mat].status & MATSTAT_RING))
	       continue;

	    if(item->type==IS_ARMOR && (materials[mat].status & MATSTAT_NOTARMOR))
	       continue;

	    if(materials[mat].appearprob >= 750) {
	       msg.newmsg("Somehow you feel fortunate.", CH_GREEN);
	    }
	    break;
	 }
      }
   }
   else
      mat=material;

   /* apply the material to item */
   item->material=mat;

   /* durability */
   item->turnsleft=materials[mat].durability;

   /* apply the material weight modifier */
   item->weight=(int16u)( materials[mat].wmod * item->weight);

   /* apply the damage and armorvalue modifiers */
   if(item->type == IS_WEAPON2H || item->type == IS_WEAPON1H)
      item->meldam_mod+=materials[mat].dam;

   if(item->type == IS_MISWEAPON || item->type==IS_MISSILE )
      item->misdam_mod+=materials[mat].dam;

   if(item->type == IS_ARMOR || item->type == IS_SHIELD || item->type == IS_BRACELET )
      item->ac+=materials[mat].dam;

   item->dv+=materials[mat].dv;

   /* hack -- identify now */
//   item->status|=ITEM_IDENTIFIED;

}


/***********************************************************/
/*                                                         */
/* Item generator routines                                 */
/*                                                         */
/***********************************************************/

item_def *init_moneyitem(int16s type)
{
   item_def *iptr;

   iptr = (item_def*)malloc(sizeof(item_def));

   if(!iptr) {
      msg.newmsg("init_moneyitem(): no memory!", CH_RED);
      return NULL;
   }
   mem_clear(iptr, sizeof(item_def));

   /* if type is <0 init a random valuable */
   if(type<0 || type >= num_valuables)
      type = RANDU(num_valuables);

   /* set name */
   my_strcpy(iptr->name, valuables[type].name, ITEM_NAMEMAX);

   /* set other item attributes */
   iptr->turnsleft = -1;
   iptr->type=IS_MONEY;
   iptr->group=type;
   iptr->align=NEUTRAL;
   iptr->material=valuables[type].material;
   iptr->weight=valuables[type].weight;

   return iptr;  
}


item_def *init_scrollitem(int16s type)
{
   item_def *iptr;

   iptr = (item_def*)malloc(sizeof(item_def));

   if(!iptr) {
      msg.newmsg("init_scrollitem(): no memory!", CH_RED);
      return NULL;
   }

   mem_clear(iptr, sizeof(item_def));

   /* if type is <0 init a random valuable */
   if(type<0 || type >= num_scrolls)
      type = RANDU(num_scrolls);

   /* set item data from scroll template (items.cc)*/
   *iptr=templ_scroll;

   /* check for self written scroll type */
   if(type==SCROLL_SELFWRITTEN)
      type++;

   iptr->group=type;
   iptr->pmod1=list_scroll[type].group;
   iptr->pmod2=list_scroll[type].spell;
   iptr->pmod3=list_scroll[type].skill;

   /* scroll label in "sname" */
   my_strcpy(iptr->sname, list_scroll[type].uname, sizeof(iptr->sname));

   /* scroll realname in "rname" */
   my_strcpy(iptr->rname, list_scroll[type].name, sizeof(iptr->rname));

   /* automagically identify known scrolls */
   if(list_scroll[type].flags & SCFLAG_IDENTIFIED)
      iptr->status |= ITEM_IDENTIFIED;

   set_material(iptr, -1);

   return iptr;  
}

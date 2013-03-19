/* player and monster
	inventory routines

   Legend of Saladir

   (C) Erno Tuomainen 1997/1998

*/

#include "inventor.h"
#include "generate.h"
#include "player.h"
#include "file.h"
#include "shops.h"
#include "textview.h"
#include "imanip.h"

const char *file_inventoryhelp = "inventor.hlp";

/****************************************/
/* initializes a new node for inventory */
/****************************************/
inv_info *inv_inititem(Tinventory *inventory)
{
   Tinvpointer newnode, start;

   start=inventory->first;

   /* allocate new node for inventory */
   newnode=new inv_info;
   if(!newnode) {
      msg.newmsg("inv_inititem(): no memory!", CH_RED);
      return NULL;
   }

   mem_clear(newnode, sizeof(inv_info));

   newnode->i.turnsleft = -1;
   newnode->slot = -1;
   newnode->x = 0;
   newnode->y = 0;

   // if inventory is empty
   if(!start) {
      inventory->first=newnode;
      newnode->next=NULL;
   }
   else {
      newnode->next=inventory->first;
      inventory->first=newnode;
   }

   return newnode;
}

/* place "count" items of "*item" to inventory "inv" */
/* update pack weight also */
Tinvpointer inv_createitem(Tinventory *inv, item_def *item, int32u count)
{
   Tinvpointer newitem=NULL;

   if(!inv || !item)
      return NULL;

   /* ensure that always atleast ONE item gets created! */
   if(!count) count = 1;

   newitem=inv_inititem(inv);

   if(!newitem) {
      msg.newmsg("inv_createitem(): no memory!", CH_RED);
      return NULL;
   }

   /* place item data and count */
   newitem->count=count;
   newitem->i=*item;

   /* update inv weight */
   inv_calcweight(inv);

   return newitem;
}


/*
 * This is a wrapper for inv_additem, it simulates my old
 * level_createitem()-function. 
 * Why? Previously I had separate lists for level items and
 * monster/player inventories. They're now unified.
 *
 */
Tinvpointer level_createitem(level_type *level, int16s x, int16s y,
			     int16u group, int16s type, int16u count,
			     int16s material)
{
   Tinvpointer inode;

   inode = inv_additem(&level->inv, group, type, count, material);

   if(!inode) {
      msg.newmsg("Error: inv_additem() returned a NULL pointer.", 
		 CHB_RED);
      msg.vnewmsg(CHB_RED, "Group = %d, Type = %d, count = %d!", 
		  group, type, count);
      return NULL;
   }

   /* should set some random stats for the item in question */
   item_setrandomcondition(&inode->i);

   /* init level coordinates */
   inode->x = x;
   inode->y = y;

   /* if we created a item which can't be walked on */
   /* mark item not passable */
   if(inode->i.status & ITEM_NOTPASSABLE)
      level->loc[y][x].flags &= ( 0xffff ^ CAVE_PASSABLE );

   return inode;
}

Tinvpointer inv_additem(Tinventory *inventory, int16u group, 
		 int16s type, int16u count, int16s material)
{
   Tinvpointer inode=NULL;

   item_def *items, *newitem;
//   item_def newitem;

   items=NULL;
   if(group==IS_MONEY) {

      newitem = init_moneyitem(type);
      if(!newitem) {
	 msg.newmsg("init_moneyitem(): returned null!", CH_RED);
	 return NULL;
      }

      inode=inv_createitem(inventory, newitem, count);

      free(newitem);
      newitem = NULL;

      return inode;
   }
   else if(group==IS_ARMOR) {
      items=armor;
      if(type < 0 || type>=num_armors)
	 type = RANDU(num_armors);

   }
   else if(group==IS_WEAPON1H || group==IS_WEAPON2H || group==IS_MISWEAPON) {
      items=weapons;

      if(type < 0 || type>=num_weapons)
	 type = RANDU(num_weapons);
   }
   else if(group==IS_LIGHT || group==IS_FOOD || group==IS_CONTAINER
	   || group==ISMG_MISCITEM ) {
      items=miscitems;

      if(type < 0 || type>=num_miscitems)
	 type = RANDU(num_miscitems);
   }
   else if(group==IS_SPECIAL) {
      items=SPECIAL_ITEMS;
      if(type < 0 || type>=num_specials)
	 type = RANDU(num_specials);
   }
   else if(group==IS_SCROLL) {
      newitem = init_scrollitem(type);
      if(!newitem)
	 return NULL;

      inode=inv_createitem(inventory, newitem, count);

      free(newitem);
      newitem = NULL;

      return inode;
   }
   else {
      msg.vnewmsg(CHB_RED, "Error: Item group %d not supported"
		  " by inv_additem().", group);
      return NULL;
   }

   if(items) {
      newitem=&items[type];

      inode=inv_createitem(inventory, newitem, count);
      if(!inode)
	 return NULL;

      set_material(&inode->i, material);
      if(inode->i.type==IS_CONTAINER) {
	 /* container gets created here! */
	 if( !container_init(&inode->i) ) 
	    msg.vnewmsg(CHB_RED, 
			"Failed to initialize container %s.", 
			inode->i.name);
      }

      return inode;
   }

   return inode;
}

/*
** This routine counts all items listed in position x, y
** in the level
*/
/*
int32u inv_countitems(Tinventory *inv, int16s x, int16s y)
{
   int8u num=0;

   Tinvpointer ptr;

   ptr=inv->first;

   while(ptr) {
      if(ptr->x==x && ptr->y==y || (x < 0 && y < 0))
	 num++;

      ptr=ptr->next;

   }
   return num;
}
*/

void inv_clearequipslot(Tinventory *inv, _Tequipslot *equip, int16u slot)
{
   if(!equip)
      return;

   /* stupid check for 2handed weapons */
   /* 2 handed weapons can only be held in HANDS so it works */
   if(equip[slot].item->i.type == IS_WEAPON2H ||
      equip[slot].item->i.type == IS_MISWEAPON) {
      equip[EQUIP_LHAND].reserv=0;
      equip[EQUIP_RHAND].reserv=0;
   }
   else
      equip[slot].reserv=0;

   equip[slot].item=NULL;
   equip[slot].in_use=0;   
}

int16s inv_removeitem( Tinventory *inv, _Tequipslot *equip, 
		       Tinvpointer remthis, int32s count )
{
   Tinvpointer last, ptr;
   bool found=false;

   ptr=last=inv->first;

   if(count==0)
      return 0;

   // return if no itemlist
   if(!ptr) return -1;
   // if trying to remove NULL node
   if(!remthis) return -1;

   while(!found) {
      if(remthis==ptr) {
	 found=true;
	 break;
      }
      if(!ptr->next) break;
      last=ptr;
      ptr=ptr->next;

   }
   // remove the requested node at this location
   if(found) {
      /* if item is equipped */
      if( ptr->slot!=-1 ) {
	 inv_clearequipslot(inv, equip, ptr->slot);
      }

      /* substract the requested item count */
      if(( count >= (int32s)ptr->count  ) || (count<0)) {
	 count=ptr->count;
      }
      
      ptr->count-=count;

      /* update inv weight */
//      inv->weight-=count * ptr->i.weight;

//      if(inv==&player.inv) {
//	 GAME_NOTIFYFLAGS|=GAME_FLAGSCHG;
//      }

      /* if count goes to zero */
      if(ptr->count==0) {
	 if(inv->first==ptr)
	    inv->first=ptr->next;
	 else
	    last->next=ptr->next;

	 delete ptr;
      }

      inv_calcweight(inv);

      return count;
   }
   else return 0;
}


/*
** This routine will pile items in the inventory...
** it will use memory compare for this...
*/
void inv_pileitems_coords(Tinventory *inv)
{
   Tinvpointer iptr, startptr, rptr;

   startptr=inv->first;

   // check if there is nothing to pile...
   // no need to pile if only one item in inventory
   if(!startptr || !startptr->next)
      return;

   iptr=startptr->next;

   while(startptr) {

      /* pile, but not equipped items */
      if(startptr->slot < 0) {
	 iptr=startptr->next;

	 while(iptr) {
	    rptr=iptr;
	    iptr=iptr->next;

	    if(rptr->x==startptr->x && rptr->y==startptr->y) {
	       if( !(mem_comp(&startptr->i, &rptr->i, sizeof(item_def))) ) {
		  startptr->count+=rptr->count;

//		  inv->weight+=rptr->count * rptr->i.weight;

		  inv_removeitem(inv, NULL, rptr, -1);
		  // remove iptr from inventory...
	       }
	    }
	 }
      }
      startptr=startptr->next;
   }

//   inv_calcweight(inv);
}


/*
 * Create item piles
 *
 */
void inv_pileitems(Tinventory *inv)
{
   Tinvpointer iptr, startptr, rptr;

   startptr=inv->first;

   // check if there is nothing to pile...
   // no need to pile if only one item in inventory
   if(!startptr || !startptr->next)
      return;

   iptr=startptr->next;

   while(startptr) {

      /* pile, but not equipped items */
//      if(!(startptr->i.status & ITEM_EQUIPPED)) {
      if(startptr->slot < 0) {
	 iptr=startptr->next;

	 while(iptr) {
	    rptr=iptr;
	    iptr=iptr->next;

	    if( !(mem_comp(&startptr->i, &rptr->i, sizeof(item_def))) ) {
	       startptr->count+=rptr->count;

//	       inv->weight+=rptr->count * rptr->i.weight;

	       inv_removeitem(inv, NULL, rptr, -1);
	       // remove iptr from inventory...
	    }
	 }
      }
      startptr=startptr->next;
   }

//   inv_calcweight(inv);
}

/******************************************/
/* take one item out from a pile of items */
/******************************************/
Tinvpointer inv_splitpile(Tinventory *inv, Tinvpointer pile)
{
   Tinvpointer splitnode;

   if(pile->count < 2)
      return pile;
  
   splitnode=inv_inititem(inv);

   if(splitnode) {
      splitnode->i = pile->i;
      splitnode->x = pile->x;
      splitnode->y = pile->y;
      splitnode->count = 1;
      pile->count--;

      return splitnode;
   }
   else
      return NULL;

}


void inv_init(Tinventory *inv, _Tequipslot *equip)
{
   inv->first = NULL;
   inv->capasity = 0;
   inv->copper = 0;

   inv_calcweight(inv);

   /* init inventory EQUIPMENT */
   if(equip != NULL) {
      for(int16u i=0; i<MAX_EQUIP; i++) {
	 equip[i].item=NULL;
	 equip[i].in_use=0;
	 equip[i].reserv=0;
	 equip[i].status=EQSTAT_OK;
      }
   }
}

/*
 * Remove everything from the inventory pointed by 'inv'
 * Free memory
 */
void inv_removeall(Tinventory *inv, _Tequipslot *equip)
{
   Tinvpointer ptr, rem;

   ptr=inv->first;

   while(ptr) {
      rem=ptr;
      ptr=ptr->next;

      item_clearall(&rem->i);
      delete rem;
   }

   inv_init(inv, equip);
}

/*
 * Calculate inventory weight, will recurse if inventory contains 
 * containers
 *
 * The given inventory is updated with the current weight,
 * also any subinventories will be updated with their weights!
 * So this needs only be called for any "master inventory", ie. for
 * player inventory, level inventory or monster inventory.
 *
 * Result:
 * 32-bit unsigned weight (1000 is 1kg)
 */
int32u inv_calcweight(Tinventory *inv)
{
   Tinvpointer iptr;

   if(!inv)
      return 0;

   inv->weight = 0;

   iptr=inv->first;

   while(iptr) {
      /* if item is a container, recurse and calculate it */
      if(iptr->i.inv!=NULL) {
	 inv->weight += inv_calcweight(iptr->i.inv);
      }
      inv->weight += iptr->i.weight * iptr->count;

      iptr = iptr->next;
   }

   /* update the money amount at the same time */
   inv->copper = inv_moneyamount(inv);

   if(inv == &player.inv)
      GAME_NOTIFYFLAGS |= GAME_MONEYCHG;

   return inv->weight;
}

/*
 * Return the weight of a item, will also take care of container items!
 *
 * result:
 * 32-bit unsigned weight (1000 is 1kg)
 *
 */
int32u item_calcweight(item_def *item)
{
   int32u total=0;

   if(!item)
      return 0;

   if(item->inv!=NULL)
      total += inv_calcweight(item->inv);

   total += item->weight;

   return total;
}

/*
 * Calculate the amount of money stored in the inventory 'inv'
 * will recurse if inventory contains any container items
 *
 * Result:
 * 32-bit unsigned money amount in copper coins (8 copper = 1 gold)
 * 
 */
int32u inv_moneyamount(Tinventory *inv)
{
   Tinvpointer iptr;
//   real mod;
   int32u total=0;

   if(!inv->first)
      return 0;

//   inv_pileitems(inv);

   iptr=inv->first;
   total=0;
   while(iptr) {
//      if(iptr->i.type == IS_MONEY && iptr->slot<0) {
      if(iptr->i.type == IS_MONEY && iptr->i.group < num_valuables) {
	 total += valuables[iptr->i.group].value * iptr->count;

/*
	 if(iptr->i.group==MONEY_GOLD) {
	    mod=1.0/valuables[MONEY_COPPER].value;

	    total+=(int32u)(iptr->count*mod);
	    total += valuables[MONEY_GOLD].value;
	 }

	 if(iptr->i.group==MONEY_SILVER) {
	    mod=1.0/valuables[MONEY_SILVER].value;

	    total+=(int32u)(iptr->count*mod);
	 }

	 if(iptr->i.group==MONEY_COPPER) {
	    total+=iptr->count;
	 }
*/
      }
      /* if item has an inventory, recurse*/
      else if(iptr->i.inv != NULL) {
	 total += inv_moneyamount(iptr->i.inv);
      }

      iptr=iptr->next;
   }

   inv->copper = total;

   return total;
}

Titemlistptr _collectmoneypointers_recurse(
   Tinventory *inv, 
   Titemlistptr ptrlist
   )
{
   Tinvpointer invptr;

   if(!inv || !ptrlist)
      return NULL;

   invptr=inv->first;

   while(invptr) {
      if(invptr->i.type == IS_MONEY) {
	 ptrlist->ptr=invptr;
	 ptrlist++;
      }
      else if( invptr->i.inv!=NULL ){
	 ptrlist = _collectmoneypointers_recurse(invptr->i.inv, ptrlist);
      }
      ptrlist->ptr=NULL;
     
      invptr=invptr->next;
   }
   
   return ptrlist;
}

Titemlistptr inv_collectmoneyptr(Tinventory *inv)
{
   Titemlistptr ptrlist=NULL;
//   Tinvpointer invptr;
   int16u itemcount=0, i;

   if(!inv) {
      msg.newmsg("Error: NULL inventory passed to inv_collectmoney().",
		 CHB_RED);
      return NULL;
   }

   itemcount = inv_countitems(inv, IS_MONEY, -1, -1, true);

   if(!itemcount)
      return NULL;


   ptrlist=(Titemlistptr)malloc((itemcount + 2) * sizeof(_playerptrlistdef));
   if(!ptrlist) {
      msg.newmsg("Error: can't determine the money amount!", CH_RED);
      return NULL;
   }

   /* clear all pointers first */
   for(i=0; i<itemcount+1; i++) {
      (ptrlist+i)->ptr = NULL;
      (ptrlist+i)->sel = 0;
   }

   if(!_collectmoneypointers_recurse(inv, ptrlist)) {
      free(ptrlist);
      return NULL;
   }

   return ptrlist;
}

/*
bool invnode_initmoney(Tinvpointer node, int8u moneytype, int32u count)
{
   if(!node)
      return false;

   strncpy(node->i.name, valuables[moneytype].name, ITEM_NAMEMAX);
   node->count=count;
   node->i.type=IS_MONEY;
   node->i.group=moneytype;
   node->i.align=NEUTRAL;
   node->i.material=valuables[moneytype].material;
   node->i.weight=valuables[moneytype].weight;

   return true;
}
*/

int32u inv_countitems(Tinventory *inv, int16s type, 
		      int16s x, int16s y, bool recursive)
{
   int32u total;
   Tinvpointer invptr;

   if(!inv)
      return 0;

   total = 0;
   invptr=inv->first;
   while(invptr) {

      if(invptr->i.type != IS_CONTAINER) {
	 if(invptr->i.type != type && type>=0) {
	    invptr=invptr->next;
	    continue;
	 }
      }

      if(x>=0 && y>=0) {
	 if(!(x==invptr->x && y==invptr->y)) {
	    invptr=invptr->next;
	    continue;
	 }
      }

      total++;

      /* do we want the count from all inventory levels ?*/
      if(invptr->i.inv != NULL && recursive)
	 total += inv_countitems(invptr->i.inv, type, -1, -1, true);

      invptr=invptr->next;
   }
      
   return total;
}

/*
** Build a list of item pointers which match to the filter. If filter -1
** then build a list of all items.
** Returns: A pointer to list of pointers sorted by itemtype
*/
Titemlistptr inv_builditemarray(Tinventory *inv, int16s filter,
				      int16s x, int16s y)
{
   Titemlistptr ptrlist;
   Tinvpointer invptr;
   int16u itemcount=0, i, j;

   if(inv == NULL) {
      msg.newmsg("Error: Inv_builditemarray() got NULL inventory.", CHB_RED);

      return NULL;
   }

   itemcount = inv_countitems(inv, filter, x, y, false);

//   if(!itemcount)
//      return NULL;


   ptrlist=new _playerptrlistdef[itemcount+2];
   if(!ptrlist) {
      msg.newmsg("inv_listitems -- out of memory!", CHB_RED);
      msg.newmsg("I suggest you save the game and exit now!", CH_RED);
      return NULL;
   }

   /* clear all pointers first */
   for(i=0; i<itemcount+1; i++) {
      (ptrlist+i)->ptr = NULL;
      (ptrlist+i)->sel = 0;
   }

   /* collect pointers to items, now knows about filtered item type too :) */
   invptr=inv->first;
   i=0;
   while(invptr) {

      if(invptr->i.type != IS_CONTAINER) {
	 if(invptr->i.type != filter && filter>=0) {
	    invptr=invptr->next;
	    continue;
	 }
      }

      if(x>=0 && y>=0) {
	 if(!(x==invptr->x && y==invptr->y)) {
	    invptr=invptr->next;
	    continue;
	 }
      }
      (ptrlist+i)->ptr=invptr;
      i++;
      (ptrlist+i)->ptr=NULL;
      

      invptr=invptr->next;
   }

   /* if no filter matches, return a NULL list*/
   if(!i) {
      return ptrlist;
   }

   // now we have all the pointers to inventory items
   // and last pointer is NULL
   // let's sort them out by item type... lowest first

   bool SWAPDONE=true;
   while(SWAPDONE) {
      SWAPDONE=false;
      i=0;
      j=1;
      while((ptrlist+j)->ptr) {
	 if((ptrlist+i)->ptr->i.type > (ptrlist+j)->ptr->i.type) {
	    SWAPDONE=true;
	    invptr=(ptrlist+i)->ptr;
	    (ptrlist+i)->ptr=(ptrlist+j)->ptr;
	    (ptrlist+j)->ptr=invptr;
	 }
	 i++;
	 j++;
      }
   }
   return ptrlist;
}

bool inv_paymoney(Tinventory *inv, Tinventory *tinv, _Tequipslot *equip,
		  int32u copperneed, bool checkonly)
{
   int32u copperamt, i;
   int32u copper, silver, gold;
   Titemlistptr moneylist;

   if(!inv)
      return false;

   copperamt = inv_moneyamount(inv);

   /* if not enough money, return false */
   if(copperamt < copperneed)
      return false;

   /* check only if money is enough */
   if(checkonly) {
      return true;
   }

   /* build pointer list from money items */
   moneylist = inv_collectmoneyptr(inv);
   
   if(!moneylist) {
      return false;
   }

   /* now we have one array with all money items (coin piles)
    * The array must now be scanned and correct amount
    * of money removed from the inventory.
    *
    */

   i=0;

   while(moneylist[i].ptr && copperneed>0) {
      if(moneylist[i].ptr->i.group < num_valuables) {
	 copperamt = valuables[moneylist[i].ptr->i.group].value * 
	    moneylist[i].ptr->count;

	 /* always remove the source first */
	 inv_removeitem(inv, equip, moneylist[i].ptr, -1);

	 if(copperamt == copperneed) {
	    shopkeeper_coppervalue(copperamt, &gold, &silver, &copper);
	    if(gold>0)
	       inv_additem(tinv, IS_MONEY, MONEY_GOLD, gold, -1);
	    if(silver>0)
	       inv_additem(tinv, IS_MONEY, MONEY_SILVER, silver, -1);
	    if(copper>0)
	       inv_additem(tinv, IS_MONEY, MONEY_COPPER, copper, -1);

	    copperneed-=copperamt;
	    copperamt=0;
	 }
	 else if(copperamt < copperneed) {
	    shopkeeper_coppervalue(copperamt, &gold, &silver, &copper);
	    if(gold>0)
	       inv_additem(tinv, IS_MONEY, MONEY_GOLD, gold, -1);
	    if(silver>0)
	       inv_additem(tinv, IS_MONEY, MONEY_SILVER, silver, -1);
	    if(copper>0)
	       inv_additem(tinv, IS_MONEY, MONEY_COPPER, copper, -1);

	    copperneed-=copperamt;
	    copperamt=0;
	 }
	 else {
	    shopkeeper_coppervalue(copperneed, &gold, &silver, &copper);
	    if(gold>0)
	       inv_additem(tinv, IS_MONEY, MONEY_GOLD, gold, -1);
	    if(silver>0)
	       inv_additem(tinv, IS_MONEY, MONEY_SILVER, silver, -1);
	    if(copper>0)
	       inv_additem(tinv, IS_MONEY, MONEY_COPPER, copper, -1);

	    copperamt-=copperneed;
	    copperneed=0;
	 }

	 /* if money still left, add it back to source inventory */
	 if(copperamt > 0) {
	    shopkeeper_coppervalue(copperamt, &gold, &silver, &copper);
	    if(gold>0)
	       inv_additem(inv, IS_MONEY, MONEY_GOLD, gold, -1);
	    if(silver>0)
	       inv_additem(inv, IS_MONEY, MONEY_SILVER, silver, -1);
	    if(copper>0)
	       inv_additem(inv, IS_MONEY, MONEY_COPPER, copper, -1);
	 }


      }
      i++;
   }

   inv_freemultilist(&moneylist);

   return true;
}

/* removes coins from inventory, priority is on GOLD -> SILVER -> COPPER.
   So most valuable coins will be removed first
   */
#ifdef OLDMONEYPAYINGROUTINEISHERE_AND_ITISSHITTYFORCONTAINERITEMS
/* return total money value in gold coins and
   update 'gold', 'silver' and 'copper' variables with
   the right pointer to the money
   */
int32u inv_countmoney(Tinventory *inv, Tinvpointer *gold, 
		      Tinvpointer *silver, Tinvpointer *copper)
{
   Tinvpointer iptr;
   real mod;
   int32u total;

   if(!gold || !silver || !copper) {
      msg.newmsg("Error with moneypointers at inv_countmoney()", CH_RED);
      return 0;
   }

   if(!inv->first)
      return 0;

   inv_pileitems(inv);

   *gold=NULL;
   *silver=NULL;
   *copper=NULL;

   iptr=inv->first;
   total=0;
   while(iptr) {

      if(iptr->i.type == IS_MONEY && iptr->slot<0) {
	 if(iptr->i.group==MONEY_GOLD) {
	    *gold=iptr;
	    mod=1.0/valuables[MONEY_COPPER].value;

	    /* calculate total in copper coins */
	    total+=(int32u)(iptr->count*mod);
	 }

	 if(iptr->i.group==MONEY_SILVER) {
	    *silver=iptr;
	    mod=1.0/valuables[MONEY_SILVER].value;

	    /* calculate total in copper coins */
	    total+=(int32u)(iptr->count*mod);
	 }

	 if(iptr->i.group==MONEY_COPPER) {
	    *copper=iptr;

	    total+=iptr->count;
	 }
      }

      iptr=iptr->next;
   }

   /* return total in copper coins */
   return (int32u)total;
}

bool inv_paymoney(Tinventory *inv, Tinventory *tinv, _Tequipslot *equip,
		  int32u copperneed, bool checkonly)
{
   Tinvpointer gold, silver, copper;

//     Tinvpointer newnode;

   int32u copperamount;
   int32u igold, isilver, icopper;

   real mod, imod;

   copperamount=inv_countmoney(inv, &gold, &silver, &copper);

   if(copperamount<copperneed)
      return false;

   if(checkonly)
      return true;

   /* substract the money */
   copperamount-=copperneed;

   mod=valuables[MONEY_COPPER].value;	// .125
   imod=1.0/mod;								// 8

   igold=(int32u)(copperamount / imod);
   copperamount-=(int32u)(igold * imod);

   mod=valuables[MONEY_SILVER].value;	// .25
   imod=1.0/mod;								// 4

   isilver=(int32u)(copperamount / imod);
   copperamount-=(int32u)(isilver * imod);
   icopper=copperamount;

   /* remove OLD money first */
   if(gold)
      inv_removeitem(inv, equip, gold, -1);
   if(silver)
      inv_removeitem(inv, equip, silver, -1);
   if(copper)
      inv_removeitem(inv, equip, copper, -1);

   /* update new gold amount on source inventory */
   if(igold>0) {
     inv_additem(inv, IS_MONEY, MONEY_GOLD, igold, -1);
   }

   if(isilver>0) {
      inv_additem(inv, IS_MONEY, MONEY_SILVER, isilver, -1);
   }

   if(icopper>0) {
      inv_additem(inv, IS_MONEY, MONEY_COPPER, icopper, -1);

   }
   /* now build destination inventory */

   if(!tinv)
      return false;

   mod=valuables[MONEY_COPPER].value;	// .125
   imod=1.0/mod;			// 8

   igold=(int32u)(copperneed / imod);
   copperneed-=(int32u)(igold * imod);

   mod=valuables[MONEY_SILVER].value;	// .25
   imod=1.0/mod;			// 4

   isilver=(int32u)(copperneed / imod);
   copperneed-=(int32u)(isilver * imod);
   icopper=copperneed;

   if(igold) {
      inv_additem(tinv, IS_MONEY, MONEY_GOLD, igold, -1);

   }
   if(isilver) {
      inv_additem(tinv, IS_MONEY, MONEY_SILVER, isilver, -1);

   }
   if(icopper) {
      inv_additem(tinv, IS_MONEY, MONEY_COPPER, icopper, -1);

   }

   /* all ok */

   return true;
}
#endif

/*
 * Remove UNPAID flag from every item inside inventory 'inv' 
 *
 * Used when a creature commits a succesfull steal
 * (Get out of the shop with unpaid items)
 *
 */
void inv_payall(Tinventory *inv)
{
   Tinvpointer ptr1;

   ptr1=inv->first;

   while(ptr1) {
      if(ptr1->i.status & ITEM_UNPAID)
	 ptr1->i.status ^= ITEM_UNPAID;

      ptr1=ptr1->next;
   }

}

/*
 * Transfer item from src inventory to dest inventory
 *
 */
Tinvpointer inv_transfer2inv(level_type *level,
		      Tinventory *src, _Tequipslot *srceq, Tinventory *dest, 
		      Tinvpointer node, int32s count, int16s x, int16s y)
{
   Tinvpointer litem;

   if(src == dest ) {
      msg.newmsg("Error: inv_transfer2inv() trying to move items from same"
		 " source and destination inventory!", CHB_RED);
      return NULL;
   }

   if(src == NULL || dest == NULL) {
      msg.newmsg("Error: inv_transfer2inv() got NULL source or destination"
		 " inventory pointer.", CHB_RED);
      return NULL;
   }

   /* if count < 0 transfer all items */
   if(count < 0)
      count = node->count;

   /* init new level item */
   litem=inv_createitem(dest, &node->i, count);

   if(!litem) {
      return NULL;
   }

   if(level) {
      if(src == &level->inv)
	 srceq = NULL; // levels have no equipment slots

      /* set coords and item data */
      if(dest == &level->inv) {
	 litem->x=x;
	 litem->y=y;
	 litem->slot = -1;
      }
      else {
	 litem->x=0;
	 litem->y=0;
      }
   }

   /* finally remove the item(s) */
   inv_removeitem(src, srceq, node, count);

   return litem;
}


bool inv_dropall(Tinventory *inventory, _Tequipslot *equip, 
		 level_type *level, int16s x, int16s y)
{
   /* drop all items */
   while(inventory->first != NULL) {

      if(!inv_transfer2inv(level, inventory, equip, &level->inv, 
			   inventory->first, -1, x, y))
      {
	 msg.newmsg("Error: inv_dropall() failed!", CHB_RED);
	 return false;
      }
   }

   /* init the inventory */
   inv_init(inventory, equip);
   return true;

/*
   Tinvpointer iptr, dptr;
   item_info *litem;

   iptr=inventory->first;

   while(iptr) {
      litem=level_inititem(level);
      if(!litem) {
	 return false;
      }
      litem->x=x;
      litem->y=y;
      litem->count=iptr->count;
      litem->i=iptr->i;

      dptr=iptr;
      iptr=iptr->next;

      inv_removeitem(inventory, equip, dptr, -1);
   }
*/

}

_playerptrlistdef *inv_list_changecategory(Tinventory *inv,
					   _playerptrlistdef *oldlist, 
					   int16s select, int16s x, int16s y)
{
   Titemlistptr newlist;

   int16s filter, i;
   bool change;
   filter=-1;
   i=0;

   change=false;
   while(gategories[i].out) {
      if(gategories[i].out==select) {
	 filter=i;
	 change=true;
	 break;
      }
      i++;
   }
   if(select=='.') {
      filter=-1;
      change=true;
   }

   if(change) {
      newlist = inv_builditemarray(inv, filter, x, y);

      if(oldlist && newlist ) {
	 delete [] oldlist;
      }
      oldlist = newlist;

      return oldlist;
   }

   /* if no change then return the original list */
   return NULL;
}

int32u iar_cw(_playerptrlistdef *lst)
{
   int32u tw = 0;

   if(!lst)
      return 0;

   while(lst->ptr != NULL) {
      tw += (item_calcweight(&lst->ptr->i) * lst->ptr->count);

      lst++;
   }
   return tw;
}

void selectionheader(char *header, int16s ch_filter)
{
   int16u i=0;
   int16s usefilter=-1;

   while(gategories[i].out) {
      if(gategories[i].out==ch_filter) {
	 usefilter = i;
	 break;
      }
      i++;
   }

   my_clrscr();
   my_setcolor(CH_WHITE);
   my_cputs(1, header);
   my_setcolor(CH_DGRAY);
   drawline(2, '=');

   if(usefilter >= 0) {
      my_gotoxy(SCREEN_COLS - strlen(gategories[usefilter].name) - 4, 2);

      my_setcolor(C_WHITE);
      my_printf("[ %s ]", gategories[usefilter].name);
   }
   else {
      my_gotoxy(SCREEN_COLS-15, 2);
      my_setcolor(C_WHITE);
      my_printf("[ all items ]");
   }

   my_gotoxy(1, 3);
}

void selectionprompt(int32u weight)
{
   int16u i=0;

   for(i=0; i<NUMOFITEMGROUPS; i++) {
      tempstr[i]=gategories[i].out;
   }
   tempstr[i]=0;

   sprintf(i_hugetmp, "Select item group with [ %s or . for all ]", tempstr);

   my_setcolor(CH_DGRAY);
   drawline(SCREEN_LINES-3, '=');
   my_gotoxy(55, SCREEN_LINES-3);
   my_setcolor(C_YELLOW);
   if(weight>0)
      my_printf("[ Total of %4.2fkg ]", (real)(weight)/1000);

   my_setcolor(C_WHITE);
   clearline(SCREEN_LINES-2);
   clearline(SCREEN_LINES-1);
   clearline(SCREEN_LINES);

   my_cputs(SCREEN_LINES-2, i_hugetmp);
   my_setcolor(C_GREEN);
   my_cputs(SCREEN_LINES, "To get help with this screen, press CTRL+I.");
/*
   my_cputs(SCREEN_LINES-1, 
	    "[a-z Select] [A-Z Toggle] [1 open container] [2 move selected]");
   my_cputs(SCREEN_LINES,
	    "[2,3,PGDN,DOWN move down] [8,9,PGUP,UP move up] [SPC/ESC close]");
*/
}

/*
 * Wrapper for sinle selection inventory browsing!
 */
Tinvpointer inv_listitems(Tinventory *inv, char *prompt, int16s filter, 
			bool resmode, int16s x, int16s y )
{
   Titemlistptr listptr;
   Tinvpointer selected_item = NULL;

   /* call multi selector */
   listptr = inv_listitems_multi(inv, prompt, filter, resmode, false, x, y);

   /* if multilister returns NULL pointer -> no selection was made */
   if(!listptr) {
      return NULL;
   }

   /* selected item must be the first indexed pointer */
   selected_item = listptr[0].ptr;

   /* free multiselection array */
   inv_freemultilist(&listptr);

   return selected_item;
}

Titemlistptr inv_buildmultilist(Titemlistptr ptrlist)
{
   Titemlistptr newlist = NULL;
   int32u j, i, itemcount;

   /* multiselect, return a list of selected items!
    *
    * must be FREED BY THE CALLER!
    */
   itemcount=0;

   /* count selected items */
   i = 0;
   while( (ptrlist+i)->ptr != NULL) {
      if((ptrlist+i)->sel != 0)
	 itemcount++;	 
      i++;
   }

   /* if no items selected */
   if(!itemcount)
      return NULL;

   newlist = (Titemlistptr)malloc((itemcount + 1) * 
				  sizeof(_playerptrlistdef));
   if(!newlist) {
      return NULL;
   }
   else {
      /* collect selected items */
      i = 0;
      j = 0;
      while(i<itemcount) {
	 if((ptrlist+j)->sel != 0) {
	    (newlist+i)->ptr = (ptrlist+j)->ptr;
	    i++;
	 }
	 j++;
      }
      (newlist+i)->ptr = NULL;
   }

   return newlist;
}

void inv_freemultilist(Titemlistptr *multilist)
{
   if(!multilist)
      return;

   if(!(*multilist))
      return;

   free((*multilist));
   *multilist = NULL;
}

char text_defaultinv[] = "inventory of a ";
const char text_levelinv[] = " (Ground)";
const char text_playerinv[] = " (Backpack)";

Titemlistptr inv_listitems_multi(
   Tinventory *inv, 
   char *preprompt, 
   int16s filter, 
   bool resmode, 
   bool multiselect,
   int16s x, 
   int16s y )
{
   Tinvpointer selptr, info, container;
   Titemlistptr ptrlist, newlist;
   int32u arrayweight = 0;
   int16u itemcount=0, selection, pitemcount=0;
   int16s i=0, j=0, lasttype, actfilter;
   bool selected=false, cancel=true, dynprompt=false, darklevel=false;
   char *prompt=NULL;

   if(!inv) {
      msg.newmsg("Error: Tried to access NULL inventory!", CHB_RED);
      return NULL;
   }

   if(filter < 0)
      actfilter='.';
   else
      actfilter=gategories[filter].out;

   /* pile items in the inventory */
   if(x>=0 && y>=0)
      inv_pileitems_coords(inv);
   else
      inv_pileitems(inv);

   /* first build a list of items, if no items to match filter */
   /* it will return a pointer to list full of NULLs! !!!!     */
   ptrlist=inv_builditemarray(inv, filter, x, y);

   if(!ptrlist)
      return NULL;

   arrayweight = iar_cw(ptrlist);

   if(inv==&player.inv) 
      i = my_strlen(preprompt) + my_strlen(text_playerinv) + 1;
   else if(inv==&c_level->inv) {
      i = my_strlen(preprompt) + my_strlen(text_levelinv) + 1;

      if(!player_has_los(c_level, x, y))
	 darklevel=true;
   }

   if(i>0)
      prompt = (char *) malloc(i * sizeof(char) + 1);

   if(prompt) {
      my_strcpy(prompt, preprompt, i);
      if(inv==&player.inv) 
	 my_strcat(prompt, text_playerinv, i);
      else if(inv==&c_level->inv)
	 my_strcat(prompt, text_levelinv, i);

      dynprompt = true;
   }
   else {
      prompt = text_defaultinv;
      dynprompt = false;
   }

   /* we need to do a redraw after this */
   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

   /* show screen title */
   selectionheader(prompt, actfilter);

   itemcount=0;
   lasttype=-1;
   selptr=NULL;
   i=j=0;
   while(1) {
      info=(ptrlist+i)->ptr;

      if(info) {
	 if(info->i.type != lasttype && !darklevel) {
	    lasttype=info->i.type;
	    my_setcolor(C_YELLOW);

	    my_printf("%s ('%c')\n", gategories[lasttype].name,
		      gategories[lasttype].out);
	 }
	 if(my_gety() < SCREEN_LINES-3) {

	    my_setcolor(CH_MAGENTA);
	    if((ptrlist+i)->sel!=0)
	       my_printf(" +");
	    else
	       my_printf("  ");

	    my_setcolor(C_RED);
//	 my_printf("  %c", 'A'+itemcount);
	    my_printf(" %c", 'a'+itemcount);
	    my_setcolor(C_WHITE);
	    my_printf(") ");
	    if(info->slot >= 0) {
	       my_setcolor(C_CYAN);
	       my_printf("[%s] ", equip_slotdesc[info->slot]);
	       my_setcolor(C_WHITE);
	    }
	    if(darklevel)
	       my_printf("something");
	    else
	       item_printinfo(&info->i, info->i.weight, info->count, NULL);

	    my_printf("\n");
	    itemcount++;
	    i++;
	 }
      }
      if( !info || my_gety()>=SCREEN_LINES-3 || (('a'+itemcount)>'z')) {
	 while(1) {
	    selectionprompt(arrayweight);

	    if((ptrlist+i)->ptr) {
	       my_gotoxy(1, SCREEN_LINES-3);
	       my_setcolor(CH_GREEN);
	       my_printf("[...more]");
	    }
	    if(j!=0) {
	       my_gotoxy(1, 2);
	       my_setcolor(CH_GREEN);
	       my_printf("[...more]");
	    }
	    if(!itemcount) {
	       my_gotoxy(1, 4);
	       my_printf("..Nothing...\n");
	    }
	    selection=my_getch();
	    if(filter < 0) {
	       newlist=inv_list_changecategory(inv, ptrlist, selection, x, y);
	       if(newlist!=NULL) {
		  actfilter = selection;
		  ptrlist=newlist;
		  arrayweight = iar_cw(ptrlist);

		  itemcount=i=j=0;
		  lasttype=-1;
		  selectionheader(prompt, actfilter);
		  break;
	       }
	    }
	    if(selection==MYKEY_CTRLI) {
	       if(chdir_tohelpdir()) {
		  viewfile(file_inventoryhelp);
		  itemcount=0;
		  lasttype=-1;
		  selectionheader(prompt, actfilter);
		  i=j;
		  break;
	       }
	    }
	    if(selection==13 || selection==10 ||
	       selection==PADENTER || selection==KEY_ENTER) {
	       cancel=false;
	       selected=true;
	       break;
	    }
	    else
	    if(selection==32 || selection==KEY_ESC) {
	       selected=true;
	       cancel=true;
	       selptr=NULL;
	       break;
	    }
	    else
	    /* select item commands */
	    if((selection>='a') && (selection < ('a'+itemcount))) {
	       selptr=(ptrlist + (selection-'a')+j )->ptr;

	       (ptrlist + (selection-'a')+j)->sel = 1;
	       selected=true;
	       cancel=false;
	       break;
	    }
	    /* multiselection */
	    else if((selection>='A') && (selection < ('A'+itemcount))) {
	       /* toggle selection */
	       if( (ptrlist + (selection-'A')+j)->sel == 1)
		  (ptrlist + (selection-'A')+j)->sel = 0;
	       else
		  (ptrlist + (selection-'A')+j)->sel = 1;
	       
	       cancel=false;
	       if(!multiselect) {
		  selected=true;
		  break;
	       }
	       else {
		  itemcount=0;
		  lasttype=-1;
		  selectionheader(prompt, actfilter);
		  i=j;
		  break;
	       }
	    }
	    /* move to container */
	    else if(selection==MYKEY_CTRLT && !darklevel) {
	       clearline(SCREEN_LINES);
	       clearline(SCREEN_LINES-1);
	       clearline(SCREEN_LINES-2);

	       newlist = inv_buildmultilist(ptrlist);
	       if(!newlist) {
		  my_setcolor(CH_RED);
		  my_cputs(SCREEN_LINES-1, 
			   "No items selected for transfer.");
		  my_setcolor(CHB_WHITE);
		  my_cputs(SCREEN_LINES, 
			   "[Press a key]");
		  my_getch();
		  break;
	       }

	       my_setcolor(CH_YELLOW);
	       clearline(SCREEN_LINES-1);
	       my_cputs(SCREEN_LINES-1, 
			"Move selected items, select a container"
			" [press item letter]?");
	       selection = my_getch();

	       if(!((tolower(selection)>='a') && 
		  (tolower(selection) < ('a'+itemcount)))) {
		  inv_freemultilist(&newlist);
		  break;
	       }

	       container=(ptrlist + (tolower(selection)-'a')+j )->ptr;

	       if(container->i.inv != NULL) {
		  i = 0;

		  /* transfer all selected items to the container */
		  while(newlist[i].ptr!=NULL) {
		     if(container == newlist[i].ptr) {
			clearline(SCREEN_LINES-1);
			my_cputs(SCREEN_LINES-1, 
				 "Can't store container inside itself. [KEY]");
			my_getch();
		     }
		     else if( newlist[i].ptr->slot>=0) {
			clearline(SCREEN_LINES-1);
			my_cputs(SCREEN_LINES-1, 
				 "Can't move equipped items. [KEY]");
			my_getch();
		     }
		     else {
			inv_transfer2inv(c_level, inv, NULL, container->i.inv,
					 newlist[i].ptr, -1, 
					 0, 0);
		     }
		     i++;
		  }
		  inv_freemultilist(&newlist);

		  if(x>=0 && y>=0)
		     inv_pileitems_coords(inv);
		  else
		     inv_pileitems(inv);

//		  if(filter<0)
		  newlist=inv_list_changecategory(inv, ptrlist, 
						  actfilter, x, y);
//		  else
//		     newlist=inv_list_changecategory(inv, ptrlist, 
//						     gategories[filter].out,
//						     x, y);

		  if(newlist!=NULL) {
		     ptrlist=newlist;
		     arrayweight = iar_cw(ptrlist);
		  }


	       }
	       else {
		  inv_freemultilist(&newlist);
		  clearline(SCREEN_LINES-1);
		  my_setcolor(CH_RED);
		  my_cputs(SCREEN_LINES-1, 
			   "That item can't contain anything.");
		  my_setcolor(CHB_WHITE);
		  my_cputs(SCREEN_LINES, 
			   "[Press a key]");
		  my_getch();
	       }

	       itemcount=0;
	       lasttype=-1;
	       selectionheader(prompt, actfilter);
	       i=0;
	       j=0;
	       break;
	    }
	    /* open container commands */
//	    else if((selection>='A') && (selection < ('A'+itemcount))) {
	    else if(selection==MYKEY_CTRLO && !darklevel) {
	       my_setcolor(CH_YELLOW);
	       clearline(SCREEN_LINES);
	       clearline(SCREEN_LINES-1);
	       clearline(SCREEN_LINES-2);
	       my_cputs(SCREEN_LINES-1, 
			"Open which container [press item letter]?");
	       selection = my_getch();

	       if(!((tolower(selection)>='a') && 
		  (tolower(selection) < ('a'+itemcount)))) {
		  break;
	       }

	       selptr=(ptrlist + (tolower(selection)-'a')+j )->ptr;

	       if(selptr->i.inv != NULL) {
		  container = selptr;

		  char * newprompt=NULL;

		  newprompt = (char *)malloc(sizeof(char) * 
					     (my_strlen(container->i.name) + 
					      my_strlen(text_defaultinv) + 1));
		  if(newprompt) {
		     my_strcpy(newprompt, text_defaultinv,
			       (my_strlen(container->i.name) + 
				my_strlen(text_defaultinv) + 1));
		     my_strcat(newprompt, container->i.name,
			       (my_strlen(container->i.name) + 
				my_strlen(text_defaultinv) + 1));

		     newlist = inv_listitems_multi(container->i.inv, 
						   newprompt, 
						  filter, false, true, -1, -1);
		     free(newprompt);
		  }
		  else {
		     newlist = inv_listitems_multi(container->i.inv, 
						   "Item inventory", 
						  filter, false, true, -1, -1);

		  }

		  if(newlist) {
		     i=0;

		     /* transfer all selected items to upper container */
		     while(newlist[i].ptr!=NULL) {
			inv_transfer2inv(c_level, container->i.inv, NULL, inv,
					 newlist[i].ptr, -1, 
					 container->x, container->y);
			i++;
		     }
		     
		     inv_freemultilist(&newlist);

		     if(x>=0 && y>=0)
			inv_pileitems_coords(inv);
		     else
			inv_pileitems(inv);

//		     if(filter<0)
//			newlist=inv_list_changecategory(inv, ptrlist, 
//							'.', x, y);
//		     else
		     newlist=inv_list_changecategory(inv, ptrlist, 
						     actfilter,
						     x, y);
		     if(newlist!=NULL) {
			ptrlist=newlist;
			arrayweight = iar_cw(ptrlist);
		     }

		  }
		  itemcount=0;
		  lasttype=-1;
		  selectionheader(prompt, actfilter);
		  i=0;
		  j=0;
		  break;
	       }
	       else {
		  clearline(SCREEN_LINES-1);
		  my_setcolor(CH_RED);
		  my_cputs(SCREEN_LINES-1, 
			   "It doesn't contain anything.");
		  my_setcolor(CHB_WHITE);
		  my_cputs(SCREEN_LINES, 
			   "[Press a key]");
		  my_getch();
	       }
	    }
	    else if( (selection=='3' || selection==KEY_NPAGE ||
		      selection=='2' || selection==KEY_DOWN) 
		     && (ptrlist+i)->ptr) {
	       pitemcount=itemcount;
	       selectionheader(prompt, actfilter);
	       itemcount=0;
	       lasttype=-1;
	       if(selection=='2' || selection == KEY_DOWN )
		  j=j+1;
	       else
		  j=i;

	       i=j;
	       break;
	    }
	    else if(selection=='9' || selection==KEY_PPAGE ||
		    selection=='8' || selection==KEY_UP) {
	       selectionheader(prompt, actfilter);
//	       i=j-SCREEN_LINES-4;
	       if(selection == '9' || selection==KEY_PPAGE)
		  i=j-pitemcount;
	       else
		  i=j-1;
	       j=i;
	       itemcount=0;
	       lasttype=-1;
	       if(i<0) {
		  i = j = 0;
	       }
	       break;
	    }
	 }
      }
      if(selected) break;
   }

   if(!cancel)
      newlist = inv_buildmultilist(ptrlist);
   else
      newlist = NULL;

   // free temp buffer
   delete [] ptrlist;

   if(dynprompt)
      free(prompt);

   /* return the multiselection list */
   return newlist;

}

/*
void inv_returnitempointer(Tinventory *inv, int16s x, int16s y, int16s type)
{
   if( inv_countitems(inv, type, x, y, false) == 1) {
      return giveiteminfo

   }


}
*/

Tinventory *inv_selectsource(Tinventory *s1, Tinventory *s2, 
			     int16s s2_x, int16s s2_y, int16s type)
{
   int32u s1_count=0, s2_count=0;

   if(!s1 && !s2)
      return NULL;
   if(!s1)
      return s2;
   if(!s2)
      return s1;

   /* check if s1 contains items of 'type', recursively */
   s1_count = inv_countitems(s1, type, -1, -1, true);
   
   /* check if s2 (ground) contains items of 'type', recursively */
   s2_count = inv_countitems(s2, type, s2_x, s2_y, true);

   if(s1_count && s2_count) {
      if( confirm_yn("Do you want to browse the ground (y) or your "
		     "inventory (n)", true, true)) 
	 return s2;
      else
	 return s1;
   }

   if(s1_count)
      return s1;
   if(s2_count)
      return s2;

   /* else return NULL */
   return NULL;
}

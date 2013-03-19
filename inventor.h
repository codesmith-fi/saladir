/* inventory.h

*/
#ifndef _INVENTORY_H_DEFINED
#define _INVENTORY_H_DEFINED

#include "saladir.h"

#include "output.h"
#include "types.h"
#include "defines.h"

#include "generate.h"

Tinvpointer inv_inititem(Tinventory *inventory);
Tinvpointer inv_createitem(Tinventory *inv, item_def *item, int32u count);

int16s inv_removeitem( Tinventory *inv, _Tequipslot *equip, 
		       Tinvpointer remthis, int32s count );
//bool inv_removeequipped( Tinventory *inv, int16s slot);

void inv_clearequipslot(Tinventory *inv, _Tequipslot *equip, int16u slot);

Tinvpointer inv_splitpile(Tinventory *inv, Tinvpointer pile);

void inv_removeall(Tinventory *inv, _Tequipslot *equip);
void inv_pileitems(Tinventory *inv);
void inv_pileitems_coords(Tinventory *inv);
bool inv_paymoney(Tinventory *inv, Tinventory *tinv, _Tequipslot *equip,
		  int32u copperneed, bool checkonly);
//int32u inv_countmoney(Tinventory *inv, Tinvpointer *gold, 
//		      Tinvpointer *silver, Tinvpointer *copper);
int32u inv_moneyamount(Tinventory *inv);

bool inv_dropall(Tinventory *inventory, _Tequipslot *equip, 
		 level_type *level, int16s x, int16s y);
Tinvpointer inv_transfer2inv(level_type *level,
		      Tinventory *src, _Tequipslot *srceq, Tinventory *dest, 
		      Tinvpointer node, int32s count, int16s x, int16s y);

void inv_payall(Tinventory *inv);
void inv_init(Tinventory *inv, _Tequipslot *equip);

Tinvpointer level_createitem(level_type *level, int16s x, int16s y,
			     int16u group, int16s type, int16u count,
			     int16s material);
Tinvpointer inv_additem(Tinventory *inventory, int16u group, int16s type, 
		 int16u count, int16s material);
Tinvpointer inv_listitems(Tinventory *inv, char *prompt, int16s filter, 
			bool resmode, int16s x, int16s y);

void inv_freemultilist(Titemlistptr *multilist);
Titemlistptr inv_listitems_multi(Tinventory *inv, 
			       char *prompt, int16s filter, 
			       bool resmode, bool multiselect, 
			       int16s x, int16s y);

int32u inv_calcweight(Tinventory *inv);
int32u item_calcweight(item_def *item);

//int32u inv_countitems(Tinventory *inv, int16s x, int16s y);
int32u inv_countitems(Tinventory *inv, int16s type, 
		      int16s x, int16s y, bool recursive);
Tinventory *inv_selectsource(Tinventory *s1, Tinventory *s2, 
			     int16s s2_x, int16s s2_y, int16s type);


#endif

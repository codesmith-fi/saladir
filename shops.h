/* shops.h for saladir*/

#ifndef _SHOPS_H_DEFINED
#define _SHOPS_H_DEFINED

#include "saladir.h"
#include "types.h"
#include "defines.h"
#include "caves.h"
#include "creature.h"
#include "inventor.h"
#include "monster.h"
#include "birth.h"
#include "raiselev.h"
#include "player.h"
#include "classes.h"

/* defines how far the shopkeeper will chase its targets 
 * (a distance to the shop door!
 */
#define SHOPKEEPER_MAXDOORDIST 15

bool shop_addshopkeeper(level_type *level, int8u roomnum);
void shop_equip(level_type *level, int8u roomnum);
void shop_init(level_type *level);
//void shopkeeper_greet(level_type *level, int8u roomnum, bool direction);
void shopkeeper_greet(level_type *level, _monsterlist *owner);

void shopkeeper_clean(level_type *level, _monsterlist *remove);
void shopkeeper_coppervalue(int32u copper, int32u *pgold, 
			    int32u *psilver, int32u *pcopper);
bool shopkeeper_buy(level_type *level, _monsterlist *keeper, Tinvpointer item);
int16s shopkeeper_move(level_type *level, _monsterlist *keeper);

#endif

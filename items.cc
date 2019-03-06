/* items.cpp (C) Erno Tuomainen 1997
**
**	item list
**
*/

#include "items.h"
#include "output.h"
#include "magic.h"


_matlist materials[]=
{
     { "leather", 0, 0, 0, 0, CH_DGRAY, 
       MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS | MATSTAT_NOTWEAPON,
       0, 10, 600, 0.8, 0.4 },   // armors only
     { "chain", 1, 0, 0, 0, C_WHITE, MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS | MATSTAT_NOTWEAPON,
       0, 30, 600, 1.0, 0.5 },   // armors only
     { "wooden", -2, 0, 0, 0, CH_DGRAY,   MATSTAT_NOTARMOR,
       0, 10, 500, 0.7, 0.2 },
     { "glass", 			0, 0, 0, 0, CH_CYAN,    MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS|MATSTAT_NOTARMOR| MATSTAT_CANSHARD|MATSTAT_RING,
       0, 4, 680, 0.5, 1.0 },
     { "copper", 0, 0, 0, 0, C_YELLOW,   MATSTAT_NOTBOWS|MATSTAT_RING,
       0, 100, 500, 1.0, 0.5 },
     { "iron", 			0, 0, 0, 0, C_WHITE, MATSTAT_NOTBOWS|MATSTAT_RING,
       0, 150, 500, 1.0, 0.8 },
     { "steel", 0, 0,  0, 0, C_WHITE,   MATSTAT_NOTBOWS|MATSTAT_RING,
       0, 200, 500, 1.1, 0.8 },
     { "stone",        1, 0, 0, 0, CH_DGRAY,   MATSTAT_NOTMISSILE|MATSTAT_NOTARMOR|MATSTAT_NOTBOWS| MATSTAT_CANSHARD,
       0, 5, 500, 3.0, 0.4 },
     { "silver", 1, 0, 0, 0, CH_WHITE,   MATSTAT_NOTBOWS|MATSTAT_RING,
       0, 80, 700, 0.9, 1.1 },
     { "golden", 1, 0, 0, 0, CH_YELLOW,  MATSTAT_NOTBOWS|MATSTAT_RING,
       0, 70, 750, 2.0, 2.2 },
     { "crystal",		2, 0, 0, 0, CH_BLUE, MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS|MATSTAT_NOTARMOR|MATSTAT_RING,
       0, 100, 740, 1.0, 1.1 },
     { "elven", 3, 0, 0, 0, CH_WHITE, MATSTAT_RING,
       0, 200, 700, 1.0, 1.2 },
     { "dwarven", 		3, 0, 0, 0, CH_BLUE, MATSTAT_RING,
       0, 200, 750, 1.5, 1.3 },
     { "orcish", 		4, 0, 0, 0, CH_GREEN, MATSTAT_RING,
       0, 200, 750, 2.5, 1.4 },
     { "mithril",		4, 0, 0, 0, CH_YELLOW, MATSTAT_NOTBOWS|MATSTAT_RING,
       0, 220, 760, 2.0, 1.5 },
     { "platinium",		5, 0, 0, 0, CH_CYAN,    MATSTAT_NOTBOWS|MATSTAT_RING,
       0, 220, 770, 2.0, 1.6 },
     { "ebony", 1, 0, 0, 0, CH_WHITE,   MATSTAT_NOTARMOR,
       0, 20, 750, 2.0, 1.0 },
     { "diamond", 	   7, 0, 0, 0, CH_WHITE,   MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS|MATSTAT_NOTARMOR|MATSTAT_RING,
       0,  300, 800, 3.0, 1.6 },
     { "adamantium", 	8, 0, 0, 0, CH_MAGENTA, MATSTAT_RING,
       0,  400, 800, 3.0, 1.8 },
     { "white dragonscale", 10, 0, 0, 0, CH_WHITE,   MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
       0, 4000, 800, 3.0, 2.5 },   // dragon materials
     { "red dragonscale",   10, 0, 0, 0, CH_RED,     MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
       0, 4000, 800, 3.0, 2.5 },
     { "green dragonscale", 10, 0, 0, 0, CH_GREEN,   MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
       0, 4000, 800, 3.0, 2.5 },
     { "blue dragonscale",  10, 0, 0, 0, CH_BLUE,    MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
       0, 4000, 800, 3.0, 2.5 },
     { "black dragonscale", 10, 0, 0, 0, CH_DGRAY,   MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
       0, 4000, 800, 3.0, 2.5 },

     { "cloth", 0, 0, 0, 0, CH_WHITE, MATSTAT_SPECIAL,
       0, 2000, 600, 1.0, 1.0 },   // cloth items
     { "papyrus", 0, 0, 0, 0, CH_WHITE, MATSTAT_SPECIAL,
       0, 2000, 600, 1.0, 1.0 },   // cloth items

     { NULL, 0 }
};


char *condition[]=
{
     "new",
     "almost new",
     "used",
     "worn",
     "almost broken",
     "broken",
     NULL
};

char *food_condition[]=
{
     "new",
     "good",
     "dubious"
     "stinking"
     "tainted"
     "rotten",
     NULL
};

char *light_condition[]=
{
     "new",
     "",
     ""
     "burning"
     "nearly dead"
     "burned out",
     NULL
};

/* Weapon list for Saladir
**
** Note that these are only item TEMPLATES. When item is generated to
** level or monster it is randomized in several ways
*/


item_def weapons[]=
{
     { "dagger", "", "", 0, IS_WEAPON1H, WPN_DAGGER,
       1000, 0, 0, 0, 200, NEUTRAL, 1, 4, 1, 4, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "tanto", "", "", 0, IS_WEAPON1H, WPN_DAGGER,
       1000, 0, 0, 0, 500, NEUTRAL, 2, 4, 2, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "staff", "", "", 0, IS_WEAPON2H, WPN_STAFF,
       1500, ITEM_ENCHANTED, 0, 0, 500, NEUTRAL, 1, 8, 1, 2, 0, 0, 
       -1, 0, 0, 0, MAT_IRON, 0, 0, 0,
       { 0, 
	 0,
	 { 20, 0, 0, 0, 0, 0, 0 }, /* resistances */
	 4, 15, 0, 0, 0, 0, 0, 0, 5,
	 0, 0, 0, 0
       }
     },
     { "small axe", "", "", 0, IS_WEAPON1H, WPN_AXE,
       2000, 0, 0, 0, 2000, NEUTRAL, 2, 3, 1, 4, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "short bow", "", "", 0, IS_MISWEAPON, WPN_BOW,
       1000, 0, 0, 0, 500, NEUTRAL, 1, 2, 1, 2, 0, 2, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "short sword", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       1700, 0, 0, 0, 2000, NEUTRAL, 2, 4, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "wakizashi", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       1700, 0, 0, 0, 1000, NEUTRAL, 3, 4, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "broadsword", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       2000, 0, 0, 0, 1000, NEUTRAL, 4, 3, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "mace", "", "", 0, IS_WEAPON1H, WPN_BLUNT,
       1600, 0, 0, 0, 1500, NEUTRAL, 3, 4, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "long bow", "", "", 0, IS_MISWEAPON, WPN_BOW,
       2000, 0, 0, 0, 1000, NEUTRAL, 1, 2, 1, 2, 0, 4, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "war axe", "", "", 0, IS_WEAPON2H, WPN_AXE,
       3000, 0, 0, 0, 3000, NEUTRAL, 3, 4, 2, 2, 1, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "saber", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       2700, 0, 0, 0, 3000, NEUTRAL, 4, 4, 2, 2, 0, 2, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "morningstar", "", "", 0, IS_WEAPON2H, WPN_BLUNT,
       3500, 0, 0, 0, 3000, NEUTRAL, 4, 4, 3, 2, 1, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "crossbow", "", "", 0, IS_MISWEAPON, WPN_CROSSBOW,
       3500, 0, 0, 0, 1000, NEUTRAL, 1, 2, 1, 2, 0, 6, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "long sword", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       4000, 0, 0, 0, 3000, NEUTRAL, 3, 6, 2, 1, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "battle axe", "", "", 0, IS_WEAPON2H, WPN_AXE,
       4000, 0, 0, 0, 3000, NEUTRAL, 3, 6, 2, 2, 1, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "katana", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       4200, 0, 0, 0, 3000, NEUTRAL, 3, 6, 1, 2, 2, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "claymore", "", "", 0, IS_WEAPON2H, WPN_SWORD,
       4500, 0, 0, 0, 3000, NEUTRAL, 5, 4, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "warhammer", "", "", 0, IS_WEAPON2H, WPN_BLUNT,
       5000, 0, 0, 0, 3000, NEUTRAL, 5, 4, 3, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "dai-katana", "", "", 0, IS_WEAPON2H, WPN_SWORD,
       8000, 0, 0, 0, 3000, NEUTRAL, 4, 6, 2, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },

     /* missiles */
     { "arrow", "", "", 0, IS_MISSILE, WPN_MISSILE_1,
       8, 0, 0, 0, 20, NEUTRAL, 1, 2, 2, 4, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "bolt", "", "", 0, IS_MISSILE, WPN_MISSILE_2,
       16, 0, 0, 0, 40, NEUTRAL, 1, 2, 3, 3, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "rock", "", "", 0, IS_MISSILE, WPN_MISSILE_3,
       4, 0, 0, 0, 250, NEUTRAL, 1, 2, 2, 2, 0, 0, -1, 0, 0, 0, MAT_STONE, 0, 0, 0 },

     /* end of list */
     { "", "", "" } /* end of the list */
};

/* armor templates */
item_def armor[]=
{
     { "helmet", "", "", 0, IS_ARMOR, ARM_HELMET,
       1000, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON },
     { "left pauldron", "", "", 0, IS_ARMOR, ARM_LHANDARM,
       1500, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON },
     { "right pauldron", "", "", 0, IS_ARMOR, ARM_RHANDARM,
       1500, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON },
     { "cuirass", "", "", 0, IS_ARMOR, ARM_BODY,
       2000, 0, 0, 0, 2000, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON },
     { "gauntlets", "", "", 0, IS_ARMOR, ARM_GLOVES,
       500, 0, 0, 0, 200, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON },
     { "greaves", "", "", 0, IS_ARMOR, ARM_LEGARM,
       1000, 0, 0, 0, 1000, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON },
     { "boots", "", "", 0, IS_ARMOR, ARM_BOOTS,
       500, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON },
     { "left heavy pauldron", "", "", 0, IS_ARMOR, ARM_LHANDARM,
       2000, 0, 0, 0, 1400, NEUTRAL,
       1, 2, 1, 2, -2, 0,
       -1, 0, 3, 0, MAT_IRON },
     { "right heavy pauldron", "", "", 0, IS_ARMOR, ARM_RHANDARM,
       2000, 0, 0, 0, 1400, NEUTRAL,
       1, 2, 1, 2, -2, 0,
       -1, 0, 3, 0, MAT_IRON },
     { "crown", "", "", 0, IS_ARMOR, ARM_HELMET,
       600, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 0, 0, MAT_IRON },
     { "heavy cuirass", "", "", 0, IS_ARMOR, ARM_BODY,
       3000, 0, 0, 0, 4000, NEUTRAL,
       1, 2, 1, 2, -2, 0,
       -1, 0, 3, 0, MAT_IRON },
     { "spiked gauntlets", "", "", 0, IS_ARMOR, ARM_GLOVES,
       800, 0, 0, 0, 200, NEUTRAL,
       1, 2, 3, 2, 0, 0,
       -1, 0, 2, 0, MAT_IRON },
     { "heavy greaves", "", "", 0, IS_ARMOR, ARM_LEGARM,
       1500, 0, 0, 0, 2400, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 3, 0, MAT_IRON },
     { "running shoes", "Shoes of haste", "", 0, IS_ARMOR, ARM_BOOTS,
       1000, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 2, 0, 0, MAT_IRON },
     { "buckler", "", "", 0, IS_SHIELD, ARM_SHIELD,
       200, 0, 0, 0, 500, NEUTRAL,
       1, 2, 2, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON, 0, 0, 0 },
     { "small shield", "", "", 0, IS_SHIELD, ARM_SHIELD,
       500, 0, 0, 0, 800, NEUTRAL,
       1, 1, 2, 2, 0, 0,
       -1, 0, 2, 0, MAT_IRON, 0, 0, 0 },
     { "medium shield", "", "", 0, IS_SHIELD, ARM_SHIELD,
       1000, 0, 0, 0, 1200, NEUTRAL,
       1, 1, 2, 2, 0, 0,
       -1, 0, 3, 0, MAT_IRON, 0, 0, 0 },
     { "large shield", "", "", 0, IS_SHIELD, ARM_SHIELD,
       1500, 0, 0, 0, 2000, NEUTRAL,
       1, 1, 2, 2, 0, 0,
       -1, 0, 4, 0, MAT_IRON, 0, 0, 0 },

     { "cloak", "", "", 0, IS_ARMOR, ARM_CLOAK,
       200, 0, 0, 0, 400, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, C_WHITE },
     { "hooded cloak", "", "", 0, IS_ARMOR, ARM_CLOAK,
       220, 0, 0, 0, 500, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, CH_DGRAY },
     { "shirt", "", "", 0, IS_ARMOR, ARM_SHIRT,
       200, 0, 0, 0, 400, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, CH_RED },
     { "pants", "", "", 0, IS_ARMOR, ARM_PANTS,
       200, 0, 0, 0, 700, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, CH_BLUE },
     { "sandals", "", "", 0, IS_ARMOR, ARM_BOOTS,
       200, 0, 0, 0, 500, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, CH_BLUE },

     { "", "", "" } /* end of the list */
};

/* 
 * template for scroll item, when a scroll is created
 * the data is first copied from here to initialize the item 
 *
 * Most important "fields" for scroll item are:
 * 'type'  = indicates that the item is a scroll
 * 'group' = indicates the group of scroll (ie. SCROLLGROUP_MAGIC)
 * 'pmod1' = scroll "effect" or spell 
 * 'pmod2' = the recorded skill value for a spell
 *
 */
item_def templ_scroll=
{
   "scroll", "", "", 0, IS_SCROLL, 0,
   800, 0, 0, 0, 100, NEUTRAL, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, MAT_PAPYRUS,
   0, 0, CH_WHITE,
};


item_def SPECIAL_ITEMS[]=
{
   /* rocks */
   { "large stone boulder", "", "", 0, IS_SPECIAL, SPECIAL_BOULDER,
     10, ITEM_NOTPASSABLE, 0, 0, 800000, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 0, 0, 0, MAT_STONE, 0, 0, 0 },
   { "large rock", "", "", 0, IS_SPECIAL, SPECIAL_ROCK,
     10, ITEM_NOTPASSABLE, 0, 0, 450000, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 0, 0, 0, MAT_STONE, 0, 0, 0 },

/*
   { "large beehive", "", "", 0, IS_SPECIAL, SPEC_BEEHIVE,
     100, 0, 0, 0, 1000000, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     -1, 0, 0, 0, MAT_FABRIC, 0, 'O', CH_CYAN },
*/
   { "", "", "" } /* end of the list */

};

item_def miscitems[]=
{
   /* lanterns */
   { "small torch", "", "", 0, IS_LIGHT, LIGHT_SMALL,
     200, 0, 0, 0, 1200, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     400000, 3, 0, 0, MAT_WOOD, 0, 0, 0 },
   { "torch", "", "", 0, IS_LIGHT, LIGHT_MEDIUM,
     300, 0, 0, 0, 1800, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     600000, 5, 0, 0, MAT_WOOD, 0, 0, 0 },
   { "large torch", "", "", 0, IS_LIGHT, LIGHT_LANTERN,
     400, 0, 0, 0, 2500, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     600000, 7, 0, 0, MAT_WOOD, 0, 0, 0 },
   { "lantern", "", "", 0, IS_LIGHT, LIGHT_LANTERN,
     1000, 0, 0, 0, 1200, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     800000, 10, 0, 0, MAT_WOOD, 0, 0, 0 },

   /* food items here
    *  ! food items have pmod1 as a percentage multiplier for
    *    food nutrition. 100 is normal
    */
   { "small ration", "", "", 0, IS_FOOD, FOOD_RATION,
     600, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 100, 0, 0, MAT_GOLD, 0, 0, 0 },
   { "melon", "", "", 0, IS_FOOD, FOOD_MELON,
     600, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 110, 0, 0, MAT_ORCISH, 0, 0, 0 }, 
   { "large bread", "", "", 0, IS_FOOD, FOOD_RATION,
     800, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 120, 0, 0, MAT_GOLD, 0, 0, 0 },
   { "cream pie", "", "", 0, IS_FOOD, FOOD_RATION,
     600, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 100, 0, 0, MAT_SILVER, 0, 0, 0 },
   { "carrot", "", "", 0, IS_FOOD, FOOD_RATION,
     500, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 150, 0, 0, MAT_GOLD, 0, 0, 0 },
   { "large ration", "", "", 0, IS_FOOD, FOOD_RATION,
     1000, 0, 0, 0, 5000, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 100, 0, 0, MAT_GOLD, 0, 0, 0 },
   { "iron ration", "", "", 0, IS_FOOD, FOOD_RATION, /* 2 x nutr */
     1400, 0, 0, 0, 2000, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 200, 0, 0, MAT_GOLD, 0, 0, 0 },

   { "ring", "", "", 0, IS_RING, 0,
     2000, 0, 0, 0, 50, NEUTRAL,
     0, 0, 0, 0, 0, 0,
     -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },   

   /* containers */
   { "bag", "", "", 0, IS_CONTAINER, 0,
     200, 0, 0, 0, 1500, NEUTRAL,
     1, 2, 1, 2, 0, 0,
     -1, 0, 0, 0, MAT_LEATHER, 0, 0, 0 },
   { "pouch", "", "", 0, IS_CONTAINER, 0,
     100, 0, 0, 0, 800, NEUTRAL,
     1, 2, 1, 2, 0, 0,
     -1, 0, 0, 0, MAT_LEATHER, 0, 0, 0 },
   { "chest", "", "", 0, IS_CONTAINER, 0,
     250, 0, 0, 0, 8500, NEUTRAL,
     1, 2, 1, 2, 0, 0,
     -1, 0, 0, 0, MAT_WOOD, 0, 0, 0 },

   { "", "", "" } /* end of the list */
};

item_def missiles[]=
{
     { "arrow", "", "", 0, IS_MISSILE, WPN_MISSILE_1,
       8, 0, 0, 0, 20, NEUTRAL, 1, 2, 1, 4, 0, 0, -1, 0, 0, 0, MAT_IRON },
     { "bolt", "", "", 0, IS_MISSILE, WPN_MISSILE_2,
       16, 0, 0, 0, 40, NEUTRAL, 1, 2, 1, 6, 0, 0, -1, 0, 0, 0, MAT_IRON },
     { "", "", "" } /* end of the list */
};

item_def artifacts[]=
{
     { "huge battle axe", "Great battle axe", "Thoron", 0, IS_WEAPON1H, WPN_AXE,
       500000, ITEM_ARTIFACT,
       0, 0, 300, NEUTRAL, 6, 6, 3, 4, 4, 0, -1, 4, 0, 0, MAT_ORCISH },
/*
     { "shiny katana", "Katana of resistance", "Em'ladir", 0, 
     IS_WEAPON2H, WPN_SWORD,
       70000, ITEM_ARTIFACT | ITEM_MODDEX,
       RES_FIRE | RES_ACID | RES_POISON | RES_COLD, 0, 300, NEUTRAL,
       5, 6, 1, 2, 4, 0, -1, 4, 0, 0, MAT_PLATINIUM },
     { "lantern", "Lantern", "Tha'nthol's light", 0, IS_LIGHT, LIGHT_LANTERN,
       20000, ITEM_ARTIFACT, 0, 0, 1200, NEUTRAL,
       1, 4, 2, 2, 0, 0,
       -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
*/
     { "", "", "" } /* end of the list */
};

_Tvaluable valuables[]=
{
   { "copper coin", IS_MONEY, MAT_COPPER, 10,   1 },
   { "silver coin", IS_MONEY,	MAT_SILVER, 5,  5 },
   { "gold coin",   IS_MONEY,	MAT_GOLD, 15,  10 },
 
   { "", 0, 0, 0, 0 } /* end of the list */
};

/*
struct item_def potions[]=
{
{ "cure blindness", "", POTION, MISSILES,
  ITEM_OK, 0, CURE_BLINDNESS, 5, NEUTRAL, 1, 1, 0, 0, -1, 0 },
{ "blindness", "", POTION, MISSILES,
       ITEM_OK, BLINDNESS, 0, 5, NEUTRAL, 1, 1, 0, 0, -1, 0 },

{ "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }	// end of list
};
*/

char *outfits[]=
{
     "heavy",
     "light",
     "nice",
     "rusty",
     "clear",
     "blazing",
     "blinking",
     "hazy",
     "smoky",
     "wobbly",
     "dusty",
     "synthy",
     "ancient",
     "watery",
     "shady",
     NULL
};



/* food */
_foodlist foodlist[]=
{
   { "ration", 1000 },
   { "melon",  1000 },
   { "corpse", 60 },
   { "small ration", 1000 },
   { "large ration", 1000 },
   { "iron ration", 1000 },
   { NULL }
};

/*
 * This list if for descriptions of item categories
 *
 * Also, normally the item char to output will be taken from here
 *
 */
_typelist gategories[]=
{
   {'(', "One handed weapons"},
   {')', "Two handed weapons"},
   {'}', "Missile weapons"},
   {'[', "Shields"},
   {']', "Armor"},
   {'-', "Tools"},
   {'&', "Amulets"},
   {'\\', "Wands"},
   {'=', "Rings"},
   {'"', "Books"},
   {'?', "Scrolls"},
   {'!', "Potions"},
   {'{', "Instruments"},
   {'+', "Lights"},
   {'/', "Missile ammunition"},
   {'~', "Bracelets"},
   {'%', "Comestibles"},
   {'$', "Valuables"},
   {'*', "Rocks & gems"},
   {'_', "Specials"},
   {'0', "Containers"},
   { 0, NULL }
};

/* 
 * Faces for special items 
 * if item type==IS_SPECIAL then the face to output will be taken from
 * this list, must cope with special item indexing!
 */

Toutface item_outfaces[]=
{
   { 'O', C_WHITE },   /* SPECIAL_BOULDER */
   { '*', CH_DGRAY },  /* SPECIAL_LROCK    */
   { 'O', CH_YELLOW }, /* SPECIAL_BEEHIVE */
   { '_', CH_WHITE },  /* SPECIAL_ALTAR_LAWFUL */
   { '_', C_WHITE },   /* SPECIAL_ALTAR_NEUTRAL */
   { '_', CH_DGRAY },  /* SPECIAL_ALTAR_CHAOTIC */
};

_Ttorch torches[]=
{
     { 5,5,
       -2,-2,
       "....."
       ".***."	/* no light torch */
       ".***."
       ".***."
       "....."},

     { 7,7,
       -3,-3,
       "......."
       "..***.."	/* small torch */
       ".*****."
       ".*****."
       ".*****."
       "..***.."
       "......."},

     { 9,9,
       -4,-4,
       "........."
       "...***..."	/* medium torch */
       "..*****.."
       ".*******."
       ".*******."
       ".*******."
       "..*****.."
       "...***..."
       "........."},

     { 11, 11,
       -5, -5,
       "..........."
       "....***...."	/* large torch */
       "...*****..."
       "..*******.."
       ".*********."
       ".*********."
       ".*********."
       "..*******.."
       "...*****..."
       "....***...."
       "..........."},

     {0, 0, 0, 0, NULL}
};












































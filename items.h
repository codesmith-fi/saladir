/* items.h

	definitions for items

*/


#ifndef _ITEMS_H_DEFINED
#define _ITEMS_H_DEFINED


#include "saladir.h"

#include "types.h"
#include "hitpoint.h"

#define WEIGHT_KILO 1000

/** item categories
 **
 ** If you change these, remember to revise the item category descriptions
 **/
#define IS_WEAPON1H	0     /* weapon groups below */
#define IS_WEAPON2H	1
#define IS_MISWEAPON	2
#define IS_SHIELD	3
#define IS_ARMOR	4     /* armor groups below */
#define IS_TOOL		5
#define IS_AMULET	6
#define IS_WAND		7
#define IS_RING		8
#define IS_BOOK		9
#define IS_SCROLL	10
#define IS_POTION	11
#define IS_INSTRU	12
#define IS_LIGHT	13
#define IS_MISSILE	14
#define IS_BRACELET     15
#define IS_FOOD		16
#define IS_MONEY	17
#define IS_ROCKGEM	18
#define IS_SPECIAL      19
#define IS_CONTAINER    20    /* bags, chests etc */
#define NUMOFITEMGROUPS 21

#define ISMG_MISCITEM     0xff  /* multigroup */

#define SPECIAL_BOULDER 0
#define SPECIAL_ROCK    1
#define SPECIAL_BEEHIVE 2
#define SPECIAL_ALTAR   3

#define LIGHT_MAXNUM		4
#define LIGHT_NOLIGHT	0
#define LIGHT_SMALL		1
#define LIGHT_MEDIUM		2
#define LIGHT_LANTERN	3

// weapon groups SAME as weapon skills!!!
#define WPN_HAND			0	// hand combat */
#define WPN_DAGGER		1
#define WPN_SWORD		2	// swords (not daggers)
#define WPN_AXE			3
#define WPN_BLUNT		4
#define WPN_POLEARM		5
#define WPN_STAFF		6
#define WPN_BOW			7	// item is a bow weapon
#define WPN_CROSSBOW            8
#define WPN_MISSILE_1		9  // missile item for bows
#define WPN_MISSILE_2          10  // missile item for crossbows
#define WPN_MISSILE_3          11
#define WPN_GENMISSILE         12  // missile
//#define WPN_CROSSBOW		9

#define WEAPONS_DAGGER 0
#define WEAPONS_TANTO 1
#define WEAPONS_STAFF 2
#define WEAPONS_SMALLAXE 3
#define WEAPONS_SHORTBOW 4
#define WEAPONS_SHORTSWORD 5
#define WEAPONS_WAKIZASHI 6
#define WEAPONS_BROADSWORD 7
#define WEAPONS_MACE 8
#define WEAPONS_LONGBOW 9
#define WEAPONS_WARAXE 10
#define WEAPONS_SABER 11
#define WEAPONS_MORNINGSTAR 12
#define WEAPONS_CROSSBOW 13
#define WEAPONS_LONGSWORD 14
#define WEAPONS_BATTLEAXE 15
#define WEAPONS_KATANA 16
#define WEAPONS_CLAYMORE 17
#define WEAPONS_WARHAMMER 18
#define WEAPONS_DAIKATANA 19
#define WEAPONS_ARROW 20
#define WEAPONS_BOLT 21
#define WEAPONS_ROCK 22

#define ARMOR_HELMET 0
#define ARMOR_PAULDRONL 1
#define ARMOR_PAULDRONR 2
#define ARMOR_CUIRASS 3
#define ARMOR_GAUNTLETS 4
#define ARMOR_GREAVES 5
#define ARMOR_BOOTS 6
#define ARMOR_PAULDRONHL 7
#define ARMOR_PAULDRONHR 8
#define ARMOR_CROWN 9
#define ARMOR_CUIRASSH 10
#define ARMOR_GAUNTLESSPIKED 11
#define ARMOR_GREAVESH 12
#define ARMOR_RUNNINGSHOES 13
#define ARMOR_BUCKLER 14
#define ARMOR_SHIELDSM 15
#define ARMOR_SHIELDMD 16
#define ARMOR_SHIELDLG 17
#define ARMOR_CLOAK 18
#define ARMOR_HOODEDCLOAK 19
#define ARMOR_SHIRT 20
#define ARMOR_PANTS 21
#define ARMOR_SANDALS 22

#define MISCI_TORCHSM    0
#define MISCI_TORCHMD    1
#define MISCI_TORCHLG    2
#define MISCI_LANTERN    3
#define MISCI_RATIONSM   4
#define MISCI_MELON      5
#define MISCI_BREAD      6
#define MISCI_PIE        7
#define MISCI_CARROT     8
#define MISCI_RATIONLG   9
#define MISCI_IRONRATION 10
#define MISCI_RING       11
#define MISCI_BAG        12
#define MISCI_POUCH      13
#define MISCI_CHEST      14

/* armor groups */
#define ARM_CLOAK       EQUIP_CLOAK
#define ARM_SHIRT       EQUIP_SHIRT
#define ARM_PANTS       EQUIP_PANTS
#define ARM_LEGARM    	EQUIP_LEGS
#define ARM_BODY        EQUIP_BODY
#define ARM_LHANDARM    EQUIP_LARM
#define ARM_RHANDARM    EQUIP_RARM
#define ARM_BOOTS       EQUIP_BOOTS
#define ARM_GLOVES      EQUIP_GLOVES
#define ARM_HELMET      EQUIP_HEAD
#define ARM_SHIELD      EQUIP_HANDS


/* food types */
#define FOOD_RATION		0x0002
#define FOOD_MELON      0x0001
#define FOOD_CORPSE     0x0000
#define FOOD_RATIONS    0x0003
#define FOOD_RATIONL    0x0004
#define FOOD_RATIONI    0x0005

#define MIS_ARROW		0x00010000 // can be used in bows
#define MIS_BOLT		0x00020000 // can be used in crossbows

/* magic group scroll effects */
/* see magic.h for spells */

/* *************************************************************** */

/* alignments */
#define ALIGNMENT_LIMIT	4000
#define CHAOTIC_S		0
#define CHAOTIC_E    ( ALIGNMENT_LIMIT - 1 )
#define NEUTRAL_S		ALIGNMENT_LIMIT
#define NEUTRAL_E		( ALIGNMENT_LIMIT * 2 )
#define LAWFUL_S 		( ( ALIGNMENT_LIMIT * 2 ) + 1)
#define LAWFUL_E 		( ALIGNMENT_LIMIT * 3 )
#define CHAOTIC		0
#define NEUTRAL		( ( ALIGNMENT_LIMIT * 3 ) / 2 )
#define LAWFUL			( ALIGNMENT_LIMIT * 3 )
#define RANDALIGN		-1

#define ITEM_BROKEN		5

#define ITEM_CURSED		0x00000008
#define ITEM_BLESSED		0x00000020
#define ITEM_ENCHANTED          0x04000000 /* item is enchanted */
#define ITEM_NOTPASSABLE        0x08000000 /* occupies whole location */
#define ITEM_UNPAID	        0x10000000 /* unpaid item */
#define ITEM_GENERATED	        0x20000000 /* for artifacts, set if generat */
#define ITEM_ARTIFACT           0x40000000 /* true if artifact */
#define ITEM_IDENTIFIED	        0x80000000 /* true if item is known */

#define MAT_NOMATERIAL	-1
#define MAT_LEATHER	 0 /* armor materials only */
#define MAT_CHAIN	 1 /* armor materials only */
#define MAT_WOOD	 2
#define MAT_GLASS	 3 /* not armor */
#define MAT_COPPER	 4
#define MAT_IRON	 5
#define MAT_STEEL	 6
#define MAT_STONE	 7
#define MAT_SILVER	 8
#define MAT_GOLD	 9
#define MAT_CRYSTAL	 10 /* not armor */
#define MAT_ELVEN	 11
#define MAT_DWARVEN	 12
#define MAT_ORCISH	 13
#define MAT_MITHRILL	 14
#define MAT_PLATINIUM	 15
#define MAT_EBONY	 16 /* not armor */
#define MAT_DIAMOND	 17
#define MAT_ADAMANTIUM	 18
#define MAT_DRAGONWHITE	 19	/* armor materials */
#define MAT_DRAGONRED	 20
#define MAT_DRAGONGREEN	 21
#define MAT_DRAGONBLUE	 22
#define MAT_DRAGONBLACK	 23

/* for cloths and some special items */
#define MAT_FABRIC       24
#define MAT_PAPYRUS      25

/* special material status flags */
/* material flags */
#define MATSTAT_NOTARMOR    0x00000001	/* material is not for armor */
#define MATSTAT_NOTBOWS	    0x00000002	/* not for bow weapons */
#define MATSTAT_NOTWEAPON   0x00000004	/* material is not for weapons */
#define MATSTAT_NOTMISSILE  0x00000008	/* material is not for weapons */
#define MATSTAT_RING        0x00000010  /* for rings too */
#define MATSTAT_SPECIAL     0x40000000  /* for cloth/scrolls only */
#define MATSTAT_CANSHARD    0x80000000	/* can break immendiately */

/* conditions */
#define COND_NEW		0
#define COND_GOOD		1
#define COND_USED		2
#define COND_WORN	3
#define COND_BAD	4
#define COND_BROKEN	5

/* money indexes to the valuables array */
#define MONEY_COPPER	0
#define MONEY_SILVER	1
#define MONEY_GOLD	2

#define ITEM_NAMEMAX    40

/* food definition struct */
typedef struct
{
      char     *name;
      int16s   nutr;
} _foodlist;

typedef struct
{
      char *name;		// description of the material
      int8s dam;		// damage/ac modifier
      int8s hit;     // change to hit mod
      int8s dv;		// defense value
      int8s speed;   // speed modifier
      int8u color;	// material color
      int32u status;	// special material flags
      int32u resist; // resistances
      int16s durability; // break propability (condition goes worse)
      int16u appearprob; // how often appear (0-100)
      real wmod;    // weight modifier, multiplier
      real vmod;    // item (gold) value modifier
} _matlist;

typedef struct
{
      char out;
      char *name;
} _typelist;

typedef struct
{
      int16u out;
      int16u color;
} Toutface;

#define EFLG_PERMCURSED 0x00000001 /* permanently cursed */

/* pack for enchantments */
typedef struct
{
      /* misc enchantments, bit flags on/off */
      int32u pos_eff; /* positive effects */
      int32u neg_eff; /* negative effects */

      /* material/magic resistances (0=nothing)*/
      Tresistpack res;

      /* stats enchantments */
      int8s STR;
      int8s WIS;
      int8s DEX;
      int8s CON;
      int8s CHA;
      int8s INT;
      int8s TGH;
      int8s LUC;
      int8s SPD;

      /* special damage */
      int8u dmat;  /* fire, cold etc. */
      int8u ds;    /* dice sides */
      int8u dt;    /* dice times */
      int8s dmod;  /* dam modifier */
} Tenchantments;

struct _Sinventory;

// item definition
struct _Sitemdata
{
      char name[ITEM_NAMEMAX+1];	// name when not identified
      char rname[ITEM_NAMEMAX+1];	// name when identified
      char sname[ITEM_NAMEMAX+1];	// if named item, here is the name
      int8u icond;	// item damage status
      int8u type;	// item type, weapon, armor, tool etc..
      int32u group;	// item group (weapons/armor/food mainly)

      int32u price;	// base price in copper coins
      int32u status;	// item status (broken, rusty etc.)

      /* remove */
      int32u resist;	// resistances

      int32u special;	// other special attributes
      int32s weight;	// item weight (1000 is 1kg)
      int16s align;	// item alignment
      
      int8u melee_dt;	// dice times
      int8u melee_ds;	// melee damage dice sides
      int8u missi_dt;	// melee damage dice
      int8u missi_ds;
      int8s meldam_mod;	// melee damage modifier
      int8s misdam_mod;	// melee damage modifier
      
      int32s turnsleft;	// remaining turns until breaks, -1 is infinite
      int16s pmod1;	// item modifier
      int16s ac;	// armor class modifier
      int16s dv;	// defense value;
      int16s material;	// item material
      int16s pmod2;	// item modifier
      int16s pmod3;	// item modifier
      int16s pmod4;	// item modifier

      /* items enchantments */
      Tenchantments ench;

      /* inventory for containers */
      struct _Sinventory *inv;
};

typedef struct _Sitemdata item_def;

typedef struct
{
      char name[ITEM_NAMEMAX+1];	// name when not identified
      int16u type;	 // item type, weapon, armor, tool etc..
      int16u material;   // item material
      int32u weight;
//      real value;	 // coin value in GOLD coins
      int16u value;
} _Tvaluable;

typedef struct
{
      int16u sx;
      int16u sy;
      int16s dx, dy;	/* delta x, delta y from player position */
      char *data;
} _Ttorch;


/* items.cpp */
extern _Tvaluable valuables[];
extern item_def weapons[];
extern item_def armor[];
extern item_def artifacts[];
extern item_def miscitems[];
extern item_def templ_scroll;
extern char *food_condition[];
extern char *condition[];
extern char *outfits[];
extern _typelist gategories[];
extern Toutface item_outfaces[];
extern _foodlist foodlist[];
extern _Ttorch torches[];
extern _matlist materials[];
extern item_def SPECIAL_ITEMS[];

#endif

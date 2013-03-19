/* defines
*/

#include "saladir.h"

#include "types.h"
#include "items.h"
#include "creature.h"
#include "skills.h"
#include "hitpoint.h"
#include "status.h"
#include "quest.h"
#include "pathfind.h"

#ifndef _DEFINES_H_DEFINED
#define _DEFINES_H_DEFINED

/* needed macros */
#define ABS(u)    ( (u) >= 0 ? (u) : -(u))
#define SIGN(u)   ((u)==0 ? 0 : _sign_(u))
#define _sign_(u) ( (u) > 0 ? 1 : -1 )

#define LOS_MAXARRAY 361

#define NAMEMAX			20
#define TITLEMAX                30

#define ITEM_T_MAX		30
#define ITEM_S_MAX		20

#define TEMP_MAX			200

// directions of move
#define DIR_UP			8
#define DIR_DOWN		2
#define DIR_RIGHT		6
#define DIR_LEFT		4
#define DIR_UPRIGHT  9
#define DIR_UPLEFT   7
#define DIR_DNRIGHT  3
#define DIR_DNLEFT   1

#define DIR_SELF     5

/* carry weight defines */
#define INV_OK       0
#define INV_BURDEN   1
#define INV_STRAIN   2
#define INV_OVERLOAD 3

#define EQUIP_HEAD	0
#define EQUIP_NECK	1
#define EQUIP_LRING	2
#define EQUIP_RRING	3
#define EQUIP_HANDS	4	/* duplicate */
#define EQUIP_LHAND	4
#define EQUIP_RHAND	5
#define EQUIP_MISSILE	6
#define EQUIP_TOOL      7

#define EQUIP_CLOAK	8
#define EQUIP_SHIRT	9
#define EQUIP_GLOVES	10
#define EQUIP_PANTS	11
#define EQUIP_BOOTS	12
#define EQUIP_BODY	13
#define EQUIP_LARM	14
#define EQUIP_RARM	15
#define EQUIP_LEGS	16
#define MAX_EQUIP	17

//#define EQUIP_LIGHT             16
//#define EQUIP_MISWEAPON         17

#define EQSTAT_NOLIMB   11
#define EQSTAT_BROKEN   10
#define EQSTAT_OK       0

//#define KEY_PCENTER  10

#define GAME_DO_REDRAW     0x80000000 // true if screen needs redraw
#define GAME_SHOWALLSTATS  0x0fffffff

#define GAME_EXPERCHG      0x00000001
//#define GAME_STATUSCHG     0x00000002  // true if status line needs redraw
#define GAME_ATTRIBCHG     0x00000004
#define GAME_MONEYCHG      0x00000008
#define GAME_HPSPCHG       0x00000010
#define GAME_ALIGNCHG      0x00000020 
#define GAME_LEVELCHG      0x00000040
#define GAME_EDITORCHG     0x00000080
#define GAME_CONDCHG       0x00000100

/* food levels, max values of level */
#define FOOD_MAXNUTR    24000
#define FOOD_BLOATED    24000
#define FOOD_SATIATED   19200
#define FOOD_FULL       14400
#define FOOD_HUNGRY     9600
#define FOOD_STARVING   4800
#define FOOD_FAINTING   2400
#define FOOD_FAINTED	0

/* index of status array */
#define STAT_ARRAYSIZE  9   /* the size of whole stat array */
#define STAT_BASICARRAY 7  /* number of basic stats */
#define STAT_STR	0
#define STAT_TGH	1
#define STAT_CON	2
#define STAT_CHA	3
#define STAT_DEX	4
#define STAT_WIS	5
#define STAT_INT	6
#define STAT_LUC	7
#define STAT_SPD	8

#define NUM_QUICKSKILLS 10

/* heigth
#define STAT_AC	9
#define STAT_HP	10
#define STAT_SP	11
*/

#define STATMAX_SPEED	180
#define STATMAX_LUCK		20
#define STATMAX_GEN		99
#define STATMIN_GEN		0

#define BASE_SPEED 100	/* monster base speed */
#define BASE_TIMENEED 1000

/* time needed by certain actions */
#define TIME_MOVEAROUND    800
#define TIME_OPENDOOR	   600
#define TIME_CLOSEDOOR	   600
#define TIME_PICKUP	   600
#define TIME_SEARCH        900
#define TIME_AUTOSEARCH    400
#define TIME_MELEEATTACK   600	/* per hand */
#define TIME_EATKILO	   400 /* per kilo */
#define TIME_DROPITEM      400
#define TIME_MISSILEATTACK 600


#define TACTIC_COWARD	0
#define TACTIC_VERYDEF	1
#define TACTIC_DEF		2
#define TACTIC_NORMAL	3
#define TACTIC_AGGR		4
#define TACTIC_VERYAGGR	5
#define TACTIC_BERZERK	6

/* magic elements */
#define ELEMENT_NOTHING 0
#define ELEMENT_FIRE    1
#define ELEMENT_POISON  2
#define ELEMENT_COLD    3
#define ELEMENT_ELEC    4
#define ELEMENT_WATER   5
#define ELEMENT_ACID    6

/* inventory structure for players and monsters */
typedef struct invnode
{
      int16s x;		// position in the level
      int16s y;

      int32u count;	// number of this kind of items in inventory
      struct invnode *next;	// pointer to next item in inventory

      int16s slot;      // if item is equipped, slot number 
      item_def i;	// item definition structure
} inv_info;

typedef inv_info *Tinvpointer;

/*
 * One equipment slot
 */
typedef struct
{
      Tinvpointer item;
      int8u in_use;
      int8u reserv;
      int8u status;
} _Tequipslot;

/*
 * Creatures inventory, inventory holds a linked list of
 * items (type Tinvpointer), and the array of equipped
 * items (type _Tequipslot). Weight is the total weight of
 * the inventory.
 *
 */
typedef struct _Sinventory
{
      Tinvpointer first;
//      _Tequipslot equip[MAX_EQUIP+1];
      int32u weight;
      int32u copper;
      int32u capasity;
} Tinventory;

typedef struct {
      int16s initial;	/* initial status value */
      int16s temp;	/* temporary change */
      int16s perm;      /* permanent status change */
      int16s min;	/* minimum value */
      int16s max;       /* maximum value */
} _statpack;

typedef struct
{
      int16u group;
      int16u type;
      int8u select;
} Tquickskill;

typedef struct
{
      char name[NAMEMAX+1];
      char title[TITLEMAX+1];
      int16s pos_x;		// position in the dungeon
      int16s pos_y;
      int8u spos_x;		// position in the screen
      int8u spos_y;
      int16s lreg_x, lreg_y;
      int16s wildx;
      int16s wildy;

      int16s attackbonus;

      int16s dungeon;    // in which dungeon player is
                         // 0 is OUTWORLD
      int16s dungeonlev; // dungeonlevel index where player is...
      int16s timetaken;  // how much "timeunits" used in current turn
      int16s align;
      int8u prace;
      int8u pclass;

      int8u lastdir;

      int8u color;      // color of player char

//      Gametime nutritime;

      int32u movecount;
      int32u exp;
      int32u light;
      int32u bill;	 // player bill to the shop
      int16u num_places; // number of places visited
      int16u num_levels; // number of levels visited
      int16u num_kills; 

//      int16s ac;		// armor class (PV)
      int16s hp;		// hitpoints
      int16s sp;		// spellpoints
      int16s hp_max;		// hitpoints
      int16s sp_max;		// spellpoints

      int8u level;

      int32u regentime;

      int32s nutr;       // food status

      int8u sight;      // distance of sight

      int32u weight;	 // player weight (1000 is 1kg)
//      int32u packweight; // inventory weight in grams (1000 is 1kg)
      real goldamount;

      int8u gender;	/* player's sex */
      int8u tactic;

      int8s hitwall;

      int32u status;    /* status flags */

      bool huntmode;
      bool alive;    	// life status
      bool repeatwalk;
      bool monsterinsight;
      bool iteminsight;
      bool searchmode;
      int16s inroom;	// set to -1 if player is not in any room, 
                        // else the room index

      struct _dungeonleveldef *levptr; /* pointer to dungeon level */

      /* equipment wielded, pointer is NULL if not wielding anything */
//      _Tequipslot equip[MAX_EQUIP+1];

      _statpack	stat[STAT_ARRAYSIZE+1];
      _hpslot hpp[HPSLOT_MAX+1];	// hitpoints pack
      _Tequipslot equip[MAX_EQUIP+1];

      /* player inventory - linked list */
      Tinventory inv;

      /* linked skill list */
      Tskillpointer skills;

      Tcondpointer conditions;

      Tquestpointer quests;

      Tquickskill qskills[NUM_QUICKSKILLS];

      /* walking path if any */
      Tpathlist path;
} playerinfo;

#ifdef _OLDINVENTORY
// level item structure
typedef struct levinode
{
      int32u count;	         // count of these items (pile)
      int16s x;		         // position in the level
      int16s y;

      item_def i;      // item structure
      levinode *next; // pointer to next item or NULL
} item_info;

typedef struct
{
      item_info *ptr;
      int8u	sel;
} _ptrlistdef;
#endif


typedef struct
{
      inv_info *ptr;
      int8u	sel;
} _playerptrlistdef;

typedef _playerptrlistdef *Titemlistptr;

typedef struct
{
      int16s dam; /* damage modifier */
      int16s spd;	/* speed modify */
      int16s hit; /* hit modify */
      int16s pv;	/* absorbed hp */
      int16s dv; /* defense value, */

} _Ttactic;

// level monster structure
typedef struct _levmonster
{
      int32u id;       /* monster generation ID */
      int32s npc;      /* if monster is NPC, this is the number */

      int32u hpregen;  /* hitpoints regeneration */
      int32u bill;
      int32u exp;

      int32u movecount;
      
      int16s x;	    // position in the level
      int16s y;
      int16s time;     // time left from last move
      
      int16s base_hp;      
      int16s hp;       // current hitpoints of the monster
      int16s sp;       // current spellpoints
//      int16s ac;			// armor class
      int16s hp_max;        // current hitpoints of the monster
      int16s sp_max;        // current spellpoints
      int16s targetx;
      int16s targety;

      int16s attackbonus;
      
      int16s rev_x1;		// monster location limits, shopkeepers
      int16s rev_x2;
      int16s rev_y1;
      int16s rev_y2;
      int16s roomnum;	// shopkeeper room index

      int16s dist_2_player; 
      bool   cansee_player;

      int8u light;
      
      int16s inroom;
      int8u lastdir;    // direction of last move
      int8u sindex;		// special index, ie for shopkeepers guard route
      int8u tactic;
      
      _monsterdef m;
      
      /* hitpoints array */
      _hpslot 		hpp[HPSLOT_MAX+1];	// hitpoints pack
      
      /* stat array */
      _statpack	stat[STAT_ARRAYSIZE+1];
      
      /* equipment wielded */
      _Tequipslot equip[MAX_EQUIP+1];
//      _Tequipslot equip[MAX_EQUIP+1];
      
      /* linked skill list */
      Tskillpointer skills;

      Tcondpointer conditions;
      
      /* monster inventory, exactly as players inv */
      Tinventory inv;

      _levmonster *target; // who is the monster attacking
      // NULL if player (and MST_ATTACKMODE is set)
      _levmonster *next;	// points to next monster in level

      /* walking path if any */
      Tpathlist path;
      
} _monsterlist;

typedef struct
{
      int32u monid; /* monster generation id, for next monster */      
} Tgamedata;


#endif

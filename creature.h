/*
** creature.h
** definitions for creature.cpp
*/

#ifndef _CREATURE_H_DEFINED
#define _CREATURE_H_DEFINED

#include "hitpoint.h"

#define CDESC_MAX  40
#define CNAME_MAX  20

#define RACE_HUMAN	 0
#define RACE_CATHUMAN 1
#define RACE_HIGHELF  2
#define RACE_GRAYELF	 3
#define RACE_DARKELF	 4

#define RACEGEN_SPECIAL	0x00000001 /* special race, not for random monsters */

/* define bits for monster status flags */
#define MST_PURSUEITEM	0x00000001
#define MST_CANTMOVE	0x00000002 // it's unable to move for some reason
#define MST_FLEEMODE    0x00000004 // monster is fleeing for its life
#define MST_ATTACKMODE  0x00000008 // this bit on monster is in attackmode
#define MST_KEEPERHATES	0x01000000 // monster hated by shopkeeper
#define MST_INSIDESHOP	0x02000000 // for shopkeepers,
#define MST_HATEPLAYER	0x04000000 // for shopkeepers, hates player will attack
#define MST_GUARDDOOR   0x08000000
#define MST_SHOPKEEPER  0x10000000 // it is a shopkeeper
#define MST_KNOWN       0x20000000 // bit set if creature unknown (Chat?)
#define MST_ALIVE       0x40000000 // monster is alive
#define MST_GENERATED	0x80000000 // this is set if monster already generated once
                                   // for special NPC's mainly

#define BEHV_FRIENDLY	0x00000001  // monster is generally friendly
#define BEHV_GETALL 	0x00000002  // get EVERYTHING comes in hand
#define BEHV_SENSEBEST	0x00000004  // sense best item and target it
#define BEHV_CANUSEITEM	0x00000008  // if set with animal, can carry one item
#define BEHV_ANIMAL	0x00000010  // animal monster, no class, no inventory
#define BEHV_FLYING	0x00000020  // flying creature, has hands but
#define BEHV_SWIMMING   0x00000040  /* swimming creature */
                                    // they are called WINGS
/* attack flags */
#define ATTACK_BITE	0x00000001  // does bite attacks
#define ATTACK_HIT	0x00000002  // hits with hands
#define ATTACK_KICK     0x00000004  // mainly kicks (horse?)

#define ANGRY_NORMAL		0
#define ANGRY_TO_MONSTER 	-5
#define ANGRY_TO_PLAYER		-10

/* special monster types */
#define NPC_BILLGATES	1
#define NPC_SPARHAWK	2
#define NPC_THOMAS      3
#define NPC_NATASHA	4

/* monster and player 'sex' values */
#define SEX_UNKNOWN	0
#define SEX_MALE	1
#define SEX_FEMALE	2
#define SEX_NEUTRAL	3

/* targetting flags for monsters
   monster can attack specific bodypart when
   a flag has been set in the race description
   structure */
#define TARGET_HEAD	0x01
#define TARGET_BODY	0x02
#define TARGET_LHAND	0x04
#define TARGET_RHAND	0x08
#define TARGET_LEGS	0x10

/* carrying weight burnen levels */
#define WGH_OVERLOAD    98 /* values out of 100 which is the capasity */
#define WGH_STRAIN      90
#define WGH_BURDEN      75

typedef struct
{
      char *desc;
      int16u hour;
      int16u min;

} _Tfoodstat;

/*
** npc race definition
*/
typedef struct _Snpcrace
{
      char *name;			// name of this race
      char *desc;			// race description (if any)
      char out;      // char to output
      int8u color;   // color of output
      
      int32u attacktypes;     // which kind of attack does the race do
      int32u behave;		// behaviour & status flags
      int32u weight;    // 1000 is 1kg
      int32u status;    // status flags (is he alive, confuzed etc)
      
      int16s align;		// alignment of the monster
      int8u hp_plev_dt;	// hp gain (dice count) per level
      int8u hp_plev_ds; // hp gain (dice sides) per level
      int8u sp_plev_dt;
      int8u sp_plev_ds;
      int16s hp_base;	// starting hp (level 1)
      int16s sp_base;	// starting sp	(level 1)
      int16s ac;      	// armor class
      int16s attitude;	// general attitude (pissing factor :-)
      int32u exp;
      
      int8u	dam_dt;		// hand damage dice
      int8u dam_ds;		// hand damage dice
      int8s dam_mod;
      
      int8u targetflags;	// where this monster can target (legs, head etc.)
      
      // bodyparts, -1 doesn't exist
      // 0 or positive is the hit probability for player
      int16s bodyparts[HPSLOT_MAX];
      
      int32u GENFLAGS;	// generation flags

      /* race resistances */
      Tresistpack res;

      int8u STR;		// class starting values
      int8u WIS;
      int8u DEX;
      int8u CON;
      int8u CHA;
      int8u INT;
      int8u TGH;
      int8u LUC;
      int8u SPD;
      
} _npcracedef;


typedef struct
{
      int16s DUNGEON;
      int16s LEVEL;
      int16s X;
      int16s Y;
      
} _Tappearlist;

typedef struct _Smonster
{
      char desc[CDESC_MAX+1];	// short description
      char name[CNAME_MAX+1];	// if named monster, name
      int16u longdesc;        // long description number	(from desclist)
      
      int32u weight;    // 1000 is 1kg
      int8u level;      // creatures level
      int16s align;		// alignment of the monster
      int8u special;		// special monster type (NULL if normal)
      
      int8u race;			// human, elf... etc
      int8u mclass;		// fighter, mage etc...
      int8u gender;
      int16s attitude;
      int32u behave;		// behaviour & status flags
      int8u STR;
      int8u WIS;
      int8u DEX;
      int8u CON;
      int8u CHA;
      int8u INT;
      int8u TGH;
      int8u LUC;
      int8u SPD;
      int32u status;    // status flags (is he alive, confuzed etc)
      
} _monsterdef;

/* public tables and variables */
extern const int8u eqslot_from_hpslot[];
extern char *desclist[];
extern _Tappearlist appearlist[];
extern _monsterdef shopkeeper_list[];
extern _monsterdef npc_list[];
extern _npcracedef npc_races[];
extern const char *gendertext[];
extern _Tfoodstat food_status[];
extern const char *gender_art2[];
extern const char *gender_art1[];
extern const char *gender_art3[];
extern char *bodyparts[];
extern char *bodyparts_flying[];
extern const char *bodypart_art[];
extern const char *txt_statnames[];
extern const char *txt_statnames_short[];
extern const char *equip_slotdesc[];
extern const real hp_bpmod[];

#endif 

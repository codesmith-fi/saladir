/********************************************/
/*** level generation related definitions ***/
/********************************************/

#ifndef _CAVES_H_DEFINED
#define _CAVES_H_DEFINED

#include "saladir.h"

#include "types.h"
#include "defines.h"
#include "creature.h"

#define CHANCE_END	10
#define CHANCE_TURN	40

//#define MINSIZEX		70		// minimum level sizes x 
// and y
//#define MINSIZEY		20		// these should be the
//same as MAPWIN size
// maximum level sizes x and y
#define MAXSIZEX	160
#define MAXSIZEY	60


#define CAVE_NOLIT	  0x0001 // no light here
#define CAVE_SEEN	  0x0002 // grid hasn't been seen
#define CAVE_IDENT	  0x0004 // grid hasn't been identified
#define CAVE_TRAPIDENT	  0x0008
#define CAVE_WATER	  0x0010 // water
#define CAVE_MOUNTAIN     0x0020 // mountain
#define CAVE_PASSABLE 	  0x0080 // this grid can be walked into
#define CAVE_DOOR         0x0100
#define CAVE_MONSTERLIGHT 0x2000
#define CAVE_TMPLIGHT	  0x4000 // temporary light
#define CAVE_LOSTMP	  0x8000

#define TYPE_DARK			0	// Dark square
#define TYPE_PATH			1	// corridor path
#define TYPE_SLOWPATH	2	// corridor
#define TYPE_STAIRUP		3
#define TYPE_STAIRDOWN	4
#define TYPE_DOORCLOS	5 // unlocked closed door
#define TYPE_DOORLOCK	6	// locked door
#define TYPE_DOOROPEN	7	// open door
#define TYPE_DOORSECR	8 // secret door (printed as wall)
#define TYPE_DOORTRAP	9 // trapped door
#define TYPE_WALLIP		10	// impassable wall
#define TYPE_WALLPA		11	// passable wall, digging
#define TYPE_ROOMFLOOR	12 // floor for room
#define TYPE_PASSAGE		13 // passage(dungeon)
#define TYPE_GRASS		14
#define TYPE_MOUNTAIN   22
#define TYPE_HMOUNTAIN  23
#define TYPE_VOLCANO    24
#define TYPE_HILLS      25
#define TYPE_WATER	   26
#define TYPE_SEA        27
#define TYPE_FOREST     28
#define TYPE_SWAMP      29
#define TYPE_PLAINS     30
#define TYPE_GRASSLAND  31
#define TYPE_ROAD       32
#define TYPE_CORNFIELD  33
#define TYPE_FIELD      34
#define TYPE_SNOW       35
#define TYPE_SNOWTREE   36
#define TYPE_SNOWPLAIN  37
#define TYPE_ICE        38
#define TYPE_LAVA       39
#define TYPE_HOTGROUND	40
#define TYPE_TREE			41
#define TYPE_BRIDGEV		42
#define TYPE_BRIDGEH		43
#define TYPE_DUNGEON1   60
#define TYPE_DUNGEON2   61
#define TYPE_DUNGEON3   62
#define TYPE_VILLAGE    63
#define TYPE_TOWN1		64
#define TYPE_TOWN2		65

/* these are ????? */
#define TYPE_FIRSTTRAP  70
#define TYPE_TRAP_BOULDER 70
#define TYPE_TRAP_BOMB	71
#define TYPE_TRAPWATER	72
#define TYPE_TRAPROCK	73
#define TYPE_TRAPPIT	74

#define ROOM_NORMAL1	0
#define ROOM_NORMAL2	1
#define ROOM_LAIR 	3
#define ROOM_SHOP		10	// room is a shop
#define ROOM_VAULT	11	// room is a vault
#define ROOM_CASTLE  12
#define ROOM_MISC		100

#define ROOM_MINX		6
#define ROOM_MINY		6
#define ROOM_MAXX		20
#define ROOM_MAXY		10

#define ROOM_NAMEMAX 40
#define ROOM_MAXNUM	20
#define ROOM_MIN		4

#define ROOM_IS_VISITED	0x01
#define ROOM_PLAYERHERE 0x80

#define ROOMTMPL_SHOP1		0
#define ROOMTMPL_SHOP2		1
#define ROOMTMPL_SHOP3		2
#define ROOMTMPL_SHOP4		3
#define ROOMTMPL_SHOP5		4
#define ROOMTMPL_SHOP6		5
#define ROOMTMPL_CASTLE1	6
#define ROOMTMPL_FARM		7
#define ROOMTMPL_HOUSE1		8
#define ROOMTMPL_HOUSE2		9

/* level flags */
#define LEVEL_VISITED    0x80

/* dungeon structure definitions & flags */
#define DUNGEON_KNOWN    0x01
#define DUNGEON_VISITED  0x80 

                           /* NULL is outworld*/
#define NUM_DUNGEONTYPES 4
#define DTYPE_MAZE   0x00  /* maze type level */
#define DTYPE_ROOMY  0x01  /* room type level */
#define DTYPE_ROOMY2 0x02  /* room type level with lots of doors */
#define DTYPE_MAZE2	0x03	/* deteriorated maze */

#define DTYPE_TOWN	0x80  /* Town level! */
#define DTYPE_WILDHUNT 0xfe /* wilderness hunt */
#define DTYPE_RANDOM 0xff  /* level is randomly selected */

#define STAIRUP1     0x01
#define STAIRUP2     0x02
#define STAIRDOWN1   0x03
#define STAIRDOWN2   0x04
#define STAIROUT     0x05

/* dungeon locations, maily for NPC appearing list */
#define PLACE_DPRIMITIVE   1
#define PLACE_DTHANTHOL	   2
#define PLACE_DABYSS       3
#define PLACE_TSANTHEL     4

typedef struct
{
      char	out;		// char to output
      int8u	color;	// color of char
      int16u flags;	// terrain modifier flags
      int8u	timemod;	// walk time modifier (percentage 1-xxx)
      char	*desc;	// short description
} _terrinfo;

/* single grid in cave */
/*
typedef struct
{ 
      int16u flags;
      int8u  type; 
      int8u  sval;  
      int8u  doorfl;	

} cave_type;
*/

typedef struct
{
      int16u flags;
      int16u type;  /* room, corridor, shop etc.. */
      int8u  sval;  /* trap strenght, secret door level etc...*/
      /* for staircases, it's the staircase number */
      int8u  doorfl;	/* door flags, see above */
      int8u trap;
      int8u trap_st;
} cave_type;

#define SHOPTYPE_ARMOUR  1
#define SHOPTYPE_WEAPON  2
#define SHOPTYPE_GENERAL 3
#define SHOPTYPE_BOOK	 4
#define SHOPTYPE_POTION  5
#define SHOPTYPE_MAGIC	 6
#define SHOPTYPE_FOOD    7

typedef struct
{
      char name[ROOM_NAMEMAX+1]; // room name
      int8u type;			// room type, shop, vault etc..
      int8s shoptype;		// shop type
      int8u shopname;   // index to shopnametable
      int8u flags;		// room special flags
      int8u sellp;		// 100 = items are not over or low priced
      int8u buyp;			// 100 = will pay the real item value from items
      int16s x1;			// coordinates for room
      int16s x2;
      int16s y1;
      int16s y2;
      int16s doorx;		// coords for the door
      int16s doory;
      _monsterlist *owner;	// room owner
} _roomdef;

typedef struct
{
      int16s sizex;		// level width
      int16s sizey;		// level height
      int16u danglev;		// level danger level

      int16u roomcount;
      /* array of rooms in the level */
      _roomdef rooms[ROOM_MAXNUM+1];

//      item_info *items;		// linked list for items on the level
      Tinventory inv;           // level inventory

      _monsterlist *monsters;	// linked list for monsters in this level

      // memory locations for every horizontal row in the level map
      cave_type *loc[MAXSIZEY+1];

} level_type;

// test

#define MAX_DLEVELS  50
typedef struct _dungeonleveldef
{
      char *name;    /* name for this level */
      int16u index;  /* special index, needed for savefiles */
      int16u danger; /* danger level for this level */
      int16u dtype;  /* required dungeon type */      
      int16s outx;   /* if exiting to OUTSIDE here are the coordinates */
      int16s outy;   /* otherwise these are NULL */
      int32u flags;  /* flags for the level */
      _dungeonleveldef *linkfrom1;  // which level exits to this level
      _dungeonleveldef *linkto1;    // where does this level exit to
      _dungeonleveldef *linkfrom2;     
      _dungeonleveldef *linkto2;      
} _dungeonleveldef;

typedef struct
{
      char *name;   	/* name of this dungeon */
      char *desc;
      int16u index;	/* dungeon index number */
      _dungeonleveldef *levels;
      int16s x;        	/* position in main map */
      int16s y;
      int8u staircase;  /* which staircase to enter */
      int16u danger;   	/* danger level of this dungeon */
      int16u flags;     /* dungeon status flags, is it found etc... */
      int8u out;        /* terrain type to output if cave found */
} _dungeondef;


/* directions of movement (for level generator)
   also directions of rooms and doors etc.
   */
#define DIR_NORTH    0
#define DIR_EAST     1
#define DIR_SOUTH    2
#define DIR_WEST     3
#define DIR_UNKNOWN  100

typedef struct
{
      int16s sx;
      int16s sy;
      int16s doordir;
      int16u roomtype;
      char *name;
      char *room;
} Troomtemplate;

#include "output.h"

extern const char *roomnames[];
extern _terrinfo terrains[];
extern _dungeonleveldef Testhole[];
extern _dungeondef dungeonlist[];
extern Troomtemplate lairtemplates[];
extern Troomtemplate towntemplates[];

#endif /* caves.h */

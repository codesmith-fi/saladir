/*
** DUNGEON STRUCTURE
**
*/

#include "caves.h"

_dungeonleveldef Testhole[]=
{
  { "Top", 1, 0, 
    DTYPE_ROOMY, 2, 12, 0, NULL, Testhole+1, NULL, NULL},
  { "", 2, 0,
    DTYPE_ROOMY2, 0, 0, 0, Testhole,   Testhole+2, NULL, NULL},
  { "", 3, 0,
    DTYPE_ROOMY2, 0, 0, 0, Testhole+1, Testhole+3, NULL, NULL},
  { "Bottom", 4, 0,
    DTYPE_ROOMY, 19, 52, 0, Testhole+2, NULL, NULL, NULL},
  { NULL }
};

_dungeonleveldef Abyss[]=
{
  {"Entrance", 1, 0,
   DTYPE_ROOMY, 19, 54, 0, NULL,     Abyss+1, NULL, NULL},
  {"", 2, 0,
   DTYPE_ROOMY2, 0, 0, 0, Abyss,   Abyss+2, NULL, NULL},
  {"", 3, 0,
   DTYPE_ROOMY, 0, 0, 0, Abyss+1, Abyss+3, NULL, NULL},
  {"", 4, 0, 
   DTYPE_ROOMY2, 0, 0, 0, Abyss+2, Abyss+4, NULL, NULL},
  {"", 5, 0,
   DTYPE_RANDOM, 0, 0, 0, Abyss+3, Abyss+5, NULL, NULL},
  {"", 6, 0,
   DTYPE_RANDOM, 0, 0, 0, Abyss+4, Abyss+6, NULL, NULL},
  {"", 7, 0,
   DTYPE_RANDOM, 0, 0, 0, Abyss+5, Abyss+7, NULL, NULL},
  {"", 8, 0,
   DTYPE_RANDOM, 0, 0, 0, Abyss+6, Abyss+8, NULL, NULL},
  {"", 9, 0,
   DTYPE_RANDOM, 0, 0, 0, Abyss+7, NULL, NULL, NULL},
  { NULL }
};

_dungeonleveldef Bottoms[]=
{
  {"Level 1",   1, 0,
   DTYPE_MAZE2, 6, 2, 0, NULL,     Bottoms+1, NULL, NULL},
  {"Level 2",   2, 0,
   DTYPE_MAZE2, 0, 0, 0, Bottoms,   Bottoms+2, NULL, NULL},
  {"Level 3",   3, 0,
   DTYPE_MAZE2, 0, 0, 0, Bottoms+1, Bottoms+3, NULL, NULL},
  {"Level 4",   4, 0,
   DTYPE_RANDOM, 0, 0, 0, Bottoms+2, Bottoms+4,     NULL, NULL},
  {"Level 5",   5, 0,
   DTYPE_ROOMY2, 0, 0, 0, Bottoms+3, Bottoms+5,     NULL, NULL},
  {"Level 6",   6, 0,
   DTYPE_ROOMY2, 0, 0, 0, Bottoms+4, Bottoms+6,     NULL, NULL},
  {"Level 7",   7, 0,
   DTYPE_MAZE2, 0, 0, 0, Bottoms+5, Bottoms+7,     NULL, NULL},
  {"Level 8",   8, 0,
   DTYPE_MAZE2, 0, 0, 0, Bottoms+6, Bottoms+8,     NULL, NULL},
  {"Level 9",   9, 0,
   DTYPE_MAZE2, 0, 0, 0, Bottoms+7, Bottoms+9,     NULL, NULL},
  {"Level 10", 10, 0,
   DTYPE_MAZE2, 0, 0, 0, Bottoms+8, NULL,     NULL, NULL},
  { NULL }
};

_dungeonleveldef town_santhel[]=
{
  {"Town", 1, 0,
   DTYPE_TOWN, 8, 33, 0, NULL, town_santhel+1, NULL, NULL},
  {"Dungeons of Santhel", 2, 0,
   DTYPE_ROOMY, 0, 0, 0, town_santhel, town_santhel+2, NULL, NULL},
  {"Dungeons", 3, 0,
   DTYPE_ROOMY, 0, 0, 0, town_santhel+1, NULL, NULL, NULL},
  { NULL }
};

_dungeonleveldef Outworld[]=
{
  { "", 1, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL},
  { NULL }
};

char dungeondesc_out[]=
"Salmorrian mountains\0";

char dungeondesc_primit1[]=
"This dungeon looks dull and boresome.\0";

char dungeondesc_primit2[]=
"You've entered the alternate entrance, but still it looks dull and boresome.\0";

char dungeondesc_thanthol[]=
"There are the famous cavers of Tha'nthol, the bored elf wizard.\0";

char dungeondesc_Abyss[]=
"Abyss, a soon to be a place of danger and hunger :-).\0";

char towndesc_santhel[]=
"A small town with one castle and few houses. Oddly there're only few people here.\0";

/*
** This is a list of dungeons in Saladir, see caves.h for definitions
**
*/

_dungeondef dungeonlist[]=
{
  /* first is the wilderness (outworld map) */
  {"Salmorrian mountains",  dungeondesc_out, 0,
   Outworld, 0, 0, 0, 0, 0, 0 }, 
  
  /* here are the dungeons */
  {"Very primitive dungeon", dungeondesc_primit1, PLACE_DPRIMITIVE,
   Testhole, 2, 12, STAIRDOWN1, 1, 0, TYPE_DUNGEON2 },
  {"Very primitive dungeon", dungeondesc_primit2, PLACE_DPRIMITIVE,
   Testhole+3, 19, 52, STAIRDOWN2, 1, 0, TYPE_DUNGEON2 },
  {"Caverns of Tha'nthol", dungeondesc_thanthol, PLACE_DTHANTHOL,
   Bottoms, 6, 2, STAIRDOWN1, 1, 0, TYPE_DUNGEON1 },
  {"Abyss", dungeondesc_Abyss, PLACE_DABYSS,
   Abyss, 19, 54, STAIRDOWN1, 1, 0, TYPE_DUNGEON3 },
  {"Santhel", towndesc_santhel, PLACE_TSANTHEL,
   town_santhel, 8, 33, STAIRDOWN1, 0, 0, TYPE_TOWN1 },
  { NULL, NULL, 0, NULL, 0, 0, 0, 0, 0, 0 }
};


/*
** terrain information
**
** char, color, description
*/
struct _terrinfo terrains[]=
{
  { ' ', C_BLACK,       0, 100, "Dark", }, 
  { '.', C_WHITE,	CAVE_PASSABLE, 100, "Path" },
  { '.', CH_DGRAY,	CAVE_PASSABLE, 110, "Slow path" },
  { '<', C_GREEN,	CAVE_PASSABLE, 100, "stairs up" },
  { '>', C_GREEN,	CAVE_PASSABLE, 100, "stairs down" },
  { '+', C_WHITE,	CAVE_DOOR, 100, "closed door" },
  { '+', C_WHITE,	CAVE_DOOR, 100, "locked door" },
  { '/', C_WHITE,	CAVE_PASSABLE|CAVE_DOOR, 100, "open door" },
  { '#', CH_DGRAY,	0, 100, "Wall" }, 
  { '+', C_RED,	        0, 100, "trapped door" },
  { '#', CH_DGRAY,	0, 100, "Wall" },
  { '#', CH_DGRAY,	0, 100, "Wall" },
  { '.', C_WHITE,	CAVE_PASSABLE, 100, "Floor" },
  { '.', C_WHITE,	CAVE_PASSABLE, 100, "Passage" },
  { '.', C_GREEN,	CAVE_PASSABLE, 100, "Grass" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '^', CH_DGRAY,	CAVE_MOUNTAIN, 100, "Mountains" },
  { '^', CH_WHITE,	CAVE_MOUNTAIN, 100, "High mountains" },
  { '^', C_RED,	        CAVE_MOUNTAIN, 100, "Volcano" },
  { '~', C_YELLOW,	CAVE_PASSABLE, 160, "Hills" },
  { '=', CH_BLUE,	CAVE_PASSABLE|CAVE_WATER, 140, "Water" },
  { '~', C_BLUE,	   CAVE_PASSABLE|CAVE_WATER, 160, "Deep water" },
  { 'T', C_GREEN,	CAVE_PASSABLE, 110, "Forest" },
  { '"', C_GREEN,	CAVE_PASSABLE, 120, "Swamp" },
  { '"', CH_GREEN,	CAVE_PASSABLE, 100, "Plains" },
  { ',', CH_GREEN,	CAVE_PASSABLE, 110, "Grassland" },
  { '.', C_YELLOW,	CAVE_PASSABLE, 80, "Road" },
  { '%', C_YELLOW,	CAVE_PASSABLE, 100, "Cornfield" },
  { '.', CH_DGRAY,	CAVE_PASSABLE, 100, "Field" },
  { '&', CH_WHITE,	CAVE_PASSABLE, 120, "Snow" },
  { 'T', CH_WHITE,	CAVE_PASSABLE, 100, "Trees (snow)" },
  { '"', CH_WHITE,	CAVE_PASSABLE, 100, "Tundra" },
  { '=', CH_WHITE,	CAVE_PASSABLE, 150, "Ice" },
  { '~', CH_RED,		CAVE_PASSABLE, 100, "Lava" },
  { '.', C_RED,		CAVE_PASSABLE, 100, "Hot ground" },
  { 'T', CH_GREEN,	CAVE_PASSABLE, 100, "A Tree" },
  { '|', C_YELLOW,	CAVE_PASSABLE, 100, "Wooden bridge" },
  { '-', C_YELLOW,	CAVE_PASSABLE, 100, "Wooden bridge" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { 'o', C_RED,	   CAVE_PASSABLE, 100, "Dungeon" },
  { '*', CH_DGRAY,	CAVE_PASSABLE, 100, "Cave" },
  { 'O', CH_WHITE,	CAVE_PASSABLE, 100, "Large opening" },
  { '#', CH_WHITE,	CAVE_PASSABLE, 100, "Village" },
  { 'O', CH_BLUE,	CAVE_PASSABLE, 100, "Small town" },
  { 'O', CH_RED,	CAVE_PASSABLE, 100, "Town" },
  { 'o', CH_BLUE,	CAVE_PASSABLE, 100, "Tower" },
  { 'O', CH_CYAN,	CAVE_PASSABLE, 100, "Tower" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '^', CH_RED,        CAVE_DOOR, 100, "boulder trap" },
  { '^', C_RED,	        CAVE_PASSABLE, 100, "bomb trap" },
  { '^', C_BLUE,		CAVE_PASSABLE, 100, "water trap" },
  { '^', CH_DGRAY,	CAVE_PASSABLE, 100, "rock trap" },
  { '^', C_WHITE,	CAVE_PASSABLE, 100, "pit trap" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { 0, 0, 0, 0, ""}
};

/* Here're the templates for lairs */
/* descriptions:
   xsize, ysize,
   door direction,
   string for the room,

   # unpassable wall
   & dark unpassable wall (will not show up)
   . room floor
   , dark room floor
   ! door will be created here
   ? monster will be created here
   a-Z specific monster
*/

const char *roomnames[]={
  "an ordinary room",
  "a dull looking room",
  "a dull looking room",
  "lair",
  "dull looking room",
  "dull looking room",
  "dull looking room",
  "dull looking room",
  "dull looking room",
  "dull looking room",
  "a shop",
  "dangerous looking room",
  "a castle",
  NULL
};

Troomtemplate lairtemplates[]=
{
  { 	7, 7,
   	DIR_EAST,
	ROOM_SHOP,
	"a shop",
	"#######" /* small shop */
	"#.....#"
	"#.....#"
	"#.....!"
	"#.....#"
	"#.....#"
	"#######"
  },
  { 	7, 7,
   	DIR_WEST,
	ROOM_SHOP,
	"a shop",
	"#######"	/* small shop */
	"#.....#"
	"#.....#"
	"!.....#"
	"#.....#"
	"#.....#"
	"#######"
  },
  { 	7, 7,
   	DIR_SOUTH,
	ROOM_SHOP,
	"a shop",
	"#######"	/* small shop */
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"###!###"
  },
  { 	7, 7,
   	DIR_NORTH,
	ROOM_SHOP,
	"a shop",
	"###!###"	/* small shop */
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"#######"
  },
  { 	10, 10,
   	DIR_WEST,
	ROOM_LAIR,
	"lair",
	"##&&&&&&##"
	"##&,,,,&##"
	"#&&,,,,&&#"
	"#&,,,,,,&&"
	"!,,,,?,,,&"
	"#&,,,,,,&&"
	"#&,,,,,,&#"
	"#&&,,,,&&#"
	"##&&,,&&##"
	"###&&&&###"
  },
  { 	10, 10,
   	DIR_WEST,
	ROOM_LAIR,
	"lair",
	"##########"	/* zoo */
	"#........#"
	"#.b..b...#"
	"#...b..b.#"
	"!.b...b..#"
	"#...b..b.#"
	"#.b...b..#"
	"#...b..b.#"
	"#........#"
	"##########"
  },
  { 	15, 10,
   	DIR_EAST,
	ROOM_LAIR,
	"lair",
	"###############"
	"###....###....#"
	"###....###....#"
	"##......####..#"
	"#...?....###..!"
	"##......####..#"
	"##......###..##"
	"###....###..###"
	"####.......####"
	"###############"
  },
  { 	15, 10,
   	DIR_NORTH,
	ROOM_LAIR,
	"small cave",
	"####!##########"
	"####.#####....#"
	"###..k.###....#"
	"##.k....####..#"
	"#.....k.k###..#"
	"##..k..k####..#"
	"##k.....###..##"
	"###..k.###..###"
	"####.......####"
	"###############"
  },
  { 	15, 10,
   	DIR_EAST,
	ROOM_LAIR,
	"lair",
	"&&&&&&&&&&&&&&&" /* very dark lair of monsters */
	"&,,,g,&&&,,s,,&"
	"&&,G,&&,&,&,&&&"
	"&&,,&&,g,,&,&&&"
	"&,g,,&&,&&&,&,!"
	"&,g,g&&,,s&,&,&"
	"&,,s,&&,&&&,&,&"
	"&&,,&&&,&#&,&,&"
	"&&&,,,,,&#&,,,&"
	"#&&&&&&&&&&&&&&"
  },
  { 	11, 15,
   	DIR_SOUTH,
	ROOM_LAIR,
	"lair",
	"###########"
	"#.........#"
	"#.#######.#"
	"#.#..####.#"
	"#.#...###.#"
	"#.#.?...#.#"
	"#.#...#.#.#"
	"#.#..##.#.#"
	"#.#####.#.#"
	"#.....#.#.#"
	"#####.#.#.#"
	"#...#.#.#.#"
	"#.#.#.#.#.#"
	"#.#...#...#"
	"#!#########"
  },
  { 	15, 15,
   	DIR_NORTH,
	ROOM_LAIR,
	"lair",
	"############!##"
	"#.....#.####.##"
	"#####.#.......#"
	"#...#.#.#######"
	"#.#.#.#.#.#...#"
	"#.#.#.#.#.#.?.#"
	"#.###.#.#.#...#"
	"#.......#.#...#"
	"#.#####.#.##.##"
	"#.#.#.#...##.##"
	"#.....######.##"
	"#####........##"
	"#,,,#.#########"
	"#,,,..........#"
	"###############"
  },
  { 	30, 30,
   	DIR_EAST,
	ROOM_LAIR,
	"lair",
	"##############################"	   /* big lair */
	"##,,,,,##########..####,,,,,,#"
	"#,,,,,,,########....###,,,,,,#"
	"##,,,,,,,###.....?...##,,#,,,#"
	"###,,,######.#.......##,###,,#"
	"####,#######.####...###,###,##"
	"####,#####....#########,##,,##"
	"####,#####.......######,##,###"
	"####.........###..#####,,,,###"
	"####.############.#####,,#####"
	"####.############..###,,######"
	"####..##....######.,,,,,######"
	"#####....##..############.####"
	"#######.####..##########...###"
	"#######.#####.........#......!"
	"#######.#############......###"
	"#######.####....########..####"
	"##......####.##.....###..#####"
	"##..###...##.###...###..######"
	"##.######.#..###..####.#######"
	"##.######....###..##....######"
	"##.#########..##.##..##.######"
	"##..##..#####.##.##.###...####"
	"###.#...#####..####.#####..###"
	"###.......####.####.######.###"
	"###......#####......######.###"
	"###..?.....########.######.###"
	"##.....###..######..######...#"
	"###.#######........#########.#"
	"##############################"
  },
  { 	30, 22,
   	DIR_SOUTH,
	ROOM_LAIR,
	"lair",
	"##############################"	   /* big lair */
	"##.########################.##"
	"#....##..######..####.......##"
	"#.........#####.............##"
	"##...?...######.......###...##"
	"###.....########.....###..####"
	"###....#####...............###"
	"####....####...........G..#.##"
	"#####..###...................#"
	"####.............####.#...#..#"
	"##......######..########..####"
	"####..#########..########...##"
	"#....##########...########...#"
	"#....#########.....#######...#"
	"##......#####........#####...#"
	"##.......##.....#...#####....#"
	"##............####..#####...##"
	"###...........#,,##..###....##"
	"####..##.....##,,,##......####"
	"##....###..#,,,,,,,##...######"
	"###...######,,,,,,,###########"
	"################!#############"
  },
  { 0, 0, 0, 0, NULL, NULL }	/* end of the list */
};

/* desc:
   & do not change type
   | wooden bridge
   # wall
   . room floor
   = water
*/

Troomtemplate towntemplates[]=
{
  { 	15, 6,
   	DIR_SOUTH,
	ROOM_SHOP,
	"a shop",
	"###############"	/* shop */
	"#.............#"
	"#.............#"
	"#.............#"
	"#....##!##....#"
	"######&&&######"
  },
  { 	15, 6,
   	DIR_NORTH,
	ROOM_SHOP,
	"a shop",
	"######&&&######"	/* shop */
	"#....##!##....#"
	"#.............#"
	"#.............#"
	"#.............#"
	"###############"
  },
  { 	11, 6,
   	DIR_NORTH,
	ROOM_SHOP,
	"a shop",
	"#####!#####"	/* small shop */
	"#.........#"
	"#.........#"
	"#.........#"
	"#.........#"
	"###########"
  },
  { 	11, 6,
   	DIR_SOUTH,
	ROOM_SHOP,
	"a shop",
	"###########"	/* small shop */
	"#.........#"
	"#.........#"
	"#.........#"
	"#.........#"
	"#####!#####"
  },
  { 	8, 6,
   	DIR_EAST,
	ROOM_SHOP,
	"a shop",
	"########"	/* small shop */
	"#......#"
	"#......!"
	"#......#"
	"#......#"
	"########"
  },
  { 	8, 6,
   	DIR_WEST,
	ROOM_SHOP,
	"a shop",
	"########"	/* small shop */
	"#......#"
	"!......#"
	"#......#"
	"#......#"
	"########"
  },
  { 	21, 17,
   	DIR_NORTH,
	ROOM_CASTLE,
	"a castle",
	"&========|||========&"  /* small castle */
	"==########!########=="
	"=##......#.#......##="
	"=#....####.####....#="
	"=##+###.......###+##="
	"=#....#............#="
	"=#....#............#="
	"=#....#.....#####+##="
	"=#....#.....#......#="
	"=#....#.....#......#="
	"=#....#.....#......#="
	"=#....#.....#####+##="
	"=#....#.....#......#="
	"=#....+.....+......#="
	"=##...#.....#.....##="
	"==#################=="
	"&===================&"
  },
  { 	15, 10,
   	DIR_SOUTH,
	ROOM_MISC,
	"a farm",
	"#######&#######"
	"#.....#&#.....#"
	"#.....#&#.....#"
	"##+####&#.....#"
	"&&&&&&&&#.....#"
	"##+####&###!###"
	"#.....#&&&&&&&&"
	"#.....#&&&&&&&&"
	"#.....#&&&&&&&&"
	"#######&&&&&&&&"
  },
  { 	7, 6,
   	DIR_SOUTH,
	ROOM_MISC,
	"a house",
	"#######"
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"##!####"
  },
  { 	7, 8,
   	DIR_NORTH,
	ROOM_MISC,
	"a house",
	"&##!##&"
	"##...##"
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"##...##"
	"&#####&"
  },
  { 0, 0, 0, 0, NULL, NULL }	/* end of the list */
};



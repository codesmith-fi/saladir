/* 
 * Legend of Saladir 
 * (C)1997/1998 by Erno Tuomainen
 * 
 * variable.cc - global variables
*/

#include "variable.h"

/* global game data, must be saved */
Tgamedata GD;

#include "_version.h"

char SALADIR_VERSTRING[] = 
"Legend of Saladir " PROC_VERSION PROC_PLATFORM " (C)1997/98 Erno Tuomainen";

char tempstr[TEMP_MAX*2];
char nametemp[TEMP_MAX];
char nametemp2[TEMP_MAX];
char player_killer[TEMP_MAX];
char i_piletxt[40];
char i_manytxt[20];
char i_stattxt[100];
char i_identxt[100];
char i_pricetxt[100];
char i_hugetmp[1024];

bool GAME_NOT_ALIVE;
bool GAME_CHEATER;

bool global_initmode;

//_gameconfig CONFIGVARS;

/* line of sight distance */
int16u DELTA;

int32u GAME_NOTIFYFLAGS;

// this holds the player info
playerinfo player;
level_type *c_level;

// lreg variables hold the last map region which was
// printed out!
//int16s	lreg_x, lreg_y;

/* these will be initialized during startup time */
int16u num_artifacts;
int16u num_weapons;
int16u num_armors;
int16u num_npcraces;
int16u num_shopkeepers;
int16u num_valuables;
int16u num_classes;
int16u num_scrolls;
int16u num_materials;
int16u num_lairs;
int16u num_dungeons;
int16u num_rooms;
int16u num_miscitems;
int16u num_specials;

int16u myoutputdelay;

// message class
Message msg;

// time
Gametime passedtime;
Gametime worldtime;
Weather weather;

/* HACK FOR EDITOR */
int8u editerrain;
bool editmode;
bool drawmode;

volatile	sint err_sigint;
volatile	sint err_sigsegv;
volatile	sint err_sigfpu;


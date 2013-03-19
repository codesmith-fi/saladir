/* variable.h */

/* global variables */

#ifndef _VARIABLE_H_DEFINED
#define _VARIABLE_H_DEFINED

#include "saladir.h"

#include "output.h"
#include "types.h"
#include "defines.h"
#include "weather.h"
//#include "version.h"

extern Tgamedata GD;

extern bool global_initmode;

extern char SALADIR_VERSTRING[];

extern bool GAME_NOT_ALIVE;
extern bool GAME_CHEATER;

extern char TXT_ITEMPICKUP[];
extern char player_killer[TEMP_MAX];
extern int16u DELTA;
extern level_type *c_level;
extern playerinfo player;
//extern int16s lreg_x, lreg_y;
extern int16u num_weapons, num_miscitems,
   num_artifacts, num_lairs,
   num_rooms,
   num_armors,
   num_scrolls,
   num_dungeons,
   num_specials,
   num_valuables, num_shopkeepers,
   num_classes, num_npcraces,
   num_materials;
extern class Message msg;
extern class Gametime passedtime;
extern class Gametime worldtime;

extern class Weather weather;

extern char tempstr[400];
extern char nametemp[200];
extern char nametemp2[200];
extern int32u GAME_NOTIFYFLAGS;
extern int16u myoutputdelay;
extern int8u editerrain;
extern bool editmode;
extern bool drawmode;
extern char i_piletxt[40];
extern char i_manytxt[20];
extern char i_stattxt[100];
extern char i_identxt[100];
extern char i_pricetxt[100];
extern char i_hugetmp[1024];
extern volatile	sint err_sigint;
extern volatile	sint err_sigsegv;
extern volatile	sint err_sigfpu;

#endif

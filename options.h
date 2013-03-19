/*
 * options.h for the Legend of Saladir roguelike
 * (C) 1997/1998 by Erno Tuomainen
 *
 */

#ifndef _OPTIONS_H_DEFINED
#define _OPTIONS_H_DEFINED

/* option types */
#define CFG_END	    0
#define CFG_BOOLVAR 1
#define CFG_STRVAR  2
#define CFG_INTVAR  3
#define CFG_SINTVAR 4

/* type defining the option entry */
typedef struct {
      char *keyword;
      char *desc;
      void *ptr;
      int8u type;
      int16s min;
      int16s max;
} _Tcfginfo;

typedef struct
{
      bool DEBUGMODE;
      bool item_disturb;      // walk mode is disturbed by items
      bool monster_disturb;   // --    by monsters in sight
      bool stair_disturb;
      bool door_disturb;
      bool danger_disturb;
      bool autopickup;
      bool autodoor;
      bool autopush;
      bool repeatupdate;      // update while in repeat walk
      bool droppiles;
//      bool los_line;           // which line of sight type is used
      bool lightmonster;
      bool foodwarn;
      bool getallmoney;       // if set, get all money at once
      bool showdice;	      // show damage dice after every throw
      bool inversesight;      // show visible areas inversed
      bool anykeymore;	      // if set any key will continue when (more)
      bool compactmessages;
      bool colormessages;
      bool colortext;
      bool showlight; 
      bool cmdsort;
      bool targetline;
      bool cheat;
      int16s health_alarm;    // alarm player when health goes below this
      int16s repeatcount;     // max number of repeats per command
      int16u ticksperminute;

      char pickuptypes[20];   // autopickup types

} _gameconfig;


/* public functions */
void game_config(void);
void config_init(void);
void config_listvars(void);

bool config_save(void);
bool config_load(void);

extern _gameconfig CONFIGVARS;

#endif /* options.h */

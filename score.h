/*
 * Score.h for Legend of Saladir
 *
 * (C)1997/1998 by Erno Tuomainen
 *
 */

#ifndef _SCORE_H_DEFINED
#define _SCORE_H_DEFINED

//#include "time.h"

/* 
 * version of the scorefile, *MUST* be incremented
 * whenever the score file format is changed! 
 */
#define SCORE_FILEVERSION 0x01

/* just another constant :-) */
#define CHECKSUM_START 0x7f2eb21a

typedef struct
{
      /* name of the player */
      char name[NAMEMAX+1];      
      char title[TITLEMAX+1];
      char username[UNIX_USERNAMELEN+1];
      char deathreason[TEMP_MAX+1];

      int32u checksum;
      int32u final;

      time_t time;

      int32u copper; /* how much copper player had */      
      int32u moves;  /* number of moves player took */
      int32u exp;    /* number of experience gained */
      int16u quests; /* number of quests completed */
      int16u places; /* number of places visited */
      int16u levels; /* number of dungeon levels visited */
      int16u kills;
      int16u prace;   /* race of the player */
      int16u pclass;  /* class of the player */

      int16u position;
      int16u dindex;  /* dungeon index (where died) */

      int8u level;   /* player level */      
      int8u psex;
      
} Tscore;

time_t score_calculate(void);
bool score_showbest(int16u count, int16u personal, time_t current);

#endif

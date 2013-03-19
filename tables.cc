/*
** Tables.cpp for Saladir
** (C)1997 Erno Tuomainen
*/

#include "tables.h"

/* 
 * table for keypad movements. They are x and y delta values.
 * this table is also used for monster movement (random)
 * (actually these are used in *many* places )
 */

int16s move_dx[] = { 0, -1, 0, 1, -1, 0, 1, -1,  0,  1, 0, 0, 0 };
int16s move_dy[] = { 0,  1, 1, 1,  0, 0, 0, -1, -1, -1, 0, 0, 0 };

/*
 * directions and their reverse directions 
 * these tables are used by the pathfinder 
 */
int8u dir_normal[]= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
int8u dir_invers[]= { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

char *name_syllables[]=
{
 "jas", "min", "on", "bal", "ta", "zar",
 "men", "thol", "su", "vi", "er", "no",
 "spar", "hawk", "ke", "mi", "ber", "he",
 "dos", "sal", "adir", "na", "tash", "ha",
 "inka", "cae", "sar", "sha", "tho",
 "bel", "gar", "ion", "ath", "pol", "gara",
 "sau", "ron", "gand", "alf", "bil", "bo",
 NULL
};


char *syllables[]=
{
   "a", "am", "ain", "aro", "anx", "ax", "ayx",
   "boo", "blah", "benz", "in", "gaz", "olr", "ne",
   "cha", "too", "min", "olth", "rez", "read", "me", "elp",
   "ix", "moron", "utns", "alm", "elm", "ilm", "dona", "dina",
   "six", "in", "nid", "nim", "hop", "melz", "janz", "as",
   "yrn", "asm", "min", "tho", "mas", "moon", "lite", "ern", "otu",
   "omain", "en", "sal", "adir", "leg", "end", "dom", "ains",
   "cux", "bet", "ter", "ton", "not", "to", "tak", "mut",
   "de", "dip", "wat", "rand", "xo", "mel", "pid",
   "eiz", "ex", "epat", "ena", "ble", "fuc", "byt", "me",
   "ux", "ex", "pox", "xep", "soc", "ra", "tes", "uni", "hop",
   NULL
};

/*
** How dexterity changes speed (time units consumination)
** calculated in init.cpp
*/
int16s DEX_SPEED[101];

/* constitution to hit points regeneration */
int32u CON_HITP[101];

int16u SKILL_ADV[101];

_Ttactic tacticeffects[]=
{
	{ -4, 70, -30, 8, 30 },
	{ -2, 80, -20, 6, 20 },
	{ -1, 90, -10, 4, 10 },
	{  0, 100, 0, 0, 0 },
	{  1, 100, 5, -4, -10 },
	{  2, 100, 10, -6, -20 },
	{  4, 100, 20, -8, -30 },
};



/*
real COSTABLE[362];
real SINTABLE[362];


int16u los_table_size;
int16s los_tx_max[LOS_MAXARRAY];
int16s los_ty_max[LOS_MAXARRAY];
int16s los_tx_min[LOS_MAXARRAY];
int16s los_ty_min[LOS_MAXARRAY];
*/

//Tprecalcline LOS_LINETABLE[MAX_LOSLINES+1];


int8u move_dirpriority_flee[8][8]=
{

   { 7, 8, 4, 9, 1, 3, 6, 2 },	/* flee direction priority */
   { 3, 6, 2, 9, 1, 7, 8, 4 },
   { 9, 8, 6, 7, 3, 1, 4, 2 },
   { 1, 4, 2, 7, 3, 9, 8, 6 },
   { 1, 3, 2, 4, 6, 8, 7, 8 },
   { 7, 9, 8, 4, 6, 2, 1, 3 },
   { 9, 3, 6, 8, 2, 4, 7, 1 },
   { 7, 1, 4, 8, 2, 6, 9, 3 },
/*
	{ 7, 8, 4, 9, 1, 6, 2, 3 },
   { 3, 6, 2, 9, 1, 8, 4, 7 },
   { 9, 8, 6, 7, 3, 4, 2, 1 },
   { 1, 4, 2, 7, 3, 8, 6, 9 },
   { 1, 3, 2, 4, 6, 7, 8, 9 },
   { 7, 9, 8, 4, 6, 1, 3, 2 },
   { 9, 3, 6, 8, 2, 7, 1, 4 },
   { 7, 1, 4, 8, 2, 9, 3, 6 },
*/
};

int8u move_dirpriority_attack[8][9]=
{
   { 3, 6, 2, 9, 1, 8, 4, 7 },
   { 7, 8, 4, 9, 1, 6, 2, 3 },
   { 1, 4, 2, 7, 3, 8, 6, 9 },
   { 9, 8, 6, 7, 3, 4, 2, 1 },

   { 8, 7, 9, 4, 6, 1, 3, 2 },
   { 2, 1, 3, 4, 6, 7, 8, 9 },
   { 4, 1, 7, 8, 2, 9, 3, 6 },
   { 6, 3, 9, 8, 2, 7, 1, 4 },
};


int8u move_dirpriority_circulate[8][9]=
{
   { 3, 6, 2, 9, 1, 8, 4, 7 },
	{ 7, 8, 4, 9, 1, 6, 2, 3 },	/* flee direction priority */
   { 1, 4, 2, 7, 3, 8, 6, 9 },
   { 9, 8, 6, 7, 3, 4, 2, 1 },

   { 8, 7, 9, 4, 6, 1, 3, 2 },
   { 2, 1, 3, 4, 6, 7, 8, 9 },
   { 4, 1, 7, 8, 2, 9, 3, 6 },
   { 6, 3, 9, 8, 2, 7, 1, 4 },
/*
   { 2, 1, 3, 4, 6, 7, 8, 9 },
   { 8, 7, 9, 4, 6, 1, 3, 2 },
   { 4, 1, 7, 8, 2, 9, 3, 6 },
   { 6, 3, 9, 8, 2, 7, 1, 4 },

   { 9, 8, 6, 7, 3, 4, 2, 1 },
   { 1, 4, 2, 7, 3, 8, 6, 9 },
	{ 7, 8, 4, 9, 1, 6, 2, 3 },
   { 3, 6, 2, 9, 1, 8, 4, 7 },
*/
};



/* raiselev.cpp */

#ifndef _RAISELEV_H_DEFINED
#define _RAISELEV_H_DEFINED


#include "saladir.h"

#include "output.h"

#include "types.h"
#include "defines.h"
#include "tables.h"
#include "monster.h"
#include "utility.h"


#define MAXNUM_LEVELS	100

void gain_experience(level_type *level, _monsterlist *mptr, int32u gain);
void player_checkexpneeded(void);
bool player_checklevelraise(void);
void monster_advancelevels(_monsterlist *mptr);
bool monster_checklevelraise(level_type *level, _monsterlist *monster,
			     bool initmode);
void init_exptable(void);
extern int32u expneeded[MAXNUM_LEVELS+1];


#endif

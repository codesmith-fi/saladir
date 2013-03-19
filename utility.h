/* utiity.h */
#ifndef _UTILITY_H_DEFINED
#define _UTILITY_H_DEFINED

#include "saladir.h"

#include "output.h"
#include "types.h"
#include "tables.h"
#include "dungeon.h"
#include "generate.h"


/* utility.cpp */
void calculate_itembonus(_monsterlist *);
void game_noticeevents(level_type *);
void set_stat_initial(_statpack *pstat, int16s value, int16s max, int16s min);
void change_stat_initial(_statpack *pstat, int8s amount, bool player);
void change_stat_perm(_statpack *pstat, int8s amount, bool player);
void change_stat_temp(_statpack *pstat, int8s amount, bool player);
int16s get_stat(_statpack *pstat);
void calculate_hp(_hpslot *hpack, int16s basehp, int16u race);
int16s calculate_totalhp(_hpslot *hpack);
int16s calc_time(int16s speed, int16s time, int8u tactic);

//int16s calculate_slotac(int8u bodypart, _Tequipslot *eqslots );
//int16s calculate_slot_pv(int8u bodypart, _Tequipslot *eqslots );
int32u calc_carryweight(_statpack *stats, int32u weight);

void change_alignment(int16s *align, char dir, int16s amount);


#endif

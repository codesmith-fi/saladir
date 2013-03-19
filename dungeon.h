/* dungeon.cpp */

#ifndef _DUNGEON_H_DEFINED
#define _DUNGEON_H_DEFINED

/* result codes for move_item() */
#define BLOCKED_MONSTER 1
#define BLOCKED_WALL    2

void monster_torchlos(level_type *, _monsterlist *);
void crush_items(level_type *level, int16s x, int16s y, Tinvpointer nothis);

Tinvpointer largestitem(level_type *level, int16u x, int16u y);

inline void output_type(int16u, int16u);
inline bool output_items(level_type *, int16u, int16u);
bool output_monsters(level_type *, int16u, int16u);
bool new_output_monsters(level_type *level);
bool new_output_items(level_type *level);

int16s printmaze(level_type *, int16s, int16s);
void showplayer(level_type *);
void show_at(level_type *, int16s x, int16s y);

int8u moveplayer(int8u, level_type *, bool);
void repeatmove(int8u, level_type *);
int8u sur_countall(level_type *, int16u, int16u);
int8u sur_countpass(level_type, int16u, int16u);
int8u sur_countpass_dia(level_type *, int16u, int16u);
int16u distance(int16s, int16s, int16s, int16s);
void showstatus(bool);
void do_redraw(level_type *);
_monsterlist *istheremonster(level_type *, int16u, int16u);
Tinvpointer giveiteminfo(level_type *, int16u, int16u);
void noticestuff(level_type *level);
void game_passturn(level_type *, bool, bool);
int8s age_weapon(item_def *item, int16s luck, bool player);
bool player_has_los(level_type *, int16s, int16s);
int8u move_item(level_type *level, Tinvpointer item, int8u dir, 
	       bool checkmonster);

#endif /* dungeon.h */

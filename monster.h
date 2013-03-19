/* monster.h */

#ifndef _MONSTER_H_DEFINED
#define _MONSTER_H_DEFINED

void monster_getangry(level_type *level, 
		      _monsterlist *monster, _monsterlist *target);
void monster_shouldflee(level_type *level, _monsterlist *mptr);

void monster_gainedexperience(level_type *, _monsterlist *, _monsterlist *);
bool monster_useitems(level_type *level, _monsterlist *monster);
void monster_talk(_monsterlist *monster);
void monster_sprintf(char *dest, _monsterlist *mptr, 
		     bool upcase, bool nameonly);
char *monster_takename(_monsterdef *monster);
int16s move_monster(_monsterlist *, level_type *);
void handle_monsters(level_type *, int16u slots);
void act_specialmonster(_monsterlist *, level_type *);
int16s cansee(level_type *, int16s, int16s, int16s, int16s, int16u, bool);
void monster_chatspecials(_monsterlist *);
void monster_showdesc(_monsterlist *);
void monster_checkstat(_monsterlist *, level_type *);
//void generate_corpse(level_type *, _monsterdef *, int16u, int16u);
void generate_corpse(level_type *, _monsterlist *);
//int16s monster_takedamage(level_type *, _monsterlist *, _monsterlist *, 
//			  int16u, int8s bodypart);
void monster_cleantargets(level_type *, _monsterlist *);
void eat_specialmonster(level_type *level, int16u);
void eat_billgates(level_type *level);
void monster_checkturn(_monsterlist *, level_type *);
int8u monster_moveFROMtarget(level_type *, int16s *, int16s *, int16s, int16s);
int8u monster_moveTOtarget(level_type *, int16s *, int16s *, 
			   int16s, int16s, bool);


#endif /* monster.h */


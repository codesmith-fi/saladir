/* player.h */

#ifndef _PLAYER_H_DEFINED
#define _PLAYER_H_DEFINED

void player_showstatus(void);
bool player_push(level_type *level, int8u);
void player_rangedattack(void);
void player_killedmonster(_monsterlist *mptr);

void player_quickskill(level_type *level, int16u slot);
bool player_useskill(level_type *level, int16u group, int16s skill);

bool item_isweapon(item_def *item);
bool item_isarmor(item_def *item);

void player_handlestatus(level_type *level, int32u ptime);
bool player_inroom(level_type *level);
int8u dir_askdir(char *, bool);
//void remove_all();
void player_hitwall(level_type *level);
//int16s player_takehit(level_type *level, int16s, int8s, char *);
void player_checkstat(level_type *level, bool, bool);
void player_init();
void player_meleeattack(level_type *, _monsterlist *);
//int16s player_getmeleehit(level_type *level,  char *message,
//			  int16u damage, int8u bodypart, 
//			  _monsterlist *monster);
void player_chat(void);
void player_lookdir(void);
//item_info *level_listitems(level_type *, int16u, int16u, char *);
//void level_pileitems(level_type *);
void player_handleitems(void);
//inv_info *player_listitems(char *, int16s, int16s *);
void player_dropitem(void);
void player_showinv(void);
void player_getitem(void);
void player_eat(void);
void player_go_down(level_type **);
void player_go_up(level_type **);
void player_equip(void);
void player_opendoor(void);
void player_closedoor(void);
void player_paybill(void);
void melee_learnskills(Tskillpointer *skills, item_def *iptr, int8u amount);
void player_changetactics(void);
int16u calculate_meleehit(item_def *iptr, _monsterlist *attack, 
			  _monsterlist *target, int8u bpdiff);
bool decide_meleeweapon(_Tequipslot *equip, bool *useleft, bool *useright);

void player_go_outworld(level_type **level);

bool player_autopickup(level_type *level);

void item_printinfo(item_def *, int32u, int32u, char *);
_playerptrlistdef *changecategory(_playerptrlistdef *, int16s);
_playerptrlistdef *player_buildlist(int16s);

void player_readscroll(void);

void freeall_player(void);

#endif

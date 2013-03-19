/* generate.h */
#ifndef _GENERATE_H_DEFINED
#define _GENERATE_H_DEFINED



bool set_terrain(level_type *level, int16s x, int16s y, int8u type);
int8u freelevel(level_type *);
bool initlevel(level_type *, int8u, int16u, int16u);
bool createlevel(level_type *);
void recurse_maze(level_type *, int16u, int16u);
void roomylevel(level_type *);
void inititems(void);
void listitems(void);
void removeitems(level_type *);
void showitems(level_type *);
void createstairs(level_type *);
_monsterlist *monster_initnode(level_type *level);
_monsterlist *monster_add(level_type *, int16u, int16u, int8u type);
void monster_removeall(level_type *);
void monster_listall(level_type *);
int8u countchars(level_type *, int8u, int16u, int16u);
bool inline ispassable(level_type *, int16s, int16s);
bool haslight(level_type *level, int16s x, int16s y);
void level_removemonster(level_type *, _monsterlist *);

#ifdef _OLDINVENTORY
item_info *level_createitem(level_type *, int16u, int16u, int8u);
int16s level_removeitem(level_type *, item_info *, int16s count);
item_info *level_inititem(level_type *level);
item_info *add_miscitem(level_type *level, int16s x, int16s y, int16s type);
item_info *add_specialitem(level_type *level, int16s x, int16s y, int16s type);
item_info *add_scroll(level_type *level, int16s x, int16s y, 
	     int16s scrolltype);
item_info *add_fooditem(level_type *level, int16s x, int16s y);
void add_weapon(item_def *);
void add_armor(item_def *);
void add_valuable(item_def *);
int16s level_add_valuables(level_type *level, int16u x, int16u y, 
				  int16s count);
#endif

char *generate_scrollname(void);
_monsterlist *monster_addspecial(level_type *level);
void generate_wildhunt(level_type *level);
void monster_cleanmeup(_monsterlist *monster);
int32s get_randomitem(int16u group);

#endif /* generate.h */

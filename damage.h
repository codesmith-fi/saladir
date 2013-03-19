/* damage.h */

#ifndef _DAMAGE_H_DEFINED
#define _DAMAGE_H_DEFINED

void damage_checkbodyparts(level_type *level, _monsterlist *mptr);
int16s damage_issue(level_type *level, 
		    _monsterlist *target, _monsterlist *attacker,
		    int8u element, int16s damage, int16s bodypart,
		    char *message);

#endif



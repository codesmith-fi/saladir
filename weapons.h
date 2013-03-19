/*
 * weapon.h
 *
 */

#ifndef _WEAPON_H_DEFINED
#define _WEAPON_H_DEFINED

bool ranged_line(level_type *level, int16u out, int16u delay, bool single,
		 int16s x1, int16s y1, int16s x2, int16s y2,
		 bool (*hitfunc)(level_type *, _monsterlist *monster,
				 int16s, int16s, bool, int16u),
		 _monsterlist *monster, int16u skill);

bool ranged_gettarget(level_type *level, int16s *, int16s *,
		      int16s *, int16s *);

bool ranged_checkhit_player(level_type *level, _monsterlist *monster, 
			    int16s x, int16s y, bool visible, int16u skill);

bool ready_newmissile(Tinventory *inv, _Tequipslot *equip, 
		      int16u type, int16u group);
bool get_rangedskill(_monsterlist *monster, int16u *group, int16u *skill);

#endif /* weapon.h */

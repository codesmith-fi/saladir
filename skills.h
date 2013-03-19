/* skills.h */

#ifndef _SKILLS_H_DEFINED
#define _SKILLS_H_DEFINED

#define SKILLGRP_ALL		0
#define SKILLGRP_WEAPON 	1
#define SKILLGRP_GENERIC 	2
#define SKILLGRP_MAGIC  	3
#define SKILLGRP_ENDLIST        0xffff

/* these must be same as the weapon groups!!! */
#define SKILL_HAND      WPN_HAND
#define SKILL_DAGGER    WPN_DAGGER
#define SKILL_SWORD     WPN_SWORD
#define SKILL_AXE       WPN_AXE
#define SKILL_BLUNT     WPN_BLUNT
#define SKILL_POLEARM   WPN_POLEARM
#define SKILL_STAFF     WPN_STAFF
#define SKILL_BOW       WPN_BOW
#define SKILL_CROSSBOW  WPN_CROSSBOW
#define SKILL_THROW     9
#define SKILL_1HWEAP    10
#define SKILL_2HWEAP	11
#define SKILL_2WEAPCOMB	12
#define SKILL_SHIELD    13
#define SKILL_ENDLIST   0xffff
#define NUM_WEAPONSKILLS SKILL_1HWEAP

/* general skills */
#define SKILL_FOODGATHER        0
#define SKILL_OBSERVATION       1 /* magic skill */
#define SKILL_ALTERATION        2 /* magic skill */
#define SKILL_MYSTICISM         3 /* magic skill */
#define SKILL_DESTRUCTION       4 /* magic skill */
#define SKILL_CONCENTRATION     5 /* magic */
#define SKILL_MANAFLOW          6
#define SKILL_HEALING           7
#define SKILL_FINDWEAKNESS      8
#define SKILL_SEARCHING         9
#define SKILL_DISARMTRAP        10

#define SKILL_MAXLEV	10	/* levels 0..10 per skill */

#define MAX_SKILLSCORE	100	/* skills are 0..100% */
#define MIN_SKILLSCORE	5

/* flags 0x01 - 0x08 are taken by magic */
#define SKILLAUTO       0x00000010 /* automatic skill */
#define SKILLAPPLY      0x00000020 /* manual use skill */

typedef struct
{
      int16u group;	// skill group
      int16u type;	// skill type inside that group
      
      int16s ini;		// initial value
      int16s cur;		// learned value
      int16s min;		// minimum value
      int16s max;		// maximum value
      
      int16u raise;	// raise counter
      int8u dice_t;	// raise dice
      int8u dice_s;
      int8u level;	// skill level
} Tskill;


typedef struct _skillnode *Tskillpointer;

typedef struct _skillnode
{
	Tskill s;
	Tskillpointer next;
} Tskillnode;

typedef struct
{
      char *name;
      const char *desc;
      int32u flags;
} Tskilltype;

bool skill_removeall(Tskillpointer *list);
Tskillpointer skill_addnew(Tskillpointer *list, int16u group, int16u type, int16s value);
bool skill_modify(Tskillpointer *skills, int16u group, int16u type, int16s value, bool addit);
bool skill_modify_raise(Tskillpointer *skills, int16u group, 
			int16u type, int16u amount, int16u addval);

int16u skill_check(Tskillpointer list, int16u group, int16u type);
bool skill_list(Tskillpointer list, int16u);
bool skill_apply(Tskillpointer list);
int16s skill_listselect(int16u group, char *prompt);
int16s skill_listselect2(Tskillpointer skills, int16u *group, char *prompt);
void init_spellskills(void);
bool skill_init(Tskillpointer *list);

#endif

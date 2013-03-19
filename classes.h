/*
xo	classes.h

   */

#ifndef _CLASSES_H_DEFINED
#define _CLASSES_H_DEFINED


#define CLASS_NOCLASS	0	/* classless monster */
#define CLASS_NINJA		1
#define CLASS_FIGHTER	2
#define CLASS_KNIGHT		3
#define CLASS_MAGE		4
#define CLASS_PRIEST		5
#define CLASS_PALADIN	6
#define CLASS_SHAMAN		7
#define CLASS_NECROMANCER 8
#define CLASS_CAVEMAN	9
#define CLASS_RANGER		10
#define CLASS_FARMER		11
#define CLASS_ROGUE		12
#define CLASS_MERCHANT	13
#define CLASS_TOURIST	14
#define CLASS_HEALER		15
#define CLASS_HUNTER		16

typedef struct
{
	char *name;
	void (*initfunc)(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp);

} Tclassdef;

extern Tclassdef classes[];


#endif

/* traps.h */

#ifndef _TRAPS_H_DEFINED
#define _TRAPS_H_DEFINED

#define TRAP_NOTRAP  0
#define TRAP_BOULDER 1
#define TRAP_BOMB    2
#define TRAP_ESHOCK  3
#define TRAP_MAXNUM  3

#define TRAPF_DOORTRAP 0x0001
#define TRAPF_CAVETRAP 0x0002
#define TRAPF_NOREMOVE 0x0004 /* do not remove the trap when goes on */

typedef struct 
{
      const char *name;
      int16u out;
      int16u color;
      int16u flags;
} Ttrap;

/* prototypes */
int8u get_dir_to(level_type *level, int16s x1, int16s y1, 
		 int16s x2, int16s y2);
bool handletrap(level_type *level, int16u dx, int16u dy, 
		_monsterlist *monster);
void trap_create(level_type *level, int16u x, int16u y, int16s type);
void trap_makevisible(level_type *level, int16u x, int16u y);
void trap_remove(level_type *level, int16s dx, int16s dy);

extern Ttrap list_traps[];

#endif


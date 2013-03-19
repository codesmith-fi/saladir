/* status.h */

#ifndef _STATUS_H_DEFINED
#define _STATUS_H_DEFINED

/* conditions */
#define CONDIT_BLOATED    0
#define CONDIT_SATIATED   1
#define CONDIT_HUNGRY     2
#define CONDIT_STARVING   3
#define CONDIT_FAINTING   4
#define CONDIT_FAINTED    5
#define CONDIT_BURDENED   6
#define CONDIT_STRAINED   7
#define CONDIT_OVERLOADED 8
#define CONDIT_CONFUSED   9
#define CONDIT_BLESSED    10
#define CONDIT_CURSED     11
#define CONDIT_STUN       12
#define CONDIT_LUCKY      13
#define CONDIT_BLEEDING   14
#define CONDIT_POISONED   15
#define CONDIT_PARALYSED  16
#define CONDIT_BADLEGS    17
#define CONDIT_BADLARM    18
#define CONDIT_BADRARM    19
#define CONDIT_DEAD       20
#define CONDIT_MAX        20 

/* condition groups */
#define CONDGRP_NOTSET    0
#define CONDGRP_FOOD      1
#define CONDGRP_PW        2

/* condition flags */
#define CONDF_NOVIS    0x0001 /* don't show on status line */

/* condition data struct */
typedef struct
{
      int16u type;
      int32s val;
      int32u time;
} Tcondition;

/* condition list pointer */
typedef struct _condlist *Tcondpointer;

/* struct for condition linked list node */
typedef struct _condlist
{
      Tcondition cond;
      Tcondpointer next;
} Tcondnode;

/* struct for condition description list */
typedef struct
{
      char *name;
      char *desc;
      int16u flags;
      int16s group;
      int16u color;     
} Tcond_desc;

/* prototypes for list functions */
void cond_init(Tcondpointer *list);
int16u cond_check(Tcondpointer list, int16u cond);
Tcondpointer cond_add(Tcondpointer *list, int16u cond, int16s value);
void cond_delete_group(Tcondpointer *list, int16u group);
void cond_delete(Tcondpointer *list, int16u cond);
void cond_removeall(Tcondpointer *list);
int16u cond_check(Tcondpointer list, int16u cond);
void cond_statshow(Tcondpointer list, int16u y);

//void cond_handle(Tcondpointer *list, int32u slots);

#include "defines.h"

void cond_handle(_monsterlist *monster, int32u slots);

#endif

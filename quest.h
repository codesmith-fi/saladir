/* quest.h */

#ifndef _QUEST_H_DEFINED
#define _QUEST_H_DEFINED

/* quest flags */
#define QFLAG_GENERATED  0x0001
#define QFLAG_SOLVED     0x0002
#define QFLAG_UNSOLVABLE 0x0004 
#define QFLAG_REWARDED   0x0008 /* quest has been rewarded */
#define QFLAG_DONOTASK   0x8000 /* add immendiately without asking */

/* quest solve conditions */
#define QNEED_ITEM      1  /* return an item to the giver */
#define QNEED_KILLNPC   2  /* kill a monster of certain race */
#define QNEED_MONSTER   3  /* kill some monster, not NPC */
#define QNEED_MEETNPC   4  /* must meet someone */

/* quest result codes */
#define QSTAT_OK        0
#define QSTAT_PREQUEST  1
#define QSTAT_ALREADYIN 2
#define QSTAT_SOLVED    3
#define QSTAT_ERROR    10

/* quests */
#define QUEST_THOMASBISKUP     0
#define QUEST_KILLGATES        1
#define QUEST_MEETSPARHAWK     2
#define QUEST_TOTALNUM         3

typedef struct
{
      char *name;      /* quest description */
      char *desc;      /* quest talk description */
      char *solvetxt;  /* shown after quest has been solved */
      int16u flags;
      int16u owner;    /* quest owner, who shall issue this quest */
      int16u qnum;     /* if npc has multiple quests, this is the order num*/
      int16s prequest; /* if quest is required to get this quest */
      int16u solve;    /* quest solve condition */
      int16u cond1;    /* for items this is group   
			  for monsters this is a race */
      int16u cond2;    /* for items this is item id,
			  for monsters this is NPC number */
      int8u rewardimm;  /* */
      int32s exp;      /* experience gain for completing the quest */

} Tquestdata;

typedef struct
{
      int16u id;      /* quest id */
      int16u monid;   /* who gave the quest, id number */
      int16u dungeon; /* dungeon where the quest was given */
      int16u level;   /* level of that dungeon */
      int16u flags;   /* conditions concering the quest */
      char giver[40]; /* name of the monster who gave this quest */
} Tquest;

typedef struct _questlist *Tquestpointer;

typedef struct _questlist
{
      Tquest quest;
      Tquestpointer next;
} Tquestnode;

Tquestpointer quest_createnode(Tquestpointer *list);
void quest_init(Tquestpointer *list);
Tquest *quest_check(Tquestpointer *list, int16u quest);
void quest_removeall(Tquestpointer *list);
bool quest_delete(Tquestpointer *list, int16u quest);
void quest_showall(Tquestpointer *list);
int8u quest_isavailable(Tquestpointer *qlist, int16u quest);
void quest_complete(Tquestpointer *qlist, int16u qnum);
int16u quest_getflags(int16u quest);


#include "defines.h"

int16s quest_unfinished(Tquestpointer *list, _monsterlist *monster);
int8u quest_add(Tquestpointer *list, _monsterlist *monster, int16u quest);
int16s quest_nextavail(Tquestpointer *list, _monsterlist *monster);
void quest_showdesc(int16u);
char *quest_showname(int16u);
bool quest_ownerdied(Tquestpointer *list, _monsterlist *mptr);
bool quest_checkkill(Tquestpointer *qlist, _monsterlist *mptr);
void quest_reward(Tquestpointer *qlist, int16u qnum);

#endif

/* talk.h */

#ifndef _TALK_H_DEFINED
#define _TALK_H_DEFINED

typedef struct
{
      char *word;
      void (*talk)(_monsterlist *monster);
} Twordlist;

/* talk packet, contains much information about the
   current situation 
*/
typedef struct
{
      _monsterlist *monster;
      level_type *level;      

} Ttalkpack;

void wordtalk(_monsterlist *monster, char *word);

#endif

/*
 * talk.cc for Legend of Saladir
 *
 * (C)1997/1998 Erno Tuomainen
 *
 */

#include "saladir.h"

#include "output.h"
#include "types.h"
#include "creature.h"

#include "monster.h"
#include "quest.h"

#include "talk.h"
#include "NPCtalk.h"

typedef struct _Swordfilt
{
      char *orig;
      char *replace;
} Twordfilter;


char *resp_unknown[]=
{
   "Sorry, I know nothing about that.",
   "Sorry, I really can't help you with that.",
   NULL
};

void talk_unknown(_monsterlist *monster)
{

   ww_print(resp_unknown[0]);
}

char *resp_noname[]=
{
   "I won't tell you my name.",
   "My name is not your business.",
   NULL
};

#define RESP_GREETKNOWCOUNT 4
char *resp_greetknow[]=
{
     "We meet again, %s.",
     "Oh, the world is so small, %s.",
     "Nice to see you again, %s!",
     "Where've you been, %s?",
     NULL
};

void talk_greet(_monsterlist *monster)
{
   if(monster->m.status & MST_KNOWN) {
      zprintf(resp_greetknow[RANDU(RESP_GREETKNOWCOUNT)], player.name);
   }
   else {
      monster->m.status|=MST_KNOWN; // monster has been met now
      zprintf("Nice to meet you %s, my name is %s.", 
	      player.name, monster->m.name);
   }
}

void talk_rumour(_monsterlist *monster)
{
   ww_print("Nothing new...");
}

void talk_quest(_monsterlist *monster)
{
   int16s qnum;

   /* one active quest per monster only */
   qnum=quest_unfinished(&player.quests, monster);
   if(qnum >= 0) {
      ww_print("You know, I already gave you a quest. Please try to finish "
	       "it first.\n");
      monster_sprintf(nametemp, monster, true, true);
      my_setcolor(C_CYAN);
      zprintf("%s has given you a quest \"%s\", which is still unfinished.", 
	      nametemp, quest_showname(qnum));
      return;
   }

   /* find next available quest */
   qnum=quest_nextavail(&player.quests, monster);

   if(qnum<0 || qnum>=QUEST_TOTALNUM) {
      ww_print("Sorry, I don't have any quests for you. "
	       "Maybe you should keep asking!");
      return;
   }

   quest_showdesc(qnum);

   if(!(quest_getflags(qnum) & QFLAG_DONOTASK)) {

      my_setcolor(CH_WHITE);
      my_printf("\n\nDo you accept this quest (Y/n)?");
      my_setcolor(C_WHITE);
      if(confirm_yn(NULL, true, false)) {
	 my_printf("\n");
	 quest_add(&player.quests, monster, qnum);
      }
      else
	 my_printf("\nOk, as you wish.\n");
   }
   else
      quest_add(&player.quests, monster, qnum);

}

/*
 * general wordlist, words regognized by most
 * creatures (not animals
 *
 */
char _filt_name[] = "name";
char _filt_quest[] = "quest";
char _filt_news[] = "news";

Twordlist words_general[]=
{
   { _filt_name, talk_greet },
   { _filt_news, talk_rumour },
   { _filt_quest, talk_quest },
   { NULL, NULL },
};

/* 
 * This table is to recognize multiple words which actually
 * mean the same thing in the game
 *
 * It contains pairs of words (char pointers),
 * original written word, new word
 *
 * The routine scans this table and compares the original written
 * word to the first word in each pair in this table. If match was
 * found the original word will be replaced with the second word
 * of the pair.
 *
 */
Twordfilter filtertable[] =
{
   { "name", _filt_name },
   { "greet", _filt_name },
   { "hello", _filt_name },
   { "hi", _filt_name },
   { "yo", _filt_name },
   { "work", _filt_quest },
   { "quest", _filt_quest },
   { "job", _filt_quest },
   { "news", _filt_news },
   { "rumour", _filt_news },
   { NULL, NULL }
};

/*
 * Replace "same meaning" words with one replacement
 * See the above table "filterwords"
 *
 * returns a pointer to a word to be used in chat
 *
 */
char *filteroutwords(char *word)
{
   Twordfilter *ptr;

   ptr = filtertable;

   while(ptr->orig) {
      if(my_stricmp(ptr->orig, word)==0) {
	 return ptr->replace;
      }
      ptr++;
   }

   /* no match - return the original */
   return word;
}

/*
 * Scan the wordlist given in parameter "wlist" search for a word "word".
 * returns pointer to a wordlist entry or NULL if no match found
 *
 */
Twordlist *wt_scanwordlist(Twordlist *wlist, char *word)
{
   char *response;
   response=NULL;

   if(!wlist)
      return NULL;

   while(wlist->word!=NULL) {
      if(my_stricmp(word, wlist->word)==0) {
	 if(wlist->talk==NULL) {
	    zprintf("Error: Talk function for word \"%s\" is NULL!\n", word);
	    return NULL;
	 }
	 else
	    return wlist;
      }

      wlist++;
   }

   return NULL;
}

void wordtalk(_monsterlist *monster, char *word)
{
   Twordlist *wptr;

   if(!word) {
      ww_print("Error: Illegal talk word (=NULL).\n");
      return;
   }

   /* filter out multiple greet words */
   if( my_stricmp(word, player.name)==0 )
      word = _filt_name;

   word = filteroutwords(word);
   if(!word) {
      ww_print("Error in filteroutwords() (returned NULL)\n");
      return;
   }

   /* is there a match in NPC's own wordlist */
   wptr = wt_scanwordlist( NPCchatinfo[monster->m.special].wordlist, word );
   if(wptr) {
      wptr->talk(monster);
      return;
   }

   /* if not, is there a match in the general wordlist */
   wptr = wt_scanwordlist( words_general, word );
   if(wptr) {
      wptr->talk(monster);
      return;
   }

   /* otherwise */
   talk_unknown(monster);
}



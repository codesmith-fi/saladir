/**************************************************************************
 * quest.cc --                                                            *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 12.05.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 12.05.1998                                         *
 *                                                                        *
 **************************************************************************
 *                                                                        *
 * (C) 1997, 1998 by Erno Tuomainen.                                      *
 * All rights reserved.                                                   *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 *                                                                        *
 **************************************************************************/

#include "saladir.h"
#include "output.h"

#include "scrolls.h"
#include "monster.h"
#include "file.h"
#include "textview.h"
#include "raiselev.h"
#include "quest.h"

/* list of special quests */
Tquestdata quest_list[]=
{
   { "Return Thomas Biskup to his own realm",
     "Hey, wonderful that you asked. You probably are wondering why I am "
     "wandering around in this world. And so am I.\nOne day when I was "
     "installing the latest design of my corruption trap in one of the "
     "dungeons of Drakalor Chain, the trap became really wild...\n"
     "...the next thing I noticed was that I was stuck in this place. I "
     "don't even know what place this is, but it surely looks boring when "
     "compared to my own realm. Dungeons are so primitive and huge, the "
     "creatures are stupid and can't do nothing but fight.\n"
     "Now, I want out! I want a way to get back to my own world. Please, "
     "if you could just help me, I can do you a great favour in exchange.",
     "You helped Thomas Biskup to get back to his own realm.",
     0, NPC_THOMAS, 0, -1, 
     QNEED_ITEM, IS_SCROLL, SCROLL_TELEPORT, 0, 2000 },
   { "Ruthlessly terminate Bill Gates",
     "<weep> You surely have heard that everyone is after me. My company is "
     "being "
     "hacked into pieces and nothing goes well with Windows2000. The life "
     "is pure misery for me, thus I am asking if you could do a big favor "
     "for me and to the whole world. Would you please kill me?",
     "You have ruthlessly killed Bill Gates.",
     0, NPC_BILLGATES, 0, -1, 
     QNEED_KILLNPC, 0, NPC_BILLGATES, 1, 1000
   },
   { "Meet the burly adventurer called Sparhawk",
     "Sparhawk is looking for you you should go and seek him out.",
     "You met Sparhawk.",
     QFLAG_DONOTASK, NPC_NATASHA, 0, -1, 
     QNEED_MEETNPC, 0, NPC_SPARHAWK, 0, 500 },

   { NULL, 0, 0, 0, 0, 0, 0 }
};

char qname_illegal[]="Illegal quest name!";

/* quest status flags, is it solved etc. this array will be saved */
//int16u quest_flags[QUEST_TOTALNUM]={ 0, 0 };

int16u quest_getflags(int16u quest)
{
     if(quest < QUEST_TOTALNUM)
	  return quest_list[quest].flags;
     else
	  return 0;
}

int8u quest_isavailable(Tquestpointer *qlist, int16u quest)
{
   Tquest *qp;

   qp = quest_check(qlist, quest);

   /* the quest was in the list, 
    * check if it is completed 
    */
   if(qp) {
      if( qp->flags & QFLAG_SOLVED )
	 return QSTAT_SOLVED;
      else
	 return QSTAT_ALREADYIN;      
   }

   if(quest_list[quest].prequest >= 0) {
      qp = quest_check(qlist, quest_list[quest].prequest);
      if(qp) {	 
	 if(!(qp->flags & QFLAG_SOLVED))
	    return QSTAT_PREQUEST;
      }

   }

   /* it's ok to add */
   return QSTAT_OK;
}

void quest_init(Tquestpointer *list)
{
   *list=NULL;
}

Tquest *quest_check(Tquestpointer *list, int16u quest)
{
   Tquestpointer qptr;

   qptr=*list;

   while(qptr) {
      if(qptr->quest.id==quest)
	 return &qptr->quest;

      qptr=qptr->next;
   }
   return ((Tquest *)NULL);
}

/* check if there're unfinished quests for the monster */
int16s quest_unfinished(Tquestpointer *list, _monsterlist *monster)
{
   Tquestpointer qptr;

   qptr=*list;

   while(qptr) {
      if(qptr->quest.monid==monster->id && !(qptr->quest.flags & QFLAG_SOLVED))
	 return qptr->quest.id;

      qptr=qptr->next;
   }

   return -1; 
}

/*
 * Create a node into the questlist
 *
 * NOTE! The node will be *EMPTY*!!!
 */
Tquestpointer quest_createnode(Tquestpointer *list)
{
   Tquestpointer nptr;

   nptr=new Tquestnode;

   if(nptr) {
      mem_clear(nptr, sizeof(Tquestnode));
      nptr->next=NULL;

      if(*list != NULL) {
	 nptr->next = *list;
      }
      
      *list=nptr;  
   }
   return nptr;
}

int8u quest_add(Tquestpointer *list, _monsterlist *monster, int16u quest)
{
   Tquestpointer newnode;
   int8u qstat;

   if(!monster) {
      msg.addwait("Error: quest_add() monster pointer is NULL!", CH_RED);
      return QSTAT_ERROR;
   }

   /* test if the quest is already in the list (so it has been 
    * either generated or completed since all generated quests
    * are in the players quest list
    */
   if(quest_check(list, quest)!=NULL)
      return QSTAT_ALREADYIN;

   /* test if the quest is available, ie. all prequisities are taken
    * care of
    */
   qstat=quest_isavailable(list, quest);

   if(qstat!=QSTAT_OK)
      return qstat;

   /* create a node */

   /* add a new node */
   newnode = quest_createnode(list);
   if(!newnode) {
      msg.addwait("Error: Can't add quest. No memory!", CH_RED);
      return QSTAT_ERROR;
   }


   /* init quest */
   newnode->quest.monid=monster->id;
   newnode->quest.id=quest;
   newnode->quest.level=player.levptr->index;
   newnode->quest.dungeon=player.dungeon;
   newnode->quest.flags |= QFLAG_GENERATED;
   
   monster_sprintf(nametemp, monster, false, true);
   my_strcpy(newnode->quest.giver, nametemp, sizeof(newnode->quest.giver));
   
   my_setcolor(C_CYAN);
   msg.vnewmsg(C_CYAN, "You are now involved in a quest: %s!", quest_list[quest].name);
   zprintf("You're now involved in a quest: %s!\n", quest_list[quest].name);

   return QSTAT_OK;
}

void quest_removeall(Tquestpointer *list)
{
   Tquestpointer del;

   while(*list) {
      del=*list;

      *list=(*list)->next;

      delete del;
   }

   quest_init(list);
}


bool quest_delete(Tquestpointer *list, int16u quest)
{
   Tquestpointer cptr, dptr, prev;

   dptr=cptr=*list;
   while(cptr) {
      prev=dptr;
      dptr=cptr;
      cptr=cptr->next;

      if(dptr->quest.id==quest) {

	 if(*list==dptr)
	    *list=(*list)->next;
	 else {
	    prev->next=dptr->next;
	 }
	 delete dptr;
	 return true;
      }
   }

   return false;
}

void quest_showall(Tquestpointer *list)
{
   FILE *fh;
   Tquestpointer qptr;
   int16u i=1;
   
   qptr=*list;

/*
   if(!qptr) {
      msg.newmsg("You are not involved in any quest.", C_WHITE);
      return;
   }
*/

   chdir_totempdir();
   fh=fopen(FILE_TMPTEXT, "w");

   if(!fh) {
      msg.newmsg("Error: unable to view quests.", CH_RED);
      return;
   }

   
   fprintf(fh, "\001You're involved in these quests:\n\n");

   i=1;
   while(qptr) {
      /* first write all unfinished quests */
      if(!(qptr->quest.flags & QFLAG_SOLVED)) { 
//	 !(qptr->quest.flags & QFLAG_UNSOLVABLE)) {
	 fprintf(fh, "\002%2d. \007%s.\n", i, quest_list[qptr->quest.id].name);
	 fprintf(fh, "    Given at %s level %d by %s\n", 
		 dungeonlist[qptr->quest.dungeon].name,
		 qptr->quest.level, qptr->quest.giver );
	 i++;
      }
      qptr=qptr->next;
   }
   if(i==1)
      fprintf(fh, "\002None.\n");

   fprintf(fh, "\n\001The quests you have solved by now:\n\n");

   qptr=*list;
   i=1;
   while(qptr) {
      /* first write all unfinished quests */
      if(qptr->quest.flags & QFLAG_SOLVED) {
	 fprintf(fh, "\002%2d. \007%s.\n", i, 
		 quest_list[qptr->quest.id].solvetxt);
	 fprintf(fh, "    It was given by %s at %s level %d.\n", 
		 qptr->quest.giver,
		 dungeonlist[qptr->quest.dungeon].name,
		 qptr->quest.level);
	 i++;
      }
      qptr=qptr->next;
   }
   if(i==1)
      fprintf(fh, "\002None.\n");


#ifdef QUESTUNSOLVABLESTUFF
   fprintf(fh, "\n\001Quests which are not solvable but you're involved!\n\n");

   qptr=*list;
   i=1;
   while(qptr) {
      /* first write all unfinished quests */
      if(qptr->quest.flags & QFLAG_UNSOLVABLE) {
	 fprintf(fh, "\002%2d. \007%s.\n", i, quest_list[qptr->quest.id].name);
	 fprintf(fh, "    Given at %s level %d by %s\n", 
		 dungeonlist[qptr->quest.dungeon].name,
		 qptr->quest.level, qptr->quest.giver );
      }
      qptr=qptr->next;
      i++;
   }
   if(i==1)
      fprintf(fh, "\002None.\n");
#endif

   fclose(fh);

   /* finally view the file */
   viewfile(FILE_TMPTEXT);
}

/*********************************/
/* END OF LIST RELATED FUNCTIONS */
/*********************************/

int16s quest_nextavail(Tquestpointer *qlist, _monsterlist *monster)
{
   int16s q=0, sq=-1;
   int16u lindx=0xffff;
   
   /* monster needs to be a special npc */
   if(monster->m.special == 0) {
      return -1;
   }

   my_printf("%d\n", monster->m.special);

   while(quest_list[q].name!=NULL) {
      if(monster->m.special == quest_list[q].owner) {
	 if(quest_isavailable(qlist, q)==QSTAT_OK) {
	    if(lindx>quest_list[q].qnum) {
	       lindx=quest_list[q].qnum;
	       sq=q;
	    }
	 }
      }
      q++;
   }
   
   return sq;
}

/*
 * Check if the player has any quests which involve killing
 * a monster. Again compare mptr if that monster is on the
 * kill list. 
 *
 */
bool quest_checkkill(Tquestpointer *qlist, _monsterlist *mptr)
{
   Tquestpointer qp;

   qp=*qlist;

   while(qp) {
      if( quest_list[qp->quest.id].solve == QNEED_KILLNPC) {
	 if(quest_list[qp->quest.id].cond2 == mptr->npc && mptr->npc>0 ) {
	    /* yes there was a NPC to be killed */

	    quest_complete(qlist, qp->quest.id);

	    if(quest_list[qp->quest.id].rewardimm) {
	       quest_reward(qlist, qp->quest.id);
	    }
	 }
      }
      qp=qp->next;
   }
   return false;
}

void quest_reward(Tquestpointer *qlist, int16u qnum)
{
   Tquest *qp;

   qp = quest_check(qlist, qnum);

   if(!qp) return;

   if(qp->flags & QFLAG_REWARDED) 
      return;

   /* ensure that the quest will not be rewarded again */
   qp->flags |= QFLAG_REWARDED;
   
   /* reward the player */
   if( quest_list[qnum].exp > 0) {
      msg.vnewmsg(CH_GREEN, "You gain %d exp for completing a quest.",
		  quest_list[qnum].exp);
      gain_experience(c_level, NULL, quest_list[qnum].exp);
   }
}

void quest_complete(Tquestpointer *qlist, int16u qnum)
{
   Tquest *qp;

   qp = quest_check(qlist, qnum);
   if(qp) {
      qp->flags |= QFLAG_SOLVED;

   }

//   quest_delete(qlist, qnum);
}

/* this is called every time a monster dies
 * removes all quests whose owner dies.
 */
bool quest_ownerdied(Tquestpointer *list, _monsterlist *mptr)
{
   Tquestpointer qptr;
   bool res=false;

   qptr=*list;

   while(qptr) {             
      if(!(qptr->quest.flags & QFLAG_SOLVED) && 
	 qptr->quest.monid == mptr->id) { 
//	 !quest_list[qptr->quest.id].rewardimm) {
	 msg.vnewmsg(CH_RED, "Quest \"%s\" cannot be completed since "
		     "the owner has just died.\n", 
		     quest_list[qptr->quest.id].name );

	 qptr->quest.flags |= QFLAG_UNSOLVABLE;

	 res=true;
      }
      qptr=qptr->next;
   }

   return res;
}

void quest_showdesc(int16u qnum)
{
   if(qnum>=QUEST_TOTALNUM) {
      my_printf("Error: Trying to query quest %d, which is not available (Max=%d)!\n",
		qnum, QUEST_TOTALNUM-1);
      return;
   }

   if(quest_list[qnum].desc!=NULL)
      ww_print(quest_list[qnum].desc);
   else
      my_printf("Error: quest %d has no description (=NULL)!\n", qnum);

}

char *quest_showname(int16u qnum)
{
   if(qnum>=QUEST_TOTALNUM) {
      my_printf("Error: Trying to query quest %d, which is not available (Max=%d)!\n",
		qnum, QUEST_TOTALNUM-1);
      return qname_illegal;
   }

   if(quest_list[qnum].name!=NULL)
      return quest_list[qnum].name;
   else
      return qname_illegal;   
}

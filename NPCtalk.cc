/*
 * NPC Talk information
 *
 */

#include "saladir.h"
#include "output.h"
#include "talk.h"
#include "creature.h"
#include "NPCtalk.h"

/*
 * NPC talk function prototypes 
 */
void NPCtalk_sparhawk_name(_monsterlist *);
void NPCtalk_sparhawk_town(_monsterlist *);
void NPCtalk_sparhawk_help(_monsterlist *);

/*
 * Special NPC Wordlists
 *
 */
Twordlist NPCwlist_sparhawk[] =
{
   { "help", NPCtalk_sparhawk_help },
   { "name", NPCtalk_sparhawk_name },
   { "town", NPCtalk_sparhawk_town },

   { NULL, NULL }
};

/*
 * A table for special NPC characters. Using this table
 * they greet players when a chat mode begins.
 *
 * NOTE: THERE MUST BE AS MANY STRUCTURES IN THIS ARRAY
 * as there is active NPC characters!!!!!!!!!!!!!!!!!!!!
 *
 */

Tchatinfo NPCchatinfo[]=
{
     /* greetings text for unknown monsters */
     { "Who're you and what do you want?", 
       "Why do you keep pestering me?", NULL 
     },
     /* billgates */
     { "", "", NULL 
     },
     /* sparhawk */
     { "( For a while you've have a chilled feeling. It's like this"
       " person can see through your soul)\n\n"
       "(With a booming voice) Greetings traveller, my name is Sparhawk."
       " From the distance I noticed that you could use some help. I am "
       "willing to help you but first, would you tell me who are you? ",
       "Hello again, how can I help you with?", 
       NPCwlist_sparhawk
     },
     /* Thomas biskup */
     { "", "", NULL 
     },
     /* Natasha */
     { "", "", NULL 
     },
     { NULL, NULL, NULL 
     }
};

/**********************************************************************/
/*                         CHAT FUNCTIONS                             */
/**********************************************************************/

char *txt_sparhawk_help[] =
{
   "I can provide you with quite a bit of information, just keep asking "
   "for help and I will try to aid you if possible. But first, a word or "
   "two on talking.\n\n "
   "When you're talking with people you can always talk about jobs, quests, "
   "news and such. Some persons can privide you with information on places "
   "and towns.\n\n"
   "You don't need to write whole phrases "
   "it's enough to just type the most important word. Also remember that "
   "it's always polite to greet the person before going into further "
   "discussion.",

   "I wouldn't suggest for you to go fighting without weapons, not "
   "unless you're very skilled in weaponless combat. Check out your "
   "equipment ('q') and equip whatever you might be carrying for now. "
   "If you have some gold with you why not visit some local shops here, "
   "they sell some pretty darn good equipment.",
  
   "Dungeons and caves are mostly dark. You need some light to see what "
   "is happening in there. I can see that you already have got something "
   "which can help you with that. Just equip it and you can do much better.",

   "Salmorria and all the towns are all using the same financial system. "
   "We have three types of coins: gold, silver and copper. A gold coin "
   "is worth of 10 copper coins and 2 silver coins. A silver coin is "
   "worth of 5 copper coins.",

   "Did you know there's a dungeon nearby to the northwest of this "
   "town. I've been in there myself, the habitants are not too "
   "aggressive and mostly quite easy to kill or evade. When you've "
   "prepared yourself, why not go and visit there.",

   "I've now told you everything I can help you with. When you meet "
   "new people you should try to talk with them. I hope you best with "
   "your travels and maybe we meet again.",

   NULL
};

void NPCtalk_sparhawk_help(_monsterlist *monster)
{
   static int hi = 0;

   ww_print(txt_sparhawk_help[hi++]);

   if(txt_sparhawk_help[hi]==NULL) {
      hi=0;
   }
}

void NPCtalk_sparhawk_town(_monsterlist *monster)
{
   ww_print("Santhel is a small town located in the western parts of "
	    "Sallmorria. We stay mostly by ourselves but occasionally "
	    "have some visitors from far east and other parts of the "
	    "continent. I suggest that you visit the local stores around "
	    "here before doing anything else, we have a decent supply "
	    "of goods and many merchants are staying here.");
}

void NPCtalk_sparhawk_name(_monsterlist *monster)
{
   /* identify sparhawk */
   if( !(monster->m.status & MST_KNOWN) ) {
      zprintf("Nice to meet you %s! ",
	      player.name);
      ww_print("It's been a while since we met a stranger in town.");

      monster->m.status |= MST_KNOWN;

      NPCtalk_sparhawk_help(monster);
   }
   else {
      zprintf("Hello again %s, I assume you're doing well.", player.name);
   }

}

/*
 * Scrolls.cc for the Legend of Saladir
 * (C) 1997/1998 Erno Tuomainen
 *
 * This contains scroll specific routines
 *
 */
#include "saladir.h"

#include "output.h"
#include "types.h"
#include "magic.h"

#include "player.h"
#include "scrolls.h"

/* local prototypes */
void scroll_rumour(void);
void scroll_blank(void);
void scroll_amnesia(level_type *level);


/*******************/
/* list of scrolls */
/*******************/
Tscroll list_scroll[]=
{
   { "my own", "", "", SCROLLGROUP_GENERIC, SCROLL_SELFWRITTEN, 
     100, SCFLAG_IDENTIFIED
   },
   { "blank paper", "", "", SCROLLGROUP_GENERIC, SCROLL_BLANK, 
     100, 0
   },
   { "rumour", "", "", SCROLLGROUP_GENERIC, SCROLL_RUMOUR, 
     100, SCFLAG_AUTOIDENTIFY
   },
   { "healing", "", "", SCROLLGROUP_MAGIC, SPELL_HEALING, 
     40, 0
   },
   { "teleporting", "", "", SCROLLGROUP_MAGIC, SPELL_TELEPORT, 
     40, 0 },
   { "cursing", "", "", SCROLLGROUP_MAGIC, SPELL_CURSE, 
     40, 0 },
   { "uncursing", "", "", SCROLLGROUP_MAGIC, SPELL_UNCURSE, 
     40, 0 },
   { "blessing", "", "", SCROLLGROUP_MAGIC, SPELL_BLESS, 
     40, 0 },
   { "confuzion", "", "", SCROLLGROUP_MAGIC, SPELL_CONFUZE, 
     40, 0 },
   { "identify", "", "", SCROLLGROUP_MAGIC, SPELL_IDENTIFY, 
     40, SCFLAG_AUTOIDENTIFY },
   { "amnesia", "", "", SCROLLGROUP_GENERIC, SCROLL_AMNESIA, 
     100, SCFLAG_AUTOIDENTIFY },
   { "magic mapping", "", "", SCROLLGROUP_MAGIC, SPELL_MAGICMAP, 
     40, SCFLAG_AUTOIDENTIFY },

   { NULL, "", "", 0, 0, 0, 0 },
};

char *txt_rumours[]=
{
   "Chaos is messing up the Drakalor Chain, adventurer needed! Contact Thomas"
    " for more info.",
   "Salmorrian mountains needs a good world designer, "
    "Contact ernomat@evitech.fi for more info.",
   "Isn't it nice to have a rumour scroll in every roguelike.",
   "You're being watched...",
   "Some monsters can be lethal.",
   "You can buy items inside shops.",
   "You can sell items inside shops, just drop them.",
   "Salmorrian mages are constantly developing new spells.",
   "You should try naming the scrolls you find.",
   "There's a huge ocean to the east.",
   "Cursing a shopkeeper doesn't help a lot.",
   "Don't take this note too seriously.",
   "You could really use some food and coffee occasionally.",
   "Try asking for a quest when talking with someone.",
   "Observation is the key to survivement.",
   NULL
};

bool scroll_readit(level_type *level, item_def *item)
{
//   item_info *iptr;

   if(item->type!=IS_SCROLL) {
      msg.newmsg("This item contains no writing.", C_WHITE);
      return false;
   }

   if(item->group != SCROLL_BLANK)
      msg.newmsg("The scroll disappears in a huge puff of smoke.", 
		 C_WHITE);
   /*
    * Identify autoidentify scrolls
    * but self written scrolls need not to be identified
    * 
    * There's no special flag for self written scroll,
    * just make sure that self written scrolls are identified
    * when they are created!
    *
    */
//   if( !(item->status & ITEM_SELFSCROLL) ) {

   if(!(item->status & ITEM_IDENTIFIED)) {
      if( list_scroll[item->group].flags & SCFLAG_AUTOIDENTIFY ) {
	 
	 item->status |= ITEM_IDENTIFIED;
	 list_scroll[item->group].flags |= SCFLAG_IDENTIFIED;

	 /* now identify all similar items in the current level */
//	 iptr=level->items;
//	 while(iptr) {
//	    if(iptr->i.type == IS_SCROLL &&
//	       iptr->i.group == item->group)
//	       iptr->i.status |= ITEM_IDENTIFIED;
//
//	    iptr=iptr->next;
//
//	 }	 

	 sprintf(tempstr, "This is a scroll of %s.", 
		 list_scroll[item->group].name);
	 msg.add(tempstr, C_YELLOW);
	 showmore(true, true);

      }
   }

   /* for magical scrolls */
   if(item->pmod1==SCROLLGROUP_MAGIC) {
      spell_zap(level, item->pmod2, item->pmod3, true);
   }
   else {

      /* for non magical scrolls */
      switch(item->group) {
	 case SCROLL_BLANK:
	    scroll_blank();
	    break;
	 case SCROLL_RUMOUR:
	    scroll_rumour();
	    break;
	 case SCROLL_AMNESIA:
	    scroll_amnesia(level);
	    break;

	 default:
	    msg.newmsg("Woo-opsis! Scroll is not yet implemented!", CH_RED);
	    break;
      }
   }


   return true;
}

void scroll_rumour(void)
{
   int16u num_rumours=0;
   
   /* count rumours first */
   while(txt_rumours[num_rumours]!=NULL)
      num_rumours++;

   msg.newmsg("The scroll contains a message:", C_WHITE);
   msg.newmsg(txt_rumours[RANDU(num_rumours)], C_GREEN);
}

#define NUM_BLANKMSG 4
char *txt_blankscrollmsg[]=
{
   "There's no writing on this scroll.",
   "The scroll is blank!",
   "Empty scrolls won't do much.",
   "You see absolutely nothing written on this scroll.",
   "It's an empty scroll, maybe your imagination helps?",
};

void scroll_blank(void)
{
   msg.newmsg(txt_blankscrollmsg[RANDU(NUM_BLANKMSG)], C_WHITE);

}

void scroll_amnesia(level_type *level)
{
   int16s x, y;
   int16u clearmask;

   clearmask=0xffff ^ (CAVE_SEEN | CAVE_TMPLIGHT | CAVE_LOSTMP);

   msg.newmsg("What? Who? Where? ... There's a hole in your mind!", C_WHITE);

   for(y=0; y<level->sizey; y++)
      for(x=0; x<level->sizey; x++) 
	 level->loc[y][x].flags &= clearmask;

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

}

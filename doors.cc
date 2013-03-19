/*
	doors.cpp

*/

#include "saladir.h"

#include "output.h"
#include "types.h"
#include "defines.h"
#include "caves.h"
#include "monster.h"
#include "utility.h"
#include "generate.h"
#include "dungeon.h"
#include "damage.h"
#include "traps.h"
#include "inventor.h"
#include "doors.h"

char *doormsg[]={
   "Squiik..",
   "Nkrriin..",
   "Iiiiii-iii..",
   NULL };

/* turn a secret door into a normal closed door */
bool door_secret2normal(level_type *level, int16s x, int16s y)
{
   if(x<1 || x>=level->sizex || y<1 || y>=level->sizey)
      return false;

   /* is it a secret door */
   if(level->loc[y][x].type != TYPE_DOORSECR)
      return false;

   /* yes make a normal door in there */

   set_terrain(level, x, y, TYPE_DOORCLOS);

   return true;
}

bool door_open2closed(level_type *level, int16s x, int16s y)
{
   if(x<1 || x>=level->sizex || y<1 || y>=level->sizey)
      return false;

   if(level->loc[y][x].doorfl & DOOR_OPEN) {
      set_terrain(level, x, y, TYPE_DOORCLOS);

      level->loc[y][x].doorfl=DOOR_CLOSED;
      return true;
   }

   return false;
}


bool door_closed2open(level_type *level, int16s x, int16s y)
{
   if(x<1 || x>=level->sizex || y<1 || y>=level->sizey)
      return false;

   if(level->loc[y][x].doorfl & DOOR_CLOSED) {
      set_terrain(level, x, y, TYPE_DOOROPEN);

      level->loc[y][x].doorfl=DOOR_OPEN;
      return true;
   }

/*
   if(level->loc[y][x].type == TYPE_DOORCLOS ||
      level->loc[y][x].type == TYPE_DOORTRAP ) {

      level->loc[y][x].type=TYPE_DOOROPEN;
      level->loc[y][x].flags=terrains[TYPE_DOOROPEN].flags;  
      level->loc[y][x].doorfl=DOOR_OPEN;
      return true;

   }
*/

   return false;
}

/* if special is set, allow creating secret door etc. */
bool door_create(level_type *level, int16s x, int16s y, bool special)
{
     int16u dl=0;


   if(x<1 || x>=level->sizex || y<1 || y>=level->sizey)
      return false;

   /* create a closed door */
   set_terrain(level, x, y, TYPE_DOORCLOS);
   level->loc[y][x].doorfl=DOOR_CLOSED;

   if(!special)
      return true;
  
   /* secret door shown with a wall '#' */
   if(RANDU(100) < CHANCE_SECRETDOOR) {
      set_terrain(level, x, y, TYPE_DOORSECR);
   }
   
   /* stuck doors */
   if(RANDU(100) < CHANCE_STUCKDOOR) {
      level->loc[y][x].doorfl |= DOOR_STUCK;
      level->loc[y][x].sval=50+RANDU(100);      
   }


   /* this will eventually calculate the chances for doors to 
    * be trapped
    */

   dl = player.levptr->danger;

   /* if null then get the default chance */
   if(!dl) 
	dl = CHANCE_DOORTRAP;

   if(RANDU(100) < dl) {
      /* create a random trap */
      trap_create(level, x, y, -1);
   }

   return true;
}

bool isitadoor(level_type *level, int16s x, int16s y)
{   
   if(x<1 || x>=level->sizex || y<1 || y>=level->sizey)
      return false;

/*
   if(level->loc[y][x].type==TYPE_DOORCLOS)
      return true;

   if(level->loc[y][x].type==TYPE_DOORTRAP)
      return true;

   if(level->loc[y][x].type==TYPE_DOOROPEN)
      return true;
*/
   if(level->loc[y][x].flags & CAVE_DOOR)
      return true;
   
   
   return false;   
}

bool is_secretdoor(level_type *level, int16s x, int16s y)
{
   if(x<1 || x>=level->sizex || y<1 || y>=level->sizey)
      return false;

   if(level->loc[y][x].type==TYPE_DOORSECR)
      return true;

   return false;
}


/* handle stuck door, return false if door didn't open */
bool door_handlestuck(level_type *level, int16u dx, int16u dy, 
		      _monsterlist *monster)
{
   int16u ustr=0;

   if(monster) {
      ustr=get_stat(&monster->stat[STAT_STR]) + 
	 get_stat(&monster->stat[STAT_TGH]);

   }
   else {
      ustr=get_stat(&player.stat[STAT_STR]) +
	 get_stat(&player.stat[STAT_TGH]);

   }
   ustr=ustr/2;

   if(level->loc[dy][dx].sval > ustr) {
      level->loc[dy][dx].sval-=ustr;
      if(!monster) {
	 msg.newmsg("The door seems to loose up a bit, but it's still stuck.", 
		    C_WHITE);
      }
      else {
	 monster_sprintf(nametemp, monster, true, true );
	 msg.vnewdist(level, monster->x, monster->y, C_YELLOW, C_YELLOW,
		      "You hear distant banging.", 
		      "%s seems to have some problems with a door.", 
		      nametemp);
      }

      return false;
   }
   else {
      level->loc[dy][dx].sval=0;
      if(!monster) {
	 msg.newmsg("With a forceful push you manage to get the door open!", 
		    C_WHITE);
      }

      return true;
   }
     
   return false;
}

int8u door_open(level_type *level, int16u dx, int16u dy, _monsterlist *monster)
{
   static int8u msgno=0;

   if(!isitadoor(level, dx, dy)) {
      if(!monster)
	 msg.newmsg("That isn't a door.", C_WHITE);

      return DOORSTAT_NODOOR;
   }
   
   if( level->loc[dy][dx].doorfl & DOOR_CLOSED) {      
      /* is the door stuck? */
      if(level->loc[dy][dx].doorfl & DOOR_STUCK) {
	 if(!monster) {
	    if(!confirm_yn("The door is stuck, do you want to force it", false, true))
	       return DOORSTAT_STUCK;
	 }

	 if(!door_handlestuck(level, dx, dy, monster)) {

	    return DOORSTAT_STUCK;
	 }

      }
      /* is the door locked? */
      else if(level->loc[dy][dx].doorfl & DOOR_LOCKED) {
	 if(!monster) {
	    msg.newmsg("The door is locked tight.", C_CYAN);
	 }
	 return DOORSTAT_LOCKED;
      }

      /* is the door trapped? */
      if(level->loc[dy][dx].trap != TRAP_NOTRAP) {
	 /* blow a huge bomb or something here */
//	 if(door_handletrapped(level, dx, dy, monster)) {
	 if(handletrap(level, dx, dy, monster)) {
	 
	    return DOORSTAT_SUCCESS;
	 }
	 else {
	    if(!monster) {
	       msg.newmsg("The door stands unharmed.", C_CYAN);
	    }
	    return DOORSTAT_STUCK;
	 }
      }

      /**********************************************/
      /* it's a normal closed door with no features */
      /**********************************************/

      /* if action by player */
      if(!monster) {
	 msg.newmsg(doormsg[msgno++], C_YELLOW);
	 if(doormsg[msgno]==NULL) msgno=0;
      }
      /* if action by monster */
      else {
	 monster_sprintf(nametemp, monster, true, true );
	 sprintf(tempstr, "%s opened a door.", nametemp);
	 msg.add_dist(level, monster->x, monster->y, tempstr, C_YELLOW, "You hear a distant creak...", C_YELLOW);
      }

      door_closed2open(level, dx, dy);
      
      return DOORSTAT_SUCCESS;
   }
   else if(level->loc[dy][dx].doorfl & DOOR_OPEN ) {
      if(!monster)
	 msg.newmsg("The door is already wide open.", C_WHITE);

      return DOORSTAT_ALREADY_OPEN;
   }

   return DOORSTAT_NODOOR;
}


int8u door_close(level_type *level, int16u dx, int16u dy, 
		 _monsterlist *monster)
{
//   int8u dir;
   static int8u msgno=0;
   _monsterlist *mptr;

   if(!isitadoor(level, dx, dy)) {
      if(!monster)
	 msg.newmsg("That isn't a door.", C_WHITE);

      return DOORSTAT_NODOOR;
   }
   else if(level->loc[dy][dx].doorfl & DOOR_CLOSED) {
      if(!monster)
	 msg.newmsg("The door is closed!", C_WHITE);

      return DOORSTAT_ALREADY_OPEN;
   }
  
   if( level->loc[dy][dx].doorfl & DOOR_OPEN ) {

      /* if doorway blocked by an item */
      if(inv_countitems(&level->inv, -1, dx, dy, false)>0) {
	 if(!monster)
	    msg.newmsg("The door won't stay closed.", C_WHITE);
	 return DOORSTAT_STUCK;
      }

      mptr=istheremonster(level, dx, dy);
      if(mptr && !monster) {
	 monster_sprintf(nametemp, mptr, true, true );
	 sprintf(tempstr, "%s is there, slam the door", nametemp);
	 if(confirm_yn(tempstr, false, true)) {
	    /* maybe stun the monster when door slams shut ? */
	    msg.add("You slam the door.", C_CYAN);

	    /* issue some damage here!!! */

	    /* make monster angry to player */
	    monster_getangry(level, mptr, NULL);
	 }
	 else
	    return DOORSTAT_STUCK;
      }
      
      /* if action by player */
      if(monster==NULL) {
	 msg.newmsg(doormsg[msgno++], C_YELLOW);
         if(doormsg[msgno]==NULL) msgno=0;
      }
      /* if action by monster */
      else {
         monster_sprintf(nametemp, monster, true, true );
	 sprintf(tempstr, "%s closed a door.", nametemp);
	 msg.add_dist(level, monster->x, monster->y, tempstr, C_YELLOW, 
		      "You hear a distant squeak...", C_YELLOW);
      }
      /* update level with opened door */
      door_open2closed(level, dx, dy);
      return DOORSTAT_SUCCESS;
   }
   else
      msg.newmsg("Paska", C_RED);

   return DOORSTAT_NODOOR;
}



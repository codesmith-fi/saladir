/* process.cpp
**
** process and program control handling routines
*/

#include "saladir.h"

#include <signal.h>
#include <setjmp.h>

#include "output.h"

#include "types.h"
#include "inventor.h"
#include "file.h"

#include "process.h"

/* pointer for old segment violation signal handler */
void (*oldsegv)(int);
void (*oldsint)(int);


jmp_buf JMPBUF;

bool log_error(const char *fmt, ...)
{
   FILE *fh;
   va_list ap;

   struct tm *loctime;
//   static char logbuf[1024];
   const char logfile[]="error.log";
//   static bool mode=false;
   time_t ct;

   chdir_totempdir();
   fh=fopen(logfile, "a");

   if(!fh)
      return false;


   ct=time(NULL);
   loctime=localtime(&ct);
   fprintf(fh, "--- Error generated at %s", asctime(loctime));
   va_start(ap, fmt);
   vfprintf(fh, fmt, ap);
   va_end(ap);
   fprintf(fh, "\n");
	   
   fclose(fh);
   return true;
}


/*
** This will be called on error
*/
int16s ERROROUT( char *msg)
{

   freeall();

   showcursor();

   printf("Fatal error!\n%s\n", msg);

   exit(10);
}

/* free all resources */
void freeall(void)
{   
   inv_removeall(&player.inv, player.equip);
   skill_removeall(&player.skills);
   cond_removeall(&player.conditions);
   quest_removeall(&player.quests);
   
   freelevel(c_level);
   clean_tempdir();

   if(c_level)
      delete c_level;

   my_endwin();
}

void catch_SEGV(int what)
{
   handle_SEGV();
}

void catch_SINT(int what)
{
     if( confirm_yn("Do you want to quit", false, true) == true) {
	  GAME_NOT_ALIVE=false;
	  freeall();
	  exit(0);
     }
      
   msg.update(true);
   my_refresh();
//    err_sigint=1;
//    return;
}

bool init_errorhandlers(void)
{
   sint success=true;

   if(signal(SIGSEGV, catch_SEGV)==SIG_ERR) {
      my_cputs_init( CH_RED, false,
		   "Can't init Segmentation Violation signal handler.");
      success=false;      
   }
   if(signal(SIGINT, catch_SINT)==SIG_ERR) {
      my_cputs_init( CH_RED, false,
		   "Can't init Interrupt signal handler.\n");
      success=false;
   }
   return success;
}




char *fatal_errortxt=
{
   "The Legend of Saladir error handler reporting for duty, sir!\n\n"
   "WOOOPS! Something is terribly wrong in the game. It caused a \n"
   "segmentation violation error. This normally means that the program\n"
   "is writing data to somewhere it should not write to.\n\n"
   "What this means in plain english?\n\n"
   "             There is a BAD bug in the code!\n\n"
   "Try to remember what you did last before this text popped up\n"
   "and report this bug to: ernomat@evitech.fi\n\n"
   "Sorry...\n\n"
   "Press ANY KEY and I'll try to write an error report file SALADIR.ERR."
};

void handle_SEGV(void)
{
   Tinvpointer iptr;
//   item_info *leviptr;
   _monsterlist *mptr;

   flash();
   showcursor();

   my_clrscr();

   my_printf(fatal_errortxt);
   my_getch();

   FILE *fh;
   sint i;

   fh = fopen("SALADIR.ERR", "wt");

   if(!fh) {
      my_printf("\nSorry, couldn't write the error report!\n");
   }
   else {
      my_printf("\nWriting error report SALADIR.ERR... ");

      fprintf(fh, "%s\n", SALADIR_VERSTRING);
      fprintf(fh, "Error report caused by a crash!\n\n");
      fprintf(fh, "Level information:\n------------------\n");
      fprintf(fh, "Level size was: X%d * Y%d with %d rooms.\n",
	      c_level->sizex, c_level->sizey, c_level->roomcount);
      for(i=0; i<c_level->roomcount; i++) {
	 fprintf(fh, "   %s (type %d) (%d,%d -> %d,%d)\n", 
		 c_level->rooms[i].name,
		 c_level->rooms[i].type, 
		 c_level->rooms[i].x1, c_level->rooms[i].y1,
		 c_level->rooms[i].x2, c_level->rooms[i].y2);
      }
      fprintf(fh, "\nMonsters:\n---------\n");
      mptr=c_level->monsters;

      if(!mptr)
	 fprintf(fh, "   No monsters in the level.\n");
      else {
	 while(mptr) {
	    fprintf(fh, "   %s at %d,%d\n", 
		    mptr->m.desc, 
		    mptr->x, mptr->y);
	    if(mptr->target) {
	       fprintf(fh, "     targets to %s at %d,%d\n", 
		       mptr->target->m.desc, 
		       mptr->target->x, 
		       mptr->target->y);
	    }
	    if(mptr->inv.first) {
	       iptr=mptr->inv.first;
	       while(iptr) {
		  fprintf(fh, "       %s (%ld)\n", 
			  iptr->i.name, iptr->count);
		  iptr=iptr->next;
	       }
	    }
	    mptr=mptr->next;
	 }
      }

      fprintf(fh, "\nPlayer:\n-------\n");
      fprintf(fh, "Name: \"%s\", Level: %d (%ld exp), Pos: %d,%d\n", 
	      player.name,
	      player.level, 
	      player.exp, 
	      player.pos_x, player.pos_y );
      fprintf(fh, "Race: %d, class: %d, moves: %ld, sight: %d\n", 
	      player.prace, player.pclass, player.movecount, player.sight);
      fprintf(fh, "Dungeon: %d (%s) level %d, room %d\n",
	      player.dungeon, dungeonlist[player.dungeon].name, 
	      player.dungeonlev,
	      player.inroom );

      fprintf(fh, "Stats: \n");
      for(i=0; i<STAT_ARRAYSIZE; i++) {
	 fprintf(fh, "\t%d, %d, %d, %d, %d\n",
		 player.stat[i].initial,
		 player.stat[i].temp,
		 player.stat[i].perm,
		 player.stat[i].min,
		 player.stat[i].max);
      }

/*
 * Display player hitpoints
 */
      fprintf(fh, "Hitpoints (%d/%d): \n", 
	      player.hp, player.hp_max );
      for(i=0; i<HPSLOT_MAX; i++) {
	 fprintf(fh, "\t%d, %d, %d\n",
		 player.hpp[i].cur, 
		 player.hpp[i].ini, 
		 player.hpp[i].max );
      }
      iptr=player.inv.first;

/*
 * Print player inventory
 */
      if(!iptr)
	 fprintf(fh, "\nNo inventory.\n");
      else {
	 while(iptr) {
	    fprintf(fh, "  %s (%ld)\n", 
		    iptr->i.name, iptr->count);
	    iptr=iptr->next;
	 }

      }

      fprintf(fh, "\nThat's it. Please send this report to:"
	      "\n\ternomat@evitech.fi\n" );

      fclose(fh);
      my_printf("Ok!\n");
   }
   my_printf("\nFreeing resources...\n");

   freeall();

   delete c_level;
   exit(10);
}



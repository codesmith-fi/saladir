/**************************************************************************
 * main.cc --                                                             *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 18.04.1999                                         *
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
#include "inventor.h"
#include "birth.h"
#include "shops.h"
#include "player.h"
#include "options.h"
#include "compress.h"
#include "file.h"
#include "score.h"
#include "utility.h"
#include "skills.h"
#include "raiselev.h"
#include "init.h"
#include "process.h"
#include "search.h"
#include "status.h"
#include "quest.h"
#include "textview.h"
#include "_version.h"

#include "cmd.h"

int8u direction;
int16u ty;
int16u keycode=0;

int16u cyear, cmonth, cday, chour, cmin;
int16s cyearadd;

typedef struct
{
      int16u color;
      char *txt;
}Tlogo;

Tlogo txt_logo[]=
{
   { C_CYAN, "Feedback via email: ernomat@evitech.fi"},
   { C_CYAN, "Programmed and designed by Erno Tuomainen (C)1997-1999"},
   { C_WHITE, ""},
   { C_YELLOW, "A roguelike fantasy roleplaying game."},
   { C_WHITE, ""},
   { C_WHITE, "#########################     "},
   { C_WHITE, "#.......................#     "},
   { C_WHITE, "#######################.#     "},
   { C_WHITE, "   Legend of Saladir  #.######"}, 
   { C_WHITE, "###########      The  #......+"},
   { C_WHITE, "#.........#           #.######"},
   { C_WHITE, "#....$....#############.#     "},
   { C_WHITE, "#.................@.....#     "},
   { C_WHITE, "#########################     "},
   { C_WHITE, "" },
   { C_RED, "Welcome to " },
   { C_WHITE, NULL }
};

/*
void show_logo(void)
{
   int8u lindex=0, destline=SCREEN_LINES-4, tol, otol;

   hidecursor();

   while(txt_logo[lindex]!=NULL) {
      tol=otol=1;
      while( tol < destline) {

	 clearline(otol);
	 my_setcolor(CH_GREEN);
	 my_cputs(tol, txt_logo[lindex]);
	 my_refresh();

	 delay_output(150);

	 otol=tol;
	 tol++;
      }
      destline--;
      lindex++;
   }

   showcursor();

}
*/

#define NUM_LOGOLIN 8
void show_logo(void)
{
   int8u aindex=0, lindex=0, tol, otol;
   real val;
   real pii=3.141592654;

   int16s strs[NUM_LOGOLIN];
   int8u ycs[NUM_LOGOLIN];
   int8u oycs[NUM_LOGOLIN];
   int8u toys[NUM_LOGOLIN];
   int16u angls[NUM_LOGOLIN];

   bool stilldo, takenew=false;

   hidecursor();

   for(tol=0; tol<NUM_LOGOLIN; tol++) {
      ycs[tol]=1;
      oycs[tol]=1;
      strs[tol]=-1;
      angls[tol]=0;
      toys[tol]=SCREEN_LINES-4;
   }

   tol=otol=1;
   lindex=0;

   int16s tt=0;

   stilldo=true;
   while(stilldo) {
      stilldo=false;

      if(!tt) {
	 tt=10;
	 takenew=true;
      }
      tt--;

      for(aindex=0; aindex<NUM_LOGOLIN; aindex++) {
	 if(strs[aindex]<0) {

	    if(txt_logo[lindex].txt!=NULL) {

	       if(takenew) {
		  takenew=false;

		  stilldo=true;
		  strs[aindex]=lindex;
		  angls[aindex]=0;
		  ycs[aindex]=1;
		  oycs[aindex]=1;
		  toys[aindex]=SCREEN_LINES-lindex-4;

		  lindex++;
	       }
	    }
	    else
	       strs[aindex]=-1;
	 }
	 else {
	    oycs[aindex]=ycs[aindex];

	    val=(SCREEN_LINES-1) * sin(pii/180 * angls[aindex]);
	    ycs[aindex]= (int8u)val+1;

	    clearline(oycs[aindex]);
	    my_setcolor(txt_logo[strs[aindex]].color);

	    my_cputs(ycs[aindex], txt_logo[strs[aindex]].txt);
	    my_refresh();

#ifdef linux	   
	    delay_output(50);
#else
	    delay(3);
#endif

	    angls[aindex]++;

//	    if(angls[aindex] < 90 ) {
//	       stilldo=true;
//	    }
	    if(ycs[aindex]!=toys[aindex]) {
	       stilldo=true;
	    }
	    else {
	       ycs[aindex]=1;
	       oycs[aindex]=1;
	       strs[aindex]=-1;
	       angls[aindex]=0;
	    }

	 }
      }
   }
/*
  for(angle=0; angle<360; angle++) {
  val=(SCREEN_LINES/2-1) * sin(pii/180 * angle);

  otol=tol;
  tol=SCREEN_LINES/2 + (int8u)val+1;

  clearline(otol);
  my_setcolor(CH_GREEN);
  my_cputs(tol, txt_logo[lindex]);
  my_refresh();

  delay_output(50);

//      my_printf("%d\t", 1+(int8u)val);
}

lindex++;
*/
   showcursor();

}

void digline(char chr, int16s x1, int16s y1, int16s x2, int16s y2)
{
   int16s d, x, y, ax, ay, sx, sy, dx, dy;

   sx=sy=0;

   dx = x2-x1;
   dy = y2-y1;
   sx = SIGN(dx);
   sy = SIGN(dy);
   ax = ABS(dx)<<1;
   ay = ABS(dy)<<1;

   x = x1;
   y = y1;
   if (ax>ay) {		/* x dominant */
      d = ay-(ax>>1);
      for (;;) {
	 my_gotoxy(x, y);
	 my_putch(chr);
	 if (x==x2) return;
	 if (d>=0) {
	    y += sy;
	    d -= ax;
	 }
	 x += sx;
	 d += ay;
      }
   }
   else {	      	/* y dominant */
      d = ax-(ay>>1);
      for (;;) {
	 my_gotoxy(x, y);
	 my_putch(chr);
	 if (y==y2) return;
	 if (d>=0) {
	    x += sx;
	    d -= ay;
	 }
	 y += sy;
	 d += ax;
      }
   }
}


sint main(sint argc, char *argv[])
{
   bool SKIPLOGO=false;

//   int16s result;

   err_sigint=0;
   err_sigsegv=0;
   err_sigfpu=0;

   GAME_NOT_ALIVE=false;
   GAME_CHEATER=false;

   /*
    * Init config options
    */
   config_init();
   /*
    * HANDLE COMMAND LINE PARAMETERS **
    */
   if(argc>1) {
      for(sint i=1; i<argc; i++) {
	 if(!my_stricmp(argv[i], "-debug"))
	    CONFIGVARS.DEBUGMODE=true;
	 else if(!my_stricmp(argv[i], "-?")) {
	    printf("The following command line parameters are available:\n-s      skip the animated logo at startup\n-?      this help\n-debug  start the game in debug mode\n\n");
	    return 0;
	 }
	 else if(!my_stricmp(argv[i], "-s")) 
	    SKIPLOGO=true;
	 else {
	    printf("Unknown command \"%s\" in parameter list!\n", 
		   argv[i]);
	    return 0;
	 }
      }
   }


   /*
    * INIT SCREEN/TERM
    */
   my_initscr();

//   diamond_square(0.8f);

//   my_endwin();
//   return 0;

   if(!SKIPLOGO) {
      my_setcolor(CH_GREEN);
      my_cputs(SCREEN_LINES/2-1, "the");
      my_cputs(SCREEN_LINES/2, "Legend of Saladir");
      my_setcolor(CH_YELLOW);
      my_cputs(SCREEN_LINES/2+1, PROC_VERSION PROC_PLATFORM " " PROC_DATE );
      my_cputs(SCREEN_LINES/2+2, "(C)1997-1999 by Erno Tuomainen" );
      
   }
   else
      show_logo();

   global_initmode = true;


   /*
    * INIT RANDOM SYSTEM
    */
   my_cputs_init( CH_YELLOW, true, "Global game data...");
   init_game();

   /*
    * INIT RANDOM SYSTEM
    */
   my_cputs_init( CH_YELLOW, true, "Random system and dice...");
   initdice(0);


   /*
    * INIT TABLES
    */
   my_cputs_init( CH_YELLOW, true, "Tables and global variables...");
   init_tables();
   init_exptable();

   /*
    * INIT TABLES
    */
   my_cputs_init( CH_YELLOW, true, "Magic spells ...");
   init_spellskills();


   /*
    * INIT SIGNAL HANDLERS
    */
   my_cputs_init( CH_YELLOW, true, "Signal (error) handlers...");

   if(!init_errorhandlers()) {
      showmore(false, false);
      my_getch();
   }


   /*
    * DO SOME NECESSARY VARIABLE INITIALIZATION
    */
   my_cputs_init( CH_YELLOW, true, "Level structures and memory...");

   c_level = new level_type;
   
   if(!c_level) {
      my_cputs_init( CH_RED, false, "Error! No memory for level data!");
//      printf("Error! No memory for level data!\n");
//      my_getch();
      return 10;
   }

   /* clear the level structure */
   mem_clear(c_level, sizeof(level_type));

/*
  for(int16u i=0; i<MAXSIZEY; i++) {
  c_level->loc[i]=NULL;
  }
  c_level->monsters=NULL;
  c_level->items=NULL;
*/

   /***********************/
   /* INIT PLAYER         */
   /***********************/

   my_cputs_init( CH_YELLOW, true, "Directories and files...");
   if(!init_directories()) {

      showmore(false, false);

      showcursor();
      my_endwin();
      
      delete c_level;
      return 10;
   }

   /*
    * LOAD CONFIGURATION
    */
   my_cputs_init( CH_YELLOW, true, "Loading configuration file...");
   config_load();

   /***********************/
   /* INIT ALL ITEMS      */
   /***********************/
   my_cputs_init( CH_YELLOW, true, "Player...");
   player_init();

   my_cputs_init( CH_YELLOW, true, "Items...");
   inititems();

   my_cputs_init( CH_YELLOW, true, 
		  "Performing a transdimensional time jump...");

   cyear=1200+RANDU(100);
   cmonth=RANDU(TIME_MONTHS);
   cday=RANDU(TIME_DAYS);
   chour=6+RANDU(13);
   cmin=RANDU(TIME_MINUTES);

   worldtime.set(cyear, cmonth, cday, chour, cmin );

   my_cputs_init( CH_YELLOW, true, "Creating a new level...");

   createlevel(c_level);

   my_cputs_init( CH_YELLOW, true, "Initialization ok!");

   my_setcolor(CH_GREEN);
   my_cputs( SCREEN_LINES, "[continue]");
   my_getch();


   cyearadd=player_born();

   /* clear messages which came during initialization */
//   msg.clearall();

   global_initmode = false;

   /* game was loaded if < 0 */
   if(cyearadd >= 0) {
      worldtime.set(cyear+cyearadd, cmonth, cday, chour, cmin );

/*
      sprintf(tempstr, 
	      "%d years later, you suddenly find yourself "
	      "in a small cave...", 
	      cyearadd);
      msg.newmsg(tempstr, CH_WHITE);
*/
   }

   worldtime.set_events(false);

   attrset(A_NORMAL);
   my_setcolor(C_WHITE);

   do_redraw(c_level);

   drawmode=false;
   editmode=false;

   while(!GAME_NOT_ALIVE && player.alive) {
      /* check the interrupt signal */

      /* handle keyboard commands */
      read_cmd();

      /* clear message line */
//      msg.update(false);

      /*********************/
      /* LEVEL EDITOR HACK */
      /*********************/
      if(editmode && drawmode) {
	 set_terrain(c_level, player.pos_x, player.pos_y, editerrain);
	 GAME_NOTIFYFLAGS|=GAME_EDITORCHG;
      }

      game_passturn(c_level, true, true);

   }

   if(player.alive && !GAME_CHEATER) {
      if( confirm_yn("Do you want to save", true, true)==true) {
	 my_clrscr();
	 if( !save() ) {
	    my_printf("Save failed!\n");
	 }
	
	 showmore(false, false);
	 my_clrscr();
      }
   }

   if(player.inv.weight > 0) {
      if( confirm_yn("Do you want to see your stuff", true, true)==true) {
	 inv_listitems(&player.inv, "Items in your backpack", -1, true, -1,-1);
      }
   }
   time_t curscore=0;

   my_setcolor(C_WHITE);
   my_clrscr();
   if(!player.alive && !GAME_CHEATER) {
      curscore=score_calculate();

      my_printf("\n%s, you were killed by %s!\n\n", 
		player.name, player_killer);
//      my_printf("You survived %d moves!\n", player.movecount);


   }
   else {
      if(GAME_CHEATER)
	 my_printf("Sorry, cheaters don't get scores!\n\n");
      else
	 my_printf("Sorry, quitting doesn't give any scores.\n\n");
   }

   /* display scores */
   score_showbest(4, 4, curscore);

   showmore(false, false);

   my_setcolor(C_WHITE);
   my_clrscr();

   freeall_player();

   freelevel(c_level);

   if(! clean_tmpfiles() ){
      my_printf("Tempdir cleaning failed! (press key)\n");
      showmore(false, false);
   }

   if(!config_save()) {
      my_printf("Can't save the configuration.\n");
      showmore(false, false);
   }

   chdir_totempdir();
   if(testfile("error.log")) {
      my_printf("Error log was generated while you're playing.\n");
      showmore(false, false);
   }

   my_endwin();
   showcursor();   

   // delete current level from memory


//  remove_all();
   delete c_level;

#ifdef MSS
   MSS_LOG_BLOCK_LIST;
#endif

   exit(0);
}

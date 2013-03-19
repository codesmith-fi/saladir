/* 

	desc: output routines used in the game
*/

//#include "saladir.h"

#include "mycurses.h"

const char *color_names[]=
{
   "black",
   "red",
   "green",
   "yellow",
   "blue",
   "magenta",
   "cyan",
   "white",
   "gray",
   "light red",
   "light green",
   "light yellow",
   "light blue",
   "light magenta",
   "light cyan",
   "light white",
   "black blinking",
   "red blinking",
   "green blinking",
   "yellow blinking",
   "blue blinking",
   "magenta blinking",
   "cyan blinking",
   "white blinking",
   "gray blinking",
   "light red blinking",
   "light green  blinking",
   "light yellow blinking",
   "light blue blinking",
   "light magenta blinking",
   "light cyan blinking",
   "light white blinking",
};

const char *keyname_func[]=
{
   "F1",
   "F2",
   "F3",
   "F4",
   "F5",
   "F6",
   "F7",
   "F8",
   "F9",
   "F10",
   "F11",
   "F12",
   "F13",
   "F14",
   "F15",
   "F16",
   "F17",
   "F18",
   "F19",
   "F20",
   NULL
};

const char *keyname_ctrl[]=
{
   "?-?",
   "C-a",
   "C-b",
   "C-c",
   "C-d",
   "C-e",
   "C-f",
   "C-g",
   "C-h",
   "C-i",
   "C-j",
   "C-k",
   "C-l",
   "C-m",
   "C-n",
   "C-o",
   "C-p",
   "C-q",
   "C-r",
   "C-s",
   "C-t",
   "C-u",
   "C-v",
   "C-w",
   "C-x",
   "C-y",
   "C-z",
   "ESC",
   NULL
};

static int8u ct[MAX_COLORS+1]=
{
   COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE,
   COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE,
};

int32u colortable[MAX_COLORS*4+1];


/************************************************************
 * Here're somelow level output functions                   * 
 ************************************************************
 * The functions below are pretty much system specific      *
 * If you have curses, there should NOT be any probles      *
 * BUT, for any other type of output library you need to    *
 * rewrite them                                             *
 ************************************************************/

/**
 ** Routine to initialize the screen
 **
 **/
void my_initscr()
{
   if(initscr()==NULL) {
      fprintf(stderr, "Can't init the screen!");
      exit(1);
   }

   flash();

   if(has_colors()) {
      _initcolorpairs();
   }
  
   /* init screen proportions */
   SCREEN_LINES=getmaxy(stdscr);
   SCREEN_COLS=getmaxx(stdscr);
   MAPWIN_SIZEX=60;
   MAPWIN_SIZEY=SCREEN_LINES-5;

   MINSIZEX=MAPWIN_SIZEX+1;
   MINSIZEY=MAPWIN_SIZEY+1;

   if(MAPWIN_SIZEY>MAXSIZEY)
      MAPWIN_SIZEY=MAXSIZEY-5;

   /* we want to hang on until there is some input  */
   cbreak();
   noecho();

   notimeout(stdscr, TRUE);
   keypad(stdscr, TRUE);

   refresh();


}

/**
 ** Initialize all colors! We use color pairs 1-8 to store default colors
 **/
void _initcolorpairs(void)
{
   int8u i;

   numcolors=8;

   start_color();

   for(i=0; i<MAX_COLORS; i++) {
      init_pair(i+1, ct[i], COLOR_BLACK);

      colortable[i]=COLOR_PAIR(i+1);
      colortable[i+MAX_COLORS]=COLOR_PAIR(i+1)|A_BOLD;
      colortable[i+MAX_COLORS*2]=COLOR_PAIR(i+1)|A_BLINK;
      colortable[i+MAX_COLORS*3]=COLOR_PAIR(i+1)|A_BLINK|A_BOLD;
   }

   if(CONFIGVARS.DEBUGMODE) {
      attrset(colortable[CH_WHITE]);
      my_printf("This text should be LIGHT WHITE and the rest colors should be:\n\n");

      for(i=0; i<MAX_COLORS*4; i++) {
	 attrset(colortable[i]);
	 my_printf("########## ");
	 attrset(colortable[i%(MAX_COLORS*2)]);
	 my_printf("%s\n", color_names[i]);
	 
      }
      attrset(colortable[CH_WHITE]);      
      my_printf("\nIf not, then report to \"ernomat@evitech.fi\" and tell what's not correct.\n");

      showmore(false,false);
   }


}

/**
 ** Routine to free screen
 ** call this at the end of program
 **/
void my_endwin()
{
   echo();
   endwin();
}

void my_enablescroll()
{
   scrollok(stdscr, TRUE);

}

void my_disablescroll()
{
   scrollok(stdscr, FALSE);

}

/**
 ** Routine for refreshing the screen after output
 **
 **/
void my_refresh()
{
   refresh();
}

void my_clrscr(void)
{
   // attributes to A_NORMAL
   standend();

   // clear the screen
   clear();
   my_refresh();
}

void my_clr2bot(void)
{
   clrtobot();
}

void my_clr2eol(void)
{
   clrtoeol();
}

/* my_setcolor is a macro */

/* my_gotoxy is a macro */

int16u my_getx(void)
{
   int16u x, y;

   getyx(stdscr, y, x);

   return x+1;
}

int16u my_gety(void)
{
   int16u x, y;

   getyx(stdscr, y, x);

   return y+1;
}

int16u my_getch()
{
   flushinp();

   /* refresh screen before any input */
   my_refresh();

   return getch();
}


void my_cputch(int16u ch, int16u color)
{
   my_setcolor(color);
   my_addch(ch);
}

void my_beep(void)
{
   beep();
   flash();
}

// hides the cursor
void hidecursor(void)
{
   curs_set(0);
}

// show cursor
void showcursor(void)
{
   curs_set(1);
}

void clearline(int16u y)
{
   my_gotoxy(1, y);
   clrtoeol();

}

void makeborder(int16u x1, int16u y1, int16u wd, int16u hg)
{   
   my_gotoxy(x1, y1);
   /* curses origo in 0,0, my origo in 1,1 */

   vline('|', hg);   
   hline('=', wd);
   addch('+');

   my_gotoxy(x1, y1+hg-1);
   hline('=', wd);
   addch('+');

   my_gotoxy(x1+wd-1, y1);
   vline('|', hg);
   addch('+');

   my_gotoxy(x1+wd-1, y1+hg-1);
   addch('+');


}

// very simple put string
void my_puts(int16u x, int16u y, char *str)
{
   my_gotoxy(x, y);

   while(*str!=0)
      my_putch(*str++);
}

// very siple put string centered on screen
void my_cputs(int16u y, char *str)
{
   int x;
   x=(SCREEN_COLS/2) - (strlen(str)/2);
   my_puts(x, y, str);
}

int16u my_gets(char *str, int16u mlen)
{
   int16u ch;
   echo();
   ch=wgetnstr(stdscr, str, mlen);
   noecho();

   return ch;
}
/*
 * My replacement for a routine which returns a (nicer) keyname for
 * given keycode. Final rescue comes with curses unctrl().
 */
const char *my_keyname(int16u key)
{
   static const char key_spc[]="SPACE";
   static const char key_down[]="DOWN";
   static const char key_up[]="UP";
   static const char key_left[]="LEFT";
   static const char key_right[]="RIGHT";
   static const char key_unkn[]="?key?";

   if(key==' ')
      return key_spc;
   else if(key==KEY_DOWN)
      return key_down;
   else if(key==KEY_UP)
      return key_up;
   else if(key==KEY_LEFT)
      return key_left;
   else if(key==KEY_RIGHT)
      return key_right;
   else if(key<28) {
      return keyname_ctrl[key];
   }
   else if(key>=MYKEY_CURSESF1 && key<=MYKEY_CURSESF20)
      return keyname_func[key-MYKEY_CURSESF1];
   else if(key>32 && key<128)      
      return unctrl(key);
   else
      return key_unkn;

}

void my_delay(int16u delaycount)
{
/* no delay in linux ? */
#ifndef LINUX
   delay(delaycount);
#else
//   delay_output(delaycount*10);
   clock_t now, end, adds;
   adds=(CLOCKS_PER_SEC/1000*delaycount);
   now=end=clock();
   end+=adds;

   while(clock()<end);

#endif
}

/************************************************************
 * End of low level output functions                        * 
 ************************************************************ 
 * The functions above are pretty much system specific      *
 * If you have curses, there should NOT be any probles      *
 * BUT, for any other type of output library you need to    *
 * rewrite them                                             *
 ************************************************************/


 


/*
  simple text view utility 
*/


#include <curses.h>
#include <stdio.h>
#include <string.h>

#include "sysdep.h"
#include "textview.h"

#define NAMELEN 256

const char verstring[]=
"Viewtext V0.9 (28.05.98) (C) Erno Tuomainen\n"
"Derived from the Legend of Saladir roguelike game\n\n";

int main(int argc, char *argv[])
{
   static char filename[NAMELEN+1];

   if(argc<2) {
      fprintf(stderr, "%s: no filename given.\n\n", argv[0]);
      return 0;
   }
   else
      strncpy(filename, argv[1], NAMELEN);

   my_initscr();

   viewfile(filename);

   my_clrscr();
   my_endwin();

   printf(verstring);

   return 0;
} 

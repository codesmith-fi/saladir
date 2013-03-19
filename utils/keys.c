/*

 */

#include <curses.h>

int main(void)
{
   int ch;

   if(initscr()==NULL) {
      fprintf(stderr, "Can't init screen (curses)\n");
      exit(1);
   }

   noecho();
   cbreak();
   keypad(stdscr, TRUE);

   while(1) {
      ch=getch();

      if(ch=='q') 
	 break;
      else {
	 printw("Key: %04d == %s (%s)\n", ch, unctrl(ch), keyname(ch));

      }
     
   }




   echo();
   endwin();

   return 0;
}

/* output for windows console */

#ifdef WINDOWS32
#include <windows.h>

HANDLE hStdout=NULL, hStdin=NULL; 
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
#endif

#ifdef linux
#include <curses.h>
#endif


int console_init(void)
{
#ifdef WINDOWS32
   /* Get handles to STDIN and STDOUT. */ 
   hStdin = GetStdHandle(STD_INPUT_HANDLE); 
   hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 

   if (hStdin == INVALID_HANDLE_VALUE || 
       hStdout == INVALID_HANDLE_VALUE)
      return -1;

   if (! GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
      return -1;
#endif

#ifdef linux
   if(initscr()==NULL) {
      fprintf(stderr, "Can't init curses screen!\n");
      return -1;
   }
   flash();
   cbreak();
   noecho();
   notimeout(stdscr, TRUE);
   keypad(stdscr, TRUE);

   refresh();
   
#endif
   return 0;
}


void console_close(void)
{
#ifdef linux
   echo();
   endwin();
#endif

   return;
}

#ifdef WINDOWS32
int my_gotoxy(int x, int y)
{
   csbiInfo.dwCursorPosition.X = x;
   csbiInfo.dwCursorPosition.Y = y;

   if (! SetConsoleCursorPosition(hStdout, 
				  csbiInfo.dwCursorPosition)) 
      return -1;

   return 1;
}
#endif

#ifdef linux
void my_refresh(void)
{
   refresh();
}


void my_clrscr(void)
{
   standend();
   clear();
   my_refresh();
}

int my_getch(void)
{
   my_refresh();
   return getch();
}
#endif

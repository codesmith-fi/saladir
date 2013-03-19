/* output.h*/

int console_init(void);
int console_close(void);
#ifdef WINDOWS32
int my_gotoxy(int x, int y);
#endif

#ifdef linux
#include <curses.h>
void my_refresh(void);
#define my_gotoxy(x, y) move((y), (x))
#define my_printf       printw
#define my_putch(ch)    addch(ch)
#define my_addch(ch)    addch(ch)
#define my_mvaddch(x, y, ch) mvaddch((y), (x), ch)
void my_clrscr(void);
int my_getch(void);
#endif

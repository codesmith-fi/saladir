/* linux_curses.h */

#ifndef _LINUXCURSES_H_DEFINED
#define _LINUXCURSES_H_DEFINED

#include <curses.h>

#include "types.h"
#include "output.h"
#include "options.h"

/******************************************************
 * In my game, the topleft corner is x=1, y=1.        *
 * Curses topleft is x=0, y=0                         *
 ******************************************************/

/* ****************************************************
 * NOTE THESE MACROS, THEY'RE HERE FOR REASON         *
 * MAINLY because they are FASTER than function calls *
 *                                                    *
 * you need to have a similar functions when you port *
 * the game for another system without curses/ncurses *
 ******************************************************/

/* macro for outputting a character */
#define my_addch(ch)       addch(ch)
#define my_putch(ch)       addch(ch)

/* move to x,y and put char ch */
#define my_mvaddch(x,y,ch) mvaddch(y-1,x-1,ch)

/* goto x, y */
#define my_gotoxy(x,y)     move(y-1,x-1)

/* printf curses replacement */
#define my_printf          printw

/* scroll */
#define my_scroll(c)       scrl(c)

/* some needed key definitions */

#define KEY_ESC            27    // this is for PC

#ifndef KEY_ENTER
#define KEY_ENTER          13
#endif

#ifdef LINUX
#define PADENTER           KEY_ENTER
#define PADMINUS           '-'
#define PADPLUS            '+'
#endif

/* curses direct codes (with keypad set TRUE) for Function keys */
#define MYKEY_CURSESF1       KEY_F(1)
#define MYKEY_CURSESF20      KEY_F(20)

#define MYKEY_F1           KEY_F(1)
#define MYKEY_F2           KEY_F(2)
#define MYKEY_F3           KEY_F(3)
#define MYKEY_F4           KEY_F(4)
#define MYKEY_F5           KEY_F(5)
#define MYKEY_F6           KEY_F(6)
#define MYKEY_F7           KEY_F(7)
#define MYKEY_F8           KEY_F(8)
#define MYKEY_F9           KEY_F(9)
#define MYKEY_F10          KEY_F(10)

/* since curses doesn't provide symbolic defines for
 * CTRL-keys, I've defined my own
 */
#define MYKEY_CTRLA        1
#define MYKEY_CTRLB        2
#define MYKEY_CTRLC        3     /* NOT USABLE, BREAK */
#define MYKEY_CTRLD        4
#define MYKEY_CTRLE        5
#define MYKEY_CTRLF        6
#define MYKEY_CTRLG        7
#define MYKEY_CTRLH        8
#define MYKEY_CTRLI        9
#define MYKEY_CTRLJ        10
#define MYKEY_CTRLK        11
#define MYKEY_CTRLL        12
#define MYKEY_CTRLM        13
#define MYKEY_CTRLN        14
#define MYKEY_CTRLO        15
#define MYKEY_CTRLP        16
#define MYKEY_CTRLQ        17
#define MYKEY_CTRLR        18
#define MYKEY_CTRLS        19
#define MYKEY_CTRLT        20
#define MYKEY_CTRLU        21
#define MYKEY_CTRLV        22
#define MYKEY_CTRLW        23
#define MYKEY_CTRLX        24
#define MYKEY_CTRLY        25
#define MYKEY_CTRLZ        26

#define MAX_COLORS 8

enum color_types
{
   C_BLACK, C_RED, C_GREEN, C_YELLOW, C_BLUE, C_MAGENTA, C_CYAN, C_WHITE,
   CH_DGRAY, CH_RED, CH_GREEN, CH_YELLOW, CH_BLUE, CH_MAGENTA, CH_CYAN, 
   CH_WHITE,
   CB_BLACK, CB_RED, CB_GREEN, CB_YELLOW, CB_BLUE, CB_MAGENTA, CB_CYAN, 
   CB_WHITE,
   CHB_DGRAY, CHB_RED, CHB_GREEN, CHB_YELLOW, CHB_BLUE, CHB_MAGENTA, CHB_CYAN, 
   CHB_WHITE
};

extern int32u colortable[MAX_COLORS*4+1];

#define my_setcolor(color)  attrset(colortable[color])

void my_initscr(void);
void _initcolorpairs(void);
void my_endwin(void);
void my_enablescroll(void);
void my_disablescroll(void);
void my_refresh(void);
void my_clrscr(void);
void my_clr2bot(void);
void my_clr2eol(void);
int16u my_getch(void);
void my_cputch(int16u, int16u);
void my_beep(void);
void hidecursor(void);
void showcursor(void);
void clearline(int16u);
void my_puts(int16u, int16u, char *);
void my_cputs(int16u, char *);
int16u my_getx(void);
int16u my_gety(void);
int16u my_gets(char *str, int16u mlen);
void my_delay(int16u);
void makeborder(int16u x1, int16u y1, int16u wd, int16u hg);
const char *my_keyname(int16u key);

#endif

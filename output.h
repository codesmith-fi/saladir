/* output.h */

#ifndef _OUTPUT_H_DEFINED
#define _OUTPUT_H_DEFINED

#include "saladir.h"

/*
#if defined(_TS_LINUX_)
#include "linux_curses.h"
#elif defined (_TS_DOS_)
#include "dos_curses.h"
#else
# error "Please define output-functions for your system"
#endif
*/
#include "mycurses.h"

#include "caves.h"
#include "variable.h"
#include "message.h"
#include "dice.h"

/* relative position of MAP window */
#define MAPWIN_RELX	(SCREEN_COLS-MAPWIN_SIZEX)
#define MAPWIN_RELY	2

#define MAPWIN_X	(MAPWIN_RELX+1)
#define MAPWIN_Y	(MAPWIN_RELY+1)

// y location of status row
#define STATUSROW	(MAPWIN_RELY+MAPWIN_SIZEY+1)
#define MSGLINE		1
#define MSGLINES	2
#define LOCATION_NAME	1


void drawline(int16u, char);
void drawline_limit(int16u, int16u, int16u, char);
void my_cputs_init(int16u, bool, char *);
bool confirm_yn(char *prompt, bool, bool);
void my_wordwraptext(const void *, int8u, int8u, int8u, int8u);
void showmore(bool, bool);
int16s get_response(char *, char *);
void zprintf(const char *, ...)
   __attribute__ ((format (printf, 1, 2)));
void ww_print(char *);

/* global public variables */
extern int16s SCREEN_LINES, SCREEN_COLS;
extern int16s MAPWIN_SIZEX, MAPWIN_SIZEY;
extern int16u numcolors;
extern int16u MINSIZEX, MINSIZEY;

#endif /* output.h defined */

/*
 * NPCtalk.h
 *
 */

#ifndef _NPCTALK_H_INCLUDED
#define _NPCTALK_H_INCLUDED

typedef struct _Schatinfo
{
     char *firstgreet; /* when met for the first time */
     char *knowgreet; /* when met again */
     Twordlist *wordlist; /* pointer to a list of known words */
} Tchatinfo;


/* variables */
extern Tchatinfo NPCchatinfo[];


#endif /* _NPCTALK_H_INCLUDED */

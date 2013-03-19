/***************************************************************/
/* SCROLLS                                                     */
/***************************************************************/
/* 
 * (C)1997/1998 Erno Tuomainen
 * Made for Legend of Saladir
 */

#ifndef _SCROLLS_H_DEFINED
#define _SCROLLS_H_DEFINED

/* scroll groups */
#define SCROLLGROUP_GENERIC 0
#define SCROLLGROUP_MAGIC   1

/* scroll flags */
#define SCFLAG_AUTOIDENTIFY 0x0001 /* autoidentified scroll (by reading) */
#define SCFLAG_IDENTIFIED   0x0002 /* scroll is identified */
#define SCFLAG_NAMED        0x0004 /* scroll is named */
#define SCFLAG_TRIED        0x0008 

/* general group scroll effects */

/* scrolls */
#define SCROLL_SELFWRITTEN    0
#define SCROLL_BLANK    1
#define SCROLL_RUMOUR   2
#define SCROLL_HEALING  3
#define SCROLL_TELEPORT 4 
#define SCROLL_CURSE    5
#define SCROLL_UNCURSE  6
#define SCROLL_BLESS    7
#define SCROLL_CONFUZE  8
#define SCROLL_IDENTIFY 9
#define SCROLL_AMNESIA  10
#define SCROLL_MAGICMAP 11

/***********************/
/* function prototypes */
/***********************/
bool scroll_readit(level_type *level, item_def *item);

typedef struct
{
      /* name of the scroll when identified */
      char *name;
      
      /* unidentified name */
      char uname[ITEM_NAMEMAX+1]; 

      /* call name */
      char cname[ITEM_NAMEMAX+1];

      /* scroll group */
      int16u group;

      /* what spell is contained in the scroll */
      int16u spell; 

      /* recorded skill value, if it's a spell scroll */
      int16u skill;

      /* scroll flags */
      int32u flags;
} Tscroll;




/* scroll list */
extern Tscroll list_scroll[];


#endif

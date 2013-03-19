/**************************************************************************
 * pathfind.cc --                                                         *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 02.08.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 02.08.1998                                         *
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
 **************************************************************************
 * eventually this will handle pathfinding for monsters                   *
 **************************************************************************/

#include "saladir.h"

#include "output.h"
#include "types.h"
#include "caves.h"
#include "tables.h"
#include "message.h"
#include "generate.h"
#include "pathfind.h"
#include "pathfunc.h"

/************************************************/
/* basic routines for handling path linked list */
/************************************************/

/* list initializer */
void path_init(Tpathlist *list)
{
   *list = NULL;
}

/*
 * Insert item in the list
 * most recent item takes the first place in the list
 *
 */
Tpathnode *path_insert(Tpathlist *list, Tcoordinate *data)
{
   Tpathnode *newnode;

   /* allocate memory for node */
   newnode=(Tpathnode *)malloc( sizeof(Tpathnode) );

   if(!newnode)
      return NULL;

   newnode->c = *data;
   newnode->next = *list;
   *list = newnode;

   return newnode;
}

/*
 * Same as above but inserts to the list end
 */
Tpathnode *path_insertend(Tpathlist *list, Tcoordinate *data)
{
   Tpathnode *newnode;
   Tpathlist ptr;

   /* allocate memory for node */
   newnode=(Tpathnode *)malloc( sizeof(Tpathnode) );

   if(!newnode)
      return NULL;

   /* set data */
   newnode->c = *data;
   newnode->next = NULL;

   if(!(*list))
      *list = newnode;
   else {
      ptr=*list;
      while( ptr->next ) {
	 ptr=ptr->next;
      }
      ptr->next = newnode;
   }

   return newnode;
}

/*
 * Remove first item from the list 
 *
 */
void path_removefirst(Tpathlist *list)
{
   Tpathnode *aux;

   aux = *list;

   /* if empty, return and free nothing*/
   if(aux == NULL)
      return;

   /* remove first item from the list */
   *list = (*list)->next;

   /* dealloc memory */
   free(aux);
}

/* 
 * Get first item from the list 
 *
 */
Tcoordinate *path_getfirst(Tpathlist *list)
{
   if((*list)==NULL)
      return NULL;

   return &((*list)->c);
}

/*
 * Get the best item from the list. Item will also be REMOVED from the
 * list. This routine is used when the path is under construction
 *
 */
int8u path_getbest(Tpathlist *list, Tcoordinate *end, Tcoordinate *retdata)
{
   Tpathlist *aux, prev=NULL, snext=NULL, sfree=NULL;
   Tpathlist old1=NULL, old2=NULL;
   Tcoordinate *data, *rdata;
   int16u edist, estim;

   aux = list;
   edist = 9999;
   data = path_getfirst(aux);
   rdata = data;

   if(data == NULL)
      return 0;

   while(data != NULL) {
      estim = data->d + path_distance(data, end);
      if(estim < edist) {
	 edist = estim;
	 rdata = data;
	 prev = old2;
	 sfree = *aux;
	 snext = (*aux)->next;
      }
      old1 = old2;
      old2 = *aux;
      aux = &((*aux)->next);
      data = path_getfirst(aux);
   }

   *retdata = *rdata;

   free(sfree);

//	if(*list == prev)
   if(prev == NULL)
      *list = snext;
   else
      prev->next = snext;

   return 1;
}

/*
 * Clear the pathlist completely
 */
void path_clear(Tpathlist *list)
{
   while(*list)
      path_removefirst(list);

   *list = NULL;
}


void path_showall(Tpathlist alist, char ch)
{
   while(alist) {
      my_mvaddch(MAPWIN_RELX + player.spos_x + (alist->c.x - player.pos_x),
		 MAPWIN_RELY + player.spos_y + (alist->c.y - player.pos_y), 
		 ch);

      alist=alist->next;
   }

}

/*
 * This function is used when we calculate an estimate of the remaining
 * path.
 */
int16u path_distance(Tcoordinate *p1, Tcoordinate *p2)
{
   int16s dx, dy;
//   real tmp;

   dx=p2->x - p1->x;
   dy=p2->y - p1->y;

   return (unsigned int)(sqrt( (float)(dx*dx + dy*dy)) * 1.5f);
//   tmp = (float)((dx*dx + dy*dy)/2);
//   return (unsigned int)tmp;
}

/**********************************************************
 * Let's get into the business, here's the actual routine *
 * for pathfinding                                        *
 **********************************************************
 * This routine will try to find a way from (x1,y1) to    *
 * (x2,y2). The result will be a linked list of path nodes*
 * or NULL if no path can be found.                       *
 *                                                        *
 * After you receive a pointer to the first node of a path*
 * list you just need to get the first item of the list to*
 * advance along the existing path, after getting the node*
 * remove it from this list or your monster will stand    *
 * still. :-)                                             *
 **********************************************************/
Tpathlist path_findroute(level_type *level,
			 int16u x1, int16u y1, int16u x2, int16u y2)
{
   Tpathlist checklist;    /* list for pathfinder to hold nodes to be
			      examined */
   Tpathlist result;       /* list to be returned */

   /* table for directions */
   int8u *dirgrid;
   register int16u i, cx=0, cy=0;

   Tcoordinate end, now, next;
   /* result indicator */
   bool pathexists = false;

   /* init both lists */
   path_init(&checklist);
   path_init(&result);

   /* check level boundaries first, 1 block border in the edges */
   if(x1 < 1 || x2 < 1 || x1 >= level->sizex-1 || x2 >= level->sizex-1 ||
      y1 < 1 || y2 < 1 || y1 >= level->sizey-1 || y2 >= level->sizey-1) {
      msg.newmsg("Pathfinder error, out of boundaries!", CHB_RED);
      return NULL;
   }
   
   /* pathfinder needs a dirtable which is exactly the same size as
    *  the level map
    */
   dirgrid = (int8u *)malloc((level->sizey +1) * (level->sizex+1));
   if(dirgrid == NULL) {
      msg.newmsg("Pathfinder cannot allocate memory for direction table!",
		 CHB_RED);
      return NULL;
   }
   /* clear the memory, needs to be 0 */
   mem_clear(dirgrid, ((level->sizey+1) * (level->sizex+1)));
   
   /* init dir grid */
   dirgrid[y1 * level->sizex + x1] = 5;

   now.x = x1;
   now.y = y1;
   now.d = 0;

   end.x = x2;
   end.y = y2;
   end.d = 0;
 
   /* insert the starting position to the checklist */
   path_insert(&checklist, &now);

   while( path_getbest(&checklist, &end, &now)) {    
      for(i=1; i<10; i++) {
	 if(i==5) 
	    continue;	 

	 next = now;

	 /* diagonal movements should be lengthier */
//	 if(i==1 || i==3 || i==7 || i==9)
//	    next.d = now.d + 2;
//	 else
	 next.d = now.d+1;
	 cy = now.y + move_dy[i];
	 cx = now.x + move_dx[i];

	 if(cx < 1 || cx > level->sizex-2) continue;
	 if(cy < 1 || cy > level->sizey-2) continue;

	 if(ispassable(level, cx, cy) && dirgrid[cy*level->sizex+cx] == 0) {
	    dirgrid[cy * level->sizex + cx] = dir_invers[i];
	    next.x += move_dx[i];
	    next.y += move_dy[i];
	    path_insert(&checklist, &next);
	 }
      }
      if(now.x == end.x && now.y == end.y) {
	 pathexists=true;
	 break;
      }
   }

   /* clear temporary checklist, no need anymore */
   path_clear(&checklist);

   int16s len=0;

   if(pathexists) {
//      msg.addwait("Pathfound!!", CH_RED);
      now.d = 0;
      /* build the ACTUAL pathlist */
      while(!(now.x == x1 && now.y == y1)) {
	 path_insert(&result, &now);
	 len++;
	 cx = now.x;
	 cy = now.y;
	 now.x += move_dx[dirgrid[cy * level->sizex + cx]];
	 now.y += move_dy[dirgrid[cy * level->sizex + cx]];
      }
      msg.vnewmsg(CH_GREEN, "Found path len=%d!", len);
      path_showall(result, '$');
      my_refresh();
      my_getch();

   }
/*
   else
      msg.newmsg("No path found!", C_GREEN);
*/
   /* free temp direction grid */
   free(dirgrid);

   /* return the generated pathlist, caller has the responsibility
    * of freeing it 
    */
   return result;
}

bool path_movealong(Tpathlist *path, int16s *tx, int16s *ty)
{
   Tcoordinate *pos;

   pos = path_getfirst(path);

//   msg.newmsg("Trying to move path!", C_GREEN);

   /* check for existing path */
   if(pos == NULL)
      return false;

   (*tx)=pos->x;
   (*ty)=pos->y;

//   msg.vnewmsg(C_GREEN, "moved to %d,%d!", *tx, *ty);
   
   /* remove the first pathnode */
   path_removefirst(path);
   return true;
}

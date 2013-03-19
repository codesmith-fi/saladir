/*

	Single direction linked list 

  */

#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "util.h"
#include "output.h"

void list_init(Llist *list)
{
	*list = NULL;
}

/*
 * Insert item in the list
 * most recent item takes the first place in the list
 *
 */
Node *list_insert(Llist *list, Tlistdata *data)
{
   Node *newnode;

   newnode=malloc( sizeof(Node) );

   if(!newnode)
      return NULL;

   newnode->c = *data;
   newnode->next = *list;
   *list = newnode;

   return newnode;
}

/*
 * Remove first item from the list 
 *
 */
void list_removefirst(Llist *list)
{
   Node *aux;

   aux = *list;

   /* if empty, return and free nothing*/
   if(aux == NULL)
      return;

   /* remove first item from the list */
   *list = (*list)->next;

   /* dealloc memory */
   free(aux);
}

Tlistdata *list_getfirst(Llist *list)
{
   if((*list)==NULL)
      return NULL;

   return &((*list)->c);
}

int list_getbest(Llist *list, Tlistdata *end, Tlistdata *retdata)
{
   Llist *aux, prev=NULL, snext=NULL, sfree=NULL;
   Llist old1=NULL, old2=NULL;
   Tlistdata *data, *rdata;
   unsigned int edist, estim;

   aux = list;
   edist = 9999;
   data = list_getfirst(aux);
   rdata = data;

   if(data == NULL)
      return 0;

   while(data != NULL) {
      estim = data->d + distance(data, end);
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
      data = list_getfirst(aux);
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

void list_showall(Llist *list)
{
   Llist *aux;
   Tlistdata *data;

   aux = list;

   data = list_getfirst(aux);

   while(data != NULL) {
      printf("x=%d\ty=%d\tdist=%d\n", data->x, data->y, data->d);
		
      aux = &((*aux)->next);
      data = list_getfirst(aux);
   }
}

void list_dispmap(Llist *list)
{
   Llist *aux;
   Tlistdata *data;

   aux = list;

   data = list_getfirst(aux);

   while(data != NULL) {
      my_gotoxy(data->x, data->y);
      my_putch('@');
      aux = &((*aux)->next);
      data = list_getfirst(aux);
   }

}

/*
 * Clear and free all memory in the list indicated by 'list'
 *
 */
void list_clear(Llist *list)
{
   while(*list)
      list_removefirst(list);
}


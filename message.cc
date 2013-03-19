/**************************************************************************
 * weapons.cc --                                                          *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 22.04.1888                                         *
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
 **************************************************************************/

#include "message.h"

#include "file.h"
#include "textview.h"

/* a buffer for variable argument functions */
char mbuffer[1024];

Message::Message()
{
   /* no messages at start */
   msglist=NULL;
   shownext=NULL;
   lastadded=NULL;

   msgdelay=0;

   index=0;
   count=0;
   updcount=0;

   lastx=1;
   lasty=1;
}

Message::~Message()
{
   Tmsgpointer aux, tmp;

   aux=msglist;

   while(aux) {
      tmp=aux;
      aux=aux->next;

      /* clear the message */
      delete [] tmp->msg;

      /* delete node */
      delete tmp;
   }
   msglist=NULL;
   shownext=NULL;
   updcount=0;
   lastadded=NULL;
   count=0;
}

/*
** clears all messages waiting
*/
void Message::clearall(void)
{
   Tmsgpointer aux, tmp;

   aux=msglist;

   while(aux) {
      tmp=aux;
      aux=aux->next;

      /* clear the message */
      delete [] tmp->msg;

      /* delete node */
      delete tmp;

   }

   msglist=NULL;
   shownext=NULL;
   updcount=0;
   lastadded=NULL;
   count=0;
}

/* add a message with variable args */
void Message::vnewmsg(int16u color, char *format, ...)
{
   va_list ap;

   va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
   vsnprintf(mbuffer, sizeof(mbuffer)-1, format, ap);
#else
   vsprintf(mbuffer, format, ap);
#endif
   va_end(ap);

   newmsg(mbuffer, color);
}

/* similar to above, exept that the message will be displayed immendiately */
void Message::vadd(int16u color, char *format, ...)
{
   va_list ap;

   va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
   vsnprintf(mbuffer, sizeof(mbuffer)-1, format, ap);
#else
   vsprintf(mbuffer, format, ap);
#endif
   va_end(ap);

   newmsg(mbuffer, color);
   notice();
}

void Message::vaddwait(int16u color, char *format, ...)
{
   va_list ap;

   va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
   vsnprintf(mbuffer, sizeof(mbuffer)-1, format, ap);
#else
   vsprintf(mbuffer, format, ap);
#endif
   va_end(ap);

   newmsg(mbuffer, color);
   notice();
   showmore(false, true);
   msg.update(true);
}



void Message::vnewdist(level_type *level, int16s x, int16s y, 
		       int16u color2, int16u color1,
		       char *message2,
		       char *format, ...)
{

   va_list ap;
   va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
   vsnprintf(mbuffer, sizeof(mbuffer)-1, format, ap);
#else
   vsprintf(mbuffer, format, ap);
#endif
   va_end(ap);

   if(level->loc[y][x].flags & CAVE_LOSTMP) {
      newmsg(mbuffer, color1);
   }
   else
     if(message2)
       newmsg(message2, color2);
}


/*
** Add a new message to the queue. If location x, y can see to the
** location of player -> add message . Else add message2
*/
void Message::add_dist(level_type *level, int16s x, int16s y, char *message, int16u color, char *message2, int16u color2)
{
//   if(cansee(level, x, y, player.pos_x, player.pos_y, 10, true)) {
   if(level->loc[y][x].flags & CAVE_LOSTMP) {
      if(message)
	newmsg(message, color);
   }
   else
     if(message2)
       newmsg(message2, color2);
}




/* add a new message, notice immendiately */
void Message::add(char *message, int16u color)
{
   newmsg(message, color);
   notice();
}

/* add a new message, notice immendiately */
void Message::addwait(char *message, int16u color)
{
   newmsg(message, color);
   notice();
   showmore(false, true);
   msg.update(true);
}

/*
** add a message to the queue
*/
void Message::newmsg(char *message, int16u color)
{
   char *NULLMSG="Null pointer passed to Message::newmsg()";
   char *SHORTMSG="Zero length message passed to Message::newmsg()";

   Tmsgpointer newmsg, aux;

   if(!message ) {
      color=CHB_RED;
      message=NULLMSG;
   }

	/* is message too short */
   if(message[0]==0) {
      color=CHB_RED;
      message=SHORTMSG;
   }

   /* handle duplicate messages */
   if(lastadded && CONFIGVARS.compactmessages) {
      if(strcmp(lastadded->msg, message)==0) {
	 lastadded->count++;
	 if(!shownext) {
	    shownext=lastadded;
	 }
	 return;
      }
   }

   /* allocate a new message node */
   newmsg=new Tmessage;

   if(!newmsg)
     return;

   /* message color */
   newmsg->count=1;
   newmsg->color=color;
   newmsg->next=NULL;
   newmsg->delay=msgdelay;

   newmsg->msg=new char[strlen(message)+1];

   /* did we get the memory for new message */
   if(!newmsg->msg) {
      delete newmsg;
      return;
   }

   lastadded=newmsg;

   if(!shownext) {
      shownext=newmsg;
   }

   /* copy the message to newly allocated memory */
   my_strcpy(newmsg->msg, message, strlen(message)+1);
//   newmsg->msg[strlen(message)]=0;

   /* place the new message to the end */
   if(msglist==NULL)
     msglist=newmsg;
   else {
      aux=msglist;
      while(aux) {
         if(aux->next==NULL)
	   break;

	 aux=aux->next;
	 
      }
      aux->next=newmsg;
   }

   /* if list contains max number of messages, remove the oldest */
   if(count>=MSG_MAXNUM) {
      if(shownext==msglist)
      	shownext=msglist->next;
      aux=msglist;

      msglist=msglist->next;

      delete [] aux->msg;
      delete aux;
   }
   else
     count++;




}

void Message::notice(void)
{
   bool MSGNOMORE=false;
   char *dptr;
   char *oneword;
   int16u i;

//   char countstr[20];

   while(shownext) {
      my_gotoxy(lastx, lasty);
      updcount=0;

      dptr=shownext->msg;

      oneword = new char[strlen(dptr)+10];
      if(!oneword) {
	 my_printf("Error!! No memory in msg.notice()!\n");
         return;
      }

      MSGNOMORE=false;
      while(!MSGNOMORE) {
	 i=0;
	 /* first skip any spaces */
	 while( (isspace(*dptr)) && ((*dptr)!=0) )
	   dptr++;
	 /* collect a word until space comes */
	 while( !(isspace(*dptr)) && ((*dptr)!=0) )
	   oneword[i++]=(*dptr++);
	 
	 if((*dptr)==0) {
	    oneword[i]=(*dptr);
	    oneword[i+1]=0;
	    if(shownext->count>1) {
//            	sprintf(countstr, "(x%d)\0", shownext->count);
//					strcat(oneword, countstr);
	    }
	    MSGNOMORE=true;
	 }
         else {
	    oneword[i]=(*dptr);
	    oneword[i+1]=0;
	 }
	 if((my_gety()==1) && (my_getx()+strlen(oneword) > (int16u)SCREEN_COLS)) {
	    my_printf("\n");
	    clearline(my_gety());
	 }
	 if((my_getx()+strlen(oneword) > (int16u)(SCREEN_COLS-6)) && (my_gety()==2)) {
	    
	    showmore(false, true);
	    
	    for(i=0; i<MSGLINES; i++)
	      clearline(MSGLINE+i);
	    my_gotoxy(1, MSGLINE);
	 }

	 /* use color in messages if the user wants so */
	 if(CONFIGVARS.colormessages) 
	    my_setcolor(shownext->color);
	 else
	    my_setcolor(C_WHITE);

	 my_printf("%s", oneword);
      }
      my_printf(" ");

      lastx=my_getx();
      lasty=my_gety();

      my_refresh();

      if(shownext->delay>0) my_delay(shownext->delay);

      shownext=shownext->next;

      delete [] oneword;

      oneword=NULL;
   }
   

}

void Message::showall(void)
{
   Tmsgpointer aux;
   FILE *fh;
   int8u i=0;

   if(count==0) {
      msg.newmsg("Message buffer is empty.", C_WHITE);
   }

   chdir_totempdir();

   fh=fopen(FILE_TMPTEXT, "w");

   if(!fh) {
      msg.newmsg("Error: unable to view last messages.", CH_RED);
      return;
   }

   fprintf(fh, "\001Previous messages\n\n");
   aux=msglist;
   while(aux) {
      fprintf(fh, "\002%2d: \007%s", i+1, aux->msg);
      if(aux->count>1)
      	fprintf(fh, " (x%d)\n", aux->count);
      else
      	fprintf(fh, "\n");

      
      i++;
      aux=aux->next;
   }

   fclose(fh);

   /* finally view the file */
   viewfile(FILE_TMPTEXT);


   return;
   int8u cy=2;


   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

   my_clrscr();
   my_setcolor(CH_GREEN);
   drawline(1, '-');
   my_cputs(1, "+[ Previous messages ]+");
   my_gotoxy(1 ,3);

   aux=msglist;
   while(aux) {
      if(cy==SCREEN_LINES-1) {
	 my_setcolor(CH_GREEN);
	 drawline(cy, '-');
	 my_cputs(cy, "+[ More ]+");
         cy=3;
         my_getch();
         my_clrscr();
	 drawline(1, '-');
	 my_setcolor(CH_GREEN);
	 my_cputs(1, "+[ Previous messages ]+");
	 my_gotoxy(1 ,3);
      }
      my_setcolor(C_WHITE);
      my_printf("%2d) %s", i+1, aux->msg);
      if(aux->count>1)
      	my_printf(" (x%d)", aux->count);
      my_printf("\n");
      i++;
      cy++;
      aux=aux->next;
   }
   my_setcolor(CH_GREEN);
   drawline(SCREEN_LINES-1, '-');
   my_cputs(SCREEN_LINES-1, "+[ Press a key ]+");
   my_getch();
}

void Message::showlast(void)
{
   
   Tmsgpointer aux;

   aux=msglist;

   if(!aux)
     return;

   while(aux) {
      if(aux->next==NULL)
      	break;

      aux=aux->next;
   }

   update(true);

   my_gotoxy(1, MSGLINE);
   if(aux->msg) {
      my_setcolor(aux->color);
      my_printf(aux->msg);
   }


}

void Message::update(bool CLEARNOW)
{
     int16u i=0;
//     CLEARNOW=false;

     if(lastx!=1 || lasty!=MSGLINE) {
	lastx=1;
	lasty=MSGLINE;
	for(i=0; i<MSGLINES; i++)
	   clearline(MSGLINE+i);
     }
}

void Message::setdelay(int16u delaytime)
{
   msgdelay=delaytime;
}



/* output.cc */

#include "output.h"

/************************************************************
 * Not so system dependent output stuff                     *
 ************************************************************/

/* this holds the number of colors */
int16u	numcolors;

/* these two will hold the screen size x and y */
int16s SCREEN_LINES, SCREEN_COLS;

/* the size of MAP WINDOW, will be put in these */
int16s MAPWIN_SIZEX, MAPWIN_SIZEY;

/* minimum sizes for generated maps */
int16u MINSIZEX, MINSIZEY;


void drawline(int16u y, char chr)
{
   my_gotoxy(1, y);

   for(int16u i=0; i<SCREEN_COLS; i++)
      my_putch(chr);
}

void drawline_limit(int16u y, int16u x1, int16u x2, char chr)
{
   if(x1>=x2) return;

   my_gotoxy(x1, y);

   for(int16u i=x1; i<=x2; i++)
   	my_putch(chr);
}

void my_cputs_init(int16u color, bool cond, char *str)
{
   const char keytxt[] = "[KEY]";

   int y;
   int x;

   y=SCREEN_LINES-2;

   my_setcolor(color);

   if(!cond) {
      y++;
   }

   clearline(y);

   x=(SCREEN_COLS/2) - (strlen(str)/2);
   my_puts(x, y, str);

   if(!cond) {
      my_setcolor(C_GREEN);
      clearline(y+1);
      my_gotoxy(SCREEN_COLS - strlen(keytxt) -1, y+1);
      my_printf(keytxt);
      my_getch();
   }
   
   my_refresh();
}

void showmore(bool msgmode, bool shortmsg)
{
   int16u key=0, i=0;

   int16u cx, cy, col=C_WHITE;
   cx=my_getx();
   cy=my_gety();

   if(msgmode)
      msg.add("(more)", C_WHITE);

   /* wait for morekey */
   while(1) {
      if(!msgmode) {
	 my_setcolor(col);
	 col=1+RANDU(15);
	 my_gotoxy(cx, cy);
	 if(i==0 || shortmsg)
	    my_printf("(more)");
	 else
	    my_printf("(more, with SPC/ENTER/ESC)");
      }
      key=my_getch();
      if(CONFIGVARS.anykeymore) {
	 break;
      }
      else
	 if(key==10 || key==13 || key==KEY_ESC 
	    || key==KEY_ENTER || key==PADENTER || key==' ')
	    break;
      i++;
   }
}

/* Routine to print a buffer of text to screen and word wrap it.
** This will word wrap text to the region specified by
** top-left <-> bottom-right  variables and ask more if needed!
*/
void my_wordwraptext(const void *txt, int8u top, int8u bottom, int8u left, int8u right)
{
   bool NOMORE, enablemore=true;
//	char *dptr, cha;
   int8u *dptr;
   int16u i, cx, cy;
   char oneword[40];

   dptr=(int8u *)txt;

   /* no NULL strings */
   if(!dptr) 
      return;

   /* set cursor to start position */
   if(top!=bottom)  
      my_gotoxy(left, top);
   else
      enablemore=false;

//   my_clr2eol();

   
   cy=my_gety();
   cx=my_getx();

   drawline_limit(cy, left, right, ' ');
   my_gotoxy(cx, cy);

   cx=left;
   cy=top;


   NOMORE=false;
   while(!NOMORE) {
      i=0;
      oneword[0]=0;
      while( (*dptr)<32 && (*dptr)!=0 ) {
	 if((*dptr)==13 || (*dptr)==10) {
	    cx=left;
	    if(cy<bottom)
	       cy++;
	    drawline_limit(cy, left, right, ' ');
	    my_gotoxy(cx, cy);

	    if(cy>=bottom-1 && enablemore) {
	       showmore(false, false);

	       for(int16u j=top; j<bottom; j++)
		  drawline_limit(j, left, right, ' ');
	       cy=top;
	       cx=left;
	       my_gotoxy(left, top);
	    }
	 }
	 if( (*dptr)=='\t') {
	    cx+=4;
	    my_gotoxy(cx, cy);
	 }
	 if( ((*dptr)>=1) && ((*dptr)<=8))
	    my_setcolor( (*dptr) );
	 if( ((*dptr)>=21) && ((*dptr)<=28))
	    my_setcolor( (*dptr)-12 );
	 dptr++;
      }
      /* collect a word until space comes */
//   	while( !(isspace(*dptr)) && ((*dptr)!=0) )
      oneword[0]=0;
      i=0;

      while( (*dptr)>=32 && (*dptr)!=0 ) {
	 oneword[i++]=*dptr;
	 oneword[i]=0;
	 if((*dptr++)==32)
	    break;

      }
      if((*dptr)==0) NOMORE=true;

      if(cx+strlen(oneword) >= right) {
	 cx=left;
	 if(cy<bottom)
	    cy++;
	 drawline_limit(cy, left, right, ' ');
	 my_gotoxy(cx, cy);
      }

      if(cy>=bottom-1 && enablemore) {
	 showmore(false, false);
	 for(int16u j=top; j<bottom; j++)
	    drawline_limit(j, left, right, ' ');
	 cy=top;
	 cx=left;
	 my_gotoxy(left, top);
      }
      cx+=strlen(oneword);
      my_printf("%s", oneword);
   }
}

bool confirm_yn(char *prompt, bool defaultresult, bool showprompt)
{
   int16u resp;
   bool result;
   char *newstr=NULL;

   if(showprompt) {
      newstr=new char[strlen(prompt)+10];

      if(!newstr)
	 return false;

      if(defaultresult)
	 sprintf(newstr, "%s (Y/n)?", prompt);
      else
	 sprintf(newstr, "%s (y/N)?", prompt);

      msg.add(newstr, C_WHITE);

      delete [] newstr;
   }
   while(1) {
      resp=my_getch();
      if(resp==' ' || resp==KEY_ENTER || resp==PADENTER || resp==10) {
	 result=defaultresult;
	 break;
      }
      if(resp=='n' || resp=='N') {
	 result=false;
	 break;
      }
      if(resp=='Y' || resp=='y') {
	 result=true;
	 break;
      }
   }

   msg.update(true);

   return result;
}


int16s get_response(char *prompt, char *str)
{
   int16u key, endget=0, i;
   
   if(strlen(str)<1) {
      my_printf("Error, no choices in string!\n");
      return -1;
   }
   
   if(prompt)
      my_printf(prompt);
   
   my_printf(" [%s] ?", str);
   
   while(!endget) {
      key=my_getch();
      for(i=0; i<strlen(str); i++) {
         if(str[i]==key)
	    return i;
      }
   }
   return -1;
}


void ww_print(char *txt)
{
   char *dptr;
   int16s wlen, slen;
   bool wrap;
   
   dptr=txt;

   slen=0;
   wlen=0;

   /* wrap until end reached */
   while(txt[slen]!=0) {
      wrap=false;
      wlen=slen;
      while(!isspace(txt[wlen]) && txt[wlen]!=0)
	 wlen++;

      /* wrap word if going over right border */
      if( my_getx()+(wlen-slen) >= SCREEN_COLS) {
	 my_putch('\n');
	 wrap=true;
      }

      /* print the word */
      while(slen!=wlen) {
	 my_putch(txt[slen]);
	 slen++;
      }
   
      /* skip blanks */
      while(isspace(txt[slen]) && txt[slen]!=0) {
	 my_putch(txt[slen]);
	 slen++;
      }
   }
}

void zprintf(const char *format, ...)
{
   va_list ap;
   static char zbuffer[1024];

   va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
   vsnprintf(zbuffer, sizeof(zbuffer)-1, format, ap);
#else
   vsprintf(zbuffer, format, ap);
#endif
   va_end(ap);

   ww_print(zbuffer);
}

/*
 * File text viewer
 */

#include "saladir.h"

#include "output.h"

#define BUFSIZE 512

char *displaylines(char *txt, char *begintxt, int8u);
void displaybuffer(char *txt);
char *backline(char *start, char *curtxt);
char *nextline(char *, char *);
int32u processtext(char *txt);

bool viewfile(const char *filename)
{
   FILE *fh;
   size_t rc;
   long int fsize;
   
   char *readbuf;

   my_clrscr();

   /* read file as binary */
   fh=fopen(filename, "rb");

   my_setcolor(CH_RED);

   if(!fh) {
      my_printf("Error: viewfile() can't open text file \"%s\" "
		"for viewing.\n\n", filename);
      showmore(false, false);
      return false;
   }

   /* get size */
   fseek(fh, 0, SEEK_END);
   fsize=ftell(fh);
   fseek(fh, 0, SEEK_SET);

   if(fsize == 0) {
      my_printf("Error: viewfile() trying to load a text file \"%s\" "
		"of size 0.\n\n", filename);
      showmore(false, false);
      return false;
   }

   readbuf=(char *)malloc(sizeof(char) * (fsize+1));

   if(!readbuf) {
      my_printf("Error: viewfile can't allocate buffer of %d bytes.\n\n", 
		sizeof(char) * fsize);
      showmore(false, false);
      return false;

   }

   rc=fread(readbuf, sizeof(char), fsize, fh);
   readbuf[fsize]=0;
  
   displaybuffer(readbuf);

   fclose(fh);
   free(readbuf);
   return true;
}


char *findbottom(char *txt, int32u *lines)
{
   *lines=0;
   while(*txt!=0) {
      if(*txt=='\n')
	 (*lines)++;

      txt++;
   }

   return txt;
}

void displaybuffer(char *txt)
{
   char *curtxt, *topline, *bottom, *back, *bottomline;
   int16u key, i, perc;
   int32u linecount=0, cline=0;
   bool nocmd=false;

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

   my_clrscr();
   my_setcolor(C_WHITE);
   my_enablescroll();

   processtext(txt);

   bottom=findbottom(txt, &linecount);

   curtxt=txt;
   topline=curtxt;
   curtxt=displaylines(curtxt, txt, SCREEN_LINES-2);
   bottomline=curtxt;

   while(1) {
      nocmd=false;
      my_setcolor(C_YELLOW);
//      my_gotoxy(1, SCREEN_LINES-1);
      drawline(SCREEN_LINES-1, '=');
      my_setcolor(CH_YELLOW);
      my_cputs(SCREEN_LINES-1, 
	"-[HOME/t Top]-[END/b End]-[PGDN/+ NextPg]-[PGUP/- PrevPg]-[q Quit]-");
      my_gotoxy(1, SCREEN_LINES);
      perc=(int16u(((real)(SCREEN_LINES-2+cline)/(real)linecount)*100));
      if(perc>100) 
	 perc=100;
      my_printf("Line %4ld/%4ld (%3d%%)", 
		cline, linecount, perc);

      key=my_getch();

      if(key=='8' || key==KEY_UP) {
	 nocmd=true;
	 if(topline>txt) {
	    clearline(SCREEN_LINES-1);
	    clearline(SCREEN_LINES);
	    topline=backline(txt, topline);
	    bottomline=backline(txt, bottomline);
	    my_scroll(-1);
	    my_gotoxy(1, 1);
	    displaylines(topline, txt, 1);
	    cline--;
	 }
      }
      else if(key=='2' || key==KEY_DOWN) {
	 nocmd=true;
	 if(bottomline<bottom) {
	    clearline(SCREEN_LINES-1);
	    clearline(SCREEN_LINES);
	    my_gotoxy(1, SCREEN_LINES-1);
	    topline=nextline(topline, bottom);
	    bottomline=displaylines(bottomline, txt, 1);
	    my_scroll(1);
	    cline++;
	 }
	 
      }
      else if(key=='t' || key==KEY_HOME && topline>txt) {
	 curtxt=txt;
	 cline=0;
      }
      else if(key=='b' || key==KEY_END && bottomline<bottom) {
	 curtxt=bottom;
	 cline=linecount;
	 for(i=0; i<SCREEN_LINES-2; i++) {
	    back=curtxt;
	    curtxt=backline(txt, curtxt);
	    if(curtxt<back) 
	       cline--;
	    else
	       break;

	 }

      }
      else if(key=='+' || key==KEY_NPAGE || key==' ' || key=='3') {
	 if(bottomline>=bottom) { 
//	    curtxt=topline;
	    nocmd=true;
	 }
	 else {
	    curtxt=topline;
	    for(i=0; i<SCREEN_LINES-3; i++) {
	       curtxt=nextline(curtxt, bottom);
	       back=bottomline;
	       bottomline=nextline(bottomline, bottom);
	       cline++;
	       if(bottomline==back) 
		  break;


	    }
	 }
      }
      else if(key=='-' || key==KEY_PPAGE || key=='9') {
 	 if(topline > txt) {
	    curtxt=topline;
	    for(i=0; i<SCREEN_LINES-3; i++) {
	       back=curtxt;
	       curtxt=backline(txt, curtxt);
	       if(curtxt<back)
		  cline--;
	    }
	 }
	 else {
//	    curtxt=topline;
	    nocmd=true;
	 }
      }      
      else if(key=='q' || key==KEY_ENTER || key==PADENTER 
	      || key==10 || key==13 || key==KEY_ESC)
	 break;
      else {
//	 curtxt=topline;
	 nocmd=true;
      }

      if(!nocmd) {
	 my_clrscr();
	 my_setcolor(C_WHITE);
	 topline=curtxt;
	 curtxt=displaylines(curtxt, txt, SCREEN_LINES-2);
	 bottomline=curtxt;
      }

      
   }

   my_disablescroll();
}

char *nextline(char *curtxt, char *bottom)
{
   char *val;

   val=curtxt;
   while(*val!='\n' && val<bottom) {
      if(*val==0)
	 return curtxt;
      val++;
   }

   val++;

   if(val>=bottom)
      return curtxt;
   else 
      return val;
}


char *backline(char *start, char *curtxt)
{
   bool fl=false;

   curtxt--;
   while(curtxt>start) {

      if(*curtxt == '\n') {
	 if(fl)
	    break;
	 fl=true;
      }

      curtxt--;
   }

   if(curtxt>start)
      return curtxt+1;
   else 
      return start;
}

int32u processtext(register char *txt)
{
   char *spc=NULL;
   int32u cy=0, cx=1, wlen;

   while(*txt!=0) {
      wlen=0;

      while(!isspace(txt[wlen]) && txt[wlen]!=0)
	 wlen++;

      if((cx+wlen)>=(int32u)SCREEN_COLS && spc!=NULL) {
	 *spc='\n';
	 wlen=0;
	 cx=1;
	 cy++;
      }

      if(*txt==' ')
	 spc=txt;
	 
      if(*txt=='\n') {
	 cx=1;
	 cy++;
      }

      if(wlen) {
	 txt+=wlen;
	 cx+=wlen;
      }
      else {
	 txt++;
	 cx++;
      }
   }

   return cy;

}


char *displaylines(char *txt, char *begintxt, int8u numlines)
{
   char *dptr;
   int32s wlen, slen;
   int8u lc=0;
   bool wrap;

   static int16u lcolor=C_WHITE;

   if(!txt) {
      my_printf("Error: textpointer null at displaybuffer(char *txt)\n");
      return NULL;
   }

   if(!CONFIGVARS.colortext)
      lcolor=C_WHITE;

   dptr=txt;

   lc=0;
   slen=0;
   wlen=0;   
   my_setcolor(lcolor);

   /* wrap until end reached */
   while(dptr[slen]!=0) {
      wrap=false;
      wlen=slen;
      while(dptr[wlen]>32 && !isspace(dptr[wlen]) && dptr[wlen]!=0)
	 wlen++;

      /* wrap word if going over right border */
      if( my_getx()+(wlen-slen) >= SCREEN_COLS) {
	 my_putch('\n');
	 wrap=true;
	 lc++;

	 if(lc>=numlines)
	    return &dptr[slen];

      }

      /* print the word */
      while(slen<wlen && dptr[slen]!=0) {
	 my_putch(dptr[slen]);
	 slen++;
      }
   
      /* color support, STUPID but works */
      if(dptr[slen]>0 && dptr[slen]<8) {
	 if(CONFIGVARS.colortext) {
	    lcolor=(int16u)dptr[slen];
	    my_setcolor(lcolor);
	 }
	 slen++;
      }
      else if(dptr[slen]>=20 && dptr[slen]<28) {
	 if(CONFIGVARS.colortext) {	 
	    lcolor=(int16u)(dptr[slen]-12);
	    my_setcolor(lcolor);
	 }	 
	 slen++;
      }
      /* skip blanks */
      else if(isspace(dptr[slen]) && dptr[slen]!=0) {
	 /* strip carriage returns or else dos will get wild */
	 if(dptr[slen]!='\r')
	    my_putch(dptr[slen]);

	 if( my_getx() >= SCREEN_COLS) {
	    my_putch('\n');
	    lc++;
	 }
	 else if(dptr[slen]=='\n')
	    lc++;

	 slen++;

      }
      else 
	 slen++;

      if(lc>=numlines)
	 break;
   }

   return &dptr[slen];
}

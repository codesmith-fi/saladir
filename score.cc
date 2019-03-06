/*
 * Score.cc for Legend of Saladir
 *
 * (C)1997/1998 by Erno Tuomainen
 *
 * Score board 
 *
 */

#include "saladir.h"

#include "output.h"
#include "inventor.h"
#include "file.h"
#include "classes.h"

#include "score.h"

/* prototypes */
bool score_save(Tscore *);
bool score_checksum(Tscore *score, bool set);

//const char SAVE_SCOREDA[] = "SCORE01";
const char FILE_SCORETMP[] = "score.tmp";

#ifdef linux
const char FILE_SCOREFILE[] = "score";
#else
const char FILE_SCOREFILE[] = "score.dat";
#endif

const char txt_saveerror[] =
"\007There was a problem when trying to save your score. Please check "
"if \001/etc/saladir/score \007exists and is set up correctly. It should "
"be writable by the owner of the game executable (saladir).\n\n";

time_t score_calculate(void)
{
//   Tscore score;
   Tscore myscore;
//   Tinvpointer gold, silver, copper;

   /* calculate copper */
   my_strcpy(myscore.name, player.name, sizeof(myscore.name));

   my_strcpy(myscore.title, player.title, sizeof(myscore.title));

#ifdef linux
   my_strcpy(myscore.username, unix_username, sizeof(myscore.username));
#else
   my_strcpy(myscore.username, "Local", sizeof(myscore.username));
#endif

   my_strcpy(myscore.deathreason, player_killer, sizeof(myscore.deathreason));

   myscore.level = player.level;

   myscore.copper = inv_moneyamount(&player.inv);
   myscore.quests = 0;
   myscore.time = time(NULL);

   myscore.position = 0;
   myscore.dindex = player.dungeon;

   myscore.places = player.num_places;
   myscore.levels = player.num_levels;

   myscore.prace = player.prace;
   myscore.pclass = player.pclass;
   myscore.moves = player.movecount;

   myscore.kills = player.num_kills;

   myscore.psex = player.gender;

   myscore.final = myscore.copper / 8;
   myscore.final += myscore.levels * 2;
   myscore.final += myscore.places * 10;   
   
   score_checksum(&myscore, true);

   if(!score_save(&myscore)) {
#ifdef linux
      my_wordwraptext(txt_saveerror, 1, SCREEN_LINES, 1, SCREEN_COLS);
#else
      my_printf("Sorry, can't save your score!\n");
#endif

      return 0;
   }
   unset_myuid();

   return myscore.time;
}

/***********************************
 * SCORE SAVING                    *
 *                                 *
 ***********************************/
bool score_save(Tscore *score)
{
   FILE *scfile;
   _save_header head;
   bool firstscore=false;

   if(!chdir_todatadir() )
      return false;

   if( !testfile(FILE_SCOREFILE) )
      firstscore=true;

   /* try opening the file */
   scfile=fopen(FILE_SCOREFILE, "ab");

   if(scfile == NULL)
      return false;

   if(firstscore) {
//      strcpy(head.type, SAVE_SCOREDA);
      head.itype=SAVE_SCOREDA;
      head.version=SCORE_FILEVERSION;

      /* try writing the header */
      if(fwrite(&head, sizeof(_save_header), 1, scfile) != 1) {
	 fclose(scfile);
	 return false;
      }
   }
   /* try writing the score entry to the end of the file */
   if( fwrite(score, sizeof(Tscore), 1, scfile) != 1) {
      fclose(scfile);
      return false;
   }

   fclose(scfile);
   return true;
}

bool score_checksum(Tscore *score, bool set)
{
   int32u chk=CHECKSUM_START;
   int8u *ptr1;      

   ptr1=(int8u *)score;

   for(int16u i=0; i<sizeof(Tscore); i++, ptr1++) {
      chk+=*ptr1;
      chk <<= 1;
   }

   if(set) {
      score->checksum=chk;
      return true;
   }
  
   if(chk!=score->checksum)
      return false;
   
   return true;
}

bool score_checkintegrity(void)
{
   _save_header head;
   FILE *scfile;
//   Tscore score;

   scfile=fopen(FILE_SCOREFILE, "rb");
   if(scfile == NULL)
      return false;

   /* read header */
   if(fread(&head, sizeof(_save_header), 1, scfile)!=1) {
      fclose(scfile);
      return false;
   }

   /* check if it really is a score file */
   if( head.itype != SAVE_SCOREDA  || 
       (head.version != SCORE_FILEVERSION) ) {

      my_printf(
	 "Your score file is corrupted or old version. It must\n"
	 "be deleted in order to get a working score board!\nDeleting...");
      fclose(scfile);

      if(!deletefile(FILE_SCOREFILE))
	 my_printf("failed!\n");
      else
	 my_printf("Ok!\n");

      return false;
   }

/*
   while(fread(&score, sizeof(Tscore), 1, scfile)==1) {
      if(!score_checksum(&score, false)) {
	 my_printf("Someone has been editing the score entry of \"%s\".\n",
		   score.name );
      }
   }
*/
   fclose(scfile);   

   return true;
}

int score_compare(const void *a, const void *b)
{
   int32s res;
  
   res = (((Tscore*)b)->final - ((Tscore*)a)->final);

   if(!res) {
      res = (((Tscore*)b)->moves - ((Tscore*)a)->moves);
   }

   if(!res) {
      res = (((Tscore*)b)->time - ((Tscore*)a)->time);

   }


   if(res>0) return 1;
   if(res<0) return -1;

   return 0;
}

void score_showone(Tscore *score, int16u index)
{
   struct tm *loctime;

   loctime=localtime(&score->time);

   strftime(tempstr, sizeof(tempstr), "%H:%M %d/%m/%C%y", loctime);
   
   my_printf("%2d %7ld %s %s (l%d %s %s) [%dm]\n", index,
	     score->final, score->name, score->title,
	     score->level,
	     gendertext[score->psex],
	     npc_races[score->prace].name,
//	     classes[score->pclass].name,
	     score->moves);
#ifdef linux
   my_printf("           (%s) %s", score->username, asctime(loctime));
#else
   my_printf("           %s", asctime(loctime));
#endif
}

int32s getfilesize(FILE *stream)
{
   int32s posit;

   if(!stream)
      return 0;

   fseek(stream, 0, SEEK_END);
   posit=ftell(stream);
   fseek(stream, 0, SEEK_SET);

   if(posit < 0)
      return 0;

   return posit;
}

int16u score_oldscan(FILE *scfile, Tscore *alltimelowest, Tscore *selfscore,
		    int16u count, Tscore *scoreboard, int16u *index,
		    int16u personal, Tscore *selfboard, int16u *selfindex,
		   int16u *alltotal, int16u *perstotal, time_t current)
{
   Tscore temporary;

   int16u bastard=0;

   *index=0;
   *selfindex=0;

   while(fread(&temporary, sizeof(Tscore), 1, scfile)==1) {

      if(!score_checksum(&temporary, false)) {
	 bastard++;
	 continue;
      }

      (*alltotal)++;

      /* find all time lowest score */
      if( score_compare(alltimelowest, &temporary) <= 0)
	 *alltimelowest = temporary;

      /* build personal scores board */
      if( my_stricmp( temporary.name, player.name)==0) {
	 (*perstotal)++;

	 /* find current score */
	 if(temporary.time == current)
	    *selfscore = temporary;


	 if(*selfindex < personal) {
	    selfboard[*selfindex] = temporary;
	    (*selfindex)++;
	 }
	 else {
	    if( score_compare(&selfboard[*selfindex], &temporary) >= 0)
	       selfboard[*selfindex] = temporary;    

	    qsort(selfboard, personal+1, sizeof(Tscore), score_compare);

	 }
      }

      if((*index) < count) {
	 scoreboard[(*index)] = temporary;
	 (*index)++;
      }
      else {
	 if( score_compare(&scoreboard[*index], &temporary) >= 0)
	    scoreboard[*index] = temporary;

	 qsort(scoreboard, count+1, sizeof(Tscore), score_compare);
      }
   }

   return bastard;
}

int16u score_newscan(FILE *scfile, Tscore *wholeboard, 
		   Tscore *alltimelowest, Tscore *selfscore,
		    int16u count, Tscore *scoreboard, int16u *index,
		    int16u personal, Tscore *selfboard, int16u *selfindex,
		   int16u *alltotal, int16u *perstotal, time_t current)
{

   Tscore *ptr1;
   int16u modscore=0;

   *index=0;
   *selfindex=0;
   int16u i;

   ptr1=wholeboard;

   while( fread(ptr1, sizeof(Tscore), 1, scfile)==1) {
      if(score_checksum(ptr1, false)) {
	 ptr1++;
	 (*alltotal)++;
      }
      else
	 modscore++;
   }

   /* sort the whole array */
   qsort(wholeboard, (*alltotal), sizeof(Tscore), score_compare);
   
   for(i=0; i<(*alltotal); i++) {
      /* find all time lowest score */
      if( score_compare(alltimelowest, &wholeboard[i]) <= 0) {
	 *alltimelowest = wholeboard[i];
	 alltimelowest->position = i+1;
      }
      /* build personal scores board */
      if( my_stricmp( wholeboard[i].name, player.name)==0) {
	 (*perstotal)++;

	 /* find current score */
	 if(wholeboard[i].time == current) {
	    *selfscore = wholeboard[i];
	    selfscore->position = i+1;
	 }
	 if(*selfindex < personal) {
	    selfboard[*selfindex] = wholeboard[i];
	    selfboard[*selfindex].position = i+1;
	    (*selfindex)++;
	 }
      }

      if((*index) < count) {
	 scoreboard[(*index)] = wholeboard[i];
	 scoreboard[(*index)].position = i+1;
	 (*index)++;
      }
      
   }

   return modscore;
}

bool score_showbest(int16u count, int16u personal, time_t current)
{
   Tscore *scoreboard = NULL;
   Tscore *selfboard = NULL;
   Tscore *wholeboard = NULL;
   Tscore selfscore, alltimelowest;

   int32s ssize;
   int16u bastard=0;
   
   FILE *scfile;
   _save_header head;
   bool scfound1=false, scfound2=false;

   int16u index, selfindex, i;
   int16u perstotal = 0;
   int16u alltotal = 0;

   selfscore.final = 0;

   mem_clear(&selfscore, sizeof(Tscore));
   mem_clear(&alltimelowest, sizeof(Tscore));

   alltimelowest.final = 0x00ffffff;

   if(!chdir_todatadir() )
      return false;

   if(!score_checkintegrity())
      return false;

//   my_clrscr();

   /* allocate mem for the scoreboard */
   scoreboard=(Tscore *)malloc(sizeof(Tscore) * (count + 1));
   selfboard=(Tscore *)malloc(sizeof(Tscore) * (personal + 1));

   if(!scoreboard || !selfboard) {
      my_printf("Can't allocate memory for scoreboard, %ld bytes.\n", 
		(count + personal + 2)*sizeof(Tscore) );
      if(scoreboard)
	 free(scoreboard);
      if(selfboard)
	 free(selfboard);

      return false;
   }

   scfile=fopen(FILE_SCOREFILE, "rb");

   if(scfile == NULL) {
      free(scoreboard);
      free(selfboard);
      return false;
   }

   ssize=getfilesize(scfile);

   if(ssize>0) {
      wholeboard=(Tscore *)
	 malloc(ssize - sizeof(_save_header) + sizeof(Tscore));
   }
   else {
      my_printf("Error! Score file size is NULL!\n");
      fclose(scfile);
      free(scoreboard);
      free(selfboard);
   }

   /* read header */
   if(fread(&head, sizeof(_save_header), 1, scfile)!=1) {
      fclose(scfile);
      free(scoreboard);
      free(selfboard);
      if(wholeboard)
	 free(wholeboard);
      return false;
   }

   if(wholeboard) {
      bastard = score_newscan(scfile, wholeboard, &alltimelowest, &selfscore,
			      count, scoreboard, &index,
			      personal, selfboard, &selfindex,
			      &alltotal, &perstotal, current);	 
   }
   else {
      bastard = score_oldscan(scfile, &alltimelowest, &selfscore,
		    count, scoreboard, &index,
		    personal, selfboard, &selfindex,
		    &alltotal, &perstotal, current);
   }


   if(bastard > 0) {
      my_printf("Someone is cheating with the score file. %d records have\n"
		"been edited by someone. These records were ignored when\n"
		"building the score table you're about to see next!\n\n",
		bastard);
      showmore(false, false);
      my_clrscr();

   }


   if(index>0) {
      qsort(scoreboard, index, sizeof(Tscore), score_compare);

      my_setcolor(C_GREEN);
      my_printf("All time best scores (%d)\n", index);

      scfound1=false;
      for(i=0; i<index; i++) {
	 my_setcolor(C_WHITE);

	 if( (scoreboard[i].time == current) && !scfound1 &&
	     ( my_stricmp(scoreboard[i].name, player.name) == 0)) {

	    scfound1=true;
	    my_setcolor(CH_YELLOW);
	 }

	 if(!wholeboard)
	    score_showone(&scoreboard[i], i+1);
	 else
	    score_showone(&scoreboard[i], scoreboard[i].position);
      }
   }

   if(selfindex>0) {
      qsort(selfboard, selfindex, sizeof(Tscore), score_compare);

      my_setcolor(C_GREEN);
      my_printf("You've played %d times, here're your best scores (%d)\n", 
		perstotal, selfindex);

      scfound2=false;
      for(i=0; i<selfindex; i++) {
	 my_setcolor(C_WHITE);

	 if( (selfboard[i].time == current) && !scfound2 &&
	     ( my_stricmp(selfboard[i].name, player.name) == 0)) {

	    scfound2=true;
	    my_setcolor(CH_YELLOW);
	 }     

	 if(!wholeboard)
	    score_showone(&selfboard[i], i+1);
	 else
	    score_showone(&selfboard[i], selfboard[i].position);
      }
   }

   if(!scfound1 && !scfound2 && selfscore.final!=0 ) {
      my_setcolor(C_GREEN);
      my_printf("Your current score\n");

      my_setcolor(C_WHITE);

      if(!wholeboard)
	 score_showone(&selfscore, 0);
      else
	 score_showone(&selfscore, selfscore.position);

   }

   if(strlen(alltimelowest.name)> 0) {
      my_setcolor(C_GREEN);
      my_printf("All time worst score\n");
      my_setcolor(C_WHITE);

      if(!wholeboard)
	 score_showone(&alltimelowest, 0);
      else
	 score_showone(&alltimelowest, alltimelowest.position);
   }
   
   my_printf("\nThis game has been played a total of %d times.\n",
	     alltotal);
   
   fclose(scfile);
   free(scoreboard);
   free(selfboard);
   if(wholeboard)
      free(wholeboard);

   return true;
}

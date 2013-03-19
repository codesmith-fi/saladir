/*
** Misc utility routines
**
*/

#include "saladir.h"
#include "output.h"

#include "file.h"
#include "textview.h"
#include "_version.h"
#include "options.h"

//#include <ctype.h>

/***************************
 ** CONFIGURATION OPTIONS **
 ***************************/

const char *CFG_VARTYPES[]=
{
   "unknown",
   "Boolean",
   "String",
   "Integer",
   "Integer, signed",
   NULL
};

_Tcfginfo cfginfo[]= {
   { "GETALLMONEY", 
     "If true, you'll get all coins at once. If false, you'll"
     " be prompted for a coin count to get.", 
     &CONFIGVARS.getallmoney, CFG_BOOLVAR, 0, 1 },
   { "DROPPILES", 
     "If true, you'll drop item piles at once without any questions. "
     "If false, you'll be prompted for a count of items to get.", 
     &CONFIGVARS.droppiles, CFG_BOOLVAR, 0, 1 },
   { "ITEMDISTURB", 
     "If true, stops walk mode (repeat) when you stand over any item. If false"
     " you'll ignore items.", 
     &CONFIGVARS.item_disturb, CFG_BOOLVAR, 0, 1 },
   { "MONSTERDISTURB", 
     "If true, stops walk mode if a monster becomes visible, also disallows"
     " repeat walking if monsters are visible.", 
     &CONFIGVARS.monster_disturb, CFG_BOOLVAR, 0, 1 },
   { "DOORDISTURB", 
     "If true, stops walk mode if you walk next to a door.", 
     &CONFIGVARS.door_disturb, CFG_BOOLVAR, 0, 1 },
   { "STAIRDISTURB", 
     "If true, stops walk mode when you stand over a staircase.",
     &CONFIGVARS.stair_disturb, CFG_BOOLVAR, 0, 1 },
   { "REPEATUPDATE", "Show player in repeat mode", 
     &CONFIGVARS.repeatupdate, CFG_BOOLVAR, 0, 1 },
   { "ANYKEYMORE", 
     "If true, allows you to continue on (more)-prompts by pressing "
     "*any* key instead of just SPACE, ENTER or ESC.",  
     &CONFIGVARS. anykeymore, CFG_BOOLVAR, 0, 1 },
   { "FOODWARN", 
     "If true, warns you when you're about to faint.", 
     &CONFIGVARS.foodwarn, CFG_BOOLVAR, 0, 1 },
   { "COMPACTMSG", 
     "If true, compacts repeated messages in the message buffer. ",
     &CONFIGVARS.compactmessages, CFG_BOOLVAR, 0, 1 },
   { "COLORMSG", 
     "If true, the game will use colors in game messages as specified "
     "in the message. If false, all messages will show up in a plain "
     "white color. ", 
     &CONFIGVARS.colormessages, CFG_BOOLVAR, 0, 1 },
   { "COLORTXT", 
     "If true, the game will use colors in the text viewer as specified "
     "in the textfile. If false, all text will show up in a plain "
     "white color. ", 
     &CONFIGVARS.colortext, CFG_BOOLVAR, 0, 1 },
   { "INVERSESIGHT", 
     "(No effect at the moment) Show Line-of-Sight areas in inverse color", 
     &CONFIGVARS.inversesight, CFG_BOOLVAR, 0, 1 },
   { "SHOWLIGHT", 
     "(Not very usable at the moment) Display the torch lit area with "
     "bright yellow", 
     &CONFIGVARS.showlight, CFG_BOOLVAR, 0, 1 },
   { "AUTOPICKUP", 
     "If true, you will automagically pick up any item you walk into. "
     "But if a location contains multiple items, you will still have to pick "
     "then by yourself.",
     &CONFIGVARS.autopickup, CFG_BOOLVAR, 0, 1 },
   { "AUTODOOR", 
     "If true, you can walk to doors and you get a question whether you want "
     "to open the door doors or not. If false, doors are treated as walls "
     "and you'll have to open them manually with open-command.",
     &CONFIGVARS.autodoor, CFG_BOOLVAR, 0, 1 },
   { "AUTOPUSH", 
     "If trye, you'll automagically push unpassable items (ie. boulders). If "
     "false, you'll have to issue a push command by yourself.",
     &CONFIGVARS.autopush, CFG_BOOLVAR, 0, 1 },
   { "TARGETLINE", 
     "If true, the game will display a line when you're selecting a target "
     "for spells or missiles.",
     &CONFIGVARS.targetline, CFG_BOOLVAR, 0, 1 },
   { "EXTRALIGHT", 
     "If true, the game will show and calculate areas lit up by monsters. "
     "This will slow down the vision code quite dramatically.",
     &CONFIGVARS.lightmonster, CFG_BOOLVAR, 0, 1 },
   { "CMDSORT", 
     "If true, the list of available keyboard commands will be sorted out by "
     "the list of key presses required to execute the command. "
     "If false, the list "
     "will be sorted out by using the command name/description.",
     &CONFIGVARS.cmdsort, CFG_BOOLVAR, 0, 1 },
   { "CHEAT", 
     "If true, you will not die nor will you get your name written to the "
     "highscore table. Dead prohibited.", 
     &CONFIGVARS.cheat, CFG_BOOLVAR, 0, 1 },
   { "DEBUGMODE", 
     "If true, the game will show up all special debug messages", 
     &CONFIGVARS.DEBUGMODE, CFG_BOOLVAR, 0, 1 },

   /* integer variables */
   { "REPEATCOUNT", 
     "Specifies the default count for repeatable commands. ",
     &CONFIGVARS.repeatcount, CFG_SINTVAR, 0, 1000 },
   { "HEALTHALARM", 
     "If greater than 0, enables the hitpoint alarm. "
     "You'll get an alarm when your hp in any of your bodyparts falls below "
     "the configured percentage of the maximum hitpoints in that bodypart.",
     &CONFIGVARS.health_alarm, CFG_SINTVAR, 0, 100 },
   { "TICKSMIN", "Ticks/minute, how fast time passes", 
     &CONFIGVARS.ticksperminute, CFG_INTVAR, 100, 2000 },

   /* string variables */
   { "PICKUPTYPES", "A list of things you'll automagically pick up.",
     CONFIGVARS.pickuptypes, CFG_STRVAR, 0, sizeof(CONFIGVARS.pickuptypes)-1 },

//	{ "SPD", "Player speed", &player.stat[STAT_SPD].perm, CFG_INTVAR, -99, 99 },
//	{ "CON", "Player constitution", &player.stat[STAT_CON].perm, CFG_INTVAR, -99, 99 },
//	{ "DEX", "Player dexterity", &player.stat[STAT_DEX].perm, CFG_INTVAR, -99, 99 },

   /* string variables */

   { "", NULL, NULL, CFG_END}
};

/* place where options are stored */
_gameconfig CONFIGVARS;

const char *CFG_BOOLCHOICE[]=
{
   "False",
   "True",
   NULL
};

_Tcfginfo *config_getaddr(char *keyword)
{
   _Tcfginfo *cptr;

   if(!keyword)
      return NULL;

   cptr=cfginfo;

   while( cptr->type!=CFG_END) {
      if( my_stricmp(cptr->keyword, keyword)==0)
	 return cptr;
      cptr++;
   }

   return NULL;
}

int8u config_sprintdata(_Tcfginfo *cfgptr, char *str, bool valueonly)
{
   bool *boolptr;
   int16s *intptr;
   char *strptr;

   if(!cfgptr || !str )
      return 0;

   if(cfgptr->type==CFG_BOOLVAR) {
      boolptr=(bool *)cfgptr->ptr;
      if(!valueonly) {
	 sprintf(str, "Variable %s = %s", 
		 cfgptr->keyword, 
		 CFG_BOOLCHOICE[*boolptr] );
      }
      else
	 sprintf(str, "%s", CFG_BOOLCHOICE[*boolptr]);

   }
   else if(cfgptr->type==CFG_INTVAR || cfgptr->type==CFG_SINTVAR) {
      intptr=(int16s *)cfgptr->ptr;
      if(!valueonly)
	 sprintf(str, "Variable %s set to %d", cfgptr->keyword, *intptr);
      else
	 sprintf(str, "%d", *intptr);

   }
   else if(cfgptr->type==CFG_STRVAR) {
      strptr=(char *)cfgptr->ptr;
      if(!valueonly)
	 sprintf(str, "Variable %s set to \"%s\"", cfgptr->keyword, strptr);
      else
	 sprintf(str, "\"%s\"", strptr);

   }

   return 1;
}


void config_listvars(void)
{
   _Tcfginfo *cptr;
   FILE *fh;

   cptr=cfginfo;

   chdir_totempdir();
   fh=fopen(FILE_TMPTEXT, "w");

   if(!fh) {
      msg.newmsg("Error: unable list variables.", CH_RED);
      return;
   }
   

   fprintf(fh, "\001Configuration variables\n\n");

   while( cptr->type!=CFG_END) {
      config_sprintdata(cptr, tempstr, true);

      
      fprintf(fh, "\003%-15s \002%-10s\n\007%s\n\n", 
	      cptr->keyword, tempstr, cptr->desc);
      cptr++;
   }

   fprintf(fh, 
	   "\n\007To change/set variables, use the \"Change options command\" "
	   "(\001CTRL+O \007by default) and type the name of "
	   "the variable instead "
	   "of a '\002?\007' when prompted for a variable name.\n\n");
   fclose(fh);

   viewfile(FILE_TMPTEXT);
}

int8u config_change(_Tcfginfo *cfgptr, char *value)
{
   bool *boolptr;
   int16s *sintptr;
   int16u *intptr;
   char *strptr;

   int16s data;
   int16u udata;

   if(!cfgptr )
      return 0;

   if(cfgptr->type==CFG_BOOLVAR) {
      boolptr=(bool *)cfgptr->ptr;

      if(value==NULL) {
	 if(*boolptr==true)
	    *boolptr=false;
	 else
	    *boolptr=true;
      }
      else {
	 if(my_stricmp(value, CFG_BOOLCHOICE[1])==0)
	    *boolptr=true;
	 else
	    *boolptr=false;
      }

   }
   else if(cfgptr->type==CFG_INTVAR) {
      intptr=(int16u *)cfgptr->ptr;

      if(value==NULL) {
	 sprintf(tempstr, "Give a value for var %s (%d..%d)?", 
		 cfgptr->keyword, 
		 cfgptr->min, 
		 cfgptr->max);

	 msg.add(tempstr, C_WHITE);

	 my_gets(tempstr, sizeof(tempstr)-1);
	 udata=atoi(tempstr);
      }
      else
	 udata=atoi(value);

      if(udata>cfgptr->max)
	 udata=cfgptr->max;
      if(udata<cfgptr->min)
	 udata=cfgptr->min;

      *intptr=udata;
   }
   else if(cfgptr->type==CFG_SINTVAR) {
      sintptr=(int16s *)cfgptr->ptr;

      if(value==NULL) {
	 sprintf(tempstr, "Give a value for var %s (%d..%d)?", 
		 cfgptr->keyword, 
		 cfgptr->min, 
		 cfgptr->max);

	 msg.add(tempstr, C_WHITE);

	 my_gets(tempstr, sizeof(tempstr)-1);
	 data=atoi(tempstr);
      }
      else
	 data=atoi(value);

      if(data>cfgptr->max)
	 data=cfgptr->max;
      if(data<cfgptr->min)
	 data=cfgptr->min;

      *sintptr=data;
   }
   else if(cfgptr->type==CFG_STRVAR) {
      strptr=(char *)cfgptr->ptr;
      
      if(value==NULL) {
	 sprintf(tempstr, "Give a string for %s?", cfgptr->keyword);
	 msg.add(tempstr, C_WHITE);
	 my_gets(strptr, cfgptr->max);
      }
      else {
	 my_strcpy(strptr, value, cfgptr->max);
      }
   }

   config_sprintdata(cfgptr, tempstr, false);
   if(!value) {
      msg.newmsg(tempstr, C_GREEN);   
   }
   else
      my_cputs_init(CH_GREEN, tempstr, tempstr);

//      my_printf("%s\n", tempstr);

   return 1;
}

void game_config(void)
{
   _Tcfginfo *cfgptr;

   msg.add("Which variable to modify (? to list)?", C_WHITE);
   my_gets(nametemp, sizeof(nametemp)-1);

   if(strlen(nametemp)==0) {
      return;
   }

   if(!my_stricmp(nametemp, "list") || !my_stricmp(nametemp, "?")) {
      config_listvars();
      return;
   }

   cfgptr=config_getaddr(nametemp);

   if(cfgptr) {
/*
  config_sprintdata(cfgptr, tempstr, false);

  msg.newmsg(tempstr, C_GREEN);
*/
      config_change(cfgptr, NULL);

   }
   else {
      sprintf(tempstr, "Variable \"%s\" not found!", nametemp);
      msg.newmsg(tempstr, C_YELLOW);
   }


}

/*
 * Initialize configuration variables 
 *
 */
void config_init(void)
{

   CONFIGVARS.health_alarm=50;
   CONFIGVARS.repeatcount=50;
   CONFIGVARS.ticksperminute=TIME_1MIN;
   CONFIGVARS.item_disturb=true;
   CONFIGVARS.door_disturb=true;
   CONFIGVARS.foodwarn=true;
   CONFIGVARS.stair_disturb=true;
   CONFIGVARS.monster_disturb=true;
   CONFIGVARS.repeatupdate=true;
   CONFIGVARS.autopickup=false;
   CONFIGVARS.autodoor=true;
   CONFIGVARS.autopush=true;
   CONFIGVARS.getallmoney=true;
   CONFIGVARS.droppiles=false;
   CONFIGVARS.anykeymore=false;
   CONFIGVARS.inversesight=false;
   CONFIGVARS.compactmessages=true;
   CONFIGVARS.colormessages=true;
   CONFIGVARS.colortext=true;
   CONFIGVARS.showlight=true;
   CONFIGVARS.cmdsort=false;
   CONFIGVARS.lightmonster=false;
   CONFIGVARS.targetline=true;
   CONFIGVARS.DEBUGMODE=false;
   CONFIGVARS.cheat=false;

   my_strcpy(CONFIGVARS.pickuptypes, "$%?", sizeof(CONFIGVARS.pickuptypes)-1);

   /* not used */
   CONFIGVARS.showdice=false;
   CONFIGVARS.danger_disturb=true;

}

/* filename for configuration file */
const char FILE_CONFIGFILE[] = "saladir.cfg";

const char CONFIG_HEADER[] = 
"#$(CONFIG) Legend of Saladir options and configuration file\n"
"#Version " PROC_VERSION PROC_PLATFORM PROC_DATE "\n"
"#\n#This file gets rebuild everytime the configuration is saved\n"
"#From inside the game.\n#\n"
"#Lines beginning with '#' are comments and are not processed.\n#\n";

const char CONFIG_FOOTER[] =
"\n#$(END_CONFIG)\n\n";

/***********************************
 * SAVE ROUTINES FOR CONFIGURATION *
 ***********************************/
bool config_save(void)
{
   FILE *cfgfile;
   _Tcfginfo *cptr;

   if(!chdir_totempdir())
      return false;

#ifdef SALADIR_RELEASE
   CONFIGVARS.cheat = false;
#endif

   /* try to open the file for writing */
   cfgfile = fopen(FILE_CONFIGFILE, "w");

   if(cfgfile == NULL)
      return false;

   fprintf(cfgfile, CONFIG_HEADER);

   cptr=cfginfo;

   while( cptr->type!=CFG_END) {
      config_sprintdata(cptr, tempstr, true);
      fprintf(cfgfile, "\n# %s (%s)\n", cptr->desc, CFG_VARTYPES[cptr->type]);
      fprintf(cfgfile, "%-15s %s\n", cptr->keyword, tempstr);
      cptr++;
   }

   fprintf(cfgfile, CONFIG_FOOTER);
   
   fclose(cfgfile);

   changedir("..");

   return true;
}

bool config_load(void)
{
   FILE *cfgfile;
   _Tcfginfo *cptr;
   bool noparam=false;

   sint c;

   if(!chdir_totempdir())
      return false;

   /* try to open the file for writing */
   cfgfile = fopen(FILE_CONFIGFILE, "r");

   if(cfgfile == NULL)
      return false;

   c=fgetc(cfgfile);

   int16u xx=0;

   while(c!=EOF) {

      nametemp2[0]=0;
      i_hugetmp[0]=0;
      /* skip blanks */
      while( isspace(c) && c!=EOF)
	 c=fgetc(cfgfile);

      /* if a comment, skip the line */
      if(c=='#') {
	 while(c!=EOF && c!='\n') {	    
	    c=fgetc(cfgfile);
	 }
      }
      else if(isalnum(c)) {
	 /* read a config string */
	 xx=0;

	 while( !isspace(c) && c!=EOF && (xx < sizeof(i_hugetmp)-1)) {
	    i_hugetmp[xx++]=(int8u)c;
	    i_hugetmp[xx]=0;

	    c=fgetc(cfgfile);

	 }

	 if(c=='\n') {
	    noparam=true;
	 }
	 else {
	    noparam=false;
	    while( isspace(c) && c!=EOF) {
	       if(c=='\n')
		  noparam=true;
	       c=fgetc(cfgfile);	   
	       
	    }
	 }
	 /* read a value */
	 xx=0;

	 if(!noparam) {
	    while( !isspace(c) && c!=EOF && (xx < sizeof(nametemp2)-1)) {
	       if(c!='\"') {
		  nametemp2[xx++]=(int8u)c;
		  nametemp2[xx]=0;
	       }

	       c=fgetc(cfgfile);

	    }
	 }
      }

      if(strlen(i_hugetmp)>0) {
	 cptr=config_getaddr(i_hugetmp);

	 if(!cptr) {
	    sprintf(tempstr, 
		    "Error! Unknown configuration variable: \"%s\".\n", 
		    i_hugetmp);

	    my_cputs_init(CH_RED, false, tempstr);
			  
//	    my_printf("Error! Unknown configuration variable: \"%s\".\n", 
//		   i_hugetmp);
	 }
	 else if( strlen(nametemp2)>0) {
	    config_change(cptr, nametemp2);
	 }
	 else {
	    sprintf(tempstr, 
		    "Error! Config string \"%s\" configured inproperly.\n", 
		    i_hugetmp);
	    my_cputs_init(CH_RED, false, tempstr);
//	    my_printf("Error! Config string \"%s\" configured inproperly.\n", 
//		   i_hugetmp);
	 }
      }

      /* skip rest of the line */
      while(c!=EOF && c!='\n') {	    
	 c=fgetc(cfgfile);
      }
      
   }

   fclose(cfgfile);
   return true;
}

/**************************************************************************
 * file.cc --                                                             *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 12.04.1999                                         *
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
 *
 * 02.98 - To get Linux/Unix specific file/directory support, 'linux' must
 *         be defined when compiling Saladir.
 */

#include "file.h"

#include "imanip.h"
#include "skills.h"
#include "pathfunc.h"
#include "player.h"



#ifdef _TS_LINUX_
const char FILE_DIRSLASH[] = "/";
const char FILE_TEMPDIR[] = ".saladir"; 
const char FILE_DATADIR[] = "/etc/saladir";
const char FILE_BACKDIR[] = "..";
const char FILE_LOCKPRC[] = "saladir.is.running";
char unix_username[UNIX_USERNAMELEN];
#else
const char FILE_DIRSLASH[] = "\\";
const char FILE_TEMPDIR[] = "saladir";
const char FILE_DATADIR[] = ".\\";
const char FILE_BACKDIR[] = "..";
const char FILE_LOCKPRC[] = "running.sal";
#endif

const char FILE_SAVEDIR[] = "save";
const char FILE_PACKTMP[] = "packfile.tmp";
const char FILE_ERRORLOG[] = "error.log";
const char FILE_TMPTEXT[] = "textfile.tmp";

const char FILE_PLAYERINFO[] = "player.dat";
const char FILE_GAMEINFO[] = "game.dat";
const char FILE_SAVEPAT[] = "*.sav";
const char FILE_DATAPAT[] = "*.dat";
const char FILE_TEMPPAT[] = "*.tmp";
const char FILE_HELPDIR[] = "help";
const char ENVVAR_HOME[] = "HOME";
const char ENVVAR_USER[] = "USER";

const char FILE_MAPNAME[] = "outworld.map";
static int8u datafiles=0;
bool ROOTISPLAYING=false;

char FILE_HOMEDIR[PATH_MAX+1] = ""; /* user's home directory */
char FILE_STARTDIR[PATH_MAX+1]= ""; /* initial starting directory */

/*
 * LOCAL PROTOTYPES 
 *
 */
bool set_equipmentptrs(Tinventory *inv, _Tequipslot *eqptr);
bool set_dungeonptr(struct _dungeonleveldef **ptr, playerinfo *plr);

bool save_separatesavefile(void);
bool save_concatsavefile(void);

bool write_header(int16u type, _FH_ fh);
int16u read_header(_FH_ fh);
bool load_inventory(Tinventory *inv, _FH_ fh, inv_info *buf);
bool save_inventory(Tinventory *inv, _FH_ fh);
bool save_conditions(Tcondpointer cptr, _FH_ fh);
bool load_condition(Tcondpointer *cptr, _FH_ fh);
bool load_skill(Tskillpointer *sptr, _FH_ fh);
bool save_skills(Tskillpointer sptr, _FH_ fh);
bool save_questlist(Tquestpointer qptr, _FH_ fh);
bool load_quest(Tquestpointer *qptr, _FH_ fh);
bool save_pathlist(Tpathlist plst, _FH_ fh);
bool load_pathnode(Tpathlist *plst, _FH_ fh);
bool load_globals(_FH_ fh);
bool save_globals(_FH_ fh);
bool load_player(_FH_ fh);
bool save_player(_FH_ fh);

/*******************/
/* BASIC FUNCTIONS */
/*******************/

/* user id variables, suid */
static uid_t euid, ruid;

/*
 * Set user id from executable
 */
#ifdef _TS_LINUX_

const char txt_suidwarn[]=
"\007Please, check that the Set-User-ID bit (\001SUID\007) is "
"set in the main executable. "
"You can use the command \"\002chmod 4755 saladir\007\" to set it and give "
"everyone read access to the game.\n"
"Also, you shouldn't be playing as root user because you will mess up "
"access permissions of the global score file.\n";

bool set_myuid(void)
{
   sint status=0;

   /* if root is playing, do not change user id */
   if(ROOTISPLAYING)
      return true;

#ifdef _POSIX_SAVED_IDS
   status=setuid(euid);
#else
   status=setreuid(ruid, euid);
#endif
   if( status < 0) {
      my_clrscr();
      my_printf("Warning: Can't set the effective process id!\n");
      my_wordwraptext((char *)txt_suidwarn, 1, SCREEN_LINES, 
		      1, SCREEN_COLS);
      showmore(false, false);
      return false;
   }
   return true;
}

/*
 * set normal user id back
 */
bool unset_myuid(void)
{
   sint status=0;

   /* if the root is playing, do not change user id */
   if(ROOTISPLAYING)
      return true;

#ifdef _POSIX_SAVED_IDS
   status=setuid(ruid);
#else
   status=setreuid(euid, ruid);
#endif

   if(status < 0) {
      my_clrscr();
      my_printf("Warning: Can't set the user process id!\n");
      my_wordwraptext((char *)txt_suidwarn, 1, SCREEN_LINES, 
		      1, SCREEN_COLS);
      showmore(false, false);
      return false;
   }
   return true;
}

void init_uid(void)
{
   /* if root is playing, do not change user id */
   if(ROOTISPLAYING)
      return;

   ruid=getuid();
   euid=geteuid();
   unset_myuid();  
}

/*
 * Get file stats and access flags
 *
 */
bool getfilestat(const char *filename, struct stat *filestat)
{
     if(stat(filename, filestat)==0)
	  return true;

     return false;
}
#else
bool set_myuid(void) { return true; }
bool unset_myuid(void) { return true; }
void init_uid(void) { };
bool getfilestat(const char *filename, struct stat *filestat) { return false; }
#endif

const char txt_rootwarn[]=
"\007Warning: You are playing as *\001root\007*!\n"
"You will mess up the system global score file. If you "
"want all users to be able to play and save their scores "
"into the global score file, then DO NOT play as root!\n\n";

/*****************************
 * Get user's home directory *
 *****************************/
bool get_userhomedir(char *homedir, sint maxlen)
{
#ifndef _TS_LINUX_
   my_strcpy(homedir, ".\\", maxlen);
#else
   struct passwd *udata=NULL;

   if(CONFIGVARS.DEBUGMODE) {
      uid_t effeu, realu;
      realu=getuid();
      effeu=geteuid();      

      udata=getpwuid(getuid());

      my_clrscr();
      my_printf("Process information (%d, %d): \n", effeu, realu);
      if(udata) {
	 my_printf("Effective user is: %s (%d)\n", udata->pw_name,
		   udata->pw_uid);
	 my_printf("Belongs to group : %d\n", udata->pw_gid);
      }
      else
	 my_printf("Error! getpwuid() on effective user id failed!\n");

      showmore(false, false);
   }


   /* get data for user */ 
   udata=getpwuid(getuid());   

   /* did we get the passwd-data? */
   if(!udata) {

      /* if not, try environment variables "HOME" and "USER" */
      char *env_user, *env_home;
      env_home = getenv(ENVVAR_HOME);
      if(env_home) {
	 my_strcpy(homedir, env_home, maxlen);
      }
      else {
	 my_cputs_init(CH_RED, false, 
		       "Can't find your home directory!");
	 return false;
      }

      env_user = getenv(ENVVAR_USER);
      if(env_user) {
	 my_strcpy(unix_username, env_user, sizeof(unix_username));
      }
      else {
	 /*
	  * unix username is not so crucial for the workings of the 
	  *  program, so in case of failure use something...
	  */
	 my_strcpy(unix_username, "player", sizeof(unix_username));
      }
    }
   else {
      /* store the username for later use */
      my_strcpy(unix_username, udata->pw_name, sizeof(unix_username));
      my_strcpy(homedir, udata->pw_dir, maxlen);

      if(CONFIGVARS.DEBUGMODE) {
	 my_printf("\nReal user is: %s (%d)\n", udata->pw_name, udata->pw_uid);
	 my_printf("Belongs to group : %d\n", udata->pw_gid);
      }
   }

   /* give a root warning */
   if(my_stricmp(unix_username, "root")==0) {

      /* set global ROOTISPLAYING to true for later use */
      ROOTISPLAYING=true;
      my_wordwraptext((char *)txt_rootwarn, 1, SCREEN_LINES, 
		      1, SCREEN_COLS);

      showmore(false, false);
   }

#endif
   return true;
}


bool init_directories(void)
{
//     struct stat fstats;

   init_uid();
   unset_myuid();

   if(!getcwd(FILE_STARTDIR, PATH_MAX)) {
      my_cputs_init(CH_RED, false, "Can't get the working directory!");

//      my_printf("Can't get working directory!\n");
      return false;
   }

#ifdef _TS_LINUX_
   if(!get_userhomedir(FILE_HOMEDIR, sizeof(FILE_HOMEDIR)-1)) {
      my_cputs_init(CH_RED, false, "Can't locate your home directory!");
//      my_printf("Can't locate your home directory!\n");
      return false;
   }

#else
   my_strcpy(FILE_HOMEDIR, FILE_STARTDIR, PATH_MAX);
#endif

   /* test the data directory access */
   set_myuid();
   if(changedir(FILE_DATADIR)) {
      if(!testfile(FILE_MAPNAME)) {

	 my_cputs_init(CH_RED, false, 
		       "You've no access to Saladir datafiles!");
//	 my_printf("You've no access to Saladir datafiles!\n");
	 return false;
      }

      /* datafiles in /etc/saladir/ */
      datafiles=DATAFILES_INETC;

      unset_myuid();
   }
   else {
      unset_myuid();
      if(!testfile(FILE_MAPNAME)) {
	 my_cputs_init(CH_RED, false, 
		       "You've no access to Saladir datafiles!");
//	 my_printf("You've no access to Saladir datafiles!\n");
	 return false;
      }

      datafiles=DATAFILES_INHOME;
   }

   /* go to home dir */
   if(!changedir(FILE_HOMEDIR) ) {
      my_cputs_init(CH_RED, false, 
		       "Can't enter your home directory!");
//      my_printf("Can't enter the home directory!\n");
      return false;
   }

   /* test if temp already exists */   
   if( changedir(FILE_TEMPDIR) ) {
      if(!changedir(FILE_BACKDIR) )
	 return false;
   }
   else {
      /* Create the temporary data dir */
      if(!makedir(FILE_TEMPDIR) ) {
	 my_cputs_init(CH_RED, false, 
		       "Can't create temporary directory!");
//	 my_printf("Can't create temporary directory \"%s\".\n", 
//		   FILE_TEMPDIR);	 

	 return false;
      }
   }

   if(testfile(FILE_LOCKPRC)) {
      my_clrscr();
#ifdef _TS_LINUX_
      my_printf("Your home directory contains a file \"%s\",\n", 
		FILE_LOCKPRC);
#else
      my_printf("In the Saladir directory there's a file \"%s\",\n",
		FILE_LOCKPRC);
#endif
      my_printf("which indicates that you're already playing the game.\n\n"
		"If you're not playing it then the game might have\n"
		"crashed while you were playing it. In that case you\n"
		"should delete the file \"%s\" by yourself.\n\n", 
		FILE_LOCKPRC);
      return false;
   }

   /* ensure that no old tempfiles exist */
   clean_tmpfiles();

   createfile(FILE_LOCKPRC);

   return true;
}

bool clean_tmpfiles(void)
{
   unset_myuid();

   /* remove lock file */
   if(changedir(FILE_HOMEDIR) ) {
      deletefile(FILE_LOCKPRC);
   }

   return clean_tempdir();
}

/*
 * Clean the temporary directory
 *
 */
bool clean_tempdir(void)
{
   DIR *dp;
   struct dirent *ep;
   sint errors=0;

   if(!chdir_totempdir()) {
      my_printf("Can't access temporary directory!\n");
      return false;
   }

   /* open the temp dir */
   dp = opendir(".");

   if(dp != NULL) {
      while( (ep = readdir(dp)) ) {

	 /* match only temp files */
	 if(!fnmatch(FILE_TEMPPAT, ep->d_name, 0)) {
	    if(!deletefile(ep->d_name)) {
	       my_printf("Error while deleting \"%s\".\n", ep->d_name);
	       errors++;
	    }
	 }
	 if(!fnmatch(FILE_DATAPAT, ep->d_name, 0)) {
	    if(!deletefile(ep->d_name)) {
	       my_printf("Error while deleting \"%s\".\n", ep->d_name);
	       errors++;
	    }
	 }

      }
      closedir(dp);
   }
   else
      return false;

   changedir(FILE_HOMEDIR);

   return true;
}

/*
 * Changes cwd to the data directory
 *
 */
bool chdir_todatadir(void)
{
#ifdef _TS_LINUX_
   if( datafiles == DATAFILES_INETC ) {
      set_myuid();

      if( changedir(FILE_DATADIR) )
	 return true;
      return false;
   }
   else {      
      if( changedir(FILE_STARTDIR) ) 
	 return true;

      return false;
   }
#else
   if( changedir(FILE_STARTDIR) )
      return true;
   else
      return false;
#endif
}

bool chdir_tohelpdir(void)
{
   if(chdir_todatadir()) {
      return changedir(FILE_HELPDIR);
   }

   return false;
}

/* changes cwd to temp directory */
bool chdir_totempdir(void)
{
#ifdef _TS_LINUX_
   unset_myuid();
   if( changedir(FILE_HOMEDIR) ) {
      if( changedir(FILE_TEMPDIR) )
	 return true;
   }
   return false;
#else
   if( changedir(FILE_STARTDIR) ) {
      if( changedir(FILE_TEMPDIR) )
	 return true;
   }
   return false;
#endif
}

bool deletefile(const char *filename)
{
   if(!remove(filename))
      return true;

   return false;
}

/*
 * Make a file and close it
 */
bool createfile(const char *filename)
{
   _FH_ fh;

   fh=open(filename, O_WRONLY|O_CREAT|O_BINARY|O_TRUNC, 
	   UNIX_FILEPERMBITS);

   if(fh<0)
      return false;

   close(fh);
   return true;
}


/* test if file exists, return true if exists */
bool testfile(const char *filename)
{
   _FH_ fileh;

   fileh=open(filename, O_RDONLY|O_BINARY);

   if(fileh<0) {
      return false;
   }

   close(fileh);
   return true;
}

bool changedir(const char *dirname)
{
   if(!dirname)
      return false;

   if(!chdir(dirname))
      return true;

   return false;
}

bool makedir(const char	*dirname)
{
   if( strlen(dirname)<1 )
      return false;

   if( !mkdir(dirname, UNIX_DIRPERMBITS ) )
      return true;

   return false;
}


long int filesize(const char *filename)
{
   FILE *fh;
   long int a=0;

   fh = fopen(filename, "r");

   if(fh == NULL)
      return -1;

   if(fseek(fh, 0, SEEK_END)) {
      fclose(fh);
      return -1;
   }

   a = ftell(fh);

   fclose(fh);

   return a;

}

/***********************/
/* SAVE/LOAD FUNCTIONS */
/***********************/

/*
 * Save level structures into a file. For the editor 
 *
 */
bool edit_savemap(level_type *level, char *filename)
{
   _FH_ mapfile;

   /* empty level, do not save */
   if(!level) {
      return false;
   }

   if(!filename) {
      return false;
   }

   for(int16s j=0; j<level->sizey; j++) {
      for(int16s i=0; i<level->sizex; i++) {
	 set_terrain(level, i, j, level->loc[j][i].type);

      }
   }
   
   if(strlen(filename)<1) {
      sprintf(tempstr, "Illegal filename! \"%s\".", filename);
      msg.newmsg(tempstr, C_RED);

      return false;
   }

   /* create a file for writing */
   mapfile=open(filename, O_WRONLY|O_CREAT|O_BINARY|O_APPEND|O_TRUNC, 
		UNIX_FILEPERMBITS);
   
   if( mapfile<0 ) {
      sprintf(tempstr, "Can't open file \"%s\"!", filename);
      msg.newmsg(tempstr, C_RED);
      return false;
   }

   /* save level structures */
   if(save_leveldata(level, mapfile)) {
      close(mapfile);
      return true;
   }

   close(mapfile);

   return false;
}

/*
 * A load routine for level editor, loads level structures only
 *
 */
bool edit_loadmap(level_type *level, char *filename)
{
   _FH_ mapfile;
   int16u res;

   if(!filename) {
      return false;
   }

   if(strlen(filename)<1) {
      sprintf(tempstr, "Illegal filename! \"%s\".\n", filename);
      msg.newmsg(tempstr, C_RED);
      return false;
   }

   mapfile=open(filename, O_RDONLY|O_BINARY);
   if(mapfile<0) {
      sprintf(tempstr, "Can't open file \"%s\"!\n", filename);
      msg.newmsg(tempstr, C_RED);
      return false;
   }

   if( (res=read_header(mapfile)) != (int16u)SAVE_LEVDATA ) {
      msg.vnewmsg(CH_RED, "File %s is not a mapfile (%02x).", filename, res);
      showmore(false, false);
      return false;
   }
   else {
      /* free old level */
      freelevel(level);
	 
      if(load_leveldata(level, mapfile)) {
	 close(mapfile);
	 return true;
      }
   }

   close(mapfile);
   return false;
}

/* function which saves level objects to file */
#ifdef PASSIJAHAMMASHARJA
bool OLD_save_objects(level_type *level, char	*filename)
{
   _FH_ outfile;
   int res;

   int32u itemcount;
   Tinvpointer itemptr;
   _save_header head;

   itemptr=level->inv.first;

   while(itemptr)	{
      itemcount++;
      itemptr=itemptr->next;
   }
   itemptr=level->inv.first;

   strncpy(head.type, SAVE_LEVITEM,	HEADER_NAMELEN);
   head.version=CURRENT_FILEVERSION;


   outfile=open(filename, O_WRONLY|O_CREAT|O_BINARY|O_TRUNC, 
		UNIX_FILEPERMBITS);
   if(outfile)	{
      /* write the number of items */
      //   res=write(outfile, &itemcount, sizeof(int16u));

      while(itemptr)	{
	 /* save header for current item */
	 res=write(outfile, &head, sizeof(_save_header));

	 /* save the item */
	 res=write(outfile, itemptr, sizeof(inv_info));

	 itemptr=itemptr->next;
      }
      close(outfile);
   }
   else return	false;

   return true;
}

/* function which saves level objects to file */
bool save_levelitems(level_type *level, _FH_ fh)
{
   int res;

   Tinvpointer itemptr;

   if(fh<0)
      return false;

   /* init header */

   itemptr=level->inv.first;
   while(itemptr)	{
      /* save header for current item */
      res=write(fh, &head, sizeof(_save_header));
      if(res!=sizeof(_save_header))
	 return false;

      /* save the item data */
      res=write(fh, itemptr, sizeof(inv_info));

      if(res!=sizeof(inv_info))
	 return false;

      itemptr=itemptr->next;
   }

   return true;
}


bool load_levelitem(level_type *level, _FH_ fh)
{
   int res;
   inv_info itemdata, *ptr ;

   res=read(fh, &itemdata, sizeof(inv_info));
   if(res==sizeof(inv_info) ) {

      /* check if object exists in level boundaries! */
      if(itemdata.x <= 0 || itemdata.y <= 0
	 || itemdata.x >= level->sizex || itemdata.y >= level->sizey) {

	 my_printf("Object %s is beyond level coords at %d,%d.\n",
		   itemdata.i.name, itemdata.x, itemdata.y);
	 return false;
      }
      else {
	 if(CONFIGVARS.DEBUGMODE) {
	    my_printf("Object %s loaded at %d,%d (max is %d,%d).\n",
		      itemdata.i.name, itemdata.x, 
		      itemdata.y, level->sizex-1, level->sizey-1);
	 }
      }
      ptr=inv_inititem(&level->inv);

      if(ptr) {
	 
	 /***************** HACK *******************/
	 itemdata.i.inv = NULL;

	 /* now copy loaded data to item list */
	 itemdata.next=ptr->next;
	 *ptr=itemdata;
      }
      else {
	 return false;
      }
   }
   else
      return false;

   return true;
}

bool load_invitem(level_type *level, _FH_ fh, Tinventory *inv)
{
   sint res;
   Tinvpointer invptr;

   if(fh<0)
      return false;

   inv_info invdata;

   res=read(fh, &invdata, sizeof(inv_info) );
   if(res==sizeof(inv_info) ) {
      invptr=inv_inititem(inv);
      //		invptr=NULL;
      if(invptr) {
	 invptr->i=invdata.i;
	 invptr->count=invdata.count;
	 if(CONFIGVARS.DEBUGMODE) {
	    my_printf("Loaded item %s for monster.\n",
		      invdata.i.name );
	 }
      }
      else {
	 my_printf("Monster inventory init failed! (No memory?)\n");
	 return false;
      }
   }
   else
      return false;

   return true;
}

bool load_levelrooms(level_type *level, _FH_ file)
{
   sint res;

   if(file<0 || !level)
      return false;

   res=read(file, level->rooms, sizeof(_roomdef)*(ROOM_MAXNUM+1));

   if( res!=(sizeof(_roomdef)*(ROOM_MAXNUM+1)) )
      return false;

   return true;

};


bool save_levelrooms(level_type *level, _FH_ fh)
{
   sint res;

   if(!write_header(SAVE_ROOMDAT, fh)) {      
      return false;
   }

   res=write(fh, level->rooms, sizeof(_roomdef)*(ROOM_MAXNUM+1));

   if( res!=(sizeof(_roomdef)*(ROOM_MAXNUM+1)) )
      return false;

   return true;
};

#endif

/*
 * This function writes the level structure into a file
 * which was previously opened (fh)
 *
 * Data saved:
 *
 * saveheader   struct _save_header (itype = SAVE_LEVDATA)
 * levelheader  struct _save_levheader
 * leveldata    series of cave_type structures
 *
 */
bool save_leveldata(level_type *level, _FH_ fh)
{
   ssize_t rr;
   int16u i;
//   _save_levheader levhead;

   level_type *tmptype;

   if(fh<0)
      return false;

   /* init save header */
   if(!write_header(SAVE_LEVDATA, fh)) {
      msg.newmsg("Failed writing the map header!", CHB_RED);
      return false;
   }

   tmptype = (level_type *) malloc(sizeof(level_type)+1);
   if(!tmptype) {
      msg.newmsg("Unable to allocate memory!", CHB_RED);
      return false;
   }

   *tmptype = *c_level;

   /* clear pointers */
   tmptype->monsters=NULL;
   tmptype->inv.first=NULL;
   for(i=0; i<MAXSIZEY; i++)
      tmptype->loc[i]=0;

   rr = write(fh, tmptype, sizeof(level_type));
   if(rr != sizeof(level_type)) {
      free(tmptype);
      return false;
   }

   /* write the level data */
   for(i=0; i<level->sizey; i++) {
      rr = write(fh, level->loc[i], sizeof(cave_type)*level->sizex);

      /* if writing failed */
      if( rr != (ssize_t)( sizeof(cave_type) * level->sizex) ) {
	 free(tmptype);
	 return false;
      }
   }
  
   free(tmptype);
   return true;

#ifdef sontaralli
   /* init level header */
   //   sprintf(tempstr, "Level size is %d:%d", level->sizex, level->sizey);
   //   msg.newmsg(tempstr, C_GREEN);
   levhead.sizex=level->sizex;
   levhead.sizey=level->sizey;
   levhead.danglev=level->danglev;
   levhead.roomcount=level->roomcount;
   if( write(fh, &levhead, sizeof(_save_levheader)) != 
       sizeof(_save_levheader)) {

      msg.newmsg("Failed writing the level data header!", CHB_RED);
      return false;
   }

   /* write the level data */
   for(i=0; i<level->sizey; i++) {
      wr_res=write(fh, level->loc[i], sizeof(cave_type)*level->sizex);

      /* if writing failed */
      if(wr_res!=( sizeof(cave_type)*level->sizex) ) {
	 msg.newmsg("Failed writing MAPFILE", CHB_RED);
	 return false;
      }
   }

   return true;
#endif
}

/*
 * A function which restores level structure data 
 * from a file (fh)
 *
 */
bool load_leveldata(level_type *level, _FH_ fh)
{
   sint i;
   ssize_t rr;

   if(fh<0 || !level)
      return false;

   rr = read(fh, level, sizeof(level_type));
   if(rr != sizeof(level_type)) {
      return false;
   }

   if(level->sizex > MAXSIZEX || level->sizey > MAXSIZEY) {
      my_printf("Level data was corrupted!\n");
      return false;
   }

   /* read entire level data */
   for(i=0; i<level->sizey; i++) {
      /* allocate memory for a row */
      level->loc[i] = new cave_type[level->sizex]; 
      if(!level->loc[i]) {
	 return false;
      }

      /* read one horizontal line of data */
      rr=read(fh, level->loc[i], sizeof(cave_type)*level->sizex);

      /* ensure correct size of read */
      if(rr != (ssize_t)(sizeof(cave_type) * level->sizex) ) {
	 return false;
      }
   }



/*
  level->sizex=1000;
  level->sizey=1000;

  res=read(fh, &head, sizeof(_save_levheader));
  if(res!=sizeof(_save_levheader))	{
  return false;
  }

  level->sizex=head.sizex;
  level->sizey=head.sizey;
  level->danglev=head.danglev;
  level->roomcount=head.roomcount;

  if(level->sizex > MAXSIZEX || level->sizey > MAXSIZEY) {
  return false;
  }
  else {
  for(i=0; i<level->sizey; i++) {
  level->loc[i] = new cave_type[level->sizex]; // allocate row
  if(!level->loc[i]) {
  return false;
  }
  res=read(fh, level->loc[i], sizeof(cave_type)*level->sizex);
  if((res<0) || !res) {
  return false;
  }
  }
  }
*/
   return true;
}



bool save_tmpfile(level_type *level, int16s ndungeon, int16s nlevel)
{
   _FH_ fh;
   bool res=true;
   char filename[40];

   /* open the file for writing, create and truncate */
   fh=open(FILE_PACKTMP, O_WRONLY|O_CREAT|O_BINARY|O_TRUNC, 
	   UNIX_FILEPERMBITS);

   if(fh) {

      /* save level structure */

      if(!save_leveldata(level, fh))
	 msg.newmsg("couldn't save level!", CH_RED);

      /* save level inventory (=items) */
      if(!save_inventory(&level->inv, fh))
	 msg.newmsg("Couldn't save level items!", CH_RED);

      if(!save_monsters(level, fh))
	 msg.newmsg("Couldn't save monsters!", CH_RED);

      close(fh);
   }
   else
      res=false;

   sprintf(filename, "D%02dL%02d.dat", ndungeon, nlevel);

   compress(FILE_PACKTMP, filename);
   deletefile(FILE_PACKTMP);

   return res;
}

bool load_tmpfile(level_type *level, char *filename, bool compr)
{
   _FH_ fh;
   sint errors;
   sint moncount, itemcount;
   sint roomcount;
   int16u i;
   inv_info *ibuf=NULL;
   int16u htype;
   _monsterlist *lastmonster=NULL;
   bool endfile, playerload;

//   my_clrscr();
   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

   if(!testfile(filename))
      return false;

   if(compr) {
      if( !decompress(filename, FILE_PACKTMP))
	 return false;

      fh=open(FILE_PACKTMP, O_RDONLY|O_BINARY);
   }
   else {
      fh=open(filename, O_RDONLY|O_BINARY);
   }

   if(fh<0)
      return false;

   ibuf = (inv_info *)malloc(sizeof(inv_info)+1);

   if(!ibuf) {
      my_printf("load_tmpfile(): Failed to allocate memory of %ld bytes\n", 
		sizeof(inv_info)+1);
      close(fh);
      return false;
   }

   playerload=false;
   endfile=false;
   roomcount=0;
   moncount=0;
   itemcount=0;
   errors=0;
   while(!endfile && !errors) {
      /* read header first */
      htype = read_header(fh);

      switch(htype) {
	 case 0:
	    my_setcolor(CH_RED);
	    my_printf("ERROR while reading temporary file");
	    errors++;
	    break;
	 case 1:
	    endfile=true;
	    break;
	 case SAVE_LEVDATA: /* level data */
	    if(!load_leveldata(level, fh)) {
	       errors++;
	       my_printf("FATAL! Level structure loading failed!\n");
	       break;
	    }
	    break;
	 case SAVE_STATTYP: /* status and condintions */
	    if(playerload) {
	       if(!load_condition(&player.conditions, fh)) {
		  my_printf("FATAL! Player condition structure loading failed!\n");
		  errors++;
	       }
	    }
	    else if(lastmonster) {
	       if(!load_condition(&lastmonster->conditions, fh)) {
		  my_printf("FATAL! Monster condition structure loading failed!\n");
		  errors++;
	       }
	    }
	    else
	       errors++;

	    break;
	 case SAVE_WTHRTYPE:
	    if(!weather.load(fh)) {
	       my_printf("Can't load weather information.\n");
	       errors++;
	    }
	    break;
	 case SAVE_PASSTIME:
	    if(!passedtime.load(fh)) {
	       my_printf("Can't load game time information (passedtime).\n");
	       errors++;
	    }
	    break;
	 case SAVE_GAMETIME:
	    if(!worldtime.load(fh)) {
	       my_printf("Can't load game time information (worldtime).\n");
	       errors++;
	    }
	    break;
	 case SAVE_PATHCRD:
	    if(lastmonster) {
	       if(!load_pathnode(&lastmonster->path, fh)) {
		  my_printf("FATAL! Monster path load failed!\n");
		  errors++;
	       }
	    }
	    else
	       errors++;
	    break;
	 case SAVE_SKLTYPE: /* skill structure */
	    if(playerload) {
	       if(!load_skill(&player.skills, fh)) {
		  my_printf("FATAL! Player skill loading failed!\n");
		  errors++;
	       }
	    }
	    else if(lastmonster) {
	       if(!load_skill(&lastmonster->skills, fh)) {
		  my_printf("FATAL! Player skill loading failed!\n");
		  errors++;
	       }
	    }
	    else
	       errors++;
	    break;
	 case SAVE_INDVQST: /* quest information */
	    if(!load_quest(&player.quests, fh)) {
	       my_printf("FATAL! Player quest loading failed!\n");
	       errors++;
	    }
	    break;
	 case SAVE_INVHEAD: /* inventory */
	    if(playerload) { /* load into player */
	       if(!load_inventory(&player.inv, fh, ibuf)) {
		  my_printf("Failed to load player inventory!\n");
		  errors++;	      
	       }
	       else {
		  set_equipmentptrs(&player.inv, player.equip);		 
	       }
	    }
	    else if(!lastmonster) { /* load into level */
	       if(!load_inventory(&level->inv, fh, ibuf)) {
		  my_printf("Failed to load level inventory!\n");
		  errors++;	      
	       }
	       else {
		  set_equipmentptrs(&level->inv, NULL );
	       }
	    }
	    else { /* load into lastmonster */
	       if(!load_inventory(&lastmonster->inv, fh, ibuf)) {
		  my_printf("Failed to load monster inventory!\n");
		  errors++;
	       }
	       else {
		  set_equipmentptrs(&lastmonster->inv, lastmonster->equip);
	       }
	    }
	    break;
/*
  case SAVE_ROOMDAT:
  if(load_levelrooms(level, fh))
  roomcount++;
  else {
  errors++;
  my_printf("Room data loading failed!\n");
  }	    
  break;
*/
	 case SAVE_PLRDATA: /* player and game information */
	    playerload=true;
	    freeall_player();

	    if( !load_player(fh) ) {
	       my_printf("Player loading failed!\n");
	       errors++;
	    }
	    break;
	 case SAVE_NPCTYPE: /* monster structure */
	    playerload = false;
	    lastmonster=load_monster(level, fh);
	    if(lastmonster) {
	       moncount++;
	       
	    }
	    else {
	       errors++;
	       my_printf("Monster loading failed!\n");
	    }
	    break;
	 case SAVE_GLOBALD: /* game global data */
	    if(!load_globals(fh) ) {
	       my_printf("Global game info loading failed!\n");
	       errors++;
	    }
	    break;
	 default:
	    my_printf("Unknown header type %02x", htype);
	    errors++;
	    break;
      }

      if(my_gety()>SCREEN_LINES-2) {
	 showmore(false, false);
	 my_clrscr();
      }
   }

   /* free resources */
   close(fh);
   free(ibuf);
   ibuf=NULL;

   /* remove temporary unpacked file */
   deletefile(FILE_PACKTMP);

   for(i=0; i<ROOM_MAXNUM; i++) {
      level->rooms[i].owner=NULL;
   }

   /* rebuild monster targets and room owners */
   _monsterlist *m1ptr, *m2ptr;
   if(moncount>1) {
      m1ptr=level->monsters;
      while(m1ptr) {

 	 /* set room owners */
	 if(m1ptr->roomnum>=0 && m1ptr->roomnum<=ROOM_MAXNUM) {
	    level->rooms[m1ptr->roomnum].owner=m1ptr;
	 }

	 /* if target coords are 0 then there should be no target */
	 if(m1ptr->targetx==0 && m1ptr->targety==0) {
	    m1ptr=m1ptr->next;
	    continue;
	 }

	 if(m1ptr->targetx>0 && m1ptr->targety>0) {
	    m1ptr->target=NULL;
	    m2ptr=level->monsters;
	    while(m2ptr) {
	       if(m2ptr->x == m1ptr->targetx && 
		  m2ptr->y == m1ptr->targety) {
		  m1ptr->targetx=0;
		  m1ptr->targety=0;
		  m1ptr->target=m2ptr;
		  break;
	       }
	       m2ptr=m2ptr->next;
	    }
	    if(!m1ptr->target) {
	       errors++;
	       my_printf("Target for %s does not exist!", 
			 m1ptr->m.desc);
	    }
	 }
	 m1ptr=m1ptr->next;
      }
   }


   if(errors>0) {
      my_setcolor(CHB_RED);
      my_printf("There were %d errors! Press any key.", errors);
      my_getch();
      return false;
   }

   if(moncount && itemcount && CONFIGVARS.DEBUGMODE)
      my_getch();

   my_printf(".");
   my_refresh();

   return true;
}

/*
 * A function which saves all level monsters into previously
 * opened file (fh)
 */
bool save_monsters(level_type *level, _FH_ fh)
{
   int res;
   _monsterlist *mptr;
   //   _save_monheader monhead;

   if(fh<0)
      return false;

   mptr=level->monsters;

   /* process all monsters */
   while(mptr) {

      /* save monster header */
      if(!write_header(SAVE_NPCTYPE, fh)) {
	 msg.newmsg("Failed to write the monster header!", CHB_RED);
	 return false;
      }

      /* set target coordinates to targetx, targety for later use */
      mptr->targetx=0;
      mptr->targety=0;
      if(mptr->target!=NULL) {
	 mptr->targetx=mptr->target->x;
	 mptr->targety=mptr->target->y;
      }

      /* save monster data */
      res=write(fh, mptr, sizeof(_monsterlist));
      if(res!=sizeof(_monsterlist))
	 return false;

      /* save monster inventory */
      if(!save_inventory(&mptr->inv, fh)) {
	 msg.newmsg("Failed to save monster inventory!", CHB_RED);
	 return false;
      }

      /* save monster skills */
      if(!save_skills(mptr->skills, fh)) {
	 msg.newmsg("Failed to save monster skills!", CHB_RED);
	 return false;
      }

      /* save monster conditions */
      if(!save_conditions(mptr->conditions, fh)) {
	 msg.newmsg("Failed to save monster conditions!", CHB_RED);
	 return false;
      }

      /* save monster pathlist */
      if(!save_pathlist(mptr->path, fh)) {
	 msg.newmsg("Failed to save monsters walkpath!", CHB_RED);
	 return false;
      }

      /* to next monster */
      mptr=mptr->next;
   }
   return true;
}

/*
 * A routine which loads a single monster from a file 
 *
 */
_monsterlist *load_monster(level_type *level, _FH_ fh)
{
   int res;
   _monsterlist mondata, *ptr ;

   if(fh<0)
      return NULL;

   res=read(fh, &mondata, sizeof(_monsterlist) );
   if(res==sizeof(_monsterlist) ) {

      /* is monster beyond level limits */
      if(mondata.x <= 0 || mondata.y <= 0
	 || mondata.x >= level->sizex || mondata.y >= level->sizey) {
	 my_printf("Loaded monster is beyond level limits!\n"
		   "\t%s at X:%d Y:%d (skipping)", mondata.m.desc, 
		   mondata.x, mondata.y);
	 return NULL;
      }
      else {
	 if(CONFIGVARS.DEBUGMODE) {
	    my_printf("Loaded monster %s at coords %d,%d. (max is %d,%d)\n",
		      mondata.m.desc, mondata.x, mondata.y, 
		      level->sizex-1, level->sizey-1 );
	 }
      }

      /* initialize monster node */
      ptr=monster_initnode(level);

      if(ptr) {
	 mondata.next=ptr->next;
	 mondata.target=NULL;

//	 inv_init(&mondata.inv, mondata.equip);
	 mem_clear(&mondata.inv, sizeof(Tinventory));	 

	 /* equipped item pointers to NULL, this will be corrected
	  * later with set_equipmentptrs() */
	 for(int i=0; i<MAX_EQUIP; i++) {
	    mondata.equip[i].item=NULL;
	 }

	 /* HACK - REMOVE WHEN SKILLS ARE SAVED */
	 mondata.skills=NULL;
	 mondata.conditions=NULL;
	 mondata.path=NULL;

	 *ptr=mondata;
	       
	 /* now copy loaded data to item list */
      }
      else {
	 return NULL;
      }
   }
   else
      return NULL;

   return ptr;
}

level_type *load_tmplevel(int16s ndungeon, int16s nlevel)
{
//  _FH_ mapfile;
   level_type *newlevel;
   char filename[40];

   newlevel=new level_type;

   if(!newlevel)
      return NULL;

   if(!chdir_totempdir()) {
      msg.newmsg("Can't change to temp directory!", CHB_RED);
      return NULL;
   }

   /* clear the level structure first (Pointers to null) */
   mem_clear(newlevel, sizeof(level_type));

   /* filename to load */
   sprintf(filename, "D%02dL%02d.dat", ndungeon, nlevel);

   /* test if temp level exists */
   if(load_tmpfile(newlevel, filename, true)) {
      changedir(FILE_BACKDIR);
      return newlevel;
   }
   /* change back to game root */

   changedir(FILE_BACKDIR);
   /* else it will fail */
   freelevel(newlevel);
   delete newlevel;
   return NULL;
}

/*
*/

/* 
 * a generic routine for writing a specific header 
 * into a file
 *
 */
bool write_header(int16u type, _FH_ fh)
{
   _save_header head;
   ssize_t rr;

   /* set header info */
   head.version=CURRENT_FILEVERSION;
   head.itype = type;

   /* write the header */
   rr = write(fh, &head, sizeof(_save_header));

   if( rr != sizeof(_save_header) )
      return false;

   return true;
}

int16u read_header(_FH_ fh)
{
   _save_header head;
   ssize_t res;

   /* write the header */
   res = read(fh, &head, sizeof(_save_header));
   if(res == sizeof(_save_header) )
      return head.itype;
   else if(res == 0) /* eof */
      return 1;

   /* error */
   return 0;
}

/*
 * Save all skills in the list
 *
 */
bool save_skills(Tskillpointer sptr, _FH_ fh)
{
   ssize_t rr;

   if(fh < 0)
      return false;

   /* process all conditions in the list */
   while(sptr) {
      /* write header to indicate condition struct */
      if(!write_header(SAVE_SKLTYPE, fh))
	 return false;

      /* write condition info */
      rr = write(fh, &sptr->s, sizeof(Tskill));

      if(rr != sizeof(Tskill))
	 return false;

      sptr = sptr->next;
   }

   return true;
}

/*
 * Load one skill from a file
 *
 */
bool load_skill(Tskillpointer *sptr, _FH_ fh)
{
   ssize_t rr;
   Tskill buf;
   Tskillpointer ptr;

   if(fh < 0)
      return false;

   rr = read(fh, &buf, sizeof(Tskill));
   if(rr != sizeof(Tskill))
      return false;

   ptr = skill_addnew(sptr, buf.group, buf.type, 1);
   if(!ptr)
      return false;

   /* copy all data */
   ptr->s = buf;

   return true;
}

/*
 * Load a single condition from a file pointed by fh
 *
 */
bool load_condition(Tcondpointer *cptr, _FH_ fh)
{
   ssize_t rr;
   Tcondition buf;
   Tcondpointer ptr;

   if(fh < 0)
      return false;

   rr = read(fh, &buf, sizeof(Tcondition));
   if(rr != sizeof(Tcondition))
      return false;

   ptr = cond_add(cptr, buf.type, buf.val);
   if(!ptr)
      return false;

   ptr->cond = buf;

   return true;
}

/*
 * Save all conditions from a list pointed by 'cptr' to a file
 * pointed by 'fh'
 *
 * header (SAVE_STATTYP)
 * cond   (Tcondition)
 * header (SAVE_STATTYP)
 * cond   (Tcondition)
 * ...
 * ...
 */
bool save_conditions(Tcondpointer cptr, _FH_ fh)
{
   ssize_t rr;

   if(fh < 0)
      return false;

   /* process all conditions in the list */
   while(cptr) {
      /* write header to indicate condition struct */
      if(!write_header(SAVE_STATTYP, fh))
	 return false;

      /* write condition info */
      rr = write(fh, &cptr->cond, sizeof(Tcondition));

      if(rr != sizeof(Tcondition))
	 return false;

      cptr = cptr->next;
   }
   return true;
}

/*
 * A routine for loading a whole inventory 
 *
 * Reads recursively until INVFOOTER header is found
 *
 *
 * NOTE! Since each creature has a separate equipment storage
 *       which holds the pointers to inventory list items YOU MUST
 *       use set_equipmentptrs() to reset the pointers.
 *       It's possible to restore the pointers because each inventory
 *       node has a SLOT number larger than -1 if the item was equipped
 *       This slot number directly maps to the equipment slots.
 *
 */
bool load_inventory(Tinventory *inv, _FH_ fh, inv_info *buf)
{
   Tinvpointer invptr;
   ssize_t rr;
   int16u rh;

   if(fh<0)
      return false;

   if(!buf )
      return false;

   while(1) {
      rh = read_header(fh);

      /* do action for each header type */
      switch( rh ) {
	 case SAVE_INVHEAD:
	    /* recurse and load the subinventory */
	    if( ! load_inventory(buf->i.inv, fh, buf))
	       return false;
	    break;
	 case SAVE_ITMTYPE:
	    rr = read(fh, buf, sizeof(inv_info) );
	    if( rr == sizeof(inv_info) ) {

	       /* skip items if no inventory to load to */
	       if(!inv) continue;

	       /* make room for a new item */
	       invptr=inv_inititem(inv);

	       if(invptr) {
		  buf->next = invptr->next;

		  /* copy loaded item data */
		  *invptr = *buf;

		  if(CONFIGVARS.DEBUGMODE) {
		     my_printf("Loaded item %s.\n",
			       buf->i.name );
		  }

		  /* if the item was a container, init it */
		  invptr->i.inv = NULL;

		  if(invptr->i.type == IS_CONTAINER) {
		     container_init(&invptr->i);
		  }

		  buf->i.inv = invptr->i.inv;
	       }
	       else {
		  my_printf("Inventory iteminit failed!\n");
		  return false;
	       }
	    }
	    else
	       return false;
	    break;
	 case SAVE_INVFOOT:
	    /* update inventory information */
	    inv_calcweight(inv);
	    return true;
	    break;
	 case 0:
	    my_printf("Error while loading the header!\n");
	    return false;
	 case 1:
	    my_printf("Unexpected end of file while loading inventories.\n");
	    return false;
      }
   }

   showmore(false, false);

   return true;   
}



/*
 * A routine which will save whole inventory and its subinventories
 * into a file.
 *
 * The file structure will be like this:
 *
 * INVHEADER    ( main inventory starts )
 *   ITEMHEADER (item in main inventory)
 *   ITEMDATA
 *   ITEMHEADER
 *   ITEMDATA
 *   ...
 *   INVHEADER    ( sub inventory starts )
 *     ITEMHEADER ( item in sub inventory )
 *     ITEMDATA
 *     ...
 *     INVFOOTER  ( sub inventory ends )
 *   ITEMHEADER ( item in main inventory )
 *   ITEMDATA   
 * INVFOOTER    ( main inventory ends )
 */
bool save_inventory(Tinventory *inv, _FH_ fh)
{
   Tinvpointer itemptr;
   ssize_t rr;

   if(fh<0)
      return false;

   if(!inv)
      return true;

   if(!inv->first)
      return true;

   /* write inventory header */
   if(!write_header(SAVE_INVHEAD, fh))
      return false;

   itemptr=inv->first;

   /* save all items in the inventory */
   while(itemptr)	{
      /* save header for current item */
      if(!write_header(SAVE_ITMTYPE, fh))
	 return false;

      /* save the item data */
      rr = write(fh, itemptr, sizeof(inv_info));

      if( rr != sizeof(inv_info) )
	 return false;

      if(itemptr->i.type == IS_CONTAINER) {
	 if( !save_inventory(itemptr->i.inv, fh))
	    return false;
      }

      itemptr=itemptr->next;
   }

   /* write footer for the inventory */
   if(!write_header(SAVE_INVFOOT, fh))
      return false;

   return true;
}

/*
 * Save all quests stored in a list (player quest list)
 *
 */
bool save_questlist(Tquestpointer qptr, _FH_ fh)
{
   ssize_t rr;

   if(fh < 0)
      return false;

   /* process all conditions in the list */
   while(qptr) {
      /* write header to indicate condition struct */
      if(!write_header(SAVE_INDVQST, fh))
	 return false;

      /* write condition info */
      rr = write(fh, &qptr->quest, sizeof(Tquest));

      if(rr != sizeof(Tquest))
	 return false;

      qptr = qptr->next;
   }
   return true;
}

/*
 * Save entire path list
 */
bool save_pathlist(Tpathlist plst, _FH_ fh)
{
   ssize_t rr;
   

   if(fh < 0)
      return false;

   /* process all conditions in the list */
   while(plst) {
      /* write header to indicate condition struct */
      if(!write_header(SAVE_PATHCRD, fh))
	 return false;

      /* write condition info */
      rr = write(fh, &plst->c, sizeof(Tcoordinate));

      if(rr != sizeof(Tcoordinate))
	 return false;

      plst = plst->next;
   }
   return true;
}

/*
 * Load a single path coordinate
 *
 */
bool load_pathnode(Tpathlist *plst, _FH_ fh)
{
   ssize_t rr;
   Tcoordinate buf;

   if(fh < 0)
      return false;

   /* read one quest structure */
   rr = read(fh, &buf, sizeof(Tcoordinate));
   if(rr != sizeof(Tcoordinate))
      return false;

   my_printf("Path c%d,%d\n", buf.x, buf.y);

   /* add the quest back to the list */
   if(!path_insertend(plst, &buf))
      return false;

   return true;   
}

bool load_quest(Tquestpointer *qptr, _FH_ fh)
{
   ssize_t rr;
   Tquest buf;
   Tquestpointer ptr;

   if(fh < 0)
      return false;

   /* read one quest structure */
   rr = read(fh, &buf, sizeof(Tquest));
   if(rr != sizeof(Tquest))
      return false;

   /* add the quest back to the list */
   ptr = quest_createnode(qptr);
   if(!ptr)
      return false;

   ptr->quest = buf;

   return true;   
   
}

bool save_globals(_FH_ fh)
{
   ssize_t rr;

   if(fh < 0)
      return false;

   /* write header to indicate condition struct */
   if(!write_header(SAVE_GLOBALD, fh))
      return false;

   /* write condition info */
   rr = write(fh, &GD, sizeof(Tgamedata));

   if(rr != sizeof(Tgamedata))
      return false;

   return true;
}

bool load_globals(_FH_ fh)
{
   ssize_t rr;

   if(fh < 0)
      return false;

   rr = read(fh, &GD, sizeof(Tgamedata));
   if(rr != sizeof(Tgamedata))
      return false;

   return true;
}

/*
 * write player information 
 *
 *
 */
bool save_player(_FH_ fh)
{
   ssize_t rr;

   playerinfo tmpplr;

   tmpplr = player;

   if(fh<0)
      return false;

   if( !write_header(SAVE_PLRDATA, fh) ) 
      return false;

   /* save player structure */
   rr = write(fh, &tmpplr, sizeof(playerinfo));
   if(rr != sizeof(playerinfo))
      return false;

   /* save player skills */
   if(!save_skills(player.skills, fh))
      return false;

   /* save player conditions */
   if(!save_conditions(player.conditions, fh))
      return false;

   /* save quest information */
   if(!save_questlist(player.quests, fh))
      return false;

   /* save player inventory */
   if(!save_inventory(&player.inv, fh))
      return false;

   /* save global data */
   if(!save_globals(fh))
      return false;

   /* save weather data */
   if(!weather.save(fh))
      return false;

   /* save time */
   if(!worldtime.save(fh, SAVE_GAMETIME))
      return false;

   if(!passedtime.save(fh, SAVE_PASSTIME))
      return false;

   my_printf(".");
   my_refresh();

   return true;
}

/*
 * This routine loads player structure stored in a file
 *
 */
bool set_dungeonptr(struct _dungeonleveldef **ptr, playerinfo *plr)
{
   _dungeonleveldef *p;

   if(plr->dungeon > num_dungeons)
      return false;

   p = dungeonlist[plr->dungeon].levels;

   while(1) {
      if( p->index == plr->dungeonlev) {
	 *ptr = p;
	 return true;
      }
      if(!p->name) {
	 return false;
      }

      p++;
   }
//   p = &p[plr->dungeonlev];

//   *ptr = p;

   return true;
}

/*
 * Correct equipment pointers ( equip->item ) to the actual
 * inventory items which were previously loaded from the disk
 *
 */
bool set_equipmentptrs(Tinventory *inv, _Tequipslot *eqptr)
{
   Tinvpointer iptr;

   iptr = inv->first;

   /* go through the entire inventory */
   while(iptr) {
      if(eqptr) {
	 /* if item was equipped, ie. slotnum is larger than -1 */
	 if(iptr->slot >= 0 && iptr->slot < MAX_EQUIP) {
	    eqptr[iptr->slot].item = iptr;
	 }
	 else
	    iptr->slot = -1;
      }
      else
	 iptr->slot = -1;

      iptr=iptr->next;
   }

   return true;
}

bool load_player(_FH_ fh)
{
   ssize_t rr;

//   playerinfo plrtmp;

   if(fh < 0)
      return false;

   rr = read(fh, &player, sizeof(playerinfo));
   if(rr != sizeof(playerinfo))
      return false;

   /* initialize all pointer related data */
   player.levptr = NULL;
   path_init(&player.path);
   skill_init(&player.skills);
   cond_init(&player.conditions);
   quest_init(&player.quests);
   inv_init(&player.inv, player.equip);

   /* now find a correct dungeonlevel pointer */
   if( !set_dungeonptr(&player.levptr, &player) ) {
      my_printf("Illegal dungeon number %d\n", player.dungeon);
      return false;
   }

   return true;
}

void make_savedirname(const char *name, char *buf, int len)
{
   int i=0;;

   while(name[i]!=0 && i<len) {
      if( isalnum( name[i] ))
	 *buf = name[i];
      else
	 *buf = '_';

      i++;
      buf++;
   }
   *buf=0;
}

/*
 * Save everything
 *
 */
bool save(void)
{
   _FH_ fh;
   sint errors=0;

   my_printf("Saving: ");

//   char savefiledir[NAMEMAX+1];

   if(!chdir_totempdir()) {
      my_printf("Can't change to temp directory!");
      return false;
   }

   /* save current level to the temporary directory */
   if( !save_tmpfile( c_level, player.dungeon, player.dungeonlev) ) {
      my_printf("Unable to save the current level!\n");
      errors++;
   }

   /* 
    * save player information with items and all
    */
   fh=open(FILE_PLAYERINFO, O_WRONLY|O_CREAT|O_BINARY|O_TRUNC, 
	   UNIX_FILEPERMBITS);

   if(fh >= 0) {
      /* save player data */
      if(!save_player(fh)) {
	 errors++;
	 my_printf("Unable to save player and global game data!\n");
      }

      close(fh);
   }
   else 
      my_printf("Unable to open savefile for writing!\n");
   
   if(errors > 0) {
      my_printf("There were %d errors.\n", errors);
      return false;
   }

   if( !save_concatsavefile() ) {
      my_printf("Unable to create the save file.\n");
      return false;
   }

   my_printf(" Ok!\n");
   return true;
}

/*
 * Load everything
 *
 */
bool load(bool autocheck)
{
   bool nosaves=false;
   int sel;
   int errors=0;

   char savefiledir[NAMEMAX+1];   

   make_savedirname(player.name, savefiledir, 8);
   
   strcat(savefiledir, FILE_SAVEPAT+1);

   if(!chdir_totempdir())
      nosaves=true;

   if(!changedir(FILE_SAVEDIR) )
      nosaves=true;

   if(!testfile(savefiledir))
      nosaves=true;

   if(nosaves ) {
      if(autocheck)
	 return false;
      else {
	 my_printf("No save file for you, %s.\n", player.name);
	 return false;
      }
   }

   if(autocheck) {
      my_printf("Hey, you've an unfinished game! Continue it (Y/n)?");

      if( confirm_yn(NULL, true, false) ) {
	 return true;
      }
      
      my_printf("\nStarting a new game.\n");
      return false;
   }

   my_clrscr();
   my_setcolor(C_WHITE);
   my_printf("\nRestoring: ");
   if( !save_separatesavefile() ) {
	my_printf("Unable to pre-process the savefile.\n");
	errors++;
	return false;
   }

   freeall_player();
   freelevel(c_level);

   /* load player and game info */
//   my_printf("Restoring game data...\n");
   if(!load_tmpfile(c_level, FILE_PLAYERINFO, false)) {
	my_printf("Unable to restore game and player data.\n");
	errors++;
      return false;
   }

   sprintf(tempstr, "D%02dL%02d.dat", player.dungeon, player.dungeonlev);
		 
//   my_printf("Restoring level...\n");
   /* load player and game info */
   if(!load_tmpfile(c_level, tempstr, true)) {
	my_printf("Unable to restore the current level.\n");
      return false;
   }

   /*
    * Now game is restored,
    * lets delete the savefile
    */
   if(changedir(FILE_SAVEDIR) ) {
      deletefile(savefiledir);

      chdir_totempdir();
   }

   my_printf(" Ok!\n\n");

   return true;
}

#define SAVEFILENAMELEN 20
struct _savefileinfo
{
     char filename[ SAVEFILENAMELEN + 1];
     int32u len;
};

bool save_concatsavefile(void)
{
   _FH_ fh, readf;
   int8u tbyte;
   int count=0, errors=0;
   ssize_t wres;
   char savefiledir[NAMEMAX+1];
   DIR *dp;
   struct dirent *ep;
   struct _savefileinfo si;
   long int flen;

   if(!chdir_totempdir()) {
      my_printf("Couldn't access the temporary directory!");
      return false;
   }
   
   if(!changedir(FILE_SAVEDIR)) {
      if( !makedir(FILE_SAVEDIR)) { 
	 my_printf("Couldn't create the save directory \"%s\".\n",
		   FILE_SAVEDIR);
	 return false;
      }
      else
	 my_printf("Save directory created.\n");

      if(!changedir(FILE_SAVEDIR)) {
	 my_printf("Couldn't access the save directory!\n");
	 return false;
      }
   }

   /*
    * now concat all files from temp dir (..) to the current dir (save)
    */

   /* open the temp dir */
   dp = opendir("..");

   if(dp != NULL) {
      
//      strcpy(savefiledir, FILE_PACKTMP);
     
      fh=open(FILE_PACKTMP, O_WRONLY|O_CREAT|O_BINARY|O_TRUNC, 
	      UNIX_FILEPERMBITS);
      if(fh) {
	 /* match each data file (*.dat) and concat to the savefile */
	 while( (ep = readdir(dp)) ) {

	    if(!fnmatch(FILE_DATAPAT, ep->d_name, 0)) {
	       count++;

	       strcpy(si.filename, ep->d_name);

	       sprintf(tempstr, "..%s%s", FILE_DIRSLASH, ep->d_name);

	       flen = filesize(tempstr);

	       if( flen <= 0) {
		  my_printf("File %s is 0 bytes long or corrupted.\n", 
			    ep->d_name);
		  continue;
//		  errors++;
//		  break;
	       }

	       si.len = (int32u)flen;
	       wres = write(fh, &si, sizeof(_savefileinfo));
	       if(wres != sizeof(_savefileinfo)) {
		  my_printf("Unable to write %d bytes to %s\n", 
			    sizeof(_savefileinfo), savefiledir);
		  errors++;
		  break;
	       }

	       readf=open(tempstr, O_RDONLY|O_BINARY);
	       if(readf<0) {		  
		  my_printf("%s: Unable to process\n", count, ep->d_name);
		  errors++;
		  break;
	       }
	       while(1) {
		  wres = read(readf, &tbyte, sizeof(int8u));

		  if(wres==0)
		     break;

		  si.len--;

		  if(wres!=1) {
		     errors++;
		     break;
		  }
		  wres = write(fh, &tbyte, sizeof(int8u));
		  if(wres!=1) {
		     errors++;
		     break;
		  }
	       }	       
	       close(readf);

	       if(si.len != 0) {
		  my_printf("Error: Mismatch in read and written lengths!\n");
		  errors++;
		  break;
	       }
	       
	       if(CONFIGVARS.DEBUGMODE) {
		  my_printf("Processed file %s -> %s\n", 
			    tempstr, savefiledir);
	       }
	       else {
		  my_printf(".");
		  my_refresh();
	       }

	    }
	 }
	 close(fh);

	 if(!count || errors>0) {
	    deletefile(FILE_PACKTMP);
	 }
      }
      closedir(dp);
   }
   else {
      return false;
   }

   if(!count || errors>0)
      return false;

   /* 
    * Now the save file is in one piece stored in "FILE_PACKTMP"
    * lets compress it to save space
    *
    */

   make_savedirname(player.name, savefiledir, 8);
   strcat(savefiledir, FILE_SAVEPAT+1);

   my_printf(".");
   my_refresh();
   if( compress(FILE_PACKTMP, savefiledir) ) {
      my_printf(".");
      my_refresh();      
   }
   else {
      my_printf("Compression failed.\n");
      return false;
   }

   deletefile(FILE_PACKTMP);
   chdir_totempdir();

   return true;
}

bool save_separatesavefile(void)
{
   char savefiledir[NAMEMAX+1];
   ssize_t wres;
   int errors=0;
   struct _savefileinfo si;
   int8u tbyte;
   _FH_ fh, outf;

   /* since we are creating the temporary files, 
    * lets remove the old ones first (if any)
    */
   clean_tmpfiles();

   if(!chdir_totempdir()) {
      my_printf("Couldn't access the temp directory.\n");
      return false;
   }
   
   if(!changedir(FILE_SAVEDIR)) {
      my_printf("Can't access the save directory.\n");
      return false;
   }

   make_savedirname(player.name, savefiledir, 8);
   strcat(savefiledir, FILE_SAVEPAT+1);

   sprintf(tempstr, "..%s%s", FILE_DIRSLASH, FILE_PACKTMP);

   if(!decompress(savefiledir, tempstr)) {
      my_printf("Unable to decompress the save file %s.\n", savefiledir);
      return false;
   }

   /* 
    * now decompressed savefile is in tempdir file FILE_PACKTMP
    * lets separate all the files from it to the temp directory
    * FILE_TEMPDIR 
    */

   chdir_totempdir();

   fh=open(FILE_PACKTMP, O_RDONLY|O_BINARY);
   if(!fh) {
      return false;
   }

   while(1) {
      /* read header */
      wres = read(fh, &si, sizeof(_savefileinfo));
      if(wres==0) { /* EOF */
	 break;
      }
      else if(wres != sizeof(_savefileinfo)) {
	 my_printf("Unable to read %d bytes from %s\n", 
		   sizeof(_savefileinfo), savefiledir);
	 errors++;
	 break;
      }
   
      if(strlen(si.filename) == 0 || si.len == 0) {
	 my_printf("Your savefile %s is corrupted!\n", savefiledir);
	 errors++;
	 break;
      }

      /* open output file and read into it */
      outf = open( si.filename, O_WRONLY|O_CREAT|O_BINARY|O_TRUNC, 
		   UNIX_FILEPERMBITS);
      if(!outf) {
	 my_printf("Unable to open requested tempfile %s.\n", si.filename);
	 errors++;
	 break;
      }

      while(si.len) {
	 wres=read(fh, &tbyte, sizeof(int8u));
	 if(wres != sizeof(int8u)) {
	    errors++;
	    break;
	 }

	 wres=write(outf, &tbyte, sizeof(int8u));
	 if(wres != sizeof(int8u)) {
	    errors++;
	    break;
	 }

	 si.len--;
      }
      close(outf);
      my_printf(".");
      my_refresh();
   }

   deletefile(FILE_PACKTMP);
   close(fh);

   if(errors)
      return false;

   return true;
}


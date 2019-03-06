/*
 * header for file.cc 
 *
 * Legend of Saladir (C)1997/1998 by Erno Tuomainen
 *
 */

#ifndef _FILE_H_DEFINED
#define _FILE_H_DEFINED

#include "saladir.h"
#include "output.h"

/* system file stuff */
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>

#ifdef _TS_LINUX_
#include <stdio.h>
#include <pwd.h>
#endif

#include "types.h"
#include "inventor.h"
#include "options.h"
#include "compress.h"
//#include "score.h"


#define HEADER_NAMELEN 10
#define CURRENT_FILEVERSION 1

#define SAVE_INVHEAD 0x8000 /* inventory header */
#define SAVE_INVFOOT 0x8001 /* inventory footer */
#define SAVE_ITMTYPE 0x8002 /* item structure */
#define SAVE_LEVDATA 0x8003 /* level structures */
#define SAVE_NPCTYPE 0x8004 /* monster structure */
#define SAVE_ROOMDAT 0x8005 /* room structure */
#define SAVE_PLRDATA 0x8006 /* player structures */
#define SAVE_SCOREDA 0x8007 /* highscore data */
#define SAVE_STATTYP 0x8008 /* status/cond structure */
#define SAVE_INDVQST 0x8009 /* player quest structure */
#define SAVE_SKLTYPE 0x800a /* skill structures */
#define SAVE_SYSTQST 0x800b /* system quest data */
#define SAVE_GLOBALD 0x800c /* global data (GD struct) */
#define SAVE_PATHCRD 0x800d /* pathlist structure (coordinate) */
#define SAVE_GAMETIME 0x800e /* time structure for class Gametime */
#define SAVE_PASSTIME 0x800f /* time structure for class Gametime */
#define SAVE_WTHRTYPE 0x8010 /* weather structure for class Weather */

/* permission bits to use in UNIX/LINUX systems, when creating files */
#define UNIX_DIRPERMBITS (S_IRWXU | S_IRWXG)
#define UNIX_FILEPERMBITS (S_IRUSR | S_IWUSR)
#define UNIX_USERNAMELEN 10

#define DATAFILES_INETC  0x80
#define DATAFILES_INHOME 0x81


/*
 * Compatibility for *NIXes
 *
 */
#ifdef _TS_LINUX_
#define O_BINARY 0
#else
#define PATH_MAX 256
#endif

//typedef sint _FH_;

/* save "item" header */
typedef struct	{
     int16u itype;
     int16u version;
} _save_header;

typedef struct {
     int16s targetx;
     int16s targety;
     int32u aux;
} _save_monheader;

typedef struct
{
     int16s sizex;
     int16s sizey;
     int16u danglev;
     int16u roomcount;
} _save_levheader;

typedef struct
{
     int16s sizex;
     int16s sizey;
     int16u danglev;
     int16u roomcount;
} OLD_save_levheader;

#ifdef _TS_LINUX_
extern char unix_username[UNIX_USERNAMELEN];
#endif

/* public functions */
bool edit_savemap(level_type *level, char *filename);
bool edit_loadmap(level_type *level, char *filename);
bool init_directories(void);
bool clean_tempdir(void);
bool clean_tmpfiles(void);
bool save_tmpfile(level_type *level, int16s ndungeon, int16s nlevel);
bool load_tmpfile(level_type *level, const char *filename, bool);
level_type *load_tmplevel(int16s dungeon, int16s level);
bool chdir_totempdir(void);
bool chdir_todatadir(void);
bool chdir_tohelpdir(void);
bool load_invitem(level_type *, _FH_, Tinventory *);
bool save_invitem(Tinvpointer, _FH_, Tinventory *);
bool load_levelrooms(level_type *, _FH_);
bool save_levelrooms(level_type *, _FH_);
bool load_levelitem(level_type *, _FH_);
bool load_leveldata(level_type *level, _FH_ fh);
bool save_leveldata(level_type *level, _FH_ fh);
bool save_levelitems(level_type *level, _FH_ fh);
_monsterlist *load_monster(level_type *level, _FH_ fh);
bool save_monsters(level_type *level, _FH_ fh);

bool write_header(int16u, _FH_);
int16u read_header(_FH_);

bool save(void);
bool load(bool);

bool clean_tempdir(void);
bool makedir(const char	*dirname);
bool changedir(const char *dirname);
bool testfile(const char *filename);
bool deletefile(const char *filename);
bool createfile(const char *filename);

bool unset_myuid(void);
bool set_myuid(void);
void init_uid(void);
bool getfilestat(char *filename, struct stat *filestat);
bool get_userhomedir(char *homedir, sint maxlen);


extern const char FILE_ERRORLOG[];
extern const char FILE_TMPTEXT[];

#endif

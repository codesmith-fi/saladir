/*

 level converter

*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "../types.h"

#define HEADER_NAMELEN 10

#ifdef linux
#define O_BINARY 0
#endif

typedef struct {
     int16u itype;
     int16u version;
} _newsave_header;

typedef struct {
      char type[HEADER_NAMELEN+1];
      int16u	version;
} _oldsave_header;

typedef struct
{
      _oldsave_header hd;
      int16s sizex;
      int16s sizey;
      int16u danglev;
      int16u roomcount;
} old_header;

typedef struct
{
      _newsave_header hd;
      int16s sizex;
      int16s sizey;
      int16u danglev;
      int16u roomcount;
} new_header;

typedef struct
{
      int16u flags; /* terrain flags */
      int16u type;  /* room, corridor, shop etc.. */
      int8u sval;   /* trap strenght, secret door level etc...*/
                    /* for staircases, it's the staircase number */
      int8u doorfl; /* door flags, see above */
      int8u trap;   /* trap number */
      int8u trap_st; /* trap strength */
} new_cave_type;

typedef struct
{
      int16u flags; /* terrain flags */
      int16u type;  /* room, corridor, shop etc.. */
      int8u sval;   /* trap strenght, secret door level etc...*/
                    /* for staircases, it's the staircase number */
      int8u doorfl; /* door flags, see above */
      int8u trap;   /* trap number */
      int8u trap_st; /* trap strength */
} old_cave_type;


const char SAVE_LEVDATA[] = "LEVDATA";

#define SAVE_NEWLEVDATA 0x8003

int main(int argc, char *argv[])
{
   int infile, outfile, res;

   int16u i, j, k;

   old_cave_type oldgrid;
   new_cave_type newgrid;
   old_header ohead;
   new_header nhead;

   if(argc<3) {
      printf("Usage: convert <infile> <outfile>\n\n");
      return 10;
   }

   infile=open(argv[1], O_RDONLY|O_BINARY);

   if(infile<0) {
      perror(argv[1]);
      exit(10);
   }

   outfile=open(argv[2], O_TRUNC|O_WRONLY|O_CREAT|O_BINARY, S_IRUSR|S_IWUSR);

   if(outfile<0) {
      perror(argv[2]);
      exit(10);
   }

   printf("Converting old map %s to %s.\n", argv[1], argv[2]);

   printf("Reading header...");
   res=read(infile, &ohead, sizeof(old_header));
   if(res==sizeof(old_header))
      printf("Ok!\n");
   else printf("Failed!\n");

   nhead.hd.itype = SAVE_NEWLEVDATA;
   nhead.hd.version=1;

   nhead.sizex=ohead.sizex;
   nhead.sizey=ohead.sizey;
   nhead.danglev=ohead.danglev;
   nhead.roomcount=0;

   printf("Level size X=%d, Y=%d, dangerlevel=%d\n", 
	  ohead.sizex, ohead.sizey, ohead.danglev);
   printf("Writing new header...");

   res=write(outfile, &nhead, sizeof(new_header));
   if(res==sizeof(new_header))
      printf("Ok!\n");
   else printf("Failed!\n");

   printf("Now converting level grids...\n");
   for(i=0; i<nhead.sizey; i++) {
      for(j=0; j<nhead.sizex; j++) {
	 res=read(infile, &oldgrid, sizeof(old_cave_type));
         if(res!=sizeof(old_cave_type)) {
	    printf("Error in read!\n");
	    close(infile);
	    close(outfile);
            exit(10);
         }
         newgrid.flags=oldgrid.flags;
         newgrid.type=oldgrid.type;
         newgrid.sval=oldgrid.sval;
         newgrid.doorfl=oldgrid.doorfl;
	 newgrid.trap=oldgrid.trap;
	 newgrid.trap_st=oldgrid.trap_st;
	 res=write(outfile, &newgrid, sizeof(new_cave_type));
         if(res!=sizeof(new_cave_type)) {
	    printf("Error in write!\n");
	    close(infile);
	    close(outfile);
            exit(10);
         }
      }
   }
   printf("\nYeah, all OK!\n");
   close(infile);
   close(outfile);

   return 0;
}


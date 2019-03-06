/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char versionfile[] = "saladir_version_file";
const char headerfile[] = "_version.h";

const char headerdata[] = 
"/*\n"
" * Automatically generated version header\n"
" * for the legend of saladir\n"
" * (C) 1997/1998 by Erno Tuomainen\n"
" *\n"
" */\n\n";

typedef struct _version
{
      int v_major;
      int v_minor;
      int v_build;
      time_t v_date;
} TVersion;

int main(int argc, char **argv)
{
   TVersion vdat;
   FILE *file;
   tm *mytime;

   char *mytime_asc;

   vdat.v_major = 0;
   vdat.v_minor = 0;
   vdat.v_build = 1;
   vdat.v_date = time(NULL);

   file = fopen(versionfile, "r+b");
   
   if(!file) {
      file = fopen(versionfile, "wb");
      if(!file) {
	 perror(versionfile);
	 return 0;
      }

      if(fwrite(&vdat, sizeof(TVersion), 1, file)!=1) {
	 perror(versionfile);
	 fclose(file);
	 return 0;
      }
      printf("Version file created!\n");
      fclose(file);

      /* open it again*/
      file = fopen(versionfile, "r+b");

      if(!file) {
	 perror(versionfile);
	 return 0;
      }
   }

   if( fread(&vdat, sizeof(TVersion), 1, file) != 1) {
      perror(versionfile);
      fclose(file);
      return 0;
   }

   mytime = localtime(&vdat.v_date);
   printf("Previous version %d.%d.%d date %s", 
	  vdat.v_major, vdat.v_minor, vdat.v_build, asctime(mytime));

   vdat.v_date = time(NULL);
   mytime = localtime(&vdat.v_date);

   mytime_asc = (char *)malloc(strlen(asctime(mytime))+1);
   if(!mytime_asc) {
      fprintf(stderr, "Error: no memory for string.\n");
      fclose(file);
      return 0;
   }
      
   strcpy(mytime_asc, asctime(mytime));

   int i=0;
   while(mytime_asc[i] != 0) {
      if(mytime_asc[i]=='\n' || mytime_asc[i]=='\r')
	 mytime_asc[i]=0;
      i++;
   }

  
   if(argc < 2)
      vdat.v_build++;
   else if( argv[1][0]=='b')
      vdat.v_build++;
   else if( argv[1][0]=='v') {
      vdat.v_major++;
      vdat.v_minor = 0;
      vdat.v_build = 1;
   }
   else if( argv[1][0]=='m') {
      vdat.v_minor++;
      vdat.v_build = 1;
   }

   fseek(file, 0, SEEK_SET);

   if(fwrite(&vdat, sizeof(TVersion), 1, file)!=1) {
      perror(versionfile);
   }
   else
      printf("New version file written correctly!\n");

   if(!mytime) {
      fprintf(stderr, "Error: Cannot convert time.\n");
   }
   else
      printf("New version %d.%d.%d date %s.\n", 
	     vdat.v_major, vdat.v_minor, vdat.v_build, mytime_asc);

   fclose(file);

   file = fopen(headerfile, "w");
   if(!file) {
      free(mytime_asc);
      perror(headerfile);
      return 0;
   }

   fprintf(file, headerdata);
   fprintf(file, "#define PROC_VERSION \"V%d.%d.%d\"\n", 
	   vdat.v_major, vdat.v_minor, vdat.v_build);

   fprintf(file, "#define PROC_DATE \"(%s)\"\n", 
	   mytime_asc);

   fprintf(file, "#ifdef linux\n");
   fprintf(file, "#define PROC_PLATFORM \"-Linux\"\n");
   fprintf(file, "#else\n");   
   fprintf(file, "#define PROC_PLATFORM \"-DOS\"\n");
   fprintf(file, "#endif\n");

   free(mytime_asc);
   fclose(file);
   return 0;
}

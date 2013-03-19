/* memory.h
**
**
*/
#include "memory.h"

/*
** Copy 'num' bytes of memory from 'src' to 'destination'
*/
void mem_copy(void *dest, const void *src, int16u num)
{
   memcpy(dest, src, num);
}

/*
** Compare 'num' bytes of memory at blocks 's1' and 's2'
** return 0 if equal
**        + if s1>s2
**        - if s1<s2
*/
int16s mem_comp(const void *s1, const void *s2, size_t num)
{
   return memcmp(s1, s2, num);
}

/*
** Clear a region of memory
*/

void mem_clear(void *buffer, size_t num)
{
   memset(buffer, 0, num);
}


int16s my_stricmp(const char *str1, const char *str2)
{
#ifdef LINUX
   return strcasecmp(str1, str2);
#endif

#ifndef LINUX
   return stricmp(str1, str2);
#endif
}

int32u my_strcpy(char *dest, const char *src, int32u maxlen)
{
   register int32u ccount=0;
   char error[]="Error! No source string in my_strcpy()!";

   if(dest==NULL || maxlen==0)
      return 0;

   /* if no source then copy error string! */
   if(src==NULL)
      src=error;

   for(ccount=0; ccount<maxlen-1; ccount++) {
      if(src[ccount]==0)
	 break;

      dest[ccount]=src[ccount];

   }
   dest[ccount]=0;

   return ccount;
}

int32u my_strlen(const char *s)
{
   register int32u c = 0;

   if(!s)
      return 0;

   while(s[c]!=0) c++;

   return c;
}

int32u my_strcat(char *s1, const char *s2, int32u len)
{
   register int32u c;

   if(!s1)
      return 0;

   if(!s2)
      return my_strlen(s1);

   c = my_strlen(s1);

//   if( (c+my_strlen(s2)) >= len)
//      return c;

   my_strcpy((s1+c), s2, len-c );

   return my_strlen(s1);
}

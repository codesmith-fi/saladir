/* memory.h */

#ifndef _MEMORY_H_DEFINED
#define _MEMORY_H_DEFINED

#include "saladir.h"
#include "types.h"

/* copy a block of memory */
void mem_copy(void *, const void *, int16u);

/* compare a block of memory */
int16s mem_comp(const void *, const void *, size_t);

/* clear a block of memory */
void mem_clear(void *, size_t);

/* compare incasesensitively two strings */
int16s my_stricmp(const char *, const char *);

/* return lenght of a string given in s */
int32u my_strlen(const char *s);

/* copy a string into another, with length check */
int32u my_strcpy(char *dest, const char *src, int32u maxlen);

/* concatenate two strings, s1 and s1, result in s1, max len of s1 in len */
int32u my_strcat(char *s1, const char *s2, int32u len);


#endif /* memory.h defined */

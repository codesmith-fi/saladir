/*
 * compress.h for the Legend of Saladir
 *
 * 
 */
#ifndef _COMPRESS_H_DEFINED
#define _COMPRESS_H_DEFINED


typedef struct
{
      char id[4];
      int32u size;
      int8u ver;
} Tlzwheader;

#define DECODE_BUFSIZE 4096
#define CODEBITS 12
#define DICTSIZE 5021
#define MAXVALUE (1 << CODEBITS) - 1
#define MAXCODE ( MAXVALUE - 1 )
#define HASHSHIFT (CODEBITS - 8)
#define CODE_NOSTRING 0xffff

/* prototypes */
bool decompress(const char *inputfile, const char *outputfile);
bool compress(const char *inputfile, const char *outputfile);


#endif

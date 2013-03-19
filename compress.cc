
/*
 * LZW file compressor adapted from the article on Dr.Dobbs Journal
 * (C) Mark Nelson
 * Modified by Erno Tuomainen
 *
 * Legend of Saladir (C) 1997/1998
 */

#include "saladir.h"

#include "output.h"
#include "file.h"
#include "process.h"

#include "compress.h"

/*
 * Global variables
 *
 */ 
FILE *istream=NULL;
FILE *ostream=NULL;

/* dictionary */
int16u *dict_code=NULL;   /* holds the string code index */
int16u *dict_prefix=NULL; /* holds the prefix string code */
int8u *dict_append=NULL; /* holds the character to append into prefix string*/
int8u *decode_stack=NULL;

int32u bytes_in, bytes_out;

static int32u bit_buffer=0L;
static int bit_count=0;

void compress_freeall(void)
{
   if(dict_code)
      free(dict_code);
   if(dict_prefix)
      free(dict_prefix);
   if(dict_append)
      free(dict_append);
   if(decode_stack)
      free(decode_stack);

   if(istream)
      fclose(istream);
   if(ostream)
      fclose(ostream);

   ostream=NULL;
   istream=NULL;
   decode_stack=NULL;
   dict_append=NULL;
   dict_prefix=NULL;
   dict_code=NULL;

   bit_buffer=0L;
   bit_count=0;
}


void compress_error(char *str)
{
   compress_freeall();

   ERROROUT(str);
}

int16u input_code(FILE *input)
{
   int16u ret_value;

   while(bit_count <= 24) {
      bit_buffer |= (int32u) getc(input) << (24 - bit_count);
      bit_count+=8;
   }
   ret_value=bit_buffer >> (32 - CODEBITS);
   bit_buffer <<= CODEBITS;
   bit_count -= CODEBITS;

   return ret_value;
}

void output_code(FILE *output, int16u code)
{

   bit_buffer|=(int32u)code << (32 - CODEBITS - bit_count);
   bit_count+=CODEBITS;
   while(bit_count >= 8) {
      putc(bit_buffer >> 24, output);

      bytes_out++;

      bit_buffer <<= 8;
      bit_count -= 8;
   }

}

int16u find_match(int16u prefix, int16u append)
{
   long index;
   long offset;

   index = (append << HASHSHIFT) ^ prefix;
   if(index==0) 
      offset=1;
   else 
      offset = DICTSIZE - index;

   while(1) {     
      if(dict_code[index]==CODE_NOSTRING)
	 return index;

      if(dict_append[index]==append &&
	 dict_prefix[index]==prefix)
	 return index;

      index-=offset;      

      if(index < 0)
	 index+=DICTSIZE;
   }
}

int8u *decode_string(int8u *buffer, int16u code)
{
   int16u i=0;

   while(code > 255) {
      *buffer++ = dict_append[code];
      code = dict_prefix[code];
      if(i++ > DECODE_BUFSIZE) {
	 compress_error("Fatal error during expansion!");
      }
      
   }
   *buffer=code;

   return buffer;
}

bool compress(const char *inputfile, const char *outputfile)
{
   int16u next_code;
   int8u character;
   int16u string_code;
   int16u index;
   int16u i;

   bytes_in=bytes_out=0;

   /* allocate dictionary */
   dict_code=(int16u *)malloc(DICTSIZE * sizeof(int16u));
   dict_prefix=(int16u *)malloc(DICTSIZE * sizeof(int16u));
   dict_append=(int8u *)malloc(DICTSIZE * sizeof(int8u));

   if( !dict_code || !dict_prefix || !dict_append) {
      compress_freeall();
      return false;
   }

   /* open files */
   istream=fopen(inputfile, "rb");
   ostream=fopen(outputfile, "wb");

   if(!istream || !ostream) {
      compress_freeall();
      return false;
   }

   next_code=256;
   for(i=0; i<DICTSIZE; i++)
      dict_code[i]=CODE_NOSTRING;
   
   i=0;
//   printf("Compressing...\n");
   string_code=getc(istream);
   bytes_in++;

//   while( (character=getc(istream))!=(unsigned)EOF) {
   while(1) {
      if(fread(&character, sizeof(int8u), 1, istream)!=1)
	 break;

      bytes_in++;

      index=find_match(string_code, character);

      if(dict_code[index] != CODE_NOSTRING)
	 string_code=dict_code[index];
      else {
	 if(next_code <= MAXCODE) {
	    dict_code[index]=next_code++;
	    dict_prefix[index]=string_code;
	    dict_append[index]=character;
	 }
	 output_code(ostream, string_code);
	 string_code=character;
      }
   }

   output_code(ostream, string_code);
   output_code(ostream, MAXVALUE);
   output_code(ostream, 0);

   /* free resources */
   compress_freeall();
   return true;
}

bool decompress(const char *inputfile, const char *outputfile)
{
   int16u next_code;
   int16u new_code, old_code;

   int character, counter;

   int8u *string;

   dict_prefix=(int16u *)malloc(DICTSIZE * sizeof(int16u));
   dict_append=(int8u *)malloc(DICTSIZE * sizeof(int8u));

   /* allocate decode stack */
   decode_stack=(int8u *)malloc(DECODE_BUFSIZE * sizeof(int8u));

   if( !decode_stack || !dict_prefix || !dict_append) {
      compress_freeall();
      return false;
   }

   /* open files */
   istream=fopen(inputfile, "rb");
   ostream=fopen(outputfile, "wb");

   if(!istream || !ostream) {
      compress_freeall();
      return false;
   }
   
   next_code=256;
   counter=0;

   old_code=input_code(istream);
   character=old_code;
   putc(old_code, ostream);

   while( (new_code=input_code(istream)) != (MAXVALUE)) {
/*
      if(++counter==1000) {
	 counter=0;
      }
*/
      if(new_code>=next_code) {
	 *decode_stack=character;
	 string=decode_string(decode_stack+1, old_code);
      }
      else
	 string=decode_string(decode_stack, new_code);

      character=*string;
      while(string>= decode_stack)
	 putc(*string--, ostream);

      if(next_code <= MAXCODE) {
	 dict_prefix[next_code]=old_code;
	 dict_append[next_code]=character;
	 next_code++;
      }
      old_code=new_code;
   }

   compress_freeall();
   return true;

}

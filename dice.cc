/************/
/* dice.cpp */
/************/

#include "dice.h"

/****************************************************
 * A random number generator by William Tanksley    *
 ****************************************************/

/* State variables.  Feel free to make these local or global. */
static unsigned char s[256];
static int32u rseed;
static unsigned char i, j;

unsigned char RC4Random( void )
{
   unsigned char ti, tj, x;
	
   ti = s[i];
   j = (unsigned char)(j+ti);
   tj = s[j];
	
   s[i] = tj;
   s[j] = ti;
	
   x = s[ (unsigned char)(ti+tj) ];
	
   i = (unsigned char)(i+1);
   return x;
}


void RC4RandomSetSeed( int32u seed )
{
   unsigned char t, key[4];
   int32u index;
	
   rseed = seed;
	
   /* Fill the key */
   for ( t=0; t<4; t++ )
      key[t] = (unsigned char)((seed>>(t*8)) & 0xff);
	
   /* Fill the table... */
   for ( index=0; index<256; index++)
      s[index] = (unsigned char)index;
	
   /* And shuffle it. */
   for(index = 0, j = 0; index<256; index ++)
   {
      t = s[index];
      /* This was originally index%keyLength, but since
       * keyLength == 4, we can use anding by 3. */
      j += (unsigned char)(t + key[index & 3]);
      s[index] = s[j];
      s[j] = t;
   }
	
   j = i = 0;
}


int32u Random( int32u maximum )
{
   int32u result;
	
   result =(long)RC4Random() + 
      ((long)RC4Random() << 8) +
      ((long)RC4Random() << 16) +
      ((long)RC4Random() << 24);
	
   if ( !maximum )
      return result;
	
   return result % maximum;
}

void TestRC4(void)
{
   int32u sum, avg, min, max, ran, i;

   max=avg=sum=0;
   min=0xffffffff;

   RC4RandomSetSeed(time(0) + time(0)<<16 );

   for(i=0; i<1000; i++) {
      ran=1+RC4Random() % 100;
      printf("\t%ld\t|", ran);
      if(ran<min) min=ran;
      if(ran>max) max=ran;
      sum+=ran;
   }
   avg=sum/i;

   printf("\n\nAVG:%ld MIN:%ld MAX:%ld SUM:%ld\n\n", avg, min, max, sum);

}

/* END OF RANDOM NUMBER GENERATOR */



/* DICE INTERFACE */

void initdice(int16u seed)
{

   RC4RandomSetSeed(time(0) + time(0)<<16 );
}

int16s throwdice(int8u times, int8u sides, int16s dmod) 
{
   int16s dvalue=0;
   
   if(sides==0)
      return dmod;
   
   dvalue=0;
   for(int i=0; i<times; i++)
      dvalue+=1+RANDU(sides);
   
   dvalue+=dmod;
   
   if(dvalue<0)
      dvalue=0;

   return dvalue;
   
}

/*
** Return a 16-bit random number from 0 to limit-1
*/
int16u RANDU(int16u limit) {

   int16u rnum=0;

   if(!limit)
      return 0;

   rnum=(int16u)RC4Random() + ((int16u)RC4Random() << 8);

   return rnum % limit;

}

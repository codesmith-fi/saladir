/* public dice functions */

#ifndef _DICE_H_INCLUDED
#define _DICE_H_INCLUDED

#include "saladir.h"
#include "types.h"

unsigned char RC4Random( void );  /* William Tanksley */
void RC4RandomSetSeed( int32u );  /*    - "" - */
int32u Random( int32u );          /*    - "" - */
void TestRC4( void );

/* dice interface */
void initdice(int16u);
int16s throwdice(int8u, int8u, int16s);
int16u RANDU(int16u);

#endif

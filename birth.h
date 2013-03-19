/*
** birth.h
*/

#ifndef _BIRTH_H_DEFINED
#define _BIRTH_H_DEFINED

/*
#include "classes.h"
#include "dice.h"
#include "skills.h"
#include "magic.
#include "creature.h"
#include "utility.h"
#include "inventor.h"
#include "gametime.h"
#include "tables.h"
#include "init.h"
*/

#define BIRTH_MAXRESPONSES 10
#define BIRTH_ASKCOUNT     10

typedef struct
{
   char *txt;
   int16s STR;
   int16s DEX;
   int16s TGH;
   int16s WIS;
   int16s INT;
   int16s CON;
   int16s CHA;
   int16s LUC;

   int16s ALIGN;

} _Qattrpack;

typedef struct
{
   char *question;
   bool asked;
   _Qattrpack resp[BIRTH_MAXRESPONSES];
} _questiondef;

bool random_language(char *str, int16s maxlen);
bool random_name(char *str, int16s maxlen);
bool birth_giveskillweapon(Tinventory *inv, Tskillpointer skills);

int16s player_born(void);

#endif /* birth.h */


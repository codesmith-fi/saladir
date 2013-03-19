/*
** birth.h
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
bool birth_askten(void);
void player_born(void);


//extern int8u birth_askquestion(int8u);



/* magic.h */

#ifndef _MAGIC_H_DEFINED
#define _MAGIC_H_DEFINED

/* spells */
#define SPELL_HEALING   0
#define SPELL_TELEPORT  1
#define SPELL_IDENTIFY  2
#define SPELL_BLESS     3
#define SPELL_CURSE     4
#define SPELL_UNCURSE   5
#define SPELL_CONFUZE   6
#define SPELL_MAGICMAP  7

/* spell flags */
#define SPF_ALTERATION  0x00000001
#define SPF_DESTRUCTION 0x00000002
#define SPF_OBSERVATION 0x00000004
#define SPF_MYSTICISM   0x00000008

/* spell type */
typedef struct
{
      /* name of the spell */
      char *name;

      int32u flags;

      /* spellpoints required */
      int16s sp;

      /* spell skill levels required for certain target */
      /* negative means it's not possible */
      int16s self;
      int16s other;
      int16s levitem;
      int16s invitem;
      int16s direction;
      
} Tspell;

/* this is used when magic is targetted by player */
typedef struct
{
      Tinvpointer invitem;      /* inventory item targetted */

      /* x and y coordinates of the target(s) */      
      int16s x;
      int16s y;

      int8u direction;
     
} Ttarget;

void player_gettarget(level_type *level, Ttarget *target, Tspell *spell);

bool spell_zap(level_type *level, int16u spell, int16u skill, bool scroll);


extern Tspell list_spells[];

#endif

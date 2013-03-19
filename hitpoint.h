/* hitpoint.h */

#ifndef _HITPOINT_H_DEFINED
#define _HITPOINT_H_DEFINED

typedef struct
{
      int16s fire;
      int16s poison;
      int16s cold;
      int16s magic;
      int16s elec;
      int16s water;
      int16s acid;
} Tresistpack;

/* hitpoints pack */
typedef struct
{
      int16s cur;     /* current value */
      int16s ini;     /* initial value */
      int16s max;     /* maximum value */
      int16s ac;     /* armor class for bodypart */
      int16s dv;     /* defence value */
      Tresistpack res; /* element resistances */
} _hpslot;


/* hitpoint modifiers */
#define HP_HEADMOD	0.50
#define HP_HANDMOD	0.75
#define HP_LEGSMOD	0.75

#define HPSLOT_MAX       5	/* num of hpstat slots */
#define HPSLOT_HEAD	 0
#define HPSLOT_LEFTHAND	 1
#define HPSLOT_RIGHTHAND 2
#define HPSLOT_BODY	 3
#define HPSLOT_LEGS      4

#endif

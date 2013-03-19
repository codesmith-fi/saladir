/* Tables.h for Saladir */

#ifndef _TABLES_H_DEFINED
#define _TABLES_H_DEFINED

#include "saladir.h"

#include "types.h"
#include "defines.h"

/* LoS newer can be over 50 grids long */
#define MAX_LOSLEN      50
#define MAX_LOSLINES    360

/* struct for precalculated lines */
typedef struct
{
      int16u len;
      int16s line_cx[MAX_LOSLEN+1];
      int16s line_cy[MAX_LOSLEN+1];
      int16s vismax_cx;
      int16s vismax_cy;
} Tprecalcline;

extern _Ttactic tacticeffects[];
extern int16s move_dx[];
extern int16s move_dy[];
extern int8u dir_normal[];
extern int8u dir_invers[];
extern int8u move_dirpriority_circulate[8][9];
extern int8u move_dirpriority_attack[8][9];
extern int8u move_dirpriority_flee[8][8];
extern char *syllables[];
extern char *name_syllables[];
extern int16u SKILL_ADV[101];
extern int16s DEX_SPEED[101];
extern int32u CON_HITP[101];

/*
extern real COSTABLE[362];
extern real SINTABLE[362];
extern int16s los_tx_min[LOS_MAXARRAY];
extern int16s los_ty_min[LOS_MAXARRAY];
extern int16s los_tx_max[LOS_MAXARRAY];
extern int16s los_ty_max[LOS_MAXARRAY];
extern int16u los_table_size;
*/
/*
extern Tprecalcline LOS_LINETABLE[MAX_LOSLINES+1];
*/


#endif

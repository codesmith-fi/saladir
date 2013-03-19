/**************************************************************************
 * init.cc --                                                             *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1888                                         *
 *                                                                        *
 **************************************************************************
 *                                                                        *
 * (C) 1997, 1998 by Erno Tuomainen.                                      *
 * All rights reserved.                                                   *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 *                                                                        *
 **************************************************************************/

#include "init.h"

#include "variable.h"
void init_game(void)
{
   GD.monid=0;
}

/*
void init_sincos(void)
{
   real pii;
   int16u a;

   pii=(real)3.14159265453;

   for(a=0; a<360; a++) {
      SINTABLE[a]=sin(a*(pii/180));
      COSTABLE[a]=cos(a*(pii/180));
   }
}
*/

void init_skilladvance(void)
{
   real k=1.1;

   for(sint z=0; z<101; z++) {
      k=(8.0/100 * z)+1.8;

      SKILL_ADV[z]=(int16u)(10+exp(k));

//      my_printf("%d=%d\t", z, SKILL_ADV[z]);

//		my_printf("%d=%d\t", z, SKILL_ADV[z]);
   }
}

void init_tables(void)
{
   int16u i=0;

//   init_sincos();
   init_skilladvance();

   /* init DEX to SPEED table */
   for(i=0; i<101; i++) {
      DEX_SPEED[i]=(i-50)/2;
   }

   for(i=0; i<101; i++) {
      CON_HITP[i]=((100-i)*250)+1000;
   }

}


/*
void precalc_line(Tprecalcline *losline, 
		    int16s x1, int16s y1, int16s x2, int16s y2)
{
   int16s d, x, y, ax, ay, sx, sy, dx, dy;

   bool nolimit=false;
   sx=sy=0;

   losline->len=0;

   dx = x2-x1;
   dy = y2-y1;
   sx = SGN(dx);
   sy = SGN(dy);
   ax = ABS(dx)<<1;
   ay = ABS(dy)<<1;

   x = x1;
   y = y1;
   if (ax>ay) {
      d = ay-(ax>>1);
      for (;;) {

	 losline->line_cx[losline->len]=x;
	 losline->line_cy[losline->len]=y;

	 losline->len++;

	 if(distance(0, 0, x, y) >= player.sight && !nolimit) {
	    nolimit=true;
	    losline->vismax_cx=x;
	    losline->vismax_cy=y;

	 }

	 if (x==x2) {
	    return;
	 }
	 if (d>=0) {
	    y += sy;
	    d -= ax;
	 }
	 x += sx;
	 d += ay;
      }
   }
   else {
      d = ax-(ay>>1);
      for (;;) {

	 losline->line_cx[losline->len]=x;
	 losline->line_cy[losline->len]=y;

	 losline->len++;

	 if(distance(0, 0, x, y) >= player.sight && !nolimit) {
	    nolimit=true;
	    losline->vismax_cx=x;
	    losline->vismax_cy=y;

	 }

	 if (y==y2) return;
	 if (d>=0) {
	    x += sx;
	    d -= ay;
	 }
	 y += sy;
	 d += ax;
      }
   }
}

void new_initlos(int16u radius)
{
   real maxr1, maxr2, maxr3, maxr4;

   int16s x_o, y_o, x_n, y_n, x_r, y_r;
   int16u linecount=0;
   int16s z;

   if(radius<3)
      radius=3;
   if(radius>30)
      radius=30;

   player.sight=radius;

   maxr1=(1.0)*radius;
   maxr4=(1.0)*(radius+3.0);
   maxr2=(1.0)*radius*(1.1);
   maxr3=(1.0)*((radius+3.0)*1.1);

   x_n=0;
   y_n=0;

   los_table_size=0;

   for(z=0; z<MAX_LOSLINES; z++) {
      x_o=x_n;
      y_o=y_n;

      x_r=(int16s)(SINTABLE[z]*maxr2);
      y_r=(int16s)(COSTABLE[z]*maxr1);

      x_n=(int16s)(SINTABLE[z]*maxr3);
      y_n=(int16s)(COSTABLE[z]*maxr4);

      LOS_LINETABLE[z].len=0;
      if(x_n!=x_o || y_n!=y_o) {

	 precalc_line(&LOS_LINETABLE[linecount], 0, 0, x_n, y_n);

	 linecount++;
      }
   }
}

*/

/*
void init_lineofsight(int16u radius)
{
   return;
//   new_initlos(radius);
//   return;

   real maxr1, maxr2, maxr3, maxr4;

   int16s x_o, y_o, x_n, y_n, x_r, y_r;

   if(radius<3)
      radius=3;
   if(radius>30)
      radius=30;

   player.sight=radius;

   maxr1=(1.0)*radius;
   maxr2=(1.0)*radius*(1.1);
   maxr3=(1.0)*((radius+3.0)*1.1);
   maxr4=(1.0)*(radius+3.0);

   x_n=-1;
   y_n=-1;

   los_table_size=0;

   for(int16s z=0; z<360; z++) {
      x_o=x_n;
      y_o=y_n;

      x_r=(int16s)(SINTABLE[z]*maxr2);
      y_r=(int16s)(COSTABLE[z]*maxr1);

      x_n=(int16s)(SINTABLE[z]*maxr3);
      y_n=(int16s)(COSTABLE[z]*maxr4);

      if(x_n!=x_o || y_n!=y_o) {
	 los_tx_max[los_table_size]=x_n;
	 los_ty_max[los_table_size]=y_n;

	 los_tx_min[los_table_size]=x_r;
	 los_ty_min[los_table_size]=y_r;

	 los_table_size++;
      }
   }
}
*/

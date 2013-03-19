/**************************************************************************
 * player.cc --                                                           *
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

#include "saladir.h"

#include "output.h"
#include "memory.h"

#include "creature.h"
#include "inventor.h"
#include "tables.h"
#include "utility.h"
#include "magic.h"
#include "gametime.h"
#include "file.h"
#include "birth.h"

char *txt_statrating[]=
{
   "Very bad",
   "Bad",
   "Low",
   "Poor",
   "Fair",
   "Average",
   "Nice",
   "Good",
   "Very good",
   "Superhuman",
   "Immortal",
   NULL
};

typedef struct
{
      const char *name;
      const char *attr;
      int16s mod;
      int16s skl_gen;
      int16s skl_magic;
      int16s skl_weapon;
} Ttrait;

Ttrait list_traits_neg[]=
{
   { "Ugliness", txt_statnames_short[STAT_CHA], -15,
     0, 0, 0 },
   { "Slow mind", txt_statnames_short[STAT_INT], -5,
     0, 0, 5 },
   { "Slowliness", txt_statnames_short[STAT_DEX], -5,
     0, 0, 0 },

   { NULL, NULL, 0, 0, 0, 0 } /* end */
};

Ttrait list_traits_pos[]=
{
   { "Good looks", txt_statnames_short[STAT_CHA], 10,
     0, 0, 0 },
   { "Magic and Power", txt_statnames_short[STAT_WIS], 5,
     0, 15, 0 },
   { "Battle", txt_statnames_short[STAT_STR], 4,
     0, 0, 5 },
   { "Bright mind", txt_statnames_short[STAT_INT], 5,
     0, 5, 0 },
   { "Fast legs", txt_statnames_short[STAT_DEX], 5,
     0, 0, 0 },
   { "Skillfullness", txt_statnames_short[STAT_WIS], 2,
     5, 5, 5 },
   
   
};

typedef struct
{
      const char *name;
      int16s skl_gen;
      int16s skl_magic;
      int16s skl_weapon;
} Tbackground;

Tbackground list_backgrounds[]=
{
   { "Magic", 0, 30, 0 },
   { "Fighting", 0, 0, 30 },
   { "Magic & Fighting", 0, 15, 15 },

   { NULL, 0, 0, 0 }
};

#define TEACHER_ALBINIA   0
#define TEACHER_ALREDOR   1
#define TEACHER_THORNDARR 2
#define TEACHER_MANDALIN  3
#define TEACHER_FERDINAND 4
#define TEACHER_ERIOL     5
#define NUM_TEACHERS 6

const char *list_teachers[]=
{
   "Albinia, the Sorceror of Dalmn",
   "Al'redor, the Black Wizard",
   "Thorndarr, the Elf Healer",
   "Mandalin, the Fighter",
   "Ferdinand, the High Paladin",
   "Eriol, the Ranger.",
   NULL
};

int16u selected_teachers[NUM_TEACHERS+1];

#define SCHOOL_ALTERATION  0
#define SCHOOL_DESTRUCTION 1
#define SCHOOL_MYSTICISM   2
#define SCHOOL_OBSERVATION 3
const char *list_magicschools[]=
{
   "School of Alteration",
   "School of Destruction",
   "School of Mysticism",
   "School of Observation",
   NULL
};

#define NUM_AGEPREFIX 4
const char *txt_ageprefix[]=
{
   "At the age of %d",
   "When you were %d years old",
   "During your studies at the age of %d",
   "You were %d years old when",
   NULL
};

#define NUM_RANDEFFECT 2
#define RANDEFF_INHERIT 0
#define RANDEFF_PLAGUE  1
#define RANDEFF_BANDITS 2
const char *txt_events[]=
{

   "your distant uncle died and as his only relative\nyou received his "
     "fortune of %d gold!",
   "a black plague stroke the town you were studying\nat, evetually you "
     "got sick and were sick for %d months and lost %d %s.",
   "a group of bandits raided your town. While\nhelping your fellow "
     "citizens to defend against the bandits\nyou were wounded but "
     "gained %d %s!",   
   NULL
};
int8u used_effects[NUM_RANDEFFECT+1];

char welcometext[]=
"\007Welcome to the land of \002Salmorria\007, stranger. You're just "
"a few steps away from the adventure, gold and fortune. You're ready "
"to go after the Legend of Saladir...\n"
"But before anything, please select a name for yourself or press enter "
"and just let the fate decide...\n\n"
"\0";

char txt_prebirth[]=
"\7Before you can go adventuring, you must first create a character. "
"In this version you have two choices for creating your brave adventurer."
"\n\n\t\1a\7) Create your own character\n"
"\t\1b\7) Create a random character\n\n";

char txt_titlesel[]=
"\007Now you can think a title for you. Think of something which describes "
"yourself. An example, if your name is \"\002Melthor\007\", then you could be "
"\"\002Melthor \003the great.\007\" or \"\002Melthor \003the magician\007\". "
"You can leave this blank and use default \"\003the adventurer\007\" "
"instead.\n\n";

char txt_primaryweapon[]=
"\007Now it's time to pick your primary weapon skill. You will learn this "
"skill all the way up to\001 25% \007(out of 100%) and you will also get "
"a starting weapon which needs this skill.";

char txt_secondaryweapon[]=
"\007Now pick the secondary weapon skill. You will have a\001 10% "
"\007chance on the "
"skill you now select. If you pick the same skill as the primary skill, "
"you will increase it by\001 10%\007.";

char txt_statedit[]=
"\007Now edit your attributes, these attributes tell a lot about you. "
"How strong you are, how intelligent you are, and so on. "
"An average value is \00150 \007which means that you gain no bonuses but "
"don't get any negative effects either.\n\n";

char txt_statedit_auto[]=
"\007In automatic mode, you can not decrease your already calculated "
"attributes, but you have some points left to distribute as you wish.\n\n";

char txt_statedit2[]=
"\007Use \002UP/DOWN \007or \002A/Z \007keys to select attribute for "
"editing.\n"
"\002LEFT/RIGHT \007and \002-/+ \007keys will modify the value of the "
"highlighted attribute.\n"
"When all points used, press \002ENTER \007to continue, or press \002Q \007to "
"force quit (and possibly) leave unused points to the pool.\n\n";

const char *statdesc_str=
"\007High strength makes you, well eh..., stronger. It affects your carrying "
"capasity and several other things like how hard you hit your foes. ";

const char *statdesc_dex=
"\007Dexterity affects your speed and other speed related issues. High "
"dexterity allows you to act faster in all situations. ";

const char *statdesc_tgh=
"\007Toughness affects in many ways. High toughness makes you more tolerant "
"to attacks, affecting your defence rating. Low toughness makes the opposite. "
"There're some other situations too where high toughness does aid your "
"journeys.";

const char *statdesc_con=
"\007Constitution tells how your body is built. The natural strength of your "
"body. It affects your healing rate, how fast your wounds recover.";

const char *statdesc_cha=
"\007Charisma is your \"outlook\", how well you get by with others, how "
"good looking you are, and so on.\n\n"
"\001You can't edit this stat because it really doesn't affect anything "
"yet.\n";

const char *statdesc_wis=
"\007Wisdom tells something about your general knowledge.\n\n"
"\001You can't edit this stat because it really doesn't affect anything "
"yet.\n";

const char *statdesc_int=
"\007Intelligence affects your learning ability. The higher your intelligence "
"is, the better you can learn new skills, spells and so on.\n\n "
"\001You can't edit this stat because it really doesn't affect anything "
"yet.\n";

const char *statdesc_luc=
"\007";

const char *statdesc_spd=
"\007Speed is a derived attribute, it's calculated mainly by using your "
"dexterity. High speed allows you to move faster.";

const char *txt_statdescs[]=
{
   statdesc_str,
   statdesc_tgh,
   statdesc_con,
   statdesc_cha,
   statdesc_dex,
   statdesc_wis,
   statdesc_int,
   statdesc_luc,
   statdesc_spd,
   NULL
};

/* find a best weapon skill and give an item for that skill */
bool birth_giveskillweapon(Tinventory *inv, Tskillpointer skills)
{
   int16u high=0, now=0, skill=SKILL_ENDLIST;
   int16u material;

   for(sint i=0; i<NUM_WEAPONSKILLS; i++) {      
      now=skill_check(skills, SKILLGRP_WEAPON, i);
      if(now > high) {
	 high=now;
	 skill=i;
      }
   }

   if(skill==SKILL_ENDLIST) {
      return false;
   }

   /* no weapon for weaponless combat */
   if(skill==SKILL_HAND)
      return true;

   if(skill==SKILL_THROW) {
      inv_additem(inv, IS_WEAPON1H, WEAPONS_ROCK, 1, MAT_STONE);
      return true;
   }

   while(1) {
      now=RANDU(num_weapons);

      if(weapons[now].group == skill)
	 break;
   }

   high=1+RANDU(100);
   if(high < 33)
      material = MAT_STEEL;
   else if(high < 66)
      material = MAT_IRON;
   else 
      material = MAT_COPPER;

   inv_additem(inv, IS_WEAPON1H, now, 1, material);

   return true;
}

/* initialize player inventory */
void birth_initpack() {

   inv_additem(&player.inv, IS_MONEY, MONEY_GOLD, 120, -1);

   inv_additem(&player.inv, IS_WEAPON1H, WEAPONS_LONGBOW, 1, -1);
   inv_additem(&player.inv, IS_WEAPON1H, WEAPONS_ARROW, 24, -1);

   inv_additem(&player.inv, IS_LIGHT, MISCI_LANTERN, 1, -1);

   birth_giveskillweapon(&player.inv, player.skills);

   skill_modify_raise(&player.skills, 
		      SKILLGRP_MAGIC, SPELL_TELEPORT, 
		      1200, true);
   skill_modify_raise(&player.skills, 
		      SKILLGRP_GENERIC, SKILL_DISARMTRAP, 
		      200, true);

   inv_additem(&player.inv, IS_WEAPON1H, WEAPONS_STAFF, 1, MAT_ELVEN);

}

bool random_language(char *str, int16s maxlen)
{
   sint numsyll=0, cur, num, num2;
   sint spec=0, space=0;

   if(!str)
     return false;

   maxlen--;

   if(maxlen<=0)
     return false;

   while(syllables[numsyll])
     numsyll++;

   if(!numsyll)
     return 0;

   str[0]=0;
   num=num2=4+RANDU(6);
   space=1;
   while(maxlen>0) {
      cur=RANDU(numsyll);

      if((1+RANDU(100)) < 10) {
	 if(!spec && num!=num2) {
	    spec++;
	    strcat(str, "'");
            maxlen--;
            continue;
	 }
      }
      if( (1+RANDU(100)) < 30 && !space) {
	 space=1;
	 strcat(str, " ");
         maxlen--;
         continue;
      }
      maxlen-=strlen(syllables[cur]);
      if(maxlen<0 || num==0)
	return true;

      space=0;
      strcat(str, syllables[cur]);
      num--;
   }
   return true;
}


bool random_name(char *str, int16s maxlen)
{
   sint numsyll=0, cur, num, num2;
   
   if(!str)
      return false;
   
   maxlen--;
   if(maxlen<=0)
      return false;
   
   while(name_syllables[numsyll]) 
      numsyll++;
   
   if(!numsyll)
      return 0;
   str[0]=0;
   
   num=num2=2+RANDU(3);
   
   while(maxlen>0) {
      cur=RANDU(numsyll);

      maxlen-=strlen(name_syllables[cur]);
      if(maxlen<0 || num==0)
	 return true;
      
      strcat(str, name_syllables[cur]);
      num--;
   }
   return true;
}

void birth_ask_skills(void)
{
   /* first two weapon skills */
/*
   skill_modify(&player.skills, SKILLGRP_WEAPON, 
		skill_listselect(SKILLGRP_WEAPON, txt_primaryweapon),
		25, true);

   skill_modify(&player.skills, SKILLGRP_WEAPON, 
		skill_listselect(SKILLGRP_WEAPON, txt_secondaryweapon),
		10, true);

   skill_modify(&player.skills, SKILLGRP_MAGIC, 
		skill_listselect(SKILLGRP_MAGIC, "SPELL"),
		10, true);
*/

//   skill_modify(&player.skills, SKILLGRP_MAGIC, 
//		SPELL_TELEPORT, 50, true);
//   skill_modify(&player.skills, SKILLGRP_MAGIC, 
//		SPELL_IDENTIFY, 50, true);

}

void birth_random_stats(_statpack *stats)
{

   int16s statpool, i;

   /* fill pool with enough points to distribute between stats */
   
   statpool = (STAT_BASICARRAY * 40)+RANDU(STAT_BASICARRAY * 10) ;

   /* clear stats first */
   for(i=0; i<STAT_BASICARRAY; i++) {
      set_stat_initial(&stats[i], 0, STATMAX_GEN, 1);
   }

   while(statpool > 0) {

      i=RANDU(STAT_BASICARRAY);

      change_stat_initial(&stats[i], 1, true);

      statpool--;

   }

   /* set luck */
   set_stat_initial(&stats[STAT_LUC], 
		    (throwdice(1, STATMAX_LUCK/2, 0) + 
		     throwdice(1, STATMAX_LUCK/2, 0)),
		    STATMAX_LUCK, 0);
}

const char *txt_teach1=
"\007It's time to create your background. You have a choice of selecting "
"teachers you want to study with. Each teacher have a set of skills it "
"can teach you. Each phase of learning takes 3 years of your life time.\n\n";

const char *txt_teach2=
"\007You can choose a path for your magic studies.\n\n";

int16s select_textlist(char *prompt, const char **list, bool quit) 
{
   int16u num=0, cury, ch;
   
   while(list[num] != NULL) {
      my_setcolor(C_RED);
      my_printf("%d", num+1);
      my_setcolor(C_WHITE);
      my_printf("] %s\n", list[num]);
      num++;
   }
   cury=my_gety()+1;

   while(1) {
      my_gotoxy(1, cury);
      my_setcolor(C_GREEN);
      my_printf("%s? ", prompt);
      ch=my_getch();
      if(ch>='1' && ch <='9') {
	 ch-='1';
	 if(ch<num)
	    return ch;
      }

      if(ch=='q' && quit) {
	 return -1;
      }

   }
   my_printf("\n");
   return 0;
}

void birth_randomeffect(int16u age) 
{
   int16u prefix, effect;
   int16s amount, stat;
  
   if(throwdice(1, 100, 0) > 20) 
      return;

   prefix=RANDU(NUM_AGEPREFIX);
   effect=RANDU(NUM_RANDEFFECT);

   /* assure that the effect is not yet used */
   amount=0;
   while(used_effects[effect]!=0 && (amount++ < 100))
      effect=RANDU(NUM_RANDEFFECT);

   /* if it's still used, return */
   if(used_effects[effect]>0)
      return;

   used_effects[effect]=1;

   sprintf(i_hugetmp, "\n%s %s\n", txt_ageprefix[prefix], txt_events[effect]);

   if(effect==RANDEFF_INHERIT) {
      amount=50+RANDU(200);
      inv_additem(&player.inv, IS_MONEY, MONEY_GOLD, amount, -1);
      my_printf(i_hugetmp, age, amount);
   }
   else if(effect==RANDEFF_PLAGUE) {
      amount=(5+RANDU(10));
      stat=RANDU(STAT_BASICARRAY);
      change_stat_initial(&player.stat[stat], amount, true);
      amount=-amount;
      my_printf(i_hugetmp, age, 1+RANDU(5), amount, txt_statnames_short[stat]);
   }
}


int16u birth_study(bool automatic)
{
   int16s teacher, num=1, school;
   int16s learnvalue;

   int16u yearstook=6+RANDU(4);

//   my_printf("You were %d years old at the start of your studies.\n",
//	     yearstook);
   my_clrscr();

   /* clear randeffect table */
   for(num=0; num<NUM_RANDEFFECT; num++)
      used_effects[num]=0;

   while(yearstook < 20) {

      if(automatic) {
	 teacher=RANDU(NUM_TEACHERS);
      }
      else {
	 my_clrscr();
	 my_wordwraptext( txt_teach1, my_gety(), SCREEN_LINES, 1, SCREEN_COLS);

	 my_printf("At the age of %d you decided to ... \n\n", yearstook);
      
	 teacher=select_textlist("Choose a teacher (Q to end studies)", 
				 list_teachers, true);

	 if(teacher<0) {
	    my_printf("You decide to end your studies.\n");
	    return yearstook;
	 }
      }
     
      yearstook+=3;
      learnvalue=150 + RANDU(150);

      if(teacher==TEACHER_ALBINIA || teacher==TEACHER_ALREDOR ||
	 teacher==TEACHER_THORNDARR ) {

	 if(automatic) {
	    school=RANDU(4);
	 }
	 else {
	    my_wordwraptext( txt_teach2, my_gety(), 
			     SCREEN_LINES, 1, SCREEN_COLS);

	    school=select_textlist("Select a direction for your studies", 
				   list_magicschools, false);
	 }
	 switch(school) {
	    case SCHOOL_ALTERATION:
	       skill_modify_raise(&player.skills, 
				  SKILLGRP_GENERIC, SKILL_ALTERATION,
				  learnvalue/2, true);
	       break;
	    case SCHOOL_DESTRUCTION:
	       skill_modify_raise(&player.skills, 
				  SKILLGRP_GENERIC, SKILL_DESTRUCTION,
				  learnvalue/2, true);
	       break;
	    case SCHOOL_OBSERVATION:
	       skill_modify_raise(&player.skills, 
				  SKILLGRP_GENERIC, SKILL_OBSERVATION,
				  learnvalue/2, true);
	       break;
	    case SCHOOL_MYSTICISM:
	       skill_modify_raise(&player.skills, 
				  SKILLGRP_GENERIC, SKILL_MYSTICISM,
				  learnvalue/2, true);
	       break;
	    
	    default:
	       break;
	 }
	 
      }

      if(teacher==TEACHER_ALREDOR) {
	 change_alignment(&player.align, 'c', 
			  100+RANDU(ALIGNMENT_LIMIT/2));
	 skill_modify_raise(&player.skills, 
			    SKILLGRP_WEAPON, SKILL_DAGGER,
			    learnvalue, true);
	 skill_modify_raise(&player.skills, 
			    SKILLGRP_MAGIC, SPELL_CURSE, 
			    learnvalue/4, true);

      }

      if(teacher==TEACHER_ALBINIA) {
	 change_alignment(&player.align, 'n', 
			  100 + RANDU(ALIGNMENT_LIMIT/2));
	 skill_modify_raise(&player.skills, 
			    SKILLGRP_WEAPON, SKILL_STAFF,
			    learnvalue, true);
	 skill_modify_raise(&player.skills, 
		      SKILLGRP_MAGIC, SPELL_CONFUZE, 
		      learnvalue/4, true);
      }

      if(teacher==TEACHER_MANDALIN) {
	 change_alignment(&player.align, 'n', 
			  100 + RANDU(ALIGNMENT_LIMIT/2));	 


	 skill_modify_raise(&player.skills, 
			    SKILLGRP_WEAPON, SKILL_SWORD,
			    learnvalue, true);
	 skill_modify_raise(&player.skills, 
			    SKILLGRP_WEAPON, SKILL_SHIELD,
			    learnvalue/2, true);
      }

      if(teacher==TEACHER_FERDINAND) {
	 change_alignment(&player.align, 'l', 
			  100 + RANDU(ALIGNMENT_LIMIT/2));

	 skill_modify_raise(&player.skills, 
			    SKILLGRP_WEAPON, SKILL_SWORD,
			    learnvalue, true);

      }

      if(teacher==TEACHER_ERIOL) {
	 change_alignment(&player.align, 'n', 
			  100 + RANDU(ALIGNMENT_LIMIT/2));

	 skill_modify_raise(&player.skills, 
			    SKILLGRP_WEAPON, SKILL_BOW,
			    learnvalue/2, true);
	 skill_modify_raise(&player.skills, 
			    SKILLGRP_WEAPON, SKILL_DAGGER,
			    learnvalue/2, true);
	 skill_modify_raise(&player.skills, 
			    SKILLGRP_GENERIC, SKILL_FOODGATHER,
			    learnvalue/2, true);
	 skill_modify_raise(&player.skills, 
			    SKILLGRP_GENERIC, SKILL_FINDWEAKNESS,
			    learnvalue/4, true);
      }

      if(teacher==TEACHER_THORNDARR) {
	 change_alignment(&player.align, 'l', 
			  100 + RANDU(ALIGNMENT_LIMIT/2));

	 skill_modify_raise(&player.skills, 
			    SKILLGRP_GENERIC, SKILL_HEALING,
			    learnvalue/2, true);
	 skill_modify_raise(&player.skills,
			    SKILLGRP_WEAPON, SKILL_DAGGER,
			    learnvalue/4, true);
	 skill_modify_raise(&player.skills, 
			    SKILLGRP_GENERIC, SKILL_FINDWEAKNESS,
			    learnvalue/4, true);

      }

      birth_randomeffect(yearstook);

      my_printf("\nYou studied 3 years with %s.\n", 
		list_teachers[teacher]);      
   }

//   skill_addnew(&player.skills, 
//		      SKILLGRP_GENERIC, SKILL_HEALING, 50);

   showmore(false, false);

   return yearstook;
}

void birth_ask_stats(bool automatic)
{
   int8u i;

   int16s statpool=10, cstat;
   int16u stat_ty=0, stat_tx=0, cy=0, ch, desc_y=0;
    
   my_clrscr();
   
   if(!automatic)
      my_wordwraptext( txt_statedit, 1, SCREEN_LINES, 1, SCREEN_COLS);
   else
      my_wordwraptext( txt_statedit_auto, 1, SCREEN_LINES, 1, SCREEN_COLS);

   /* init randomly */
   birth_random_stats(player.stat);

   stat_ty=my_gety();
   for(i=0; i<7; i++) {
      my_setcolor(C_WHITE);
      my_printf("%-20s: ", txt_statnames[i]);
      stat_tx=my_getx();
      my_setcolor(C_YELLOW);
      cstat=get_stat(&player.stat[i]);
      my_printf("%2d (%10s)\n", cstat, txt_statrating[cstat/10]);

   }

   my_wordwraptext( txt_statedit2, my_gety()+1, SCREEN_LINES, 1, SCREEN_COLS);

   desc_y=my_gety();
   my_setcolor(C_WHITE);
   drawline(desc_y, '=');
   my_gotoxy(3, desc_y++);
   my_setcolor(CH_WHITE);
   my_printf("[ Description ]");

   my_gotoxy(stat_tx+16, stat_ty);
   my_setcolor(C_WHITE);
   my_printf("Left: %3d", statpool);
   my_gotoxy(stat_tx, stat_ty+cy);
   my_setcolor(CH_RED);
   my_printf("%2d (%10s)", get_stat(&player.stat[cy]),
	     txt_statrating[get_stat(&player.stat[cy])/10]);
   
   while(1) {
      ch=my_getch();

      cstat=get_stat(&player.stat[cy]);

      my_gotoxy(stat_tx, stat_ty+cy);
      my_setcolor(C_YELLOW);
      my_printf("%2d (%10s)", cstat, 
		txt_statrating[cstat/10]);
      
      if((ch==KEY_DOWN || ch=='z' || ch=='2') && cy<6) {
	 cy++;
	 my_wordwraptext( txt_statdescs[cy], desc_y, 
			  SCREEN_LINES, 1, SCREEN_COLS);
	 my_clr2bot();
      }
      else if((ch==KEY_UP || ch=='a' || ch=='8') && cy>0) {
	 cy--;
	 my_wordwraptext( txt_statdescs[cy], desc_y, 
			  SCREEN_LINES, 1, SCREEN_COLS);
	 my_clr2bot();
      }
      else if((ch=='+' || ch==KEY_RIGHT || ch=='6') && statpool>0 ) {
	 if(cy==STAT_CHA || cy==STAT_INT || cy==STAT_WIS) 
	    continue;

	 if(cstat < STATMAX_GEN) {
	    statpool--;
	    change_stat_initial(&player.stat[cy], 1, true);
	 }
      }
      else if(!automatic && (ch=='-' || ch==KEY_LEFT || ch=='4') && cstat>30) {
	 if(cy==STAT_CHA || cy==STAT_INT || cy==STAT_WIS) 
	    continue;

	 statpool++;
	 change_stat_initial(&player.stat[cy], -1, true);
      }
      else if((ch==32 || ch==10 || ch==KEY_ENTER)) {
	 if(statpool==0)
	    break;
	 else {
	    my_gotoxy(stat_tx+16, stat_ty+STAT_BASICARRAY);
	    my_setcolor(C_RED);
	    my_printf("You have undistributed points!", 
		      statpool);
	 }
      }
      else if(ch=='q' || ch=='Q')
	 break;

      my_gotoxy(stat_tx+16, stat_ty);
      my_setcolor(C_WHITE);
      my_printf("Left: %3d", statpool);

      my_gotoxy(stat_tx+16, stat_ty+1);
      my_printf("Carrying capasity = %4.2f", 
		((real)calc_carryweight(player.stat, player.weight))/1000);

      my_gotoxy(stat_tx, stat_ty+cy);
      my_setcolor(CH_RED);
      cstat=get_stat(&player.stat[cy]);
      my_printf("%2d (%10s)", cstat, txt_statrating[cstat/10]);
   }
   /*******************/
   /* basic stats set */
   /*******************/

   /* set alignment */
   player.align = RANDU(LAWFUL_E);

}

/*************************************/
/* Here we give birth to the player */
/*************************************/
int16s player_born(void)
{
   int16u sel;
   int16s yearstook=0;

   bool automode=false;

   my_clrscr();
   my_setcolor(CH_WHITE);
   my_printf(SALADIR_VERSTRING);
   my_wordwraptext( welcometext, 3, SCREEN_LINES, 1, SCREEN_COLS);
   my_setcolor(C_YELLOW);
   my_printf("What is your name? ");
   my_setcolor(C_GREEN);
   my_gets(player.name, NAMEMAX);
   
   if(strlen(player.name)==0) {
      random_name(player.name, sizeof(player.name)-1);
      player.name[0]=toupper(player.name[0]);
      my_setcolor(C_WHITE);
      my_printf("\nSo be it, your name is \"%s\" today.\n", player.name);
   }

   player.name[0]=toupper(player.name[0]);

   my_printf("\n");
   my_setcolor(C_WHITE);

   automode = false;
   /* first test if savefiles exist, if true then load */
   if( load(true) ) {
      if( (automode = load(false)) ) {
	 my_printf("%s, Welcome back.\nYou left while you were in %s (%s).\n", 
		   player.name, 
		   dungeonlist[player.dungeon].name,
		   player.levptr->name);
	 showmore(false, false);
	 yearstook = -1;
      }
      else {
	 my_printf("ERROR: Failed to restore the save file!\n");
      }
   }
   
   if( !automode ){
      /* no previous save files exist */
      my_setcolor(C_YELLOW);
      sprintf(tempstr, "%s, are you (m)ale or (f)emale", player.name);
      sel=get_response(tempstr, "mMfF");
      if(sel==0 || sel==1)
	 player.gender=SEX_MALE;
      else
	 player.gender=SEX_FEMALE;

      my_clrscr();
      my_wordwraptext( txt_prebirth, 1, SCREEN_LINES, 1, SCREEN_COLS);
      sel=get_response("What will it be?", "aAbB");

      if(sel==0 || sel==1) 
	 automode=false;
      else
	 automode=true;
   
      birth_ask_stats(automode);

      my_clrscr();
      my_printf("You borned at ");
      worldtime.print_worldtime();
      my_printf("\n");

      yearstook=birth_study(automode);

      birth_ask_skills();

      my_clrscr();
      my_wordwraptext( txt_titlesel, my_gety(), SCREEN_LINES, 1, SCREEN_COLS);
      my_setcolor(C_YELLOW);
      my_printf("What is your title? ");
      my_setcolor(C_GREEN);
      my_gets(player.title, TITLEMAX);

      if(strlen(player.title)<1)
	 my_strcpy(player.title, "the adventurer", TITLEMAX);

      /* init inventory */
      birth_initpack();

      calculate_hp(player.hpp, npc_races[player.prace].hp_base, RACE_HUMAN);
      player.hp_max=player.hp=calculate_totalhp(player.hpp);

      player.stat[STAT_SPD].initial= npc_races[player.prace].SPD + 
	 DEX_SPEED[ get_stat( &player.stat[STAT_DEX] ) ];

   }
           
/*   
     if(classes[player.pclass].initfunc == NULL) {
     my_printf("Class init function missing! (Not done yet!)\n");
     my_getch();
     }
     else {
     classes[player.pclass].initfunc(&player.skills, &player.inv, player.stat, player.hpp);
     }
*/

   return yearstook;
}

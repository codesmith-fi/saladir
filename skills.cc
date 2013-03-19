/**************************************************************************
 * skill.cc --                                                            *
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
#include "types.h"
#include "magic.h"
#include "init.h"
#include "tables.h"

#include "skills.h"

const char *skilllevelnames[]=
{
   "unskilled",       /* 0 */
   "beginner",
   "trainee",
   "promising",
   "apprentice",
   "journeyman",
   "skilled",
   "reliable",
   "expert",
   "master",
   "grandmaster",     /* 10 */
   NULL
};


/*******************************/
/* SKILL DESCRIPTION STRINGS   */
/*******************************/

/* WEAPON SKILLS */
const char *skilldesc_weaponless=
"Weaponless combat skill affects your fighting ability when you fight without "
"weapons, using your hands and feet only.";

const char *skilldesc_daggers=
"Daggers are small but deadly blades. Apart from their small size, they can "
"be lethal when used "
"properly. This skill tells how good you're in handling daggers. ";

const char *skilldesc_swords=
"Swords are blades bigger and heavier than daggers. Swords are slower to use "
"but they issue superior damage to daggers. This skill is for both "
"one handed and two handed swords.";

const char *skilldesc_axes=
"Axes and battle axes. ";

const char *skilldesc_blunt=
"Blunt weapons are weapons of force. Hammers and clubs.";

const char *skilldesc_polearm=
"Polearms are long, sharp poles. Proper use requires good handling.";

const char *skilldesc_staff=
"Staffs are often used by mages.";

const char *skilldesc_bow=
"Bows and crossbows. These weapons are not melee weapons, they are used "
"to attack targets from a distance.";

const char *skilldesc_throw=
"Not only bows can be used to attack distant foes, you can also throw items "
"with your bare hands.";

const char *skilldesc_twohanded=
"If you possess this skill, you gain bonuses when using two handed weapons. "
"These bonuses depend on the level of this skill.";

const char *skilldesc_onehanded=
"If you possess this skill, you gain bonuses when using one handed weapons. "
"These bonuses depend on the level of this skill.";

const char *skilldesc_twoweapon=
"Fighting successfully with two weapons, one in both of your hands, "
"requires good handling "
"of the weapon you're using and also a skill of using two weapons "
"simultaneously. ";

const char *skilldesc_shield=
"A shield or a buckler is a powerful tool in the hands of a skilled warrior. "
"This skill directly affects your attempts to use shield items as "
"protective equipment. Train it well and survive.";

/* GENERIC SKILLS */
const char *skilldesc_foodgather=
"Food gathering skill allows you to get more food out of corpses and several "
"other sources. "
"The better your knowledge is in this area, the more you are able to "
"gather food.";

const char *skilldesc_observation=
"Observation skill is learned at the Magic School of Observation. The school "
"of Observation teaches ways to examine the real nature of things. "
"The skill itself affects directly to your Observation spells. "
"Owning a high skill in this area "
"produces more successes when casting spells of Observation. It also can "
"affect your ability to learn new Observation spells.";

const char *skilldesc_mysticism=
"Mysticism skill is learned at the Magic School of Mysticism. The school "
"teaches methods to create illusions and temporary alterations of nature. "
"The skill itself affects directly to your Mysticism spells. "
"Owning a high skill in this area "
"produces more successes when casting spells of Mysticism. It also can "
"affect your ability to learn new Mysticism spells.";

const char *skilldesc_alteration=
"Alteration skill is learned at the Magic School of Alteration. The school "
"teaches magical methods and ways to alter the nature of things. "
"The skill itself affects directly to your Alteration spells. "
"Owning a high skill in this area "
"produces more successes when casting spells of Alteration. It also can "
"affect your ability to learn new Alteration spells.";

const char *skilldesc_destruction=
"Destruction skill is learned at the Magic School of Destruction. The school "
"teaches methods to permanently harm or destruct things of nature. "
"The skill itself affects directly to your Destruction spells. "
"Owning a high skill in this area "
"produces more successes when casting spells of Destruction. It also can "
"affect your ability to learn new Destruction spells.";

const char *skilldesc_concentration=
"Concentration is an important skill for mages and other spellcasters. It "
"makes possible to cast hard spells more easily. As your concentration skill "
"increases, it's more likely to get successes when casting spells.";

const char *skilldesc_manaflow=
"The Mana flow skill tells how closely your mind and the forces of the "
"Universe are tied together. You regenerate magic points faster on high "
"levels of this skill.";

const char *skilldesc_healing=
"The skill of healing tells how well you can take care your wounds. "
"Every being has it's own healing rate, but this skill will improve it if"
"the skill is in your knowledge.";

const char *skilldesc_findweakness=
"With this skill you can find weak spots from your enemies. High skill "
"causes more critical hits.";

const char *skilldesc_searching=
"\007The searching skill helps you to find hidden items, traps and other "
"special locations. "
"The skill will be used when you issue the search command.\n\n"
"You can also move with care and be more observant. To do this, turn the "
"searching mode on. This will slow you down, but increases the chances for"
" you to find something hidden. (Use the command \001:s\007).";

const char *skilldesc_disarmtrap=
"The skill makes it possible to disarm traps whose location is known to you."
" If you fail to disarm a trap, it will most probably activate and cause "
"much unhappiness for you and your body. You've been warned.";

/* MAGIC SKILL DESCRIPTIONS */
const char *skilldesc_spellteleport=
"Teleporting spell creates a temporary dimension door allowing items and/or "
"creatures to move between different planes of existence (locations).\n\n"
"The power of the teleporting effect depends on your skill level, "
"higher levels allow you to use teleporting on selected targets as well "
"as on yourself.";

const char *skilldesc_spellhealing=
"Healing spell makes it possible to heal wounded people, creatures and "
"yourself. ";

const char *skilldesc_spellidentify=
"Identify is a very useful and powerfull spell. It allows you to reveal "
"the real identity of unknown items. It goes without saying "
"that it can be used to reveal cursed items before using them.";

const char *skilldesc_spellbless=
"Bless spell allows you to bless items. Blessed item can be more powerful in "
"some situations. Higher levels of the bless spell allows you to bless "
"creatures as well.";

const char *skilldesc_spellcurse=
"Curse spell is the exact and violent opposite of bless spell. It allows "
"you to curse items and/or creatures depending on your skill level.";

const char *skilldesc_spelluncurse=
"The spell of remove curse is often a spell of relief. It makes possible "
"to uncurse items and/or creatures.";

const char *skilldesc_spellconfuze=
"This spell allows you to temporarily confuze other creatures, thus making "
"them a better target for your attacks. This cloud of confuzion can also be "
"used for fleeing the battle area, if so desired.\n\n"
"Higher skill level makes it possible to select your target more freely.";


const char *skillgroupnames[]=
{
   "all",
   "weapon",
   "generic",
   "magic",
   NULL
};


Tskilltype skills_weapon[]=
{
   { "weaponless combat", skilldesc_weaponless, SKILLAUTO },
   { "daggers", skilldesc_daggers, SKILLAUTO },
   { "swords", skilldesc_swords, SKILLAUTO },
   { "axes", skilldesc_axes, SKILLAUTO },
   { "blunt weapons", skilldesc_blunt, SKILLAUTO },
   { "polearms", skilldesc_polearm, SKILLAUTO },
   { "staffs", skilldesc_staff, SKILLAUTO },
   { "bows", skilldesc_bow, SKILLAUTO },
   { "crossbows", NULL, SKILLAUTO },
   { "throwing", skilldesc_throw,  SKILLAUTO },
   { "two handed weapons", skilldesc_twohanded, SKILLAUTO },
   { "one handed weapons", skilldesc_onehanded, SKILLAUTO },
   { "two weapon combat", skilldesc_twoweapon, SKILLAUTO },
   { "shield", skilldesc_shield, SKILLAUTO },

   { NULL, NULL, 0 }
};

Tskilltype skills_general[]=
{
   { "food gathering", skilldesc_foodgather, SKILLAUTO },

   /* magic group skills */
   { "observation", skilldesc_observation, SKILLAUTO },
   { "alteration", skilldesc_alteration, SKILLAUTO },
   { "mysticism", skilldesc_mysticism, SKILLAUTO },
   { "destruction", skilldesc_destruction, SKILLAUTO },
   { "concentration", skilldesc_concentration, SKILLAUTO },
   { "mana flow", skilldesc_manaflow, SKILLAUTO },
   { "healing", skilldesc_healing, SKILLAUTO },
   { "find weakness", skilldesc_findweakness, SKILLAUTO },
   { "searching", skilldesc_searching, 0 },
   { "disarm trap", skilldesc_disarmtrap, 0 },


   /* skills not used yet below */
   { "Survival", NULL, SKILLAUTO },
   { "Haggling", NULL, SKILLAUTO },
   { "Farming", NULL, SKILLAUTO },
   { "Hunting", NULL, SKILLAUTO },
   { "Preservation", NULL, SKILLAUTO },
   { "Mining", NULL, SKILLAUTO },
   { "Observation", NULL, SKILLAUTO },
   { "Swimming", NULL, SKILLAUTO },
   { "Climbing", NULL, SKILLAUTO },
   { "Hiding", NULL, SKILLAUTO },
   { "Sneaking", NULL, SKILLAUTO },
   { "Concentration", NULL, SKILLAUTO },
   { "Dodging", NULL, SKILLAUTO },
   { NULL, NULL, 0 }
};

Tskilltype skills_magic[]=
{
   { list_spells[SPELL_HEALING].name, skilldesc_spellhealing, 0 },
   { list_spells[SPELL_TELEPORT].name, skilldesc_spellteleport, 0 },
   { list_spells[SPELL_IDENTIFY].name, skilldesc_spellidentify, 0 },
   { list_spells[SPELL_BLESS].name, skilldesc_spellbless, 0 },
   { list_spells[SPELL_CURSE].name, skilldesc_spellcurse, 0 },
   { list_spells[SPELL_UNCURSE].name, skilldesc_spelluncurse, 0 },
   { list_spells[SPELL_CONFUZE].name, skilldesc_spellconfuze, 0 },
   { NULL, NULL, 0 }
};

Tskilltype skills_illegal[]=
{
   {"Illegal skill group!", 0 },
   { NULL, 0 }
};



/*******************************/
/*                             */
/* ROUTINES FOR SKILL HANDLING */
/*                             */
/*******************************/

bool skill_init(Tskillpointer *list)
{
     if(!list)
	  return false;

     *list = NULL;
     return true;
}

void init_spellskills(void)
{
   int16s i=0;

   while(skills_magic[i].name != NULL) {
      skills_magic[i].flags = list_spells[i].flags;
      i++;
   }
}

Tskillpointer skill_addnew(Tskillpointer *list, int16u group, 
                           int16u type, int16s value)
{
   Tskillpointer nptr;
   bool alreadyin;

   alreadyin=false;

   /* first check if the skill already exists */
   nptr=*list;
   while(nptr) {
      if(group==nptr->s.group && type==nptr->s.type) {
	 alreadyin=true;
	 break;
      }
      nptr=nptr->next;
   }

   /* don't add the skill if it already exists */
   if(!alreadyin) {
      nptr=new Tskillnode;

      if(!nptr) {
	 msg.newmsg("Can't add a new skill!", CHB_RED);
	 return NULL;
      }

      /* init the fields */
      nptr->s.group=group;
      nptr->s.type=type;
      nptr->s.cur=value;
      nptr->s.ini=value;
      nptr->s.level=0;
      nptr->s.raise=0;
//              nptr->s.rindex=0;
      nptr->s.max=MAX_SKILLSCORE;
      nptr->s.min=MIN_SKILLSCORE;
      nptr->s.dice_t=1;     /* 1d6 */
      nptr->s.dice_s=6;

      nptr->next=NULL;

      if(!(*list)) {
	 *list=nptr;
      }
      else {
	 nptr->next=*list;
	 *list=nptr;
      }
   }
//      else
//      skill_modify(nptr, value);

   return nptr;
}

/* modify the value */
bool skill_modify(Tskillpointer *skills, int16u group, int16u type, int16s value, bool addit)
{
//      msg.newmsg("Skill already exists!", C_RED);
   Tskillpointer sptr;
   bool sis;

   sptr=*skills;

   sis=false;
   while(sptr) {
      if(sptr->s.group==group && sptr->s.type==type) {
	 sis=true;
	 break;
      }
      sptr=sptr->next;
   }

   /* add the skill if it doesn't exist and addition is enabled */
   if(!sis && addit) {
      sptr=skill_addnew(skills, group, type, value);
      if(sptr)
	 return true;
      return false;
   }

   if(sptr && sis) {
      sptr->s.cur+=value;

      if(sptr->s.cur < sptr->s.min)
	 sptr->s.cur=sptr->s.min;
      if(sptr->s.cur > sptr->s.max)
	 sptr->s.cur=sptr->s.max;
      return true;
   }

   return false;
}

/*
 * returns the needed skillmarks for specific skill group
 */
int16u skill_needmarks(int16u group, int16u value)
{
   int16u need;

   need=SKILL_ADV[value];

   if(group==SKILLGRP_MAGIC)
      need=need / 3;
   else if(group==SKILLGRP_GENERIC)
      need=need / 2;

   return need;
}


/* modify the raise counter of skill */
/* will add the skill if doesn't exist */
/* if addval>0 then skill will be added if there isnt yet */
bool skill_modify_raise(Tskillpointer *skills, int16u group, 
                        int16u type, int16u amount, int16u addval)
{
   Tskillpointer sptr;
   bool sis, skilladd=false;

   const char *nameptr;

   sptr=*skills;

   sis=false;
   while(sptr) {
      if(sptr->s.group==group && sptr->s.type==type) {
	 sis=true;
	 break;
      }
      sptr=sptr->next;
   }

   if(!sis && addval>0) {
      sptr=skill_addnew(skills, group, type, addval);
      if(!sptr)
//	 return true;
	 return false;

      skilladd=true;
      sis=true;
   }

   if(sptr->s.group==SKILLGRP_WEAPON)
      nameptr=skills_weapon[sptr->s.type].name;
   else if(sptr->s.group==SKILLGRP_MAGIC)
      nameptr=skills_magic[sptr->s.type].name;
   else if(sptr->s.group==SKILLGRP_GENERIC)
      nameptr=skills_general[sptr->s.type].name;
   else {
      nameptr=skills_illegal[0].name;
   }

   if(skilladd && (skills == &player.skills) && !global_initmode) {
//      sprintf(tempstr, "You gained a new %s skill %s!", 
//	      skillgroupnames[sptr->s.group], nameptr);
//      msg.newmsg(tempstr, CH_GREEN);
      msg.vnewmsg(CH_GREEN, "You gained a new %s skill %s!", 
		  skillgroupnames[sptr->s.group], nameptr);
   }

   if( sis ) {

      if(sptr->s.cur>=MAX_SKILLSCORE)
	 return true;

      /* increase skill learn counter */
      sptr->s.raise+=amount;

      /* check for skill advance */
//      if(sptr->s.raise>= skill_needmarks(group, sptr->s.cur)) {
      while( sptr->s.raise >= skill_needmarks(group, sptr->s.cur)) {

	 /* reset mark count */
	 sptr->s.raise-=skill_needmarks(group, sptr->s.cur);

	 /* uppgrade the skill */
	 sptr->s.cur+=throwdice(sptr->s.dice_t, sptr->s.dice_s, 0);

	 if(sptr->s.cur>=MAX_SKILLSCORE)
	    sptr->s.cur=MAX_SKILLSCORE;

	 /* if player then show a nice message */
	 if(skills==&player.skills && !global_initmode) {

//	    sprintf(tempstr, "You feel more confident with %s.", nameptr);
//	    msg.newmsg(tempstr, CH_GREEN);
	    msg.vnewmsg(CH_GREEN, "You feel more confident with %s.", nameptr);
	 }
      }

      return true;
   }

   return false;
}

int16u skill_check(Tskillpointer list, int16u group, int16u type)
{
   int16u ret;

   ret=0;

   while(list) {
      if(list->s.group == group && list->s.type==type) {
	 ret=list->s.cur;
	 if(ret<list->s.min)
	    ret=list->s.min;
	 if(ret>list->s.max)
	    ret=list->s.max;

	 return ret;
      }
      list=list->next;
   }

   return ret;
}


bool skill_list(Tskillpointer list, int16u group)
{
   Tskillpointer sptr;
   Tskilltype *skillptr;
//   const char *nameptr;
   int16u scount, grp, skl;

   if(!list) {
      msg.newmsg("You don't know any special skills.", C_WHITE);
      return false;
   }

   scount=0;
   if(group!=SKILLGRP_ALL)
   {
      sptr=list;
      while(sptr) {
	 if(group==list->s.group)
	    scount++;
	 sptr=sptr->next;
      }

      if(!scount) {
	 sprintf(tempstr, "You don't know any %s skills.", skillgroupnames[group]);
	 msg.newmsg(tempstr, C_WHITE);
	 return false;
      }

   }

   my_clrscr();

   my_setcolor(C_WHITE);
   sprintf(tempstr, "%s skills you've knowledge of", skillgroupnames[group]);
   my_cputs(1, tempstr);
   my_setcolor(C_YELLOW);
   drawline(2, '-');
   my_setcolor(C_WHITE);
   my_printf("\n");

   while(list) {
      grp=list->s.group;
      skl=list->s.type;

      if(group>0 && group!=grp) {
	 list=list->next;
	 continue;
      }

      if(grp==SKILLGRP_WEAPON)
	 skillptr=&skills_weapon[skl];
      else if(grp==SKILLGRP_MAGIC)
	 skillptr=&skills_magic[skl];
      else if(grp==SKILLGRP_GENERIC)
	 skillptr=&skills_general[skl];
      else {
	 skillptr=&skills_illegal[0];
      }

      
      my_printf("%18s - %-11s [%3d%%]\n",
		skillptr->name, 
		skilllevelnames[list->s.cur/10], 
		list->s.cur);
      list=list->next;
   }
   showmore(false, false);

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

   return true;
}

bool skill_removeall(Tskillpointer *list)
{
   Tskillpointer sptr, del;

   sptr=*list;

   if(!sptr)
      return true;

   while(sptr) {
      del=sptr;
      sptr=sptr->next;

      delete del;
   }
   *list=NULL;

   return true;
}

/*
 * Apply a skill
 *
 */
bool skill_apply(Tskillpointer list)
{


   return true;
}

const char *txt_listinstru=
"\007To browse, use \002UP/DOWN \007or \002A/Z \007keys. Press "
"\002?\007 to get skill description. Pressing "
"\002ENTER \007or \002SPACE \007 selects the highlighted skill.\n";

const char *txt_listinstru2=
"\007Tag/untag quick skills with keys\001 0\007 -\001 9\007. Pressing "
"\001C\007 clears all quick skills. "
"\002ESC \007or \002Q \007exits without selection.\n";

const char *txt_skillauto=
"\007This is an \003automatic \007skill. It is used automatically in correct "
"situations if you have learned it. This skill can't be selected.\n";

int16s skill_listselect(int16u group, char *prompt)
{
   int8u box_sy, box_sx;
   int8u box_by, box_bx;

   Tskilltype *skilllist;

   bool endreach=false;
   bool topreach=false;
   bool endoflist=false;

   int16s eoffs, soffs, loffs=0, i, sel=0;
   int16u ch;

   if(group==SKILLGRP_WEAPON)
      skilllist=skills_weapon;
   else if(group==SKILLGRP_MAGIC)
      skilllist=skills_magic;
   else if(group==SKILLGRP_GENERIC)
      skilllist=skills_general;
   else {
      skilllist=skills_illegal;
   }

   Tskilltype *sptr;

   box_sy=11;  /* size y */
   box_sx=20;  /* size x */

   box_bx=2;
   box_by=2;

   char skillname[25];

   my_clrscr();
   my_setcolor(C_WHITE);

   if(!skilllist) {
      my_printf("Error! No skill list provided for skill_listselect().\n");
      showmore(false, false);
      return -1;
   }

   hidecursor();
   my_setcolor(C_GREEN);
   makeborder(box_bx-1, box_by-1, box_sx+2, box_sy+2);

   if(prompt) {
      my_wordwraptext(prompt, box_by, SCREEN_LINES, (box_bx+box_sx+2),
		      SCREEN_COLS);
   }

   my_wordwraptext(txt_listinstru, box_by+box_sy+2, SCREEN_LINES, box_bx,
		   SCREEN_COLS);


   loffs=soffs=0-(box_sy/2);
   eoffs=box_sy;

   while(1) {
      sptr=skilllist+loffs;

      endreach=false;

      for(i=0; i<box_sy; i++, sptr++) {

	 if((loffs+i)<0)
	    topreach=true;
	 else
	    topreach=false;

	 skillname[0]=0;

	 if(!topreach) {
	    if(!sptr->name) {
	       endreach=true;
	       if(!endoflist)
		  eoffs=loffs+box_sy/2 - 1;
	       endoflist=true;
	    }

	    if(!endreach) {
	       my_strcpy(skillname, sptr->name, sizeof(skillname));
	       if(strlen(skillname) > box_sx)
		  my_strcpy(skillname+(box_sx-3), "...", sizeof(skillname));
	       skillname[0]=toupper(skillname[0]);
	    }


	 }

	 if(i==box_sy/2)
	    my_setcolor(CH_YELLOW);
	 else
	    my_setcolor(CH_DGRAY);

	 my_gotoxy(box_bx, box_by+i);
	 my_printf("%20s", "");
	 my_gotoxy(box_bx+((box_sx/2)-strlen(skillname)/2), box_by+i);
	 my_printf("%s", skillname);

	 
      }

      my_gotoxy(box_bx+box_sx+2, box_bx+box_sy-1);
      my_setcolor(C_GREEN);
      my_printf("%s skill ", skillgroupnames[group]);

      if(skilllist[sel].flags & SPF_ALTERATION)
	 my_printf("of alteration");
      if(skilllist[sel].flags & SPF_DESTRUCTION)
	 my_printf("of destruction");
      if(skilllist[sel].flags & SPF_MYSTICISM)
	 my_printf("of mysticism");
      if(skilllist[sel].flags & SPF_OBSERVATION)
	 my_printf("of observation");

      if(skilllist[sel].flags & SKILLAUTO) {
	 my_setcolor(CH_RED);
	 my_printf("*Automatic*");
      }
      my_clr2eol();
      

      ch=my_getch();

      if( (ch=='z' || ch==KEY_DOWN || ch=='2') && loffs<eoffs ) {
	 loffs++;
	 sel++;
      }
      if( (ch=='a' || ch==KEY_UP || ch=='8') && loffs>soffs) {
	 loffs--;
	 sel--;
      }
      if(ch==32 || ch==KEY_ENTER || ch==PADENTER || ch==10)
	 break;

      if( ch=='?' ) {
	 /* erase description area first */
	 for(i=0; i<box_sy; i++)
	    drawline_limit(box_by+i, box_bx+box_sx+2, SCREEN_COLS, ' ');

	 my_setcolor(C_WHITE);
	 my_gotoxy(box_bx+box_sx+2, box_by);
	 if((skilllist+sel)->desc) {
	    my_wordwraptext((skilllist+sel)->desc, 
			    box_by, SCREEN_LINES, (box_bx+box_sx+2),
			    SCREEN_COLS);

	 }
	 else
	    my_printf("No description for the skill.");

      }


   }

   showcursor();

   return sel;
}


sint skill_compare(const Tskill *s1, const Tskill *s2)
{
   int16s res;

   res = s2->cur - s1->cur;

   return res;
}

int16u build_skillarray(Tskillpointer skills, int16u group, Tskill **pointer)
{
   Tskillpointer sptr;
   Tskill *newlist=NULL;

   int16u num=0;

   if(!skills)
      return 0;

   sptr=skills;

   /* count the skills */
   while(sptr) {
      if(group != SKILLGRP_ALL) {
	 if(group == sptr->s.group)
	    num++;
      }
      else
	 num++;

      sptr=sptr->next;
   }

   if(!num)
      return 0;

   newlist=new Tskill[num+1];

   if(!newlist)
      return 0;

   /* mark the end of array */
   newlist[num].group=SKILLGRP_ENDLIST;
   newlist[num].type=SKILL_ENDLIST;

   /* build the array */
   sptr=skills;
   num=0;
   while(sptr) {      
      if(group != SKILLGRP_ALL) {
	 if(group == sptr->s.group) {
	    newlist[num]=sptr->s;
	    num++;
	 }
      }
      else {
	 newlist[num]=sptr->s;
	 num++;
      }
      sptr=sptr->next;
   }

   /* sort the skill list */
   qsort(newlist, num, sizeof(Tskill), skill_compare);

   *pointer = newlist;

   return num;
}

int16s skill_listselect2(Tskillpointer skills, int16u *group, char *prompt)
{
   int8u box_sy, box_sx;
   int8u box_by, box_bx;

   bool endreach=false;
   bool topreach=false;
   bool endoflist=false;
   bool eraseneeded=false;

   int16s eoffs, soffs, loffs=0, i, j, sel=0;
   int16u ch, stype=0, num_skills=0;

   int16u listcolor;

   Tskill *sptr, *ptrlist=NULL;
   Tskilltype *origlist=NULL;

   box_sy=14;  /* size y */
   box_sx=24;  /* size x */

   box_bx=2;
   box_by=2;

   char skillname[30];

   sel=*group;

   if(sel > SKILLGRP_MAGIC) {
      sprintf(tempstr, "skill_listselect(): Trying to pass illegal skill group number (%d).", sel);
      return -1;
   }

   num_skills = build_skillarray(skills, sel, &ptrlist);

   if(!ptrlist || !num_skills) {
      sprintf(tempstr, "You don't know any %s skills.", 
	      skillgroupnames[*group]);
      msg.newmsg(tempstr, C_WHITE);
      return -1;
   }

   my_clrscr();
   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

   sel = 0;

   hidecursor();
   my_setcolor(C_GREEN);
   makeborder(box_bx-1, box_by-1, box_sx+2, box_sy+2);

   if(prompt) {
      my_wordwraptext(prompt, box_by, SCREEN_LINES, (box_bx+box_sx+2),
		      SCREEN_COLS);
   }

   my_wordwraptext(txt_listinstru, box_by+box_sy+1, SCREEN_LINES, box_bx,
		   SCREEN_COLS);
   my_wordwraptext(txt_listinstru2, my_gety()+1, SCREEN_LINES, box_bx,
		   SCREEN_COLS);

   loffs=soffs=0-(box_sy/2);
   eoffs=box_sy;

   listcolor=CH_DGRAY;

   while(1) {
      sptr=ptrlist+loffs;

      endreach=false;

      for(i=0; i<box_sy; i++, sptr++) {

	 /* test for array start boundary */
	 if((loffs+i)<0)
	    topreach=true;
	 else
	    topreach=false;

	 skillname[0]=0;

	 if(!topreach) {
	    /* test for array end boundary */
	    if(sptr->group==SKILLGRP_ENDLIST || sptr->type==SKILL_ENDLIST) {
	       endreach=true;
	       if(!endoflist)
		  eoffs=loffs+box_sy/2 - 1;
	       endoflist=true;
	    }

	    if(!endreach) {
	       stype=sptr->type;
	       if(sptr->group==SKILLGRP_WEAPON)
		  origlist=skills_weapon;
	       else if(sptr->group==SKILLGRP_MAGIC)
		  origlist=skills_magic;
	       else if(sptr->group==SKILLGRP_GENERIC)
		  origlist=skills_general;
	       else {
		  origlist=skills_illegal;
		  stype=0;
	       }

	       skillname[0]=0;
	       for(j=0; j<NUM_QUICKSKILLS; j++) {
		  if(player.qskills[j].group == sptr->group &&
		     player.qskills[j].type == sptr->type &&
		     player.qskills[j].select != 0)
		     sprintf(skillname, "(%d) ", j);
	       }

	       if(origlist[stype].flags & SKILLAUTO)
		  listcolor=CH_DGRAY;
	       else
		  listcolor=C_WHITE;

	       strcat(skillname, origlist[stype].name);
	       if(strlen(skillname) > box_sx)
		  my_strcpy(skillname+(box_sx-3), "...", sizeof(skillname));
	       skillname[0]=toupper(skillname[0]);
	    }

	 }

	 if(i==box_sy/2)
	    my_setcolor(CH_YELLOW);
	 else {
	    my_setcolor(listcolor);
	 }
//	 my_gotoxy(box_bx, box_by+i);
	 drawline_limit(box_by+i, box_bx, box_bx+box_sx-1, ' ');
	 my_gotoxy(box_bx+((box_sx/2)-strlen(skillname)/2), box_by+i);
	 my_printf("%s", skillname);
	 
      }

      /* show some info */
      stype=ptrlist[sel].type;
      if(ptrlist[sel].group==SKILLGRP_WEAPON)
	 origlist=skills_weapon;
      else if(ptrlist[sel].group==SKILLGRP_MAGIC)
	 origlist=skills_magic;
      else if(ptrlist[sel].group==SKILLGRP_GENERIC)
	 origlist=skills_general;
      else {
	 origlist=skills_illegal;
	 stype=0;
      }

      my_gotoxy(box_bx+box_sx+2, box_by+box_sy-3);

      my_setcolor(CH_GREEN);
      my_printf("%s skill ", skillgroupnames[ptrlist[sel].group]);

      if(origlist[stype].flags & SPF_ALTERATION)
	 my_printf("of alteration");
      if(origlist[stype].flags & SPF_DESTRUCTION)
	 my_printf("of destruction");
      if(origlist[stype].flags & SPF_MYSTICISM)
	 my_printf("of mysticism");
      if(origlist[stype].flags & SPF_OBSERVATION)
	 my_printf("of observation");

      if(origlist[stype].flags & SKILLAUTO) {
	 my_setcolor(CH_RED);
	 my_printf("*Automatic*");
      }
      my_clr2eol();

      my_gotoxy(box_bx+box_sx+2, box_by+box_sy-2);
      my_setcolor(C_WHITE);
      my_printf("Knowledge: %d%% of %d%% (%s)", 
		ptrlist[sel].cur, ptrlist[sel].max,
		skilllevelnames[ptrlist[sel].cur/10]);
      my_clr2eol();
      my_gotoxy(box_bx+box_sx+2, box_by+box_sy-1);
      my_printf("Needs %d marks to advance!",
		(skill_needmarks(ptrlist[sel].group, ptrlist[sel].cur) - 
		ptrlist[sel].raise) );
      my_clr2eol();
      
      ch=my_getch();

      /* erase description area first */
      if(eraseneeded) {
	 for(i=0; i<box_sy; i++)
	    drawline_limit(box_by+i, box_bx+box_sx+2, SCREEN_COLS, ' ');
	 eraseneeded=false;
      }

      if( (ch=='Z' || ch=='z' || ch==KEY_DOWN ) && sel<(num_skills-1) ) {
	 loffs++;
	 sel++;
      }
      if( (ch=='A' || ch=='a' || ch==KEY_UP ) && sel>0) {
	 loffs--;
	 sel--;
      }

      if( ch>='0' && ch<='9') {
	 if(origlist[stype].flags & SKILLAUTO) {
	    eraseneeded=true;
	    my_wordwraptext(txt_skillauto, 
			    box_by, SCREEN_LINES, (box_bx+box_sx+2),
			    SCREEN_COLS);	    
	 }
	 else {
	    for(j=0; j<NUM_QUICKSKILLS; j++) {
	       if(player.qskills[j].group == ptrlist[sel].group &&
		  player.qskills[j].type == ptrlist[sel].type &&
		  (ch-'0' != j))
	       {
		  player.qskills[j].select=0;
	       
	       }
	    }
	    j=ch-'0';
	    if(j < NUM_QUICKSKILLS) {
	       if(player.qskills[j].group == ptrlist[sel].group &&
		  player.qskills[j].type == ptrlist[sel].type &&
		  player.qskills[j].select != 0 ) 
	       {
		  player.qskills[j].select = 0;
	       }
	       else {
		  player.qskills[j].select = 1;
		  player.qskills[j].group = ptrlist[sel].group;
		  player.qskills[j].type = ptrlist[sel].type;
	       }
	    }
	 }
      }

      if(ch=='c' || ch=='C') {
	 for(j=0; j<NUM_QUICKSKILLS; j++) {
	    player.qskills[j].group = 0;
	    player.qskills[j].type = 0;
	    player.qskills[j].select=0;
	 }

      }
      if(ch==32 || ch==KEY_ENTER || ch==PADENTER || ch==10) {
	 if(origlist[stype].flags & SKILLAUTO) {
	    eraseneeded=true;
	    my_wordwraptext(txt_skillauto, 
			    box_by, SCREEN_LINES, (box_bx+box_sx+2),
			    SCREEN_COLS);	    
	 }
	 else
	    break;
      }

      if(ch==KEY_ESC || ch=='q' || ch=='Q') {
	 sel=-1;
	 break;
      }

      if( ch=='?') {
	 eraseneeded=true;

	 my_setcolor(C_WHITE);
	 my_gotoxy(box_bx+box_sx+2, box_by);

	 if(origlist[stype].desc) {
	    my_wordwraptext(origlist[stype].desc, 
			    box_by, SCREEN_LINES, (box_bx+box_sx+2),
			    SCREEN_COLS);
	 }
	 else
	    my_printf("No description for the skill.");

      }


   }

   showcursor();

   if(sel<0) {
      *group=0;
      msg.newmsg("No skill selected.", C_WHITE);
   }
   else {
      *group=ptrlist[sel].group;
      sel=ptrlist[sel].type;
   }

   /* free the temporary array */
   if(ptrlist)
      delete [] ptrlist;

   /* it should return <0 if no skill selected
    * if skill is selected, return the skill type number and
    * place skill group number into *group (parameter given)
    */

   return sel;
}

/* ************************************************************** */
/* *                                                            * */
/* ************************************************************** */


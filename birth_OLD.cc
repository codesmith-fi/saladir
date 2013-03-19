/*
**	Player birth routines for Saladir
**
*/

#include "saladir.h"

#include "birth.h"
#include "classes.h"
#include "dice.h"
#include "skills.h"

#include "gametime.h"

_questiondef birthquestions[]=
{
   {
      "It was a stormy night, you were reading a very interesting book about "
      "the history of Salmorria. Your father came and asked you to help him and a group of "
      "farmers to kill a beast which had been terrorizing the country and killed much "
      "cattle. You...",
      false, 
      {
	 { 
	    "didn't help because you're too busy reading the book.",
	    -4, -2, -4, 4,
	    4, -2, 0, 0,
	    0, 
	 }, 
	 {
	    "agreed to help your father even there was such a dog weather outside.",
	    4, 2, 4, -4,
	    -4, 2, 1, 0,
	    500, 
	 },
	 { 
	    "lied to your father and told that you have an twisted ankle and can't help much out there.",
	    -6, -3, -6, 6,
	    6, -3, -1, 0,
	    -500, 
	 },
	 { NULL }
      }
   },
   {
      "One day you were quietly walking towards your home and suddenly you were "
      "faced with a ugly looking robber who demanded you to hand all your money "
      "and other belongings to him. You... ",
      false,
      {
	 {
	    "tried to run away and escaped the situation.",
	    -1, 4, -1, 0,
	    0, 0, 1, 2,
	    0, 
	 },
	 {
	    "fought with the bastard and by killing him did a great service to the sherif of nearby town.",
	    1, 1, 1, 0,
	    0, 0, 2, 1,
	    500, 2, 0
	 },
	 {
	    "gave all your money to him in hope of a peaceful escape.",
	    -1, -1, -1, 0,
	    2, -1, 4, -2,
	    0, 
	 },
	 {
	    "cold bloodely killed the robber and after mutilating his corpse badly you took all his money.",
	    2, 2, 1, -2,
	    -2, 1, -8, -1,
	    -500, 
	 },
	 { NULL }
      }
   },
   {
      "In your youth you liked lot to...",
      false,
      {
	 {
	    "have fun and party with your friends.",
	    -2, -2, -2, -2,
	    -2, 1, 6, 0,
	    0, 
	 },
	 {
	    "examine other people and find their weaknesses.",
	    -2, -2, -2, 2,
	    2, 2, 0, 0,
	    -500, 
	 },
	 {
	    "hunt and examine the nature.",
	    -1, 4, 2, 2,
	    -4, 2, -6, 1,
	    500, 
	 },
	 {
	    "help your family with the duties at your home.",
	    2, -2, 4, -4,
	    -2, 2, 2, -1,
	    500, 
	 },
	 { NULL }
      }	 
   },
   {		
      "One morning in the nearby town you noticed a young slender boy which "
      "was being brutally teased by a group of other youngsters. You...",
      false,
      {
	 {
	    "went to help the teased one.",
	    2, 2, 2, 1,
	    -2, 2, 4, 0,
	    500, 
	 },
	 {
	    "quietly walked away fearing that the group would start teasing you instead.",
	    -2, -2, -2, -1,
	    3, -4, -2, 0,
	    0, 
	 },
	 {
	    "joined the group of teasers and did your best to hurt the boy.",
	    1, 1, 4, -3,
	    -2, 3, -6, 0,
	    -500, 
	 },
	 { NULL }
      }	
   },
   { 
      "In your youth, you ... ",
      false,
      {
	 {		
	    "lived in the country and your father was a farmer.",
	    2, 2, 2, -4,
	    -2, 2, -4, 0,
	    0, 
	 },
	 {
	    "lived in a castle where your family worked as servants.",
	    1, 4, 1, -1,
	    1, 2, 4, 0,
	    0, 
	 },
	 {
	    "lived in a castle and your family were nobles.",
	    -2, 1, -2, 2,
	    2, -3, 6, 2,
	    0, 
	 },
	 {
	    "lived a city and your father worked in a library.",
	    -2, -2, -2, 4,
	    2, -3, -4, 0,
	    0, 
	 },
	 {
	    "lost your parents early and were forced to earn your living.",
	    4, 4, 4, 2,
	    2, 3, 3, -2,
	    0, 
	 },
	 { NULL }
      }
   },
   {
      "Thinking about your friends, you had... ",
      false,
      {
	 {
	    "only few friends who were very important.",
	    1, 1, 1, 1,
	    1, 2, 3, 0,
	    0, 
	 },
	 {
	    "no friends at all.",
	    0, 0, 4, -3,
	    -3, 2, -6, -2,
	    -500, 
	 },
	 {
	    "many friends and you were liked by almost everybody.",
	    0, 0, -4, -3,
	    -3, -2, 6, 0,
	    500, 
	 },
	 { NULL }
      }
   },   
   {
      "Thinking about your near ancestors. There has been several cases of... ",
      false, 
      {
	 {
	    "people dying in hemiplegia (paralysation)",
	    -8, -2, -4, 0,
	    0, 0, 0, 0,
	    0, 
	 },
	 {
	    "people with badly deformed limbs.",
	    -4, -8, -2, 0,
	    0, 0, 0, 0,
	    0, 
	 },
	 {
	    "mental retardion.",
	    0, 0, 0, -8,
	    -4, 0, -2, 0,
	    0, 
	 },
	 {
	    "severe mental illnesses.",
	    0, 0, 0, -4,
	    -8, 0, -2, 0,
	    0, 
	 },
	 {
	    "deaths caused by a heart failure",
	    -4, 0, -2, 0,
	    0, -8, 0, 0,
	    0, 
	 },
	 {
	    "badly deformed heads. One person got a name: \"elephant man\".",
	    -2, 0, -4, 0,
	    0, 0, -8, 0,
	    0, 
	 },	   
	 { NULL }
      }
   },
   { 
      "As a gift for your 20th birthday, you received 1000 gold coins from your father. "
      "You decided to...",
      false,
      {
	 {
	    "save all the money for future.",
	    0, 0, 0, 2,
	    4, 0, -4, 0,
	    0, 
	 },
	 {
	    "arrange a great party for all your friends.",
	    0, 0, 0, -2,
	    -4, 0, 4, -2,
	    0, 
	 },
	 {
	    "buy many new books you had seen in the nearby town.",
	    0, 0, 0, 4,
	    2, 0, -4, 0,
	    0, 
	 },
	 {
	    "donate half of the money to the church and save the rest.",
	    0, 0, 0, 0,
	    2, 0, 2, 2,
	    500, 
	 },
	 { NULL }
      }
   },
   { 
      "This one is simple. Would you consider yourself as...",
      false,
      {
	 {
	    "a bad person.",
	    0, 0, 0, 0,
	    0, 0, 0, 0,
	    -500, 
	 },
	 {
	    "a person who doesn't care much about others.",
	    0, 0, 0, 0,
	    0, 0, 0, 0,
	    0, 
	 },
	 {
	    "a good and always helping type person.",
	    0, 0, 0, 0,
	    0, 0, 0, 0,
	    500, 
	 },
	 { NULL }
      }
   },
   { 
      "How big was your family?",
      false,
      {
	 {
	    "I was the only child...",
	    0, 0, 0, 0,
	    0, 0, 0, -1,
	    0, 
	 },
	 {
	    "I had one sister/brother.",
	    0, 0, 0, 0,
	    0, 0, 0, 2,
	    0, 
	 },
	 {
	    "I had a few sisters/brothers.",
	    0, 0, 0, 0,
	    0, 0, 0, 0,
	    0, 
	 },
	 {
	    "I had absolutely too many irritating sisters/brothers.",
	    0, 0, 0, 0,
	    0, 0, 0, 0,
	    0, 
	 },
	 { NULL }
      }
   },
   { NULL }   
};
   
char *desctxt_str[]=
{
   "",
   "",
   "",
   "",
   "",
   "Normal",
   "",
   "",
   "Very strong",
   "Superhuman",
   "Immortal",
	NULL
};

char welcometext[]=
	"\007Welcome to the land of \002Salmorria\007, stranger. You're just "
   "a few steps away from the adventure, gold and fortune. You're ready "
   "to go after the Legend of Saladir...\n"
//   "But most probably the death itself "
//   "is waiting for you to pay him a nice visit in his Manor of Death. \n\n"
   "But before anything, please select a name for yourself or press enter "
   "and just let the fate decide...\n\n"
	"\0";

char birthtext1[]=
{
   "\7Before you can go adventuring, you must first create a character. "
   "In this version you have two choices for creating your brave adventurer."
   "\n\n\t\1a\7) Answer ten random questions\n"
   "\t\1b\7) Create a random character\n\n"
   "\tChoice ? \0"
};

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
"\007Now pick the secondary weapon skill. You will have a 10% chance on the "
"skill you now select.";

char classtext[]=
"Now select a class for your character. A class is basically "
"your occupation, a field you've been trained/used to. "
"Nearly every creature in this game has a class. "
"And of course, every class has it's own advantages and "
"disadvantages. It's left for you to investigate which class "
"suits you best.\n\n"
"\0";

char *qpromptext[]=
{
   "You still have this last question left!",
   "You're almost done, your shape is forming up!.",
   "Three questions left, you're nearing completion.",
   "Four questions left! don't get bored!",
   "Five questions left! This is important!",
   "Six questions left. Half done already! ",
   "Seven questions left! Now I have a better clue about that!",
   "Eight questions left! Oh, I wonder what you will look like?!",
   "Nine questions left! Don't be to hasty! We are making YOU!",
   "Ten questions left! Read the questions carefully!",
   NULL
};



/* initialize player inventory */
void birth_initpack() {

   inv_additem(&player.inv, IS_MONEY, MONEY_GOLD, 120, -1);

}


void birth_askclass(void)
{
   int16u key=0, i;
   bool select=false;
   
   my_clrscr();
   my_wordwraptext( classtext, 1, SCREEN_LINES, 1, SCREEN_COLS);
   
   my_setcolor(CH_WHITE);
//   my_printf("\tSelect your class from the list.\n\n");
   
   if(num_classes==0) {
      player.pclass=CLASS_NOCLASS;
      return;
   }
   
   my_setcolor(C_WHITE);
   for(i=1; i<num_classes; i++) {

      if(classes[i].initfunc!=NULL) {
	 my_printf("\t[");
	 my_setcolor(CH_RED);
	 my_printf("%c", 'a'+i-1);
	 my_setcolor(C_WHITE);
	 my_printf("] ");
	 my_printf("%s\n", classes[i].name);
      }
//      if(classes[i].initfunc!=NULL)
//	 my_printf(" (*)\n");
//      else my_printf("\n");
   }
   i--;
   my_setcolor(C_WHITE);
   my_printf("\n\tYour selection [%c..%c]? ", 'a', 'a'+i-1);
   while(!select) {
      key=my_getch();
      if( (key>='a') && (key<='a'+i-1) ) {
	 key-='a';
         select=true;
      }
      else if((key>='A') && (key<='A'+i-1) ) {
	 key-='a';
         select=true;
      }
      else {
	 my_gotoxy(1, my_gety());
         my_setcolor(1+RANDU(15));
	 my_printf("\tYour selection [%c..%c]? ", 'a', 'a'+i-1);
      }

      if(select) {
	 if(classes[key+1].initfunc==NULL)
	    select=false;
      }

   }
   player.pclass=key+1;
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
   skill_addnew(&player.skills, SKILLGRP_WEAPON, 
		skill_listselect(SKILLGRP_WEAPON, txt_primaryweapon),
		25);

   skill_addnew(&player.skills, SKILLGRP_WEAPON, 
		skill_listselect(SKILLGRP_WEAPON, txt_secondaryweapon),
		10);
}


void player_born(void)
{
   int16u sel;
   bool askten=false;
   
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
   
   my_setcolor(C_YELLOW);
   my_printf("\n");
   sprintf(tempstr, "%s, are you (m)ale or (f)emale", player.name);
   sel=get_response(tempstr, "mMfF");
   if(sel==0 || sel==1)
      player.gender=SEX_MALE;
   else
      player.gender=SEX_FEMALE;
   
   my_clrscr();
   my_printf("You borned at ");
   worldtime.print_worldtime();
   my_wordwraptext( birthtext1, 2, SCREEN_LINES, 1, SCREEN_COLS);
   
   while(1) {
      sel=my_getch();
      if(tolower(sel)=='a' || sel==PADENTER || sel==KEY_ENTER || sel==10) {
	 askten=true;
	 break;
      }
      if(tolower(sel)=='b') {
	 askten=false;
	 break;
      }
   }
   if(askten)
      birth_askten();
   else {
      
   }

   

   birth_ask_skills();

   my_clrscr();
   my_wordwraptext( txt_titlesel, 3, SCREEN_LINES, 1, SCREEN_COLS);
   my_setcolor(C_YELLOW);
   my_printf("What is your title? ");
   my_setcolor(C_GREEN);
   my_gets(player.title, TITLEMAX);

   if(strlen(player.title)<1)
      my_strcpy(player.title, "the adventurer", TITLEMAX);

   /* init inventory */
   birth_initpack();

/*   
     if(classes[player.pclass].initfunc == NULL) {
     my_printf("Class init function missing! (Not done yet!)\n");
     my_getch();
     }
     else {
     classes[player.pclass].initfunc(&player.skills, &player.inv, player.stat, player.hpp);
}
*/
}

int8u birth_askquestion(_questiondef *ptr)
{
   _Qattrpack *aptr;
   int16u sel;
   int16u i=0;
   
   if(!ptr) return 10;
   
   my_setcolor(C_CYAN);
   my_wordwraptext( ptr->question, 3, SCREEN_LINES, 4, SCREEN_COLS);
   my_printf("\n");
   aptr=ptr->resp;
   while(aptr->txt) {
      my_setcolor(C_RED);
      my_printf("\n\t%c) ", 'A'+i);
      my_setcolor(C_WHITE);
      my_wordwraptext( aptr->txt, my_gety(), SCREEN_LINES, my_getx(), SCREEN_COLS);
      aptr++;
      i++;
   }
   my_setcolor(C_GREEN);
   my_printf("\n\n\tYour pick or (Q)uit? ");
   while(1) {
      sel=tolower(my_getch());
      if(sel=='q')
	 return 1;
      if( sel>='a' && sel < 'a'+i )
	 break;
   }
   aptr=ptr->resp+(sel-'a');

/*   
   player.hp_max+=aptr->hp;
   player.sp_max+=aptr->sp;
   
   player.hp=player.hp_max;
   player.sp=player.sp_max;
*/
   
   change_stat_initial(&player.stat[STAT_STR], aptr->STR, true);
   change_stat_initial(&player.stat[STAT_DEX], aptr->DEX, true);
   change_stat_initial(&player.stat[STAT_TGH], aptr->TGH, true);
   change_stat_initial(&player.stat[STAT_INT], aptr->INT, true);
   change_stat_initial(&player.stat[STAT_WIS], aptr->WIS, true);
   change_stat_initial(&player.stat[STAT_CON], aptr->CON, true);
   change_stat_initial(&player.stat[STAT_CHA], aptr->CHA, true);
   change_stat_initial(&player.stat[STAT_LUC], aptr->LUC, true);
   
   player.align+=aptr->ALIGN;
   
   change_stat_initial(&player.stat[STAT_LUC], BASE_SPEED+DEX_SPEED[player.stat[STAT_DEX].initial], true);
   
//   player.stat[STAT_SPD].initial=BASE_SPEED+DEX_SPEED[player.stat[STAT_DEX].initial];
   
/*
  player.DEX+=aptr->DEX;
  player.TGH+=aptr->TGH;
  player.INT+=aptr->INT;
  player.WIS+=aptr->WIS;
  player.CON+=aptr->CON;
  player.CHA+=aptr->CHA;
*/
   
   return 0;
}

/***

	Syntym„kysymyksill„ voisi m„„r„t„ tietyn m„„r„n jaetavia skillpisteit„

   N„m„ voisi sitten jakaa eri aseskillien kesken ?


***/


bool birth_askten(void)
{
   int8u qnum, askcount;
   
   _questiondef *ptr;
   
   qnum=0;
   ptr=birthquestions;
   while(ptr->question) {
      ptr->asked=false;
      qnum++;
      ptr++;
   }
   
   askcount=BIRTH_ASKCOUNT;
   if(!qnum)
      ERROROUT("player birth -- no birth questions");
   if(qnum < BIRTH_ASKCOUNT)
      askcount=qnum;
   
   for(int i=0; i<askcount; i++) {
      while(1) {
	 ptr=birthquestions+RANDU(qnum);
	 if(!ptr->asked) break;
      }
      ptr->asked=true;
      my_clrscr();
      my_gotoxy(1,1);
      my_printf(qpromptext[askcount-i-1]);
      if(birth_askquestion(ptr))
	 return false;
   }
   
   calculate_hp(player.hpp, player.hp_max, player.prace);
   player.hp_max=player.hp=calculate_totalhp(player.hpp);
   
   return true;
}



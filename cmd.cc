/**************************************************************************
 * cmd.cc --                                                              *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 24.05.1998                                         *
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

#include "variable.h"
#include "player.h"
#include "dungeon.h"
#include "raiselev.h"
#include "search.h"
#include "shops.h"
#include "skills.h"
#include "file.h"
#include "options.h"
#include "textview.h"
#include "cmd.h"

#ifndef SALADIR_RELEASE
#include "damage.h"
#endif

/* cmd prototypes */
void cmd_quit(void);

void cmd_movesouth(void);
void cmd_movenorth(void);
void cmd_moveeast(void);
void cmd_movewest(void);
void cmd_movesoutheast(void);
void cmd_movenortheast(void);
void cmd_movesouthwest(void);
void cmd_movenorthwest(void);

void cmd_walksouth(void);
void cmd_walknorth(void);
void cmd_walkeast(void);
void cmd_walkwest(void);
void cmd_walksoutheast(void);
void cmd_walknortheast(void);
void cmd_walksouthwest(void);
void cmd_walknorthwest(void);
void cmd_downstair(void);
void cmd_upstair(void);


void cmd_qskill_0(void);
void cmd_qskill_1(void);
void cmd_qskill_2(void);
void cmd_qskill_3(void);
void cmd_qskill_4(void);
void cmd_qskill_5(void);
void cmd_qskill_6(void);
void cmd_qskill_7(void);
void cmd_qskill_8(void);
void cmd_qskill_9(void);

void cmd_lastmessage(void);
void cmd_showversion(void);
void cmd_options(void);
void cmd_showhp(void);
void cmd_msgbuffer(void);
void cmd_showtime(void);
void cmd_showtimepassed(void);
void cmd_showquests(void);
void cmd_push(void);
void cmd_redraw(void);
void cmd_rest(void);
void cmd_chat(void);
void cmd_search(void);
void cmd_searchmode(void);
void cmd_useskill(void);
void cmd_cast(void);
void cmd_showac(void);
void cmd_checkbill(void);
void cmd_showkeys(void);
void cmd_showmanual(void);
void cmd_help(void);

void cmd_save(void);
void cmd_load(void);

void cheat_light(void);
void cheat_identify(void);

void editor_onoff(void);
void editor_drawmode(void);
void editor_nextterr(void);
void editor_prevterr(void);
void editor_set(void);
void editor_clear(void);
void editor_visibleoff(void);
void editor_visible(void);
void editor_load(void);
void editor_save(void);

/* debug commands */
#ifndef SALADIR_RELEASE
void debug_createlevel(void);
void debug_hittest(void);
#endif

#define CMD_NOCMD -1
#define CMD_MULTI -2

#define CMD_FEDITOR 0x01
#define CMD_FDEBUG  0x02
#define CMD_FCHEAT  0x04

/*
 * This is a complete list of player usable commands. 
 *
 * CMD_NAME, CMD_KEY_SEQUENCE, FUNCTION_TO_CALL
 *
 * Name is a plain string describing the command
 * Key sequence can contain a sequence of 1-4 keys needed to execute
 *   the command
 * Function to call is of a type "void func(void)"
 *
 */
Tcmd cmdtable[]=
{
   { "Quit without saving", { 'Q', 0, 0, 0, 0 }, cmd_quit },
   { "Equip items/View equipment", { 'q', 0, 0, 0, 0 }, player_equip },
   { "Display inventory", { 'i', 0, 0, 0, 0 }, player_showinv },
   { "Display previous message", { MYKEY_CTRLP, 0, 0, 0, 0 }, 
     cmd_lastmessage },
   { "Display version", { MYKEY_CTRLV, 0, 0, 0, 0 }, cmd_showversion },
   { "Display needed experience", { 'E', 0, 0, 0, 0 }, player_checkexpneeded },
   { "Display message buffer", { ':', 'm', 0, 0, 0 }, cmd_msgbuffer },
   { "Display world time", { ':', 't', 0, 0, 0 }, cmd_showtime },
   { "Display used game time", { ':', 'T', 0, 0, 0 }, cmd_showtimepassed },
   { "Display hitpoint status", { 'H', 0, 0, 0, 0 }, cmd_showhp },
   { "Display a list of quests", { ':', 'q', 0, 0, 0 }, cmd_showquests },
   { "Display character information", { '@', 0, 0, 0, 0 }, player_showstatus },
   { "Display your damage resistance", { 'A', 0, 0, 0, 0 }, cmd_showac },
   { "Change your tactics", { 'T', 0, 0, 0, 0 }, player_changetactics },
   { "Change game options", { '\\', 0, 0, 0, 0 }, cmd_options },
   { "Change game options", { MYKEY_CTRLO, 0, 0, 0, 0 }, cmd_options },
   { "Move north", { '8', 0, 0, 0, 0 }, cmd_movenorth },
   { "Move northeast", { '9', 0, 0, 0, 0 }, cmd_movenortheast },
   { "Move east", { '6', 0, 0, 0, 0 }, cmd_moveeast },
   { "Move southeast", { '3', 0, 0, 0, 0 }, cmd_movesoutheast },
   { "Move south", { '2', 0, 0, 0, 0 }, cmd_movesouth },
   { "Move southwest", { '1', 0, 0, 0, 0 }, cmd_movesouthwest },
   { "Move west", { '4', 0, 0, 0, 0 }, cmd_movewest },
   { "Move northwest", { '7', 0, 0, 0, 0 }, cmd_movenorthwest },
   { "Walk north", { 'w', '8', 0, 0, 0 }, cmd_walknorth },
   { "Walk northeast", { 'w', '9', 0, 0, 0 }, cmd_walknortheast },
   { "Walk east", { 'w', '6', 0, 0, 0 }, cmd_walkeast },
   { "Walk southeast", { 'w', '3', 0, 0, 0 }, cmd_walksoutheast },
   { "Walk south", { 'w', '2', 0, 0, 0 }, cmd_walksouth },
   { "Walk southwest", { 'w', '1', 0, 0, 0 }, cmd_walksouthwest },
   { "Walk west",      { 'w', '4', 0, 0, 0 }, cmd_walkwest },
   { "Walk northwest", { 'w', '7', 0, 0, 0 }, cmd_walknorthwest },
   { "Apply quickskill number 0", { ':', '0', 0, 0, 0 }, cmd_qskill_0 },
   { "Apply quickskill number 1", { ':', '1', 0, 0, 0 }, cmd_qskill_1 },
   { "Apply quickskill number 2", { ':', '2', 0, 0, 0 }, cmd_qskill_2 },
   { "Apply quickskill number 3", { ':', '3', 0, 0, 0 }, cmd_qskill_3 },
   { "Apply quickskill number 4", { ':', '4', 0, 0, 0 }, cmd_qskill_4 },
   { "Apply quickskill number 5", { ':', '5', 0, 0, 0 }, cmd_qskill_5 },
   { "Apply quickskill number 6", { ':', '6', 0, 0, 0 }, cmd_qskill_6 },
   { "Apply quickskill number 7", { ':', '7', 0, 0, 0 }, cmd_qskill_7 },
   { "Apply quickskill number 8", { ':', '8', 0, 0, 0 }, cmd_qskill_8 },
   { "Apply quickskill number 9", { ':', '9', 0, 0, 0 }, cmd_qskill_9 },

   { "Apply quickskill number 0", { '=', 0, 0, 0, 0 }, cmd_qskill_0 },
   { "Apply quickskill number 1", { '!', 0, 0, 0, 0 }, cmd_qskill_1 },
   { "Apply quickskill number 2", { '"', 0, 0, 0, 0 }, cmd_qskill_2 },
   { "Apply quickskill number 3", { '#', 0, 0, 0, 0 }, cmd_qskill_3 },
   { "Apply quickskill number 4", { '$', 0, 0, 0, 0 }, cmd_qskill_4 },
   { "Apply quickskill number 5", { '%', 0, 0, 0, 0 }, cmd_qskill_5 },
   { "Apply quickskill number 6", { '&', 0, 0, 0, 0 }, cmd_qskill_6 },
   { "Apply quickskill number 7", { '/', 0, 0, 0, 0 }, cmd_qskill_7 },
   { "Apply quickskill number 8", { '(', 0, 0, 0, 0 }, cmd_qskill_8 },
   { "Apply quickskill number 9", { ')', 0, 0, 0, 0 }, cmd_qskill_9 },

   { "Save game", { ':', 'S', 0, 0, 0 }, cmd_save },
   { "Load game", { ':', 'L', 0, 0, 0 }, cmd_load },

   { "Push", { 'p', 0, 0, 0, 0 }, cmd_push },
   { "Search", { 's', 0, 0, 0, 0 }, cmd_search },
   { "Toggle search mode", { ':', 's', 0, 0, 0 }, cmd_searchmode },
   { "Examine/Look", { 'l', 0, 0, 0, 0 }, player_lookdir },
   { "Talk with someone", { 'C', 0, 0, 0, 0 }, player_chat },
   { "Read", { 'r', 0, 0, 0, 0 }, player_readscroll },
   { "Redraw screen", { MYKEY_CTRLR, 0, 0, 0, 0 }, cmd_redraw },
   { "Open door", { 'o', 0, 0, 0, 0 }, player_opendoor },
   { "Close door", { 'c', 0, 0, 0, 0 }, player_closedoor },
   { "Rest (pass one turn)", { '.', 0, 0, 0, 0 }, cmd_rest },
   { "Rest (pass one turn)", { '5', 0, 0, 0, 0 }, cmd_rest },
   { "Apply skills", { 'S', 0, 0, 0, 0 }, cmd_useskill },
   { "Cast a spell", { 'Z', 0, 0, 0, 0 }, cmd_cast },
   { "Enter downstairs", { '>', 0, 0, 0, 0 }, cmd_downstair },
   { "Enter upstairs", { '<', 0, 0, 0, 0 }, cmd_upstair },
   { "Get item", { ',', 0, 0, 0, 0 }, player_getitem },
   { "Drop item", { 'd', 0, 0, 0, 0 }, player_dropitem },
   { "Eat food", { 'e', 0, 0, 0, 0 }, player_eat },
   { "Throw/Shoot", { 't', 0, 0, 0, 0 }, player_rangedattack },
   { "Pay your bill", { 'P', 0, 0, 0, 0 }, player_paybill },
   { "Check your bill", { ':', 'b', 0, 0, 0 }, cmd_checkbill },
   { "Display help menu", { '?', 0, 0, 0, 0 }, cmd_help },
   { "Display keybindings", { ':', '?', 'k', 0, 0 }, cmd_showkeys },
   { "Display manual", { ':', '?', 'm', 0, 0 }, cmd_showmanual },
   { "Display game options", { ':', '?', 'v', 0, 0 }, config_listvars },

   

   /* cheat and debug commands */   
   { "Cheat: Light on", { MYKEY_CTRLL, 0, 0, 0, 0 }, cheat_light },
   { "Cheat: Identify item", { MYKEY_CTRLI, 0, 0, 0, 0 }, cheat_identify },

#ifndef SALADIR_RELEASE
   { "Debug: Create level", { MYKEY_CTRLG, 0, 0, 0, 0 }, debug_createlevel },
   { "Debug: Test hit", { MYKEY_CTRLH, 0, 0, 0, 0 }, debug_hittest },
#endif

   { "Editor: ON/OFF", { MYKEY_F10, 0, 0, 0, 0 }, editor_onoff },
   { "Editor: Next terrain", { MYKEY_F1, 0, 0, 0, 0 }, editor_nextterr },
   { "Editor: Prev terrain", { MYKEY_F2, 0, 0, 0, 0 }, editor_prevterr },
   { "Editor: Dramode", { MYKEY_F3, 0, 0, 0, 0 }, editor_drawmode },
   { "Editor: Set terrain", { ' ', 0, 0, 0, 0 }, editor_set },
   { "Editor: Set map visible", { MYKEY_F7, 0, 0, 0, 0 }, editor_visible },
   { "Editor: Hide map", { MYKEY_F8, 0, 0, 0, 0 }, editor_visibleoff },
   { "Editor: Clear map with current terrain", 
     { MYKEY_F9, 0, 0, 0, 0 }, editor_clear },
   { "Editor: Save map", { MYKEY_F5, 0, 0, 0, 0 }, editor_save },
   { "Editor: Load map", { MYKEY_F6, 0, 0, 0, 0 }, editor_load },

   { NULL, { 0 }, NULL }
};

int compare_cmd(Tcmd *cmd1, Tcmd *cmd2)
{
   return strcmp(cmd1->name, cmd2->name);
}

int compare_cmd_key(Tcmd *cmd1, Tcmd *cmd2)
{
   int32s sum=0;
   int8u i;

   for(i=0; i<CMD_MAXLEN; i++) {
      sum=(int32s)(cmd1->keys[i] - cmd2->keys[i]);
      if(sum!=0)
	 break;
   }

   return (int)sum;
}

void init_cmdtable(void)
{
   int16u indx=0;

   while(cmdtable[indx].cf != NULL)
      indx++;

   if(CONFIGVARS.cmdsort)
      qsort(cmdtable, indx, sizeof(Tcmd), compare_cmd_key);
   else
      qsort(cmdtable, indx, sizeof(Tcmd), compare_cmd);
}

int16s check_cmdtable(int16u *cmdline, int8u keynum)
{
   int16s indx=0;
   int16s i;
   bool cmdmatch;

   while(cmdtable[indx].cf != NULL) {

      cmdmatch=true;
      for(i=0; i<keynum+1; i++) {
	 if(cmdline[i]!=cmdtable[indx].keys[i])
	    cmdmatch=false;
      }	

      if(cmdmatch) {
	 if(cmdtable[indx].keys[keynum+1]==0)
	    return indx;
	 else 
	    return CMD_MULTI;
      }      
      indx++;
   }

   return CMD_NOCMD;


}

/* read one full command, even multikey command */
void read_cmd(void)
{
   int16u ch;
   int16s ci, mk=0;
   bool cmdnotfull=true;

   static char txt_cmdline[128];
   static int16u cmdline[CMD_MAXLEN+1];
   
   txt_cmdline[0]=0;

   while(cmdnotfull) {
      ch=my_getch();
      msg.update(false);
      strcat(txt_cmdline, my_keyname(ch));
      cmdline[mk]=ch;

      ci=check_cmdtable(cmdline, mk);
      clearline(1);
      my_gotoxy(1, 1);
      my_setcolor(C_RED);
      if(ci==CMD_NOCMD) {
	 cmdnotfull=false;
	 mk=0;	 
	 my_printf("\"%s\" Unknown Cmd.", txt_cmdline);
      }
      else if(ci==CMD_MULTI) {
	 mk++;
	 my_printf("Cmd \"%s\"", txt_cmdline);
      }
      else {
	 cmdnotfull=false;
	 mk=0;
	 cmdtable[ci].cf();
      }
      if(mk>CMD_MAXLEN) {
	 cmdnotfull=false;
	 mk=0;
      }
   }

}

/****************************/
/* The actual commands here */
/****************************/

void cmd_quit(void)
{
   if(confirm_yn("Really quit", false, true)) {
      GAME_NOT_ALIVE=true;
   }
}

void cmd_movenorth(void)
{
   moveplayer(DIR_UP, c_level, false);
}

void cmd_movesouth(void)
{
   moveplayer(DIR_DOWN, c_level, false);
}

void cmd_moveeast(void)
{
   moveplayer(DIR_RIGHT, c_level, false);
}

void cmd_movewest(void)
{
   moveplayer(DIR_LEFT, c_level, false);
}

void cmd_movenortheast(void)
{
   moveplayer(DIR_UPRIGHT, c_level, false);
}

void cmd_movenorthwest(void)
{
   moveplayer(DIR_UPLEFT, c_level, false);
}

void cmd_movesoutheast(void)
{
   moveplayer(DIR_DNRIGHT, c_level, false);
}

void cmd_movesouthwest(void)
{
   moveplayer(DIR_DNLEFT, c_level, false);
}


void cmd_walknorth(void)
{
   repeatmove(DIR_UP, c_level);
}

void cmd_walksouth(void)
{
   repeatmove(DIR_DOWN, c_level);
}

void cmd_walkeast(void)
{
   repeatmove(DIR_RIGHT, c_level);
}

void cmd_walkwest(void)
{
   repeatmove(DIR_LEFT, c_level);
}

void cmd_walknortheast(void)
{
   repeatmove(DIR_UPRIGHT, c_level);
}

void cmd_walknorthwest(void)
{
   repeatmove(DIR_UPLEFT, c_level);
}

void cmd_walksoutheast(void)
{
   repeatmove(DIR_DNRIGHT, c_level);
}

void cmd_walksouthwest(void)
{
   repeatmove(DIR_DNLEFT, c_level);
}

void cmd_lastmessage(void)
{
   msg.showlast();
}

void cmd_showversion(void)
{
   msg.newmsg(SALADIR_VERSTRING, CH_WHITE);
}

void cmd_options(void)
{
   bool prec=CONFIGVARS.cheat;

   game_config();

   if(prec==false && CONFIGVARS.cheat) {
      msg.update(true);
      msg.newmsg("You're entering the cheatmode, no scores will be saved.",
		 C_RED);
      if(confirm_yn("Are you sure", false, true)) {
	 GAME_CHEATER=true;
      }
      else
	 CONFIGVARS.cheat = false;

   }
   
}

void cmd_showhp(void)
{
   msg.vnewmsg(CH_CYAN, "Hp: head %d/%d, left hand %d/%d, "
	   "right hand %d/%d, body %d/%d, legs %d/%d.",
	   player.hpp[HPSLOT_HEAD].cur, player.hpp[HPSLOT_HEAD].max,
	   player.hpp[HPSLOT_LEFTHAND].cur, player.hpp[HPSLOT_LEFTHAND].max,
	   player.hpp[HPSLOT_RIGHTHAND].cur, player.hpp[HPSLOT_RIGHTHAND].max,
	   player.hpp[HPSLOT_BODY].cur, player.hpp[HPSLOT_BODY].max,
	   player.hpp[HPSLOT_LEGS].cur, player.hpp[HPSLOT_LEGS].max);
}

void cmd_qskill_0(void)
{
   player_quickskill(c_level, 0);
}

void cmd_qskill_1(void)
{
   player_quickskill(c_level, 1);
}

void cmd_qskill_2(void)
{
   player_quickskill(c_level, 2);
}

void cmd_qskill_3(void)
{
   player_quickskill(c_level, 3);
}

void cmd_qskill_4(void)
{
   player_quickskill(c_level, 4);
}

void cmd_qskill_5(void)
{
   player_quickskill(c_level, 5);
}

void cmd_qskill_6(void)
{
   player_quickskill(c_level, 6);
}

void cmd_qskill_7(void)
{
   player_quickskill(c_level, 7);
}

void cmd_qskill_8(void)
{
   player_quickskill(c_level, 8);
}

void cmd_qskill_9(void)
{
   player_quickskill(c_level, 9);
}


void cmd_msgbuffer(void)
{
   msg.showall();
}

void cmd_showtime(void)
{
   worldtime.show_worldtime();
}

void cmd_showtimepassed(void)
{
   passedtime.show_passed();
}

void cmd_showquests(void)
{
   quest_showall(&player.quests);
}

void cmd_push(void)
{
   player_push(c_level, 0);
}

void cmd_redraw(void)
{
   do_redraw(c_level);
}

void cmd_rest(void)
{
   moveplayer(DIR_SELF, c_level, false);
}

void cmd_search(void)
{
   search_surroundings(c_level, NULL, false);
}

void cmd_searchmode(void)
{
   if(player.searchmode) {

      player.searchmode=false;
      msg.newmsg("You won't be so attentive anymore.", C_WHITE);
   }
   else {
      if(!skill_check(player.skills, SKILLGRP_GENERIC, SKILL_SEARCHING)) {
	 msg.newmsg("Sorry, you don't have the skill.", C_WHITE);
	 return;
      }

      player.searchmode=true;
      msg.newmsg("You're now more observant.", 
		 CH_WHITE);
   }
}

void cmd_useskill(void)
{
   player_useskill(c_level, SKILLGRP_ALL, -1);
}

void cmd_cast(void)
{
   player_useskill(c_level, SKILLGRP_MAGIC, -1);
}

void cmd_downstair(void) {
   if(c_level->loc[player.pos_y][player.pos_x].type == TYPE_STAIRDOWN 
      || (player.dungeon==0 && player.dungeonlev==0)) {
      player.lastdir=c_level->loc[player.pos_y][player.pos_x].doorfl;

      player_go_down(&c_level);
   }
   else
      msg.newmsg("You start foolishly digging your way down...", 
		 C_WHITE);
}

void cmd_upstair(void)
{
   if(c_level->loc[player.pos_y][player.pos_x].type == TYPE_STAIRUP) {
      player.lastdir=c_level->loc[player.pos_y][player.pos_x].doorfl;

      player_go_up(&c_level);

   } else
      msg.newmsg("You jump very hard...", C_WHITE);
}

void cmd_showac(void)
{
   msg.vnewmsg(CH_CYAN, "Damage resistance: Head %d, Left hand %d, "
	       "Right hand %d, Body %d, Legs %d.",
	       player.hpp[HPSLOT_HEAD].ac,
	       player.hpp[HPSLOT_LEFTHAND].ac,
	       player.hpp[HPSLOT_RIGHTHAND].ac,
	       player.hpp[HPSLOT_BODY].ac,
	       player.hpp[HPSLOT_LEGS].ac);

}

void cmd_checkbill(void)
{
   int32u gc, sc, cc;

   if(!player.bill) {
      msg.newmsg("You don't have any debts!", C_WHITE);
   }
   else {
      shopkeeper_coppervalue(player.bill, &gc, &sc, &cc);
      msg.vnewmsg(C_WHITE, "Your current bill: "
		  "%ld gold, %ld silver and %ld copper!", gc, sc, cc);
   }
}

void cmd_help(void)
{
   int16u kb;

   msg.add("Which help [M]anual, [K]eybindings or [V]ariables?", CH_WHITE);

   kb=my_getch();
   if(kb=='m' || kb=='M') {
      cmd_showmanual();
   }
   else if(kb=='K' || kb=='k')
      cmd_showkeys();
   else if(kb=='V' || kb=='v')
      config_listvars();
   else
      msg.update(true);
}

void cmd_showmanual(void)
{
   chdir_tohelpdir();
   viewfile("manual.txt");
}

void cmd_showkeys(void)
{
   FILE *fh;
   int16u indx=0, i;

   const char keyfile[]="keytable.txt";

   init_cmdtable();

   chdir_totempdir();

   fh=fopen(keyfile, "w");

   if(!fh) {
      msg.newmsg("Error: Unable to view key bindings.", CH_RED);
      return;
   }

   /* create a text file containing keyinfo */
   fprintf(fh, "\001Legend of Saladir keyboard commands\n\n");

   fprintf(fh, "\n\007Commands like '\001C-a\007' means that you need "
	   "to press keys \002CTRL \007and \002a\007"
	   " together. Some commands may require multiple keypresses, ie. "
	   "command \"\001:?k\007\" means that you'll have to press first "
	   "\002:\007, then \002? \007and finally \002k \007to "
	   "execute the command.\n\n");
   fprintf(fh, "Commands starting with '\001w\007' will repeat several times, "
	   "the repeat count can be defined in options with variable "
	   "\003REPEATCOUNT\007.\n\n");

   while(cmdtable[indx].cf != NULL) {
      tempstr[0]=0;
      for(i=0; i<CMD_MAXLEN; i++) {
	 if(cmdtable[indx].keys[i]!=0)
	    strcat(tempstr, my_keyname(cmdtable[indx].keys[i]));
/*
	    if(cmdtable[indx].keys[i] <= 0xff)
	       strcat(tempstr, unctrl(cmdtable[indx].keys[i]));
	    else
	       strcat(tempstr, keyname(cmdtable[indx].keys[i]));
*/
      }
      fprintf(fh, "\002%-9s \007%s\n", tempstr, cmdtable[indx].name);

      indx++;
   }

   fprintf(fh, "\nCommands are case sensitive.\n\n");
   fclose(fh);

   /* finally view the file */
   viewfile(keyfile);
}

void cmd_save(void)
{
   GAME_NOTIFYFLAGS |= GAME_DO_REDRAW;
   my_clrscr();

   if( save() )
      msg.newmsg("Game saved.", C_WHITE);
   else
      msg.newmsg("Saved failed!", CHB_RED);

   showmore(false, false);
}

void cmd_load(void)
{
   GAME_NOTIFYFLAGS |= GAME_DO_REDRAW;
   my_clrscr();
   if( load(false) ) {
      msg.newmsg("Game loaded!", C_WHITE);
   }
   else
      msg.newmsg("Loading failed!", CHB_RED);

   showmore(false, false);
}

void cheat_identify(void)
{
   Tinvpointer matti;
   matti=inv_listitems(&player.inv, "Identify what?", -1, true, -1, -1);

   if(matti) {
      matti->i.status|=ITEM_IDENTIFIED;
   }

}

void cheat_light(void)
{
   if(!CONFIGVARS.cheat)
      return;

   if(player.light==0)
      player.light=15;
   else
      player.light=0;
}

void cheat_onoff(void)
{

}

void editor_onoff(void)
{
   if(!CONFIGVARS.cheat)
      return;

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
   editmode=!editmode;

   if(editmode) {
      msg.add("LEVEL EDITOR on.", CHB_RED);
      msg.add("(F1) Next terrain", CH_GREEN);
      msg.add("(F2) Last terrain", CH_GREEN);
      msg.add("(F3) Drawmode on/off", CH_GREEN);
      msg.add("(F5) Save OUTWORLD", CH_RED);
      msg.add("(F6) Load OUTWORLD", CH_RED);
      msg.add("(F7) Make whole map visible", CH_GREEN);
      msg.add("(F8) Make whole map hidden", CH_GREEN);
      msg.add("(F9) Clear level with terrain", CH_GREEN);
      msg.add("(SPC) Place terrain", CH_GREEN);
      editerrain=0;
   }
   else
      msg.newmsg("Level editor OFF!", CHB_RED);
}

void editor_nextterr(void)
{
   if(!editmode)
      return;

   GAME_NOTIFYFLAGS|=GAME_EDITORCHG;
   if(terrains[editerrain+1].out!=0)
      editerrain++;
}

void editor_prevterr(void)
{
   if(!editmode)
      return;

   GAME_NOTIFYFLAGS|=GAME_EDITORCHG;
   if(editerrain > 0) 
      editerrain--;
}

void editor_drawmode(void)
{
   if(!editmode)
      return;

   msg.newmsg("Toggle drawmode", C_GREEN);
   drawmode=!drawmode;

   if(drawmode) 
      msg.newmsg("ON!", C_GREEN);
   else 
      msg.newmsg("OFF!", C_GREEN);

}

void editor_set(void)
{
   if(!editmode)
      return;

   set_terrain(c_level, player.pos_x, player.pos_y, editerrain);
}

void editor_visible(void)
{
   if(!editmode)
      return;

   for(int16u ty=0; ty<c_level->sizey; ty++) {
      for(int16u tx=0; tx<c_level->sizex; tx++) {
	 c_level->loc[ty][tx].flags |= CAVE_SEEN;
      }
   }
}


void editor_visibleoff(void)
{
   if(!editmode)
      return;

   int16u mask=0xffff - CAVE_SEEN;

   for(int16u ty=0; ty<c_level->sizey; ty++) {
      for(int16u tx=0; tx<c_level->sizex; tx++) {		  
	 c_level->loc[ty][tx].flags &= mask;
      }
   }
}

void editor_clear(void)
{
   if(!editmode)
      return;

   initlevel(c_level, editerrain, MAXSIZEX, MAXSIZEY);

   player.pos_x=10;
   player.pos_y=10;
   player.lreg_x=0;
   player.lreg_y=0;
   player.spos_x=11;
   player.spos_y=11;

   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

}

void editor_save(void)
{
   if(!editmode)
      return;

   msg.add("Save to which file:", C_GREEN);
   my_gets(nametemp, sizeof(nametemp)-1);

   msg.newmsg("Saving level...", CH_YELLOW);
   if(edit_savemap(c_level, nametemp))
      msg.newmsg("OK!", C_GREEN);
   else
      msg.newmsg("fail!", C_RED);

}
void editor_load(void)
{
   if(!editmode)
      return;

   msg.add("Load from which file:", C_GREEN);
   my_gets(nametemp, sizeof(nametemp)-1);

   msg.newmsg("Loading level...", CH_YELLOW);
   if(edit_loadmap(c_level, nametemp)) {
      player.pos_x=10;
      player.pos_y=10;
      player.lreg_x=0;
      player.lreg_y=0;
      player.spos_x=11;
      player.spos_y=11;
      do_redraw(c_level);
      sprintf(tempstr, "File \"%s\" OK!, level size is X%d * Y%d.", nametemp, c_level->sizex, c_level->sizey);
      msg.newmsg(tempstr, C_GREEN);
   }
   else
      msg.newmsg("fail!", C_RED);

}

#ifndef SALADIR_RELEASE
void debug_createlevel(void)
{
   if(!CONFIGVARS.cheat)
      return;

   player.pos_x=player.pos_y=1;
   player.spos_x=2;
   player.spos_y=2;

   createlevel(c_level);
   GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
}


void debug_hittest(void)
{
   damage_issue(c_level, NULL, NULL, ELEMENT_FIRE, 12, HPSLOT_BODY,
		"The fireball hits!");

}
#endif /* SALADIR_RELEASE */

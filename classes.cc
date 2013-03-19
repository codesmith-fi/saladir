/*

	classes.cpp for Legend of Saladir

   (C)1997/1998 Erno Tuomainen

*/

#include "saladir.h"

#include "output.h"
#include "inventor.h"
#include "birth.h"
#include "classes.h"


void init_rogue(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp);
void init_fighter(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp);
void init_hunter(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp);
void init_ninja(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp);
void init_knight(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp);
void init_merchant(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp);
void init_mage(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp);



Tclassdef classes[]=
{
   /* first class is for classless monsters */
   { "", NULL },
   { "ninja", init_ninja },
   { "fighter", init_fighter },
   { "knight", init_knight },
   { "mage", init_mage },
   { "priest", NULL },
   { "paladin", NULL },
   { "shaman", NULL },
   { "necromancer", NULL },
   { "caveman", NULL },
   { "ranger", NULL },
   { "farmer", NULL },
   { "rogue", init_rogue},
   { "merchant", init_merchant },
   { "tourist", NULL },
   { "healer", NULL },
   { "hunter", init_hunter },
   { NULL }
};


/* 80 points to distribute between weapon skills ?? */

void init_mage(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp)
{
   /* fighter, good weapon skills */
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_DAGGER, 25);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_HAND, 25);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_STAFF, 30);

   birth_giveskillweapon(inv, *skills);
}

void init_fighter(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp)
{
   /* fighter, good weapon skills */
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_SWORD, 25);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_AXE, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_HAND, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_2HWEAP, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_1HWEAP, 10);

   birth_giveskillweapon(inv, *skills);
}

void init_rogue(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp)
{

   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_DAGGER, 25);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_SWORD, 10);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_HAND, 20);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_2WEAPCOMB, 10);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_1HWEAP, 15);

   if(RANDU(100) < 50)
      inv_additem(inv, IS_WEAPON1H, WEAPONS_TANTO, 2, MAT_IRON);
   else
      inv_additem(inv, IS_WEAPON1H, WEAPONS_DAGGER, 2, MAT_IRON);

   inv_additem(inv, IS_WEAPON1H, WEAPONS_SHORTSWORD, 1, MAT_IRON);
}

void init_hunter(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp)
{
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_BOW, 25);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_POLEARM, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_THROW, 20);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_SWORD, 10);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_1HWEAP, 10);

   inv_additem(inv, IS_WEAPON1H, WEAPONS_DAGGER, 2, MAT_IRON);
   inv_additem(inv, IS_WEAPON1H, WEAPONS_SHORTBOW, 1, MAT_ELVEN);
   inv_additem(inv, IS_WEAPON1H, WEAPONS_ARROW, 10+RANDU(15), MAT_ELVEN);

   inv_additem(inv, IS_ARMOR, ARMOR_PANTS, 1, MAT_NOMATERIAL);
   inv_additem(inv, IS_ARMOR, ARMOR_SHIRT, 1, MAT_NOMATERIAL);

}

void init_ninja(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp)
{
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_HAND, 30);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_SWORD, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_2HWEAP, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_THROW, 10);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_DAGGER, 10);

   inv_additem(inv, IS_WEAPON1H, WEAPONS_KATANA, 1, MAT_IRON);
   inv_additem(inv, IS_WEAPON1H, WEAPONS_SHORTBOW, 1, MAT_WOOD);
   inv_additem(inv, IS_WEAPON1H, WEAPONS_ARROW, 10+RANDU(15), MAT_WOOD);

   inv_additem(inv, IS_ARMOR, ARMOR_SANDALS, 1, MAT_NOMATERIAL);
   inv_additem(inv, IS_ARMOR, ARMOR_HOODEDCLOAK, 1, MAT_NOMATERIAL);

}


void init_knight(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp)
{
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_SWORD, 25);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_AXE, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_HAND, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_2HWEAP, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_1HWEAP, 10);

   inv_additem(inv, IS_WEAPON1H, WEAPONS_CLAYMORE, 1, MAT_IRON);
   inv_additem(inv, IS_ARMOR, ARMOR_HELMET, 1, MAT_CHAIN);
   inv_additem(inv, IS_ARMOR, ARMOR_CUIRASS, 1, MAT_CHAIN);
   inv_additem(inv, IS_ARMOR, ARMOR_GREAVES, 1, MAT_CHAIN);
   inv_additem(inv, IS_ARMOR, ARMOR_PAULDRONL, 1, MAT_CHAIN);
   inv_additem(inv, IS_ARMOR, ARMOR_PAULDRONR, 1, MAT_CHAIN);

}

void init_merchant(Tskillpointer *skills, Tinventory *inv, _statpack *stats, _hpslot *hpp)
{
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_BOW, 75);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_POLEARM, 15);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_THROW, 60);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_SWORD, 45);
   skill_addnew(skills, SKILLGRP_WEAPON, SKILL_1HWEAP, 10);

   inv_additem(inv, IS_WEAPON1H, WEAPONS_DAGGER, 2, MAT_IRON);
//   inv_additem(inv, IS_WEAPON1H, WEAPONS_SHORTBOW, 1, MAT_ELVEN);
   inv_additem(inv, IS_WEAPON1H, WEAPONS_ARROW, 25+RANDU(25), MAT_ELVEN);

   inv_additem(inv, IS_ARMOR, ARMOR_PANTS, 1, MAT_NOMATERIAL);
   inv_additem(inv, IS_ARMOR, ARMOR_SHIRT, 1, MAT_NOMATERIAL);

}

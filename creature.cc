/*
** creature.cpp
** list of creatures for Saladir
*/

/* MUISTA KORJATA ITEM/MONSTER STRUCTUURIT POINTTERITTOMIKSI */


#include "saladir.h"

#include "output.h"
#include "classes.h"
#include "creature.h"

/* list of long descriptions */
char desc_billgates[]=
"Oh no, this is Bill Gates. How very boring! Bill is the chairman of "
"Microsoft Company. This person constantly speaks about Windows 95 and "
"NT...don't listen to him, atleast don't agree!"
"\n\0";

char desc_sparhawk[]=
"You look at Sparhawk, a scar faced adventurer. This mighty knight has "
"a long scar going on the left side of his face, maybe a souvenier from "
"a battle. "
"\n\0";

char desc_thomas[]=
"You look at Thomas Biskup, he comes from Drakalor Chain of Ankardia. "
"Obviously his teleport corruption got wild and caused him to slip to this "
"part of the reality! His body looks very mutilated by the corruptions of "
"chaos. You almost feel pity for him but then you remember all bad things "
"which caused you to suffer while adventuring in Drakalor Chain. You are "
"really glad that this world does not contain corruption traps...:-)"
"\n\0";

char desc_natasha[]=
"She has very humanly looking body, "
"but has also some cat features. Her skin is covered with short blueish "
"fur and her face looks like cat's face. But still, she is really beautiful "
"and charming. She has thad odd attraction. Her swift muscles seem to "
"be ready for danger at all times."
"\n\0";


char desc_oddmonster[]=
"This is a monster of some sort."
"\n\0";

char *desclist[]=
{
     desc_oddmonster,
     desc_billgates,
     desc_sparhawk,
     desc_thomas,
     desc_natasha,
     NULL
};

/* Descriptions:

	NPC Creature index,
	Dungeon number,
	Dungeon level,
	x, y, (0 if random)

*/

/* THIS MUST BE IN THE SAME ORDER AS THE NPC_LIST BELOW!!! */

_Tappearlist appearlist[] =
{
	{ PLACE_DPRIMITIVE,	1, 0, 0 },
	{ PLACE_TSANTHEL, 	1, 0, 0 },
	{ PLACE_DPRIMITIVE, 	4, 0, 0 },
	{ PLACE_TSANTHEL, 	1, 0, 0 },
	{ 0, 0, 0, 0 }, /* end of the list */
};

_monsterdef npc_list[] =
{
     {"dull looking chairman", "Gill Bates", NPC_BILLGATES, 48000, 4, CHAOTIC, NPC_BILLGATES, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
	50, 50, 30, 50, 50, 50, 50, 20, BASE_SPEED, 0 },
     {"scar faced knight", "Sparhawk", NPC_SPARHAWK, 121000, 6, LAWFUL, NPC_SPARHAWK, RACE_HIGHELF, CLASS_KNIGHT, SEX_MALE, 0, BEHV_FRIENDLY,
	50, 50, 56, 70, 50, 50, 50, 20, BASE_SPEED+1, 0},
     {"very corrupted adventurer", "Thomas Biskup", NPC_THOMAS, 77500, 12, LAWFUL, NPC_THOMAS, RACE_HUMAN, CLASS_MAGE, SEX_MALE, 0, BEHV_FRIENDLY,
	50, 60, 53, 50, 50, 50, 50, 20, BASE_SPEED+2, 0},
     {"beautiful cat lady", "Natasha", NPC_NATASHA, 44500, 8, NEUTRAL, NPC_NATASHA, RACE_CATHUMAN, CLASS_MAGE, SEX_FEMALE, 0, BEHV_FRIENDLY,
	45, 55, 55, 55, 68, 60, 50, 25, BASE_SPEED+3, 0},
     { "", "" }
};


_monsterdef shopkeeper_list[] =
{
     {"merchant", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
      67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1, MST_SHOPKEEPER },
     {"trader", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
      67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1, MST_SHOPKEEPER },
     {"dealer", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
      67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1, MST_SHOPKEEPER },
     {"shopkeeper", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
      67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1, MST_SHOPKEEPER },
     {"storekeeper", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
      67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1, MST_SHOPKEEPER },
     { "", "" }
};
/*
**	NPC Race definition templates,
**
*/

char racedesc_human[]=
"Humans, this tall and thin race lives in all parts of the world. They often "
"have a certain tendency to wander all around the world exploring new things "
"like searching something missing from their heart.\n\n"
"Humans do not have any special abilities or disabilities but with their "
"average skills and stats they are a notable race as they are generally good "
"in everything.\n"
"\0";

char racedesc_bat[]=
"Bats, those fiercome creatures of the night. They are "
"good in darkness with their sonar, but bats are not very "
"dangerous when confronted one by one. Thou should beware when you "
"put your nose into a cave filled with bats. They are generally "
"fast.\n"
"\0";

char racedesc_monkeys[]=
"Monkeys and gorillas are fast, and furry creatures. They're great in strength "
"but lack the wisdom and intelligence but beware, even though they are animals they "
"have the ability to exploit some items for their benefit.\n\n"
"Gorillas are the big brothers of monkeys, they're really strong but slower "
"than the monkeys.\n"
"\0";

char racedesc_spiders[]=
"Spiders are fast and mostly poisonous creatures. Some spiders are "
"really good at fighting, but as all animals, they lack the intelligence "
"and wisdom. \n"
"\0";

char racedesc_elfs[]=
"Elves, there are many elf races. High elves, wood elves, gray elves and finally "
"the dark elves.\n\n"
"They all have their own abilities. They are intelligent creatures often "
"good in magic and using missile weapons like bows. Often elves lack the "
"strength but have a good intelligence and wisdom.\n"
"\0";

char racedesc_dwarf[]=
"Dwarves are short but sturdy race. They are tough persons, perhaps lacking "
"the wit of the elves but having a better strength and constitution.\n\n "
"It's said in these parts of the world that if a dwarf has made it's mind, don't come and argue with "
"it, unless you know him well.\n"
"\0";

char racedesc_orc[]=
"Orcs are strong and deadly and in the other hand very stupid and ugly. "
"They fear nothing but the light and careless adventurers are most probably "
"their best target."
"\n\0";

char racedesc_snake[]=
"These hissing creatures are often very deadly. They move pretty fast but "
"they are small (there have been some exceptions to this). Their two-"
"branched tongue is constantly smelling the air for victims like you."
"Some of them are known to strangle by entangling themselves around the "
"poor victim. "
"\n\0";

char racedesc_kobold[]=
"Kobolds are deadly creatures. (to be continued... :-)"
"\n\0";

char racedesc_cathuman[]=
"Cat humans are a feline race mixed with some humanly features. "
"They look fast and dangerous, build with fast and strong muscles."
"\n\0";

char racedesc_cat[]=
"Cats are fast but small creatures. As carnivores and vicious "
"hunters they have an exellent night vision and very sharp nails "
"on their paws."
"\n\0";

char racedesc_nodesc[]=
"No description..."
"\n\0";


_npcracedef npc_races[]=
{
     { "human", racedesc_human, '@', C_WHITE, 0, 0, 65000, 0,
       RANDALIGN, 3, 2, 2, 4, 12, 12, 0, 0, 43,
       3, 2, 0,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       50, 50, 50, 50, 50, 50, 50, 10, BASE_SPEED },
     { "cat human", racedesc_cathuman, 'F', C_BLUE, 0, 0, 65000, 0,
       RANDALIGN, 3, 2, 2, 4, 12, 12, 0, 0, 43,
       3, 2, 0,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       RACEGEN_SPECIAL,
       { 0, 0, 0, 0, 0, 0, 0 },
       45, 55, 55, 55, 55, 60, 50, 15, BASE_SPEED+2 },
     { "high elf", racedesc_elfs, '@', CH_YELLOW, 0, 0, 60000, 0,
       LAWFUL, 2, 4, 3, 4, 12, 12, 0, 0, 45,
       3, 2, 1,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       45, 55, 54, 54, 60, 56, 43, 15, BASE_SPEED+2 },
     { "wood elf", racedesc_elfs, '@', C_YELLOW, 0, 0, 60000, 0,
       NEUTRAL, 2, 4, 2, 4, 12, 12, 0, 0, 25,
       3, 2, 1,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       47, 53, 53, 53, 56, 54, 45, 15, BASE_SPEED },
     { "gray elf", racedesc_elfs, '@', C_WHITE, 0, 0, 60000, 0,
       RANDALIGN, 2, 4, 2, 4, 12, 12, 0, 0, 25,
       3, 2, 1,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       48, 52, 53, 52, 57, 54, 44, 12, BASE_SPEED-1 },
     { "dark elf", racedesc_elfs, '@', CH_DGRAY, 0, 0, 60000, 0,
       CHAOTIC, 1, 6, 3, 4, 12, 12, 0, 0, 26,
       3, 2, 1,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       50, 56, 55, 53, 43, 56, 44, 14, BASE_SPEED },
     { "dwarf", racedesc_dwarf, '@', C_WHITE, 0, 0, 70000, 0,
       RANDALIGN, 2, 6, 3, 4, 22, 8, 0, 0, 22,
       3, 2, 1,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       60, 46, 45, 62, 46, 54, 66, 15, BASE_SPEED-1 },
     { "cat", racedesc_cat, 'f', C_YELLOW, 0, BEHV_ANIMAL, 8400, 0,
       RANDALIGN, 3, 2, 2, 4, 12, 12, 0, 0, 43,
       3, 2, 0,
       TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, -1, -1, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       34, 35, 58, 57, 39, 30, 54, 10, BASE_SPEED },
     { "kobold", racedesc_kobold, 'k', C_GREEN, 0, 0, 70000, 0,
       CHAOTIC, 1, 6, 2, 2, 9, 2, 0, 0, 20,
       1, 4, 0,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       50, 50, 50, 50, 50, 50, 50, 10, BASE_SPEED },
     { "orc", racedesc_orc, 'o', C_WHITE, 0, 0, 110000, 0,
       CHAOTIC, 3, 6, 1, 2, 24, 4, 0, 0, 30,
       3, 4, 0,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       60, 30, 40, 60, 20, 30, 70, 5, BASE_SPEED-1 },
     { "snake", racedesc_snake, 's', C_YELLOW, ATTACK_BITE, BEHV_ANIMAL, 4000, 0,
       CHAOTIC, 4, 2, 1, 2, 4, 4, 0, 0, 10,
       2, 2, 1,
       TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, -1, -1, 1, -1 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       30, 60, 55, 40, 10, 60, 50, 10, BASE_SPEED+1 },
     { "spider", racedesc_spiders, 'x', CH_DGRAY, ATTACK_BITE, BEHV_ANIMAL, 5500, 0,
       CHAOTIC, 2, 4, 1, 2, 10, 2, 0, 0, 10,
       1, 4, 0,
       TARGET_LEGS,
       { 5, -1, -1, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       56, 10, 65, 40, 10, 10, 55, 10, BASE_SPEED+1 },
     { "large spider", racedesc_spiders, 'x', CH_DGRAY, ATTACK_BITE|ATTACK_HIT, BEHV_ANIMAL, 5500, 0,
       CHAOTIC, 3, 4, 1, 2, 15, 2, 0, 0, 10,
       3, 2, 1,
       TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, -1, -1, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       60, 10, 60, 40, 10, 10, 55, 10, BASE_SPEED+1 },
     { "giant spider", racedesc_spiders, 'X', CH_DGRAY, ATTACK_BITE|ATTACK_HIT, BEHV_ANIMAL, 23000, 0,
       CHAOTIC, 3, 4, 1, 2, 24, 4, 0, 0, 10,
       3, 4, 1,
       TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, -1, -1, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       65, 10, 58, 40, 10, 10, 55, 10, BASE_SPEED+1 },
     { "bat", racedesc_bat, 'b', CH_DGRAY, ATTACK_BITE, BEHV_FLYING|BEHV_ANIMAL, 2000, 0,
       RANDALIGN, 1, 3, 1, 1, 6, 1, 0, 0, 15,
       2, 2, 1,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       30, 20, 65, 60, 20, 10, 30, 10, BASE_SPEED+2 },
     { "gorilla", racedesc_monkeys, 'G', C_YELLOW, 0, BEHV_CANUSEITEM|BEHV_ANIMAL, 86000, 0,
       RANDALIGN, 4, 5, 0, 0, 22, 0, 0, 0, 30,
       3, 4, 1,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       70, 26, 60, 65, 30, 24, 65, 4, BASE_SPEED+1 },
     { "monkey", racedesc_monkeys, 'g', C_YELLOW, 0, BEHV_CANUSEITEM|BEHV_ANIMAL, 86000, 0,
       RANDALIGN, 4, 5, 0, 0, 22, 0, 0, 0, 30,
       2, 4, 0,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       70, 26, 60, 65, 30, 24, 65, 4, BASE_SPEED+2 },

     { "deer", racedesc_nodesc, 'd', C_WHITE, 0, BEHV_ANIMAL, 122000, 0,
       RANDALIGN, 4, 5, 0, 0, 22, 0, 0, 0, 30,
       2, 4, 0,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, -1, -1, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       50, 50, 50, 50, 50, 50, 50, 4, BASE_SPEED+2 },
     { "rabbit", racedesc_nodesc, 'b', CH_WHITE, 0, BEHV_ANIMAL, 5200, 0,
       RANDALIGN, 4, 5, 0, 0, 22, 0, 0, 0, 30,
       2, 4, 0,
       TARGET_HEAD | TARGET_BODY | TARGET_LHAND | TARGET_RHAND | TARGET_LEGS,
       { 5, 2, 2, 1, 3 },
       0,
       { 0, 0, 0, 0, 0, 0, 0 },
       50, 50, 50, 50, 50, 50, 50, 4, BASE_SPEED+2 },
     { NULL, NULL, 0 }
};

const char *txt_statnames[]=
{
   "Strength",
   "Toughness",
   "Constitution",
   "Charisma",
   "Dexterity",
   "Wisdom",
   "Intelligence",
   "Luck",
   "Speed",
   NULL
};

const char *txt_statnames_short[]=
{
   "STR",
   "TGH",
   "CON",
   "CHA",
   "DEX",
   "WIS",
   "INT",
   "LUC",
   "SPD",
   NULL
};

const char *txt_luckdesc[]=
{
   "very unlucky",
   "unlucky",
   "normal",
   "lucky",
   "very lucky",
   NULL,
};

const char *gendertext[]=
{
   "UNKNOWN SEX",
   "male",
   "female",
   "neutral",
   NULL
};

const char *gender_art1[]={
     "???",
     "he",
     "she",
     "it",
     NULL,
};

const char *gender_art2[]={
     "???",
     "him",
     "her",
     "it",
     NULL,
};

const char *gender_art3[]={
     "???",
     "his",
     "her",
     "its",
     NULL,
};


_Tfoodstat food_status[]=
{
     { "Unknown", 0, 0 },
     { "Fainted!", 0, 0 },
     { "Fainting", 0, 1 },
     { "Starving", 1, 0 },
     { "Hungry", 2, 30 },
     { "", 6, 0 },
     { "Satiated", 10, 0 },
     { "Bloated", 12, 0 },
     { "Unknown", 100, 0 },
     { NULL, 0 }
};

char *bodyparts[]=
{
     "head",
     "left hand",
     "right hand",
     "body",
     "legs",
     "odd place",
     NULL
};

char *bodyparts_flying[]=
{
     "head",
     "left vane",
     "right vane",
     "body",
     "legs",
     "odd place",
     NULL
};

const char *bodypart_art[]=
{
     "is",
     "is",
     "is",
     "is",
     "are",
};

const int8u eqslot_from_hpslot[]=
{
     EQUIP_HEAD,
     EQUIP_LHAND,
     EQUIP_RHAND,
     EQUIP_BODY,
     EQUIP_LEGS,
     0
};

/*
 * when calculating initial hitpoints for a creature/player
 * each bodypart is issued a (init_hp * hp_bpmod[bodypart]) hitpoints
 *
 */ 
const real hp_bpmod[]=
{
   0.50, /* head */
   0.75, /* left hand */
   0.75, /* right hand */
   1.00, /* body */
   0.75, /* legs */
   1.00,
};

const char *equip_slotdesc[]=
{
     "head",		/* crown, helmet, jewelry */
     "neck",
     "left ring",
     "right ring",

     "left hand",	/* weapons, tools */
     "right hand",
     "missiles",
     "tool",

     "cloak",		/* clothes */
     "shirt",
     "gloves",
     "pants",
     "boots",

     "upper body", 	/* protective armor */
     "left arm",
     "right arm",
     "legs",
     "Odd slot",
     "?????????",
     "?????????",
     "?????????",
     NULL
};


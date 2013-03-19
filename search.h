/* search.h */

#ifndef _SEARCH_H_DEFINED
#define _SEARCH_H_DEFINED


#define FOUND_NOTHING        0
#define FOUND_SECRETDOOR     1
#define FOUND_SECRETCORRIDOR 2
#define FOUND_DOORTRAP       3
#define FOUND_CAVETRAP       4

int16u search_location(level_type *level, int16s x, int16s y);
bool search_surroundings(level_type *, _monsterlist *, bool);


#endif

/* doors.h
*/

#ifndef _DOORS_H_DEFINED
#define _DOORS_H_DEFINED

/* door "error" codes */
#define DOORSTAT_SUCCESS        0 /* operation succesful */
#define DOORSTAT_ALREADY_OPEN   1 /* is already open */
#define DOORSTAT_NODOOR	        2 /* is no door */
#define DOORSTAT_LOCKED         3 /* door is locked */
#define DOORSTAT_STUCK	        4 /* door is locked */
#define DOORSTAT_ALREADY_CLOSED 5 /* is already closed */

#define DOOR_OPEN	0x01 /* open door */
#define DOOR_CLOSED     0x02 /* closed but not locked */
#define DOOR_LOCKED     0x04 /* locked and closed */
#define DOOR_TRAPPED    0x08 /* trapped */
#define DOOR_STUCK      0x10 /* stuck door */

#define CHANCE_DOORTRAP   20
#define CHANCE_STUCKDOOR  25
#define CHANCE_SECRETDOOR 15


/* sval values for doors */



bool door_create(level_type *, int16s, int16s, bool);
int8u door_open(level_type *, int16u, int16u, _monsterlist *);
int8u door_close(level_type *, int16u, int16u, _monsterlist *);
bool isitadoor(level_type *level, int16s x, int16s y);
bool is_secretdoor(level_type *level, int16s x, int16s y);
bool door_secret2normal(level_type *level, int16s x, int16s y);
bool door_closed2open(level_type *level, int16s x, int16s y);

#endif

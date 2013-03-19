/* pathfunc.h */

#ifndef _PATHFUNC_H_DEFINED
#define _PATHFUNC_H_DEFINED


/* pathlist prototypes */
void path_init(Tpathlist *path);
Tpathnode *path_insert(Tpathlist *list, Tcoordinate *data);
Tpathnode *path_insertend(Tpathlist *list, Tcoordinate *data);
void path_removefirst(Tpathlist *list);
Tcoordinate *path_getfirst(Tpathlist *list);
int8u path_getbest(Tpathlist *list, Tcoordinate *end, Tcoordinate *retdata);
void path_clear(Tpathlist *list);

/* path estimation/finding functions */
int16u path_distance(Tcoordinate *p1, Tcoordinate *p2);
bool path_movealong(Tpathlist *path, int16s *tx, int16s *ty);

Tpathlist path_findroute(level_type *level,
			 int16u x1, int16u y1, int16u x2, int16u y2);

#endif

/* list.h */

typedef struct _coord
{
	int x;
	int y;
	unsigned int d;		/* distance so far */
} tcoord;

typedef tcoord Tlistdata;
typedef Tlistdata Coord;


typedef struct _listnode
{
	Tlistdata c;
	struct _listnode *next;
} Node;

typedef Node *Llist;

/* prototypes */
void list_init(Llist *list);
Node *list_insert(Llist *list, Tlistdata *data);
void list_removefirst(Llist *list);
Tlistdata *list_getfirst(Llist *list);
void list_showall(Llist *list);
void list_dispmap(Llist *list);
void list_clear(Llist *list);
int list_getbest(Llist *list, Tlistdata *end, Tlistdata *retdata);






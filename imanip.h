/* imanip.h */

void item_setrandomcondition(item_def *item);

bool item_clearall(item_def *iptr);
bool container_init(item_def *iptr);
void set_material(item_def *, int16s);
void set_alignment(item_def *item);

item_def *init_moneyitem(int16s type);
item_def *init_scrollitem(int16s type);

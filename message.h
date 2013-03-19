/* message.h
**
**	header for message.cpp
*/

// max number of messages to hold in a buffer

#ifndef _MESSAGE_H_DEFINED
#define _MESSAGE_H_DEFINED

#include "saladir.h"

#include "output.h"
#include "caves.h"
#include "memory.h"

//#include "types.h"
//#include "options.h"

#define MSG_MAXNUM	60

typedef struct _msginfo *Tmsgpointer;
typedef Tmsgpointer Tmsglist;

typedef struct _msginfo
{
      char *msg;
      int16u count;
      int16u color;
      int16u delay;
      Tmsgpointer next;
} Tmessage;

class Message
{
  private:
   Tmsglist msglist;
   Tmsglist shownext;
   Tmsgpointer lastadded;

   int8u count;
   int8u index;
   int8u updcount;

   int16u msgdelay;
   int16s lastx;
   int16s lasty;
  public:
   Message();
   ~Message();
   void vnewmsg(int16u, char *, ...);
   void vadd(int16u, char *, ...);
   void vaddwait(int16u, char *, ...);
   void newmsg(char *, int16u);  
   void add(char *, int16u);     
   void addwait(char *, int16u);
   void clearall(void);
   void notice(void);
   void showlast(void);
   void update(bool);
   void showall(void);
   void setdelay(int16u);     /* set a delay for printing */

   void add_dist(level_type *level, int16s, int16s, 
		 char *, int16u, char *, int16u);
   void vnewdist(level_type *level, int16s x, int16s y, 
		 int16u color2, int16u color1,
		 char *message2,
		 char *format, ...);

};


#endif

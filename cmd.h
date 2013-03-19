/*
cmd.h
 */

#ifndef _CMD_H_DEFINED
#define _CMD_H_DEFINED

#define CMD_MAXLEN 4

typedef struct
{
      char *name;
      int16u keys[CMD_MAXLEN+1];
      void (* cf)(void);
} Tcmd;

void read_cmd(void);

#endif

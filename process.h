/* process */

#ifndef _PROCESS_H_DEFINED
#define _PROCESS_H_DEFINED

int16s ERROROUT(char *);
bool init_errorhandlers(void);
void handle_SEGV(void);

void freeall(void);

bool log_error(const char *fmt, ...);

#endif

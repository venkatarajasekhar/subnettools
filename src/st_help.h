#ifndef ST_HELP_H
#define ST_HELP_H

#define LANG_MIN 0
#define LANG_MAX 1

#define LANG_EN  0
#define LANG_FR  0

#include "st_options.h"
extern const char *default_fmt;

void usage(int argc, char **argv, struct st_options *o);
void debug_usage(void);

#else
#endif

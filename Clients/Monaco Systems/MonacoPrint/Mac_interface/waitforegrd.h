// wait until the application is the foreground application and also notify the user that
// the application needs attention
#ifndef IN_WAITFOREGND
#define IN_WAITFOREGND

#include "mcostat.h"

McoStatus waitTillForeground(void);

#endif
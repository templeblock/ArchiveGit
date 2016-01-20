#include "misc.h"
#include "array.h"
#include "database.h"

extern void od(LPSTR lpFormat, ...);

FARCLASS PMGDatabase;
typedef PMGDatabase far *PMGDatabasePtr;
FARCLASS UpdateState;
typedef UpdateState far *UpdateStatePtr;

#define printf od

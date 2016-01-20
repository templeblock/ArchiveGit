#include "misc.h"
#include "heap.h"
#include "array.h"
#include "database.h"

extern HeapPtr system_heap;
extern void od(LPSTR lpFormat, ...);

FARCLASS PMGDatabase;
typedef PMGDatabase far *PMGDatabasePtr;
FARCLASS UpdateState;
typedef UpdateState far *UpdateStatePtr;

#define printf od

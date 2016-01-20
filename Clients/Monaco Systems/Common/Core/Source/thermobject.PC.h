#ifndef IN_THERMOBJECT
#define IN_THERMOBJECT

#include <windows.h>

typedef short (STATUSFUNC) (short state, long scale, unsigned long sdata);

typedef enum {
	Process_Start = 0,
	Process_GammutSurface,
	Process_GammutCompress,
	Process_BuildingTable,
	Process_End
} ProcessingStates;

class ThermObject {
private:
protected:
	STATUSFUNC* _func;
	unsigned long _sdata;

public:	
	ThermObject(STATUSFUNC *func, unsigned long sdata);	
	void SetUpTherm(ProcessingStates ps,long max);
	BOOL DisplayTherm(long k,ProcessingStates ps);
	
};	
	
#endif

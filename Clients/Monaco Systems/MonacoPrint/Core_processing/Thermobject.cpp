/*     Thermobject.cpp

		the thermometer object that ties a thread the the thermometer window 

		(c) Copyright 1997, Monaco Systems Inc.
		by James Vogh
		
*/

#include "thermobject.h"
#include <threads.h>


	
ThermObject::ThermObject(long *c_k,long  *m_k,int *fin,int *q,McoStatus *er,ProcessingStates *ps)
{
current_k = c_k;
max_k = m_k;
finished = fin;
quit = q;
error = er;
pstate = ps;
count = TickCount();
}

void ThermObject::SetUpTherm(ProcessingStates ps,long mk)
{
*max_k = mk;
*pstate = ps;
}

Boolean ThermObject::DisplayTherm(long k,ProcessingStates ps)
{
long current_count = TickCount();
EventRecord theEvent;
#ifdef USE_THREADS
if (*quit) return 0;
*current_k = k;
*pstate = ps;
if ((EventAvail(0xFFFF,&theEvent)) || (current_count - count > 10))
	{
	YieldToAnyThread();
	count = current_count;
	}
return 1;
#else

GetNextEvent(0xFFFF,&theEvent);
return 1;
#endif
}
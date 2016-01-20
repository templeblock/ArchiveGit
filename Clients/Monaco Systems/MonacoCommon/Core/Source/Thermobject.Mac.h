/*     Thermobject.h

		the thermometer object that ties a thread the the thermometer window 

		(c) Copyright 1997, Monaco Systems Inc.
		by James Vogh
		
*/

#ifndef IN_THERMOBJECT
#define IN_THERMOBJECT

#include "mcostat.h"
#include "process_states.h"

#define USE_THREADS


class ThermObject {
private:
protected:

public:

	long	*current_k;
	long 	*max_k;
	int		*finished;
	int		*quit;
	McoStatus *error;
	ProcessingStates *pstate;
	
	long	count;
	
	ThermObject(long *c_k,long  *m_k,int *fin,int *q,McoStatus *er,ProcessingStates *ps);
	
	void SetUpTherm(ProcessingStates ps,long max);
	Boolean DisplayTherm(long k,ProcessingStates ps);
	
};	
	
#endif
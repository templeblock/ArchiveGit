#include "thermobject.h"
	
ThermObject::ThermObject(STATUSFUNC *func, unsigned long sdata)
{
	_func = func;
	_sdata = sdata;
}

void ThermObject::SetUpTherm(ProcessingStates ps,long max)
{
	short ret;

	ret = _func(ps, max, _sdata);
/*
	if(ret == 0)
		return MCO_FAILURE;
	else
		return MCO_SUCCESS;
*/
}

Boolean ThermObject::DisplayTherm(long k, ProcessingStates ps)
{
	short ret;

	ret = _func(ps, k, _sdata);

	if(ret == 0)
		return FALSE;
	else
		return TRUE;
}

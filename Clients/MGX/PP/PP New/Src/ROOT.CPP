/*=======================================================================*\

	ROOT.C - Base class overloads new and delete to get around MFC set_new_handler

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "pp.h"
#include <malloc.h>

void *CPPRoot::operator new(size_t size)
{
	return(_fmalloc(size));
}

void CPPRoot::operator delete(void *ptr)
{
	_ffree(ptr);
}

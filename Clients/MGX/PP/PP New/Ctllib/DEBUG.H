/**************************************************************************
	Header File for Debug Code

	Written by Atif Aziz, August 1993
**************************************************************************/

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef DEBUG

	#include <assert.h>

	#define ASSERT(f)		assert(f)
        #define CHECK_PARAMETER(f, r)	ASSERT(f)	

#else

	#ifndef NULL
        	#include <stddef.h>
        #endif

	#define ASSERT(f)		((void) 0)
	#define CHECK_PARAMETER(f, r)	if (!(f)) return (r);

#endif

#endif

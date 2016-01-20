/*=================================================================
Contains:	Defines for MacIntosh/MPW compile
=================================================================*/

#ifndef _H_SCMACINT
#define _H_SCMACINT

//	Header configuration.  To determine if we are using universal headers load types.h and then
//	look to see if it has included the universal headers <ConditionalMacros.h> file.
#include <Types.h>

#ifdef __CONDITIONALMACROS__
	#define USEUNIVERSALHEADERS 1
#else
	#define USEUNIVERSALHEADERS 0
#endif

#if USEUNIVERSALHEADERS && !defined(USESROUTINEDESCRIPTORS)
	#define USESROUTINEDESCRIPTORS 1
#endif

#include "StdDef.h"
#include <string.h>

// memory model stuff - for intel only
#define scNEAR
#define scFAR
#define SChuge

#endif _H_SCMACINT

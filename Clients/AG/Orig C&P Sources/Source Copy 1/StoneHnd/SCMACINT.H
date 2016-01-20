/*=================================================================
 
	File:		SCMACINT.H

	$Header: /Projects/Toolbox/ct/SCMACINT.H 2     5/30/97 8:45a Wmanis $

	Contains:	Defines for MacIntosh/MPW compile

	Written by:	
	
	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

=================================================================*/



#ifndef _H_SCMACINT
#define _H_SCMACINT

/*
 *	Header configuration.  To determine if we are using universal headers load types.h and then
 *	look to see if it has included the universal headers <ConditionalMacros.h> file.
 */
#include <Types.h>

#ifdef __CONDITIONALMACROS__
#define USEUNIVERSALHEADERS 1
#else
#define USEUNIVERSALHEADERS 0
#endif

#if USEUNIVERSALHEADERS && !defined(USESROUTINEDESCRIPTORS)
#define USESROUTINEDESCRIPTORS 1
#endif

/* SYSTEM INCLUDES */
#include "StdDef.h"


//#include <OSUtils.h>
//#include <Events.h>
//#include <limits.h>
#include <string.h>
//#include <math.h>





	// memory model stuff - for intel only
#define scNEAR
#define scFAR
#define SChuge

//volatile is not supported by MPW
#define volatile


#define SCTickCount()           (TickCount())
#define SCSysBeep(duration)     (SysBeep((int)(duration)))

#endif /* _H_SCMACINT.H */

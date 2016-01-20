//-------------------------------------------------------------------------------
//
//	File:
//		PIUNew.h
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		As of 4/2/98 Metrowerks CodeWarrior's PPC implementation
//		of delete does not clean up after new and leaks 64K
//		each time.  This should be removed when Metrowerks
//		fixes the problem.
//
//	Use:
//		Include this in your headers or use the PIUBasic
//		library to get this override of the global new and
//		delete operators.
//
//		I suppose in a future version you could keep this
//		around and use it to debug new and delete calls to
//		track memory and leaks.
//
//	Version history:
//		1.0.0	4/2/1998	Ace		Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------
 
#ifndef __PIUNew_h__	// Defined yet?
#define __PIUNew_h__	// Only define once.

#include "PIDefines.h"	// Plug-in definitions.

#ifdef __PIMWCWMac__ // Define only for Metrowerks CodeWarrior PPC.

//-------------------------------------------------------------------------------

#include <new>

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

#ifdef MSIPL_USING_NAMESPACE	// CW Pro 3 has namespace issues.
namespace PIUNew
{
#endif

	void* operator new(unsigned long size);
	void operator delete(void* block);

#ifdef MSIPL_USING_NAMESPACE
}
#endif

//-------------------------------------------------------------------------------

#endif __PIMWCWMac__ // Define only for Metrowerks CodeWarrior PPC.

#endif // __PIUNew_h__

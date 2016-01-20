//-------------------------------------------------------------------------------
//
//	File:
//		PIUNew.cpp
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
//		1.0.0	4/2/1998	CAM		Created for Photoshop 5.0
//			Written by Charles McBrian.
//
//-------------------------------------------------------------------------------

#include "PIUNew.h"	// Includes PIDefines.h.

#ifdef __PIMWCWMac__ // Define only for Metrowerks CodeWarrior PPC.

//-------------------------------------------------------------------------------

#include <stdlib.h>

//-------------------------------------------------------------------------------
//
//	New
//
//	This routine overrides the global operator new so that we can control
//	allocation and deallocation.
//
//	You could also track these allocations to do memory and leak testing
//	in a debug version.  Just check for _DEBUG.
//
//-------------------------------------------------------------------------------

void * operator new(unsigned long size)
	{
	// You never want to use malloc and free in a plug-in normally.  You
	// should always try to favor the host's handle, buffer, or block
	// suites.  But we may not have those suites available yet, so we've
	// got to override this using malloc and free.
	
	return malloc (size);
	}

//-------------------------------------------------------------------------------
//
//	Delete
//
//	This routine overrides the global operator delete so that we can control
//	allocation and deallocation.  This is where the Metrowerks CodeWarrior
//	bug is.  On delete, 64K is leaked.
//
//	You could also track these allocations to do memory and leak testing
//	in a debug version.  Just check for _DEBUG.
//
//-------------------------------------------------------------------------------

void operator delete(void* block)
	{
	if (block != NULL)
		{
		free (block);
		}
	}	

//-------------------------------------------------------------------------------

#endif // Define only for Metrowerks CodeWarrior PPC.

// end PIUNew.cpp

/****************************************************************

	File:		SC_SYSCO.C

	$Header: /Projects/Toolbox/ct/SC_SYSCO.CPP 2	 5/30/97 8:45a Wmanis $

	Contains:	Implementation of transfer of clipboard data
				to external format.

	Written by: Lucas

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************/

/* THESE ARE STUBS AND ARE BY NO MEANS COMPLETE OR ROBUST */

#include "sctypes.h"
#ifdef SCMACINTOSH
#include <Memory.h>
#endif

/* ==================================================================== */

SystemMemoryObject::SystemMemoryObject()
{
#if defined( SCWINDOWS )
	fSYSHandle = GlobalAlloc( 0, GPTR ); // since GHND allows only 64k bytes
#elif defined( SCMACINTOSH )
	fSYSHandle = NewHandle( 0 );						  
#endif
}

/* ==================================================================== */

SystemMemoryObject::~SystemMemoryObject()
{
#if defined( SCWINDOWS )
	if ( fSYSHandle )
		GlobalFree( fSYSHandle );
#elif defined( SCMACINTOSH )
	if ( fSYSHandle )
		DisposHandle( fSYSHandle );
#endif
}

/* ==================================================================== */

void SystemMemoryObject::ReleaseMem()
{
	fSYSHandle = 0;
}

/* ==================================================================== */

long SystemMemoryObject::HandleSize( void )
{
#if defined( SCWINDOWS )
	return (size_t)GlobalSize( fSYSHandle );
#elif defined( SCMACINTOSH )
	return GetHandleSize( fSYSHandle );
#endif
}

/* ==================================================================== */

status SystemMemoryObject::SetHandleSize( long newsize )
{
#if defined( SCWINDOWS )
	fSYSHandle = GlobalReAlloc( fSYSHandle, newsize, GMEM_MOVEABLE | GMEM_ZEROINIT );
	return fSYSHandle != 0 ? scSuccess : scERRmem;
#elif defined( SCMACINTOSH ) 
	::SetHandleSize( fSYSHandle, newsize );
	return MemError() == noErr ? scSuccess : scERRmem;
#endif
}

/* ==================================================================== */

void  *SystemMemoryObject::LockHandle( void )
{
#if defined( SCWINDOWS )	
	return GlobalLock( fSYSHandle );
#elif defined( SCMACINTOSH )
	HLock( fSYSHandle );
	return *fSYSHandle;
#endif
}

/* ==================================================================== */

void SystemMemoryObject::UnlockHandle( void )
{
#if defined( SCWINDOWS )	
	GlobalUnlock( fSYSHandle );
#elif defined( SCMACINTOSH )
	HUnlock( fSYSHandle );
#endif
}

/*====================================================================*/

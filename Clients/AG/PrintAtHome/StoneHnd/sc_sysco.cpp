/****************************************************************
Contains:	Implementation of transfer of clipboard data to external format.
****************************************************************/

#include "sctypes.h"

/* ==================================================================== */

SystemMemoryObject::SystemMemoryObject()
{
	fSYSHandle = GlobalAlloc( 0, GPTR ); // since GHND allows only 64k bytes
}

/* ==================================================================== */

SystemMemoryObject::~SystemMemoryObject()
{
	if ( fSYSHandle )
		GlobalFree( fSYSHandle );
}

/* ==================================================================== */

void SystemMemoryObject::ReleaseMem()
{
	fSYSHandle = 0;
}

/* ==================================================================== */

long SystemMemoryObject::HandleSize( void )
{
	return (size_t)GlobalSize( fSYSHandle );
}

/* ==================================================================== */

status SystemMemoryObject::SetHandleSize( long newsize )
{
	fSYSHandle = GlobalReAlloc( fSYSHandle, newsize, GMEM_MOVEABLE | GMEM_ZEROINIT );
	return fSYSHandle != 0 ? scSuccess : scERRmem;
}

/* ==================================================================== */

void  *SystemMemoryObject::LockHandle( void )
{
	return GlobalLock( fSYSHandle );
}

/* ==================================================================== */

void SystemMemoryObject::UnlockHandle( void )
{
	GlobalUnlock( fSYSHandle );
}

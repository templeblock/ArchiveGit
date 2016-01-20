// ****************************************************************************
//
//  File Name:			Utilities.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Useful utility functions
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/Utilities.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "Utilities.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	::SolveQuadratic
//
//  Description:		Compute the roots of the quadratic equation with the given coefficients
//
//  Returns:			the number of roots
//
//  Exceptions:		none
//
// ****************************************************************************
//
int SolveQuadratic( YFloatType A, YFloatType B, YFloatType C, YFloatType* pRoots )
{
	if( !AreFloatsEqual(A,0.0) )
		{
		YFloatType	disc( B*B-(4.0*A*C) );
		if( disc >= 0.0 )
			{
			//
			// at least one root...
			YFloatType	discroot( ::sqrt(disc) );
			YFloatType	A2( A*2.0 );
			pRoots[0] = -(B + discroot) / A2;
			if( AreFloatsEqual(discroot,0.0) )
				{
				//
				// just the one root...
				return 1;
				}
			pRoots[1] = -(B - discroot) / A2;
			return 2;
			}
		}
	else if( !AreFloatsEqual(B,0.0) )
		{
		pRoots[0] = -C/B;
		return 1;
		}
	return 0;
}

#ifdef	_WINDOWS

// ****************************************************************************
//
//  Function Name:	::PointerToHandle
//
//  Description:		Allocates a global handle and copys the specified data
//							into it
//
//  Returns:			The allocated handle
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
HNATIVE PointerToHandle( const uBYTE* pData, YDataLength dataLength )
	{
	// Allocate a block of global memory large enough to hold the data
	HGLOBAL h = ::GlobalAlloc( GHND | GMEM_SHARE, dataLength );
	if( !h )
		throw kMemory;

	// Lock the memory
	uBYTE* p = static_cast<uBYTE*>( ::GlobalLock( h	) );
	if( !p )
		throw kMemory;

	// Copy the data								
	memcpy( p, pData, dataLength );

	// Unlock the memory
	::GlobalUnlock( h );

	return h;
	}

// ****************************************************************************
//
//  Function Name:	::HandleToPointer
//
//  Description:		Allocates memory and copys the specified handle into it
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void HandleToPointer( HNATIVE hNative, uBYTE*& pData, YDataLength& dataLength )
	{
	// Get the size of the memory
	dataLength = ::GlobalSize( hNative );

	// Allocate memory
	pData = new uBYTE[ dataLength ];

	// Lock down the handle
	uBYTE* p = static_cast<uBYTE*>( ::GlobalLock( hNative	) );

	// Copy the data
	memcpy( pData, p, dataLength );

	// Unlock the memory
	::GlobalUnlock( hNative );
	}

// ****************************************************************************
//
//  Function Name:	::HandleToHandle
//
//  Description:		Creates a handle and copys the specified handle into it
//
//  Returns:			The new handle
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
HNATIVE HandleToHandle( HNATIVE hNative )
	{
	// Get the size of the memory
	YDataLength	dataLength	= ::GlobalSize( hNative );

	// Allocate a block of global memory large enough to hold the data
	HGLOBAL h	= ::GlobalAlloc( GHND | GMEM_SHARE, dataLength );
	if( !h )
		throw kMemory;

	// Lock down the handle
	uBYTE* pSource			= static_cast<uBYTE*>( ::GlobalLock( hNative	) );
	uBYTE* pDestination	= static_cast<uBYTE*>( ::GlobalLock( h ) );

	// Copy the data
	memcpy( pDestination, pSource, dataLength );

	// Unlock the memory
	::GlobalUnlock( hNative );
	::GlobalUnlock( h );

	//	Return the new handle
	return h;
	}

// ****************************************************************************
//
//  Function Name:	::FreeHandle
//
//  Description:		Frees a block of global memory
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void FreeHandle( HNATIVE hNative )
	{
	::GlobalFree( hNative );
	}

// ****************************************************************************
//
//  Function Name:	::OutputDebugStringWithTime
//
//  Description:		Outputs a string to the debug stream along with the time
//							it was output.
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void OutputDebugStringWithTime( char* pString )
	{
	// Get the current time
	DWORD dwCurrentTime = ::GetTickCount( );

	// Format the message and time into a buffer
	char buffer[ 1024 ];
	wsprintf( buffer, "%d - %s\n", dwCurrentTime, pString );

	// Output the string
	::OutputDebugString( buffer );
	}

// ****************************************************************************
//
//  Function Name:	::GetScreenColorDepth
//
//  Description:		Gets the color depth of the screen
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
int GetScreenColorDepth( )
	{
	//	REVIEW ESV 11/25/96 - Should we use colorDepth = numplanes * bitspixel?
	HDC hdc = ::CreateCompatibleDC( NULL );
	int colorDepth = ::GetDeviceCaps( hdc, BITSPIXEL );
	::DeleteDC( hdc );
	return colorDepth;
	}

// ****************************************************************************
//
//  Function Name:	GetCDROMDrives( )
//
//  Description:		Finds CDROM volumes
//
//  Returns:			Number of drives found, zero if none
//
//  Exceptions:		None
//
// ****************************************************************************
FrameworkLinkage int GetCDROMDrives( char *szBuff, int nMaxCDs )
{
	int  nDrives = 0;
	char szDrives[202];
	char   szRoot[5];

	// obtain drive letters for all drives in system (50 max)
	DWORD nSize = ::GetLogicalDriveStrings( 200, szDrives );
	
	if ( nSize > 0 )
	{
		DWORD i = 0;
		char *srcptr = szDrives;
		
		// examine all drives
		while ( i < nSize )
		{
			// get next drive
			char *dstptr = szRoot;
			while ( *srcptr != '\0' )
			{
				*dstptr++ = *srcptr++;
				i++;
			}
			*dstptr = *srcptr++;
			i++;

			if ( *szRoot != '\0' )
			{
				// determine drive type
				UINT nType = ::GetDriveType(szRoot);

				if ( nType == DRIVE_CDROM )
				{
					// add drive letter to output buffer
					if ( nDrives < nMaxCDs )
					{
						*szBuff++ = *szRoot;
					}
					nDrives++;
				}
			}
		}
	}
	*szBuff = '\0';									// terminate buffer

	return nDrives;
}

// ****************************************************************************
//
//  Function Name:	IsFileAtPath( )
//
//  Description:		Looks for a file at the supplied path
//
//  Returns:			TRUE if found
//
//  Exceptions:		None
//
// ****************************************************************************
FrameworkLinkage BOOLEAN IsFileAtPath( RMBCString& rFile, RMBCString& rPath )
{
	// Form the complete name
	RMBCString rFullPath = rPath + "\\" + rFile;

	WIN32_FIND_DATA findFileData;
	HANDLE findFileHandle = ::FindFirstFile( (const char*)rFullPath, &findFileData );
	if ( findFileHandle == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}
	else
		::FindClose( findFileHandle );

	return TRUE;
}

#endif //_WINDOWS
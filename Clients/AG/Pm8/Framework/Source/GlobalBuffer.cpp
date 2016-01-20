// ****************************************************************************
//
//  File Name:			GlobalBuffer.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Routines to manage the shared global buffers
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
//  $Logfile:: /PM8/Framework/Source/GlobalBuffer.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "GlobalBuffer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef	IMAGER
#include "ThreadSupport.h"
#endif	// IMAGER

const uLONG		kBufferGuard				= 0xFEDCBA98;
const	uLONG		kGlobalBufferSize			= kDefaultGlobalBufferSize+sizeof(uLONG) Debug( + sizeof(kBufferGuard) + sizeof(kBufferGuard) );
const	int		kNumGlobalBuffers			= 4;
const	int		kWarnGlobalBufferUsage	= 20;

typedef	RArray<uBYTE*>								YGlobalBufferCollection;
typedef	YGlobalBufferCollection::YIterator	YGlobalBufferIterator;

			uBYTE**								_pGlobalBuffers = NULL;
//j			BOOLEAN*								_pfBufferUsed = NULL;
			BOOLEAN*								_pfBufferUsed[kNumGlobalBuffers];
static	YGlobalBufferCollection			_ExtraGlobalBuffers;

static 	void 									FreeExtraGlobalBuffers( );

#ifndef	IMAGER
static RCriticalSection globalBufferCriticalSection;
#endif	// IMAGER

// ****************************************************************************
//
//  Function Name:	FreeExtraGlobalBuffers( )
//
//  Description:		Loop through and release all allocated global buffers
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
static void FreeExtraGlobalBuffers( )
{
	TpsAssert( ( _ExtraGlobalBuffers.Count() == 0 ), "Extra global buffers were needed, and not freed." );

	YGlobalBufferIterator	iterator	= _ExtraGlobalBuffers.Start();
	for( ; iterator != _ExtraGlobalBuffers.End(); ++iterator )
	{
		delete [] (*iterator);
		*iterator	= NULL;
	}

	_ExtraGlobalBuffers.Empty();
}
	

#ifdef	TPSDEBUG

inline uLONG* _GetStartTagPtr(uBYTE* pBuffer)	{ return (uLONG*)pBuffer; }
inline uLONG* _GetBufferSizePtr(uBYTE* pBuffer)	{ return (uLONG*)(_GetStartTagPtr(pBuffer)+1); }
inline uBYTE* _GetUserDataPtr(uBYTE* pBuffer)	{ return (uBYTE*)(_GetBufferSizePtr(pBuffer)+1); }
inline uLONG& _GetBufferSize(uBYTE* pBuffer)		{ return *_GetBufferSizePtr(pBuffer); }
inline uLONG* _GetEndTagPtr(uBYTE* pBuffer)		{ return (uLONG*)( _GetUserDataPtr(pBuffer) + _GetBufferSize(pBuffer) ); }
inline uBYTE* _GetBaseBufferPtr(uBYTE* pBuffer)	{ return (pBuffer - sizeof(uLONG) - sizeof(kBufferGuard)); }

#else

inline uLONG* _GetBufferSizePtr(uBYTE* pBuffer)	{ return (uLONG*)pBuffer; }
inline uBYTE* _GetUserDataPtr(uBYTE* pBuffer)	{ return (uBYTE*)(_GetBufferSizePtr(pBuffer)+1); }
inline uLONG& _GetBufferSize(uBYTE* pBuffer)		{ return *_GetBufferSizePtr(pBuffer); }
inline uBYTE* _GetBaseBufferPtr(uBYTE* pBuffer)	{ return (pBuffer - sizeof(uLONG)); }

#endif


#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	TestAndInstallBufferGuards( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline	void TestAndInstallBufferGuards( uBYTE* pBuffer, BOOLEAN fTest )
{

#ifndef IMAGER
	RLock lock( globalBufferCriticalSection );
#endif	// IMAGER

	TpsAssert( _pGlobalBuffers, "Buffer does not exist" );
	if ( !_pGlobalBuffers )
		return;

	uLONG	ulSize	= _GetBufferSize( pBuffer );
	//	Test if requested
	if (fTest)
	{
		TpsAssert( (*_GetStartTagPtr(pBuffer)) == kBufferGuard, "Start Buffer Guard has been modified" );
		TpsAssert( (*_GetEndTagPtr(pBuffer)) == kBufferGuard, "End Buffer Guard has been modified" );
	}

	//	Install the Guards
	(*_GetStartTagPtr(pBuffer))	= kBufferGuard;
	(*_GetEndTagPtr(pBuffer))		= kBufferGuard;
}
#endif	//	TPSDEBUG

// ****************************************************************************
//
// Function Name:		StartupGlobalBuffer
//
// Description:		Allocates the global buffers
//
// Returns:				TRUE if the allocation succeeded, FALSE otherwise
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN StartupGlobalBuffer( )
{
int		i;
LPUBYTE	pBuffer;

#ifndef IMAGER
	RLock lock( globalBufferCriticalSection );
#endif	// IMAGER

	TpsAssert( ( _pGlobalBuffers == NULL ), "StartupGlobalBuffer called twice." );

	//	Create Global Buffer
	try
	{
		//	Size is DefaultGlobalSize - size field (- Guards if debug)
		_pGlobalBuffers			= new uBYTE*[ kNumGlobalBuffers ];
//j		_pfBufferUsed				= new BOOLEAN[ kNumGlobalBuffers ];
		for (i = 0; i < kNumGlobalBuffers; ++i)	//	initialize incase of failure
		{
			_pGlobalBuffers[i]	= NULL;
			_pfBufferUsed[i]		= FALSE;
		}
		for (i = 0; i < kNumGlobalBuffers; ++i)
		{
			_pGlobalBuffers[i]	= new uBYTE[ kGlobalBufferSize ];
			pBuffer	= _pGlobalBuffers[i];
			_GetBufferSize(pBuffer)	= kDefaultGlobalBufferSize;
			Debug( TestAndInstallBufferGuards( pBuffer, FALSE ) );
		}
	}
	catch ( ... )
	{
		FreeExtraGlobalBuffers( );
		if ( _pGlobalBuffers )
		{
			for ( int i = 0; i < kNumGlobalBuffers; ++i )
				delete [] _pGlobalBuffers[i];
			delete [] _pGlobalBuffers;
		}
//j		delete [] _pfBufferUsed;
		return FALSE;
	}

	return TRUE;
}

// ****************************************************************************
//
// Function Name:		ShutdownGlobalBuffer
//
// Description:		Deallocates the global buffers
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void ShutdownGlobalBuffer( )
{
#ifndef IMAGER
	RLock lock( globalBufferCriticalSection );
#endif	// IMAGER

	FreeExtraGlobalBuffers( );
	if ( _pGlobalBuffers )
	{
		for (int i = 0; i < kNumGlobalBuffers; ++i)
		{
			Debug( TestAndInstallBufferGuards( _pGlobalBuffers[i], TRUE ) );
			delete [] _pGlobalBuffers[i];
		}
		delete [] _pGlobalBuffers;
	}
	_pGlobalBuffers			= NULL;
//j	delete [] _pfBufferUsed;
}

// ****************************************************************************
//
// Function Name:		GetGlobalBuffer
//
// Description:		Gets the global buffer
//
// Returns:				The buffer
//
// Exceptions:			None
//
// ****************************************************************************
//
uBYTE* GetGlobalBuffer( uLONG ulRequestedBufferSize )
{
#ifndef IMAGER
	RLock lock( globalBufferCriticalSection );
#endif	// IMAGER

	int		i;
	uBYTE*	pBuffer = NULL;

	if ( ulRequestedBufferSize <= kDefaultGlobalBufferSize )
	{
		//
		//		Loop to find a free buffer...
		for ( i = 0; i < kNumGlobalBuffers; ++i )
		{
			if ( !_pfBufferUsed[i] )
			{
				//	mark it used and break out of loop
				pBuffer	= _pGlobalBuffers[i];
				_pfBufferUsed[i] = TRUE;
#ifdef	TPSDEBUG
				TestAndInstallBufferGuards( pBuffer, TRUE );
#endif	//	TPSDEBUG
				break;
			}
		}
	}
	else
		i = kNumGlobalBuffers;

	if ( i == kNumGlobalBuffers )
	{
		//	If no free buffer was found from the initially allocated list, create and return
		TpsAssert( _ExtraGlobalBuffers.Count() <= kWarnGlobalBufferUsage, "Using more than 5 Extra Global Buffers" );
		uLONG		ulBufferSize = ulRequestedBufferSize + sizeof(uLONG);
		Debug( ulBufferSize += sizeof(kBufferGuard) + sizeof(kBufferGuard) );
		pBuffer	= new uBYTE[ ulBufferSize ];
		_ExtraGlobalBuffers.InsertAtEnd( pBuffer );
		_GetBufferSize(pBuffer)	= ulRequestedBufferSize;
		Debug( TestAndInstallBufferGuards( pBuffer, FALSE ) );
	}

	return _GetUserDataPtr(pBuffer);
}

// ****************************************************************************
//
// Function Name:		ReleaseGlobalBuffer
//
// Description:		Gets the global buffer
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void ReleaseGlobalBuffer( uBYTE* pBuffer )
{	
#ifndef IMAGER
	RLock lock( globalBufferCriticalSection );
#endif	// IMAGER
					
	int		i;
	//
	//		Adjust buffer pointer if debug build and for size
	pBuffer	= _GetBaseBufferPtr(pBuffer);

	//
	//		Loop to find the buffer given...
	for ( i = 0; i < kNumGlobalBuffers; ++i )
	{
		if (pBuffer == _pGlobalBuffers[i])
		{		//	Found it!!!...
			TpsAssert( _pfBufferUsed[i], "Releasing a buffer that didn't know it was being used?" );
			Debug( TestAndInstallBufferGuards( pBuffer, TRUE ); )
			//
			//		Mark it free
			_pfBufferUsed[i]	= FALSE;
			pBuffer				= NULL;
			break;
		}
	}

	//	Not found in the preallocated list, try to find it in the ExtraGlobalBuffers list
	if ( i == kNumGlobalBuffers )
	{
		YGlobalBufferIterator	iterator	= _ExtraGlobalBuffers.Start( );
		for( ; iterator != _ExtraGlobalBuffers.End( ); ++iterator )
		{
			if ( pBuffer == (*iterator) )
			{		//	Found it!!!...
				Debug( TestAndInstallBufferGuards( pBuffer, TRUE ); )
				//
				//		Mark it free
				delete [] pBuffer;
				_ExtraGlobalBuffers.RemoveAt( iterator );
				pBuffer				= NULL;
				break;
			}
		}
	}
	
	TpsAssert( pBuffer == NULL, "Nice buffer... where'd you get it??" );
}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetGlobalBufferSize( )
//
//  Description:		The Global Buffer Size.
//
//  Returns:			Default Size of Buffer if NULL, or Size that is stored in buffer
//
//  Exceptions:		None
//
// ****************************************************************************
//
uLONG GetGlobalBufferSize( LPUBYTE pBuffer )
{
#ifndef IMAGER
	RLock lock( globalBufferCriticalSection );
#endif	// IMAGER

	if ( pBuffer == NULL )
	{
		return kDefaultGlobalBufferSize;
	}
	else
	{
		pBuffer	= _GetBaseBufferPtr(pBuffer);
		Debug( TestAndInstallBufferGuards( pBuffer, TRUE ) );
		return _GetBufferSize(pBuffer);
	}
}

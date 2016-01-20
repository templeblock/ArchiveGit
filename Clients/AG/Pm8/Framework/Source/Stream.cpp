// ****************************************************************************
//
//  File Name:			Stream.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RStream class
//
//  Portability:		Cross platform
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
//  $Logfile:: /PM8/Framework/Source/Stream.cpp           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include		"Stream.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include		"ChunkyStorage.h"

#ifndef		kFILETESTPROJECT
	#ifndef		IMAGER
	#include		"ApplicationGlobals.h"
	#endif
#endif

// ****************************************************************************
//
// Function Name:		RStream::RStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RStream::RStream( ):
	m_fCanRandomAccess(FALSE), m_eOpenAccess(kReadWrite), m_fPreserveFileDate( FALSE )
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RStream::~RStream( )
//
// Description:		Destructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RStream::~RStream( )
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RStream::Open( EAccessMode eOpenMode )
//
// Description:		open the stream
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RStream::Open( EAccessMode /*eOpenMode*/ )
	{
#ifndef	IMAGER
	if ( NULL == GetOpenStreamList() )
		GetOpenStreamList() = new ROpenStreamList;

	ROpenStreamInfo rInfo( this );
	if ( GetOpenStreamList()->Count() )
		{
		
		//	make sure were not already on the list
		ROpenStreamList::YIterator	iterator = GetOpenStreamList()->Start( );
		for( ; iterator != GetOpenStreamList()->End(); ++iterator)
			if ( (*iterator).m_pStream == this )
				break;

		TpsAssert(iterator == GetOpenStreamList()->End(), "Open Stream list malformed; apparent double open found" )
		}
	
	GetOpenStreamList()->InsertAtEnd( rInfo );
#endif	// IMAGER
	}

// ****************************************************************************
//
// Function Name:		RStream::Close( )
//
// Description:		close the stream
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RStream::Close( )
	{
#ifndef	IMAGER
//	TRACE( "close %x\n", this );
	TpsAssert( 0 != GetOpenStreamList()->Count(), "Open Stream list underflow" );
		
	//	find ourselves on the list
	ROpenStreamList::YIterator	iterator = GetOpenStreamList()->Start( );
	for( ; iterator != GetOpenStreamList()->End(); ++iterator )
		if ( (*iterator).m_pStream == this )
			break;

	TpsAssert(iterator != GetOpenStreamList()->End(), "Open Stream list malformed; apparent double close found" )
	GetOpenStreamList()->RemoveAt( iterator );
#endif	// IMAGER
	}


// ****************************************************************************
//
//  Function Name:	RStream::WriteFailed( )
//
//  Description:		We failed while writing. Remember not to write again.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStream::WriteFailed( const BOOLEAN fDeleteOnClose )
	{
	m_eOpenAccess = kRead;
	if( fDeleteOnClose )
		m_fDeleteOnClose = TRUE;	//	clean up the stream after ourselves
	}

// ****************************************************************************
//
// Function Name:		RStream::GetAccessMode( )
//
// Description:		return the access mode for this stream
//
// Returns:				m_eOpenAccess
//
// Exceptions:			None
//
// ****************************************************************************
//
EAccessMode RStream::GetAccessMode( )
	{
	return m_eOpenAccess;
	}

// ****************************************************************************
//
// Function Name:		RStream::AtEnd( )
//
// Description:		Return TRUE if at end of file
//
// Returns:				end of file condition
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RStream::AtEnd( ) const
	{
	//	we are at the end of this stream if the stream pointer is after the last char
	return static_cast<BOOLEAN>( GetSize() == GetPosition() );
	}

// ****************************************************************************
//
// Function Name:		RStream::SystemStartup( )
//
// Description:		startup the file system
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RStream::SystemStartup( )
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RStream::SystemShutdown( )
//
// Description:		shutdown the file system
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RStream::SystemShutdown( )
	{
	RChunkStorage::DeleteBackstore();

#ifndef	IMAGER
	ROpenStreamList* rList = GetOpenStreamList();
	TpsAssert( !rList || (0 == rList->Count()), "Streams left open" );

	delete GetOpenStreamList();
#endif	// IMAGER

	}


//	Define the open streams list to use in the file test project when we can't use
//	the one in Renaissance application globals.
#ifdef kFILETESTPROJECT
// ****************************************************************************
//
//  Function Name:	GetOpenStreamList( )
//
//  Description:		Return the open streams list
//
//  Returns:			_pOpenStreamList
//
//  Exceptions:		None
//
// ****************************************************************************
//
static ROpenStreamList* _pOpenStreamList = NULL;

#ifdef _WINDOWS // GCB 10/21/96 fix "function defined inline after being called" compile error on the Mac
inline ROpenStreamList*& GetOpenStreamList( )
#else
ROpenStreamList*& GetOpenStreamList( )
#endif
	{
	if ( NULL == _pOpenStreamList )
		_pOpenStreamList = new ROpenStreamList;

	return _pOpenStreamList;
	}
#endif	//	kFILETESTPROJECT


// ****************************************************************************
//
// Function Name:		RStream::PreserveFileDate( const BOOLEAN fPreserveFileDate )
//
// Description:		Change the file's preserve file date status.
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
void RStream::PreserveFileDate( const BOOLEAN fPreserveFileDate /* = TRUE*/ )
	{
	m_fPreserveFileDate = fPreserveFileDate;	
	}


// ****************************************************************************
//
// Function Name:		RStream::ShouldPreserveFileDate( )
//
// Description:		Return the file's preserve file date status.
//
// Returns:				^
//
// Exceptions:			none
//
// ****************************************************************************
//
BOOLEAN RStream::ShouldPreserveFileDate( )
	{
	return m_fPreserveFileDate;	
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RStream::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RStream::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RStream, this );
	}

#endif	// TPSDEBUG


// ****************************************************************************
//
// Function Name:		ROpenStreamInfo::ROpenStreamInfo( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
ROpenStreamInfo::ROpenStreamInfo( ):
	m_pStream(NULL), m_fOwner(FALSE)
	{
	NULL;
	}


// ****************************************************************************
//
// Function Name:		ROpenStreamInfo::ROpenStreamInfo( RStream* pStream, BOOLEAN fOwner )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
ROpenStreamInfo::ROpenStreamInfo( RStream* pStream, BOOLEAN fOwner /*= FALSE*/ ):
	m_pStream( pStream ), m_fOwner( fOwner )
	{
	NULL;
	}

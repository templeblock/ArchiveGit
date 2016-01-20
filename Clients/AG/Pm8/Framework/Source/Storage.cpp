// ****************************************************************************
//
//  File Name:			Storage.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RStorage class
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
//  $Logfile:: /PM8/Framework/Source/Storage.cpp                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include		"FrameworkIncludes.h"

ASSERTNAME

#include		"Storage.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include		"FileStream.h"
#include		"MemoryStream.h"

#ifndef kFILETESTPROJECT
	#include		"ApplicationGlobals.h"
#endif

// ****************************************************************************
//
//  Function Name:	RStorage::RStorage( )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStorage::RStorage( )
	:m_pStream(NULL)
	, m_fDeleteStreamOnClose(FALSE)
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RStorage::RStorage( RStream* pStream )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None`
//
// ****************************************************************************
//
RStorage::RStorage( RStream* pStream )
	:m_pStream(pStream)
	, m_fDeleteStreamOnClose(FALSE)
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RStorage::RStorage( const RMBCString& rFilename, EAccessMode eOpenMode )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		YES.
//
// ****************************************************************************
//
RStorage::RStorage( const RMBCString& rFilename, EAccessMode eOpenMode )
	:m_pStream(NULL)
	, m_fDeleteStreamOnClose(TRUE)
	{
	//	if we are opening the file for write access then try to find out whether its open for write access
	{
	ROpenStreamList::YIterator	iterator = GetOpenStreamList()->Start();
	for( ; iterator != GetOpenStreamList()->End(); ++iterator )
		{
		RMemoryMappedStream* pStream = dynamic_cast<RMemoryMappedStream*>( (*iterator).m_pStream );
			
		//	stop if another storage already has this file open
		if ( pStream && (rFilename == pStream->GetFilename()) && (eOpenMode != kRead) )
			ThrowException( kShareViolation );
		}
	}
	
	RFileStream *prfTemp = NULL;

	try
		{
		//	install file info
		//	open stream with file info and given access mode
		prfTemp = new RFileStream( rFilename );

		prfTemp->Open( eOpenMode );
		m_pStream = prfTemp;
		}
	catch(...)
		{
		//	if we catch an exception opening a read/write file
		//	we need to clean up on the file delete
		if( (NULL != prfTemp) && (kRead != eOpenMode) )
			prfTemp->DeleteOnClose( FALSE );
		delete prfTemp;
		throw;
		}
	}


// ****************************************************************************
//
//  Function Name:	RStorage::RStorage( RFileStream* pStream )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStorage::RStorage( RFileStream* pStream )
	:m_pStream(pStream)
	, m_fDeleteStreamOnClose(FALSE)
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RStorage::RStorage( RMemoryStream* pStream )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStorage::RStorage( RMemoryStream* pStream )
	:m_pStream(pStream)
	, m_fDeleteStreamOnClose(FALSE)
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RStorage::~RStorage( )
//
//  Description:		dtor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStorage::~RStorage( )
	{
	if ( m_fDeleteStreamOnClose )
		delete m_pStream;
	}


// ****************************************************************************
//
//  Function Name:	RStorage::Write( const uBYTE* pubBuffer, const YStreamLength yBufferSize )
//
//  Description:		write given data to our stream
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RStorage::Write( YStreamLength yBufferSize, const uBYTE* pubBuffer )
	{
	TpsAssert( NULL != m_pStream, "NULL member stream" );
	m_pStream->Write( yBufferSize, pubBuffer );
	}


// ****************************************************************************
//
//  Function Name:	RStorage::Read( const uBYTE* pubBuffer, YStreamLength yBufferSize )
//
//  Description:		read given data from our stream.
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RStorage::Read( YStreamLength yBufferSize, uBYTE* pubBuffer )
	{
	TpsAssert( NULL != m_pStream, "NULL member stream" );
	m_pStream->Read( yBufferSize, pubBuffer );
	}


// ****************************************************************************
//
//  Function Name:	RStorage::SeekAbsolute( const YStreamLength ySeekPosition )
//
//  Description:		move stream pointer
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RStorage::SeekAbsolute( const YStreamLength ySeekPosition )
	{
	TpsAssert( NULL != m_pStream, "NULL member stream" );
	m_pStream->SeekAbsolute( ySeekPosition );
	}


// ****************************************************************************
//
//  Function Name:	RStorage::SeekRelative( const YStreamOffset ySeekOffset )
//
//  Description:		move stream pointer based on current position.
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RStorage::SeekRelative( const YStreamOffset ySeekOffset )
	{
	TpsAssert( NULL != m_pStream, "NULL member stream" );
	m_pStream->SeekRelative( ySeekOffset );
	}


// ****************************************************************************
//
//  Function Name:	RStorage::Flush( )
//
//  Description:		flush stream to disk
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RStorage::Flush( )
	{
	TpsAssert( NULL != m_pStream, "NULL member stream" );
	m_pStream->Flush( );
	}


// ****************************************************************************
//
//  Function Name:	RStorage::Copy( RStream* pStream )
//
//  Description:		copy given stream into member stream.
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RStorage::Copy( RStream* pStream )
	{
	TpsAssert( NULL != m_pStream, "NULL member stream" );
	m_pStream->Copy( pStream );
	}



// ****************************************************************************
//
//  Function Name:	RStorage::GetAbsolutePosition( )
//
//  Description:		get current position in stream.
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
YStreamLength RStorage::GetPosition( ) const
	{
	TpsAssert( NULL != m_pStream, "NULL member stream" );
	return m_pStream->GetPosition( );
	}



// ****************************************************************************
//
//  Function Name:	RStorage::GetStorageStream( )
//
//  Description:		return member stream.
//
//  Returns:			member stream;
//
//  Exceptions:		File
//
// ****************************************************************************
//
RStream* RStorage::GetStorageStream( ) const
	{
	return m_pStream;
	}



// ****************************************************************************
//
//  Function Name:	RStorage::GetPlatformTag( )
//
//  Description:		return platform tag.
//
//  Returns:			platform tag;
//
//  Exceptions:		File
//
// ****************************************************************************
//
YPlatformTag RStorage::GetPlatformTag( )
	{
	return MacWinDos( kMac, kWin, xxx );
	}


// ****************************************************************************
//
//  Function Name:	RStorage::DeleteStreamOnClose( )
//
//  Description:		delete the storage's stream when the storage is destructed.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStorage::DeleteStreamOnClose( )
	{
	m_fDeleteStreamOnClose = TRUE;
	}


// ****************************************************************************
//
//  Function Name:	RStorage::WriteFailed( )
//
//  Description:		We failed while writing. Remember not to write again.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStorage::WriteFailed( const BOOLEAN fDeleteOnClose )
	{
	if( m_pStream )
		{
		m_pStream->WriteFailed( fDeleteOnClose );
		if( fDeleteOnClose)
			this->DeleteStreamOnClose( );
		}
	}


// ****************************************************************************
//
// Function Name:		RStorage::PreserveFileDate( const BOOLEAN fPreserveFileDate )
//
// Description:		Change the file's preserve file date status.
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
void RStorage::PreserveFileDate( const BOOLEAN fPreserveFileDate /* = TRUE*/ )
	{
	m_pStream->PreserveFileDate( fPreserveFileDate );	
	}


// ****************************************************************************
//
// Function Name:		RStorage::ShouldPreserveFileDate( )
//
// Description:		Return the file's preserve file date status.
//
// Returns:				^
//
// Exceptions:			none
//
// ****************************************************************************
//
BOOLEAN RStorage::ShouldPreserveFileDate( )
	{
	return m_pStream->ShouldPreserveFileDate( );	
	}


#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RStorage::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RStorage::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RStorage, this );
	}

#endif	// TPSDEBUG


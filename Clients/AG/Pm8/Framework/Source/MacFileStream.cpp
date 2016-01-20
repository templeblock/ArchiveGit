// ****************************************************************************
//
//  File Name:			MacFileStream.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood, Greg Beddow
//
//  Description:		Definition of the Macintosh version of the RFileStream class
//
//  Portability:		Mac specific
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
//  $Logfile:: /PM8/Framework/Source/MacFileStream.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "FileStream.h"

// ****************************************************************************
//
// Function Name:		RFileStream::RFileStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RFileStream::RFileStream( )
	: RMemoryMappedStream( )
{
	NULL;
}

// ****************************************************************************
//
// Function Name:		RFileStream::RFileStream( const RMBCString& rFilename )
//
// Description:		Constructor with file name argument
//
// Returns:				nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RFileStream::RFileStream( const RMBCString& rFilename )
	: RMemoryMappedStream ( rFilename )
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RFileStream::~RFileStream( )
//
// Description:		Destructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RFileStream::~RFileStream( )
{
	NULL;
}

// ****************************************************************************
//
// Function Name:		RFileStream::Open( )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::Open( EAccessMode eOpenMode )
{
	RMemoryMappedStream::Open( eOpenMode );
}

// ****************************************************************************
//
// Function Name:		RFileStream::Open( )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
//  Exceptions:		throws file exception
//
//
// ****************************************************************************
//
void RFileStream::Open( const char* szFilename, EAccessMode access )
{
	RMemoryMappedStream::Open( szFilename, access );
}

// ****************************************************************************
//
// Function Name:		RFileStream::Open( )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
//  Exceptions:		throws file exception
//
//
// ****************************************************************************
//
void RFileStream::Open( const FSSpecPtr pFileSpec, EAccessMode access )
{
	RMemoryMappedStream::Open( pFileSpec, access );
}

// ****************************************************************************
//
// Function Name:		RFileStream::Open( )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::Open( const RMBCString& rFilename, EAccessMode access )
{
	RMemoryMappedStream::Open( rFilename, access );
}

// ****************************************************************************
//
// Function Name:		RFileStream::GetFileIdentifier( )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RFileStream::GetFileIdentifier( XPlatformFileID& xpID )
{
	RMemoryMappedStream::GetFileIdentifier( xpID );
}

// ****************************************************************************
//
// Function Name:		RFileStream::CreateTempFile( )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RFileStream::CreateTempFile( )
{
	RMemoryMappedStream::CreateTempFile( );
}


// ****************************************************************************
//
// Function Name:		RFileStream::CreateTempFile( const XPlatformFileID& xpID )
//
// Description:		make this a file with a temp name in directory specified by the file id
//
// Returns:				none
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::CreateTempFile( const XPlatformFileID& xpID )
{	
	RMemoryMappedStream::CreateTempFile( xpID );
}


// ****************************************************************************
//
// Function Name:		RFileStream::CreateTempFile( )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RFileStream::CreateTempFile( const RMBCString& rPath )
{
	RMemoryMappedStream::CreateTempFile( rPath );
}

// ****************************************************************************
//
// Function Name:		RFileStream::Rename(const RMBCString& rFilename )
//
// Description:		rename the disk file specified by rFilename; it must not be open
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RFileStream::Rename ( const RMBCString& rFilename )
{
	RMemoryMappedStream::Rename( rFilename );
}


// ****************************************************************************
//
// Function Name:		RFileStream::Rename(const XPlatformFileID& xpID )
//
// Description:		rename the disk file specified by xpID; it must not be open
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RFileStream::Rename ( const XPlatformFileID& xpID )
{
	RMemoryMappedStream::Rename( xpID );
}

// ****************************************************************************
//
// Function Name:		RFileStream::Delete( )
//
// Description:		close the file if it's open, then delete it
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RFileStream::Delete( )
{
	RMemoryMappedStream::Delete( );
}

// ****************************************************************************
//
// Function Name:		RFileStream::DeleteOnClose( )
//
// Description:		remembers to delete the file backing this stream when it is closed
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RFileStream::DeleteOnClose( BOOLEAN fDelete )
{
	RMemoryMappedStream::DeleteOnClose( fDelete );
}

// ****************************************************************************
//
// Function Name:		RFileStream::Close( )
//
// Description:		close the file, then delete it if necessary
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RFileStream::Close( )
{
	RMemoryMappedStream::Close( );
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RFileStream::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RFileStream::Validate( ) const
{
	RObject::Validate( );
	TpsAssertIsObject( RFileStream, this );
	TpsAssert( m_FileRef != 0, "Invalid File reference" );
}

#endif	// TPSDEBUG

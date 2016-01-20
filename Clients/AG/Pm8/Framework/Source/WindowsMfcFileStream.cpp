// ****************************************************************************
//
//  File Name:			WindowsMfcFileStream.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMfcFileStream class
//
//  Portability:		Windows
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
//  $Logfile:: /PM8/Framework/Source/WindowsMfcFileStream.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "MfcFileStream.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
// Function Name:		RMfcFileStream::RMfcFileStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RMfcFileStream::RMfcFileStream( CFile& file ) : RStream( ), m_pFile( &file )
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::Read( )
//
// Description:		Reads data
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RMfcFileStream::Read( YStreamLength yBytesToRead, uBYTE* pBuffer )
	{
	m_pFile->Read( pBuffer, yBytesToRead );
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::Write( )
//
// Description:		Writes data
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RMfcFileStream::Write( YStreamLength yBytesToRead, const uBYTE* pBuffer )
	{
	UntestedCode();
	m_pFile->Write( pBuffer, yBytesToRead );
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::SeekRelative( const YStreamOffset ySeekOffset )
//
// Description:		Reposition the file pointer based on the given relative position.
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RMfcFileStream::SeekRelative( const YStreamOffset ySeekOffset )
	{
	UntestedCode();
	m_pFile->Seek( ySeekOffset, CFile::current );
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::SeekAbsolute( const YStreamLength position )
//
// Description:		Reposition the file pointer to the given absolute position.
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RMfcFileStream::SeekAbsolute( const YStreamLength yPosition )
	{
	m_pFile->Seek( yPosition, CFile::begin );
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::GetPosition( )
//
// Description:		GetPosition
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
YStreamPosition RMfcFileStream::GetPosition( ) const
	{
	return m_pFile->GetPosition( );
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::GetSize( )
//
// Description:		return the size of the stream
//
// Returns:				size of stream
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
YStreamPosition RMfcFileStream::GetSize( ) const
	{
	return m_pFile->GetLength( );
	}



// ****************************************************************************
//
// Function Name:		RMfcFileStream::Close( )
//
// Description:		close the file
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RMfcFileStream::Close( )
	{
	UntestedCode();
	m_pFile->Close( );
	}



// ****************************************************************************
//
// Function Name:		RMfcFileStream::Open( EAccessMode eOpenMode )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RMfcFileStream::Open( EAccessMode /* eOpenMode */ )
	{
	UnimplementedCode( );
	}


// ****************************************************************************
//
// Function Name:		RMfcFileStream::Copy( RStream* pStream )
//
// Description:		Copy given stream into this stream from the current 
//								stream position to the current stream position
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RMfcFileStream::Copy( RStream* /* pStream */ )
	{
	UnimplementedCode();
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::Copy( RStream* pStream, YStreamLength yLength )
//
// Description:		Copy yLength bytes from the given stream into this stream from the current 
//								stream position to the current stream position
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RMfcFileStream::Copy( RStream* /* pStream */, const YStreamLength /* yLength */ )
	{
	UnimplementedCode();
	}


// ****************************************************************************
//
// Function Name:		RMfcFileStream::Flush( )
//
// Description:		flush file's buffers to disk
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RMfcFileStream::Flush( )
	{
	UntestedCode();
	m_pFile->Flush( );
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::GetBuffer( const YStreamLength yLength )
//
// Description:		returns a buffer of the specified size containing stream
//							contents after the current marker
//
// Returns:				^
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
PUBYTE RMfcFileStream::GetBuffer( const YStreamLength /*yLength*/ )
	{
	UnimplementedCode();
	return 0;
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::ReleaseBuffer( PUBYTE pBuffer )
//
// Description:		release the given buffer which was allocated by getbuffer
//
// Returns:				none
//
// Exceptions:			none
//
// ****************************************************************************
//
void RMfcFileStream::ReleaseBuffer( PUBYTE /*pBuffer*/ ) const
	{
	UnimplementedCode();
	}


// ****************************************************************************
//
// Function Name:		RMfcFileStream::Grow( const YStreamLength yLength )
//
// Description:		Increase the stream size by the given size.
//							Use this fcn when you don't care about the contents
//							of a new stream area.
//
// Returns:				none
//
// Exceptions:			File
//
// ****************************************************************************
//
void RMfcFileStream::Grow( const YStreamLength /*yLength*/ )
	{
	UnimplementedCode();
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::FillWithByte( const uBYTE ubPattern, const YStreamLength yLength )
//
// Description:		write the given byte into the stream starting at the current marker. Write yLength bytes
//
// Returns:				none
//
// Exceptions:			File
//
// ****************************************************************************
//
void RMfcFileStream::FillWithByte( const uBYTE /*ubPattern*/, const YStreamLength /*yLength*/ )
	{
	UnimplementedCode();
	}

// ****************************************************************************
//
// Function Name:		RMfcFileStream::StreamExists( ) const
//
// Description:		return whether the file backing our stream exists
//
// Returns:				^
//
// Exceptions:			File
//
// ****************************************************************************
//
BOOLEAN RMfcFileStream::StreamExists( ) const
	{
		UnimplementedCode();
		return FALSE;
	}

#ifdef	TPSDEBUG


// ****************************************************************************
//
// Function Name:		RMfcFileStream::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RMfcFileStream::Validate( ) const
	{
	RStream::Validate( );
	TpsAssertIsObject( RMfcFileStream, this );
	ASSERT_VALID( m_pFile );
	}

#endif	// TPSDEBUG


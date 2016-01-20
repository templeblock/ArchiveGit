// ****************************************************************************
//
//  File Name:			BufferStream.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Implementation of the RBufferStream class
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
//  $Logfile:: /PM8/Framework/Source/BufferStream.cpp     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include		"BufferStream.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
// Function Name:		RBufferStream::RBufferStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RBufferStream::RBufferStream( YStreamLength growSize )
	: m_fAllocated( TRUE ),
	  m_pData( NULL ),
	  m_DataLength( 0 ),
	  m_StreamLength( 0 ),
	  m_Position( 0 ),
	  m_GrowSize( growSize )
	{
	m_pData			= new uBYTE[ 0 ];
	m_DataLength	= 0;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::RBufferStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RBufferStream::RBufferStream( LPUBYTE pBuffer, YStreamLength len )
	: m_fAllocated( FALSE ),
	  m_pData( pBuffer ),
	  m_DataLength( len ),
	  m_StreamLength( len ),
	  m_Position( 0 ),
	  m_GrowSize( kMemoryExtra )
	{
	;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::RBufferStream( )
//
// Description:		Destructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RBufferStream::~RBufferStream( )
	{
	if ( m_fAllocated )
		delete [] m_pData;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::Read( )
//
// Description:		Read the given amount of data from the stream into the given
//							buffer;
//
// Returns:				Nothing
//
// Exceptions:			kEndOfFile
//
// ****************************************************************************
//
void RBufferStream::Read( YStreamLength yBytesToRead, uBYTE* pBuffer )
	{
	YStreamLength	remainingLength	= m_StreamLength - m_Position;
	YStreamLength	copyAmount			= yBytesToRead;

	//	If there is not enough data in the buffer, reset copyAmount
	if ( remainingLength < yBytesToRead )
		copyAmount = remainingLength;
	
	//	Copy the data
	memmove( pBuffer, (m_pData + m_Position), copyAmount );
	//	Adjust the Position
	m_Position	+= copyAmount;

	//	If I did not read it all, throw end of file
	if (copyAmount < yBytesToRead)
		throw kEndOfFile;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::Write( )
//
// Description:		Write the given data into the data pointer
//
// Returns:				nothing
//
// Exceptions:			kMemory, kAccessDenied
//
// ****************************************************************************
//
void RBufferStream::Write( YStreamLength yBytesToWrite, const uBYTE* pBuffer )
	{
	YStreamLength	remainingLength	= m_StreamLength - m_Position;

	//	Make sure there is enough room to write the data
	if ( yBytesToWrite > remainingLength )
		Grow( yBytesToWrite - remainingLength );

	//	Copy the data
	memmove( (m_pData + m_Position), pBuffer, yBytesToWrite );
	//	Adjust the Position
	m_Position	+= yBytesToWrite;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::GetPosition( )
//
// Description:		return the current seek position
//
// Returns:				m_Position
//
// Exceptions:			None
//
// ****************************************************************************
//
YStreamPosition	RBufferStream::GetPosition( ) const
	{
	return  m_Position;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::SeekAbsolute( )
//
// Description:		Seek to the given absolute location
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RBufferStream::SeekAbsolute( const YStreamLength ySeekPosition )
	{
	//	If seeking past end, move to end...
	m_Position = (ySeekPosition <= m_StreamLength)? ySeekPosition : m_StreamLength;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::SeekRelative( )
//
// Description:		Seek to a new location relative to the current position
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RBufferStream::SeekRelative( const YStreamOffset ySeekOffset )
	{
	m_Position	+= ySeekOffset;
	if ( m_Position < 0 )
		m_Position	= 0;
	else if ( m_Position > m_StreamLength )
		m_Position = m_StreamLength;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::Flush( )
//
// Description:		Flush the data (nothing for a memory stream)
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RBufferStream::Flush( )
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::Copy( )
//
// Description:		Copy the stream data
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RBufferStream::Copy( RStream* pStream )
	{
	Copy( pStream, pStream->GetSize() - pStream->GetPosition() );
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::Copy( )
//
// Description:		Copy the stream to this stream
//
// Returns:				nothing
//
// Exceptions:			kMemory, other Stream errors
//
// ****************************************************************************
//
void RBufferStream::Copy( RStream* pStream, const YStreamLength length )
	{
	uBYTE* pDataToCopy = new uBYTE[length];
	try
		{
		pStream->Read( length, pDataToCopy );
		Write( length, pDataToCopy );
		}
	catch( ... )
		{
		delete [] pDataToCopy;
		throw;
		}
	delete [] pDataToCopy;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::GetSize( )
//
// Description:		Return the size of the buffer
//
// Returns:				m_StreamLength
//
// Exceptions:			None
//
// ****************************************************************************
//
YStreamLength	RBufferStream::GetSize() const
	{
	return m_StreamLength;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::RBufferStream( )
//
// Description:		FillWithByte
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RBufferStream::FillWithByte( const uBYTE , const YStreamLength )
	{
	UnimplementedCode( );
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::Grow( )
//
// Description:		Grow the size of the buffer (if it can be grown)
//
// Returns:				nothing
//
// Exceptions:			kMemory, kAccessDenied
//
// ****************************************************************************
//
void RBufferStream::Grow( const YStreamLength yLength )
	{
	if ( (m_StreamLength + yLength) > m_DataLength )
		{
		if ( !m_fAllocated )
			throw kAccessDenied;	//	If I didn't allocate it, I can't grow it...

		YStreamLength	growSize	= (((yLength + m_GrowSize - 1) / m_GrowSize) * m_GrowSize );
		uBYTE*			pNewData	= new uBYTE[ m_DataLength + growSize ];
		memmove( pNewData, m_pData, m_StreamLength );
		//	Cleanup
		delete [] m_pData;
		//	Reset pointers 
		m_pData			= pNewData;
		m_DataLength	+= growSize;
		}

	//	StreamLength is now larger
	m_StreamLength	+= yLength;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::GetBuffer( )
//
// Description:		Return a buffer that is large enough to hold yLength data
//
// Returns:				m_pData
//
// Exceptions:			kMemory
//
// ****************************************************************************
//
PUBYTE RBufferStream::GetBuffer( const YStreamLength yLength )
	{
// GCB 1/8/99 - fix bug
#if 0
	Grow( yLength );
#else
	if (m_Position + yLength > m_DataLength)
		Grow( yLength );
#endif
	return m_pData + m_Position;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::ReleaseBuffer( )
//
// Description:		Release the buffer returned from GetBuffer (here it does nothing)
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void	RBufferStream::ReleaseBuffer( PUBYTE ) const
	{
	;
	}

// ****************************************************************************
//
// Function Name:		RBufferStream::RBufferStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN	RBufferStream::StreamExists( ) const
	{
	return FALSE;
	}


#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RBufferStream::RBufferStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RBufferStream::Validate( ) const
	{
	RStream::Validate( );
	TpsAssert( m_DataLength >= m_StreamLength, "Stream is longer than data." );
	TpsAssert( m_DataLength==0 || m_pData, "There is a length, but no data. " );
	}

#endif	// TPSDEBUG

// ****************************************************************************
//
//  File Name:			UIContextData.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RUIContextData class
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
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/UIContextData.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "UIContextData.h"
#include "ChunkyStorage.h"

const YChunkTag kUIContextDataChunkId = 'UIPD';
const YStreamLength kGrowBySize = 16;

// ****************************************************************************
//
//  Function Name:	RUIContextData::RUIContextData( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData::RUIContextData( )
	: m_BufferStream( kGrowBySize )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RUIContextData::Read( )
//
//  Description:		Reads the persistant data from a storage
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUIContextData::Read( RChunkStorage& storage )
	{
	// If our chunk exists, read the data
	if( !storage.Start( kUIContextDataChunkId, FALSE ).End( ) )
		{
		// Read in the length
		YStreamLength size;
		storage >> size;

		if( size )
			{
			// Allocate a temporary buffer
			uBYTE* pData = new uBYTE[ size ];

			// Read in the data
			storage.Read( size, pData );

			// Write it into the stream
			m_BufferStream.SeekAbsolute( 0 );
			m_BufferStream.Write( size, pData );

			// Delete the temporary buffer
			delete pData;
			}

		// Get back to the parent chunk
		storage.SelectParentChunk( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RUIContextData::Write( )
//
//  Description:		Writes the persistant data to a storage
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUIContextData::Write( RChunkStorage& storage ) const
	{
	// Put the data in a new chunk
	storage.AddChunk( kUIContextDataChunkId );

	// Get the length of the data
	YStreamLength size = m_BufferStream.GetSize( );

	// Write the length
	storage << size;

	if( size )
		{
		// Get the data buffer
		const_cast<RBufferStream&>( m_BufferStream ).SeekAbsolute( 0 );
		uBYTE* pData = const_cast<RBufferStream&>( m_BufferStream ).GetBuffer( 0 );

		// Write the data
		storage.Write( size, pData );

		// Release the data buffer
		m_BufferStream.ReleaseBuffer( pData );
		}

	// Get back to the parent chunk
	storage.SelectParentChunk( );
	}

// ****************************************************************************
//
//  Function Name:	RUIContextData::Read( )
//
//  Description:		Reads from this persistant data object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUIContextData::Read( YStreamLength yBytesToRead, uBYTE* pBuffer )
	{
	m_BufferStream.Read( yBytesToRead, pBuffer );
	}

// ****************************************************************************
//
//  Function Name:	RUIContextData::Write( )
//
//  Description:		Writes to this persistant data object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUIContextData::Write( YStreamLength yulBytesToRead, const uBYTE* pBuffer )
	{
	m_BufferStream.Write( yulBytesToRead, pBuffer );
	}

// ****************************************************************************
//
//  Function Name:	RUIContextData::SeekAbsolute( )
//
//  Description:		Seeks to an absolute position in this persistant data object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUIContextData::SeekAbsolute( const YStreamLength ySeekPosition )
	{
	m_BufferStream.SeekAbsolute( ySeekPosition );
	}

// ****************************************************************************
//
//  Function Name:	RUIContextData::GetSize( )
//
//  Description:		Return the size of the buffer stream
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YStreamLength RUIContextData::GetSize( ) const
	{
	return m_BufferStream.GetSize( );
	}

// ****************************************************************************
//
//  Function Name:	RUIContextData::GetPosition( )
//
//  Description:		Return the current write marker position of the buffer stream
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YStreamLength RUIContextData::GetPosition( ) const
	{
	return m_BufferStream.GetPosition( );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RUIContextData::RBufferStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RUIContextData::Validate( ) const
	{
	RArchive::Validate( );
	TpsAssertValid( &m_BufferStream );
	}

#endif	// TPSDEBUG

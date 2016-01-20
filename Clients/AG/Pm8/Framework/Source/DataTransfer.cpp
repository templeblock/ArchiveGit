// ****************************************************************************
//
//  File Name:			DataTransfer.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RDataTransfer class
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
//  $Logfile:: /PM8/Framework/Source/DataTransfer.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"DataTransfer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::RMemoryDataTransfer( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMemoryDataTransfer::RMemoryDataTransfer( )
	: m_hData( NULL ),
	  m_pData( NULL ),
	  m_fEnumerating( FALSE )
	{
	;
	}


// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::RMemoryDataTransfer( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMemoryDataTransfer::RMemoryDataTransfer( const RMemoryDataTransfer& rhs )
	: m_hData( NULL ),
	  m_pData( NULL ),
	  m_DataLength( rhs.m_DataLength ),
	  m_DataFormat( rhs.m_DataFormat ),
	  m_fEnumerating( rhs.m_fEnumerating )
	{
	//	If there is a handle, Copy the data
	if ( rhs.m_hData )
		m_hData = ::HandleToHandle( rhs.m_hData );
	//	otherwise just copy the pointer ( if it exists )
	else if ( rhs.m_pData )
		{
		m_pData	= new uBYTE[ m_DataLength ];
		memcpy( m_pData, rhs.m_pData, m_DataLength );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::~RMemoryDataTransfer( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMemoryDataTransfer::~RMemoryDataTransfer( )
	{
	if( m_hData )
		::FreeHandle( m_hData );

	delete[] m_pData;
	}

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::SetData( )
//
//  Description:		Sets data of the specified type. The caller retains
//							ownership of pData
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMemoryDataTransfer::SetData( YDataFormat dataFormat, const uBYTE* pData, YDataLength dataLength )
	{
	TpsAssert( pData != NULL, "NULL data pointer. Use PromiseData( ) to implement delayed rendering." );
	TpsAssert( dataLength > 0, "No data." );
	TpsAssert( m_hData == NULL && m_pData == NULL, "Data already set." );

	m_pData = new uBYTE[ dataLength ];
	memcpy( m_pData, pData, dataLength );
	m_DataLength = dataLength;
	m_DataFormat = dataFormat;
	}

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::SetData( )
//
//  Description:		Sets data of the specified type. The caller gives up 
//							ownership of the handle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMemoryDataTransfer::SetData( YDataFormat dataFormat, HNATIVE handle )
	{
	TpsAssert( m_hData == NULL && m_pData == NULL, "Data already set." );
	m_hData = handle;
	m_DataFormat = dataFormat;
	}

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::PromiseData( )
//
//  Description:		Called to promise that data of the specified type is
//							available through delayed rendering.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMemoryDataTransfer::PromiseData( YDataFormat )
	{
	TpsAssertAlways( "Delayed rendering not available." );
	}

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::GetData( )
//
//  Description:		Called to retrieve data of the specified format from this
//							data transfer protocol. The caller takes ownership of the
//							returned memory.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RMemoryDataTransfer::GetData( YDataFormat dataFormat, uBYTE*& pData, YDataLength& dataLength ) const
	{
	// Make sure data of the requested type is available
	if( !IsFormatAvailable( dataFormat ) )
		{
		pData = NULL;
		return;
		}

	// If data is already available as a pointer, return it
	if( m_pData )
		{
		pData = m_pData;
		dataLength = m_DataLength;
		}

	// Otherwise, convert the handle to a pointer
	else
		{
		::HandleToPointer( m_hData, pData, dataLength );
		::FreeHandle( m_hData );
		}

	const_cast<RMemoryDataTransfer*>( this )->m_hData = NULL;
	const_cast<RMemoryDataTransfer*>( this )->m_pData = NULL;
	}

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::GetData( )
//
//  Description:		Called to retrieve data of the specified format from this
//							data transfer protocol. The caller takes ownership of the
//							returned handle.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
HNATIVE RMemoryDataTransfer::GetData( YDataFormat dataFormat ) const
	{
	// Make sure data of the requested type is available
	if( !IsFormatAvailable( dataFormat ) )
		return NULL;

	HNATIVE hNative;

	// If the data is already available as a handle, return it
	if( m_hData )
		hNative = m_hData;

	// Otherwise, convert the pointer to a handle
	else
		{
		hNative = ::PointerToHandle( m_pData, m_DataLength );
		delete [] m_pData;
		}

	const_cast<RMemoryDataTransfer*>( this )->m_hData = NULL;
	const_cast<RMemoryDataTransfer*>( this )->m_pData = NULL;

	return hNative;
	}

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::IsFormatAvailable( )
//
//  Description:		Determines if the specified data format is available from
//							this data source.
//
//  Returns:			TRUE if data is available in the specified format
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMemoryDataTransfer::IsFormatAvailable( YDataFormat dataFormat ) const
	{
	return static_cast<BOOLEAN>( ( m_hData != NULL || m_pData != NULL ) && dataFormat == m_DataFormat );
	}

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::BeginEnumFormats( )
//
//  Description:		Sets this data transfer object up to enumerate available
//							data formats
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMemoryDataTransfer::BeginEnumFormats( ) const
	{
	const_cast<RMemoryDataTransfer*>( this )->m_fEnumerating = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RMemoryDataTransfer::GetNextFormat( )
//
//  Description:		Retrieves the next data format available. Formats are
//							returned in decreasing order of priority.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMemoryDataTransfer::GetNextFormat( YDataFormat& dataFormat ) const
	{
	if( m_fEnumerating )
		{
		dataFormat = m_DataFormat;
		const_cast<RMemoryDataTransfer*>( this )->m_fEnumerating = FALSE;
		return TRUE;
		}

	return FALSE;
	}

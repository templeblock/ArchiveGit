// ****************************************************************************
//
//  File Name:			StreamScript.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RStreamScript class
//
//  Portability:		cross platform
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
//  $Logfile:: /PM8/Framework/Source/StreamScript.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include		"StreamScript.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include		"FileStream.h"

// ****************************************************************************
//
// Function Name:		RStreamScript::RStreamScript( )
//
// Description:		ctor;
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
RStreamScript::RStreamScript( )
 :m_fDeleteOnClose( FALSE ), m_pStream( NULL )
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RStreamScript::RStreamScript( RFileStream& rStream )
//
// Description:		ctor; create and associate the given file with this script
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
RStreamScript::RStreamScript( RFileStream& rStream )
 :m_fDeleteOnClose(FALSE), m_pStream( &rStream )
	{
	NULL;
	}


// ****************************************************************************
//
// Function Name:		RStreamScript::RStreamScript( const RMBCString& rFilename, const RStream::EAccessMode eAccess )
//
// Description:		ctor; create and open the given file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
RStreamScript::RStreamScript( const RMBCString& rFilename, const EAccessMode eAccess )
 :m_fDeleteOnClose(FALSE), m_pStream( NULL)
	{
	try
		{
		m_pStream = new RFileStream( rFilename );
		Open( eAccess );
		}
	catch(...)
		{
		delete m_pStream;
		throw;
		}
	m_fDeleteOnClose = TRUE;
	}

// ****************************************************************************
//
// Function Name:		RStreamScript::GetStream( )
//
// Description:		returns stream backing the script
//
// Returns:				^
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
RStream& RStreamScript::GetStream( )
	{
	TpsAssert( NULL != m_pStream, "stream script not properly initialized" );
	return *m_pStream;
	}

// ****************************************************************************
//
// Function Name:		RStreamScript::AtEnd( )
//
// Description:		returns TRUE if the stream's marker is at the end of the stream
//
// Returns:				^
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
BOOLEAN RStreamScript::AtEnd( )
	{
	TpsAssert( NULL != m_pStream, "stream script not properly initialized" );
	return m_pStream->AtEnd( );
	}

// ****************************************************************************
//
// Function Name:		RStreamScript::Open( RStream::EAccessMode eAccess )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RStreamScript::Open( EAccessMode eAccess )
	{
	TpsAssert( NULL != m_pStream, "stream script not properly initialized" );
	
	m_pStream->Open( eAccess );
	}


// ****************************************************************************
//
// Function Name:		RStreamScript::Close( )
//
// Description:		close the file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RStreamScript::Close( )
	{
	TpsAssert( NULL != m_pStream, "stream script not properly initialized" );
	m_pStream->Close( );
	}


// ****************************************************************************
//
// Function Name:		RStreamScript::Read( YStreamLength yBytesToRead, uBYTE* pBuffer )
//
// Description:		read from the member file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RStreamScript::Read( YStreamLength yBytesToRead, uBYTE* pBuffer )
	{
	TpsAssert( NULL != m_pStream, "stream script not properly initialized" );
	m_pStream->Read( yBytesToRead, pBuffer );
	}


// ****************************************************************************
//
// Function Name:		RStreamScript::Write( YStreamLength yulBytesToRead, uBYTE* pBuffer )
//
// Description:		write to the member file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RStreamScript::Write( YStreamLength yulBytesToRead, const uBYTE* pBuffer )
	{
	TpsAssert( NULL != m_pStream, "stream script not properly initialized" );
	m_pStream->Write( yulBytesToRead, pBuffer );
	}


// ****************************************************************************
//
// Function Name:		RStreamScript::~RStreamScript( )
//
// Description:		dtor; delete member stream if requested
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
RStreamScript::~RStreamScript( )
	{
	if ( NULL != m_pStream )
		m_pStream->Close( );

	if ( m_fDeleteOnClose )
		delete m_pStream;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		StreamScript::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RStreamScript::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RStreamScript, this );
	}

#endif	// TPSDEBUG

// ****************************************************************************
//
//  File Name:			PersistantObject.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the DataObject class
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
//  $Logfile:: /PM8/Framework/Source/PersistantObject.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"PersistantObject.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ChunkyStorage.h"

// ****************************************************************************
//
//  Function Name:	RPersistantObject::RPersistantObject( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPersistantObject::RPersistantObject( ) 
	: m_pStorage( NULL ),
	  m_ChunkId( NULL )
{
	NULL;
}


// ****************************************************************************
//
//  Function Name:	RPersistantObject::~RPersistantObject( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPersistantObject::~RPersistantObject( ) 
{
	NULL;
}


// ****************************************************************************
//
//  Function Name:	RPersistantObject::Associate( )
//
//  Description:		Associate the given storage and selected ID with this object
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RPersistantObject::Associate( RChunkStorage* pStorage )
{
	m_pStorage	= pStorage;
	m_ChunkId	= (pStorage)? pStorage->GetSelectedChunk( ) : NULL;
}


// ****************************************************************************
//
//  Function Name:	RPersistantObject::Allocate( )
//
//  Description:		Read in the data and call inherited Allocate
//
//  Returns:			Nothing;
//
//  Exceptions:		kMemory, kFile, etc.
//
// ****************************************************************************
//
void RPersistantObject::Allocate( )
{
	if ( m_pStorage )
	{
		TpsAssert( m_ChunkId, "There is a storage, but no associated ChunkId" );
		YChunkStorageId	currentStorageId	= m_pStorage->GetSelectedChunk( );
		m_pStorage->SetSelectedChunk( m_ChunkId );
		try
		{
			//	Rewind to beginning of storage
			m_pStorage->SeekAbsolute( 0 );
			Read( *m_pStorage );
		}
		catch( ... )
		{
			m_pStorage->SetSelectedChunk( currentStorageId );
			throw;
		}
		m_pStorage->SetSelectedChunk( currentStorageId );
	}

	RDataObject::Allocate( );
}

// ****************************************************************************
//
//  Function Name:	RPersistantObject::Deallocate( )
//
//  Description:		Write out the data and call Deallocate
//							Deallocate can fail to write.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPersistantObject::Deallocate( )
{
	BOOLEAN	fDeallocate	= TRUE;

	if ( m_pStorage )
	{
		TpsAssert( m_ChunkId, "There is a storage, but no associated ChunkId" );
		YChunkStorageId	currentStorageId	= m_pStorage->GetSelectedChunk( );
		m_pStorage->SetSelectedChunk( m_ChunkId );
		try
		{
			//	Rewind to beginning of storage
			m_pStorage->SeekAbsolute( 0 );
			Write( *m_pStorage );
			PurgeData( );
		}
		catch( ... )
		{
			fDeallocate = FALSE;
		}
		//
		//	Since the chunk may have moved, I need to retrieve the 
		//		potentially new position
		m_ChunkId  = m_pStorage->GetSelectedChunk( );
		m_pStorage->SetSelectedChunk( currentStorageId );
	}

	if ( fDeallocate )
		RDataObject::Deallocate( );
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPersistantObject::Validate( )
//
//  Description:		Object Validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPersistantObject::Validate( ) const
{
	RDataObject::Validate( );
}
#endif	//	TPSDEBUG

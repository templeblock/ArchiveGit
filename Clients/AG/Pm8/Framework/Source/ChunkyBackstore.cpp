// ****************************************************************************
//
//  File Name:			ChunkBackstore.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RChunkBackstore class
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
//  $Logfile:: /PM8/Framework/Source/ChunkyBackstore.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************
#include "FrameworkIncludes.h"

ASSERTNAME

#include "ChunkyBackstore.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RChunkBackstore::Initialize( RChunkStorage* pParent, const YChunkTag yChunkTag)
//
//  Description:		
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkBackstore::Initialize(  )
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Copy( const YChunkInfo& yci, YStreamLength yNewSize )
//
//  Description:		move chunk to backstore so that we may write past the location
//								of its index
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkBackstore::Copy( const YChunkInfo& yci, YStreamLength yNewSize )
	{
	RChunkStorage::Copy( yci, yNewSize );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Grow( )
//
//  Description:		move chunk to backstore so that we may write past the location
//								of its index
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkBackstore::Grow( )
	{
	//	it is not currently possible to grow the stream once its index has been
	//		written; later we will move the data to a backstore and access it there
	UnimplementedCode();
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RChunkBackstore::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RChunkBackstore::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RChunkBackstore, this );
	}

#endif	// TPSDEBUG

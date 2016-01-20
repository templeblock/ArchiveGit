// ****************************************************************************
//
//  File Name:			DataObject.cpp
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
//  $Logfile:: /PM8/Framework/Source/DataObject.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"DataObject.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef	IMAGER
#include "ApplicationGlobals.h"
#endif	//	IMAGER

// ****************************************************************************
//			Static Class Data Initialization
// ****************************************************************************
//
YAccessCount		RDataObject::m_gAccessCounter = 0;

// ****************************************************************************
//
//  Function Name:	RDataObject::RDataObject( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDataObject::RDataObject( ) 
	: m_LastAccessed( 0 ),
	  m_LockCount( 0 ),
	  m_fInMemory( FALSE )
	{
#ifndef	IMAGER
	//	add item to frameworks list of data objects that can be purged.
	GetDataObjectList().InsertAtStart( this );
#endif	//	IMAGER
	}


// ****************************************************************************
//
//  Function Name:	RDataObject::~RDataObject( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDataObject::~RDataObject( ) 
	{
	TpsAssert( !IsInMemory(), "The DataObject being deleted is still in memory, call Deallocate before calling delete" );
	TpsAssert( !IsLocked(), "The DataObject being deleted is locked." );
#ifndef	IMAGER
	//	remove item from the Framework's list of data objects
	GetDataObjectList().Remove( this );
#endif	//	IMAGER
	}


// ****************************************************************************
//
//  Function Name:	RDataObject::Allocate( )
//
//  Description:		At this level, just set the in memory flag to true.
//							Functions that inherited from this class should call
//							this function after perfoming there own allocate.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDataObject::Allocate( )
	{
	TpsAssert( !IsLocked( ), "Allocate called on a Locked DataObject" );
	TpsAssert( !IsInMemory(), "Allocate called on DataObject already in memory" );
	m_fInMemory = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDataObject::Deallocate( )
//
//  Description:		At this level, just set the in memory flag to false.
//							Functions that inherited from this class should call
//							this function after perfoming there own deallocate.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDataObject::Deallocate( )
	{
	TpsAssert( !IsLocked(), "Deallocate called on Locked DataObject" );
	TpsAssert( IsInMemory(), "Deallocate called on a DataObject not in memory" );
	m_fInMemory = FALSE;
	}


// ****************************************************************************
//
//  Function Name:	RDataObject::Lock( )
//
//  Description:		Call the allocater of the object is not in memory
//
//  Returns:			Nothing
//
//  Exceptions:		memory or file
//
// ****************************************************************************
//
void RDataObject::Lock( )
	{
	TpsAssert( (m_LockCount < 10), "Lock Count is greater than 10 is this good?" );
	if ( !IsInMemory() )
		Allocate( );
	++m_LockCount;
	m_LastAccessed = ++m_gAccessCounter;
	}

// ****************************************************************************
//
//  Function Name:	RDataObject::Unlock( )
//
//  Description:		Let the object be deallocated
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RDataObject::Unlock( )
	{
	TpsAssert( (m_LockCount > 0), "Lock Count is less than 1, this is bad." );
	--m_LockCount;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RDataObject::Validate( )
//
//  Description:		Object Validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDataObject::Validate( ) const
	{
	RObject::Validate( );
	TpsAssert( ((m_LockCount>=0)&&IsInMemory()), "A DataObject has a negative lock count" );
	}
#endif	//	TPSDEBUG

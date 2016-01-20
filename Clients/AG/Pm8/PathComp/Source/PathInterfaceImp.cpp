// ****************************************************************************
//
//  File Name:			PathInterfaceImp.cpp
//
//  Project:			Path Component
//
//  Author:				Lance Wilson
//
//  Description:		Implementation of the RPathInterfacesImp class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software.
//							500 Redwood Blvd
//							Novato, CA 94948
//							(415) 482-4400
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/PathComp/Source/PathInterfaceImp.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "PathIncludes.h"
ASSERTNAME

#include "PathInterfaceImp.h"
#include "PathDocument.h"
#include "PathView.h"

#include "PropertyAttribImp.h"

// ****************************************************************************
//
//  Function Name:	RPathHolderImp::RPathHolderImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPathHolderImp::RPathHolderImp( )
{
}

// ****************************************************************************
//
//  Function Name:	RPathHolderImp::~RPathHolderImp( )
//
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPathHolderImp::~RPathHolderImp( )
{
}

#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RPathHolderImp::Validate( )
//
//  Description:		Validate class.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathHolderImp::Validate( ) const
{
//	RPathHolder::Validate( );
}
#endif	//	TPSDEBUG


// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::RPathInterfaceImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPathInterfaceImp::RPathInterfaceImp( const RPathView* pView )
	: IPathInterface( pView )
	, m_pPathDocument( (RPathDocument *)( pView->GetComponentDocument( ) ) )
{
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RPathInterfaceImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RPathView, pView );
	return new RPathInterfaceImp( static_cast<const RPathView*>( pView ) );
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::GetData( )
//
//  Description:		Return the data from the object that this interface references.
//
//  Returns:			new RGraphicHolder (containing the data)
//
//  Exceptions:		kMemory, kUnknownException
//
// ****************************************************************************
//
RComponentDataHolder*	RPathInterfaceImp::GetData( )
{
	return new RPathHolderImp();
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::EditData( )
//
//  Description:		Edit the object
//
//  Returns:			nothing
//
//  Exceptions:		kMemory, kUnknownException
//
// ****************************************************************************
//
void	RPathInterfaceImp::EditData( )
{
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::SetData( )
//
//  Description:		Set this data into the object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathInterfaceImp::SetData( RComponentDataHolder* /*pHolder*/ )
{
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::ClearData( )
//
//  Description:		Free memory associated with this interface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathInterfaceImp::ClearData( )
{
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::Read( )
//
//  Description:		Read in the data into the associated Graphic.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
void RPathInterfaceImp::Read( RChunkStorage& /*storage*/ )
{
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::Write( )
//
//  Description:		Write the data to the given storage
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk errors
//
// ****************************************************************************
//
void RPathInterfaceImp::Write( RChunkStorage& /*storage*/ )
{
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::Render( )
//
//  Description:		Render the data in the view.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
void RPathInterfaceImp::Render( RDrawingSurface& /*ds*/, const R2dTransform& /*transform*/,
										  const RIntRect& /*rcRender*/, const RIntRect& /*rcLocation*/ )
{
}


// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::GetPath( )
//
//  Description:		Get a pointer to the path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPath* RPathInterfaceImp::GetPath( )
{
	return &(m_pPathDocument->m_rPath);
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::SetPath( )
//
//  Description:		Sets a new path in the document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathInterfaceImp::SetPath( const RPath& rPath, BOOLEAN fResize )
{
	m_pPathDocument->m_rPath = rPath;

	if (fResize)
	{
		m_pPathDocument->ResizeView( rPath.GetPathSize(), TRUE );
	}
	else
	{
		m_pPathDocument->InvalidateAllViews();
	}
}


//////////////////////////////////////////////////////////////////////////////////////
// RPathObjectPropertiesImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RPathObjectPropertiesImp::RPathObjectPropertiesImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPathObjectPropertiesImp::RPathObjectPropertiesImp( const RComponentView* pView )
	: IObjectProperties( pView )
	, m_pPathDocument( (RPathDocument *) pView->GetComponentDocument() )
{
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RPathObjectPropertiesImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RPathView, pView );
	return new RPathObjectPropertiesImp( static_cast<const RPathView*>( pView ) );
}

BOOLEAN RPathObjectPropertiesImp::FindProperty( YInterfaceId iid, RInterface** ppInterface )
{
	switch (iid)
	{
	case kOutlineAttribInterfaceId:
	case kOutlineColorAttribInterfaceId:
		*ppInterface = new ROutlineAttribImp( m_pPathDocument->m_sOutlineFormat, m_pPathDocument );
		break;

	case kFillColorAttribInterfaceId:
		*ppInterface = new RColorImp( m_pPathDocument->m_rFillColor, m_pPathDocument );
		break;

	default:
		*ppInterface = NULL;
	}

	return (*ppInterface != NULL);
}

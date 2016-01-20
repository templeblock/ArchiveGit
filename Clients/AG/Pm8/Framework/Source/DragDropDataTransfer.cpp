// ****************************************************************************
//
//  File Name:			DragDropDataTransfer.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the drag&drap data transfer classes
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
//  $Logfile:: /PM8/Framework/Source/DragDropDataTransfer.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"DragDropDataTransfer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "SelectionTracker.h"
#include "CopySelectionAction.h"
#include "DeleteSelectionAction.h"
#include "PasteAction.h"
#include "ViewDrawingSurface.h"
#include "StandaloneApplication.h"
#include "ApplicationGlobals.h"

// ****************************************************************************
//
//  Function Name:	RDragDropDataTarget::RDragDropDataTarget( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropDataTarget::RDragDropDataTarget( RDataRenderer* pDataRenderer ) : TargetBaseClass( pDataRenderer )
	{
	;
	}

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	RDragDropDataTarget::~RDragDropDataTarget( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropDataTarget::~RDragDropDataTarget( )
	{
	delete m_pOleDataSource;
	}

#endif

// ****************************************************************************
//
//  Function Name:	RDragDropDataTarget::DoDragDrop( )
//
//  Description:		Begins a drag/drop operation. The functions does not
//							return until the drag/drop operation is complete.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YDropEffect RDragDropDataTarget::DoDragDrop( YDropEffect allowableEffects )
	{
	GetInDragDrop( ) = TRUE;

#ifdef _WINDOWS
	RRenaissanceOleDropSource oleDropSource;
	return m_pOleDataSource->DoDragDrop( allowableEffects, NULL, &oleDropSource );
#endif

	GetInDragDrop( ) = FALSE;
	}

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	RDragDropDataSource::RDragDropDataSource( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropDataSource::RDragDropDataSource( COleDataObject* pDataObject )
	:  RMFCDataTransferSource( pDataObject ),
		m_pTransferDocument( NULL ),
		m_pDropTargetView( NULL )
	{
	;
	}

#endif

// ****************************************************************************
//
//  Function Name:	RDragDropInfo::RDragDropInfo( )
//
//  Description:		Default constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropInfo::RDragDropInfo( )
	{
	m_Data.m_pSourceView = NULL;
	}
	 
// ****************************************************************************
//
//  Function Name:	RDragDropInfo::RDragDropInfo( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropInfo::RDragDropInfo( RView* pSourceView,
									   const RRealPoint& dragStartPoint,
										const RRealSize& dragOffset,
										BOOLEAN fUseTargetDefaultSize )
	{
	m_Data.m_pSourceView = pSourceView;
	m_Data.m_DragStartPoint = dragStartPoint;
	m_Data.m_DragOffset = dragOffset;
	m_Data.m_fUseTargetDefaultSize = fUseTargetDefaultSize;
	}

// ****************************************************************************
//
//  Function Name:	RDragDropInfo::Copy( )
//
//  Description:		Copies this drag drop info block to the specified data
//							target.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragDropInfo::Copy( RDataTransferTarget& dataTarget ) const
	{
	dataTarget.SetData( kDragDropFormat, reinterpret_cast<const uBYTE*>( &m_Data ), sizeof( m_Data ) );
	}

// ****************************************************************************
//
//  Function Name:	RDragDropInfo::Paste( )
//
//  Description:		Pastes this drag drop info block from the specified data
//							source.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragDropInfo::Paste( const RDataTransferSource& dataSource )
	{
	uBYTE* pData;
	YDataLength dataLength;
	dataSource.GetData( kDragDropFormat, pData, dataLength );
	TpsAssert( dataLength >= sizeof( m_Data ), "Invalid data length." );
	memcpy( &m_Data, pData, sizeof( m_Data ) );
	delete [] pData;
	}

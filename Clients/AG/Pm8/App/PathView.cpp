// ****************************************************************************
//
//  File Name:			PathView.cpp
//
//  Project:			Path Component
//
//  Author:				Lance Wilson
//
//  Description:		Declaration of the RPathDocument class
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
//  $Logfile:: /PM8/App/PathView.cpp                                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:08p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include	"PathView.h"
#include	"PathDocument.h"
#include "DrawingSurface.h"

//#include "RenaissanceResource.h"

// ****************************************************************************
//
//  Function Name:	RPathView::RPathView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPathView::RPathView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView )
	: RComponentView( boundingRect, pDocument, pParentView )
	{
	EnableRenderCache( FALSE );
	}

// ****************************************************************************
//
//  Function Name:	RPathView::OnXEditComponent( )
//
//  Description:		Called to edit this component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathView::OnXEditComponent( EActivationMethod, const RRealPoint& )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RPathView::EnableRenderCacheByDefault( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if the render cache is enabled by default for this
//							component
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPathView::EnableRenderCacheByDefault( ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RView::SetClipRegion( )
//
//  Description:		Creates a new clip region that is the intersection of the
//							current clip region and a region that will clip all output
//							to the bounds of this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathView::SetClipRegion( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const
	{
	// Intersect our useable area
	RRealSize oneUnit( 1, 1 );
	::DeviceUnitsToLogicalUnits( oneUnit, drawingSurface );

	RRealRect rect( GetUseableArea( !kInsetFrame ) );
	rect.m_Right += oneUnit.m_dx; rect.m_Bottom += oneUnit.m_dy;

	drawingSurface.IntersectClipRect( rect, transform );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPathView::Validate( )
//
//  Description:		Validate the object.
//							
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathView::Validate( ) const
	{
	RComponentView::Validate( );
	TpsAssertIsObject( RPathView, this );
	}

#endif	//	TPSDEBUG


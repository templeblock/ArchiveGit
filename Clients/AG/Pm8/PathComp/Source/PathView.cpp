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
//  $Logfile:: /PM8/PathComp/Source/PathView.cpp                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "PathIncludes.h"
ASSERTNAME

#include	"PathView.h"
#include	"PathDocument.h"
#include "PathInterfaceImp.h"
#include "DrawingSurface.h"

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
//  Function Name:	RGraphicView::GetInterface( )
//
//  Description:		Return The Interface pointer if the given component can support
//							the desired interface.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RInterface* RPathView::GetInterface( YInterfaceId interfaceId ) const
{
	static	InterfaceToCreateFunction interfaceCreationMap[] =
	{
		{ kPathInterfaceId,					RPathInterfaceImp::CreateInterface },
		{ kObjectPropertiesInterfaceId,	RPathObjectPropertiesImp::CreateInterface },
	} ;
	
	TpsAssert( interfaceId != kInvalidInterfaceId, "Invalid InterfaceId passed to GetInterface" )

	for (int i = 0; i < NumElements(interfaceCreationMap); ++i)
	{
		if ( interfaceId == interfaceCreationMap[i].interfaceId )
			return interfaceCreationMap[i].creator( this );
	}

	return RComponentView::GetInterface( interfaceId );
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

// ****************************************************************************
//
//  Function Name:	RPathView::RenderInternalTrackingFeedback( )
//
//  Description:		Renders the tracking feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathView::RenderInternalTrackingFeedback( RDrawingSurface& drawingSurface,
														  const R2dTransform& transform,
														  const RView& ) const
{
	// Draw interior feedback
	RPathDocument*	pDocument = (RPathDocument *) GetComponentDocument();
	TpsAssertIsObject( RPathDocument, pDocument );

	if (pDocument)
	{
		RColor crFill; // Transparent fill
		RSolidColor crPen = drawingSurface.GetPenColor().GetSolidColor();

		RRealSize size = GetBoundingRect( ).GetUntransformedSize( );
		RIntRect rcRender( GetBoundingRect( ).m_TransformedBoundingRect );
		rcRender *= transform;

		pDocument->Render( drawingSurface, transform, rcRender, size, crFill, kStroked, crPen, 0 );
	}
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


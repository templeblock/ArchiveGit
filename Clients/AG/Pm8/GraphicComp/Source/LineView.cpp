// ****************************************************************************
//
//  File Name:			LineView.cpp
//
//  Project:			Line Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RLineView class
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
//  $Logfile:: /PM8/GraphicComp/Source/LineView.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#include	"LineView.h"
#include	"GraphicDocument.h"
#include "EditGraphicAction.h"
#include "Path.h"
#include "ComponentTypes.h"
#include "Menu.h"
// #include "RenaissanceResource.h"

// ****************************************************************************
//
//  Function Name:	RLineView::RLineView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RLineView::RLineView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView )
	: RGraphicView( boundingRect, pDocument, pParentView )
	{
	// Dont cache lines
	m_pRenderCache->Enable( FALSE );
	}

// ****************************************************************************
//
//  Function Name:	RLineView::ApplyResizeConstraint( )
//
//  Description:		Constrains the scale factors as appropriate
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RLineView::ApplyResizeConstraint( const RRealSize& scaleFactor ) const
	{	
	if( GetComponentDocument()->GetComponentType( ) == kVerticalLineComponent )
		return RRealSize(1.0, scaleFactor.m_dy);
	else
		return RRealSize(scaleFactor.m_dx, 1.0);
	}

// ****************************************************************************
//
//  Function Name:	RLineView::MaintainAspectRatioIsDefault( )
//
//  Description:		Returns TRUE if the default behavior when resizing is to
//							maintain the components aspect ratio. The opposite
//							behavior is obtained by holding down the control key.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RLineView::MaintainAspectRatioIsDefault( ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RLineView::CanChangeDefaultAspect( )
//
//  Description:		Returns TRUE if the user is allowed to use the control key
//							to reverse the maintain aspect default
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RLineView::CanChangeDefaultAspect( ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RLineView::CanResizeHorizontal( )
//
//  Description:		Returns TRUE if the user is allowed to resize this component
//							horizontally from the scale dialog
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RLineView::CanResizeHorizontal( ) const
	{
	return ( GetComponentDocument( )->GetComponentType( ) == kHorizontalLineComponent );
	}

// ****************************************************************************
//
//  Function Name:	RLineView::CanResizeVertical( )
//
//  Description:		Returns TRUE if the user is allowed to resize this component
//							horizontally from the scale dialog
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RLineView::CanResizeVertical( ) const
	{
	return ( GetComponentDocument( )->GetComponentType( ) == kVerticalLineComponent );
	}

// ****************************************************************************
//
//  Function Name:	RLineView::Scale( )
//
//  Description:		Resize the view about the specified point
//
//							Lines don't scale like normal graphics. Rather, each
//							line piece stays the same size, and more are displayed.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RLineView::Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN )
	{
	// Resize
	if( GetParentView( ) )
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.UnrotateAndResizeAboutPoint( centerOfScaling, ApplyResizeConstraint( scaleFactor ), GetMinimumSize( ), GetMaximumSize( ), FALSE );
		SetBoundingRect( tmp );
		}
	else
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.UnrotateAndResizeAboutPoint( centerOfScaling, ApplyResizeConstraint( scaleFactor ) );
		SetBoundingRect( tmp );
		}

	//	Undefine the cached outlinepath
	UndefineOutlinePath( );
	}

// ****************************************************************************
//
//  Function Name:	RLineView::AlternateScale( )
//
//  Description:		Scales the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RLineView::AlternateScale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN )
	{
	//	This is used when scaling due to printer changed, so let it scale
	if( GetParentView( ) )
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.ResizeAboutPoint( centerOfScaling, scaleFactor, GetMinimumSize( ), GetMaximumSize( ), FALSE );
		SetBoundingRect( tmp );
		}
	else
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.ResizeAboutPoint( centerOfScaling, scaleFactor );
		SetBoundingRect( tmp );
		}

	//	Undefine the cached outlinepath
	UndefineOutlinePath( );
	}

// ****************************************************************************
//
//  Function Name:	RLineView::EnableRenderCacheByDefault( )
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
BOOLEAN RLineView::EnableRenderCacheByDefault( ) const
	{
	return FALSE;
	}

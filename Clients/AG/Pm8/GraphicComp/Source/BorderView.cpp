// ****************************************************************************
//
//  File Name:			BorderView.cpp
//
//  Project:			Border Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RBorderView class
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
//  $Logfile:: /PM8/GraphicComp/Source/BorderView.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#include	"BorderView.h"
#include	"GraphicDocument.h"
#include "Psd3BorderGraphic.h"
#include "SingleSelection.h"
#include "DrawingSurface.h"
#include "GraphicApplication.h"
#include "ComponentTypes.h"
#include "Menu.h"
// #include "RenaissanceResource.h"
#include "BorderInterfaceImp.h"
#include "StandaloneDocument.h"

const YFloatType kNormalBorderRatio				= 0.0906;
const YFloatType kWideBorderRatio				= 0.1813;
const YFloatType kNormalMiniBorderRatio		= 0.107;
const YFloatType kWideMiniBorderRatio			= 0.176;
const YFloatType kMiniBorderExpansionRatio	= 0.22;

const YChunkTag kWideBorderFlagChunkId = 0x0002A01F;

// ****************************************************************************
//
//  Function Name:	RBorderView::RBorderView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBorderView::RBorderView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView )
	: RGraphicView( boundingRect, pDocument, pParentView )
{
	m_fWideBorder = FALSE;

	// Initialize the interior bounding rect
	UpdateInteriorBoundingRect();
	
	// Dont cache borders
	m_pRenderCache->Enable( FALSE );
}

// ****************************************************************************
//
//  Function Name:	RBorderView::Read( )
//
//  Description:		
//							
//  Returns:			Override of RGraphicView::Read() because we need to update
//							the border's thickness after its bounding rect has been
//							read in from storage.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::Read( RChunkStorage& storage )
{	
	RGraphicView::Read( storage );
	
	// Look for a wide border flag chunk	
	RChunkStorage::RChunkSearcher rSearcher;
	rSearcher = storage.Start(kWideBorderFlagChunkId, !kRecursive);
	if (!rSearcher.End())
	{
		storage >> m_fWideBorder;
		storage.SelectParentChunk();
	}
	
	UpdateInteriorBoundingRect();
}

// ****************************************************************************
//
//  Function Name:	RBorderView::Write( )
//
//  Description:		
//							
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::Write( RChunkStorage& storage ) const
{
	RGraphicView::Write( storage );
	
	// Add a wide border flag chunk
	storage.AddChunk(kWideBorderFlagChunkId);
		{
		storage << m_fWideBorder;
		}
	storage.SelectParentChunk();
}

// ****************************************************************************
//
//  Function Name:	RBorderView::Rotate( )
//
//  Description:		Rotates the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::Offset( const RRealSize& offset, BOOLEAN fInvalidate )
	{
	RGraphicView::Offset( offset, fInvalidate );
	UpdateInteriorBoundingRect( );
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::Rotate( )
//
//  Description:		Rotates the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::Rotate( const RRealPoint& centerOfRotation, YAngle angle )
	{
	RGraphicView::Rotate( centerOfRotation, angle );
	UpdateInteriorBoundingRect( );
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::Scale( )
//
//  Description:		Resize the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio )
	{
	// Resize
	if( GetParentView( ) )
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.UnrotateAndResizeAboutPoint( centerOfScaling, scaleFactor, GetMinimumSize( ), GetMaximumSize( ), fMaintainAspectRatio );
		SetBoundingRect( tmp );
		}
	else
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.UnrotateAndResizeAboutPoint( centerOfScaling, scaleFactor );
		SetBoundingRect( tmp );
		}

	// Update the interior bounding rect
	UpdateInteriorBoundingRect( );
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::AlternateScale( )
//
//  Description:		Resize the view about the specified point
//
//							Borders don't scale like normal graphics. Rather, each
//							border piece stays the same size, and more are displayed.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::AlternateScale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio )
	{
	Scale( centerOfScaling, scaleFactor, fMaintainAspectRatio );

	// Update the border thickness
	UpdateInteriorBoundingRect();
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::Resize( )
//
//  Description:		Resizes this view to the specified size
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::Resize( const RRealSize& newSize )
	{
	RGraphicView::Resize( newSize );
	UpdateInteriorBoundingRect( );
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::FlipHorizontal( )
//
//  Description:		Flips this component about the specified y axis
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::FlipHorizontal( YRealCoordinate yAxis )
	{
	RGraphicView::FlipHorizontal( yAxis );
	UpdateInteriorBoundingRect( );
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::FlipVertical( )
//
//  Description:		Flips this component about the specified x axis
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::FlipVertical( YRealCoordinate xAxis )
	{
	RGraphicView::FlipVertical( xAxis );
	UpdateInteriorBoundingRect( );
	}

// ****************************************************************************
//							 
//  Function Name:	RBorderView::DrawSelectionFrame( )
//
//  Description:		Draws a selection frame about the component. Does not
//							include resize or rotate handles
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::DrawSelectionFrame( RDrawingSurface& drawingSurface,
												  const R2dTransform& transform ) const
	{
	// Call the base method
	RComponentView::DrawSelectionFrame( drawingSurface, transform );
	
	YComponentBoundingRect rInteriorBoundingRect = GetInteriorBoundingRect();
	RSingleSelection::SelectionRectFromObjectRect( rInteriorBoundingRect, this, FALSE );
	drawingSurface.FrameVectorRect( rInteriorBoundingRect, transform );
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::HitTest( )
//
//  Description:		Determines if the specified point lies within this component
//
//  Returns:			TRUE:		The point is within this component
//							FALSE:	The point is not within this component
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RBorderView::HitTest( const RRealPoint& point ) const
	{
	// Check to make sure we did not hit inside it
	YComponentBoundingRect tmp( GetBoundingRect() );
	if ( tmp.PointInRect( point ) && !m_rInteriorBoundingRect.PointInRect( point ) )
		return RGraphicView::HitTest( point );
	else
		return FALSE;
	}

// ****************************************************************************
//							 
//  Function Name:	RBorderView::HitTestSelectionFrame( )
//
//  Description:		Hit tests the selection from drawn abovve
//
//  Returns:			TRUE if the frame was hit
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RBorderView::HitTestSelectionFrame( const RRealPoint& point ) const
	{
	// Call the base method
	if( RComponentView::HitTestSelectionFrame( point ) )
		{
		// We hit inside the outer frame, now make sure we didnt hit inside the inner frame		
		YComponentBoundingRect rInteriorBoundingRect = GetInteriorBoundingRect();
		RSingleSelection::SelectionRectFromObjectRect( rInteriorBoundingRect, this, FALSE );
		return static_cast<BOOLEAN>( !rInteriorBoundingRect.PointInRect( point ) );
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::RenderInternalTrackingFeedback( )
//
//  Description:		Renders the tracking feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::RenderInternalTrackingFeedback( RDrawingSurface& drawingSurface,
														const R2dTransform& transform,
														const RView& targetView ) const
	{
	// Get the border thickness and convert it to device units
	RRealSize borderThicknessSize( ( GetInteriorBoundingRect( ).Width( ) - GetBoundingRect().Width( ) ) / 2,
											 ( GetInteriorBoundingRect( ).Width( ) - GetBoundingRect().Width( ) ) / 2 );



	//RRealSize rInitialSize(GetReferenceSize());
	//YRealDimension nBorderThickness = (rInitialSize.m_dx > rInitialSize.m_dy) ? rInitialSize.m_dy : rInitialSize.m_dx;	
	//nBorderThickness *= (m_fWideBorder ? kWideBorderRatio : kNormalBorderRatio);
	//RRealSize borderThicknessSize( -nBorderThickness, -nBorderThickness );
	::LogicalUnitsToDeviceUnits( borderThicknessSize, targetView );

	// Create a component bounding rect, and convert it to a selection rect
	YComponentBoundingRect boundingRect( GetBoundingRect().GetUntransformedSize( ) );
	boundingRect *= transform;
	RSingleSelection::SelectionRectFromDeviceObjectRect( boundingRect, FALSE );

	// Deflate our bounding rect by the border thickness
	boundingRect.Inflate( borderThicknessSize );

	// Draw it
	drawingSurface.FrameVectorRect( RIntVectorRect( boundingRect ) );
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::GetMinimumSize( )
//
//  Description:		Accessor
//
//  Returns:			The minimum size this view is allowed
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RBorderView::GetMinimumSize( ) const
	{
	if (GetParentView())
		return GetParentView()->GetSize();	
	else
		return GetRDocument()->GetMinObjectSize(kMiniBorderComponent);
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::GetMinimumSize( )
//
//  Description:		Accessor
//
//  Returns:			The minimum size this view is allowed
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RBorderView::GetMaximumSize( ) const
	{
	if (GetParentView())
		return GetParentView()->GetSize();	
	else
		return GetRDocument()->GetMaxObjectSize(kMiniBorderComponent);
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::MaintainAspectRatioIsDefault( )
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
BOOLEAN RBorderView::MaintainAspectRatioIsDefault( ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::SetBoundingRect( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::SetBoundingRect( const YComponentBoundingRect& boundingRect )
{
	RGraphicView::SetBoundingRect( boundingRect );
	UpdateInteriorBoundingRect( );
}

// ****************************************************************************
//
//  Function Name:	RBorderView::IntersectsRect( )
//
//  Description:		Return if this component intersects the given rectangle in
//							its parents space...
//
//  Returns:			TRUE if the rectangle intersects the component
//
//  Exceptions:		File
//
// ****************************************************************************
//
BOOLEAN RBorderView::IntersectsRect( const RRealRect& targetRect ) const
	{
	//	TRUE if it intersects the bounds AND the target is not totally contained 
	//	within the interrior
	if ( targetRect.IsIntersecting( GetBoundingRect( ).m_TransformedBoundingRect ) )
		{		
		RRealRect					realBoundingRect		= m_rInteriorBoundingRect.m_TransformedBoundingRect;
		RRealRect					intersectRect;
		intersectRect.Intersect( targetRect, realBoundingRect );
		return intersectRect != targetRect;
		}
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::GetInterface( )
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
RInterface* RBorderView::GetInterface( YInterfaceId interfaceId ) const
	{
	static InterfaceToCreateFunction interfaceCreationMap[ ] =
		{
			{ kBorderInterfaceId, RBorderInterfaceImp::CreateInterface },
		};

	TpsAssert( interfaceId != kInvalidInterfaceId, "Invalid InterfaceId passed to GetInterface" )
	for (int i = 0; i < NumElements( interfaceCreationMap ); ++i )
		{
		if ( interfaceId == interfaceCreationMap[ i ].interfaceId )
			return interfaceCreationMap[ i ].creator( this );
		}

	return RGraphicView::GetInterface( interfaceId );
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::SetWideBorder()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
void RBorderView::SetWideBorder(BOOLEAN fWideBorder)
	{
	// Set the new border type
	m_fWideBorder = fWideBorder;

	//	Update the interior rect
	UpdateInteriorBoundingRect( );

	// Invalidate the view
	Invalidate();
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::CalculateOutlinePath( )
//
//  Description:		Calculate the border path for this object.
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RBorderView::CalculateBorderPath( RPath* pPath )
	{
	EPathOperatorArray	opArray;
	RIntPointArray			ptArray;

	opArray.InsertAtEnd( kMoveTo );			//	Outer rectangle
	opArray.InsertAtEnd( kLine );
	opArray.InsertAtEnd( (EPathOperator)4 );
	opArray.InsertAtEnd( kClose );
	ptArray.InsertAtEnd( GetBoundingRect().m_TopLeft );
	ptArray.InsertAtEnd( GetBoundingRect().m_TopRight );
	ptArray.InsertAtEnd( GetBoundingRect().m_BottomRight );
	ptArray.InsertAtEnd( GetBoundingRect().m_BottomLeft );
	ptArray.InsertAtEnd( GetBoundingRect().m_TopLeft );

	opArray.InsertAtEnd( kMoveTo );			//	Inner Rectangle
	opArray.InsertAtEnd( kLine );
	opArray.InsertAtEnd( (EPathOperator)4 );
	opArray.InsertAtEnd( kClose );
	ptArray.InsertAtEnd( m_rInteriorBoundingRect.m_TopLeft );
	ptArray.InsertAtEnd( m_rInteriorBoundingRect.m_TopRight );
	ptArray.InsertAtEnd( m_rInteriorBoundingRect.m_BottomRight );
	ptArray.InsertAtEnd( m_rInteriorBoundingRect.m_BottomLeft );
	ptArray.InsertAtEnd( m_rInteriorBoundingRect.m_TopLeft );

	opArray.InsertAtEnd( kEnd );

	pPath->Undefine( );
	pPath->Define( opArray, ptArray );
	}

// ****************************************************************************
//
//  Function Name:	RBorderView::UpdateInteriorBoundingRect( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderView::UpdateInteriorBoundingRect()
{	
	// Compute the border thickness based the current size
	RRealSize rInitialSize(GetReferenceSize());
	YRealDimension nBorderThickness = (rInitialSize.m_dx > rInitialSize.m_dy) ? rInitialSize.m_dy : rInitialSize.m_dx;	
	nBorderThickness *= (m_fWideBorder ? kWideBorderRatio : kNormalBorderRatio);

	// Update the bounding rect
	m_rInteriorBoundingRect = GetBoundingRect();
	m_rInteriorBoundingRect.Inflate(RRealSize(-nBorderThickness, -nBorderThickness));

	UndefineOutlinePath( );
}

// ****************************************************************************
//
//  Function Name:	RBorderView::EnableRenderCacheByDefault( )
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
BOOLEAN RBorderView::EnableRenderCacheByDefault( ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RMiniBorderView::RMiniBorderView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMiniBorderView::RMiniBorderView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView )
:	RBorderView( boundingRect, pDocument, pParentView )
{
}

// ****************************************************************************
//
//  Function Name:	RMiniBorderView::GetMinimumSize( )
//
//  Description:		Accessor
//
//  Returns:			The minimum size this view is allowed
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RMiniBorderView::GetMinimumSize( ) const
	{
	return RRealSize(GetRDocument()->GetMinObjectSize(kMiniBorderComponent));
	}

// ****************************************************************************
//
//  Function Name:	RMiniBorderView::GetMinimumSize( )
//
//  Description:		Accessor
//
//  Returns:			The minimum size this view is allowed
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RMiniBorderView::GetMaximumSize( ) const
	{
	return RRealSize(GetRDocument()->GetMaxObjectSize(kMiniBorderComponent));
	}

// ****************************************************************************
//
//  Function Name:	RMiniBorderView::UpdateInteriorBoundingRect( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMiniBorderView::UpdateInteriorBoundingRect()
{	
	// Determine the border thickness
	YRealDimension nBorderThickness;
	
	RStandaloneDocument* pTopLevelDocument = GetComponentDocument()->GetTopLevelDocument();	
	if (pTopLevelDocument)
	{
		RRealSize rInitialSize = static_cast<RRealSize>(pTopLevelDocument->GetDefaultObjectSize(kMiniBorderComponent));
		nBorderThickness = (rInitialSize.m_dx > rInitialSize.m_dy) ? rInitialSize.m_dy : rInitialSize.m_dx;	
		nBorderThickness *= (m_fWideBorder ? kWideMiniBorderRatio : kNormalMiniBorderRatio);

		// Update the bounding rect
		m_rInteriorBoundingRect = GetBoundingRect();
		m_rInteriorBoundingRect.Inflate(RRealSize(-nBorderThickness, -nBorderThickness));
	}
	else
	{
		RBorderView::UpdateInteriorBoundingRect();
	}
}

// ****************************************************************************
//
//  Function Name:	RMiniBorderView::SetWideBorder()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
void RMiniBorderView::SetWideBorder(BOOLEAN fWideBorder)
	{
	if (m_fWideBorder == fWideBorder) return;

	RIntSize rViewSize = GetReferenceSize();
	if (fWideBorder)
		{
		// Expand the border by 22%
		rViewSize.Scale(RRealSize(1.0 + kMiniBorderExpansionRatio, 1.0));
		}
	else
		{
		// Contract the border by 22%
		rViewSize.Scale(RRealSize(1.0 / (1.0 + kMiniBorderExpansionRatio), 1.0));
		}
	
	Resize(rViewSize);
	
	// Call the base member
	RBorderView::SetWideBorder( fWideBorder );
	}


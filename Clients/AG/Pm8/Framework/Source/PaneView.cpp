// ****************************************************************************
//
//  File Name:			PaneView.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the PaneView class
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
//  $Logfile:: /PM8/Framework/Source/PaneView.cpp                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "PaneView.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"GradientFill.h"
#include	"DrawingSurface.h"
#include	"ChunkyStorage.h"
#include "FrameworkResourceIds.h"
#include "Document.h"
#include "ComponentView.h"
#include "ComponentDocument.h"
#include "ComponentTypes.h"

//	chunk tags
const uLONG kGradientBackdropTag	= 'GBKD';
const uLONG kBackgroundColorTag	= 'CBKD';
const uLONG	kPaneViewFlagTag		= 'PVFT';

const	uLONG	kPaneViewPadding		= 32;


//	Current version of Color data. 
const YVersion			kBackgroundColorAngleReversedVersion( kDefaultDataVersion + 1 );

// Command Map
RCommandMap<RPaneView, RView> RPaneView::m_CommandMap;

RCommandMap<RPaneView, RView>::RCommandMap( )
	{
	RouteCommandToFunction( (RCommandTarget* (RView::*)() const) RView::GetActiveComponent );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::RPaneView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPaneView::RPaneView( RDocument* pDocument, RView* pParentView )
	:	RView( ),
		m_pParentView( pParentView ),
		m_BleedAmount( 0, 0 )
	{
	Initialize( pDocument );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::RenderBackground( )
//
//  Description:		Render this view's background. 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::RenderBackground( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
	{
	// Get the output rect
	RRealRect outputRect = GetUseableArea( !kInsetFrame );

	//	If we have 'PaneBleed' add it in here.
	if ( HasPaneBleed( ) )
		outputRect.Inflate( m_BleedAmount );
	
	// Put the output rect into a vector rect so we can transform it to test against rcRender
	RRealVectorRect tempRect( outputRect );
	tempRect *= transform;

	// Dont bother if we are in coloring book mode, or if we are clipped out
	if( !drawingSurface.GetColoringBookMode( ) && tempRect.m_TransformedBoundingRect.IsIntersecting( RRealRect( rcRender ) ) )
		{
		//	Prep drawing.
		drawingSurface.RestoreDefaults( );
		drawingSurface.SetForegroundMode( kNormal );
		
		//	Prep the color for use if its a gradient.
		//	Don't include the frame inset in the output rect since it was already included  
		// by our caller.
		RColor	backgroundColor	= m_rBackgroundColor;
		backgroundColor *= transform;

		backgroundColor.SetBoundingRect( outputRect );
		drawingSurface.SetFillColor( backgroundColor );		

		// Render
		drawingSurface.FillRectangle( outputRect, transform );
		}
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::RenderSurfaceAdornments( )
//
//  Description:		Render any "surface adornments" such as desktop, panel
//							surface, background color/gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::RenderSurfaceAdornments( RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
{
	RView* pParentView = GetParentView();
	if (pParentView)
	{
		R2dTransform	parentTransform;
		parentTransform.MakeIdentity();
		ApplyTransform( parentTransform, FALSE, drawingSurface.IsPrinting( ) );
		parentTransform.Invert( );
		parentTransform	= parentTransform * transform;
		pParentView->RenderSurfaceAdornments( pView, drawingSurface, parentTransform, rcRender );
	}
}

// ****************************************************************************
//
//  Function Name:	RPaneView::RenderBehindComponentsWithSoftEffects( )
//
//  Description:		Render all components, if any, behind the current one
//							with soft effects.
//
//							Handles nested subviews such as grouped components.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::RenderBehindComponentsWithSoftEffects( const YComponentViewCollection& fComponentViewCollection, RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
{
	RView* pParentView = GetParentView();
	if (!pParentView)
		RView::RenderBehindComponentsWithSoftEffects( fComponentViewCollection, pView, drawingSurface, transform, rcRender );
	else
	{
		// recursively render behind components in parent views
		const YComponentViewCollection& parentComponentViewCollection = pParentView->GetZOrderState();

		RComponentView* renderBehindThisView = pView;
		RComponentView* pThisComponentView = dynamic_cast<RComponentView*>(const_cast<RPaneView*>(this));
		if (pThisComponentView)
		{
#if 0
			RComponentDocument* pThisComponentDocument = pThisComponentView->GetComponentDocument();
			YComponentType thisComponentType = pThisComponentDocument->GetComponentType();
			if (thisComponentType == kGroupComponent)
#else
			// GCB 6/15/98 - test for any group component including custom objects
			YComponentViewIterator iterator = m_ComponentViewCollection.Start();
			if (iterator != m_ComponentViewCollection.End())
#endif
			{
				// limit the recursion
				renderBehindThisView = pThisComponentView;
			}
		}

		R2dTransform	parentTransform;
		parentTransform.MakeIdentity();
		ApplyTransform( parentTransform, FALSE, drawingSurface.IsPrinting( ) );
		parentTransform.Invert( );
		parentTransform	= parentTransform * transform;

		pParentView->RenderBehindComponentsWithSoftEffects( parentComponentViewCollection, renderBehindThisView, drawingSurface, parentTransform, rcRender );

		// render behind components in this view
		RView::RenderBehindComponentsWithSoftEffects( fComponentViewCollection, pView, drawingSurface, transform, rcRender );
	}
}

// ****************************************************************************
//
//  Function Name:	void AdjustRectForSoftEffectsBehind(
//								const YComponentViewCollection&	fComponentViewCollection, 
//								const RComponentView*				pView, 
//								const R2dTransform&					transform,
//								RRealRect&								rect ) const
//
//  Description:		If this component has objects behind it with soft effects
//							that render outside their bounds, adjust rect outward such
//							that the portion of their soft effects intersecting the
//							area behind this component can be properly rendered.
//
//							Useful for setting up an offscreen bitmap of the correct
//							size for components with soft effects that render outside
//							their bounds.
//
//							Handles nested subviews such as grouped components.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::AdjustRectForSoftEffectsBehind( const YComponentViewCollection& fComponentViewCollection, const RComponentView* pView, const R2dTransform& transform, RRealRect& rect ) const
{
	RView* pParentView = GetParentView();
	if (!pParentView)
		RView::AdjustRectForSoftEffectsBehind( fComponentViewCollection, pView, transform, rect );
	else
	{
		// recursively adjust rect for behind components in parent views
		const YComponentViewCollection& parentComponentViewCollection = pParentView->GetZOrderState();

		RComponentView* adjustBehindThisView = const_cast<RComponentView*>(pView);
		RComponentView* pThisComponentView = dynamic_cast<RComponentView*>(const_cast<RPaneView*>(this));
		if (pThisComponentView)
		{
#if 0
			RComponentDocument* pThisComponentDocument = pThisComponentView->GetComponentDocument();
			YComponentType thisComponentType = pThisComponentDocument->GetComponentType();
			if (thisComponentType == kGroupComponent)
#else
			// GCB 8/25/98 - test for any group component including custom objects
			YComponentViewIterator iterator = m_ComponentViewCollection.Start();
			if (iterator != m_ComponentViewCollection.End())
#endif
			{
				// limit the recursion
				adjustBehindThisView = pThisComponentView;
			}
		}

		R2dTransform	parentTransform;
		parentTransform.MakeIdentity();
		ApplyTransform( parentTransform, FALSE, FALSE );
		parentTransform.Invert( );
		R2dTransform	inverseParentTransform( parentTransform );
		parentTransform	= parentTransform * transform;

		RRealRect parentRect( rect );
		R2dTransform inverseTransform( transform );
		inverseTransform.Invert( );
		parentRect *= inverseTransform;
		parentRect *= parentTransform;

		pParentView->AdjustRectForSoftEffectsBehind( parentComponentViewCollection, adjustBehindThisView, parentTransform, parentRect );

		// adjust rect for behind components in this view
		RView::AdjustRectForSoftEffectsBehind( fComponentViewCollection, pView, transform, rect );

		if (parentRect.IsIntersecting( rect ))
		{
			rect.m_Left = ::Min( parentRect.m_Left, rect.m_Left );
			rect.m_Top = ::Min( parentRect.m_Top, rect.m_Top );
			rect.m_Right = ::Max( parentRect.m_Right, rect.m_Right );
			rect.m_Bottom = ::Max( parentRect.m_Bottom, rect.m_Bottom );
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RPaneView::Write( )
//
//  Description:		Writes this projects data to the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RPaneView::Write( RChunkStorage& storage ) const
	{
	// Call the base class
	RView::Write( storage );

	// Write the background color
	storage.AddChunk( kBackgroundColorTag );
	storage.GetCurrentChunkCurrentDataVersion() = kBackgroundColorAngleReversedVersion;
	storage << m_rBackgroundColor;
	storage.SelectParentChunk( );

	//	Write the pane view flags tag
	storage.AddChunk( kPaneViewFlagTag );

	storage << m_BleedAmount;
	uBYTE	ubPadding[ kPaneViewPadding ];
	memset( ubPadding, 0, sizeof(ubPadding ) );
	storage.Write( sizeof(ubPadding), ubPadding );

	storage.SelectParentChunk( );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::Read( )
//
//  Description:		Reads from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RPaneView::Read( RChunkStorage& storage )
	{
	// Call the base class
	RView::Read( storage );
	
	//	Remember the chunk tree level where we are starting our read.
	YChunkStorageId yCurrentChunk = storage.GetSelectedChunk( );

	// Locate the position of the background color in the chunk tree.
	RChunkStorage::RChunkSearcher searcher = storage.Start( kBackgroundColorTag, FALSE );

	// Read the background color.
	storage >> m_rBackgroundColor;

	if ( storage.GetCurrentChunkCurrentDataVersion() < kBackgroundColorAngleReversedVersion )
	{
		//	If this storage is before the current data version where the color was reversed,
		//		reverse the angle (but only if it is a gradient )
		if ( m_rBackgroundColor.GetFillMethod() == RColor::kGradient )
		{
			TpsAssert( m_rBackgroundColor.m_pGradient != NULL, "Gradient in Color, but m_pGradient is NULL" );
			m_rBackgroundColor.m_pGradient->SetGradientAngle( kPI + m_rBackgroundColor.m_pGradient->GetGradientAngle() );
		}
	}

	storage.SetSelectedChunk( yCurrentChunk );

	//	See if the pane flag tags are there
	searcher = storage.Start( kPaneViewFlagTag );
	if ( !searcher.End( ) )
	{
		storage >> m_BleedAmount;
		uBYTE	ubPadding[ kPaneViewPadding ];
		storage.Read( sizeof(ubPadding), ubPadding );
	}

	//	Return to the level of the chunk tree that our caller expects.
	storage.SetSelectedChunk( yCurrentChunk );

	//	Now that the panel is completly read in, let it update its layout
	if( m_pParentView )
		m_pParentView->XUpdateAllViews( kLayoutChanged, 0 );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::SetMouseCapture( )
//
//  Description:		Captures the mouse to this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::SetMouseCapture( )
	{
	TpsAssert( m_pParentView, "SetMouseCapture with no parent view." );
	m_pParentView->SetMouseCapture( );
	RView::SetMouseCapture( );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::ReleaseMouseCapture( )
//
//  Description:		Releases the mouse capture. Unnecessary on the Mac
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::ReleaseMouseCapture( )
	{
	TpsAssert( m_pParentView, "ReleaseMouseCapture with no parent view." );
	m_pParentView->ReleaseMouseCapture( );
	RView::ReleaseMouseCapture( );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::SetKeyFocus( )
//
//  Description:		Set the Windows key focus to be this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::SetKeyFocus( )
	{
	TpsAssert( GetParentView( ), "No parent view." );
	GetParentView( )->SetKeyFocus( );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::IsEventPending( )
//
//  Description:		Check if there are any pending events in the Queue.
//							This query can be made during idle processing to check
//							if control should return to the framework.
//
//  Returns:			TRUE if the are system events pending
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPaneView::IsEventPending( ) const
	{
	// Just ask our parent
	TpsAssert( GetParentView( ), "No parent view." );
	return GetParentView( )->IsEventPending( );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::IsKeyEventPending( )
//
//  Description:		Check if there are any pending key events in the Queue.
//
//  Returns:			TRUE if the next system event is a key event
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPaneView::IsKeyEventPending( )	const
	{
	// Just ask our parent
	TpsAssert( GetParentView( ), "No parent view." );
	return GetParentView( )->IsKeyEventPending( );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::GetBackgroundColor( )
//
//  Description:		Accessor
//
//  Returns:			The background color of the view.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RColor RPaneView::GetBackgroundColor( ) const
	{
	return m_rBackgroundColor;	
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::HasBackgroundColor( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if this panes background color is not transparent
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPaneView::HasBackgroundColor( ) const
	{
	return static_cast<BOOLEAN>( m_rBackgroundColor.GetFillMethod( ) != RColor::kTransparent );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::HasPaneColor( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if this pane has a bleed amount
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPaneView::HasPaneBleed( ) const
	{
	return static_cast<BOOLEAN>((m_BleedAmount.m_dx!=0)||(m_BleedAmount.m_dy!=0));
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::GetDPI( )
//
//  Description:		Accessor
//
//  Returns:			The DPI of the screen
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RPaneView::GetDPI( ) const
	{
	// If we do not have a parent, just get the screen DPI
	if( GetParentView( ) == NULL )
		return GetScreenDPI( );

	// otherwise, ask our parent
	else
		return GetParentView( )->GetDPI( );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::GetParentView( )
//
//  Description:		Accessor
//
//  Returns:			The parent view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RView* RPaneView::GetParentView( ) const
	{
	return m_pParentView;
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::SetParentView( )
//
//  Description:		Accessor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::SetParentView( RView* pParentView )
	{
	m_pParentView = pParentView;
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::GetCursorPosition( )
//
//  Description:		Gets the current cursor position within this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealPoint RPaneView::GetCursorPosition( ) const
	{
	// Just ask our parent
	TpsAssert( GetParentView( ), "No parent view." );
	return ConvertPointToLocalCoordinateSystem( GetParentView( )->GetCursorPosition( ) );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::ActivateView( )
//
//  Description:		Activate the view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::ActivateView( )
	{
	TpsAssert( GetParentView( ), "No parent view." );
	GetParentView( )->ActivateView( );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::UpdateScrollBars( )
//
//  Description:		Update the scroll bars
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::UpdateScrollBars( EScrollUpdateReason scrollUpdateReason )
	{
	if( GetParentView( ) )
		GetParentView( )->UpdateScrollBars( scrollUpdateReason );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::GetUseableArea( )
//
//  Description:		Accessor
//
//  Returns:			The rectangle of this pane that is useable after all
//							decarations have been rendered.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RPaneView::GetUseableArea( const BOOLEAN fInsetFrame /*= TRUE*/ ) const
	{
	// Just ask our parent
	return GetParentView( )->GetUseableArea( fInsetFrame );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::GetViewableArea( )
//
//  Description:		Accessor
//
//  Returns:			The useable area of this view that is visible due to 
//							scrolling.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RPaneView::GetViewableArea( ) const
	{
	// Get our parents viewable area
	RRealRect parentViewableArea = m_pParentView->GetViewableArea( );

	// Get our view transform and invert it
	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );
	transform.Invert( );

	// Use this transform to convert our parents viewable area into our coordinate system
	return parentViewableArea * transform;
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::EditComponent( )
//
//  Description:		Edits a component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RPaneView::EditComponent( RComponentView* pComponent,
														EActivationMethod activationMethod,
														const RRealPoint& mousePoint,
														YResourceId& editUndoRedoPairId,
														uLONG ulEditInfo )

	{
	// If we have a parent, let it decide
	if( GetParentView( ) )
		return GetParentView( )->EditComponent( pComponent, activationMethod, mousePoint, editUndoRedoPairId, ulEditInfo );

	// Otherwise, let the base class handle
	else
		return RView::EditComponent( pComponent, activationMethod, mousePoint, editUndoRedoPairId, ulEditInfo );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::ApplyTransform( )
//
//  Description:		Adds this views transform operations to the given
//							transform.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::ApplyTransform( R2dTransform& transform, BOOLEAN fApplyParentsTransform, BOOLEAN fIsPrinting ) const
	{
	// If we are applying our parents transform, do it now
	if( fApplyParentsTransform && m_pParentView )
		m_pParentView->ApplyTransform( transform, fApplyParentsTransform, fIsPrinting );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::InvalidateVectorRect( )
//
//  Description:		Invalidates the given vector rect in this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::InvalidateVectorRect( const RRealVectorRect& rect, BOOLEAN fErase )
	{
	// Dont bother if we dont have a parent
	if( !GetParentView( ) )
		return;

	// Figure out the rectangle in our parent that needs invalidating
	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );

	// Invalidate the rect
	GetParentView( )->InvalidateVectorRect( rect * transform, fErase );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::ValidateVectorRect( )
//
//  Description:		Validates the given vector rect in this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::ValidateVectorRect( const RRealVectorRect& rect )
	{
	// Figure out the rectangle in our parent that needs invalidating
	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );

	// Invalidate the rect
	if( GetParentView( ) )
		GetParentView( )->ValidateVectorRect( rect * transform );
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
void RPaneView::SetClipRegion( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const
	{
		//	Adjust for pane bleed
		if ( HasPaneBleed( ) )
		{
			// Get our useable area
			RRealRect rect( GetUseableArea( !kInsetFrame ) );
			rect.Inflate( m_BleedAmount );
			drawingSurface.IntersectClipRect( rect, transform );
		}
		else
			RView::SetClipRegion( drawingSurface, transform );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::RectInView( )
//
//  Description:	calls it's parent to ensure the rect passed is visible
//					in it's view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::RectInView( const RRealVectorRect& rect ) 
{
	// Dont bother if we dont have a parent
	if( !GetParentView( ) )
		return;

	// Figure out the rectangle in our parent that needs invalidating
	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );

	
	GetParentView( )->RectInView( rect * transform );
}

// ****************************************************************************
//
//  Function Name:	RPaneView::AutoScroll( )
//
//  Description:		Does autoscrolling
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::AutoScroll( const RRealPoint& mousePoint, YRealDimension scrollAmount, BOOLEAN fIgnoreTime, EDirection allowableDirections )
	{
	// Get our view transform
	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );

	// Transform the point and call our parent
	if( GetParentView( ) )
		GetParentView( )->AutoScroll( mousePoint * transform, scrollAmount, fIgnoreTime, allowableDirections );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::WillAutoScroll( )
//
//  Description:		Determines if we will autoscroll
//
//  Returns:			TRUE if we will autoscroll
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPaneView::WillAutoScroll( const RRealPoint& mousePoint )
	{
	// Get our view transform
	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );

	// Transform the point and call our parent
	if( GetParentView( ) )
		return GetParentView( )->WillAutoScroll( mousePoint * transform );

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::SetupAutoScrollTimer( )
//
//  Description:		Sets up the autoscroll timer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::SetupAutoScrollTimer( const RRealPoint& mousePoint )
	{
	// Get our view transform
	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );

	// Transform the point and call our parent
	if( GetParentView( ) )
		GetParentView( )->SetupAutoScrollTimer( mousePoint * transform );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::OnFrameComponent( )
//
//  Description:		Called when a component is framed
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::OnFrameComponent( RComponentView* pComponent )
	{
	// If we have a parent, pass on the notification
	if( GetParentView( ) )
		GetParentView( )->OnFrameComponent( pComponent );
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::SetPaneBleed( )
//
//  Description:		Called to set the pane bleed amount
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::SetPaneBleed( RIntSize bleedAmount )
	{
	m_BleedAmount = bleedAmount;
	}

// ****************************************************************************
//
//  Function Name:	RPaneView::GetPaneBleed( ) const
//
//  Description:		Called to retrieve the pane bleed amount
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RPaneView::GetPaneBleed( ) const
	{
	return m_BleedAmount;
	}

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	RPaneView::GetCWnd( )
//
//  Description:		Gets the top level CWnd
//
//  Returns:			The CWnd&
//
//  Exceptions:		None
//
// ****************************************************************************
//
CWnd& RPaneView::GetCWnd( ) const
	{
	return m_pParentView->GetCWnd( );
	}

#endif

// ****************************************************************************
//
//  Function Name:	RPaneView::SetBackgroundColor( const RColor rBackgroundColor )
//
//  Description:		Install the given background color
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::SetBackgroundColor( const RColor& rBackgroundColor )
	{
	m_rBackgroundColor = rBackgroundColor;
	}

#ifdef TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPaneView::Validate( )
//
//  Description:		Validate the object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPaneView::Validate( ) const
	{
	RView::Validate( );
	TpsAssertIsObject( RPaneView, this );

	if( m_pParentView )
		TpsAssertValid( m_pParentView );
	}

#endif // TPSDEBUG

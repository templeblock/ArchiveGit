// ****************************************************************************
//
//  File Name:		EditCropControl.cpp
//
//  Project:		Renaissance Application Component
//
//  Author:			Lance Wilson
//
//  Description:	Implmentation of the REditCropView and
//                REditCropDocument classes
//
//	Portability:	Win32
//
//	Developed by:	Broderbund Software
//					   500 Redwood Blvd
//					   Novato, CA 94948
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include "EditCropControl.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "BitmapImage.h"
#include "ImageInterface.h"
#include "ImageLibrary.h"
#include "UndoableAction.h"
#include	"MultiUndoManager.h"
#include "GenericGroupedObjectDialog.h"
//#include "RenaissanceResource.h"
#include "GraphicCompResource.h"
#include "PathDocument.h"
#include "TabbedEditImage.h"

const YTint kNonCroppedTintLevel = 30;

class REditPointAction : public RUndoableAction
{
// Construction & destruction
public :
											REditPointAction( REditCropView* pView, int nIndex, RRealPoint ptOld, RRealPoint ptNew  );

// Operations
public :
	virtual BOOLEAN					Do( );
	virtual void						Undo( );

// Scripting Operations
public :
	virtual BOOLEAN					WriteScript( RScript& script ) const;

//	Identifier
public :
	static YActionId					m_ActionId;

// Member data
protected :

	REditCropView*				m_pView;
	RRealPoint					m_ptNew;
	RRealPoint					m_ptOld;
	int							m_nIndex;

#ifdef	TPSDEBUG
//	Debugging code
public :
	virtual	void						Validate( ) const;
#endif
};

YActionId REditPointAction::m_ActionId( "REditPointAction" );

// ****************************************************************************
//
//  Function Name:	REditPointAction::REditPointAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		kUnknownError
//
// ****************************************************************************
//
REditPointAction::REditPointAction( REditCropView* pView, int nIndex, RRealPoint ptOld, RRealPoint ptNew ) :
	RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_SPECIAL_EFFECT, STRING_UNDO_CHANGE_SPECIAL_EFFECT ),
	m_pView( pView ),
	m_nIndex( nIndex ),
	m_ptOld( ptOld ),
	m_ptNew( ptNew )
{
}


// ****************************************************************************
//
//  Function Name:	REditPointAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditPointAction::Do( )
{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	try
	{
		m_pView->SetPoint( m_nIndex, m_ptNew );
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	REditPointAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditPointAction::Undo( )
{
	RUndoableAction::Undo( );

	try
	{
		m_pView->SetPoint( m_nIndex, m_ptOld );
		((RView *) m_pView)->Invalidate();
	}
	catch (...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	REditPointAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditPointAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	REditPointAction::Validate( )
//
//  Description:	Validate the object.
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditPointAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( REditPointAction, this );
}

#endif	//	TPSDEBUG


/////////////////////////////////////////////////////////////////////

class REditModeAction : public RUndoableAction
{
// Construction & destruction
public :
										REditModeAction( REditCropView* pView, RRealPointArray& array, REditCropView::EMode eMode );

// Operations
public :
	virtual BOOLEAN				Do( );
	virtual void					Undo( );

// Scripting Operations
public :
	virtual BOOLEAN				WriteScript( RScript& script ) const;

//	Identifier
public :
	static YActionId				m_ActionId;

// Member data
protected :

	REditCropView*					m_pView; 
	RRealPointArray				m_ptArray;
	RRealPointArray				m_ptOldArray;
	REditCropView::EMode			m_eMode;
	REditCropView::EMode			m_eOldMode;

#ifdef	TPSDEBUG
//	Debugging code
public :
	virtual	void					Validate( ) const;
#endif
};

YActionId REditModeAction::m_ActionId( "REditModeAction" );

// ****************************************************************************
//
//  Function Name:	REditModeAction::REditModeAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		kUnknownError
//
// ****************************************************************************
//
REditModeAction::REditModeAction( REditCropView* pView, RRealPointArray& ptArray, REditCropView::EMode eMode ) :
	RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_SPECIAL_EFFECT, STRING_UNDO_CHANGE_SPECIAL_EFFECT ),
	m_pView( pView ),
	m_ptArray( ptArray ),
	m_eMode( eMode ),
	m_ptOldArray( pView->m_ptArray ),
	m_eOldMode( pView->m_eMode )
{
}


// ****************************************************************************
//
//  Function Name:	REditModeAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditModeAction::Do( )
{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	try
	{
		m_pView->SetMode( m_eMode );
		m_pView->SetPoints( m_ptArray );
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	REditModeAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditModeAction::Undo( )
{
	RUndoableAction::Undo( );

	try
	{
		m_pView->SetMode( m_eOldMode );
		m_pView->SetPoints( m_ptOldArray );
	}
	catch (...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	REditModeAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditModeAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	REditModeAction::Validate( )
//
//  Description:	Validate the object.
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditModeAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( REditModeAction, this );
}

#endif	//	TPSDEBUG


/////////////////////////////////////////////////////////////////////


// ****************************************************************************
//
//  Function Name: REditCropView::REditCropView( )
//
//  Description:	 Constructor
//
//  Returns:		 Nothing
//
// ****************************************************************************
//
REditCropView::REditCropView( CDialog* pDialog, int nControlId, RControlDocument* pDocument ) :
	RScrollControlView( pDialog, nControlId, pDocument ),
	m_eMode( kDefault ),
	m_nTrackingIndex( -1 ),
	m_pImageView( NULL ),
	m_yScaleFactor( 1 ),
	m_fTintedImage( FALSE )
{
	m_ptArray.Empty();
}

// ****************************************************************************
//
//  Function Name: REditCropView::SetImageInfo( )
//
//  Description:	 Provides the necessary information to render
//						 the non-clipped area of an image.
//
//  Returns:		 Nothing
//
// ****************************************************************************
//
void REditCropView::SetImageInfo( RComponentView* pView, RBitmapImage* pImage )
{
	m_fTintedImage = FALSE;
	m_pImageView = pView; 
	m_pImage = pImage; 
}

// ****************************************************************************
//
//  Function Name:	REditCropView::SetMode( )
//
//  Description:		Set the views mode
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::SetMode( EMode eMode ) 
{ 
	m_eMode = eMode; 
}

// ****************************************************************************
//
//  Function Name:	REditCropView::Render( )
//
//  Description:		Render this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::Render( RDrawingSurface& ds, const R2dTransform& transform, const RIntRect& rcRender ) const
{
	RScrollControlView::Render( ds, transform, rcRender );

	BOOLEAN fClosed = BOOLEAN( kTracking != m_eMode );
	RenderPoints( ds, transform, 0, fClosed );
}

// ****************************************************************************
//
//  Function Name:	REditCropView::RenderBackground( )
//
//  Description:		Render background for this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::RenderBackground( RDrawingSurface& ds, const R2dTransform& transform, const RIntRect& rcRender ) const
{
	RScrollControlView::RenderBackground( ds, transform, rcRender );

	try
	{
		RImageInterface* pImageInterface = static_cast<RImageInterface*>(
			m_pImageView->GetInterface( kImageInterfaceId ) );

		RBitmapImage* pImage = (RBitmapImage *) pImageInterface->GetImage();
		delete pImageInterface;

		if (pImage->GetClippingRPath() && !pImage->PathFromImport())
		{
			if (!m_fTintedImage)
			{
				RImageLibrary().Tint( *m_pImage, kNonCroppedTintLevel );
				m_fTintedImage = TRUE;
			}

			RRealRect rViewable = GetViewableArea();

			RRealRect rRect( CalcBoundingRectOfEmbeddedComponents( FALSE ) );
/*			RRealRect rRect( m_pImage->GetSizeInLogicalUnits() );
			rRect.m_Right *= m_yScaleFactor;
			rRect.m_Bottom *= m_yScaleFactor;
			rRect.CenterRectInRect( rViewable );
*/			rRect *= transform;

			m_pImage->Render( ds, rRect );
		}
	}
	catch (...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	REditCropView::RenderPoints( )
//
//  Description:		Point Renderer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::RenderPoints( RDrawingSurface& ds, R2dTransform transform, int nStartIndex, BOOLEAN fClose ) const
{
	if (m_ptArray.Count())
	{
		YDrawMode yOldDrawMode = ds.GetForegroundMode( );
		RColor crOldPenColor   = ds.GetPenColor();
		RColor crOldFillColor  = ds.GetFillColor();

		ds.SetForegroundMode( kXOR );
		ds.SetPenColor( RSolidColor( kWhite ) );
		ds.SetFillColor( RSolidColor( kWhite ) );

		RRealPointArrayIterator iterator = m_ptArray.Start();
		RRealRectArrayIterator iterator2 = m_rectArray.Start();

		if (nStartIndex > 0)
		{
			iterator  += nStartIndex;
			iterator2 += nStartIndex;
		}

		RRealPoint ptStart = *iterator;
		ds.MoveTo( ptStart, transform );

		do
		{
			RIntPoint pt = (*iterator) * transform;
			ds.LineTo( pt );

			RIntRect rect( *iterator2 );
			::LogicalUnitsToDeviceUnits( rect, ds );
			rect.Offset( pt - rect.GetCenterPoint() );

			if (kTracking != m_eMode)
			{
				ds.SetForegroundMode( yOldDrawMode );
				ds.SetPenColor( RSolidColor( kBlack ) );

				ds.FillRectangle( rect );
				ds.FrameRectangle( rect );

				ds.SetForegroundMode( kXOR );
				ds.SetPenColor( RSolidColor( kWhite ) );

				ds.MoveTo( pt );
			}
			else
				ds.FillRectangle( rect );

			iterator++;
			iterator2++;

		} while (iterator != m_ptArray.End());

		if (fClose)
			ds.LineTo( m_ptArray[0], transform );

		ds.SetForegroundMode( yOldDrawMode );
		ds.SetPenColor( crOldPenColor );
		ds.SetFillColor( crOldFillColor );
	}
}

// ****************************************************************************
//
//  Function Name:	REditCropView::PointsToPath( )
//
//  Description:		Converts the points in the image into a path.
//
//							Check for any hit components.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditCropView::PointsToPath( RPath& rPath, BOOLEAN fDeletePoints )
{
	if (kEditingPoints != m_eMode)
		return FALSE;

	// Create a transform to get the points into 
	// a normalized quadrant 1 coordinate space.
	R2dTransform transform;
	transform.PostScale( 1, -1 );

	EPathOperatorArray	opArray;	// the RPath operations
	RIntPointArray			ptArray;	// the RPath points

	RRealPointArrayIterator iterator = m_ptArray.Start();

	opArray.InsertAtEnd( kMoveTo );
	ptArray.InsertAtEnd( (*iterator) * transform );
	iterator++;

	opArray.InsertAtEnd( kLine );
	opArray.InsertAtEnd( EPathOperator( m_ptArray.Count() - 1 ) );

	while (iterator != m_ptArray.End())
		ptArray.InsertAtEnd( (*iterator++) * transform );

	opArray.InsertAtEnd( kClose );
	opArray.InsertAtEnd( kEnd );

	rPath.Undefine();
//	rPath.Define( opArray, ptArray );
	rPath.MergePoints( opArray, ptArray );

	if (fDeletePoints)
	{
		m_rectArray.Empty();
		m_ptArray.Empty();
		SetMode( kDefault );
	}

	return rPath.IsDefined();
}

// ****************************************************************************
//
//  Function Name:	REditCropView::GetHitPoint( )
//
//  Description:		Checks for any hit point.
//
//  Returns:			index of point hit if found; otherwise -1.
//
//  Exceptions:		None
//
// ****************************************************************************
//
int REditCropView::GetHitPoint( const RRealPoint& pt ) const
{
	RRealRectArrayIterator iterator = m_rectArray.Start();
	RRealRectArrayIterator end = m_rectArray.End();

	for (int i = 0; iterator != end; i++, iterator++)
	{
		if ((*iterator).PointInRect( pt ))
			return i;
	}

	return -1;
}

// ****************************************************************************
//
//  Function Name:	REditCropView::GetResizeTracker( )
//
//  Description:		Gets a tracker to use for resizing
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionTracker* REditCropView::GetResizeTracker( ) const
	{
	RRealRect rRect = CalcBoundingRectOfEmbeddedComponents( FALSE );
	return new RGroupDialogResizeSelectionTracker( GetSelection( ), rRect, TRUE );
	}

// ****************************************************************************
//
//  Function Name:	REditCropView::GetRotateTracker( )
//
//  Description:		Gets a tracker to use for rotating
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionTracker* REditCropView::GetRotateTracker( ) const
	{
	RRealRect rRect = CalcBoundingRectOfEmbeddedComponents( FALSE );
	return new RGroupDialogRotateSelectionTracker( GetSelection( ), rRect );
	}

// ****************************************************************************
//
//  Function Name:	REditCropView::RemovePoints( )
//
//  Description:		Clears the point list
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::RemovePoints()
{
	if (m_ptArray.Count())
	{
		RRealPointArray ptArray;
		RUndoableAction* pAction = new REditModeAction( this, ptArray, kDefault );
		GetRDocument()->SendAction( pAction );

		((RView *) this)->Invalidate();
	}
}

// ****************************************************************************
//
//  Function Name:	REditCropView::SetPoint( )
//
//  Description:		Sets the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::SetPoint( int nIndex, const RRealPoint& pt )
{
	m_ptArray[nIndex] = pt;

	// Build point rectangle for display
	RRealRect& rRect = m_rectArray[nIndex];
	rRect.Offset( pt - rRect.GetCenterPoint() );
}

// ****************************************************************************
//
//  Function Name:	REditCropView::SetPoints( )
//
//  Description:		Sets the control points
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::SetPoints( const RRealPointArray& ptArray )
{
	m_rectArray.Empty();
	m_ptArray = ptArray;

	if ( ptArray.Count() == 0 )
	{
		((RView *) this)->Invalidate();
		return;
	}

	// Rebuild rect array
	RRealPointArrayIterator iterator = m_ptArray.Start();

	RRealSize dpi = GetScreenDPI( );
	RRealRect rFirstRect( 0, 0, 8 * kSystemDPI / dpi.m_dx, 8 * kSystemDPI / dpi.m_dy );
	RRealRect rRect( 0, 0, 6 * kSystemDPI / dpi.m_dx, 6 * kSystemDPI / dpi.m_dy );

	rFirstRect.Offset( *iterator++ - rFirstRect.GetCenterPoint() );
	m_rectArray.InsertAtEnd( rFirstRect );

	while (iterator != m_ptArray.End())
	{
		rRect.Offset( *iterator - rRect.GetCenterPoint() );
		m_rectArray.InsertAtEnd( rRect );
		iterator++;
	}

	((RView *) this)->Invalidate();
}

// ****************************************************************************
//
//  Function Name:	REditCropView::PointFromIndex( )
//
//  Description:		Returns the point at the specifed index
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealPoint&	REditCropView::PointFromIndex( int nIndex )
{
	TpsAssert( nIndex >= 0 && nIndex < m_ptArray.Count(), "Invalid Index!" );
	return m_ptArray[nIndex];
}

// ****************************************************************************
//
//  Function Name:	REditCropView::OnXLButtonDown( )
//
//  Description:		Default handler for the button down XEvent.
//
//							Check for any hit components.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys )
{
	CWnd& cWnd = GetCWnd();

	if (cWnd.GetFocus() != &cWnd)
		cWnd.SetFocus();

	YComponentBoundingRect rRect = m_pImageView->GetBoundingRect();

	switch (m_eMode)
	{
	case kCollectPoints:

		if (!rRect.PointInRect( mousePoint ))
			break;

		m_rectArray.Empty();
		m_ptArray.Empty();
		m_eMode = kTracking;
		SetMouseCapture();

		m_rcBoundingRect = RRealRect( 0, 0, 0, 0 );

		// Fall Through

	case kTracking:
		{
			if (!rRect.PointInRect( mousePoint ))
				break;

			CClientDC dc( &cWnd );
			RDcDrawingSurface ds;
			ds.Initialize( (HDC) dc );

			R2dTransform transform;
			GetViewTransform( transform, FALSE );

			if (m_MouseDownCount == 2 || (m_rectArray.Count() && m_rectArray[0].PointInRect( mousePoint )))  
			{
				ReleaseMouseCapture();
				SetMode( kDefault );

				RRealPointArray ptArray( m_ptArray );
				m_ptArray.Empty();

				RUndoableAction* pAction = new REditModeAction( this, ptArray, kEditingPoints );
				GetRDocument()->SendAction( pAction );
			}
			else
			{
				m_ptArray.InsertAtEnd( mousePoint );
				m_rcBoundingRect.AddPointToRect( mousePoint );

				// Build point rectangle for display
				RRealRect rRect( 0, 0, 6, 6 );

				if (m_ptArray.Count() == 1)
					rRect = RRealRect( 0, 0, 8, 8 );

				DeviceUnitsToLogicalUnits( rRect, ds ) ;
				rRect.Offset( mousePoint - rRect.GetCenterPoint() );
				m_rectArray.InsertAtEnd( rRect );

				RenderPoints( ds, transform, m_ptArray.Count() - 1, FALSE );

				m_ptLast = mousePoint;
			}

			ds.DetachDCs();
		}

		break;

	case kEditingPoints:
		{
			m_nTrackingIndex = GetHitPoint( mousePoint );

			if (m_nTrackingIndex >= 0)
			{
				m_ptTrackedPoint = m_ptArray[m_nTrackingIndex];
				SetMode( kTrackingPoint );
				SetMouseCapture();
			}
		}

		break;

	default:
		RScrollControlView::OnXLButtonDown( mousePoint, modifierKeys );
		return;
	}

	OnXSetCursor( mousePoint, modifierKeys );
}

// ****************************************************************************
//
//  Function Name:	REditCropView::OnXLButtonUp( )
//
//  Description:		Default handler for the button up XEvent.
//
//							Will end any current mouse tracking.  If a component was hit
//							but never moved, it will activate it.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::OnXLButtonUp( const RRealPoint& mousePoint, YModifierKey modifierKeys )
{
	// if we are tracking, end
	if (kTracking == m_eMode)
		OnXSetCursor( mousePoint, modifierKeys );

	else if (kTrackingPoint == m_eMode)
	{
		ReleaseMouseCapture();
		
		RRealPoint point( mousePoint );
		RRealRect rRect = m_pImageView->GetBoundingRect().m_TransformedBoundingRect;
		rRect.PinPointInRect( point );

		RUndoableAction* pAction = new REditPointAction( this, m_nTrackingIndex, m_ptTrackedPoint, point );
		GetRDocument()->SendAction( pAction );
		SetMode( kEditingPoints );

		OnXSetCursor( mousePoint, modifierKeys );
	}
	
	else
	{
		RScrollControlView::OnXLButtonUp( mousePoint, modifierKeys );
	}
}

// ****************************************************************************
//
//  Function Name:	REditCropView::OnXMouseMove( )
//
//  Description:		Default handler for the mouse move XEvent.
//
//							Will continue processing any current tracking or start tracking
//							any component hit if the delta move is larger than a defined slop
//							distance.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void REditCropView::OnXMouseMove( const RRealPoint& point, YModifierKey modifierKeys )
{
	RRealPoint mousePoint( point );

	if (kTracking != m_eMode && kTrackingPoint != m_eMode)
	{
		RScrollControlView::OnXMouseMove( mousePoint, modifierKeys );
		return;
	}

	POINT	pt;
	::GetCursorPos( &pt );
	GetCWnd( ).ScreenToClient( &pt );

	RRealPoint devicePoint = RRealPoint( pt.x, pt.y );
	RRealPoint localPoint  = devicePoint;
	::DeviceUnitsToLogicalUnits( localPoint, *this );
	localPoint = ConvertPointToLocalCoordinateSystem( localPoint );

	// Setup the autoscrolling timer
	SetupAutoScrollTimer( localPoint );

	// Determine if we are autoscrolling
	if (WillAutoScroll( localPoint ))
		AutoScroll( localPoint, kLineScrollAmount, TRUE );

	if (kTracking == m_eMode)
	{
		R2dTransform transform;
		GetViewTransform( transform, FALSE );

		// Render rubberband
		CClientDC dc( &GetCWnd() );
		RDcDrawingSurface ds;
		ds.Initialize( (HDC) dc );

		YDrawMode mode = ds.GetForegroundMode();
		RColor crOldColor = ds.GetPenColor();

		ds.SetForegroundMode( kXOR );
		ds.SetPenColor( RSolidColor( kWhite ) );

		RRealPointArrayIterator iterator = m_ptArray.End();
		RRealPoint ptEnd = *(--iterator);

		((RDrawingSurface * ) &ds)->MoveTo( ptEnd, transform );
		ds.LineTo( m_ptLast, transform );
		((RDrawingSurface * ) &ds)->MoveTo( ptEnd, transform );
		ds.LineTo( mousePoint, transform );

		ds.SetPenColor( crOldColor );
		ds.SetForegroundMode( mode );
		ds.DetachDCs();

		m_ptLast = mousePoint;

		OnXSetCursor( mousePoint, modifierKeys );
	}
	else if (kTrackingPoint == m_eMode)
	{
		RRealRect rRect = m_pImageView->GetBoundingRect().m_TransformedBoundingRect;
		rRect.PinPointInRect( mousePoint );

		RRealPoint oldPoint = m_ptArray[m_nTrackingIndex];
		RRealRect rInvalidRect( m_rectArray[m_nTrackingIndex] );

		m_ptArray[m_nTrackingIndex] = mousePoint;
		m_rectArray[m_nTrackingIndex].Offset( mousePoint - oldPoint );

		rInvalidRect.AddPointToRect( mousePoint );

		int nNext = (m_nTrackingIndex + 1) % m_ptArray.Count();
		int nPrev = (m_nTrackingIndex > 0 ? m_nTrackingIndex - 1 : m_ptArray.Count() - 1);

		rInvalidRect.AddPointToRect( m_ptArray[nPrev] );
		rInvalidRect.AddPointToRect( m_ptArray[nNext] );

		InvalidateVectorRect( rInvalidRect, TRUE );
		OnXSetCursor( mousePoint, modifierKeys );
	}
}

// ****************************************************************************
//
//  Function Name:	REditCropView::DoDragDrop( )
//
//  Description:		Begins a drag&drop operation using a tracker
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::DoDragDrop( const RRealPoint& mousePoint, BOOLEAN, YDropEffect )
	{
	// Create a drag tracker
	RRealRect rRect = CalcBoundingRectOfEmbeddedComponents( FALSE );
	m_pActiveTracker = new RGroupDialogDragSelectionTracker( GetSelection( ), rRect );

	// Begin the tracker
	m_pActiveTracker->BeginTracking( mousePoint, 0 );
	}

//*****************************************************************************
//
// Function Name:  REditCropView::OnXDragEnter
//
// Description:    Drag enter message
//
// Returns:        Drop effect
//
// Exceptions:	    None
//
//*****************************************************************************
//
YDropEffect REditCropView::OnXDragEnter( RDataTransferSource&, YModifierKey, const RRealPoint& )
	{
	return kDropEffectNone;
	}

// ****************************************************************************
//
//  Function Name:	REditCropView::OnXSetCursor( )
//
//  Description:		Default handler for the SetCursor XEvent.
//
//							Will set the cursor to be the generic arrow cursor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropView::OnXSetCursor( const RRealPoint& mousePoint, YModifierKey modifierKeys )
{
	if (kCollectPoints == m_eMode || kTracking == m_eMode)
	{
		if (m_pImageView && m_pImageView->HitTest( mousePoint ))
		{
			GetCursorManager( ).SetCursor( CURSOR_FREEHAND );
			return;
		}
	}

	RScrollControlView::OnXSetCursor( mousePoint, modifierKeys );
}

// ****************************************************************************
//
//  Function Name:	REditCropView::ConvertPointToLocalCoordinateSystem( )
//
//  Description:		Converts a point in our parents coordinate system to one
//							in our coordinate system
//
//  Returns:			The converted point
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealPoint REditCropView::ConvertPointToLocalCoordinateSystem( const RRealPoint& point ) const
	{
	if (kTracking == m_eMode)
		return RControlView::ConvertPointToLocalCoordinateSystem( point );

	return RScrollControlView::ConvertPointToLocalCoordinateSystem( point );
	}

// ****************************************************************************
//
//  Function Name:	REditCropView::CalcBoundingRectOfEmbeddedComponents( )
//
//  Description:		Normally, determines the bounding rect of the embedded 
//                   components, but we want to include the non-imbedded image
//
//  Returns:			The converted point
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RRealRect REditCropView::CalcBoundingRectOfEmbeddedComponents( BOOLEAN fIncludeSelectionHandles ) const
{
	if (m_pImage)
	{
//		RRealRect rViewable = GetViewableArea();

		RRealRect rRect( m_pImage->GetSizeInLogicalUnits() );
		rRect.m_Right *= m_yScaleFactor;
		rRect.m_Bottom *= m_yScaleFactor;
//		rRect.CenterRectInRect( rViewable );

		RRealPoint pt = rRect.GetCenterPoint();
		rRect.Offset( RRealSize( -pt.m_x, -pt.m_y ) );

		return rRect;
	}

	return RScrollControlView::GetBoundingRectOfEmbeddedComponents( fIncludeSelectionHandles );
}

// ****************************************************************************
//
//  Function Name:	REditCropView::DetermineComponentBoundingRect()
//
//  Description:	Determines the bouding rect of the image component
//
//  Returns:		nothing
//
// ****************************************************************************
//
void REditCropView::DetermineComponentBoundingRect( BOOLEAN fResize )
{
	try
	{
		RRealRect  rImageRect( m_pImage->GetSizeInLogicalUnits() );
		RRealPoint ptCenter( rImageRect.GetCenterPoint() );

		if (fResize)
		{
			RRealRect rArea = GetViewableArea();

			RRealSize scale( 
				rArea.Width()  / rImageRect.Width(), 
				rArea.Height() / rImageRect.Height() );

			if (scale.m_dy < scale.m_dx) 
				scale.m_dx = scale.m_dy;

			m_yScaleFactor = scale.m_dx;
		}

		R2dTransform xform;
		xform.PostTranslate( -ptCenter.m_x, -ptCenter.m_y );
		xform.PostScale( m_yScaleFactor, m_yScaleFactor );
		rImageRect *= xform;

		REditImageInterface* pInterface = static_cast<REditImageInterface*>(
			m_pImageView->GetInterface( kEditImageInterfaceId ) );
		RImageEffects rEffects = pInterface->GetImageEffects();
		delete pInterface;

		YComponentBoundingRect rBoundingRect( 0, 0,
			rImageRect.Width()  * rEffects.m_rCropArea.m_Right,
			rImageRect.Height() * rEffects.m_rCropArea.m_Bottom );
		rBoundingRect.Offset( RIntSize(
			rImageRect.m_Left + rImageRect.Width()  * rEffects.m_rCropArea.m_Left,
			rImageRect.m_Top  + rImageRect.Height() * rEffects.m_rCropArea.m_Top ) );

		m_pImageView->SetBoundingRect( rBoundingRect );
	}
	catch (...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	REditCropView::IsCropped()
//
//  Description:		reports TRUE if a crop has been applied
//							parent dialog
//
//  Returns:			BOOLEAN
//
// ****************************************************************************
//
BOOLEAN REditCropView::IsCropped()
{
	RImageInterface* pImageInterface = static_cast<RImageInterface*>( m_pImageView->GetInterface( kImageInterfaceId ) );
	RBitmapImage* pImage = (RBitmapImage *) pImageInterface->GetImage();
	delete pImageInterface;

	return ( pImage->GetClippingRPath() && ! pImage->PathFromImport() );
}


////////////////////////////////////////////////////////////////////////////////


// ****************************************************************************
//
//  Function Name:	REditCropDocument::REditCropDocument( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditCropDocument::REditCropDocument( RTabbedEditCrop *pParentDlg ) :
	m_pParentDlg( pParentDlg )
{
}

// ****************************************************************************
//
//  Function Name:	REditCropDocument::CreateView( )
//
//  Description:		Creates a new scrollable control view
//
//  Returns:			New view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RControlView* REditCropDocument::CreateView( CDialog* pDialog, int nControlId )
{
	return new REditCropView( pDialog, nControlId, this );
}

// ****************************************************************************
//
//  Function Name:	REditCropDocument::GetDocumentOfType( )
//
//  Description:		Finds the first document of the specified type
//
//  Returns:			Pointer to the first document of the specified type.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* REditCropDocument::GetDocumentOfType( YComponentType strType )
{
	YComponentIterator iterator = GetComponentCollectionStart( );
	YComponentIterator end = GetComponentCollectionEnd( );

	for ( ; iterator != end; iterator++)
	{
		if ((*iterator)->GetComponentType() == strType)
			return (*iterator);
	}

	return NULL;
}

// ****************************************************************************
//
//  Function Name:	REditCropDocument::FreeAllActions()
//
//  Description:	purges undo buffer by going directly through the Undo Manager
//
//  Returns:		nothing
//
// ****************************************************************************
//
void REditCropDocument::FreeAllActions()
{
	TpsAssertValid( m_pUndoManager );
	
	m_pUndoManager->FreeAllActions();
}

// ****************************************************************************
//
//  Function Name:	REditCropDocument::SendAction()
//
//  Description:	Override of SendAction, so we can enable the
//                an associated undo window.
//
//  Returns:		ancestors SendAction
//
// ****************************************************************************
//
BOOLEAN REditCropDocument::SendAction( RAction* pAction )
{
	TpsAssertValid( m_pUndoManager );

	BOOLEAN bRet = RScrollControlDocument::SendAction( pAction );

	m_pParentDlg->UpdateButtonControls();

	return bRet;
}

// ****************************************************************************
//
//  Function Name:	REditCropDocument::Undo( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditCropDocument::Undo( )
{
	TpsAssertValid( m_pUndoManager );

	m_pUndoManager->UndoAction( );
	m_pUndoManager->FreeUndoneActions( );

	m_pParentDlg->UpdateButtonControls();
}
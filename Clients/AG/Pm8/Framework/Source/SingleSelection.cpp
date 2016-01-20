// ****************************************************************************
//
//  File Name:			SingleSelection.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RSingleSelection class
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
//  $Logfile:: /PM8/Framework/Source/SingleSelection.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "SingleSelection.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentView.h"
#include "DrawingSurface.h"
#include "ComponentAttributes.h"
#include "utilities.h"

const YIntDimension	kSelectionOutlineWidth	= 1;
const YIntDimension	kRotateHandleRenderSize	= 7;
const YIntDimension	kResizeHandleHitSize		= 16;	//	approx 1/2 cursor size of 32
const YIntDimension	kRotateHandleHitSize		= 16;	//	approx 1/2 cursor size of 32
const YIntDimension	kCornerResizeHandleRenderSize	= 7;
const YIntDimension	kEdgeResizeHandleRenderSize	= 5;
const YRealDimension	kRotateHandleLineLengthRatio	= 0.125;
const YIntDimension	kSelectionOutlineDistanceFromObject = 0;//2;

const YRealDimension	kDummyScale			= 100.0;
const YRealDimension	kConstrainedScale	= 1.0;

// ****************************************************************************
//
//  Function Name:	RSingleSelection::RSingleSelection( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSingleSelection::RSingleSelection( const RComponentView* pSelectedObject, RView* pView )
	: m_pSelectedObject( pSelectedObject ),
	  RSelection( pView )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::~RSingleSelection( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSingleSelection::~RSingleSelection( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::Clone( )
//
//  Description:		virtual constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelection* RSingleSelection::Clone( )
	{
	return new RSingleSelection( *this );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetObjectBoundingRect( )
//
//  Description:		Retrieves the bounding rect of the selected object
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RSingleSelection::GetObjectBoundingRect( YComponentBoundingRect& boundingRect ) const
	{
	boundingRect = m_pSelectedObject->GetBoundingRect( );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetSelectionBoundingRect( )
//
//  Description:		Retrieves the bounding rect of this selection
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RSingleSelection::GetSelectionBoundingRect( YSelectionBoundingRect& boundingRect ) const
	{
	boundingRect = m_pSelectedObject->GetBoundingRect( );
	SelectionRectFromObjectRect( boundingRect, m_pView, TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::SelectionRectFromObjectRect( )
//
//  Description:		Converts the given bounding rect to a selection rect
//							by inflating
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSingleSelection::SelectionRectFromObjectRect( YComponentBoundingRect& objectBoundingRect, const RView* pView, BOOLEAN fOutset )
	{
	// Convert to device units	
	::LogicalUnitsToDeviceUnits( objectBoundingRect, *pView );

	// Create the selection rect
	SelectionRectFromDeviceObjectRect( objectBoundingRect, fOutset );

	// Convert back to logical units
	::DeviceUnitsToLogicalUnits( objectBoundingRect, *pView );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::SelectionRectFromDeviceObjectRect( )
//
//  Description:		Converts the given bounding rect to a selection rect
//							by inflating
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSingleSelection::SelectionRectFromDeviceObjectRect( YComponentBoundingRect& objectBoundingRect, BOOLEAN fOutset )
	{
	// Dont bother if the rect is too small
	if( objectBoundingRect.IsEmpty( ) )
		return;

	// If Outsetting, inflate the rect. Otherwise, deflate the rect
	if( fOutset == TRUE )
		objectBoundingRect.Inflate( RRealSize( (kSelectionOutlineWidth+kSelectionOutlineDistanceFromObject)/2.F, (kSelectionOutlineWidth+kSelectionOutlineDistanceFromObject)/2.F ) );
	else
		objectBoundingRect.Inflate( RRealSize( -( kSelectionOutlineWidth+kSelectionOutlineDistanceFromObject ), -( kSelectionOutlineWidth+kSelectionOutlineDistanceFromObject ) ) );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetObjectMaximumSize( )
//
//  Description:		Retrieves the maximum size that the object may be.
//
//  Returns:			The maximum size
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RSingleSelection::GetObjectMaximumSize( ) const
	{
	return m_pSelectedObject->GetMaximumSize( );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetObjectMinimumSize( )
//
//  Description:		Retrieves the minimum size that the object may be.
//
//  Returns:			The minimum size
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RSingleSelection::GetObjectMinimumSize( ) const
	{
	return m_pSelectedObject->GetMinimumSize( );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetSelectionMinimumSize( )
//
//  Description:		Retrieves the minimum size that the selection rect may
//							be.
//
//  Returns:			The minimum size
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RSingleSelection::GetSelectionMinimumSize( ) const
	{
	// Retrieve the object minimum size
	RRealSize minimumSize = m_pSelectedObject->GetMinimumSize( );

	// Convert it to device units
	::LogicalUnitsToDeviceUnits( minimumSize, *m_pView );

	// Adjust the size
	minimumSize.m_dx += ( kSelectionOutlineWidth + kSelectionOutlineDistanceFromObject + 1 ) * 2;
	minimumSize.m_dy += ( kSelectionOutlineWidth + kSelectionOutlineDistanceFromObject + 1 ) * 2;

	// Convert back to logical units
	::DeviceUnitsToLogicalUnits( minimumSize, *m_pView );

	return minimumSize;
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::Render( )
//
//  Description:		Draws the selection rect and handles
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RSingleSelection::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, BOOLEAN fRenderIfLocked ) const
	{
	// Get the component attributes
	const		RComponentAttributes& componentAttributes = m_pSelectedObject->GetComponentAttributes( );
	BOOLEAN	fIsLocked = componentAttributes.IsLocked( );

	if( fRenderIfLocked || !fIsLocked )
		{
		// Set surface attributes
		drawingSurface.SetPenWidth( kSelectionOutlineWidth );
		drawingSurface.SetPenStyle( kSolidPen );
		
		RRealRect			resizeHandleRect;

		// Draw the resize handles
		if( componentAttributes.IsResizable( FALSE ) )
			{
			RArray<RRealRect>					rectList;
			RRealSize	scaleFactor( kDummyScale, kDummyScale );
			scaleFactor = m_pSelectedObject->ApplyResizeConstraint( scaleFactor );

			GetResizeSelectionHandle( resizeHandleRect, transform, kTopLeftResizeHandle, TRUE );
			rectList.InsertAtEnd( resizeHandleRect );

			GetResizeSelectionHandle( resizeHandleRect, transform, kTopRightResizeHandle, TRUE );
			rectList.InsertAtEnd( resizeHandleRect );

			GetResizeSelectionHandle( resizeHandleRect, transform, kBottomRightResizeHandle, TRUE );
			rectList.InsertAtEnd( resizeHandleRect );

			GetResizeSelectionHandle( resizeHandleRect, transform, kBottomLeftResizeHandle, TRUE );
			rectList.InsertAtEnd( resizeHandleRect );

			if ( !AreFloatsEqual( scaleFactor.m_dy, kConstrainedScale ) )
				{
				GetResizeSelectionHandle( resizeHandleRect, transform, kBottomResizeHandle, TRUE );
				rectList.InsertAtEnd( resizeHandleRect );

				GetResizeSelectionHandle( resizeHandleRect, transform, kTopResizeHandle, TRUE );
				rectList.InsertAtEnd( resizeHandleRect );
				}

			if ( !AreFloatsEqual( scaleFactor.m_dx, kConstrainedScale ) )
				{
				GetResizeSelectionHandle( resizeHandleRect, transform, kRightResizeHandle, TRUE );
				rectList.InsertAtEnd( resizeHandleRect );

				GetResizeSelectionHandle( resizeHandleRect, transform, kLeftResizeHandle, TRUE );
				rectList.InsertAtEnd( resizeHandleRect );
				}

			RArray<RRealRect>::YIterator	iterator		= rectList.Start( );
			RArray<RRealRect>::YIterator	iteratorEnd = rectList.End( );
			if( fIsLocked )
				{
				while ( iterator != iteratorEnd )
					drawingSurface.FrameRectangle( *iterator++ );
				}
			else
				{
				while ( iterator != iteratorEnd )
					drawingSurface.FillRectangle( *iterator++ );
				}
			}

		// Draw the rotate line and handle
		if( componentAttributes.IsRotatable( ) )
			DrawRotateHandle( drawingSurface, transform, rcRender );

		// Draw the selection frame
		m_pSelectedObject->DrawSelectionFrame( drawingSurface, transform );
		}
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::DrawRotateHandle( )
//
//  Description:		Draws the rotation line and handle
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RSingleSelection::DrawRotateHandle( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& ) const
	{
	// Get the bounding rect
	YSelectionBoundingRect boundingRect;
	GetSelectionBoundingRect( boundingRect );

	// Get the center point; this will be one end point of the rotate handle line
	RRealPoint centerPoint = boundingRect.GetCenterPoint( );

	// Get the other end point
	RRealPoint lineEndPoint = GetRotateHandleCenterPoint( );

	// Draw the line
	drawingSurface.MoveTo( centerPoint, transform );
	drawingSurface.LineTo( lineEndPoint, transform );

	// Now draw the rotate handle
	// Convert to device units
	::LogicalUnitsToDeviceUnits( lineEndPoint, *m_pView );

	// Now build the rotate handle, using this point as the middle of the left side
	RRealRect	rotateHandleRect;
	rotateHandleRect.m_Left		= lineEndPoint.m_x - ( kRotateHandleRenderSize / 2 );
	rotateHandleRect.m_Top		= lineEndPoint.m_y - ( kRotateHandleRenderSize / 2 );
	rotateHandleRect.m_Right	= lineEndPoint.m_x + ( kRotateHandleRenderSize / 2 ) + 1;
	rotateHandleRect.m_Bottom	= lineEndPoint.m_y + ( kRotateHandleRenderSize / 2 ) + 1;
	::DeviceUnitsToLogicalUnits( rotateHandleRect, *m_pView );

	// Now draw the rotate handle
	rotateHandleRect *= transform;
	drawingSurface.FillRectangle( rotateHandleRect );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetRotateHandle( )
//
//  Description:		Retrieves a vector rect representing the rotation handle.
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RSingleSelection::GetRotateHandle( RRealVectorRect& rotateHandleRect ) const
	{
	// Get the bounding rect
	YSelectionBoundingRect boundingRect;
	GetSelectionBoundingRect( boundingRect );

	RRealPoint		centerPoint = boundingRect.GetCenterPoint( );
	R2dTransform	transform( boundingRect.GetTransform( ) );
	transform.PostTranslate( centerPoint.m_x-boundingRect.m_TopLeft.m_x, centerPoint.m_y-boundingRect.m_TopLeft.m_y );

	//	Break the transform into its components
	YAngle			angle;
	YRealDimension	xScale;
	YRealDimension	yScale;
	transform.Decompose( angle, xScale, yScale );

	// Figure out where to end the line
	RRealPoint		lineEndPoint	= GetRotateHandleCenterPoint( );
	YRealDimension	distance			= centerPoint.Distance( lineEndPoint );

	if ( xScale < 0 )
		xScale = -xScale;
	rotateHandleRect.Set( RRealSize(distance/xScale,1), transform );

	// Convert to device units
	RRealSize	handleSize( kRotateHandleHitSize/2, kRotateHandleHitSize/2 );
	::DeviceUnitsToLogicalUnits( handleSize, *m_pView );

	rotateHandleRect.Inflate( handleSize );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetRotateHandleCenterPoint( )
//
//  Description:		Retrieves the center point of the rotate handle
//
//  Returns:			See above
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RRealPoint RSingleSelection::GetRotateHandleCenterPoint( ) const
	{
	// Get the bounding rect and center point
	YSelectionBoundingRect boundingRect;
	GetSelectionBoundingRect( boundingRect );
	RRealPoint centerPoint = boundingRect.GetCenterPoint( );

	// We are going to use ratios of similar triangles to calculate the position of the rotate handle
	YRealDimension deltaX = boundingRect.m_BottomRight.m_x - boundingRect.m_BottomLeft.m_x;
	YRealDimension deltaY = boundingRect.m_BottomRight.m_y - boundingRect.m_BottomLeft.m_y;

	YRealDimension deltaXPrime = deltaX * ( 0.5 + kRotateHandleLineLengthRatio );
	YRealDimension deltaYPrime = deltaY * ( 0.5 + kRotateHandleLineLengthRatio );

	return RRealPoint( centerPoint.m_x + deltaXPrime, centerPoint.m_y + deltaYPrime );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetResizeSelectionHandle( )
//
//  Description:		Retrieves a vector rect representing requested resize handle.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RSingleSelection::GetResizeSelectionHandle( RRealRect& rHandle, const R2dTransform& transform, EHitLocation eHit, BOOLEAN fRender ) const
	{
	RRealPoint					point;
	YSelectionBoundingRect	boundingRect;
	RIntSize						outsetSize = (fRender)? RRealSize( kCornerResizeHandleRenderSize, kCornerResizeHandleRenderSize ) : 
																	RRealSize( kResizeHandleHitSize, kResizeHandleHitSize );

	//	Get the bounding rectangle
	GetSelectionBoundingRect( boundingRect );

	//	Get the proper point to transform
	switch ( eHit )
		{
		case kLeftResizeHandle:
			::midpoint( point, boundingRect.m_TopLeft, boundingRect.m_BottomLeft );
			outsetSize.m_dx	= Max( kEdgeResizeHandleRenderSize, YIntDimension(outsetSize.m_dx/2) );
			break;

		case kTopResizeHandle:
			::midpoint( point, boundingRect.m_TopLeft, boundingRect.m_TopRight );
			outsetSize.m_dy	= Max( kEdgeResizeHandleRenderSize, YIntDimension(outsetSize.m_dy/2) );
			break;

		case kRightResizeHandle:
			::midpoint( point, boundingRect.m_TopRight, boundingRect.m_BottomRight );
			outsetSize.m_dx	= Max( kEdgeResizeHandleRenderSize, YIntDimension(outsetSize.m_dx/2) );
			break;

		case kBottomResizeHandle:
			::midpoint( point, boundingRect.m_BottomLeft, boundingRect.m_BottomRight );
			outsetSize.m_dy	= Max( kEdgeResizeHandleRenderSize, YIntDimension(outsetSize.m_dy/2) );
			break;

		case kTopLeftResizeHandle:
			point	= boundingRect.m_TopLeft;
			break;

		case kTopRightResizeHandle:
			point	= boundingRect.m_TopRight;
			break;

		case kBottomLeftResizeHandle:
			point	= boundingRect.m_BottomLeft;
			break;

		case kBottomRightResizeHandle:
			point	= boundingRect.m_BottomRight;
			break;

		default :
			TpsAssertAlways( "Asking for the selection handle of invalid type" );
			point	= boundingRect.GetCenterPoint();
		}

		RIntSize		halfOutset( outsetSize.m_dx/2, outsetSize.m_dy/2);
		::LogicalUnitsToDeviceUnits( point, *m_pView );
		rHandle.m_Left		= point.m_x - halfOutset.m_dx;
		rHandle.m_Top		= point.m_y - halfOutset.m_dy;
		rHandle.m_Right	= point.m_x + outsetSize.m_dx - halfOutset.m_dx;
		rHandle.m_Bottom	= point.m_y + outsetSize.m_dy - halfOutset.m_dy;
		::DeviceUnitsToLogicalUnits( rHandle, *m_pView );		

		rHandle	*= transform;
	}


// ****************************************************************************
//
//  Function Name:	RSingleSelection::DrawDragTrackingRect( )
//
//  Description:		Draws the selection tracking rect for dragging
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSingleSelection::DrawDragTrackingFeedback( RDrawingSurface& drawingSurface,
																 const R2dTransform& transform,
																 const RRealSize& offset ) const
	{
	if( m_pSelectedObject->GetComponentAttributes( ).IsMovable( ) )
		{
		// Setup the drawing surface
		drawingSurface.SetPenWidth( kSelectionOutlineWidth );
		drawingSurface.SetPenStyle( kSolidPen );

		// Get the components bounding rect
		YComponentBoundingRect boundingRect = m_pSelectedObject->GetBoundingRect( );

		// Add the translate operation
		boundingRect.Offset( offset );

		// Tell the object to draw tracking feedback
		m_pSelectedObject->RenderTrackingFeedback( drawingSurface, boundingRect.GetTransform( ) * transform, *GetView( ) );
		}
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::DrawRotateTrackingRect( )
//
//  Description:		Draws the selection tracking rect for rotating.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

void RSingleSelection::DrawRotateTrackingFeedback( RDrawingSurface& drawingSurface,
																	const R2dTransform& transform,
																	const RRealPoint& rotationCenter,
																	YAngle rotationAngle ) const
	{
	if( m_pSelectedObject->GetComponentAttributes( ).IsRotatable( ) )
		{
		// Setup the drawing surface
		drawingSurface.SetPenWidth( kSelectionOutlineWidth );
		drawingSurface.SetPenStyle( kSolidPen );

		// Get the components bounding rect
		YComponentBoundingRect boundingRect = m_pSelectedObject->GetBoundingRect( );

		// Add the rotate operation
		boundingRect.RotateAboutPoint( rotationCenter, rotationAngle );

		// Tell the object to draw tracking feedback
		m_pSelectedObject->RenderTrackingFeedback( drawingSurface, boundingRect.GetTransform( ) * transform, *GetView( ) );
		}
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::DrawResizeTrackingRect( )
//
//  Description:		Draws the selection tracking rect for resizing.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RSingleSelection::DrawResizeTrackingFeedback( RDrawingSurface& drawingSurface,
																	const R2dTransform& transform,
																	const RRealPoint& scalingCenter,
																	const RRealSize& scaleFactor,
																	BOOLEAN fMaintainAspectRatio ) const
	{
	if( m_pSelectedObject->GetComponentAttributes( ).IsResizable( ) )
		{
		// Setup the drawing surface
		drawingSurface.SetPenWidth( kSelectionOutlineWidth );
		drawingSurface.SetPenStyle( kSolidPen );

		// Get the components bounding rect
		YComponentBoundingRect boundingRect = m_pSelectedObject->GetBoundingRect( );

		// Constrain the scale factor
		RRealSize constrainedScaleFactor = m_pSelectedObject->ApplyResizeConstraint( scaleFactor );

		// Dont force to maintain aspect ratio if it really doesnt want to
		fMaintainAspectRatio = fMaintainAspectRatio && ::AreFloatsEqual( constrainedScaleFactor.m_dx, constrainedScaleFactor.m_dy );

		// Add the scale operation
		boundingRect.UnrotateAndScaleAboutPoint( scalingCenter, constrainedScaleFactor, m_pSelectedObject->GetMinimumSize( ), m_pSelectedObject->GetMaximumSize( ), fMaintainAspectRatio );

		// Tell the object to draw tracking feedback
		m_pSelectedObject->RenderTrackingFeedback( drawingSurface, boundingRect.GetTransform( ) * transform, *GetView( ) );
		}
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetDragTrackingFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rect of drag tracking feedback
//
//  Returns:			Bounding rect
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RRealRect RSingleSelection::GetDragTrackingFeedbackBoundingRect( const R2dTransform& transform,
																					  const RRealSize& offset ) const
	{
	// Get the components bounding rect
	YComponentBoundingRect boundingRect = m_pSelectedObject->GetBoundingRect( );

	// Add the translate operation
	boundingRect.Offset( offset );

	// Get the objects feedback size
	return m_pSelectedObject->GetTrackingFeedbackBoundingRect( boundingRect.GetTransform( ) * transform );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetRotateTrackingFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rect of resize tracking feedback
//
//  Returns:			Bounding rect
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RRealRect RSingleSelection::GetRotateTrackingFeedbackBoundingRect( const R2dTransform& transform,
																						 const RRealPoint& rotationCenter,
																						 YAngle rotationAngle ) const
	{
	// Get the components bounding rect
	YComponentBoundingRect boundingRect = m_pSelectedObject->GetBoundingRect( );

	// Add the rotate operation
	boundingRect.RotateAboutPoint( rotationCenter, rotationAngle );

	// Get the objects feedback size
	return m_pSelectedObject->GetTrackingFeedbackBoundingRect( boundingRect.GetTransform( ) * transform );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::GetResizeTrackingFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rect of resize tracking feedback
//
//  Returns:			Bounding rect
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RRealRect RSingleSelection::GetResizeTrackingFeedbackBoundingRect( const R2dTransform& transform,
																						 const RRealPoint& scalingCenter,
																						 const RRealSize& scaleFactor,
																						 BOOLEAN fMaintainAspectRatio ) const
	{	
	// Get the components bounding rect
	YComponentBoundingRect boundingRect = m_pSelectedObject->GetBoundingRect( );

	// Constrain the scale factor
	RRealSize constrainedScaleFactor = m_pSelectedObject->ApplyResizeConstraint( scaleFactor );

	// Dont force to maintain aspect ratio if it really doesnt want to
	fMaintainAspectRatio = fMaintainAspectRatio && ::AreFloatsEqual( constrainedScaleFactor.m_dx, constrainedScaleFactor.m_dy );

	// Add the scale operation
	boundingRect.UnrotateAndScaleAboutPoint( scalingCenter, constrainedScaleFactor, m_pSelectedObject->GetMinimumSize( ), m_pSelectedObject->GetMaximumSize( ), fMaintainAspectRatio );

	// Get the objects feedback size
	return m_pSelectedObject->GetTrackingFeedbackBoundingRect( boundingRect.GetTransform( ) * transform );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::HitTest( )
//
//  Description:		Hits test this selection
//
//  Returns:			The location of this selection that was hit
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RSelection::EHitLocation RSingleSelection::HitTest( const RRealPoint& point ) const
	{
	RRealRect	rect;
	RRealSize	scaleFactor(kDummyScale,kDummyScale);	//	This is just to be able to see if the object is constrainted

	scaleFactor = m_pSelectedObject->ApplyResizeConstraint( scaleFactor );

	// Get the component attributes
	const RComponentAttributes& componentAttributes = m_pSelectedObject->GetComponentAttributes( );

	if( componentAttributes.IsResizable( ) )
		{
		R2dTransform	transform;
		// Test the resize handles
		GetResizeSelectionHandle( rect, transform, kTopLeftResizeHandle, FALSE );
		if( rect.PointInRect( point ) )
			return kTopLeftResizeHandle;

		GetResizeSelectionHandle( rect, transform, kTopRightResizeHandle, FALSE );
		if( rect.PointInRect( point ) )
			return kTopRightResizeHandle;

		GetResizeSelectionHandle( rect, transform, kBottomRightResizeHandle, FALSE );
		if( rect.PointInRect( point ) )
			return kBottomRightResizeHandle;

		GetResizeSelectionHandle( rect, transform, kBottomLeftResizeHandle, FALSE );
		if( rect.PointInRect( point ) )
			return kBottomLeftResizeHandle;

		if ( !AreFloatsEqual( scaleFactor.m_dy, kConstrainedScale ) )
			{
			GetResizeSelectionHandle( rect, transform, kTopResizeHandle, FALSE );
			if( rect.PointInRect( point ) )
				return kTopResizeHandle;

			GetResizeSelectionHandle( rect, transform, kBottomResizeHandle, FALSE );
			if( rect.PointInRect( point ) )
				return kBottomResizeHandle;
			}

		if ( !AreFloatsEqual( scaleFactor.m_dx, kConstrainedScale ) )
			{
			GetResizeSelectionHandle( rect, transform, kRightResizeHandle, FALSE );
			if( rect.PointInRect( point ) )
				return kRightResizeHandle;

			GetResizeSelectionHandle( rect, transform, kLeftResizeHandle, FALSE );
			if( rect.PointInRect( point ) )
				return kLeftResizeHandle;
			}

		}

	// Test the selection rect
	if( componentAttributes.IsMovable( ) )
		{
		if( m_pSelectedObject->HitTestSelectionFrame( point ) )
			return kInside;
		}

	// Test the rotate handle
	if( componentAttributes.IsRotatable( ) )
		{
		RRealVectorRect	vRect;
		GetRotateHandle( vRect );
		if( vRect.PointInRect( point ) )
			return kRotateHandle;
		}

	return kNothing;
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::Invalidate( )
//
//  Description:		Invalidates the region occupied by this selection in the
//							given view.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RSingleSelection::Invalidate( BOOLEAN fInvalidateIfLocked ) const
	{
	if( fInvalidateIfLocked || m_pSelectedObject->GetComponentAttributes( ).IsLocked( ) == FALSE )
		{
		// Get the full soft effects bounding rect and invalidate it
		YSelectionBoundingRect boundingRect;
		boundingRect = m_pSelectedObject->GetFullSoftEffectsBoundingRect();
		SelectionRectFromObjectRect( boundingRect, m_pView, TRUE );
		RRealRect invalidRect = boundingRect.m_TransformedBoundingRect;
		
		// Extend the rectangle to contain the rotate handle
		invalidRect.AddPointToRect( GetRotateHandleCenterPoint( ) );
			
		// Inflate by the rotate handle size
		RRealSize rotateHandleSize( kRotateHandleRenderSize + 1, kRotateHandleRenderSize + 1);
		::DeviceUnitsToLogicalUnits( rotateHandleSize, *m_pView );
		invalidRect.Inflate( rotateHandleSize );
		
		// Do the invalidate
		m_pView->InvalidateVectorRect( invalidRect );
		}
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::IsSelected( )
//
//  Description:		Determines if the given selectable object is selected by
//							this selection.
//
//  Returns:			TRUE if the object is selected
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
BOOLEAN RSingleSelection::IsSelected( const RComponentView* pSelectableObject )
	{
	return static_cast<BOOLEAN>( m_pSelectedObject == pSelectableObject );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::MaintainAspectRatioIsDefault( )
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
BOOLEAN RSingleSelection::MaintainAspectRatioIsDefault( ) const
	{
	return m_pSelectedObject->MaintainAspectRatioIsDefault( );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::CanChangeDefaultAspect( )
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
BOOLEAN RSingleSelection::CanChangeDefaultAspect( ) const
	{
	return m_pSelectedObject->CanChangeDefaultAspect( );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::IsHigherZOrder( )
//
//  Description:		Returns TRUE if the object selected by this selection has
//							a higher z-order than the object selected by the specified
//							selection
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RSingleSelection::IsHigherZOrder( const RSingleSelection& selection ) const
	{
	return static_cast<BOOLEAN>( m_pView->GetComponentZPosition( m_pSelectedObject ) > m_pView->GetComponentZPosition( selection.m_pSelectedObject ) );
	}

// ****************************************************************************
//
//  Function Name:	RSingleSelection::IsHigherZOrder( )
//
//  Description:		Returns TRUE if the object selected by this selection has
//							a higher z-order than the specified z-order
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RSingleSelection::IsHigherZOrder( YComponentZPosition zPosition ) const
	{
	return static_cast<BOOLEAN>( m_pView->GetComponentZPosition( m_pSelectedObject ) > zPosition );
	}

// ****************************************************************************
//
//  Function Name:	SingleSelection::GetBoundingRectIncludingHandles
//
//  Description:		Calculates a bounding rect for the specified object that
//							includes the resize and rotate handles
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RSingleSelection::GetBoundingRectIncludingHandles( const RComponentView* pComponentView )
	{
	// Create a new single selection
	RSingleSelection selection( pComponentView, pComponentView->GetParentView( ) );

	// Get the bounding rect of the component
	RRealRect rect = pComponentView->GetBoundingRect( ).m_TransformedBoundingRect;

	// Get the component attributes
	const RComponentAttributes& componentAttributes = pComponentView->GetComponentAttributes( );

	RRealRect handleRect;

	// Get the resize handles
	if( componentAttributes.IsResizable( ) )
		{
		R2dTransform	transform;
		selection.GetResizeSelectionHandle( handleRect, transform, kTopLeftResizeHandle, TRUE );
		rect.AddRectToRect( handleRect );

		selection.GetResizeSelectionHandle( handleRect, transform, kTopRightResizeHandle, TRUE );
		rect.AddRectToRect( handleRect );

		selection.GetResizeSelectionHandle( handleRect, transform, kBottomRightResizeHandle, TRUE );
		rect.AddRectToRect( handleRect );

		selection.GetResizeSelectionHandle( handleRect, transform, kBottomLeftResizeHandle, TRUE );
		rect.AddRectToRect( handleRect );
		}

	// Get the rotate handle
	if( componentAttributes.IsRotatable( ) )
		{
		RRealVectorRect	vRect;
		selection.GetRotateHandle( vRect );
		rect.AddRectToRect( vRect.m_TransformedBoundingRect );
		}

	return rect;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RSingleSelection::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSingleSelection::Validate( )	const
	{
	RSelection::Validate( );
	TpsAssertIsObject( RSingleSelection, this );
	TpsAssert( m_pSelectedObject, "NULL selected object." );
	TpsAssertValid( m_pSelectedObject );
	}

#endif

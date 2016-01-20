// ****************************************************************************
//
//  File Name:			SelectionTracker.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RDragSelectionTracker class
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
//  $Logfile:: /PM8/Framework/Source/SelectionTracker.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"SelectionTracker.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "CompositeSelection.h"
#include "SingleSelection.h"
#include	"StandaloneView.h"
#include "MoveSelectionAction.h"
#include "RotateSelectionAction.h"
#include "ResizeSelectionAction.h"
#include "CopySelectionAction.h"
#include "DragDropDataTransfer.h"
#include "ViewDrawingSurface.h"
#include "Document.h"

const YModifierKey kAspectModifier = kModifierControl;
const YModifierKey kRotateConstrainModifier = kModifierControl;

// ****************************************************************************
//
//  Function Name:	RSelectionTracker::RSelectionTracker( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionTracker::RSelectionTracker( RCompositeSelection* pSelection )
	: RMouseTracker( pSelection->GetView( ) ),
	  m_pSelection( pSelection )
	{
	m_pFeedbackRenderer = new RSelectionTrackingFeedbackRenderer( pSelection->GetView( ), this );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTracker::~RSelectionTracker( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionTracker::~RSelectionTracker( )
	{
	delete m_pFeedbackRenderer;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTracker::BeginTracking( )
//
//  Description:		Called when tracking begins; ie. when the mouse button goes
//							down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionTracker::BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Save the mouse down point
	m_TrackingMouseDownPoint = mousePoint;

	// Call the base class
	RMouseTracker::BeginTracking( mousePoint, modifierKeys );

	// Draw the initial tracking rect
	m_pFeedbackRenderer->BeginTracking( mousePoint );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTracker::ContinueTracking( )
//
//  Description:		Called to continue tracking; ie. when the mouse moves.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionTracker::ContinueTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Constrain the mouse point
	RRealPoint constrainedPoint = mousePoint;
	ApplyConstraint( constrainedPoint );

	// Call the base class
	RMouseTracker::ContinueTracking( constrainedPoint, modifierKeys );

	// Draw feedback
	m_pFeedbackRenderer->ContinueTracking( constrainedPoint );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTracker::EndTracking( )
//
//  Description:		Called when tracking ends; ie. when the mouse button goes
//							up.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionTracker::EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Call the base class
	RMouseTracker::EndTracking( mousePoint, modifierKeys );

	// Draw feedback
	m_pFeedbackRenderer->EndTracking( );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTracker::CancelTracking( )
//
//  Description:		Called when tracking is canceled. 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionTracker::CancelTracking( )
	{
	// Call the base method
	RMouseTracker::CancelTracking( );

	// Put back the selection
	m_pSelection->Render( );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTracker::Render( )
//
//  Description:		Called to render the tracker feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionTracker::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const
	{
	m_pFeedbackRenderer->Render( drawingSurface, transform );
	}

// ****************************************************************************
//
//  Function Name:	RDragSelectionTracker::RDragSelectionTracker( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragSelectionTracker::RDragSelectionTracker( RCompositeSelection* pSelection )
	: RSelectionTracker( pSelection )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RDragSelectionTracker::BeginTracking( )
//
//  Description:		Called when tracking begins; ie. when the mouse button goes
//							down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragSelectionTracker::BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Remove the selection
	m_pSelection->Remove( FALSE );

	// Call the base method
	RSelectionTracker::BeginTracking( mousePoint, modifierKeys );
	}

// ****************************************************************************
//
//  Function Name:	RDragSelectionTracker::EndTracking( )
//
//  Description:		Called when tracking ends; ie. when the mouse button goes
//							up.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragSelectionTracker::EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Constrain the mouse point
	RRealPoint constrainedPoint = mousePoint;
	ApplyConstraint( constrainedPoint );

	// Call the base method
	RSelectionTracker::EndTracking( constrainedPoint, modifierKeys );

	// Create and do a move selection action
	RMoveSelectionAction* pAction = new RMoveSelectionAction( m_pSelection, constrainedPoint - m_TrackingMouseDownPoint ); 
	m_pView->GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RDragSelectionTracker::Render( )
//
//  Description:		Called to render the tracker feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragSelectionTracker::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RRealPoint& point ) const
	{
	// Render the tracking rectangle
	m_pSelection->DrawDragTrackingFeedback( drawingSurface, transform, point - m_TrackingMouseDownPoint );
	}

// ****************************************************************************
//
//  Function Name:	RDragSelectionTracker::GetFeedbackBoundingRect( )
//
//  Description:		Returns the feedback bounding rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RDragSelectionTracker::GetFeedbackBoundingRect( const RRealPoint& point ) const
	{
	// Get the view transform
	R2dTransform transform;
	m_pView->GetViewTransform( transform, TRUE );

	// Get the bounding rect, in device units
	RRealRect boundingRect = m_pSelection->GetDragTrackingFeedbackBoundingRect( transform, point - m_TrackingMouseDownPoint );

	// Convert back to logical units
	transform.Invert( );
	return boundingRect * transform;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTracker::ApplyConstraint( )
//
//  Description:		Constrains the mouse point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionTracker::ApplyConstraint( RRealPoint& ) const
	{
	;
	}
	
#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RDragSelectionTracker::Validate( )
//
//  Description:		Validate the Object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragSelectionTracker::Validate( ) const
	{
	RMouseTracker::Validate( );
	TpsAssertIsObject( RDragSelectionTracker, this );
	TpsAssertValid( m_pView );
	TpsAssertValid( m_pSelection );
	}

#endif	//	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RRotateSelectionTracker::RRotateSelectionTracker( )
//
//  Description:		constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRotateSelectionTracker::RRotateSelectionTracker( RCompositeSelection* pSelection )
	: RSelectionTracker( pSelection )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionTracker::BeginTracking( )
//
//  Description:		Called when tracking begins; ie. when the mouse button goes
//							down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateSelectionTracker::BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Remove the selection
	m_pSelection->Remove( FALSE );

	// Get the controlling object
	RSingleSelection* pHitObject = m_pSelection->GetHitSingleSelection( mousePoint );
	TpsAssert( pHitObject, "No hit object!" );

	// Save the original handle point
	m_TrackingOriginalHandlePoint = pHitObject->GetRotateHandleCenterPoint( );

	// Get its bounding rect
	YSelectionBoundingRect objectBoundingRect;
	pHitObject->GetObjectBoundingRect( objectBoundingRect );

	// Save away the initial rotation angle of the controlling object. We might 
	// need it to do constraining
	RRealSize delta = m_TrackingOriginalHandlePoint - objectBoundingRect.GetCenterPoint( );
	m_ControllingObjectInitialAngle = ::atan2( -delta.m_dy, delta.m_dx );

	// Get the center of the selection and use it as the center of rotation
	YSelectionBoundingRect boundingRect;
	m_pSelection->GetSelectionBoundingRect( boundingRect );
	m_TrackingRotationCenter = boundingRect.GetCenterPoint( );

	// Compute the initial angle
	delta = m_TrackingOriginalHandlePoint - m_TrackingRotationCenter;
	m_TrackingInitialRotationAngle = ::atan2( delta.m_dy, delta.m_dx );

	// If the appropriate modifiey key is down, constrain the rotation
	m_fConstrainRotation = static_cast<BOOLEAN>( modifierKeys & kRotateConstrainModifier );

	// Call the base class
	RSelectionTracker::BeginTracking( mousePoint, modifierKeys );
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionTracker::EndTracking( )
//
//  Description:		Called when tracking ends; ie. when the mouse button goes
//							up.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateSelectionTracker::EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Constrain the mouse point
	RRealPoint constrainedPoint = mousePoint;
	ApplyConstraint( constrainedPoint );

	// Call the base class
	RSelectionTracker::EndTracking( constrainedPoint, modifierKeys );

	// Calculate the angle
	YAngle angle = CalcAngle( constrainedPoint );

	// Create and do a rotate selection action
	RRotateSelectionAction* pAction = new RRotateSelectionAction( m_pSelection, m_TrackingRotationCenter, angle ); 
	m_pView->GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionTracker::CalcAngle( )
//
//  Description:		Calculates the current angle for this rotation tracker
//
//  Returns:			The angle
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAngle RRotateSelectionTracker::CalcAngle( const RRealPoint& mousePoint ) const
	{
	// Calculate the new rotation angle. Use the law of cosines:
	//		c^2 = a^2 + b^2 - 2abcosC
	//		where:
	//			A = The original handle position
	//			B = The current mouse position
	//			C = The center of rotation

	YRealDimension a = m_TrackingRotationCenter.Distance( m_TrackingOriginalHandlePoint );
	YRealDimension b = m_TrackingRotationCenter.Distance( mousePoint );
	YRealDimension c = mousePoint.Distance( m_TrackingOriginalHandlePoint );

	// The distance from the center point to the original handle point should be positive
	TpsAssert( ::AreFloatsGT( a, 0.0 ), "Distance from rotation center to handle must be positive." );

	YAngle angle = 0.0;

	// Check for the mouse point over the rotation center
	if( !::AreFloatsEqual( b, 0.0 ) )
		{
		YFloatType cosAngle = ( ( a * a ) + ( b * b ) - ( c * c ) ) / ( 2 * a * b );

		// Constrain the cos to legal values. I have seen cases where it was very slightly outside,
		// when b is close to 0
		cosAngle = ::Min( cosAngle, 1.0F );
		cosAngle = ::Max( cosAngle, -1.0F );

		angle = ::acos( cosAngle );
		}

	// Rotate the frame of reference parallel to the x-axis
	R2dTransform rotationTransform;
	rotationTransform.PreRotateAboutPoint( m_TrackingRotationCenter.m_x, m_TrackingRotationCenter.m_y, m_TrackingInitialRotationAngle );
	RRealPoint temp = mousePoint * rotationTransform;

	// If the mouse is below the line through the center of rotation and parallel to the x-axis, we are rotating clockwise
	if( ::AreFloatsGT( temp.m_y, m_TrackingRotationCenter.m_y ) )
		angle *= -1;

	// If necessary, contrain the angle
	angle = ConstrainAngle( angle );

	return angle;
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionTracker::Render( )
//
//  Description:		Called to render the tracker feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateSelectionTracker::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RRealPoint& point ) const
	{
	// Calculate the angle
	YAngle angle = CalcAngle( point );

	// Draw the feedback line
	drawingSurface.MoveTo( m_TrackingRotationCenter, transform );
	drawingSurface.LineTo( point, transform );

	// Render the tracking rectangle
	m_pSelection->DrawRotateTrackingFeedback( drawingSurface, transform, m_TrackingRotationCenter, angle );
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionTracker::GetFeedbackBoundingRect( )
//
//  Description:		Returns the feedback bounding rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RRotateSelectionTracker::GetFeedbackBoundingRect( const RRealPoint& point ) const
	{
	// Get the view transform
	R2dTransform transform;
	m_pView->GetViewTransform( transform, TRUE );

	// Calculate the angle
	YAngle angle = CalcAngle( point );

	// Get the bounding rect, in device units
	RRealRect boundingRect = m_pSelection->GetRotateTrackingFeedbackBoundingRect( transform, m_TrackingRotationCenter, angle );

	// Convert back to logical units
	transform.Invert( );
	boundingRect *= transform;

	// Add the feedback line
	boundingRect.AddPointToRect( point );

	return boundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionTracker::ApplyConstraint( )
//
//  Description:		Applys the rotation constraint if the correct modifier
//							key was down when rotation started. We constrain to 45 deg.
//							angles
//
//  Returns:			The new angle
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAngle RRotateSelectionTracker::ConstrainAngle( YAngle angle ) const
	{
	const YAngle kConstraintAmount = ::DegreesToRadians( 15.0 );

	// Only do if we are constrained
	if( m_fConstrainRotation )
		{
		// Add the amount we have rotated to the inital angle of the controlling object
		YAngle totalAngle = angle + m_ControllingObjectInitialAngle;

		// Compute the minimum the new angle will be
		YAngle minimumAngle = static_cast<YIntDimension>( ( totalAngle / kConstraintAmount ) ) * kConstraintAmount;

		// Compute the amount we are over the minimum
		YAngle delta = totalAngle - minimumAngle;

		// If the delta is greater than half the constrain amount, bump up to the next
		// increment
		if( delta > 0 )
			{
			if( delta > ( kConstraintAmount / 2.0 ) )
				minimumAngle += kConstraintAmount;
			}
		else
			{
			if( -delta > ( kConstraintAmount / 2.0 ) )
				minimumAngle -= kConstraintAmount;
			}

		return minimumAngle - m_ControllingObjectInitialAngle;
		}

	return angle;
	}
	
#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RDragSelectionTracker::Validate( )
//
//  Description:		Validate the Object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateSelectionTracker::Validate( ) const
	{
	RMouseTracker::Validate( );
	TpsAssertIsObject( RRotateSelectionTracker, this );
	TpsAssertValid( m_pView );
	TpsAssertValid( m_pSelection );
	}

#endif	//	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RResizeSelectionTracker::RResizeSelectionTracker( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RResizeSelectionTracker::RResizeSelectionTracker( RCompositeSelection* pSelection )
	: RSelectionTracker( pSelection ),
		m_fFixVertical( FALSE ),
		m_fFixHorizontal( FALSE )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RResizeSelectionTracker::BeginTracking( )
//
//  Description:		Called when tracking begins; ie. when the mouse button goes
//							down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResizeSelectionTracker::BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Remove the selection
	m_pSelection->Remove( FALSE );

	// Get the hit object
	RSingleSelection* pHitObject = m_pSelection->GetHitSingleSelection( mousePoint );
	TpsAssert( pHitObject, "No hit object!" );

	// Get its selection bounding rect
	YSelectionBoundingRect selectionBoundingRect;
	pHitObject->GetSelectionBoundingRect( selectionBoundingRect );

	// Get the bounding rect of the actual object
	YSelectionBoundingRect objectBoundingRect;
	pHitObject->GetObjectBoundingRect( objectBoundingRect );

	// Calculate the minimum scale factor
	RRealSize minimumObjectSize = pHitObject->GetObjectMinimumSize( );
	RRealSize maximumObjectSize = pHitObject->GetObjectMaximumSize( );
	RRealSize currentObjectSize = objectBoundingRect.WidthHeight( );
	m_MinimumScaleFactor.m_dx = minimumObjectSize.m_dx / currentObjectSize.m_dx;
	m_MinimumScaleFactor.m_dy = minimumObjectSize.m_dy / currentObjectSize.m_dy;
	m_MaximumScaleFactor.m_dx = maximumObjectSize.m_dx / currentObjectSize.m_dx;
	m_MaximumScaleFactor.m_dy = maximumObjectSize.m_dy / currentObjectSize.m_dy;

	// Hit test to figure out what kind of resizing we are doing
	RSelection::EHitLocation eHitLocation = m_pSelection->HitTest( mousePoint );

	// Get the center and start of scaling
	RRealPoint selectionScalingCenter;

	switch( eHitLocation )
		{
		case RSelection::kTopLeftResizeHandle :
			selectionScalingCenter = selectionBoundingRect.m_BottomRight;
			m_ScalingCenter = objectBoundingRect.m_BottomRight;
			m_MouseDownPoint = objectBoundingRect.m_TopLeft;
			break;

		case RSelection::kTopRightResizeHandle :
			selectionScalingCenter = selectionBoundingRect.m_BottomLeft;
			m_ScalingCenter = objectBoundingRect.m_BottomLeft;
			m_MouseDownPoint = objectBoundingRect.m_TopRight;
			break;

		case RSelection::kBottomRightResizeHandle :
			selectionScalingCenter = selectionBoundingRect.m_TopLeft;
			m_ScalingCenter = objectBoundingRect.m_TopLeft;
			m_MouseDownPoint = objectBoundingRect.m_BottomRight;
			break;

		case RSelection::kBottomLeftResizeHandle :
			selectionScalingCenter = selectionBoundingRect.m_TopRight;
			m_ScalingCenter = objectBoundingRect.m_TopRight;
			m_MouseDownPoint = objectBoundingRect.m_BottomLeft;
			break;

		case RSelection::kLeftResizeHandle :
			::midpoint(selectionScalingCenter,selectionBoundingRect.m_TopRight,selectionBoundingRect.m_BottomRight);
			::midpoint(m_ScalingCenter,objectBoundingRect.m_TopRight,objectBoundingRect.m_BottomRight);
			::midpoint(m_MouseDownPoint,objectBoundingRect.m_TopLeft,objectBoundingRect.m_BottomLeft);
			m_fFixVertical = TRUE;
			break;

		case RSelection::kTopResizeHandle :
			::midpoint(selectionScalingCenter,selectionBoundingRect.m_BottomLeft,selectionBoundingRect.m_BottomRight);
			::midpoint(m_ScalingCenter,objectBoundingRect.m_BottomLeft,objectBoundingRect.m_BottomRight);
			::midpoint(m_MouseDownPoint,objectBoundingRect.m_TopLeft,objectBoundingRect.m_TopRight);
			m_fFixHorizontal = TRUE;
			break;

		case RSelection::kRightResizeHandle :
			::midpoint(selectionScalingCenter,selectionBoundingRect.m_TopLeft,selectionBoundingRect.m_BottomLeft);
			::midpoint(m_ScalingCenter,objectBoundingRect.m_TopLeft,objectBoundingRect.m_BottomLeft);
			::midpoint(m_MouseDownPoint,objectBoundingRect.m_TopRight,objectBoundingRect.m_BottomRight);
			m_fFixVertical = TRUE;
			break;

		case RSelection::kBottomResizeHandle :
			::midpoint(selectionScalingCenter,selectionBoundingRect.m_TopLeft,selectionBoundingRect.m_TopRight);
			::midpoint(m_ScalingCenter,objectBoundingRect.m_TopLeft,objectBoundingRect.m_TopRight);
			::midpoint(m_MouseDownPoint,objectBoundingRect.m_BottomLeft,objectBoundingRect.m_BottomRight);
			m_fFixHorizontal = TRUE;
			break;

		default :
			TpsAssert( NULL, "Tracker is not resizing." );
		}

	// Save the difference between the size of the selection and object rects
	m_RectDelta = selectionScalingCenter - m_ScalingCenter;

	// Determine if we should maintain aspect ratio or not. Ask the component what its default
	// behavior is, and invert that if the conrol key is down
	m_fMaintainAspectRatio = pHitObject->MaintainAspectRatioIsDefault( );
	if( pHitObject->CanChangeDefaultAspect( ) && ( modifierKeys & kAspectModifier ) )
		m_fMaintainAspectRatio = static_cast<BOOLEAN>( !m_fMaintainAspectRatio );

	//	If we are fixing either Vertical or Horizontal dimension, we can't be 
	//		maintaining aspect ratio
	if ( m_fFixVertical || m_fFixHorizontal )
		m_fMaintainAspectRatio = FALSE;

	// Save the inverse transform of the object
	m_InverseTransform = objectBoundingRect.GetTransform( );
	m_InverseTransform.Invert( );

	// Call the base class
	RSelectionTracker::BeginTracking( mousePoint, modifierKeys );
	}

// ****************************************************************************
//
//  Function Name:	RResizeSelectionTracker::EndTracking( )
//
//  Description:		Called when tracking ends; ie. when the mouse button goes
//							up.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResizeSelectionTracker::EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Constrain the mouse point
	RRealPoint constrainedPoint = mousePoint;
	ApplyConstraint( constrainedPoint );

	// Call the base class
	RSelectionTracker::EndTracking( constrainedPoint, modifierKeys );

	// Compute the final scale factor
	RRealSize scaleFactor = CalcScaleFactor( constrainedPoint );

	// Create and do a resize selection action
	RResizeSelectionAction* pAction = new RResizeSelectionAction( m_pSelection, m_ScalingCenter, scaleFactor, m_fMaintainAspectRatio );
	m_pView->GetRDocument( )->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RResizeSelectionTracker::CalcScaleFactor( )
//
//  Description:		Calculates the scale factor we have resized to given a 
//							mouse point
//
//  Returns:			Scale factor
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RResizeSelectionTracker::CalcScaleFactor( const RRealPoint& point ) const
	{
	// Adjust the mouse point to take into account the difference in the size of selection rect and object rect
	RRealPoint adjustedMousePoint = point + m_RectDelta;

	// Untransform the points. We really dont want to work in a tranformed reference frame
	RRealPoint transformedMousePoint = adjustedMousePoint * m_InverseTransform;
	RRealPoint transformedMouseDownPoint = m_MouseDownPoint * m_InverseTransform;
	RRealPoint transformedScalingCenter = m_ScalingCenter * m_InverseTransform;

	// Calculate a new scale factor
	RRealSize scaleFactor;
	if ( AreFloatsEqual((transformedMouseDownPoint.m_x-transformedScalingCenter.m_x), 0.0) )
		scaleFactor.m_dx = 1.0;
	else
		scaleFactor.m_dx = ( transformedMousePoint.m_x - transformedScalingCenter.m_x ) / ( transformedMouseDownPoint.m_x - transformedScalingCenter.m_x );
	if ( AreFloatsEqual( (transformedMouseDownPoint.m_y-transformedScalingCenter.m_y), 0.0) )
		scaleFactor.m_dy = 1.0;
	else
		scaleFactor.m_dy = ( transformedMousePoint.m_y - transformedScalingCenter.m_y ) / ( transformedMouseDownPoint.m_y - transformedScalingCenter.m_y );

	// If we are preserving aspect ratio, do so here
	if( m_fMaintainAspectRatio )
		{
		if( scaleFactor.m_dx > scaleFactor.m_dy )
			scaleFactor.m_dy = scaleFactor.m_dx;
		else
			scaleFactor.m_dx = scaleFactor.m_dy;

		// Constrain the scale factor
		ConstrainScaleFactor( scaleFactor );
		}
	else
		{
		// Constrain the scale factor
		ConstrainScaleFactor( scaleFactor );

		if ( m_fFixVertical )
			scaleFactor.m_dy = 1.0;
		else if ( m_fFixHorizontal )
			scaleFactor.m_dx = 1.0;
		}

	return scaleFactor;
	}

// ****************************************************************************
//
//  Function Name:	RResizeSelectionTracker::ConstrainScaleFactor( )
//
//  Description:		Constrains the scale factor within a min and a max
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResizeSelectionTracker::ConstrainScaleFactor( RRealSize& scaleFactor ) const
	{
	// Make sure we have not gone past the minimum size
	if( scaleFactor.m_dx < m_MinimumScaleFactor.m_dx )
		scaleFactor.m_dx = m_MinimumScaleFactor.m_dx;
	if( scaleFactor.m_dy < m_MinimumScaleFactor.m_dy )
		scaleFactor.m_dy = m_MinimumScaleFactor.m_dy;

	// Make sure we have not gone past the maximum size
	if( scaleFactor.m_dx > m_MaximumScaleFactor.m_dx )
		scaleFactor.m_dx = m_MaximumScaleFactor.m_dx;
	if( scaleFactor.m_dy > m_MaximumScaleFactor.m_dy )
		scaleFactor.m_dy = m_MaximumScaleFactor.m_dy;
	}

// ****************************************************************************
//
//  Function Name:	RResizeSelectionTracker::Render( )
//
//  Description:		Called to render the tracker feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResizeSelectionTracker::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RRealPoint& point ) const
	{
	RRealSize scaleFactor = CalcScaleFactor( point );

	// Draw the tracking rectangle
	m_pSelection->DrawResizeTrackingFeedback( drawingSurface, transform, m_ScalingCenter, scaleFactor, m_fMaintainAspectRatio );
	}

// ****************************************************************************
//
//  Function Name:	RResizeSelectionTracker::GetFeedbackBoundingRect( )
//
//  Description:		Returns the feedback bounding rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RResizeSelectionTracker::GetFeedbackBoundingRect( const RRealPoint& point ) const
	{
	// Get the view transform
	R2dTransform transform;
	m_pView->GetViewTransform( transform, TRUE );

	RRealSize scaleFactor = CalcScaleFactor( point );

	RRealRect boundingRect = m_pSelection->GetResizeTrackingFeedbackBoundingRect( transform, m_ScalingCenter, scaleFactor, m_fMaintainAspectRatio );

	// Convert back to logical units
	transform.Invert( );
	return boundingRect * transform;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTrackingFeedbackRenderer::RSelectionTrackingFeedbackRenderer( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionTrackingFeedbackRenderer::RSelectionTrackingFeedbackRenderer( RView* pView,
																								RSelectionTracker* pSelectionTracker )
	: RBitmapTrackingFeedbackRenderer( pView ),
	  m_pSelectionTracker( pSelectionTracker )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTrackingFeedbackRenderer::GetFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rect necessary to contain the feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RSelectionTrackingFeedbackRenderer::GetFeedbackBoundingRect( const RRealPoint& point ) const
	{
	return m_pSelectionTracker->GetFeedbackBoundingRect( point );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionTrackingFeedbackRenderer::RenderFeedback( )
//
//  Description:		Renders the feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionTrackingFeedbackRenderer::RenderFeedback( RDrawingSurface& drawingSurface,
																		  const R2dTransform& transform,
																		  const RRealPoint& point ) const
	{
	m_pSelectionTracker->Render( drawingSurface, transform, point );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RResizeSelectionTracker::Validate( )
//
//  Description:		Validate the Object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResizeSelectionTracker::Validate( ) const
	{
	RMouseTracker::Validate( );
	TpsAssertIsObject( RResizeSelectionTracker, this );
	TpsAssertValid( m_pView );
	TpsAssertValid( m_pSelection );
	}

#endif	//	TPSDEBUG


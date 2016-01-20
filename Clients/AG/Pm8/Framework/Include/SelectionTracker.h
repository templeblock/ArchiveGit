// ****************************************************************************
//
//  File Name:			SelectionTracker.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RSelectionTracker class
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
//  $Logfile:: /PM8/Framework/Include/SelectionTracker.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SELECTIONTRACKER_H_
#define		_SELECTIONTRACKER_H_

#include		"MouseTracker.h"
#include		"TrackingFeedbackRenderer.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RView;
class RCompositeSelection;
class RSelectionTrackingFeedbackRenderer;

// ****************************************************************************
//
//  Class Name:	RSelectionTracker
//
//  Description:	Base class for selection trackers
//
// ****************************************************************************
//
class FrameworkLinkage RSelectionTracker : public RMouseTracker
	{
	// Construction & Destruction
	public :
															RSelectionTracker( RCompositeSelection* pSelection );
															~RSelectionTracker( );

	// Operations
	public :
		virtual void									BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void									ContinueTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void									EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void									CancelTracking( );
		virtual void									Render( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;


	// Implementation
	protected :
		virtual void									ApplyConstraint( RRealPoint& mousePoint ) const;
		virtual void									Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RRealPoint& point ) const = 0;
		virtual RRealRect								GetFeedbackBoundingRect( const RRealPoint& point ) const = 0;

	// Members
	protected :
		RCompositeSelection*							m_pSelection;
		RRealPoint										m_TrackingMouseDownPoint;

	// Members
	private :
		RSelectionTrackingFeedbackRenderer*		m_pFeedbackRenderer;

	friend class RSelectionTrackingFeedbackRenderer;
	};

// ****************************************************************************
//
//  Class Name:	RDragSelectionTracker
//
//  Description:	This class is used to encapsulate a drag selection operation
//
// ****************************************************************************
//
class FrameworkLinkage RDragSelectionTracker : public RSelectionTracker
	{
	// Construction & Destruction & Initialization
	public :
												RDragSelectionTracker( RCompositeSelection* pSelection );

	// Operations
	public :
		virtual void						BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void						EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );

	// Implementation
	protected :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RRealPoint& point ) const;
		virtual RRealRect					GetFeedbackBoundingRect( const RRealPoint& point ) const;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RRotateSelectionTracker
//
//  Description:	This class is used to encapsulate a rotate selection operation
//
// ****************************************************************************
//
class FrameworkLinkage RRotateSelectionTracker : public RSelectionTracker
	{
	// Construction & Destruction & Initialization
	public :
												RRotateSelectionTracker( RCompositeSelection* pSelection );

	// Operations
	public :
		virtual void						BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void						EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );

	protected:
		virtual YAngle						ConstrainAngle( YAngle angle ) const;

	// Implementation
	private :
		YAngle								CalcAngle( const RRealPoint& mousePoint ) const;
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RRealPoint& point ) const;
		virtual RRealRect					GetFeedbackBoundingRect( const RRealPoint& point ) const;

	// Member data
	private :
		RRealPoint							m_TrackingRotationCenter;
		RRealPoint							m_TrackingOriginalHandlePoint;
		YAngle								m_TrackingInitialRotationAngle;
		YAngle								m_ControllingObjectInitialAngle;
		BOOLEAN								m_fConstrainRotation;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RResizeSelectionTracker
//
//  Description:	This class is used to encapsulate a drag selection operation
//
// ****************************************************************************
//
class FrameworkLinkage RResizeSelectionTracker : public RSelectionTracker
	{
	// Construction & Destruction & Initialization
	public :
												RResizeSelectionTracker( RCompositeSelection* pSelection );

	// Operations
	public :
		virtual void						BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void						EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );

	// Implementation
	private :
		RRealSize							CalcScaleFactor( const RRealPoint& point ) const;
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RRealPoint& point ) const;
		virtual RRealRect					GetFeedbackBoundingRect( const RRealPoint& point ) const;

	protected:
		virtual void						ConstrainScaleFactor( RRealSize& scaleFactor ) const;

	// Member data
	protected :
		RRealPoint							m_MouseDownPoint;
		RRealPoint							m_ScalingCenter;
		BOOLEAN								m_fFixVertical;
		BOOLEAN								m_fFixHorizontal;
		BOOLEAN								m_fMaintainAspectRatio;
		RRealSize							m_RectDelta;
		RRealSize							m_MinimumScaleFactor;
		RRealSize							m_MaximumScaleFactor;
		R2dTransform						m_InverseTransform;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RSelectionTrackingFeedbackRenderer
//
//  Description:	Class to render selection tracking feedback
//
// ****************************************************************************
//
class RSelectionTrackingFeedbackRenderer : public RBitmapTrackingFeedbackRenderer
	{
	// Construction
	public :
												RSelectionTrackingFeedbackRenderer( RView* pView,
																							   RSelectionTracker* pSelectionTracker );

	// Implementation
	protected :
		virtual RRealRect					GetFeedbackBoundingRect( const RRealPoint& point ) const;

		virtual void						RenderFeedback( RDrawingSurface& drawingSurface,
																	 const R2dTransform& transform,
																	 const RRealPoint& point ) const;

	// Member data
	private :
		RSelectionTracker*				m_pSelectionTracker;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _SELECTIONTRACKER_H_

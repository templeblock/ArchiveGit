// ****************************************************************************
//
//  File Name:			SelectionRectTracker.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RSelectionRectTracker class
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
//  $Logfile:: /PM8/Framework/Include/SelectionRectTracker.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SELECTIONRECTTRACKER_H_
#define		_SELECTIONRECTTRACKER_H_

#include	"MouseTracker.h"
#include "TrackingFeedbackRenderer.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RSelectionRectTrackingFeedbackRenderer;

// ****************************************************************************
//
//  Class Name:		RSelectionRectTracker
//
//  Description:		A mouse tracker which draws the selection 
//
// ****************************************************************************
//
class FrameworkLinkage RSelectionRectTracker : public RMouseTracker
	{
	// Construction & Destruction
	public :
																RSelectionRectTracker( RView* pView );
																~RSelectionRectTracker( );
																
	// Operations											
	public :													
		virtual void										BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void										ContinueTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void										EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void										Render( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;
																
	// Implementation										
	private :												
		RRealRect											GetSelectionRect( const RRealPoint& mousePoint ) const;
																
	// Member data											
	private :												
		RRealPoint											m_SelectionAnchorPoint;
		RRealPoint											m_LastPoint;
		RRealRect											m_SelectionRect;
		YModifierKey										m_MouseDownModifierKeys;
		RSelectionRectTrackingFeedbackRenderer*	m_pFeedbackRenderer;

	friend class RSelectionRectTrackingFeedbackRenderer;

#ifdef	TPSDEBUG
	//	Debug Code
	public :
		virtual	void						Validate( ) const;
#endif	//	TPSDEBUG
	};

// ****************************************************************************
//
//  Class Name:	RSelectionTrackingFeedbackRenderer
//
//  Description:	Class to render selection tracking feedback
//
// ****************************************************************************
//
class RSelectionRectTrackingFeedbackRenderer : public RBitmapTrackingFeedbackRenderer
	{
	// Construction
	public :
												RSelectionRectTrackingFeedbackRenderer( RSelectionRectTracker* pSelectionRectTracker,
																									 RView* pView );

	// Implementation
	protected :
		virtual RRealRect					GetFeedbackBoundingRect( const RRealPoint& point ) const;

		virtual void						RenderFeedback( RDrawingSurface& drawingSurface,
																	 const R2dTransform& transform,
																	 const RRealPoint& point ) const;

	// Member data
	private :
		RSelectionRectTracker*			m_pSelectionRectTracker;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _SELECTIONRECTTRACKER_H_

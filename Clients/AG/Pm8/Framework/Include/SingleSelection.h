// ****************************************************************************
//
//  File Name:			SingleSelection.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RSingleSelection class
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
//  $Logfile:: /PM8/Framework/Include/SingleSelection.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SINGLESELECTION_H_
#define		_SINGLESELECTION_H_

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifndef		_COMPONENTVIEW_H_
#include		"ComponentView.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RSingleSelection
//
//  Description:	Represents a single selected object. It is currently designed
//						for component views, but could easily be modified to support
//						other objects, either using templates, or with a base class.
//
// ****************************************************************************
//
class FrameworkLinkage RSingleSelection : public RSelection
	{
	// Construction, destruction & Initialization
	public :
												RSingleSelection( const RComponentView* pSelectedObject, RView* pView );
		virtual								~RSingleSelection( );
		virtual RSelection*				Clone( );

	// Operations
	public :
		virtual void						GetSelectionBoundingRect( YSelectionBoundingRect& boundingRect ) const;
		void									GetObjectBoundingRect( YComponentBoundingRect& boundingRect ) const;
		RRealSize							GetSelectionMinimumSize( ) const;
		RRealSize							GetObjectMinimumSize( ) const;
		RRealSize							GetObjectMaximumSize( ) const;
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, BOOLEAN fRenderIfLocked = TRUE ) const;
		virtual void						Invalidate( BOOLEAN fInvalidateIfLocked = TRUE ) const;
		virtual BOOLEAN					IsSelected( const RComponentView* pSelectableObject );
		virtual EHitLocation				HitTest( const RRealPoint& point ) const;
		BOOLEAN								MaintainAspectRatioIsDefault( ) const;
		BOOLEAN								CanChangeDefaultAspect( ) const;
		BOOLEAN								IsHigherZOrder( const RSingleSelection& selection ) const;
		BOOLEAN								IsHigherZOrder( YComponentZPosition zPosition ) const;

		virtual void						DrawDragTrackingFeedback( RDrawingSurface& drawingSurface,
																				  const R2dTransform& transform,
																				  const RRealSize& offset ) const;

		virtual void						DrawRotateTrackingFeedback( RDrawingSurface& drawingSurface,
																					 const R2dTransform& transform,
																					 const RRealPoint& rotationCenter,
																					 YAngle rotationAngle ) const;

		virtual void						DrawResizeTrackingFeedback( RDrawingSurface& drawingSurface,
																					 const R2dTransform& transform,
																					 const RRealPoint& scalingCenter,
																					 const RRealSize& scaleFactor,
																					 BOOLEAN fMaintainAspectRatio ) const;

		virtual RRealRect					GetDragTrackingFeedbackBoundingRect( const R2dTransform& transform,
																								 const RRealSize& offset ) const;

		virtual RRealRect					GetRotateTrackingFeedbackBoundingRect( const R2dTransform& transform,
																									const RRealPoint& rotationCenter,
																									YAngle rotationAngle ) const;

		virtual RRealRect					GetResizeTrackingFeedbackBoundingRect( const R2dTransform& transform,
																									const RRealPoint& scalingCenter,
																									const RRealSize& scaleFactor,
																									BOOLEAN fMaintainAspectRatio ) const;

		static void							SelectionRectFromObjectRect( YComponentBoundingRect& objectBoundingRect, const RView* pView, BOOLEAN fOutset );
		static void							SelectionRectFromDeviceObjectRect( YComponentBoundingRect& objectBoundingRect, BOOLEAN fOutset );
		static RRealRect					GetBoundingRectIncludingHandles( const RComponentView* pCompnentView );

		RRealPoint							GetRotateHandleCenterPoint( ) const;

	// Implementation
	protected :
		void									GetResizeSelectionHandle( RRealRect& rHandle, const R2dTransform& transform, EHitLocation eHit, BOOLEAN fRender ) const;
		void									GetRotateHandle( RRealVectorRect& rHandle ) const;
		void									DrawRotateHandle( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;

	// Member data
	private :
		const RComponentView*			m_pSelectedObject;

		friend class RCompositeSelection::RSelectionIterator;

#ifdef	TPSDEBUG
	// Debugging
	public :
		virtual void						Validate( ) const;
#endif
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _SINGLESELECTION_H_

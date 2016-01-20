// ****************************************************************************
//
//  File Name:			Selection.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RSelection class
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
//  $Logfile:: /PM8/Framework/Include/Selection.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SELECTION_H_
#define		_SELECTION_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RView;
class RComponentView;
class RDrawingSurface;

typedef RRealVectorRect YSelectionBoundingRect;

// ****************************************************************************
//
//  Class Name:	RSelection
//
//  Description:	Base selection class.
//
// ****************************************************************************
//
class FrameworkLinkage RSelection : public RObject
	{
	// Construction
	public :
												RSelection( RView* pView );
		virtual RSelection*				Clone( ) = 0;

	// Operations
	public :
		enum EHitLocation { kNothing, kInside, kRotateHandle, 
							kLeftResizeHandle, kTopResizeHandle, kRightResizeHandle, kBottomResizeHandle,
							kTopLeftResizeHandle, kTopRightResizeHandle, kBottomRightResizeHandle, kBottomLeftResizeHandle };
		
		virtual void						GetSelectionBoundingRect( YSelectionBoundingRect& boundingRect ) const = 0;
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, BOOLEAN fRenderIfLocked = TRUE ) const = 0;
		virtual void						Invalidate( BOOLEAN fInvalidateIfLocked = TRUE ) const = 0;
		virtual BOOLEAN					IsSelected( const RComponentView* pSelectableObject ) = 0;

		virtual EHitLocation				HitTest( const RRealPoint& point ) const = 0;

		virtual void						DrawDragTrackingFeedback( RDrawingSurface& drawingSurface,
																				  const R2dTransform& transform,
																				  const RRealSize& offset ) const = 0;

		virtual void						DrawRotateTrackingFeedback( RDrawingSurface& drawingSurface,
																					 const R2dTransform& transform,
																					 const RRealPoint& rotationCenter,
																					 YAngle rotationAngle ) const = 0;

		virtual void						DrawResizeTrackingFeedback( RDrawingSurface& drawingSurface,
																					 const R2dTransform& transform,
																					 const RRealPoint& scalingCenter,
																					 const RRealSize& scaleFactor,
																					 BOOLEAN fMaintainAspectRatio ) const = 0;

		virtual RRealRect					GetDragTrackingFeedbackBoundingRect( const R2dTransform& transform,
																								 const RRealSize& offset ) const = 0;

		virtual RRealRect					GetRotateTrackingFeedbackBoundingRect( const R2dTransform& transform,
																									const RRealPoint& rotationCenter,
																									YAngle rotationAngle ) const = 0;

		virtual RRealRect					GetResizeTrackingFeedbackBoundingRect( const R2dTransform& transform,
																									const RRealPoint& scalingCenter,
																									const RRealSize& scaleFactor,
																									BOOLEAN fMaintainAspectRatio ) const = 0;

		RView*								GetView( ) const;
	

	// Member data
	protected :
		RView*								m_pView;

#ifdef	TPSDEBUG
	// Debugging
	public :
		virtual void						Validate( )  const = 0;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RSelection::GetView( )
//
//  Description:		Accessor
//
//  Returns:			The view to which this selection belongs
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RView* RSelection::GetView( ) const
	{
	return m_pView;
	}


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _SELECTION_H_

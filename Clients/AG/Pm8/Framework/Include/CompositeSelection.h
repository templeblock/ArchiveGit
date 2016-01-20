// ****************************************************************************
//
//  File Name:			CompositeSelection.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RCompositeSelection class
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
//  $Logfile:: /PM8/Framework/Include/CompositeSelection.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMPOSITESELECTION_H_
#define		_COMPOSITESELECTION_H_

#ifndef		_SELECTION_H_
#include		"Selection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef RList<RSelection*> YSelectionCollection;
typedef YSelectionCollection::YIterator YSelectionCollectionIterator;

class RSingleSelection;
class RMouseTracker;
class RDataTransferTarget;
class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RCompositeSelection
//
//  Description:	Represents a multiple selected objects
//
// ****************************************************************************
//
class FrameworkLinkage RCompositeSelection : public RSelection
	{
	// Construction, destruction & Initialization
	public :
												RCompositeSelection( RView* pView );
												RCompositeSelection( const RCompositeSelection& rhs );
		virtual								~RCompositeSelection( );
		virtual RSelection*				Clone( );

	// Operations
	public :
		RRealRect							GetBoundingRect( ) const;
		virtual void						GetSelectionBoundingRect( YSelectionBoundingRect& boundingRect ) const;
		RRealRect							GetBoundingRectIncludingHandles( ) const;
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, BOOLEAN fRenderIfLocked = TRUE ) const;
		void									Render( BOOLEAN fRenderIfLocked = TRUE ) const;
		void									Remove( BOOLEAN fRenderIfLocked = TRUE ) const;
		virtual void						Invalidate( BOOLEAN fInvalidateIfLocked = TRUE ) const;
		virtual BOOLEAN					IsSelected( const RComponentView* pSelectableObject );
		virtual EHitLocation				HitTest( const RRealPoint& point ) const;
		RSingleSelection*					GetHitSingleSelection( const RRealPoint& point ) const;
		BOOLEAN								IsEmpty( ) const;
		BOOLEAN								SetCursor( const RRealPoint& mousePoint ) const;
		void									UnselectAll( BOOLEAN fRender = TRUE );
		void									Select( const RComponentView* pSelectableObject, BOOLEAN fRender );
		void									Select( const RComponentDocument* pDocument, BOOLEAN fRender );
		void									Unselect( const RComponentView* pSelectableObject, BOOLEAN fRender = TRUE );
		void									Unselect( RSingleSelection* pSelection );
		void									ToggleSelect( const RComponentView* pSelectableObject );
		void									Read( RArchive& archive );
		void									Write( RArchive& archive ) const;
		void									SortByZOrder( );
		RMouseTracker*						GetSelectionTracker( const RRealPoint& mouseDownPoint );
		void									Hide( );

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

		RCompositeSelection&				operator=( const RCompositeSelection& rhs );
		YCounter								Count( ) const;

	// Iteration
	public :
		class FrameworkLinkage RSelectionIterator
			{
			// Construction
			public :
														RSelectionIterator( YSelectionCollectionIterator iterator );

			// Operations
			public :
				RComponentView*					operator*( ) const;
				RSelectionIterator&				operator++( );
				RSelectionIterator&				operator--( );
				BOOLEAN								operator!=( const RSelectionIterator& rhs ) const;

			// Member data
			private :
				YSelectionCollectionIterator	m_Iterator;
			};

		typedef RSelectionIterator YIterator;
		YIterator							Start( ) const;
		YIterator							End( ) const;

	// Implementation
	private :
		void									DeleteSelectionCollection( );
		void									SetupRender( RDrawingSurface& drawingSurface, R2dTransform& transform ) const;
		void									DoRender( BOOLEAN fRenderIfLocked ) const;
		void									SetResizeCursor( RSingleSelection* pHitObject, EHitLocation eHitLocation ) const;
		BOOLEAN								ShouldRenderSelection( ) const;

	// Member data
	private :
		YSelectionCollection				m_SelectionCollection;
		BOOLEAN								m_fSelectionVisible;
		BOOLEAN								m_fSelectionHidden;

#ifdef	TPSDEBUG
	// Debugging
	public :
		virtual void						Validate( ) const;
#endif
	};

typedef RCompositeSelection::YIterator		YSelectionIterator;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::IsEmpty( )
//
//  Description:		Determines if there are any selected objects
//
//  Returns:			TRUE if the selection is empty.
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RCompositeSelection::IsEmpty( ) const
	{
	return static_cast<BOOLEAN>( m_SelectionCollection.Count( ) == 0 );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Count( )
//
//  Description:		Returns the number of objects selected by this composite
//
//  Returns:			count
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YCounter RCompositeSelection::Count( ) const
	{
	return m_SelectionCollection.Count( );
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _COMPOSITESELECTION_H_

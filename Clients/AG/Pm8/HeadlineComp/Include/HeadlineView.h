// ****************************************************************************
//
//  File Name:			HeadlineView.h
//
//  Project:			Headline Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RHeadlineView class
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
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlineView.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HEADLINEVIEW_H_
#define		_HEADLINEVIEW_H_

#include "ComponentView.h"
#include "HeadlineDocument.h"

// ****************************************************************************
//
//  Class Name:	RHeadlineView
//
//  Description:	The Headline view class. This class handles various
//						xEvents, and dispatches various xActions.
//
// ****************************************************************************
//
class RHeadlineView : public RComponentView
	{
	// Construction & Destruction
	public :
												RHeadlineView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView );

	// xEvent Handlers
	protected :
		virtual void						OnXEditComponent( EActivationMethod activationMethod, const RRealPoint& mousePoint );

	//	Operations
	public :
		virtual void						Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio );
		virtual void						SetFrame( EFrameType eNewFrame );

	// Functions called on selectable objects
	public :
		virtual void						RenderInternalTrackingFeedback( RDrawingSurface& drawingSurface,
																				        const R2dTransform& transform,
																				        const RView& targetView ) const;

	//	Accessors
	public :
		virtual void						CalculateOutlinePath( );
		RHeadlineDocument*				GetHeadlineDocument( ) const;
		virtual BOOLEAN					HasSoftEffects( ) const;

	//	Functions for supporting extensions to the API through components
	public :
	//	virtual BOOLEAN					GetInterfaceId( YCommandID commandId, YInterfaceId& interfaceId ) const;
		virtual RInterface*				GetInterface( YInterfaceId id ) const;

	// Operations
	protected :
		virtual void						SetBoundingRect( const YComponentBoundingRect& boundingRect );
		virtual BOOLEAN					MaintainAspectRatioIsDefault( ) const;
		virtual BOOLEAN					WantsGlowAndShadowInsideBounds() const;
		virtual YRealDimension			SoftEffectDimension( const RRealSize& size ) const;
		virtual BOOLEAN					HasOldStyleShadow() const { return FALSE; }

	// Member data
	private :
		RMBCString							m_HeadlineString;

// Debugging stuff
#ifdef	TPSDEBUG
	public :
		virtual void						Validate( ) const;
#endif
	} ;

// ****************************************************************************
//					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RHeadlineView::GetHeadlineDocument( )
//
//  Description:		Return the document associated with this view 
//							(casted to a RHeadlineDocument)
//
//  Returns:			(RHeadlineDocument*)GetRDocument( );
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RHeadlineDocument* RHeadlineView::GetHeadlineDocument( ) const
	{
	TpsAssertIsObject( RHeadlineDocument, GetRDocument( ));
	return static_cast<RHeadlineDocument*> (GetRDocument( ) );
	}

#endif	//	_HEADLINEVIEW_H_

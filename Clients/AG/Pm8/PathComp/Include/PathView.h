// ****************************************************************************
//
//  File Name:			PathView.h
//
//  Project:			Path Component
//
//  Author:				Lance Wilson
//
//  Description:		Declaration of the RPathDocument class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software.
//							500 Redwood Blvd
//							Novato, CA 94948
//							(415) 482-4400
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/PathComp/Include/PathView.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PathView_H_
#define		_PathView_H_

#include "ComponentView.h"

// ****************************************************************************
//
//  Class Name:	RPathView
//
//  Description:	The Group view class. This class handles various xEvents,
//						and dispatches various xActions.
//
// ****************************************************************************
//
class RPathView : public RComponentView
{
// Construction & Destruction
public :
									RPathView( 
										const YComponentBoundingRect& boundingRect,
										RComponentDocument* pDocument,
										RView* pParentView );

// Operations
protected :
	virtual void				OnXEditComponent( EActivationMethod activationMethod, const RRealPoint& mousePoint );
	virtual RInterface*		GetInterface( YInterfaceId id ) const;

	//	Functions for supporting extensions to the API through components
public :
	virtual BOOLEAN			EnableRenderCacheByDefault( ) const;
	virtual void				SetClipRegion( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;

// Functions called on selectable objects
public :
	virtual void				RenderInternalTrackingFeedback( 
										RDrawingSurface& drawingSurface,
										const R2dTransform& transform,
										const RView& targetView ) const;

private:

	uLONG				m_nRefCnt;

// Debugging stuff
#ifdef	TPSDEBUG
	public :
		virtual void			Validate( ) const;
#endif
} ;

#endif	//	_PathView_H_

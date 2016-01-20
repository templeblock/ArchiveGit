// ****************************************************************************
//
//  File Name:			GraphicView.h
//
//  Project:			Graphic Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGraphicView class
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
//  $Logfile:: /PM8/GraphicComp/Include/GraphicView.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GRAPHICVIEW_H_
#define		_GRAPHICVIEW_H_

#include "ComponentView.h"

class RGraphicDocument;

// ****************************************************************************
//
//  Class Name:	RGraphicView
//
//  Description:	The Graphic view class. This class handles various
//						xEvents, and dispatches various xActions.
//
// ****************************************************************************
//
class RGraphicView : public RComponentView
	{
	// Construction & Destruction
	public :
												RGraphicView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView );

	// xEvent Handlers
	protected :
		virtual void						OnXDisplayChange( );

	// Operations
	public :
		virtual void						OnXEditComponent( EActivationMethod activationMethod, const RRealPoint& mousePoint );
		RGraphicDocument*					GetGraphicDocument( ) const;
		virtual BOOLEAN					HitTest( const RRealPoint& point ) const;
		virtual BOOLEAN					GetColor( RColor& color ) const;
		virtual void						ApplyColor( const RColor& color );
		virtual BOOLEAN					GetInterfaceId( YCommandID commandId, YInterfaceId& interfaceId ) const;
		virtual RInterface*				GetInterface( YInterfaceId id ) const;

	//	Accessors
	public :
		virtual void						CalculateOutlinePath( );

// Debugging stuff
#ifdef	TPSDEBUG
	public :
		virtual void						Validate( ) const;
#endif
	} ;

// ****************************************************************************
//
//  Class Name:	RBackdropView
//
//  Description:	View class for banner backdrop graphics
//
// ****************************************************************************
//
class RBackdropView : public RGraphicView
	{
	// Construction & Destruction
	public :
												RBackdropView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView );

	// Operations
	public :
		virtual void						Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio );
	};

#endif	//	_GRAPHICVIEW_H_

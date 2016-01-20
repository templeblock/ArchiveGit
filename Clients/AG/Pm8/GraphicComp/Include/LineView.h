// ****************************************************************************
//
//  File Name:			LineView.h
//
//  Project:			Line Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RLineView class
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
//  $Logfile:: /PM8/GraphicComp/Include/LineView.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_LINEVIEW_H_
#define		_LINEVIEW_H_

#include "GraphicView.h"

// ****************************************************************************
//
//  Class Name:	RLineView
//
//  Description:	The Line view class. This class handles various
//						xEvents, and dispatches various xActions.
//
// ****************************************************************************
//
class RLineView : public RGraphicView
	{
	// Construction & Destruction
	public :
												RLineView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView );

	// Operations
	protected :
		virtual RRealSize					ApplyResizeConstraint( const RRealSize& scaleFactor ) const;
		virtual BOOLEAN					MaintainAspectRatioIsDefault( ) const;
		virtual BOOLEAN					CanChangeDefaultAspect( ) const;
		virtual BOOLEAN					CanResizeHorizontal( ) const;
		virtual BOOLEAN					CanResizeVertical( ) const;
		virtual void						Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio );
		virtual void						AlternateScale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio );
		virtual BOOLEAN					EnableRenderCacheByDefault( ) const;
	} ;

#endif	//	_LINEVIEW_H_

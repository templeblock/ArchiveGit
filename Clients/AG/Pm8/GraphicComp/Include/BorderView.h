// ****************************************************************************
//
//  File Name:			BorderView.h
//
//  Project:			Border Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RBorderView class
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
//  $Logfile:: /PM8/GraphicComp/Include/BorderView.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_BORDERVIEW_H_
#define		_BORDERVIEW_H_

#include "GraphicView.h"

// ****************************************************************************
//
//  Class Name:	RBorderView
//
//  Description:	The Border view class. This class handles various
//						xEvents, and dispatches various xActions.
//
// ****************************************************************************
//
class RBorderView : public RGraphicView
{
// Construction & Destruction
public :
											RBorderView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView );

// Overrides
public :		
	virtual void						Read( RChunkStorage& storage );
	virtual void						Write( RChunkStorage& storage ) const;
	
	virtual void						Offset( const RRealSize& offset, BOOLEAN fInvalidate = TRUE );
	virtual void						Rotate( const RRealPoint& centerOfRotation, YAngle angle );
	virtual void						Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio );
	virtual void						AlternateScale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio );
	void									Resize( const RRealSize& newSize );
	void									FlipHorizontal( YRealCoordinate yAxis );
	void									FlipVertical( YRealCoordinate xAxis );
	
	virtual void						DrawSelectionFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;
	virtual BOOLEAN					HitTest( const RRealPoint& point ) const;
	virtual BOOLEAN					HitTestSelectionFrame( const RRealPoint& point ) const;
	virtual void						RenderInternalTrackingFeedback( RDrawingSurface& drawingSurface,
																			        const R2dTransform& transform,
																			        const RView& targetView ) const;

	virtual RRealSize					GetMinimumSize( ) const;
	virtual RRealSize					GetMaximumSize( ) const;
	virtual BOOLEAN					MaintainAspectRatioIsDefault( ) const;
	
	virtual void						SetBoundingRect( const YComponentBoundingRect& boundingRect );
	const YComponentBoundingRect&	GetInteriorBoundingRect() const {return m_rInteriorBoundingRect;}
	virtual BOOLEAN					IntersectsRect( const RRealRect& targetRect ) const;		
	
	virtual RInterface*				GetInterface( YInterfaceId id ) const;		
	
	virtual void						SetWideBorder(BOOLEAN fWideBorder);

// Implementation
protected :
	virtual void						CalculateBorderPath( RPath* pPath );		
	virtual void						UpdateInteriorBoundingRect();
	virtual BOOLEAN					EnableRenderCacheByDefault( ) const;

// Data members
protected:
	BOOLEAN								m_fWideBorder;
	YComponentBoundingRect			m_rInteriorBoundingRect;	
} ;

class RMiniBorderView : public RBorderView
{
public:
											RMiniBorderView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView );

	virtual RRealSize					GetMinimumSize( ) const;
	virtual RRealSize					GetMaximumSize( ) const;
	
	virtual void						SetWideBorder(BOOLEAN fWideBorder);

protected:
	virtual void						UpdateInteriorBoundingRect();
};

#endif	//	_BORDERVIEW_H_

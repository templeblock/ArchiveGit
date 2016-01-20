//
//  File Name:		EditCropControl.h
//
//  Project:		Renaissance Application Component
//
//  Author:			Lance Wilson
//
//  Description:	Declaration of the REditCropView and
//                REditCropDocument classes
//
//	Portability:	Win32
//
//	Developed by:	Broderbund Software
//					   500 Redwood Blvd
//					   Novato, CA 94948
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

#ifndef _EDITCROPCONTROL_H_
#define _EDITCROPCONTROL_H_

#include "ScrollableControl.h"
#include "TrackingFeedbackRenderer.h"
#include "Path.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class RTabbedEditCrop;

class REditCropView : public RScrollControlView
{
// Construction
public:

	enum EMode { kDefault, kCollectPoints, kTracking, kEditingPoints, kTrackingPoint };


									REditCropView( CDialog* pDialog, int nControlId, RControlDocument* pDocument );

// Operations
public :
	virtual RRealPoint			ConvertPointToLocalCoordinateSystem( const RRealPoint& point ) const;
	virtual RRealRect				CalcBoundingRectOfEmbeddedComponents( BOOLEAN fIncludeSelectionHandles ) const;
	virtual void					Render( RDrawingSurface& ds, const R2dTransform& transform, const RIntRect& rcRender ) const;
	virtual void					RenderBackground( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;

	virtual RSelectionTracker*	GetResizeTracker( ) const;
	virtual RSelectionTracker*	GetRotateTracker( ) const;

	void								RemovePoints();
	void								SetPoint( int nIndex, const RRealPoint& pt );
	void								SetPoints( const RRealPointArray& ptArray );
	RRealPoint&						PointFromIndex( int nIndex );

	BOOLEAN							IsImageTinted() { return m_fTintedImage; }
	BOOLEAN							PointsToPath( RPath& rPath, BOOLEAN fDeletePoints = TRUE );
	void								SetMode( EMode eMode );

	void								SetImageComponent(  );
	void								SetImageInfo( RComponentView* pView, RBitmapImage* pImage );
	void								DetermineComponentBoundingRect( BOOLEAN fResize = FALSE );

	void								SetScaleFactor( YScaleFactor scale )
										{ m_yScaleFactor = scale; }

	YScaleFactor					GetScaleFactor( )
										{ return m_yScaleFactor; }

	BOOLEAN							HasPoints() { return m_ptArray.Count() > 0; }
	BOOLEAN							IsCropped();

// Implmentation
protected:

	int								GetHitPoint( const RRealPoint& pt ) const;
	void								RenderPoints( RDrawingSurface& ds, R2dTransform transform, int nStartIndex = 0, BOOLEAN fClose = TRUE ) const;

// xEvent dispatchers						
public :											
	virtual void					OnXSetCursor( const RRealPoint& mousePoint, YModifierKey modifierKeys );
	virtual void 					OnXLButtonUp( const RRealPoint& mousePoint, YModifierKey modifierKeys );
	virtual void 					OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys );
	virtual void 					OnXMouseMove( const RRealPoint& mousePoint, YModifierKey modifierKeys );

//	virtual void					OnXCharacter( const RCharacter& character, YModifierKey modifierKeys );

	// Implementation
protected :
	virtual void					DoDragDrop( const RRealPoint& mousePoint, BOOLEAN fUseTargetDefaultSize, YDropEffect );
	virtual YDropEffect			OnXDragEnter( RDataTransferSource& dataSource, YModifierKey modifierKeys, const RRealPoint& point );

private:

	typedef	RArray<RRealRect>				RRealRectArray;
	typedef	RRealRectArray::YIterator	RRealRectArrayIterator;

	RRealPointArray	m_ptArray;
	RRealRectArray		m_rectArray;
	EMode					m_eMode;

	RRealPoint			m_ptLast;
	RRealRect			m_rcBoundingRect;

	RComponentView*   m_pImageView;
	mutable RBitmapImage* m_pImage;

	YScaleFactor		m_yScaleFactor;
	int					m_nTrackingIndex;
	RRealPoint			m_ptTrackedPoint;

	mutable BOOLEAN	m_fTintedImage;

	friend class REditModeAction;
};

class REditCropDocument : public RScrollControlDocument
{
public:
										REditCropDocument( RTabbedEditCrop *pParentDlg );

	virtual RControlView*		CreateView( CDialog* pDialog, int nControlId );
	RComponentDocument*			GetDocumentOfType( YComponentType strType );

	virtual BOOLEAN				SendAction( RAction* pAction );
	void								FreeAllActions();
	void								Undo( );

private:

	CWnd*								m_pButtonUndo;
	RTabbedEditCrop*				m_pParentDlg;
};

#endif // _EDITCROPCONTROL_H_

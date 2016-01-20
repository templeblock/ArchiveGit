// ****************************************************************************
//
//  File Name:		ScrollableControl.h
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Declaration of the RScrollableControlView class
//
//	Portability:	Win32
//
//	Developed by:	Broderbund Software
//					500 Redwood Blvd
//					Novato, CA 94948
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

#ifndef		_SCROLLCONTROL_H_
#define		_SCROLLCONTROL_H_

#include "ControlContainer.h"
#include "ComponentDocument.h"
#include "ComponentView.h"

// ****************************************************************************
//
//  Class Name:	RScrollControlView
//
//  Description:	Preview control which can scroll and zoom images.
//
// ****************************************************************************
class RScrollControlView : public RControlView
	{
	public:
										RScrollControlView( CDialog* pDialog, int nControlId, RControlDocument* pDocument );

	// Operations
	public :
		virtual void				ApplyTransform( 
											R2dTransform& transform, 
											BOOLEAN fApplyParentsTransform, 
											BOOLEAN fIsPrinting ) const;

		virtual BOOLEAN			SubclassControl( CDialog* pDialog, int nControlId );

		virtual RRealRect			CalcBoundingRectOfEmbeddedComponents( BOOLEAN fIncludeSelectionHandles ) const;
		virtual void				SetZoomLevel( YZoomLevel zoomLevel, BOOLEAN fUpdateScrollBars = TRUE );
		virtual void				UpdateScrollBars( EScrollUpdateReason );

		void							ZoomIn( );
		void							ZoomOut( );

		BOOLEAN						IsMaxZoomLevel() { return m_bMaxZoomLevel; }
		BOOLEAN						IsMinZoomLevel() { return m_bMinZoomLevel; }

	// Accessors
	public :
		virtual RRealSize			GetDPI( ) const;
		virtual RRealPoint		ConvertPointToLocalCoordinateSystem( const RRealPoint& point ) const;
		virtual RRealRect			GetViewableArea( ) const;
		YZoomLevel					GetZoomLevel( ) const;

	// xEvent dispatchers						
	public :											
		virtual void 				OnXLButtonUp( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void 				OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void				OnXCharacter( const RCharacter& character, YModifierKey modifierKeys );

	// Member data
	protected :

		typedef RArray<YZoomLevel>		   RZoomLevelMap;
		typedef RZoomLevelMap::YIterator RZoomLevelIterator;

		BOOLEAN						m_fIsDragging;

		RRealRect					m_rUseableArea;
		RZoomLevelMap				m_ZoomLevelMap;
		YZoomLevel					m_ZoomLevel;

		BOOLEAN						m_bMaxZoomLevel;
		BOOLEAN						m_bMinZoomLevel;
	};

inline RRealRect RScrollControlView::CalcBoundingRectOfEmbeddedComponents( BOOLEAN fIncludeSelectionHandles ) const
{
	return RControlView::GetBoundingRectOfEmbeddedComponents( fIncludeSelectionHandles );
}

class RScrollControlDocument : public RControlDocument
	{
	public:

		virtual RControlView*	CreateView( CDialog* pDialog, int nControlId );

		YCounter						UndoCount();
	};

#endif	// _SCROLLCONTROL_H_

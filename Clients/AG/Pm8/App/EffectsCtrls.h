//****************************************************************************
//
// File Name:		EffectsCtrls.h
//
// Project:			Renaissance Application Component
//
// Author:			Lisa Wu
//
// Description:		Declarations for custom controls related to image effects 
//					typed dialog
//
// Portability:		Win32
//
// Developed by:	Broderbund Software
//						500 Redwood Blvd
//						Novato, CA 94948
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
//****************************************************************************

#ifndef _EFFECTSCTRLS_H_
#define _EFFECTSCTRLS_H_ 

#if _MSC_VER >= 1000
#pragma once
#endif 

#include "WinGridCtrl.h"
#include "BitmapImage.h"
#include "ControlContainer.h"
#include "SoftShadowSettings.h"
#include "SoftGlowSettings.h"
#include "SelectionTracker.h"
#include "SearchResult.h"
#include "ImageColor.h"

/////////////////////////////////////////////////////////////////////////////
//						Preview control
//
// general class that displays a 256 bitmap using the
// RBitmapImage class

class RBmpCtrl : public CStatic
{
public:
						RBmpCtrl() ;
	virtual				~RBmpCtrl() ;

	BOOL				SetResourceBmp( int resID );

protected:
	//{{AFX_MSG(RBmpCtrl)
	afx_msg void			OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	RBitmapImage		*m_pRBitmap;		// the bitmap
};

/////////////////////////////////////////////////////////////////////////////
// Preset Grid control

class RPresetGridCtrl : public RGridCtrl<HBITMAP>
{
public:
						RPresetGridCtrl() ;
	virtual				~RPresetGridCtrl() ;

	void				Unload( );
	BOOLEAN				LoadBitmaps( int nBeginID, int nEndID ) ;
	void				SetData(int nBeginID, int nEndID);
	virtual COLORREF	GetGridBackgroundColor() const;

protected:	// Generated message map functions

	virtual void		DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) ;
	virtual void		MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) ;

	//{{AFX_MSG(RPresetGridCtrl)
	//}}AFX_MSG

//	DECLARE_MESSAGE_MAP()

private:
	BOOL				m_bHasNames;
	CStringArray		m_nameStrArray;
	YFont				m_font;
	
} ;

// ****************************************************************************
//
//  Class Name:		RShadowGlowControlDoc
//
//  Description:	Control document class for shadow glow custom control
//
// ****************************************************************************
//
class RShadowGlowControlDoc : public RControlDocument 
{
	// Construction
	public :
										RShadowGlowControlDoc();
		virtual							~RShadowGlowControlDoc();

	// Operations
	public :
		virtual RRealSize				GetMinObjectSize( const YComponentType&, BOOLEAN fMaintainAspect = FALSE );
		virtual RControlView*			CreateView( CDialog* pDialog, int nControlId, int theType );
				void					ShadowSettingsChanged(const RSoftShadowSettings& theSetting);

	// Members
	public :
};

// ****************************************************************************
//
//  Class Name:		RShadowGlowControlView
//
//  Description:	Control view class for the shadow glow custom control
//
// ****************************************************************************
//
enum { kAllPurposeType, kShadowType, kGlowType } ;

class RShadowGlowControlView : public RControlView
{
	// Construction
	public :
										RShadowGlowControlView( CDialog* pDialog, int nControlId, RControlDocument* pDocument, int type );
		virtual							~RShadowGlowControlView();

		void							EnableMouseEvents( BOOLEAN enableMouse);

	private:

		virtual void					Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		virtual void 					OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void					OnXLButtonUp( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void					OnXMouseMove( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void					OnXRButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys );

				YAngle					CalcShadowAngle( const RRealPoint& pt );
				YPercentage				CalcShadowOffset( const RRealPoint& pt );
				
				RColor					GetBackgroundColor( ) const;
				void					RenderSurfaceAdornments( RComponentView* /* pView */, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;

	// Implementation
	protected :

	//	Members
	private :
		CDialog*						m_pDialog;
		BOOLEAN							m_bMovingShadow;	// are we dragging the shadow?
		BOOLEAN							m_bEnableMouse;

		int								m_nControlViewType;

		RComponentView*					m_pImageView;		// the component view

		RRealPoint						m_centerPoint;		// the center of the view - also center of component
		YRealDimension					m_minOffset;		// max offset for the component
		YRealDimension					m_maxOffset;		// min offset for the component
		RSoftShadowSettings				m_curShadowSetting;	// current shadow setting during mouse movement
};


#endif

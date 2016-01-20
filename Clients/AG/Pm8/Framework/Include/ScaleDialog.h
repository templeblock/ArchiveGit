//****************************************************************************
//
// File Name:		ScaleDialog.h
//
// Project:			Renaissance Framework Component
//
// Author:			Shelah Horvitz
//
// Description:	Definition of RScaleDialog.   
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/ScaleDialog.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// RScaleDialog dialog

#ifndef _SCALEDIALOG_H_
#define _SCALEDIALOG_H_

#ifndef _FRAMEWORKRESOURCEIDS_H_
#include "FrameworkResourceIDs.h"
#endif

#include "SliderCtrl.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:		RScaleDialog
//
//  Description:		Dialog which allows the user to choose the scale in which
//							selected objects will be displayed.
//
// ****************************************************************************
//
class RScaleDialog : public CDialog
{
// Construction
public:
	RScaleDialog( RRealSize& minScaleFactor, RRealSize& maxScaleFactor, CWnd* pParent = NULL);   // standard constructor

	// Range is 10% - 400%
	enum	EScaleRange { kDefaultScale = 100 };

	enum	EScaleDirection	{
				kUndefined,
				kAllDirections,
				kHorizontalOnly,
				kVerticalOnly
	};

	enum	ETimerData	{
				kScrollEventID		=  1,
				kEditEventID		=	2,
				kTimeOutMillisecs =  5
	};

// Dialog Data
	//{{AFX_DATA(RScaleDialog)
	enum { IDD = DIALOG_SCALE };
	CStatic				m_staticDisplay;
	CStatic				m_staticDots;
	CSpinButtonCtrl	m_spinScale;
	RSliderCtrl			m_sliderScale;
	int					m_iScale;
	//}}AFX_DATA

	EScaleDirection	m_eScaleDirection;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RScaleDialog)
	protected:
	virtual	void		DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Interface
public :
	void					GetScaleInfo( RRealSize& scaleFactor, BOOLEAN& fAspectCorrect ) const;

// Implementation
protected:
	CEdit					m_editScale;
	CBitmap				m_bmpDots;
	CBitmap				m_bmpDisplay;

	CSize					m_sizeOrigBitmap;
	CPoint				m_ptBitmapCenter;

	CRect					m_rcStaticDisplay;
	CSize					m_sizeStaticDisplay;
	CPoint				m_ptStaticCenter;

	int					m_iPrevPos;
	EScaleDirection	m_eOldScaleDirection;
	UINT					m_uTimerID;
	UINT					m_uEditTimerID;

	RRealSize			m_minScaleFactor;
	RRealSize			m_maxScaleFactor;

	// Generated message map functions
	//{{AFX_MSG(RScaleDialog)
	afx_msg	void		OnRadioScaleAllDirections();
	afx_msg	void		OnRadioScaleHorizontalOnly();
	afx_msg	void		OnRadioScaleVerticalOnly();
	afx_msg	void		OnSetfocusEditScale();
	afx_msg	void		OnChangeEditScale();
	afx_msg	void		OnKillfocusEditScale();
	afx_msg	void		OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg	void		OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg	void		OnTimer(UINT nIDEvent);
	afx_msg	void		OnDestroy();
	afx_msg	void		OnPaint();
	//}}AFX_MSG

	virtual	BOOL		OnInitDialog();

				void		MapDotBitmap();
				void		InitializeScaleControls();
				void		InitializeOriginalBitmap();
				void		InitializeRadioControls();
				void		StretchDisplayBitmap( int iPos );
				void		DeriveSourceAndDestinationRects( float flScale, CRect& rcSource, CRect& rcDestination );
				void		GetScaledRect( float flScale, CRect& rc );
				void		CropRectangle( float flHorzCropFactor, float flVertCropFactor, CRect& rc );
				BOOL		HackStretchBltApproximation( CDC* pDestDC, CDC* pSourceDC, int iPos );

				int		GetMinScale();
				int		GetMaxScale();
				void		UpdateScaleControls();

	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _SCALEDIALOG_H_

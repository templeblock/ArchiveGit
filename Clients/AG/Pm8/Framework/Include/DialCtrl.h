//****************************************************************************
//
// File Name:		DialCtrl.h
//
// Project:			Renaissance Framework Component
//
// Author:			Shelah Horvitz
//
// Description:	Definition of the dial control.   
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
//  $Logfile:: /PM8/Framework/Include/DialCtrl.h                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _DIALCTRL_H_
#define _DIALCTRL_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

/////////////////////////////////////////////////////////////////////////////
// User-defined messages

#define WM_USER_DIAL_LMOUSEDOWN	( WM_USER + 901 )
#define WM_USER_DIAL_MOVED			( WM_USER + 902 )
#define WM_USER_DIAL_LMOUSEUP		( WM_USER + 903 )

// ****************************************************************************
//
//  Class Name:		RDialCtrl
//
//  Description:		Control shaped like a dial which allows the user to make
//							a numeric selection by spinning a circular dial with the 
//							mouse.
//
// ****************************************************************************
//
class RDialCtrl : public CButton
{
public:
	// Construction and destruction
	RDialCtrl();
	virtual ~RDialCtrl();

// Attributes
public:

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RDialCtrl)
//	public:
//	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void				SetRange( int iMin, int iMax );
	void				SetPosition( int iPosition );
	void				CalcPosition();

protected:
	BOOL				m_fDrawingDisabled;
	BOOL				m_fInitialPaint;
	BOOL				m_fMouseDown;

	CRect				m_rcClient;
	CRect				m_rcBounds;
	CPoint			m_ptDialCtr;
	double			m_dblRadius;
	double			m_dblAngle;
	const double	m_dbl_160Degrees;
	const double	m_dbl_90Degrees;

	COLORREF			m_crFace;

	CPen				m_penFace;
	CPen				m_penHighlight;
	CPen				m_penShadow;
	CPen				m_penDkShadow;

	CBrush			m_brushFace;
	CBrush			m_brushHighlight;
	CBrush			m_brushShadow;
	CBrush			m_brushDkShadow;

	int				m_iMinRange;
	int				m_iMaxRange;
	int				m_iPosition;
	double			m_dblTotalPositions;

	BOOL				Initialize();
	void				InitializeRects();
	void				DrawDial( CDC* pDC );
	BOOL				IsPointOnFace( const CPoint& pt ) const;
	void				CalcAngle( const CPoint& pt );

	// Generated message map functions
	//{{AFX_MSG(RDialCtrl)
	afx_msg	void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	afx_msg	void	OnPaint();
	afx_msg	void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg	void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg	void	OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _DIALCTRL_H_

/////////////////////////////////////////////////////////////////////////////

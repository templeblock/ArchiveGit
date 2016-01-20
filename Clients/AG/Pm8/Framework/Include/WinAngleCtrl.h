//****************************************************************************
//
// File Name:   WinAngleCtrl.h
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Definition of the RWinAngleCtrl object
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/WinAngleCtrl.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _WINANGLECTRL_H_
#define _WINANGLECTRL_H_

//#include "BitmapImage.h"
#include "ScratchBitmapImage.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// Define a user message for notifying 
// the parent when the angle has changed.
#define UM_ANGLE_CHANGE	 (WM_USER + 200)

typedef	RArray<RIntPoint>					   YPointCollection;
typedef	YPointCollection::YIterator		YPointIterator;

class FrameworkLinkage RWinAngleCtrl : public CWnd
{
// Construction
public:
	RWinAngleCtrl( BOOLEAN fSnapToGuides = TRUE );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RWinAngleCtrl)
	protected:
//	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual			~RWinAngleCtrl();

	YAngle			GetAngle( ) ;
	void				SetAngle( YAngle angle ) ;

protected:

	BOOLEAN			Initialize() ;
	void				ComputeAngle( CPoint point ) ;
//	void				UpdateBitmap() ;

// Generated message map functions
protected:
	//{{AFX_MSG(RWinAngleCtrl)
	afx_msg BOOL	OnEraseBkgnd( CDC* pDC );
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnEnable( BOOL fEnable );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

//	RScratchBitmapImage	m_biImage ;
	RIntRect					m_rcDestRect ;

	YPointCollection		m_pointCollection ;
	YAngle					m_flAngle ;
	YAngle					m_flOldAngle ;

	BOOLEAN					m_fMouseCaptured ;
	BOOLEAN					m_fSnapToGuides ;
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _WINANGLECTRL_H_
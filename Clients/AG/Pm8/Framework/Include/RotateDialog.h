//****************************************************************************
//
// File Name:		RotateDialog.h
//
// Project:			Renaissance Framework Component
//
// Author:			Shelah Horvitz
//
// Description:	Definition of RRotateDialog.   
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
//  $Logfile:: /PM8/Framework/Include/RotateDialog.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// RRotateDialog dialog

#ifndef _ROTATEDIALOG_H_
#define _ROTATEDIALOG_H_

#ifndef _FRAMEWORKRESOURCEIDS_H_
#include "FrameworkResourceIDs.h"
#endif

#ifndef _BITMAPIMAGE_H_
#include "BitmapImage.h"
#endif // _BITMAPIMAGE_H_

#include "WinAngleCtrl.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:		RRotateDialog
//
//  Description:		Dialog which allows the user to choose the rotation with 
//							which selected objects will be displayed.
//
// ****************************************************************************
//
class RRotateDialog : public CDialog
{
// Construction
public:
	enum	ERotationRange {
				kMinRotation		=   0,
				kDefaultRotation	=   0,
				kMaxRotation		= 359
			};

	// how often is the preview updates (milliseconds).
	enum {kTimerID = 1, kMinUpdateInterval = 500};

	RRotateDialog( CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(RRotateDialog)
	enum { IDD = DIALOG_ROTATE };

	CSpinButtonCtrl	m_spinRotate;
	CStatic				m_staticDisplay;
	CEdit					m_editRotation;
	CStatic				m_rotatePlaceholder;
	//}}AFX_DATA

	int					m_iRotation;
	DWORD					m_dwLastUpdateTime;

	RWinAngleCtrl		m_dialRotate;
	BOOLEAN				m_bInitialised;

	YAngle				GetRotationAngle() const;
	void					SetRotationAngle( YAngle angle );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RRotateDialog)
	protected:
	virtual	void		DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	RBitmapImage		m_rBitmapImage;
	RIntSize				m_sizeBmpImage;

	RIntRect				m_rectStaticDisplay;
	RIntSize				m_rSizeStaticDisplay;
	RIntPoint			m_rPtStaticCenter;

	UINT					m_uTimerID;

	int					m_iPrevPos;
	BOOL					m_fScrolling;

	void					RotateDisplay( int iPos );
	void					DeriveSourceAndDestinationRects( const RIntSize& sizeBmpRotated, RIntRect& rectSource, RIntRect& rectDest );

	afx_msg LRESULT	OnDialAngleChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void		OnTextChange();

	void					OnTimer( UINT nID );
	void					OnDestroy( );


	// Generated message map functions
	//{{AFX_MSG(RRotateDialog)
	virtual	BOOL		OnInitDialog();
	afx_msg	void		OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _ROTATEDIALOG_H_

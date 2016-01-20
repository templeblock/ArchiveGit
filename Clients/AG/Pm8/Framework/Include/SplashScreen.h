/*****************************************************************************
 *
 *	@doc
 *
 *	@module	Splash.H - Betsy startup splash screen header file |
 *
 *			 	CG: This file was added by the Splash Screen component.
 *
 *	Authors:	Jim Beveridge
 *
 *	Copyright (c) 1996 Turning Point Software.  All Rights Reserved.
 *
 *	$Header:: /PM8/Framework/Include/SplashScreen.h 1     3/03/99 6:16p Gbeddo $
 *
 ****************************************************************************/

#ifndef _SPLASH_SCRN_H_
#define _SPLASH_SCRN_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

class FrameworkLinkage CSplashWnd : public CWnd
{
// Construction
protected:
	CSplashWnd();

// Attributes:
public:
	CDC			m_MemDC;
	HBITMAP		m_hOldBitmap;
	HPALETTE		m_hPalette;
	enum			{kTimerID = 25, kTimerElapse = 3000};

// Operations
public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static void CloseSplashScreen();
	static BOOL PreTranslateAppMessage(MSG* pMsg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	~CSplashWnd();
	virtual void PostNcDestroy();

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;

// Generated message map functions
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif

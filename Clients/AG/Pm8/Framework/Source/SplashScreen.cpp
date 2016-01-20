//****************************************************************************
//
// File Name:		SplashScreen.cpp
//
// Project:			Framework Component
//
// Author:			Daniel Selman
//
// Description:	Class to display the Renaissance Splash screen at startup
//						This code is based on the MSVC4.1 Splash screen component
//						but has been subsequently modified by Jim Beveridge at TPS.
//						Daniel Selman has made minor integration changes.
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
//  $Logfile:: /PM8/Framework/Source/SplashScreen.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

/////////////////////////////////////////////////////////////////////////
// INCLUDE FILES
#ifndef	_WINDOWS
	#error "This file should only be compiled under Windows"
#endif

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "Mainframe.h"
#include	"FrameworkResourceIDs.h"
#include "DialogUtilities.h"
#include "SplashScreen.h"


/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;
CSplashWnd::CSplashWnd()
{
	m_hOldBitmap	= NULL;
	m_hPalette		= NULL;
}

CSplashWnd::~CSplashWnd()
{
	//	Delete the loaded palette
	VERIFY( ::DeleteObject( m_hPalette ) );

	// Clear the static window pointer.
	ASSERT(c_pSplashWnd == this);
	c_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	c_bShowSplashWnd = bEnable;
}

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/)
{
	if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new CSplashWnd;
	if (!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();
}

BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if (c_pSplashWnd == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		c_pSplashWnd->HideSplashScreen();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}

BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
	// Load the bitmap from resource and cache off the palette
	BITMAP bm;
	HBITMAP hBitmap;
	   
 	// Load the bitmap from resource and get its palette..
   hBitmap = LoadResourceBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(BITMAP_SPLASH_SCREEN), &m_hPalette );
	ASSERT( hBitmap != NULL );

	if ( m_MemDC.CreateCompatibleDC( NULL ) != FALSE )
	{
		m_hOldBitmap = (HBITMAP) ::SelectObject( m_MemDC, hBitmap );

		if ( m_hOldBitmap != NULL )
		{
			CBitmap *pBitmap = CBitmap::FromHandle( hBitmap );

			if ( pBitmap != NULL )
			{
				pBitmap->GetBitmap(&bm);
				::GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

				return CreateEx(WS_EX_TOPMOST,
					AfxRegisterWndClass( 0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
					NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);
			}
		}
	}

	return -1;
}

void CSplashWnd::HideSplashScreen()
{
	// Destroy the window, and update the mainframe.
	DestroyWindow();
	AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
	// MemDC Cleanup
   HGDIOBJ hSplashScreenBitmap = SelectObject(m_MemDC,m_hOldBitmap);
	if ( hSplashScreenBitmap )
		VERIFY( ::DeleteObject( hSplashScreenBitmap ) );
   
	// Free the C++ class.
	delete this;
	c_pSplashWnd = NULL;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	SetTimer(kTimerID, kTimerElapse, NULL);

	return 0;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	ASSERT( m_hPalette != NULL );
	   
	// Realize the palette into the dcs
   HPALETTE hOldPaintPalette = ::SelectPalette( dc, m_hPalette, FALSE);
   dc.RealizePalette( );
   HPALETTE hOldMemPalette = ::SelectPalette( m_MemDC, m_hPalette, FALSE);
   ::RealizePalette( m_MemDC );

	CRect rect;
	GetWindowRect( rect );

	// Blit the memory dc into the screen dc.
   ::BitBlt( dc, 0,0,rect.Width(),rect.Height(),m_MemDC,0,0,SRCCOPY);

	// Restore the old palettes.
	if ( hOldPaintPalette )
		VERIFY( m_hPalette == ::SelectPalette( dc, hOldPaintPalette, FALSE) );
	if ( hOldMemPalette )
		VERIFY( m_hPalette == ::SelectPalette( m_MemDC, hOldMemPalette, FALSE) );
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
	UNREFERENCED_PARAMETER(nIDEvent);

	// Destroy the splash screen window.
	HideSplashScreen();
}

/************************************************************************
 *
 *		CSplashWnd::CloseSplashScreen
 *
 * @mfunc	The mainframe can call this to forcibly close the
 *				splash screen.
 *
 */

//static
void CSplashWnd::CloseSplashScreen()
{
	if (c_pSplashWnd)
	{
		c_pSplashWnd->KillTimer(kTimerID);

		// Destroy the splash screen window.
		c_pSplashWnd->HideSplashScreen();
	}
}


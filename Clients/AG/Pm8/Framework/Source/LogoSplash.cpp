//****************************************************************************
//
// File Name:  LogoSplash.cpp
//
// Project:    Renaissance user interface
//
// Author:     Lance Wilson
//
// Description: Implemenets a splash control for displaying the
//              print shop logo.
//
// Portability: Windows Specific
//
// Developed by:  Broderbund Software, Inc.
//						500 Redwood Blvd.
//						Novato, CA 94948
//						(415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/LogoSplash.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"
ASSERTNAME

#include "LogoSplash.h"
#include "FrameworkResourceIDs.h"	// Resource defines

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RLogoSplash

RLogoSplash::RLogoSplash()
{
	m_hBitmap = NULL ;
}

RLogoSplash::~RLogoSplash()
{
	if( m_hBitmap )
		DeleteObject( m_hBitmap ) ;
}

BEGIN_MESSAGE_MAP(RLogoSplash, CStatic)
	//{{AFX_MSG_MAP(RLogoSplash)
	ON_WM_NCPAINT( )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RLogoSplash message handlers

void RLogoSplash::PreSubclassWindow() 
{
//	SetWindowLong( m_hWnd, GWL_STYLE, GetStyle() | SS_BITMAP ) ;
	m_hBitmap = LoadBitmap( AfxGetResourceHandle(), 
		MAKEINTRESOURCE( BITMAP_APPLICATION_LOGO ) ) ;

//	SetBitmap( m_hBitmap ) ;
	BITMAP bm ;
	GetObject( m_hBitmap, sizeof( bm ), &bm ) ;
	CRect rect( 0, 0, bm.bmWidth - 1, bm.bmHeight - 1 ) ;

	AdjustWindowRectEx( rect, GetStyle(), FALSE, GetExStyle() ) ;
	SetWindowPos( NULL, 0, 0, rect.Width(), rect.Height(), 
		SWP_NOMOVE | SWP_NOZORDER ) ;
}

void RLogoSplash::OnNcPaint()
{
	CWindowDC dc(this) ;

	CDC dcMem ;
	dcMem.CreateCompatibleDC( &dc ) ;
	
	BITMAP bm ;
	GetObject( m_hBitmap, sizeof( bm ), &bm ) ;

	HBRUSH  hBrush  = CreateSolidBrush( GetSysColor( COLOR_BTNSHADOW ) ) ;
	HBITMAP hBitmap = (HBITMAP) dcMem.SelectObject( m_hBitmap ) ;
	HBRUSH  hOldBrush = (HBRUSH) dc.SelectObject( hBrush ) ;

	COLORREF crOldTextColor = dc.SetTextColor( RGB( 0, 0, 0 ) ) ;
	COLORREF crOldBkColor   = dc.SetBkColor( RGB( 255, 255, 255 ) ) ;

	dc.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, 0xB8074AL ) ;

	dc.SelectObject( hBitmap ) ;
	dc.SelectObject( hOldBrush ) ;
	dc.SetTextColor( crOldTextColor ) ;
	dc.SetTextColor( crOldBkColor ) ;

	::DeleteObject( hBrush ) ;
}

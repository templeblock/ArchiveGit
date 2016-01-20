//****************************************************************************
//
// File Name:		ColorPalette.h
//
// Project:			Renaissance user interface
//
// Author:			Lance Wilson
//
// Description:	Definition of RColorPalette dialog object.  
//
// Portability:	Windows Specific
//
// Developed by:  Broderbund Software
//						500 Redwood Blvd.
//						Novato, CA 94948
//						(415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/ColorPalette.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#define UM_COLOR_CHANGED	WM_USER + 100 

class RBitmapImage;

class FrameworkLinkage RWinColorPaletteWell : public CStatic
{
// Construction
public:

							RWinColorPaletteWell();

   void					SetColor( RColor& crColor );
	RColor&				SelectedColor( ) { return m_crColor; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RWinColorDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RWinColorPalette)
	afx_msg void OnPaint( );
	afx_msg void OnLButtonDown( UINT, CPoint pt );
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg UINT OnGetDlgCode( );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	RColor				m_crColor;
	BOOLEAN				m_fFlyoversEnabled;
} ;

class FrameworkLinkage RWinColorPalette : public CStatic
{
// Construction
public:

							RWinColorPalette();
							~RWinColorPalette( );

   RSolidColor			SelectedColor() { return m_crSelected; }
   void					SetColor( RSolidColor crColor );
	void					EnableFlyovers( BOOLEAN fEnabled = TRUE );

	// This method must be called at application start-up prior to the first use
	// of the color palette dialog since the palette bitmap is cached once it is
	// first pulled from the resource.  In order to over-ride the color picker's
	// palette, this must be called prior to user input control. -RIP-
	static void			SetPaletteBitmapID( UINT nID ) { m_uPaletteBitmapID = nID; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RWinColorDlg)
	protected:
	virtual void PreSubclassWindow( );
	//}}AFX_VIRTUAL

// Implementation
protected:

	static RBitmapImage&		GetPaletteBitmapImage( );

	static COLORREF			ColorFromPoint( RIntPoint ptCell );
	static RIntPoint			CellFromPoint( CPoint pt );
	static RIntPoint			FindColor( YPlatformColor crColor, BOOL fExact = FALSE );
	
	// Generated message map functions
	//{{AFX_MSG(RWinColorPalette)
	afx_msg void OnPaint( );
	afx_msg void OnLButtonDown( UINT, CPoint pt );
	afx_msg void OnMouseMove( UINT, CPoint pt );
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	afx_msg UINT OnGetDlgCode( );
	afx_msg void OnDestroy( );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	RIntPoint			m_ptSelected;
	RIntPoint			m_ptHilited;
	RIntRect				m_rcColorChip;

	YPlatformColor		m_crHilited;
	YPlatformColor		m_crSelected;
	BOOLEAN				m_fFlyoversEnabled;

	static UINT			m_uPaletteBitmapID;
} ;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif  // COLORPALETTE_H
//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// CTLTOOLS.H - Public utility functions for working with controls

#ifndef _CTLTOOLS_H_
#define _CTLTOOLS_H_

BOOL StyleOn( HWND hWnd, DWORD dwFlag );
BOOL IsWordBitSet( HWND hWnd, int nOffset, WORD wBit );
void SetWordBit( HWND hWnd, int nOffset, WORD wBit, BOOL on );
void SetLongBit( HWND hWnd, int nOffset, LONG lBit, BOOL on );
void HilightRect( HDC hDC, LPRECT lpRect, HBRUSH hBrush );
void HilightFrame( HDC hDC, LPRECT lpRect, HBRUSH hBrush, int iWidth, BOOL bSetColors );
void GrayWindow( HDC hDC, HWND hWindow, DWORD dwStyle );
void GrayWholeWindow( HDC hDC, HWND hWindow, DWORD dwStyle );
void MgxGrayArea( HDC hDC, int x, int y, int iWidth, int iHeight);
int ColorText( HDC hDC, int x, int y, LPTSTR szStr, int length, COLOR Color );
void DrawBoxFrame( HDC hDC, LPRECT lpRect );
void DrawSculptedBox( HDC hDC, LPRECT lpRect, BOOL bFocus, BOOL bDown,
                        BOOL bDouble );
void DrawBitmap( HDC hDC, int x, int y, HBITMAP hBitmap );

class CDrawTools
{
public:
	CDrawTools();
	~CDrawTools();
	void OnSysColorChange();
	HBRUSH GetBtnFaceBrush() { return m_hBtnFaceBrush; }
	HBRUSH GetBtnShadowBrush() { return m_hBtnShadowBrush; }
	HBRUSH GetBtnHilightBrush() { return m_hBtnHilightBrush; }
	HBRUSH GetBtnGrayBrush() { return m_hBtnGrayBrush; }
	HPEN GetBtnFacePen() { return m_hBtnFacePen; }
	HPEN GetBtnShadowPen() { return m_hBtnShadowPen; }
	HPEN GetBtnHilightPen() { return m_hBtnHilightPen; }
	HPEN GetFramePen() { return m_hFramePen; }

private:
	void FreeUpTools();

private:
	HBRUSH 	m_hBtnFaceBrush;
	HBRUSH 	m_hBtnShadowBrush;
	HBRUSH 	m_hBtnHilightBrush;
	HBRUSH 	m_hBtnGrayBrush;
	HPEN 	m_hBtnFacePen;
	HPEN 	m_hBtnShadowPen;
	HPEN 	m_hBtnHilightPen;
	HPEN	m_hFramePen;
};

extern CDrawTools DrawTools;

#endif // _CTLTOOLS_H_


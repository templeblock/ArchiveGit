// BmStatic.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "BmStatic.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette* pOurPal;
/////////////////////////////////////////////////////////////////////////////
// CBmpStatic

CBmpStatic::CBmpStatic()
{
}

CBmpStatic::~CBmpStatic()
{
}


BEGIN_MESSAGE_MAP(CBmpStatic, CStatic)
	//{{AFX_MSG_MAP(CBmpStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpStatic message handlers

void CBmpStatic::OnPaint() 
{
	if(GetStyle() & SS_BITMAP)
	{
		CPaintDC dc(this); // device context for painting
		dc.SelectPalette(pOurPal, FALSE);
		dc.RealizePalette();
		CRect rcClient;
		GetClientRect(&rcClient);
		CBitmap* pBmp = CBitmap::FromHandle(GetBitmap());
		Util::TransparentBltBitmap(dc, rcClient, *pBmp, NULL, pOurPal, 0,0);
	}
	else
		CStatic::OnPaint();
}

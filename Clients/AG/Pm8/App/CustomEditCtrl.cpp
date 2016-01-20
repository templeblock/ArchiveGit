// CustomEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CustomEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomEditCtrl

CCustomEditCtrl::CCustomEditCtrl():
	m_pEditFont			(NULL),
	m_bCustCtrlColors	(FALSE),
	m_nBorderStyle		(kNoEditBorderStyle),
	m_czFrameSize		(0,0)
{
}

CCustomEditCtrl::~CCustomEditCtrl()
{
	if (m_pEditFont)
		delete m_pEditFont;
}


BEGIN_MESSAGE_MAP(CCustomEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CCustomEditCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()     
	ON_WM_KILLFOCUS() 
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT,OnSetFont)
END_MESSAGE_MAP()

/* ============================================
		CCustomEditCtrl::DrawCustomBorder()
		Do the sunken or the raised border
=============================================== */
void CCustomEditCtrl::DrawCustomBorder(CDC& theDC, CRect& winRect)
{

	CBrush		brHighlight, brShadow;
	brHighlight.CreateSolidBrush((m_nBorderStyle == kBumpBorderStyle) ? 
											m_clrBoxHighlight : m_clrBoxShadow );
	brShadow.CreateSolidBrush((m_nBorderStyle == kBumpBorderStyle) ? 
											m_clrBoxShadow : m_clrBoxHighlight );

	CPen			nullPen;
	nullPen.CreateStockObject(NULL_PEN);

	int width = m_czFrameSize.cx;
	int height= m_czFrameSize.cy;

	// top and left
	const int	polySize = 6;
	POINT			points[polySize];
	points[0].x = winRect.left + width;
	points[0].y = winRect.bottom - height;
	points[1].x = winRect.left;
	points[1].y = winRect.bottom;
	points[2].x = winRect.left;
	points[2].y = winRect.top;
	points[3].x = winRect.right;
	points[3].y = winRect.top;
	points[4].x = winRect.right - width;
	points[4].y = winRect.top + height;
	points[5].x = winRect.left + width;
	points[5].y = winRect.top + height;

	CBrush* oldBrush = theDC.SelectObject(&brHighlight);
	CPen* oldPen = theDC.SelectObject(&nullPen);
	theDC.Polygon(points, polySize);
	
	// bottom and right
	points[2].x = points[3].x;
	points[2].y = points[1].y;
	points[5].x = points[4].x;
	points[5].y = points[0].y;
	
	theDC.SelectObject(&brShadow);
	theDC.Polygon(points, polySize);

	theDC.SelectObject(oldPen);
	theDC.SelectObject(oldBrush);

}

/* ============================================
		CCustomEditCtrl::SetEditFont()
		Set the custom font into the control
=============================================== */
void CCustomEditCtrl::SetEditFont(CString strEditFont,int nEditFontSize )
{
	if (m_pEditFont)
		delete m_pEditFont;

	m_pEditFont = new CFont;
	VERIFY (m_pEditFont->CreatePointFont (nEditFontSize*10, strEditFont, NULL));
   SendMessage(WM_SETFONT, (WPARAM)m_pEditFont->GetSafeHandle());
	 
}

/* ============================================
		CCustomEditCtrl::Set3DColors()
		Set the custom 3d colors into the control
=============================================== */
void CCustomEditCtrl::Set3DColors(COLORREF highlightClr, COLORREF shadowClr)
{
	m_clrBoxHighlight = highlightClr;
	m_clrBoxShadow = shadowClr;
	
	m_bCustCtrlColors = TRUE;	// once the color is set, this flag is true
}


/////////////////////////////////////////////////////////////////////////////
// CCustomEditCtrl message handlers

void CCustomEditCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar != '\r')
		CEdit::OnChar(nChar,nRepCnt,nFlags );
}

void CCustomEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == '\r')
		GetParent()->SendMessage(WM_KEYDOWN,(WPARAM)'\r',1);
}

/////////////////
// Handle WM_CTLCOLOR reflected from parent:
// Must also set text and background colors, that's a Windows thing.
// 
HBRUSH CCustomEditCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
   // By making the brush static, only one is used for all
   // instances of CCustomEditCtrl. The compiler doesn't initialize it
   // until the first time CCustomEditCtrl::CtlColor is called      
	//
   static CBrush s_brush(m_clrBehindText);

   pDC->SetBkColor(m_clrBehindText);    // required for edit controls
   pDC->SetTextColor(m_clrText);  // ditto

	return s_brush;               // return background brush   
}

//////////////////
// I got focus: force repaint
//
void CCustomEditCtrl::OnSetFocus(CWnd* pOldWnd)
{
   Invalidate();           // invalidate client area
   UpdateWindow();         // force repaint now   
   CEdit::OnSetFocus(pOldWnd);
}
//////////////////
// I lost focus: force repaint
//
void CCustomEditCtrl::OnKillFocus(CWnd* pNewWnd)
{
   Invalidate();           // invalidate client area
   UpdateWindow();         // force repaint now   
   CEdit::OnKillFocus(pNewWnd);
}


long CCustomEditCtrl::OnSetFont(WPARAM wParam, LPARAM /* lParam */)
{   
	
	HFONT hFont = (HFONT)wParam;
	if (hFont != NULL)
	{
		CDC *pDC = GetDC();
		
		pDC->SelectObject(hFont);
		ReleaseDC(pDC);
	}
	return Default();
}

void CCustomEditCtrl::PreSubclassWindow() 
{
	CEdit::PreSubclassWindow();

	SetEditFont("Arial", 12);
   m_clrBehindText = RGB(0,0,0);    
   m_clrText = RGB(255,255,255);

	// default them at system color first
	m_clrBoxHighlight = GetSysColor(COLOR_3DLIGHT);
	m_clrBoxShadow = GetSysColor(COLOR_3DSHADOW);

	m_nBorderStyle = kSunkenBorderStyle;
	
	LONG theStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if ( theStyle & WS_EX_CLIENTEDGE )
	{
		m_czFrameSize.cx = GetSystemMetrics(SM_CXEDGE);
		m_czFrameSize.cy = GetSystemMetrics(SM_CYEDGE);
	}


}

void CCustomEditCtrl::OnNcPaint() 
{
	if (m_nBorderStyle == kNoEditBorderStyle ||
			(m_czFrameSize.cx ==0 && m_czFrameSize.cy == 0))	// no border
		return;											// do nothing

	// Get window DC that we will be drawing into.
	CWindowDC dc(this);

	// Compute the rectangle we will be updating.
	CRect windowRect;
	GetWindowRect(windowRect);
	ScreenToClient(windowRect);

	// make it zero based
	windowRect.OffsetRect(-windowRect.left, -windowRect.top);

	if (!m_bCustCtrlColors)		// using windows colors
	{
		UINT brdType = (m_nBorderStyle == kBumpBorderStyle) ?
							EDGE_RAISED : EDGE_SUNKEN;
		dc.DrawEdge(windowRect, brdType, BF_RECT);		// use this cool call
	}
	else
	{
		DrawCustomBorder(dc, windowRect);
	}

	// Do not call CEdit::OnNcPaint() for painting messages

}


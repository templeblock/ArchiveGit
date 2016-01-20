// SplshTxt.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "SplshTxt.h"
#include "util.h"
#include "bmptiler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette* pOurPal;  // PrintMaster Palette

/////////////////////////////////////////////////////////////////////////////
// CSplashText dialog


CSplashText::CSplashText(CString csText, CBitmap& bitmap, COLORREF crBackColor, CWnd* pParent /*=NULL*/)
	: CDialog(CSplashText::IDD, pParent),
	m_csText(csText),
	m_cbBitmap(bitmap),
	m_crBkgndColor(crBackColor)
{
}



void CSplashText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashText)
	DDX_Control(pDX, IDC_STATIC_TEXT, m_cStaticText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashText, CDialog)
	//{{AFX_MSG_MAP(CSplashText)
   ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashText message handlers

BOOL CSplashText::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rcWindow;
	GetWindowRect(&rcWindow);

	m_cStaticText.SetWindowText(m_csText);

	CDC* pDC = GetDC();
	CSize szTextSize = pDC->GetTextExtent(m_csText);
	CPoint ptCenter = rcWindow.CenterPoint();
	rcWindow.SetRect(ptCenter.x - szTextSize.cx / 2,
						ptCenter.y - szTextSize.cy / 2,
						ptCenter.x + szTextSize.cx / 2,
						ptCenter.y + szTextSize.cy / 2);

	rcWindow.InflateRect(20,20);

	SetWindowPos(&wndTop, rcWindow.left, rcWindow.top, rcWindow.Width(),
				rcWindow.Height(), SWP_NOOWNERZORDER);

	CRect rcClient;
	GetClientRect(&rcClient);
	ptCenter = rcClient.CenterPoint();
	CRect rcText(ptCenter.x - szTextSize.cx / 2,
						ptCenter.y - szTextSize.cy / 2,
						ptCenter.x + szTextSize.cx / 2,
						ptCenter.y + szTextSize.cy / 2);


	m_cStaticText.SetWindowPos(&wndTop, rcText.left, rcText.top, rcText.Width(),
				rcText.Height(), SWP_NOOWNERZORDER | SWP_SHOWWINDOW);

	m_cStaticText.Invalidate();
	ReleaseDC(pDC);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSplashText::OnEraseBkgnd(CDC* pDC) 
{
   CRect       rcFill;

   GetClientRect(&rcFill);

   if(m_cbBitmap.m_hObject != NULL)
   {
		  // Tile the background.
		CBmpTiler Tiler;
		Tiler.TileBmpInRect(*pDC, rcFill, m_cbBitmap);
   }
   else
	   pDC->FillSolidRect(rcFill, m_crBkgndColor);

   return TRUE;
	
}


HBRUSH CSplashText::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = NULL;
	
	if(nCtlColor == CTLCOLOR_STATIC )
	{
        // set background color to transparent
        pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
        pDC->SetBkMode(TRANSPARENT);


        hbr = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	}
	else
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

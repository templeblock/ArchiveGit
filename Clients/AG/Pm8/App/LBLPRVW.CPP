/*
// $Workfile: LBLPRVW.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LBLPRVW.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     12/23/98 1:50p Psasse
// Code clean up
// 
//    Rev 1.0   14 Aug 1997 15:22:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:00   Fred
// Initial revision.
// 
//    Rev 1.2   20 Jul 1997 13:21:44   Jay
// Fixes for label preview highlight
// 
//    Rev 1.1   22 Oct 1996 15:25:58   johno
// Added CS_GLOBALCLASS to style (RegisterMyClass)
// 
//    Rev 1.0   14 Mar 1996 13:32:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:12   FRED
// Initial revision.
// 
//    Rev 1.2   06 Oct 1995 15:23:20   FRED
// Win32 SendMessage() changes
// 
//    Rev 1.1   10 Aug 1995 13:22:40   JAY
//  
// 
//    Rev 1.0   20 Jul 1995 12:52:50   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"

#include "lblprvw.h"
#include "paper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelPreview

WNDPROC CLabelPreview::m_pfnSuperWndProc = NULL;

LONG CALLBACK AFX_EXPORT CLabelPreview::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/
	
	CLabelPreview* pLabelPreview = new CLabelPreview;
	pLabelPreview->Attach(hWnd) ;
	
	pLabelPreview->m_fCreatedByClassName = TRUE ;
	
	/*
	// Subclass the window by changing the window procedure to AfxWndProc
	// and pass the current message to it.
	*/
	
	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);

#ifdef STRICT
	return ::CallWindowProc((WNDPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#endif
}

BOOL CLabelPreview::RegisterMyClass()
{
	WNDCLASS wcls;
	
	m_pfnSuperWndProc = ::DefWindowProc;
	
	static const char szClassName[] = "MlsLabelPreview";
	
	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CLabelPreview::FirstMsgOnlyWndProc);
	}
	
	// JRO
  // wcls.style = CS_DBLCLKS;
  wcls.style = CS_GLOBALCLASS | CS_DBLCLKS;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CLabelPreview::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;
	
	return (RegisterClass(&wcls) != 0);
}

void CLabelPreview::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

CLabelPreview::CLabelPreview()
{
	m_fCreatedByClassName = FALSE;
	m_pLabelData = NULL;
	m_fWantSelect = FALSE;
	m_nSelectedLabel = -1;
}

CLabelPreview::~CLabelPreview()
{
}

BEGIN_MESSAGE_MAP(CLabelPreview, CWnd)
	//{{AFX_MSG_MAP(CLabelPreview)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelPreview message handlers

void CLabelPreview::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (m_pLabelData != NULL)
	{
		CRect crClient;
		GetClientRect(&crClient);

		m_pLabelData->DrawPage(&dc, crClient, FALSE, &m_crDrawn);

		ShowSelection(&dc, TRUE);
	}
}

void CLabelPreview::SetLabel(CPaperInfo* pLabelData)
{
	m_pLabelData = pLabelData;
	InvalidateRect(NULL);
}

void CLabelPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_fWantSelect)
	{
		TRACE("Clicked on point %d, %d\r\n", point.x, point.y);

		CFixedPoint Dims = m_pLabelData->PaperDims();

		int nSlots = m_pLabelData->Slots();
		CRect crDrawn = m_crDrawn;
//		crDrawn.InflateRect(-1, -1);
		CFixed lWidth = MakeFixed(crDrawn.Width());
		CFixed lHeight = MakeFixed(crDrawn.Height());

		for (int nSlot = 0; nSlot < nSlots; nSlot++)
		{
			CFixedRect Bound = m_pLabelData->SlotBounds(nSlot);

			CRect crSlot;

			crSlot.left = crDrawn.left + FixedInteger(MulDivFixed(Bound.Left,
																	 lWidth,
																	 Dims.x));
			crSlot.top = crDrawn.top + FixedInteger(MulDivFixed(Bound.Top,
																	 lHeight,
																	 Dims.y));
			crSlot.right = crDrawn.left + FixedInteger(MulDivFixed(Bound.Right,
																	 lWidth,
																	 Dims.x));
			crSlot.bottom = crDrawn.top + FixedInteger(MulDivFixed(Bound.Bottom,
																	 lHeight,
																	 Dims.y));

			if (crSlot.PtInRect(point))
			{
				SelectLabel(nSlot);
				break;
			}
		}
	}
	else
	{
#ifdef WIN32
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
#else
		GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID(), MAKELPARAM(GetSafeHwnd(), BN_CLICKED));
#endif
	}
}

void CLabelPreview::OnLButtonDblClk(UINT nFlags, CPoint point)
{
#ifdef WIN32
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_DOUBLECLICKED), (LPARAM)GetSafeHwnd());
#else
	GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID(), MAKELPARAM(GetSafeHwnd(), BN_DOUBLECLICKED));
#endif
}

/*
// Select a label.
*/

void CLabelPreview::SelectLabel(int nLabel)
{
	if (m_nSelectedLabel != nLabel)
	{
		ShowSelection(NULL, FALSE);
		m_nSelectedLabel = nLabel;
		ShowSelection(NULL, TRUE);
	}
}

void CLabelPreview::ShowSelection(CDC* pInDC, BOOL fHighlight)
{
	if (m_nSelectedLabel != -1 && !m_crDrawn.IsRectEmpty())
	{
		CDC* pDC = pInDC;

		if (pDC == NULL)
		{
			pDC = GetDC();
		}

	/*
	// Compute where to draw the select.
	*/

		//CFixedPoint Dims = m_pLabelData->PaperDims();
		CFixedRect Bound = m_pLabelData->SlotBounds(m_nSelectedLabel);

		// Same as in CPaperInfo::DrawPage().
		const CFixed lTractorWidth = MakeFixed(7)/16;			/* 7/16" */
		const int nMinBorder = 4;

		CRect crClient;
		GetClientRect(&crClient);
		CRect crDrawn = m_crDrawn;
//		CFixed lWidth = MakeFixed(crDrawn.Width());
//		CFixed lHeight = MakeFixed(crDrawn.Height());
		CFixed lWidth = MakeFixed(crClient.Width());
		CFixed lHeight = MakeFixed(crClient.Height());

		// Get the scale factor for the page.
		CFixed lScale = ((lWidth < lHeight) ? lWidth : lHeight) - MakeFixed(nMinBorder);

		// Compute the larger dimension.
		CFixed lScaleDenom;

		CFixedPoint Page = m_pLabelData->PaperDims();
		BOOL fTractor = (m_pLabelData->Type() == m_pLabelData->PAPER_Continuous);

		// Tractor feed will have more X reduction.
		CFixed lExtraXScale = fTractor ? lTractorWidth*2 : 0;
		CFixed lExtraYScale = 0;

		if (Page.x + lExtraXScale > Page.y + lExtraYScale)
		{
			lScaleDenom = Page.x + lExtraXScale;
		}
		else
		{
			lScaleDenom = Page.y + lExtraYScale;
		}

		lScale = DivFixed(lScale, lScaleDenom);

		CRect crSlot;

		crSlot.left = crDrawn.left + FixedInteger(MulFixed(Bound.Left, lScale));
		crSlot.top = crDrawn.top + FixedInteger(MulFixed(Bound.Top, lScale));
		crSlot.right = crDrawn.left + FixedInteger(MulFixed(Bound.Right, lScale));
		crSlot.bottom = crDrawn.top + FixedInteger(MulFixed(Bound.Bottom, lScale));

		COLORREF clrHighlight = fHighlight
							? ::GetSysColor(COLOR_HIGHLIGHT)
							: RGB(255, 255, 255);

		m_pLabelData->DrawSlot(pDC->GetSafeHdc(),
									  crSlot,
									  FALSE,
									  TRUE,
									  &clrHighlight);

		if (pInDC == NULL)
		{
			ReleaseDC(pDC);
		}
	}
}

/*
// $Workfile: phoprjprvw.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/phoprjprvw.cpp $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 2     1/30/99 11:32p Psasse
// Updated and fairly stable EasyPrints code
// 
*/ 

#include "stdafx.h"
#include "pmw.h"

#include "photoprj.h"
#include "phoprjprvw.h"
#include "paper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define NUM_SUBSECTIONS 3

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectPreview

WNDPROC CPhotoProjectPreview::m_pfnSuperWndProc = NULL;

LONG CALLBACK AFX_EXPORT CPhotoProjectPreview::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/
	
	CPhotoProjectPreview* pPhotoProjectPreview = new CPhotoProjectPreview;
	pPhotoProjectPreview->Attach(hWnd) ;
	
	pPhotoProjectPreview->m_fCreatedByClassName = TRUE ;
	
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

BOOL CPhotoProjectPreview::RegisterMyClass()
{
	WNDCLASS wcls;
	
	m_pfnSuperWndProc = ::DefWindowProc;
	
	static const char szClassName[] = "MlsPhotoProjectPreview";
	
	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CPhotoProjectPreview::FirstMsgOnlyWndProc);
	}
	
	// JRO
  // wcls.style = CS_DBLCLKS;
  wcls.style = CS_GLOBALCLASS | CS_DBLCLKS;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CPhotoProjectPreview::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;
	
	return (RegisterClass(&wcls) != 0);
}

void CPhotoProjectPreview::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

CPhotoProjectPreview::CPhotoProjectPreview()
{
	m_fCreatedByClassName = FALSE;
	m_pPhotoPrjData = NULL;
	m_fWantSelect = FALSE;
	m_nSelectedPhoto = -1;
	m_nSelectedSubSection = -1;
}

CPhotoProjectPreview::~CPhotoProjectPreview()
{
}

BEGIN_MESSAGE_MAP(CPhotoProjectPreview, CWnd)
	//{{AFX_MSG_MAP(CPhotoProjectPreview)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectPreview message handlers

void CPhotoProjectPreview::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	BOOL fEraseBackground = FALSE;

	if (m_pPhotoPrjData != NULL)
	{
		CRect crClient;
		GetClientRect(&crClient);

		for (int x = 0; x < NUM_SUBSECTIONS; x++)
		{
			if(x == 0)
				fEraseBackground = TRUE;
			else
				fEraseBackground = FALSE;

			CPhotoLabelData* pData = m_pPhotoPrjData->m_pSubAreaData[x];
			pData->DrawPage(&dc, crClient, FALSE, &m_crDrawn, 0, fEraseBackground);
		}

		ShowSelection(&dc, TRUE);
	}
}

void CPhotoProjectPreview::SetPhotoProject(CPhotoPrjData* pPhotoPrjData)
{
	m_pPhotoPrjData = pPhotoPrjData;
	InvalidateRect(NULL);
}

void CPhotoProjectPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_fWantSelect)
	{
		TRACE("Clicked on point %d, %d\r\n", point.x, point.y);

		CFixedPoint Dims = m_pPhotoPrjData->m_pSubAreaData[0]->PaperDims();

		for (int x = 0; x < NUM_SUBSECTIONS; x++)
		{
			if((m_pPhotoPrjData->m_pSubAreaData[x]->SlotsAcross() != 0) &&
				 (m_pPhotoPrjData->m_pSubAreaData[x]->SlotsDown() != 0))
			{
		
				int nSlots = m_pPhotoPrjData->m_pSubAreaData[x]->Slots();

				CRect crDrawn = m_crDrawn;
				CFixed lWidth = MakeFixed(crDrawn.Width());
				CFixed lHeight = MakeFixed(crDrawn.Height());

				for (int nSlot = 0; nSlot < nSlots; nSlot++)
				{
					CFixedRect Bound = m_pPhotoPrjData->m_pSubAreaData[x]->SlotBounds(nSlot);

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
						SelectPhotoProject(x, nSlot);
						break;
					}
				}
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

void CPhotoProjectPreview::OnLButtonDblClk(UINT nFlags, CPoint point)
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

void CPhotoProjectPreview::SelectPhotoProject(int nSubSection, int nPhoto)
{
	if ((m_nSelectedPhoto != nPhoto) ||
		  (m_nSelectedSubSection != nSubSection))
	{
		ShowSelection(NULL, FALSE);
		m_nSelectedPhoto = nPhoto;
		m_nSelectedSubSection = nSubSection;
		ShowSelection(NULL, TRUE);
	}
}

void CPhotoProjectPreview::ShowSelection(CDC* pInDC, BOOL fHighlight)
{
	if (m_nSelectedPhoto != -1 && m_nSelectedSubSection >= 0 && m_nSelectedSubSection <= NUM_SUBSECTIONS && !m_crDrawn.IsRectEmpty())
	{
		CDC* pDC = pInDC;

		if (pDC == NULL)
		{
			pDC = GetDC();
		}

	/*
	// Compute where to draw the select.
	*/

		CFixedRect Bound = m_pPhotoPrjData->m_pSubAreaData[m_nSelectedSubSection]->SlotBounds(m_nSelectedPhoto);

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

		CFixedPoint Page = m_pPhotoPrjData->m_pSubAreaData[m_nSelectedSubSection]->PaperDims();
		BOOL fTractor = (m_pPhotoPrjData->m_pSubAreaData[m_nSelectedSubSection]->Type() == m_pPhotoPrjData->m_pSubAreaData[m_nSelectedSubSection]->PAPER_Continuous);

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

		m_pPhotoPrjData->m_pSubAreaData[m_nSelectedSubSection]->DrawSlot(pDC->GetSafeHdc(),
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

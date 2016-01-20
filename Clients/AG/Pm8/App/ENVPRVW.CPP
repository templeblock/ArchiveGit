/*
// $Workfile: ENVPRVW.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVPRVW.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:20   Fred
// Initial revision.
// 
//    Rev 1.1   31 Jul 1996 11:57:02   Fred
// 16-bit compatibility
// 
//    Rev 1.0   14 Mar 1996 13:33:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:22   FRED
// Initial revision.
// 
//    Rev 1.3   06 Oct 1995 15:23:18   FRED
// Win32 SendMessage() changes
// 
//    Rev 1.2   10 Aug 1995 13:22:42   JAY
//  
// 
//    Rev 1.1   08 Aug 1995 11:01:44   JAY
// Envelope stuff.
// 
//    Rev 1.0   28 Jul 1995 12:31:40   JAY
// Initial revision.
// 
//    Rev 1.0   20 Jul 1995 12:52:50   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"

#include "envprvw.h"
#include "printer.h"
#include "paper.h"
#include "util.h"

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define	COLOR_BTNHILIGHT	COLOR_BTNHIGHLIGHT
#endif
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePreview

WNDPROC CEnvelopePreview::m_pfnSuperWndProc = NULL;

LONG CALLBACK AFX_EXPORT CEnvelopePreview::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/
	
	CEnvelopePreview* pEnvelopePreview = new CEnvelopePreview;
	pEnvelopePreview->Attach(hWnd) ;
	
	pEnvelopePreview->m_fCreatedByClassName = TRUE ;
	
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

BOOL CEnvelopePreview::RegisterMyClass()
{
	WNDCLASS wcls;
	
	m_pfnSuperWndProc = ::DefWindowProc;
	
	static const char szClassName[] = "MlsEnvelopePreview";
	
	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CEnvelopePreview::FirstMsgOnlyWndProc);
	}
	
	wcls.style = CS_GLOBALCLASS | CS_DBLCLKS;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CEnvelopePreview::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;
	
	return (RegisterClass(&wcls) != 0);
}

void CEnvelopePreview::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

CEnvelopePreview::CEnvelopePreview()
{
	m_fCreatedByClassName = FALSE;

	m_wPosition = 0;
	m_fSelected = FALSE;
	m_fTractor = FALSE;
}

CEnvelopePreview::~CEnvelopePreview()
{
}

BEGIN_MESSAGE_MAP(CEnvelopePreview, CWnd)
	//{{AFX_MSG_MAP(CEnvelopePreview)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePreview message handlers

static WORD LaserIcon[] =
{
	IDI_ENV_TLD0,
	IDI_ENV_TCD0,
	IDI_ENV_TRD0,
	IDI_ENV_TLDN,
	IDI_ENV_TCDN,
	IDI_ENV_TRDN,
	IDI_ENV_TLU0,
	IDI_ENV_TCU0,
	IDI_ENV_TRU0,
	IDI_ENV_TLUN,
	IDI_ENV_TCUN,
	IDI_ENV_TRUN,
	IDI_ENV_WLD0,
	IDI_ENV_WCD0,
	IDI_ENV_WRD0,
	IDI_ENV_WLDN,
	IDI_ENV_WCDN,
	IDI_ENV_WRDN,
	IDI_ENV_WLU0,
	IDI_ENV_WCU0,
	IDI_ENV_WRU0,
	IDI_ENV_WLUN,
	IDI_ENV_WCUN,
	IDI_ENV_WRUN,
};

static WORD TractorIcon[] =
{
	IDI_EVD_TLD0,
	IDI_EVD_TCD0,
	IDI_EVD_TRD0,
	IDI_EVD_TLDN,
	IDI_EVD_TCDN,
	IDI_EVD_TRDN,
	IDI_EVD_TLU0,
	IDI_EVD_TCU0,
	IDI_EVD_TRU0,
	IDI_EVD_TLUN,
	IDI_EVD_TCUN,
	IDI_EVD_TRUN,
	IDI_EVD_WLD0,
	IDI_EVD_WCD0,
	IDI_EVD_WRD0,
	IDI_EVD_WLDN,
	IDI_EVD_WCDN,
	IDI_EVD_WRDN,
	IDI_EVD_WLU0,
	IDI_EVD_WCU0,
	IDI_EVD_WRU0,
	IDI_EVD_WLUN,
	IDI_EVD_WCUN,
	IDI_EVD_WRUN,
};

void CEnvelopePreview::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect crClient;
	GetClientRect(&crClient);

	Util::Draw3dBorder(
							 &dc,
							 crClient,
							 GetSysColor(COLOR_BTNHILIGHT),
							 GetSysColor(COLOR_BTNSHADOW),
							 GetSysColor(COLOR_BTNFACE),
							 GetSysColor(COLOR_BTNTEXT),
							 RGB(255,255,255),
							 2,
							 TRUE,
							 TRUE,
							 FALSE);

/*
// Construct the name for the icon.
*/

	int nIndex = 0;

	if ((m_wPosition & ENVELOPE_OMASK) == ENVELOPE_Wide)
	{
		nIndex += 12;
	}

	switch (m_wPosition & ENVELOPE_HMASK)
	{
		case ENVELOPE_Center:
		{
			nIndex++;
			break;
		}
		case ENVELOPE_Right:
		{
			nIndex += 2;
			break;
		}
		default:
		{
			break;
		}
	}

	if ((m_wPosition & ENVELOPE_FMASK) == ENVELOPE_FlapUp)
	{
		nIndex += 6;
	}

	if ((m_wPosition & ENVELOPE_RMASK) == ENVELOPE_RRotate)
	{
		nIndex += 3;
	}

/*
// Compute the dimensions of the client area.
*/

	int nClientWidth = crClient.Width();
	int nClientHeight = crClient.Height();

/*
// Get the dimensions of the icon.
*/

	int nWidth = GetSystemMetrics(SM_CXICON);
	int nHeight = GetSystemMetrics(SM_CYICON);

/*
// Draw the highlight if necessary.
*/
	if (m_fSelected)
	{
		int nHighlight = (nClientWidth - nWidth)/4;
		int nHighlightY = (nClientHeight - nHeight)/4;

		if (nHighlightY < nHighlight)
		{
			nHighlight = nHighlightY;
		}
		if (nHighlight < 3)
		{
			nHighlight = 3;
		}

		CRect crHighlight;
		crHighlight.left = crClient.left + nHighlight;
		crHighlight.top = crClient.top + nHighlight;
		crHighlight.right = crClient.right - nHighlight;
		crHighlight.bottom = crClient.bottom - nHighlight;

		CPen Pen;
		Pen.CreatePen(PS_SOLID, 2, ::GetSysColor(COLOR_HIGHLIGHT));
		CPen* pOldPen = dc.SelectObject(&Pen);

		if (pOldPen != NULL)
		{
			CBrush Brush;
			Brush.CreateStockObject(NULL_BRUSH);
			CBrush* pOldBrush = dc.SelectObject(&Brush);
			if (pOldBrush != NULL)
			{
				dc.Rectangle(crHighlight);
				dc.SelectObject(pOldBrush);
			}
			dc.SelectObject(pOldPen);
		}
	}

	HICON hIcon;
	WORD* pIcon = m_fTractor ? TractorIcon : LaserIcon;

	if ((hIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(pIcon[nIndex]))) != NULL)
	{
	/*
	// Compute the position of the icon.
	*/

		int x = crClient.left + (nClientWidth - nWidth)/2;
		int y = crClient.top + (nClientHeight - nHeight)/2;

	/*
	// Draw the icon.
	*/

		dc.DrawIcon(x, y, hIcon);
	}
}

void CEnvelopePreview::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef WIN32
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
#else
	GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID(), MAKELPARAM(GetSafeHwnd(), BN_CLICKED));
#endif
}

void CEnvelopePreview::OnLButtonDblClk(UINT nFlags, CPoint point)
{
#ifdef WIN32
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_DOUBLECLICKED), (LPARAM)GetSafeHwnd());
#else
	GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID(), MAKELPARAM(GetSafeHwnd(), BN_DOUBLECLICKED));
#endif
}

void CEnvelopePreview::Select(BOOL fSelected /*=TRUE*/)
{
	if (m_fSelected != fSelected)
	{
		m_fSelected = fSelected;
		Invalidate();
	}
}

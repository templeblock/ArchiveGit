/*
// $Workfile: PMWWIZ.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWWIZ.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 2     5/26/98 2:01p Dennis
// Added call to ReplaceDialogText().
// 
//    Rev 1.0   14 Aug 1997 15:24:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:58   Fred
// Initial revision.
// 
//    Rev 1.11   27 Aug 1996 08:46:14   Fred
// Selects palette into source DC for StretchBlt
// 
//    Rev 1.10   15 Aug 1996 15:55:46   Fred
// 256 color wizard bitmaps
// 
//    Rev 1.9   09 Aug 1996 17:05:00   Fred
// 16-bit: Only centers wizard once
// 
//    Rev 1.8   07 Aug 1996 10:38:16   Fred
// Fixed bug where scroll for 16-bit tab control was misplaced
// 
//    Rev 1.7   07 Aug 1996 10:17:20   Fred
// Fixed some tab control redraw bugs
// 
//    Rev 1.6   07 Aug 1996 08:42:16   Fred
//  
// 
//    Rev 1.5   06 Aug 1996 15:54:14   Fred
//  
// 
//    Rev 1.4   05 Aug 1996 18:55:30   Fred
//  
// 
//    Rev 1.3   05 Aug 1996 10:32:56   Fred
// 16-bit PMW classes
// 
//    Rev 1.2   01 Aug 1996 09:23:12   Fred
// More 16-bit changes
// 
//    Rev 1.1   31 Jul 1996 11:57:08   Fred
// 16-bit compatibility
// 
//    Rev 1.0   24 Jul 1996 18:24:36   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwwiz.h"
#include "util.h"
#ifndef WIN32
#include <afxpriv.h>
#include <auxdata.h>
#endif

#include "pmwcfg.h"


#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define	COLOR_BTNHILIGHT	COLOR_BTNHIGHLIGHT
#endif
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette *pOurPal;		// THE PrintMaster Palette (from PMWVIEW.CPP)

/////////////////////////////////////////////////////////////////////////////
// CPmwWizardBitmap

CPmwWizardBitmap::CPmwWizardBitmap()
{
}

CPmwWizardBitmap::~CPmwWizardBitmap()
{
}

BEGIN_MESSAGE_MAP(CPmwWizardBitmap, CWnd)
	//{{AFX_MSG_MAP(CPmwWizardBitmap)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPmwWizardBitmap::Initialize(UINT nID, CWnd* pParent)
{
	CWnd* pControl = pParent->GetDlgItem(nID);
	if (pControl != NULL)
	{
		CRect crControl;
		pControl->GetWindowRect(crControl);
		pParent->ScreenToClient(crControl);
		Create(NULL, "", pControl->GetStyle(), crControl, pParent, nID, NULL);
		SetWindowPos(pControl, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		pControl->DestroyWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPmwWizardBitmap message handlers

// OnPaint() is called when the bitmap needs to be repainted.

void CPmwWizardBitmap::OnPaint() 
{
	CPaintDC dc(this);
	DrawBitmap(&dc);
}

// DrawBitmap() is the main function for drawing the bitmap.

void CPmwWizardBitmap::DrawBitmap(CDC* pDC)
{
	CBitmap* pBitmap = ((CPmwWizardPage*)GetParent())->GetBitmap();
	if (pBitmap != NULL)
	{
		BITMAP Bitmap;
		if (Util::GetBitmap(pBitmap, &Bitmap))
		{
			// Create a bitmap which will be used to draw the image.
			// When the image is complete, it will be drawn onto the screen.
			CRect crContent;
			GetClientRect(crContent);

			CBitmap bmContent;
			bmContent.CreateCompatibleBitmap(pDC, crContent.Width(), crContent.Height());

			CDC dcContent;
			dcContent.CreateCompatibleDC(pDC);

			dcContent.SelectObject(&bmContent);

			CRect crBitmap(0, 0, Bitmap.bmWidth, Bitmap.bmHeight);

			CDC dcBitmap;
			dcBitmap.CreateCompatibleDC(pDC);
			dcBitmap.SelectObject(pBitmap);
			CPalette* pOldBitmapPal = dcBitmap.SelectPalette(pOurPal, FALSE);

			// Draw the bitmap onto the content bitmap.
			CPalette* pOldPal = dcContent.SelectPalette(pOurPal, FALSE);
			dcContent.RealizePalette();
			dcContent.StretchBlt(
				crContent.left,
				crContent.top,
				crContent.Width(),
				crContent.Height(),
				&dcBitmap,
				crBitmap.left,
				crBitmap.top,
				crBitmap.Width(),
				crBitmap.Height(),
				SRCCOPY);
			dcContent.SelectPalette(pOldPal, FALSE);
			dcContent.RealizePalette();

			// Draw the border onto the content.
			DrawBorder(dcContent, crContent);

			// Copy the content bitmap to the screen.
			pDC->BitBlt(
				crContent.left,
				crContent.top,
				crContent.Width(),
				crContent.Height(),
				&dcContent,
				0,
				0,
				SRCCOPY);

			dcContent.DeleteDC();
			bmContent.DeleteObject();

			dcBitmap.SelectPalette(pOldBitmapPal, FALSE);
			dcBitmap.DeleteDC();
		}
	}
}

void CPmwWizardBitmap::DrawBorder(CDC& dc, CRect& crRect)
{
	// Draw the sunken 3D border:
	//
	//    D-----------DW
	//    |B---------BL|
	//    ||          ||
	//    ||          ||
	//    |B          ||
	//    DL----------L|
	//    W------------W

	int nWidth = crRect.Width();
	int nHeight = crRect.Height();

	if ((nWidth >= 2) && (nHeight >= 2))
	{
		CRect crSide;

		// D horizontal
		crSide.SetRect(0, 0, nWidth-1, 1);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

		// D vertical
		crSide.SetRect(0, 1, 1, nHeight-1);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

		// B horizontal
		crSide.SetRect(1, 1, nWidth-2, 2);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

		// B vertical
		crSide.SetRect(1, 2, 2, nHeight-2);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

		// W horizontal
		crSide.SetRect(0, nHeight-1, nWidth, nHeight);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// W vertical
		crSide.SetRect(nWidth-1, 0, nWidth, nHeight-1);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// L horizontal
		crSide.SetRect(1, nHeight-2, nWidth-1, nHeight-1);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNFACE));

		// L vertical
		crSide.SetRect(nWidth-2, 1, nWidth-1, nHeight-2);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNFACE));
	}
}

BOOL CPmwWizardBitmap::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwWizardPage

CPmwWizardPage::CPmwWizardPage(UINT nIDTemplate, UINT nIDCaption /*=0*/) :
	PROPERTY_PAGE_CLASS(nIDTemplate, nIDCaption)
{
}

CPmwWizardPage::CPmwWizardPage(LPCTSTR lpszTemplateName, UINT nIDCaption /*=0*/) :
	PROPERTY_PAGE_CLASS(lpszTemplateName, nIDCaption)
{
}

CPmwWizardPage::~CPmwWizardPage()
{
}

BEGIN_MESSAGE_MAP(CPmwWizardPage, PROPERTY_PAGE_CLASS)
	//{{AFX_MSG_MAP(CPmwWizardPage)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Default GetBitmap().
CBitmap* CPmwWizard::GetBitmap(void)
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwWizardPage message handlers

BOOL CPmwWizardPage::OnInitDialog() 
{
	INHERITED::OnInitDialog();

	m_Bitmap.Initialize(IDC_BITMAP, this);		// Standard bitmap ID.

   GetConfiguration()->ReplaceDialogText(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPmwWizardPage::OnEraseBkgnd(CDC* pDC) 
{
	BOOL fResult = FALSE;
	
	if (pDC->SaveDC() != 0)
	{
		CWnd* pBitmap = GetDlgItem(IDC_BITMAP);
		if (pBitmap != NULL)
		{
			ASSERT(pBitmap == &m_Bitmap);
			CRect crBitmap;
			pBitmap->GetWindowRect(crBitmap);
			ScreenToClient(crBitmap);
			pDC->ExcludeClipRect(crBitmap);
		}
		
		fResult = INHERITED::OnEraseBkgnd(pDC);
		
		pDC->RestoreDC(-1);
	}
	
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwWizard

CPmwWizard::CPmwWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: PROPERTY_SHEET_CLASS(nIDCaption, pParentWnd, iSelectPage)
{
}

CPmwWizard::CPmwWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: PROPERTY_SHEET_CLASS(pszCaption, pParentWnd, iSelectPage)
{
}

CPmwWizard::~CPmwWizard()
{
}

void CPmwWizard::InitDialog(void)
{
}

BEGIN_MESSAGE_MAP(CPmwWizard, PROPERTY_SHEET_CLASS)
	//{{AFX_MSG_MAP(CPmwWizard)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CPmwWizard::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (INHERITED::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	// Call dummy init dialog function (works for both 16 and 32 bit.)
	InitDialog();
	
	return 0;
}

#ifndef WIN32

// The following code is a modified (hack-up) version of the DLGPROP.CPP module
// that ships with MSVC 1.52c. It tries to implement a reasonable subset of the
// WIN32 property sheet/page objects in wizard mode.

/////////////////////////////////////////////////////////////////////////////
// Property sheet global data

static HBITMAP hbmScroll;
static CSize sizeScroll;

// amount to inflate the selected tab
static const CSize sizeSelTab(2, 2);

// extra width & height for a tab past text width
static const CSize sizeTabTextMargin(10, 3);

void AFXAPI PmwWizardDeleteObject(HGDIOBJ* pObject)
{
	ASSERT(pObject != NULL);
	if (*pObject != NULL)
	{
		DeleteObject(*pObject);
		*pObject = NULL;
	}
}

struct _PMW_WIZARD_DLGPROP_TERM
{
	~_PMW_WIZARD_DLGPROP_TERM()
	{
		PmwWizardDeleteObject((HGDIOBJ*)&hbmScroll);
	}
};

static const _PMW_WIZARD_DLGPROP_TERM dlgpropTerm;

/////////////////////////////////////////////////////////////////////////////
// implementation helpers

static void AFXAPI SetCtrlFocus(HWND hWnd)
{
	if (::SendMessage(hWnd, WM_GETDLGCODE, 0, 0L) & DLGC_HASSETSEL)
		::SendMessage(hWnd, EM_SETSEL, 0, -1);
	::SetFocus(hWnd);
}

static void AFXAPI EnableDlgItem(HWND hWnd, UINT nID, BOOL bEnable)
{
	HWND hWndItem = ::GetDlgItem(hWnd, nID);
	if (hWndItem != NULL)
		::EnableWindow(hWndItem, bEnable);
}

static BOOL IsCharAfterAmpersand(LPTSTR lpsz, TCHAR chFind)
{
	ASSERT(AfxIsValidString(lpsz));

	AnsiLowerBuff(&chFind, 1);
	while (*lpsz != '\0')
	{
		if (*lpsz == '&')
		{
			++lpsz; // Note: '&' is not lead-byte
			if (*lpsz != '&')
			{
				TCHAR ch = *lpsz;
				AnsiLowerBuff(&ch, 1);
				return ch == chFind;
			}
		}
		lpsz = AnsiNext(lpsz);
	}
	return FALSE;
}

static HWND GetFirstLevelChild(HWND hWndDlg, HWND hWndLevel)
{
	if ((hWndLevel == hWndDlg) ||
		!(::GetWindowLong(hWndLevel,GWL_STYLE) & WS_CHILD))
	{
		return NULL;
	}

	HWND hWnd = hWndLevel;
	do
	{
		if (hWndLevel == hWndDlg)
			break;
		if (!(::GetWindowLong(hWndLevel,GWL_STYLE) & WS_CHILD))
			break;
		hWnd = hWndLevel;
	} while ((hWndLevel = ::GetParent(hWndLevel)) != NULL);

	return hWnd;
}

static HWND FindNextControl(HWND hWndDlg, HWND hWnd, TCHAR ch)
{
	ASSERT(hWndDlg != NULL);
	TCHAR szText[256];
	HWND hWndStart;
	HWND hWndFirst;
	DWORD dwDlgCode;

	// Check if we are in a group box so we can find local mnemonics.
	hWndStart = GetFirstLevelChild(hWndDlg, hWnd);
	hWndFirst = ::GetNextDlgGroupItem(hWndDlg, hWndStart, FALSE);
	hWndFirst = ::GetNextDlgGroupItem(hWndDlg, hWndFirst, TRUE);
	while ((hWndStart = ::GetNextDlgGroupItem(hWndDlg, hWndStart, FALSE)) != NULL)
	{
		if (hWndStart == hWnd || hWndStart == hWndFirst)
			break;

		// Only check for matching mnemonic if control doesn't want characters
		// and control isn't a static control with SS_NOPREFIX
		dwDlgCode = (DWORD) ::SendMessage(hWndStart, WM_GETDLGCODE, 0, 0L);
		if (!(dwDlgCode & DLGC_WANTCHARS) && (!(dwDlgCode & DLGC_STATIC) ||
			!(::GetWindowLong(hWndStart,GWL_STYLE)&& SS_NOPREFIX)))
		{
			::GetWindowText(hWndStart, szText, sizeof(szText));
			if (IsCharAfterAmpersand(szText, ch))
				return hWndStart;
		}
	}

	hWnd = hWndStart = GetFirstLevelChild(hWndDlg, hWnd);
	for (;;)
	{
		hWnd = ::GetWindow(hWnd,GW_HWNDNEXT);
		if (hWnd == NULL)
			hWnd = ::GetWindow(hWndDlg, GW_CHILD);

		// Only check for matching mnemonic if control doesn't want characters
		// and control isn't a static control with SS_NOPREFIX
		dwDlgCode = (DWORD) ::SendMessage(hWnd, WM_GETDLGCODE, 0, 0L);
		if (!(dwDlgCode & DLGC_WANTCHARS) && (!(dwDlgCode & DLGC_STATIC) ||
			!(::GetWindowLong(hWnd,GWL_STYLE) & SS_NOPREFIX)))
		{
			::GetWindowText(hWnd, szText, sizeof(szText));
			if (IsCharAfterAmpersand(szText, ch))
				break;
		}

		if (hWnd == hWndStart)
			return NULL;
	}

	return hWnd;
}

static BOOL PASCAL WalkPreTranslateTree(HWND hWndStop, MSG* pMsg)
{
	ASSERT(hWndStop == NULL || ::IsWindow(hWndStop));
	ASSERT(pMsg != NULL);

	// walk from the target window up to the hWndStop window checking
	//  if any window wants to translate this message

	for (HWND hWnd = pMsg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd))
	{
		CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
		if (pWnd != NULL)
		{
			// target window is a C++ window
			if (pWnd->PreTranslateMessage(pMsg))
				return TRUE; // trapped by target window (eg: accelerators)
		}

		// got to hWndStop window without interest
		if (hWnd == hWndStop)
			break;
	}
	return FALSE;       // no special processing
}

////////////////////////////////////////////////////////////////////////////
// CPmwWizardTabItem

// CMlsTabItem represents one graphical tab
CPmwWizardTabItem::CPmwWizardTabItem(LPCTSTR szCaption, int nWidth)
{
	ASSERT(AfxIsValidString(szCaption));
	m_strCaption = szCaption;
	m_nWidth = nWidth;
	m_rect.SetRectEmpty();
	m_rectPrev.SetRectEmpty();
}

void CPmwWizardTabItem::Draw(CDC* pDC, HFONT hFont, BOOL bCurTab)
{
	CRect rectItem = m_rect;
	BOOL bClipped = (rectItem.Width() < m_nWidth);

	if (bCurTab)
		rectItem.InflateRect(sizeSelTab.cx, sizeSelTab.cy);

	HPEN hOldPen = (HPEN)pDC->SelectObject(afxData.hpenBtnHilite);

	pDC->MoveTo(rectItem.left, rectItem.bottom - 1);
	pDC->LineTo(rectItem.left, rectItem.top + 2);
	pDC->LineTo(rectItem.left + 2, rectItem.top);
	pDC->LineTo(rectItem.right - 1, rectItem.top);

	pDC->SelectObject(afxData.hpenBtnShadow);
	if (!bClipped)
	{
		// Draw dark gray line down right side
		pDC->LineTo(rectItem.right - 1, rectItem.bottom);

		// Draw black line down right side
		pDC->SelectObject(afxData.hpenBtnText);
		pDC->MoveTo(rectItem.right, rectItem.top + 2);
		pDC->LineTo(rectItem.right, rectItem.bottom);
	}
	else
	{
		// draw dark gray "torn" edge for a clipped tab
		for (int i = rectItem.top ; i < rectItem.bottom ; i += 3)
		{
			// This nifty (but obscure-looking) equation will draw
			// a jagged-edged line.
			int j = ((6 - (i - rectItem.top) % 12) / 3) % 2;
			pDC->MoveTo(rectItem.right + j, i);
			pDC->LineTo(rectItem.right + j, min(i + 3, rectItem.bottom));
		}
	}

	// finally, draw the tab's text
	HFONT hOldFont = NULL;
	if (hFont != NULL)
		hOldFont = (HFONT)::SelectObject(pDC->m_hDC, hFont);

	pDC->SelectObject(afxData.hpenBtnText);

	CSize text = pDC->GetTextExtent(m_strCaption, m_strCaption.GetLength());

	pDC->ExtTextOut(m_rect.left + (bClipped ? m_nWidth : m_rect.Width())/2 -
		text.cx/2, rectItem.top + rectItem.Height()/2 - text.cy/2,
		ETO_CLIPPED, &rectItem, m_strCaption, m_strCaption.GetLength(), NULL);

	if (hOldPen != NULL)
		pDC->SelectObject(hOldPen);
	if (hOldFont != NULL)
		pDC->SelectObject(hOldFont);
}

////////////////////////////////////////////////////////////////////////////
// CPmwWizardPropertyPage -- one page of a tabbed dialog

BEGIN_MESSAGE_MAP(CPmwWizardPropertyPage, CDialog)
	//{{AFX_MSG_MAP(CPmwWizardPropertyPage)
	ON_WM_CTLCOLOR()
	ON_WM_NCCREATE()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPmwWizardPropertyPage::CPmwWizardPropertyPage(UINT nIDTemplate, UINT nIDCaption)
{
	ASSERT(nIDTemplate != NULL);
	CommonConstruct(MAKEINTRESOURCE(nIDTemplate), nIDCaption);
}

CPmwWizardPropertyPage::CPmwWizardPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption)
{
	ASSERT(AfxIsValidString(lpszTemplateName));
	CommonConstruct(lpszTemplateName, nIDCaption);
}

void CPmwWizardPropertyPage::CommonConstruct(LPCTSTR lpszTemplateName, UINT nIDCaption)
{
	m_lpDialogTemplate = lpszTemplateName;
	if (nIDCaption != 0)
		VERIFY(m_strCaption.LoadString(nIDCaption));
	else
		LoadCaption();

	m_bChanged = FALSE;
}

CPmwWizardPropertyPage::~CPmwWizardPropertyPage()
{
}

void CPmwWizardPropertyPage::SetModified(BOOL bChanged)
{
	m_bChanged = bChanged;
	CPmwWizardPropertySheet* pSheet = (CPmwWizardPropertySheet*)m_pParentWnd;
	ASSERT(pSheet != NULL);
	ASSERT(pSheet->IsKindOf(RUNTIME_CLASS(CPmwWizardPropertySheet)));
	pSheet->PageChanged();
}

LRESULT CPmwWizardPropertyPage::OnWizardBack(void)
{
	return 0;
}

LRESULT CPmwWizardPropertyPage::OnWizardNext(void)
{
	return 0;
}

BOOL CPmwWizardPropertyPage::OnWizardFinish(void)
{
	return TRUE;
}
	
void CPmwWizardPropertyPage::OnOK()
{
	ASSERT_VALID(this);
	m_bChanged = FALSE;

	Default();  // do not call CDialog::OnOK as it will call EndDialog
}

void CPmwWizardPropertyPage::OnCancel()
{
	ASSERT_VALID(this);
	m_bChanged = FALSE;

	Default();  // do not call CDialog::OnOK as it will call EndDialog
}

BOOL CPmwWizardPropertyPage::OnKillActive()
{
	ASSERT_VALID(this);

	// override this to perform validation;
	//  return FALSE and this page will remain active...
	if (!UpdateData(TRUE))
	{
		TRACE0("UpdateData failed during page deactivation\n");
		// UpdateData will set focus to correct item
		return FALSE;
	}
	return TRUE;
}

void CPmwWizardPropertyPage::CancelToClose()
{
	ASSERT_VALID(this);
	CPmwWizardPropertySheet* pSheet = (CPmwWizardPropertySheet*)m_pParentWnd;
	ASSERT(pSheet != NULL);
	ASSERT(pSheet->IsKindOf(RUNTIME_CLASS(CPmwWizardPropertySheet)));
	pSheet->CancelToClose();
}

BOOL CPmwWizardPropertyPage::ProcessTab(MSG* /*pMsg*/)
{
	// Handle tabbing back into the property sheet when tabbing away from
	// either end of the dialog's tab order
	if (GetKeyState(VK_CONTROL) < 0)
		return FALSE;

	BOOL bShift = GetKeyState(VK_SHIFT) < 0;
	if ((::SendMessage(::GetFocus(), WM_GETDLGCODE, 0, 0) &
		(DLGC_WANTALLKEYS | DLGC_WANTMESSAGE | DLGC_WANTTAB)) == 0)
	{
		HWND hWndFocus = ::GetFocus();
		HWND hWndCtl = hWndFocus;
		if (::IsChild(m_hWnd, hWndCtl))
		{
			do
			{
				HWND hWndParent = ::GetParent(hWndCtl);
				ASSERT(hWndParent != NULL);
				static const TCHAR szComboBox[] = _T("combobox");
				TCHAR szCompare[sizeof(szComboBox)+1];
				::GetClassName(hWndParent, szCompare, sizeof(szCompare));

				int nCmd = bShift ? GW_HWNDPREV : GW_HWNDNEXT;
				if (lstrcmpi(szCompare, szComboBox) == 0)
					hWndCtl = ::GetWindow(hWndParent, nCmd);
				else
					hWndCtl = ::GetWindow(hWndCtl, nCmd);

				if (hWndCtl == NULL)
				{
					SetCtrlFocus(::GetNextDlgTabItem(m_pParentWnd->m_hWnd,
						m_hWnd, bShift));
					return TRUE; // handled one way or the other
				}
			}
			while ((::GetWindowLong(hWndCtl, GWL_STYLE) &
				(WS_DISABLED | WS_TABSTOP)) != WS_TABSTOP);
		}
	}
	return FALSE;
}

BOOL CPmwWizardPropertyPage::PreTranslateKeyDown(MSG* pMsg)
{
	CPmwWizardPropertySheet* pSheet = (CPmwWizardPropertySheet*)m_pParentWnd;
	ASSERT(pSheet->IsKindOf(RUNTIME_CLASS(CPmwWizardPropertySheet)));

	ASSERT(pMsg->message == WM_KEYDOWN);
	DWORD dwDlgCode = ::SendMessage(::GetFocus(), WM_GETDLGCODE, 0, (LPARAM)pMsg);
	
	if (dwDlgCode & DLGC_WANTMESSAGE)
	{
		return FALSE;
	}

	if (pMsg->wParam == VK_TAB)
	{
		if (dwDlgCode & DLGC_WANTTAB)
			return FALSE;

		// handle tab key
		if (ProcessTab(pMsg))
			return TRUE;
	}
	else if (pMsg->wParam == VK_RETURN && pSheet->m_hWndDefault == NULL)
	{
		if (dwDlgCode & DLGC_WANTALLKEYS)
			return FALSE;

		// handle return key
		m_pParentWnd->PostMessage(WM_KEYDOWN, VK_RETURN, pMsg->lParam);
		return TRUE;
	}
	else if (pMsg->wParam == VK_ESCAPE)
	{
		if (dwDlgCode & DLGC_WANTALLKEYS)
			return FALSE;

		// escape key handled
		m_pParentWnd->PostMessage(WM_KEYDOWN, VK_ESCAPE, pMsg->lParam);
		return TRUE;
	}
	return FALSE;
}

BOOL CPmwWizardPropertyPage::PreTranslateMessage(MSG* pMsg)
{
	HWND hFocusBefore = ::GetFocus();

	CPmwWizardPropertySheet* pSheet = (CPmwWizardPropertySheet*)m_pParentWnd;
	ASSERT(pSheet->IsKindOf(RUNTIME_CLASS(CPmwWizardPropertySheet)));

	// special case for VK_RETURN and "edit" controls with ES_WANTRETURN
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		static const TCHAR szEdit[] = _T("edit");
		TCHAR szCompare[sizeof(szEdit)+1];

		::GetClassName(hFocusBefore, szCompare, sizeof(szCompare));
		if (lstrcmpi(szCompare, szEdit) == 0 &&
			(::GetWindowLong(hFocusBefore, GWL_STYLE) & ES_WANTRETURN))
		{
			::SendMessage(hFocusBefore, WM_CHAR, '\n', 0);
			return TRUE;
		}
	}

	// otherwise check for special accelerators
	BOOL bResult;
	if (pMsg->message == WM_KEYDOWN && PreTranslateKeyDown(pMsg))
		bResult = TRUE;
	else
		bResult = pSheet->PreTranslateMessage(pMsg);

	// if focus changed, make sure buttons are set correctly
	HWND hFocusAfter = ::GetFocus();
	if (hFocusBefore != hFocusAfter)
		pSheet->CheckDefaultButton(hFocusBefore, hFocusAfter);

	return bResult;
}

#pragma pack(1)
struct DLGTEMPLATE
{
	DWORD lStyle;
	BYTE  bNumberOfItems;
	WORD  x;
	WORD  y;
	WORD  cx;
	WORD  cy;
};
#pragma pack()

void CPmwWizardPropertyPage::LoadCaption()
{
	HINSTANCE hInst = AfxFindResourceHandle(m_lpDialogTemplate, RT_DIALOG);
	ASSERT(hInst != NULL);
	HRSRC hResource = ::FindResource(hInst, m_lpDialogTemplate, RT_DIALOG);
	ASSERT(hResource != NULL);
	HGLOBAL hTemplate = ::LoadResource(hInst, hResource);
	ASSERT(hTemplate != NULL);

	// resources don't have to be freed or unlocked in Win32
	DLGTEMPLATE FAR* pDlgTemplate =
		(DLGTEMPLATE FAR*)::LockResource(hTemplate);
	ASSERT(pDlgTemplate != NULL);
	LPCSTR p = (LPCSTR)((BYTE FAR*)pDlgTemplate + sizeof(DLGTEMPLATE));
	// skip menu stuff
	p+= (*p == 0xffff) ? 2 : lstrlen(p)+1;
	// skip window class stuff
	p+= (*p == 0xffff) ? 2 : lstrlen(p)+1;
	// we're now at the caption
	m_strCaption = p;
}

BOOL CPmwWizardPropertyPage::CreatePage()
{
	if (!Create(m_lpDialogTemplate, m_pParentWnd))
		return FALSE; // Create() failed...

	// Must be a child for obvious reasons, and must be disabled to prevent
	// it from taking the focus away from the tab area during initialization...
	ASSERT((GetStyle() & (WS_DISABLED | WS_CHILD)) == (WS_DISABLED | WS_CHILD));

	return TRUE;    // success
}

BOOL CPmwWizardPropertyPage::OnSetActive()
{
	if (m_hWnd == NULL)
	{
		if (!CreatePage())
			return FALSE;

		ASSERT(m_hWnd != NULL);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwWizardPropertyPage Message Handlers

BOOL CPmwWizardPropertyPage::OnNcCreate(LPCREATESTRUCT lpcs)
{
	DWORD dwStyle = GetStyle();
	dwStyle &= ~(WS_CAPTION|WS_BORDER);
	dwStyle |= WS_GROUP|WS_TABSTOP|WS_CLIPSIBLINGS;
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	return CDialog::OnNcCreate(lpcs);
}

int CPmwWizardPropertyPage::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CDialog::OnCreate(lpcs) == -1)
		return -1;

	CRect rect;
	GetWindowRect(&rect);

	return 0;
}

HBRUSH CPmwWizardPropertyPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	LRESULT lResult;
	if (pWnd->SendChildNotifyLastMsg(&lResult))
		return (HBRUSH)lResult;

	if (!GrayCtlColor(pDC->m_hDC, pWnd->GetSafeHwnd(), nCtlColor,
	  afxData.hbrBtnFace, afxData.clrBtnText))
		return (HBRUSH)Default();
	return afxData.hbrBtnFace;
}

void CPmwWizardPropertyPage::OnClose()
{
	GetParent()->PostMessage(WM_CLOSE);
}

////////////////////////////////////////////////////////////////////////////
// CPmwWizardTabControl -- implementation of tabs along the top of dialog

BEGIN_MESSAGE_MAP(CPmwWizardTabControl, CWnd)
	//{{AFX_MSG_MAP(CPmwWizardTabControl)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPmwWizardTabControl::CPmwWizardTabControl()
{
	m_rectScroll.SetRectEmpty();
	m_nCurTab = -1;
	m_nFirstTab = -1;
	m_nScrollState = SCROLL_NULL;
	m_bInSize = FALSE;
	m_hBoldFont = m_hThinFont = NULL;

	if (hbmScroll == NULL)
	{
		// Note: If this LoadBitmap call fails, it is likely that
		//  _AFX_NO_PROPERTY_RESOURCES is defined in your .RC file.
		// To correct the situation, remove the following line from your
		//  resource script:
		//      #define _AFX_NO_PROPERTY_RESOURCES
		// This should be done using the Resource.Set Includes... command.

		// all bitmaps must live in the same module
		HINSTANCE hInst =
			AfxFindResourceHandle(MAKEINTRESOURCE(AFX_IDB_SCROLL), RT_BITMAP);
		VERIFY(hbmScroll =
			LoadBitmap(hInst, MAKEINTRESOURCE(AFX_IDB_SCROLL)));

		BITMAP bmStruct;
		VERIFY(GetObject(hbmScroll, sizeof(BITMAP), &bmStruct));
		sizeScroll.cx = bmStruct.bmWidth / 5;
		sizeScroll.cy = bmStruct.bmHeight;
	}
}

CPmwWizardTabControl::~CPmwWizardTabControl()
{
	for (int i = 0 ; i < GetItemCount() ; i++)
		delete (CPmwWizardTabItem*)m_tabs[i];
	PmwWizardDeleteObject((HGDIOBJ*)&m_hBoldFont);
	PmwWizardDeleteObject((HGDIOBJ*)&m_hThinFont);
}

BOOL CPmwWizardTabControl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
	UINT nID)
{
	// create a new window class without CS_DBLCLKS
	return CreateEx(0,
		AfxRegisterWndClass(CS_VREDRAW|CS_HREDRAW,
		LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_BTNFACE+1)),
		NULL, dwStyle, rect.left, rect.top, rect.right-rect.left,
		rect.bottom-rect.top, pParentWnd->GetSafeHwnd(), (HMENU)nID);
}

void CPmwWizardTabControl::AddTab(LPCTSTR lpszCaption)
{
	m_tabs.Add(new CPmwWizardTabItem(lpszCaption, -1));
	if (m_nCurTab == -1)
	{
		m_nCurTab = 0;
		m_nFirstTab = 0;
	}
	if (m_hWnd != NULL)
	{
		SetFirstTab(m_nFirstTab);
		Invalidate();
	}
}

void CPmwWizardTabControl::RemoveTab(int nTab)
{
	// remove the tab item
	delete (CPmwWizardTabItem*)m_tabs[nTab];
	m_tabs.RemoveAt(nTab);

	// adjust internal indices
	if (m_nCurTab > nTab)
		--m_nCurTab;
	if (m_nCurTab >= GetItemCount())
		m_nCurTab = 0;
	if (m_hWnd != NULL)
	{
		SetFirstTab(0);
		Invalidate();
	}
}

void CPmwWizardTabControl::OnSetFocus(CWnd* /*pOldWnd*/)
{
	DrawFocusRect();
}

void CPmwWizardTabControl::OnKillFocus(CWnd* /*pNewWnd*/)
{
	DrawFocusRect();
}

UINT CPmwWizardTabControl::OnGetDlgCode()
{
	return CWnd::OnGetDlgCode() | DLGC_WANTARROWS;
}

void CPmwWizardTabControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_LEFT || nChar == VK_RIGHT)
		NextTab(nChar==VK_RIGHT); //
	else
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CPmwWizardTabControl::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	HBRUSH hBrush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLOR,
		(WPARAM)pDC->m_hDC, MAKELONG(GetParent()->m_hWnd, CTLCOLOR_DLG));
	::FillRect(pDC->m_hDC, &rect, hBrush);
	return TRUE;
}

void CPmwWizardTabControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	m_rectScroll.SetRect(cx - sizeScroll.cx, m_nHeight-1-sizeScroll.cy, cx, m_nHeight-1);
	if (!m_bInSize)
	{
		m_bInSize = TRUE;
		SetFirstTab(m_nFirstTab);   // recalc all tab positions
		ScrollIntoView(m_nCurTab);  // make sure current selection still in view
		m_bInSize = FALSE;
	}
}

int CPmwWizardTabControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// initialize fonts to fonts derived from parent window
	ASSERT(::GetParent(m_hWnd) != NULL);
	if (m_hBoldFont != NULL)
		return 0;

	m_hBoldFont = (HFONT)::SendMessage(::GetParent(m_hWnd), WM_GETFONT, 0, 0);
	if (m_hBoldFont != NULL)
	{
		LOGFONT lf;
		VERIFY(GetObject(m_hBoldFont, sizeof(LOGFONT), &lf));
		lf.lfWeight = FW_BOLD;
		m_hBoldFont = CreateFontIndirect(&lf);
		ASSERT(m_hBoldFont != NULL);
		lf.lfWeight = FW_LIGHT;
		m_hThinFont = CreateFontIndirect(&lf);
		ASSERT(m_hThinFont != NULL);
	}
	return 0;
}

BOOL CPmwWizardTabControl::NextTab(BOOL bNext)
{
	return SetCurSel((m_nCurTab + (bNext ? 1 : -1) + GetItemCount()) % GetItemCount(), TRUE);
}

BOOL CPmwWizardTabControl::SetCurSel(int nTab, BOOL fNotify /*=FALSE*/)
{
	if (nTab == m_nCurTab)
	{
		// if tab is not completely visible
		if (!IsTabVisible(m_nCurTab, TRUE))
			ScrollIntoView(m_nCurTab);
		return TRUE;
	}

	// attempt to switch to new tab
	if (GetParent()->SendMessage(TCM_TABCHANGING, 0, 0) == 0)
	{
		// succesful switch, send notify and invalidate tab control
		int nOldTab = m_nCurTab;
		m_nCurTab = nTab;
		if (fNotify)
		{
			GetParent()->SendMessage(TCM_TABCHANGED, 0, 0);
		}
		InvalidateTab(nOldTab);
		InvalidateTab(m_nCurTab);
		ScrollIntoView(m_nCurTab);
		return TRUE;
	}
	return FALSE;
}

BOOL CPmwWizardTabControl::IsTabVisible(int nTab, BOOL bComplete) const
{
	BOOL bResult = FALSE;
	if ((nTab >= 0) && (nTab < GetItemCount()))
	{
		CPmwWizardTabItem* pItem = GetTabItem(nTab);
		if (pItem->m_rect.IsRectNull())
			bResult = FALSE;
		else if (pItem->m_rect.Width() >= pItem->m_nWidth)
			bResult = TRUE;
		else// partially visible
			bResult = !bComplete;
	}
	return bResult;
}

void CPmwWizardTabControl::OnPaint()
{
	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);

	// Draw all the tabs that are currently within view
	for (int i = 0 ; i < GetItemCount() ; i++)
	{
		if (IsTabVisible(i) && (i != m_nCurTab))
			GetTabItem(i)->Draw(&dc, m_hThinFont, FALSE);
	}
	// Draw the current tab last so that it gets drawn on "top"
	if (m_nCurTab != -1)
	{
		GetTabItem(m_nCurTab)->Draw(&dc, m_hBoldFont, TRUE);
	}

	// Draw the line underneath all the tabs
	CRect rectItem;
	rectItem.SetRectEmpty();
	if (m_nCurTab != -1)
	{
		rectItem = GetTabItem(m_nCurTab)->m_rect;
	}
	
	HPEN hOldPen = (HPEN)dc.SelectObject(afxData.hpenBtnHilite);
	
	CRect rect;
	GetClientRect(&rect);
	
	dc.MoveTo(rect.left, m_nHeight - 1);
	if (!rectItem.IsRectNull())
	{
		// this leaves a gap in the line if the currently selected
		// tab is within view.
		dc.LineTo(rectItem.left - sizeSelTab.cx, m_nHeight - 1);
		dc.MoveTo(rectItem.right + sizeSelTab.cx + 1, m_nHeight - 1);
	}
	dc.LineTo(rect.right-1, m_nHeight - 1);
	
	dc.SelectObject(afxData.hpenBtnText);
	dc.LineTo(rect.right-1, rect.bottom-1);
	dc.LineTo(rect.left-1, rect.bottom-1);

	dc.SelectObject(afxData.hpenBtnHilite);
	dc.MoveTo(rect.left, rect.bottom-2);
	dc.LineTo(rect.left, m_nHeight-1);

	dc.SelectObject(afxData.hpenBtnShadow);
	dc.MoveTo(rect.right-2, m_nHeight);
	dc.LineTo(rect.right-2, rect.bottom-2);
	dc.LineTo(rect.left, rect.bottom-2);

	if (hOldPen != NULL)
		dc.SelectObject(hOldPen);

	if (CanScroll())
		DrawScrollers(&dc);

	if (GetFocus() == this)
		DrawFocusRect(&dc);
}

void CPmwWizardTabControl::DrawFocusRect(CDC* pDC)
{
	if (!IsWindowVisible())
		return;

	if (IsTabVisible(m_nCurTab))
	{
		// obtain usable DC for drawing
		CDC* pTempDC = NULL;
		if (pDC == NULL)
		{
			pDC = pTempDC = GetDC();
			GetParent()->SendMessage(WM_CTLCOLOR,
				(WPARAM)pDC->m_hDC, MAKELONG(GetParent()->m_hWnd, CTLCOLOR_DLG));
		}
	
		// draw and cleanup
		pDC->DrawFocusRect(GetTabItem(m_nCurTab)->m_rect);
		if (pTempDC != NULL)
			ReleaseDC(pTempDC);
	}
}

void CPmwWizardTabControl::DrawScrollers(CDC* pDC)
{
	ASSERT(pDC != NULL);

	// Choose image bitmap depending on scroll state
	int iImage = 0;

	// Choose bitmap depending on scroll state
	if (IsTabVisible(0, TRUE))
		iImage = 2;
	else if (IsTabVisible(GetItemCount()-1, TRUE))
		iImage = 4;

	if (!m_bScrollPause)
	{
		if (m_nScrollState == SCROLL_LEFT)
			iImage = 1;
		else if (m_nScrollState == SCROLL_RIGHT)
			iImage = 3;
	}

	CDC dcTemp;
	dcTemp.CreateCompatibleDC(pDC);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(dcTemp.m_hDC, hbmScroll);

	pDC->BitBlt(m_rectScroll.left, m_rectScroll.top, m_rectScroll.Width(),
		m_rectScroll.Height(), &dcTemp, iImage * sizeScroll.cx, 0, SRCCOPY);

	::SelectObject(dcTemp.m_hDC, hbmOld);
}

void CPmwWizardTabControl::LayoutTabs(int nTab)
{
	// This function recalcs the positions of all the tabs, assuming the
	// specified tab is the first (leftmost) visible tab.

	ASSERT(nTab >= 0 && nTab < GetItemCount());

	CPmwWizardTabItem* pItem = NULL;
	int x = sizeSelTab.cx;
	CRect rectClient;
	GetClientRect(&rectClient);

	CClientDC dc(NULL); // could occur before creation
	HFONT hOldFont = NULL;
	if (m_hThinFont != NULL)
		hOldFont = (HFONT)::SelectObject(dc.m_hDC, m_hThinFont);

	m_nHeight = dc.GetTextExtent(_T("M"), 1).cy +
		sizeTabTextMargin.cy * 2 + sizeSelTab.cy * 2;

	for (int i = 0; i < GetItemCount(); i++)
	{
		pItem = GetTabItem(i);
		if (pItem->m_nWidth < 0)
		{
			CSize text = dc.GetTextExtent(pItem->m_strCaption,
				pItem->m_strCaption.GetLength());
			pItem->m_nWidth = text.cx + sizeTabTextMargin.cx * 2;
		}
		// everything before the first tab is not visible
		if (i<nTab)
			pItem->m_rect.SetRectEmpty();
		// calculate locations for all other tabs
		else
		{
			pItem->m_rect.SetRect(x, sizeSelTab.cy,
				x + pItem->m_nWidth, m_nHeight-sizeSelTab.cy);
			x += pItem->m_nWidth + sizeSelTab.cx;
		}
	}

	// do they all fit?
	if (m_tabs.GetSize() > 0)
	{
		pItem = GetTabItem(m_tabs.GetSize()-1);
		x = rectClient.right - (sizeScroll.cx/3 + sizeScroll.cx);
		if (pItem->m_rect.right > ((nTab==0) ? rectClient.right : x))
		{
			int i = m_tabs.GetSize();
			while (i-- > 0)
			{
				pItem = GetTabItem(i);
				if (pItem->m_rect.left > x)
					pItem->m_rect.SetRectEmpty();
				else
				{
					if (pItem->m_rect.right > x)
						pItem->m_rect.right = x;
					break;
				}
			}
		}
	}

	if (hOldFont != NULL)
		::SelectObject(dc.m_hDC, hOldFont);
}

void CPmwWizardTabControl::SetFirstTab(int nTab)
{
	ASSERT(m_hWnd != NULL);
	LayoutTabs(nTab);
	m_nFirstTab = nTab;
}

void CPmwWizardTabControl::Scroll(int nDirection)
{
	ASSERT(nDirection == SCROLL_LEFT || nDirection == SCROLL_RIGHT);
	ASSERT(CanScroll());

	switch (nDirection)
	{
	case SCROLL_LEFT:
		if (IsTabVisible(0))
			return;
		SetFirstTab(m_nFirstTab - 1);
		break;
	case SCROLL_RIGHT:
		if (IsTabVisible(GetItemCount()-1, TRUE))
			return;
		SetFirstTab(m_nFirstTab + 1);
		break;
	}

	// repaint everything except the scroll btns
	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.right = m_rectScroll.left - 1;
	InvalidateRect(&rectClient);
}

void CPmwWizardTabControl::ScrollIntoView(int nTab)
{
	ASSERT((nTab >= 0) && (nTab < GetItemCount()));
	if ((nTab >= 0) && (nTab < GetItemCount()))
	{
		int nOldFirstTab = m_nFirstTab;
		// do we need to scroll left or right?
		int nIncrement = (nTab > m_nFirstTab) ? 1 : -1;
		// scroll over until completely visible or until the desired tab is
		// the first tab.  This handles the case where a tab is bigger than
		// the window
		while (!IsTabVisible(nTab, TRUE) && nTab != m_nFirstTab)
			SetFirstTab(m_nFirstTab + nIncrement);
		// if the same first tab we haven't moved so don't invalidate
		if (nOldFirstTab != m_nFirstTab)
			Invalidate();
	}
}

BOOL CPmwWizardTabControl::CanScroll()
{
	// if either the first or the last tab is not visible, it's scrollable
	return (GetItemCount() > 0) && (!IsTabVisible(0) || !IsTabVisible(GetItemCount()-1, TRUE));
}

void CPmwWizardTabControl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (this == GetCapture())
	{
		ASSERT(m_nScrollState == SCROLL_LEFT || m_nScrollState == SCROLL_RIGHT);

		int nNewState = TabFromPoint(point);
		BOOL bPause = !(nNewState == m_nScrollState);

		if (bPause == m_bScrollPause)
			return;

		if (bPause)
			KillTimer(TIMER_ID);
		else
		{
			VERIFY(SetTimer(TIMER_ID, TIMER_DELAY, NULL) == TIMER_ID);
			Scroll(m_nScrollState);
		}

		m_bScrollPause = bPause;
		InvalidateTab(m_nScrollState);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CPmwWizardTabControl::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	Capture(SCROLL_NULL);
}

void CPmwWizardTabControl::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
	int nTab = TabFromPoint(point);
	switch (nTab)
	{
	case -1:
		break;

	case SCROLL_LEFT:
		if (!IsTabVisible(0))
		{
			Scroll(nTab);
			Capture(nTab);
		}
		break;
	case SCROLL_RIGHT:
		if (!IsTabVisible(GetItemCount()-1, TRUE))
		{
			Scroll(nTab);
			Capture(nTab);
		}
		break;

	default:
		ASSERT(nTab >= 0);
		SetCurSel(nTab, TRUE);    // this will check to make sure switch is ok
		break;
	}
	SetFocus();
}

void CPmwWizardTabControl::OnTimer(UINT nTimerID)
{
	if (nTimerID == CPmwWizardTabControl::TIMER_ID)
	{
		ASSERT((m_nScrollState == SCROLL_LEFT) || (m_nScrollState == SCROLL_RIGHT));
		ASSERT(!m_bScrollPause);
		Scroll(m_nScrollState);
	}
}

void CPmwWizardTabControl::Capture(int nDirection)
{
	ASSERT((m_nScrollState == SCROLL_LEFT) ||
		   (m_nScrollState == SCROLL_RIGHT) ||
		   (m_nScrollState == SCROLL_NULL));

	switch (nDirection)
	{
	case SCROLL_LEFT:
	case SCROLL_RIGHT:
		SetCapture();
		VERIFY(SetTimer(TIMER_ID, TIMER_DELAY, NULL) == TIMER_ID);
		InvalidateTab(nDirection);
		break;

	case SCROLL_NULL:
		::ReleaseCapture();
		KillTimer(TIMER_ID);
		InvalidateTab(m_nScrollState);
		break;

	default:
		ASSERT(FALSE);
		break;
	}

	m_nScrollState = nDirection;
	m_bScrollPause = FALSE;
}

void CPmwWizardTabControl::InvalidateTab(int nTab, BOOL bInflate)
{
	CRect rect;
	switch (nTab)
	{
	case -1:
		GetClientRect(rect);
		rect.bottom = m_nHeight;
		break;
			
	case SCROLL_NULL:
		rect.SetRectEmpty();
		break;
	
	case SCROLL_LEFT:
	case SCROLL_RIGHT:
		rect = m_rectScroll;
		break;
	
	default:
		if ((nTab >= 0) && (nTab < GetItemCount()))
		{
			rect = GetTabItem(nTab)->m_rect;
			if (bInflate)
			{
				rect.InflateRect(sizeSelTab.cx, sizeSelTab.cy);
				rect.right += CX_BORDER;
			}
		}
	}
	InvalidateRect(&rect, nTab >= 0);
}

int CPmwWizardTabControl::TabFromPoint(CPoint pt)
{
	// are we on the scroll buttons?
	if (CanScroll() && m_rectScroll.PtInRect(pt))
	{
		if (pt.x < m_rectScroll.left + (m_rectScroll.Width() / 2))
			return SCROLL_LEFT;
		else
			return SCROLL_RIGHT;
	}

	// are we on a tab?
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (GetTabItem(i)->m_rect.PtInRect(pt))
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwWizardPropertySheet -- a tabbed "dialog" (really a popup-window)

BEGIN_MESSAGE_MAP(CPmwWizardPropertySheet, CWnd)
	//{{AFX_MSG_MAP(CPmwWizardPropertySheet)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(ID_WIZBACK, OnWizardBack)
	ON_COMMAND(ID_WIZNEXT, OnWizardNext)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPmwWizardPropertySheet::CPmwWizardPropertySheet(UINT nIDCaption, CWnd* pParent, UINT iSelPage)
{
	m_strCaption.LoadString(nIDCaption);
	CommonConstruct(pParent, iSelPage);
}

CPmwWizardPropertySheet::CPmwWizardPropertySheet(LPCTSTR pszCaption, CWnd* pParent, UINT iSelPage)
{
	ASSERT(pszCaption != NULL);
	m_strCaption = pszCaption;
	CommonConstruct(pParent, iSelPage);
}

void CPmwWizardPropertySheet::CommonConstruct(CWnd* pParent, UINT iSelPage)
{
	m_pParentWnd = pParent;
	m_nCurPage = iSelPage;
	m_hFocusWnd = NULL;
	m_bParentDisabled = FALSE;
	m_hWndDefault = NULL;
	m_hLastFocus = NULL;

	m_hFont = NULL;         // font is created after first page is created

	m_dwWizardButtons = PSWIZB_BACK|PSWIZB_NEXT;
	
	// Note: m_sizeButton, m_cxButtonGap, and m_sizeTabMargin are calculated for real later
	m_sizeButton.cx = 0;
	m_sizeButton.cy = 0;
	m_cxButtonGap = 0;
	m_sizeTabMargin.cx = 0;
	m_sizeTabMargin.cy = 0;
}

CPmwWizardPropertySheet::~CPmwWizardPropertySheet()
{
	PmwWizardDeleteObject((HGDIOBJ*)&m_hFont);
}

void CPmwWizardPropertySheet::PageChanged()
{
	BOOL bEnabled = FALSE;
	for (int i = 0; i < GetPageCount(); i++)
	{
		if (GetPage(i)->m_bChanged)
		{
			bEnabled = TRUE;
			break;
		}
	}
}

void CPmwWizardPropertySheet::CancelToClose()
{
	::EnableDlgItem(m_hWnd, IDCANCEL, FALSE);

	// Note: If this _AfxLoadString call fails, it is likely that
	//  _AFX_NO_PROPERTY_RESOURCES is defined in your .RC file.
	// To correct the situation, remove the following line from your
	//  resource script:
	//      #define _AFX_NO_PROPERTY_RESOURCES
	// This should be done using the Resource.Set Includes... command.

	TCHAR szCaption[256];
	VERIFY(_AfxLoadString(AFX_IDS_PS_CLOSE, szCaption) != 0);
	::SetDlgItemText(m_hWnd, IDCANCEL, szCaption);
}

BOOL CPmwWizardPropertySheet::CreateStandardButtons()
{
	TCHAR szCaption[256];
	HWND hWnd = NULL;
	
	// Create the back button.
	VERIFY(_AfxLoadString(IDS_WIZBACK, szCaption) != 0);
	hWnd = ::CreateWindow(_T("button"), szCaption, WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_GROUP|BS_PUSHBUTTON, 0, 0, 0, 0, m_hWnd, (HMENU)ID_WIZBACK, AfxGetInstanceHandle(), NULL);
	if (m_hFont != NULL) ::SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hFont, 0);
	
	// Create the next/finish button.
	VERIFY(_AfxLoadString(IDS_WIZNEXT, szCaption) != 0);
	hWnd = ::CreateWindow(_T("button"), szCaption, WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_GROUP|BS_PUSHBUTTON, 0, 0, 0, 0, m_hWnd, (HMENU)ID_WIZNEXT, AfxGetInstanceHandle(), NULL);
	if (m_hFont != NULL) ::SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hFont, 0);
	
	// Create the cancel button.
	VERIFY(_AfxLoadString(IDS_WIZCANCEL, szCaption) != 0);
	hWnd = ::CreateWindow(_T("button"), szCaption, WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_GROUP|BS_PUSHBUTTON, 0, 0, 0, 0, m_hWnd, (HMENU)IDCANCEL, AfxGetInstanceHandle(), NULL);
	if (m_hFont != NULL) ::SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hFont, 0);
	
	return TRUE;
}

BOOL CPmwWizardPropertySheet::ProcessTab(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
	{
		HWND hWnd = ::GetFocus();
		if ((::SendMessage(hWnd,WM_GETDLGCODE, 0, 0) &
			(DLGC_WANTALLKEYS | DLGC_WANTMESSAGE | DLGC_WANTTAB)) == 0)
		{
			BOOL bShift = (GetKeyState(VK_SHIFT) < 0);
			if (GetKeyState(VK_CONTROL) < 0) // control-tab
			{
				if (bShift)
				{
					OnWizardBack();
				}
				else
				{
					OnWizardNext();
				}
				return TRUE;
			}
			else if (!::IsChild(GetActivePage()->m_hWnd, pMsg->hwnd))
			{
				HWND hWndPage = GetActivePage()->m_hWnd;
				HWND hNextControl = ::GetNextDlgTabItem(m_hWnd, pMsg->hwnd, bShift);
				if (hNextControl == hWndPage)
				{
					// Moving onto page. Make sure it has a "tabable" item.
					HWND hWndCtrl = ::GetWindow(hWndPage, GW_CHILD);
					hWndCtrl = ::GetNextDlgTabItem(hWndPage, hWndCtrl, bShift);
					if ((::GetWindowLong(hWndCtrl, GWL_STYLE) & WS_TABSTOP) != 0)
					{
						SetCtrlFocus(hWndCtrl);
						return TRUE;
					}
					
					hNextControl = ::GetNextDlgTabItem(m_hWnd, hNextControl, bShift);
					SetCtrlFocus(hNextControl);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void CPmwWizardPropertySheet::CheckDefaultButton(HWND hFocusBefore, HWND hFocusAfter)
{
	ASSERT(hFocusBefore != hFocusAfter);

	// determine old default button
	HWND hOldDefault = NULL;
	DWORD dwOldDefault = 0;
	if (::IsWindow(hFocusBefore) && ::IsChild(m_hWnd, hFocusBefore))
	{
		hOldDefault = hFocusBefore;
		if (hFocusBefore != NULL)
			dwOldDefault = (DWORD)::SendMessage(hFocusBefore, WM_GETDLGCODE, 0, 0);
		if (!(dwOldDefault & (DLGC_DEFPUSHBUTTON|DLGC_UNDEFPUSHBUTTON)))
		{
			hOldDefault = ::GetDlgItem(m_hWnd, ID_WIZNEXT);
			dwOldDefault = (DWORD)::SendMessage(hOldDefault, WM_GETDLGCODE, 0, 0);
		}
	}

	// determine new default button
	HWND hWndDefault = NULL;
	DWORD dwDefault = 0;
	if (::IsChild(m_hWnd, hFocusAfter))
	{
		hWndDefault = hFocusAfter;
		if (hFocusAfter != NULL)
			dwDefault = (DWORD)::SendMessage(hFocusAfter, WM_GETDLGCODE, 0, 0);
		if (!(dwDefault & (DLGC_DEFPUSHBUTTON|DLGC_UNDEFPUSHBUTTON)))
		{
			hWndDefault = ::GetDlgItem(m_hWnd, ID_WIZNEXT);
			dwDefault = (DWORD)::SendMessage(hWndDefault, WM_GETDLGCODE, 0, 0);
		}
	}

	// set new styles
	if (hOldDefault != hWndDefault && (dwOldDefault & DLGC_DEFPUSHBUTTON))
		::SendMessage(hOldDefault, BM_SETSTYLE, BS_PUSHBUTTON, TRUE);

	if (dwDefault & DLGC_UNDEFPUSHBUTTON)
		::SendMessage(hWndDefault, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE);

	// remember special case default button
	m_hWndDefault = (hWndDefault == hFocusAfter ? hFocusAfter : NULL);
}

void CPmwWizardPropertySheet::CheckFocusChange()
{
	HWND hWndFocus = ::GetFocus();
	if (hWndFocus != m_hLastFocus)
	{
		CheckDefaultButton(m_hLastFocus, hWndFocus);
		m_hLastFocus = hWndFocus;
	}
}

BOOL CPmwWizardPropertySheet::PreTranslateMessage(MSG* pMsg)
{
	BOOL bResult = FALSE;

	// post message to check for change in focus later
	if (pMsg->message != WM_KICKIDLE)
	{
		MSG msg;
		PeekMessage(&msg, NULL, WM_KICKIDLE, WM_KICKIDLE, PM_REMOVE);
		PostMessage(WM_KICKIDLE);
	}

	// process special case keystrokes
	if (ProcessChars(pMsg))
		bResult = TRUE;
	else if (ProcessTab(pMsg))
		bResult = TRUE;
	else
	{
		// handle normal accelerator keystrokes
		CPmwWizardPropertyPage* pPage = GetActivePage();
		if ((::IsChild(pPage->m_hWnd, pMsg->hwnd) &&
			::IsDialogMessage(pPage->m_hWnd, pMsg)) ||
			::IsDialogMessage(m_hWnd, pMsg))
		{
			bResult = TRUE;
		}
	}

	if (!bResult)
		bResult = CWnd::PreTranslateMessage(pMsg);

	// handle WM_KICKIDLE message to check for focus changes
	if (pMsg->message == WM_KICKIDLE)
		CheckFocusChange();

	return bResult;
}

HWND CPmwWizardPropertySheet::FindNextControl(HWND hWnd, TCHAR ch)
{
	CPmwWizardPropertyPage* pPage = GetActivePage();
	if (pPage == NULL)
		return NULL;

	HWND hWndFocusPage = hWnd;
	HWND hWndFocusSheet = hWnd;
	if (::IsChild(pPage->m_hWnd, hWnd)) // current focus is on the page
		hWndFocusSheet = pPage->m_hWnd;
	else
		hWndFocusPage = ::GetWindow(pPage->m_hWnd, GW_CHILD);

	HWND hWndNext = ::FindNextControl(pPage->m_hWnd, hWndFocusPage, ch);
	if (hWndNext == NULL) // if not found on page
		hWndNext = ::FindNextControl(m_hWnd, hWndFocusSheet, ch);

	return hWndNext;
}

BOOL CPmwWizardPropertySheet::ProcessChars(MSG* pMsg)
{
	CPmwWizardPropertyPage* pPage = GetActivePage();
	if (pPage == NULL)
		return FALSE;

	HWND hWnd = pMsg->hwnd;
	UINT message = pMsg->message;

	if (hWnd == NULL)
		return FALSE;

	switch (message)
	{
		case WM_SYSCHAR:
			/* If no control has focus, and Alt not down, then ignore. */
			if ((::GetFocus == NULL) && (GetKeyState(VK_MENU) >= 0))
				return FALSE;

			// fall through

		case WM_CHAR:
			/* Ignore chars sent to the dialog box (rather than the control). */
			if (hWnd == m_hWnd || hWnd == pPage->m_hWnd)
				return FALSE;

			WORD code = (WORD)(DWORD)::SendMessage(hWnd, WM_GETDLGCODE, pMsg->wParam,
				(LPARAM)(LPMSG)pMsg);

			// If the control wants to process the message, then don't check
			// for possible mnemonic key.

			// Check if control wants to handle this message itself
			if (code & DLGC_WANTMESSAGE)
				return FALSE;

			if ((message == WM_CHAR) && (code & DLGC_WANTCHARS))
				return FALSE;

			HWND hWndNext = FindNextControl(hWnd, (TCHAR)pMsg->wParam);
			if (hWndNext == NULL) // nothing found
				return FALSE;

			// once we know we are going to handle it, call the filter
			if (CallMsgFilter(pMsg, MSGF_DIALOGBOX))
				return TRUE;

			GotoControl(hWndNext, (TCHAR)pMsg->wParam);
			return TRUE;
	}
	return FALSE;
}

void CPmwWizardPropertySheet::GotoControl(HWND hWnd, TCHAR ch)
{
	HWND  hWndFirst;
	for (hWndFirst = NULL; hWndFirst != hWnd;
		hWnd = FindNextControl(hWnd, ch))
	{
		if (hWndFirst == NULL)
			hWndFirst = hWnd;

		WORD code = (WORD)(DWORD)::SendMessage(hWnd, WM_GETDLGCODE, 0, 0L);
		// If a non-disabled static item, then jump ahead to nearest tabstop.
		if (code & DLGC_STATIC && ::IsWindowEnabled(hWnd))
		{
			CPmwWizardPropertyPage* pPage = GetActivePage();
			if (::IsChild(pPage->m_hWnd, hWnd))
				hWnd = ::GetNextDlgTabItem(pPage->m_hWnd, hWnd, FALSE);
			else
				hWnd = ::GetNextDlgTabItem(m_hWnd, hWnd, FALSE);
			code = (WORD)(DWORD)::SendMessage(hWnd, WM_GETDLGCODE, 0, 0L);
		}

		if (::IsWindowEnabled(hWnd))
		{
			// Is it a Pushbutton?
			if (!(code & DLGC_BUTTON))
			{
				SetCtrlFocus(hWnd);
			}
			else
			{
				// Yes, click it, but don't give it the focus.
				if ((code & DLGC_DEFPUSHBUTTON) ||
					(code & DLGC_UNDEFPUSHBUTTON))
				{
					// flash the button
					::SendMessage(hWnd, BM_SETSTATE, TRUE, 0L);
					::SendMessage(hWnd, BM_SETSTATE, FALSE, 0L);

					// Send the WM_COMMAND message.
					::SendMessage(::GetParent(hWnd), WM_COMMAND,
						(WPARAM)_AfxGetDlgCtrlID(hWnd),
						MAKELONG(hWnd, BN_CLICKED));
				}
				else
				{
					::SetFocus(hWnd);
					// Send click message if button has a UNIQUE mnemonic
					if (FindNextControl(hWnd, ch) == hWnd)
					{
						::SendMessage(hWnd, WM_LBUTTONDOWN, 0, 0L);
						::SendMessage(hWnd, WM_LBUTTONUP, 0, 0L);
					}
				}
			}
			return;
		}
	}
}

int CPmwWizardPropertySheet::DoModal()
{
	int nResult = IDABORT;

	// cannot call DoModal on a dialog already constructed as modeless
	ASSERT(m_hWnd == NULL);

	// allow OLE servers to disable themselves
	CWinApp* pApp = AfxGetApp();
	pApp->EnableModeless(FALSE);

	// find parent HWND
	CWnd* pParentWnd = CWnd::FromHandle(_AfxGetSafeOwner(m_pParentWnd));

	// create the dialog, then enter modal loop
	if (Create(pParentWnd, WS_SYSMENU|WS_POPUP|WS_CAPTION|DS_MODALFRAME))
	{
		// disable parent (should not disable this window)
		m_bParentDisabled = FALSE;
		if (pParentWnd != NULL && pParentWnd->IsWindowEnabled())
		{
			pParentWnd->EnableWindow(FALSE);
			m_bParentDisabled = TRUE;
		}
		ASSERT(IsWindowEnabled());  // should not be disabled to start!
		SetActiveWindow();

		// for tracking the idle time state
		BOOL bShown = (GetStyle() & WS_VISIBLE) != 0;
		m_nID = -1;

		if (!bShown)
		{
			// show and activate the window
			bShown = TRUE;
			ShowWindow(SW_SHOWNORMAL);
		}

		// acquire and dispatch messages until a WM_QUIT message is received.
		MSG msg;
		while (m_nID == -1 && m_hWnd != NULL)
		{
			// phase1: check to see if we can do idle work
			if (!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
			{
				// send WM_ENTERIDLE since queue is empty
				if (pParentWnd != NULL &&
					!(pParentWnd->GetStyle() & DS_NOIDLEMSG))
				{
					pParentWnd->SendMessage(WM_ENTERIDLE,
						MSGF_DIALOGBOX, MAKELONG(m_hWnd, 0));
				}
			}

			// phase2: pump messages while available
			do
			{
				// pump message -- if WM_QUIT assume cancel and repost
				if (!PumpMessage())
				{
					::PostQuitMessage((int)msg.wParam);
					m_nID = IDCANCEL;
					break;
				}

			} while (m_nID == -1 && m_hWnd != NULL &&
				::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));
		}

		nResult = m_nID;
		if (m_hWnd != NULL)
			EndDialog(nResult);
	}

	// allow OLE servers to enable themselves
	pApp->EnableModeless(TRUE);

	return nResult;
}

BOOL CPmwWizardPropertySheet::PumpMessage()
{
	ASSERT_VALID(this);

	MSG msg;
	if (!::GetMessage(&msg, NULL, NULL, NULL))
		return FALSE;

	//  let's see if the message should be handled at all
	if (CallMsgFilter(&msg, MSGF_DIALOGBOX))
		return TRUE;
	// process this message
	if (!WalkPreTranslateTree(m_hWnd, &msg))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return TRUE;
}

BOOL CPmwWizardPropertySheet::Create(CWnd* pParent, DWORD dwStyle, DWORD dwExStyle)
{
	return CreateEx(
		dwExStyle,
		AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_SAVEBITS, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_BTNFACE+1)),
		m_strCaption, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, pParent->GetSafeHwnd(), NULL);
}

////////////////////////////////////////////////////////////////////////////

void CPmwWizardPropertySheet::AddPage(CPmwWizardPropertyPage* pPage)
{
	ASSERT(pPage != NULL);
	ASSERT(pPage->IsKindOf(RUNTIME_CLASS(CPmwWizardPropertyPage)));
	ASSERT_VALID(pPage);
	m_pages.Add(pPage);
	ASSERT(pPage->m_pParentWnd == NULL);
	pPage->m_pParentWnd = this;
}

void CPmwWizardPropertySheet::RemovePage(CPmwWizardPropertyPage* pPage)
{
	ASSERT(pPage != NULL);
	ASSERT(pPage->IsKindOf(RUNTIME_CLASS(CPmwWizardPropertyPage)));
	for (int i = 0; i < GetPageCount(); i++)
	{
		if (GetPage(i) == pPage)
		{
			RemovePage(i);
			return;
		}
	}
	ASSERT(FALSE);  // pPage not found
}

void CPmwWizardPropertySheet::RemovePage(int nPage)
{
	ASSERT(m_hWnd == NULL || GetPageCount() > 1);
	ASSERT(nPage >= 0 && nPage < GetPageCount());

	// adjust active page in case of removing active page
	BOOL bRemoveActive = (nPage == m_nCurPage);
	if (m_hWnd != NULL && bRemoveActive)
	{
		int nNewPage = nPage+1;
		if (nNewPage >= GetPageCount())
			nNewPage = 0;
		VERIFY(SetActivePage(nNewPage));
	}

	// remove the page
	CPmwWizardPropertyPage* pPage = GetPage(nPage);
	m_pages.RemoveAt(nPage);
	ASSERT(m_nCurPage != nPage);
	if (m_nCurPage > nPage)
		--m_nCurPage;
	pPage->DestroyWindow();
	pPage->m_pParentWnd = NULL;
}

void CPmwWizardPropertySheet::EndDialog(int nEndID)
{
	ASSERT_VALID(this);

	m_nID = nEndID;
	DestroyWindow();
}

BOOL CPmwWizardPropertySheet::DestroyWindow()
{
	// re-enable parent if it was disabled
	CWnd* pParentWnd = m_pParentWnd != NULL ? m_pParentWnd : GetParent();
	if (m_bParentDisabled && pParentWnd != NULL)
		pParentWnd->EnableWindow();

	// transfer the focus to ourselves to give the active control
	//  a chance at WM_KILLFOCUS
	if (::GetActiveWindow() == m_hWnd && ::IsChild(m_hWnd, ::GetFocus()))
	{
		m_hFocusWnd = NULL;
		SetFocus();
	}
	// hide this window and move activation to the parent
	SetWindowPos(NULL, 0, 0, 0, 0,
		SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE |
		SWP_NOSIZE | SWP_NOZORDER);

	pParentWnd = GetParent();
	if (pParentWnd != NULL)
		pParentWnd->SetActiveWindow();

	// finally, destroy this window
	BOOL bResult = CWnd::DestroyWindow();

	// delete the font (will be created next time DoModal/Create is called)
	PmwWizardDeleteObject((HGDIOBJ*)&m_hFont);

	return bResult;
}

BOOL CPmwWizardPropertySheet::SetActivePage(int nPage)
{
	CPmwWizardPropertyPage* pPage;
	CRect rect;
	rect.SetRectEmpty();

	// get rectangle from previous page if it exists
	if (m_nCurPage >= 0)
	{
		pPage = GetPage(m_nCurPage);
		if (pPage->m_hWnd != NULL)
			pPage->GetWindowRect(&rect);
		ScreenToClient(&rect);
	}

	// activate next page
	if (nPage >= 0)
	{
		pPage = GetPage(nPage);
		ASSERT(pPage->m_pParentWnd == this);
		if (!pPage->OnSetActive())
			return FALSE;
	}
	m_nCurPage = nPage;

	// layout next page
	if (m_nCurPage >= 0)
	{
		if (!rect.IsRectEmpty())
		{
			pPage->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
		}
		pPage->ShowWindow(SW_SHOW);
		pPage->EnableWindow();
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwWizardPropertySheet message handlers

int CPmwWizardPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// fix-up the system menu so this looks like a dialog box
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	ASSERT(pSysMenu != NULL);
	int i, nCount = pSysMenu->GetMenuItemCount();
	for (i = 0; i < nCount; i++)
	{
		UINT nID = pSysMenu->GetMenuItemID(i);
		if (nID != SC_MOVE && nID != SC_CLOSE)
		{
			pSysMenu->DeleteMenu(i, MF_BYPOSITION);
			i--;
			nCount--;
		}
	}

	// set active page and active tab
	SetActivePage(m_nCurPage);

	// initialize font used for buttons
	ASSERT(m_hFont == NULL);
	CPmwWizardPropertyPage* pPage = GetActivePage();
	ASSERT_VALID(pPage);

	HFONT hFont = (HFONT)pPage->SendMessage(WM_GETFONT);
	if (hFont != NULL)
	{
		LOGFONT logFont;
		VERIFY(::GetObject(hFont, sizeof(LOGFONT), &logFont));
		m_hFont = CreateFontIndirect(&logFont);
	}

	// set page's z-order correctly (side effect of SetActivePage)
//	SetActivePage(m_nCurPage);

	// calculate button sizes and separator
	CRect rect(6, 0, 45, 16);  // button size is (45,16)
	pPage->MapDialogRect(rect);
	m_sizeButton.cx = rect.right;
	m_sizeButton.cy = rect.bottom;
	m_cxButtonGap = rect.left;

	// calculate tab margin area
	rect.bottom = rect.right = 6;   // std dialog margin is 6 dialog units
	pPage->MapDialogRect(rect);
	m_sizeTabMargin.cx = rect.right;
	m_sizeTabMargin.cy = rect.bottom;

	// create standard buttons
	if (!CreateStandardButtons())
		return -1;

	RecalcLayout();

	CenterWindow();

	return 0;   // success
}

void CPmwWizardPropertySheet::OnPaint()
{
	Default();
	
	CDC* pDC = GetDC();
	if (pDC != NULL)
	{
		CRect crRect;
		GetClientRect(crRect);
		
		pDC->SetBkMode(TRANSPARENT);
	
		CRect crMargins(6, 6, 0, 0);
		CPmwWizardPropertyPage* pPage = GetActivePage();
		pPage->MapDialogRect(crMargins);
		
		int nY = crRect.Height()-m_sizeTabMargin.cy-m_sizeButton.cy-crMargins.top-1;
	
		HPEN hOldPen = (HPEN)(pDC->SelectObject(afxData.hpenBtnShadow));
		pDC->MoveTo(crMargins.left, nY);
		pDC->LineTo(crRect.right-crMargins.left, nY);
		pDC->SelectObject(afxData.hpenBtnHilite);
		pDC->MoveTo(crMargins.left, nY+1);
		pDC->LineTo(crRect.right-crMargins.left, nY+1);
		pDC->SelectObject(hOldPen);
		
		ReleaseDC(pDC);
	}
}

void CPmwWizardPropertySheet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	PostMessage(WM_KICKIDLE);

	if (nState == WA_INACTIVE)
		m_hFocusWnd = ::GetFocus();

	CWnd::OnActivate(nState, pWndOther, bMinimized);
}

void CPmwWizardPropertySheet::OnCancel()
{
	ASSERT_VALID(this);

	GetActivePage()->OnCancel();
	EndDialog(IDCANCEL);
}

LRESULT CPmwWizardPropertySheet::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID(this);

	CPmwWizardPropertyPage* pActivePage = GetActivePage();
	ASSERT_VALID(pActivePage);
	return _AfxCallWndProc(
		pActivePage, pActivePage->m_hWnd, WM_COMMANDHELP, wParam, lParam);
}

LRESULT CPmwWizardPropertySheet::OnGetFont(WPARAM, LPARAM)
{
	return MAKELONG(m_hFont, 0);
}

void CPmwWizardPropertySheet::OnWizardBack()
{
	CPmwWizardPropertyPage* pActivePage = GetActivePage();
	if (pActivePage != NULL)
	{
		if (m_nCurPage > 0)
		{
			if (pActivePage->OnWizardBack() == 0)
			{
				if (pActivePage->m_hWnd != NULL)
				{
					pActivePage->ShowWindow(SW_HIDE);
				}
				SetActivePage(m_nCurPage-1);
			}
		}
	} 
}

void CPmwWizardPropertySheet::OnWizardNext()
{
	CPmwWizardPropertyPage* pActivePage = GetActivePage();
	if (pActivePage != NULL)
	{
		if ((m_dwWizardButtons & (PSWIZB_FINISH|PSWIZB_DISABLEDFINISH)) != 0)
		{
			if (pActivePage->OnWizardFinish())
			{
				if (pActivePage->m_hWnd != NULL)
				{
					pActivePage->ShowWindow(SW_HIDE);
				}
				EndDialog(ID_WIZFINISH);
			}
		}
		else
		{
			if (m_nCurPage < GetPageCount()-1)
			{
				if (pActivePage->OnWizardNext() == 0)
				{
					if (pActivePage->m_hWnd != NULL)
					{
						pActivePage->ShowWindow(SW_HIDE);
					}
					SetActivePage(m_nCurPage+1);
				}
			}
		}
	} 
}

void CPmwWizardPropertySheet::OnClose()
{
	ASSERT_VALID(this);
	OnCancel();
}

void CPmwWizardPropertySheet::RecalcLayout()
{
	// determine size of the active page (active page determines initial size)
	CRect rectPage;
	GetActivePage()->GetWindowRect(rectPage);

	// determine total size of the buttons
	int cxButtonTotal = m_sizeButton.cx+m_sizeButton.cx+m_cxButtonGap+m_sizeButton.cx;
	int nWidth = max(2*m_sizeTabMargin.cx + rectPage.Width() + 3, 2*m_sizeTabMargin.cx + cxButtonTotal);
	int nHeight = m_sizeTabMargin.cy+rectPage.Height()+2*m_sizeTabMargin.cy+m_sizeButton.cy+m_sizeTabMargin.cy;

	CRect rectSheet(0, 0, nWidth, nHeight);
	CRect rectClient = rectSheet;
	::AdjustWindowRectEx(rectSheet, GetStyle(), FALSE, GetExStyle());
	SetWindowPos(NULL, 0, 0, rectSheet.Width(), rectSheet.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	GetActivePage()->SetWindowPos(NULL, m_sizeTabMargin.cx+1, m_sizeTabMargin.cy, nWidth - m_sizeTabMargin.cx*2 - 3, rectPage.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	int x = nWidth - m_sizeTabMargin.cx - cxButtonTotal;
	int y = (nHeight - m_sizeTabMargin.cy) - m_sizeButton.cy;

	// Position the back button.
	HWND hBack = ::GetDlgItem(m_hWnd, ID_WIZBACK);
	if (hBack != NULL)
	{
		::EnableWindow(hBack, (m_dwWizardButtons & PSWIZB_BACK) != 0);
		::MoveWindow(hBack, x, y, m_sizeButton.cx, m_sizeButton.cy, TRUE);
	}                                                                       
	x += m_sizeButton.cx;
	
	// Position the next/finish button.
	HWND hNext = ::GetDlgItem(m_hWnd, ID_WIZNEXT);
	if (hNext != NULL)
	{
		if ((m_dwWizardButtons & (PSWIZB_FINISH|PSWIZB_DISABLEDFINISH)) == 0)
		{
			// Make sure text is "Next".
			TCHAR szCaption[256];
			VERIFY(_AfxLoadString(IDS_WIZNEXT, szCaption) != 0);
			::SetWindowText(hNext, szCaption);
			::EnableWindow(hNext, (m_dwWizardButtons & PSWIZB_NEXT) != 0);
		}
		else
		{
			// Make sure text is "Finish".
			TCHAR szCaption[256];
			VERIFY(_AfxLoadString(IDS_WIZFINISH, szCaption) != 0);
			::SetWindowText(hNext, szCaption);
			::EnableWindow(hNext, (m_dwWizardButtons & PSWIZB_FINISH) != 0);
		}
		::MoveWindow(hNext, x, y, m_sizeButton.cx, m_sizeButton.cy, TRUE);
	}
	x += m_sizeButton.cx+m_cxButtonGap;
	
	// Position the cancel button.
	HWND hCancel = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (hCancel != NULL)
	{
		::EnableWindow(hCancel, TRUE);
		::MoveWindow(hCancel, x, y, m_sizeButton.cx, m_sizeButton.cy, TRUE);
	}

	if (((UINT)::GetFocus() == 0) || (::GetFocus() == m_hWnd))
	{
		HWND hWndCtrl = ::GetWindow(m_hWnd, GW_CHILD);
		hWndCtrl = ::GetNextDlgTabItem(m_hWnd, hWndCtrl, FALSE);
		if ((::GetWindowLong(hWndCtrl, GWL_STYLE) & WS_TABSTOP) != 0)
		{
			SetCtrlFocus(hWndCtrl);
		}
	}
}

HBRUSH CPmwWizardPropertySheet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	LRESULT lResult;
	if (pWnd->SendChildNotifyLastMsg(&lResult))
		return (HBRUSH)lResult;

	if (!GrayCtlColor(pDC->m_hDC, pWnd->GetSafeHwnd(), nCtlColor,
	  afxData.hbrBtnFace, afxData.clrBtnText))
		return (HBRUSH)Default();
	return afxData.hbrBtnFace;
}

void CPmwWizardPropertySheet::OnSetFocus(CWnd* /*pOldWnd*/)
{
	if (m_hFocusWnd != NULL)
	{
		ASSERT(m_hFocusWnd != NULL);
		::SetFocus(m_hFocusWnd);
	}
}

void CPmwWizardPropertySheet::SetWizardMode()
{
}

void CPmwWizardPropertySheet::SetWizardButtons(DWORD dwFlags)
{
	m_dwWizardButtons = dwFlags;
	RecalcLayout();
}

IMPLEMENT_DYNAMIC(CPmwWizardPropertyPage, CDialog)
IMPLEMENT_DYNAMIC(CPmwWizardTabControl, CWnd)
IMPLEMENT_DYNAMIC(CPmwWizardPropertySheet, CWnd)
#endif

/////////////////////////////////////////////////////////////////////////////

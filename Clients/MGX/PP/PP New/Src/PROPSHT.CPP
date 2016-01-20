// propsht.cpp : implementation file
//

#include "pp.h"
#include "astral.h"
#include "propsht.h"
#include "ctxhelp.h"

extern LPSTR RGamma_Str;
extern LPSTR GGamma_Str;
extern LPSTR BGamma_Str;
extern LPSTR GammaLock_Str;
extern LPSTR GammaDisable_Str;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////
// CPPPropertySheet Class


//	CPPPropertySheet differs slightly from of MFC's CPropertySheet.
//	It's only real difference is that 
// 	(1) It must be used with CPPPropertyPage
// 	(2) It has two member functions HandleOK() and HandleCancel()
//		which calls the member function of the same name in every
//		CPPPropertyPage that has ever been made active.

IMPLEMENT_DYNAMIC(CPPPropertySheet, CPropertySheet)

//---------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------
CPPPropertySheet::CPPPropertySheet(UINT nIDCaption, 
                             CWnd *pParentWnd, UINT nIDHelp, UINT iSelectPage) 
   : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_nIDHelp = nIDHelp;
}

//---------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------
CPPPropertySheet::CPPPropertySheet(LPCTSTR pszCaption, 
                             CWnd *pParentWnd, UINT nIDHelp, UINT iSelectPage)
   : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_nIDHelp = nIDHelp;
}

//---------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------
CPPPropertySheet::~CPPPropertySheet()
{
}

//---------------------------------------------------------------------
// OK button command handler.  
//---------------------------------------------------------------------
void CPPPropertySheet::HandleOK()
{
	int iNumPages = GetPageCount();
	for (int iPage = 0; iPage < iNumPages; iPage++)
	{
   		CPPPropertyPage * pPage =  (CPPPropertyPage *)GetPage(iPage);
   		if (pPage && pPage->PageEverActive())
      		pPage->HandleOK();
	} 
}

//---------------------------------------------------------------------
// Cancel button command handler.  
//---------------------------------------------------------------------
void CPPPropertySheet::HandleCancel()
{
	int iNumPages = GetPageCount();
	for (int iPage = 0; iPage < iNumPages; iPage++)
	{
   		CPPPropertyPage * pPage =  (CPPPropertyPage *)GetPage(iPage);
   		if (pPage && pPage->PageEverActive())
      		pPage->HandleCancel();
	} 
}

BEGIN_MESSAGE_MAP(CPPPropertySheet, CPropertySheet)
	ON_WM_CREATE()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPPPropertySheet message handlers

int CPPPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Add the preview window to the property sheet.

	int nReturn;
	if ((nReturn = CPropertySheet::OnCreate(lpCreateStruct)) != 0)
		return nReturn;

	CWnd *pOKWnd = GetDlgItem(IDOK);
	if (pOKWnd)
		pOKWnd->EnableWindow(FALSE);
	return 0;
}

BOOL CPPPropertySheet::OnQueryNewPalette()
{
	HWND hDlg = GetSafeHwnd();
	return(HandleQueryNewPalette(hDlg, NO, NO));
}

void CPPPropertySheet::OnPaletteChanged(CWnd* pFocusWnd)
{
	ASSERT(pFocusWnd != NULL);

	HWND hDlg = GetSafeHwnd();
 	HWND hwndPaletteChange = pFocusWnd->GetSafeHwnd();
	if (IsOurWindow(hwndPaletteChange))
		FORWARD_WM_PALETTECHANGED(hDlg, hwndPaletteChange, SendMessageToChildren);
}

BOOL CPPPropertySheet::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HWND hWndCursor;

	hWndCursor = pWnd->GetSafeHwnd();
	return( SetupCursor( hWndCursor, nHitTest, m_nIDHelp ) );
}

void CPPPropertySheet::OnHelp()
{
	CPropertyPage* pActivePage = GetActivePage();
	if (pActivePage)
		pActivePage->SendMessage(WM_COMMAND, ID_HELP);
}
 
IMPLEMENT_DYNAMIC(CPPPropertyPage, CPropertyPage)

//---------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------
CPPPropertyPage::CPPPropertyPage(UINT nIDTemplate, 
                             UINT nIDCaption)
   : CPropertyPage(nIDTemplate, nIDCaption)
{
	m_fPageEverActive = FALSE;
	m_nIDTemplate = nIDTemplate;
}

BOOL CPPPropertyPage::OnSetActive()
{
	m_fPageEverActive = TRUE;
	return(CPropertyPage::OnSetActive());
}

void CPPPropertyPage::OnOK()
{
	CPPPropertySheet* pSheet = (CPPPropertySheet*)m_pParentWnd;
	ASSERT(pSheet != NULL);
	ASSERT(pSheet->IsKindOf(RUNTIME_CLASS(CPPPropertySheet)));
	pSheet->HandleOK();
	CPropertyPage::OnOK();
}

void CPPPropertyPage::OnCancel()
{
	CPPPropertySheet* pSheet = (CPPPropertySheet*)m_pParentWnd;
	ASSERT(pSheet != NULL);
	ASSERT(pSheet->IsKindOf(RUNTIME_CLASS(CPPPropertySheet)));
	pSheet->HandleCancel();
	CPropertyPage::OnCancel();
}

void CPPPropertyPage::HandleOK()
{
	// default does nothing - override to handle OK case
}

void CPPPropertyPage::HandleCancel()
{
	// default does nothing - override to handle Cancel case
}
 
void CPPPropertyPage::SetModified(BOOL bHaveApplyNow, BOOL bChanged)
{
	if (bHaveApplyNow)
		CPropertyPage::SetModified(bChanged);

	CPropertySheet* pSheet = (CPropertySheet*)m_pParentWnd;

	ASSERT(pSheet != NULL);

	CWnd *pWndItem = pSheet->GetDlgItem(IDOK);
	if (pWndItem != NULL)
		pWndItem->EnableWindow(bChanged);
}

BEGIN_MESSAGE_MAP(CPPPropertyPage, CPropertyPage)
	ON_NOTIFY_RANGE(UDN_DELTAPOS, 0, 32767, OnDeltaPos)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_PALETTECHANGED()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_MENUSELECT()
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPPPropertyPage message handlers

void CPPPropertyPage::OnDeltaPos(UINT idFrom, NMHDR FAR *pnmhdr, LRESULT *pResult)
{
	if (pnmhdr->code == UDN_DELTAPOS)
	{
		CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(pnmhdr->hwndFrom);
		if (pSpin && pSpin->IsKindOf(RUNTIME_CLASS(CSpinFixedCtrl)))
			pSpin->OnNotifyDeltaPos((LPNM_UPDOWN)pnmhdr);
	}
	*pResult = 0;
}	

void CPPPropertyPage::OnHScroll(UINT uCode, UINT iPos, CScrollBar *pScrollBar)
{
	if (pScrollBar)
	{
		STRING szClass;
		GetClassName(pScrollBar->GetSafeHwnd(), szClass, sizeof(szClass)-1);
		if (StringsEqual(szClass, TRACKBAR_CLASS))
		{
			SliderOnScroll(pScrollBar->GetSafeHwnd(), uCode, (int)iPos);
			return;
		}
	}
	CPropertyPage::OnHScroll(uCode, iPos, pScrollBar);
}
    
void CPPPropertyPage::OnVScroll(UINT uCode, UINT iPos, CScrollBar *pScrollBar)
{
	if (pScrollBar)
	{
		STRING szClass;
		GetClassName(pScrollBar->GetSafeHwnd(), szClass, sizeof(szClass)-1);
		if (StringsEqual(szClass, TRACKBAR_CLASS))
		{
			SliderOnScroll(pScrollBar->GetSafeHwnd(), uCode, (int)iPos);
			return;
		}
	}
	CPropertyPage::OnVScroll(uCode, iPos, pScrollBar);
}
    
BOOL CPPPropertyPage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
 	CPoint pt;
	HWND hChild, hWndCursor;

	hWndCursor = pWnd->GetSafeHwnd();
	// check for disabled control
	GetCursorPos( &pt );
	if( hChild = MyChildWindowFromPoint( hWndCursor, pt ))
	{
    	hWndCursor = hChild;
	}

	return( SetupCursor( hWndCursor, nHitTest, m_nIDTemplate ) );
}

void CPPPropertyPage::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	HWND hDlg = GetSafeHwnd();
	MeasureItem( hDlg, lpMeasureItemStruct, NO );
}

void CPPPropertyPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HWND hDlg = GetSafeHwnd();
	DrawItem( hDlg, lpDrawItemStruct, 0 );
}

void CPPPropertyPage::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	HWND hDlg = GetSafeHwnd();
	HelpContext.OnHints( hDlg, 0 );
	HintLine(nItemID);
}

void CPPPropertyPage::OnHelp()
{
	if (m_nIDTemplate)
		Help(HELP_CONTEXT, m_nIDTemplate);		
}


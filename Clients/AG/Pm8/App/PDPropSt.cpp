// PDPropSt.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "PDPropSt.h"
#include "util.h"
#include "bmptiler.h"
#include "rmagcom.h"    // For CResourceManager

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette* pOurPal;  // PrintMaster Palette

#define BKG_IMAGE IDB_GALLERY_PROJ_SIDEBAR

/////////////////////////////////////////////////////////////////////////////
// CPDPropSheet

IMPLEMENT_DYNAMIC(CPDPropSheet, CPropertySheet)

CPDPropSheet::CPDPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPDPropSheet::CPDPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CPDPropSheet::~CPDPropSheet()
{
}


BEGIN_MESSAGE_MAP(CPDPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPDPropSheet)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPDPropSheet message handlers

BOOL CPDPropSheet::OnEraseBkgnd(CDC* pDC) 
{
   CRect       crFill;

   GetClientRect(&crFill);

	CBitmap bmpBackground;
	CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());
	if(rlProductSpecific.LoadBitmap(bmpBackground, CSharedIDResourceManager::riArtGallerySidebar, pOurPal) != TRUE)
		return CPropertySheet::OnEraseBkgnd(pDC);
   // Tile the background.
	CBmpTiler Tiler;
	Tiler.TileBmpInRect(*pDC, crFill, bmpBackground);
   return TRUE;
	
}

BOOL CPDPropSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	CTabCtrl* pTabCtrl = GetTabControl();
	pTabCtrl->ShowWindow(SW_HIDE);
	SetWindowText(m_strCaption);
	return bResult;
}


/*
// $Workfile: PRVWDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PRVWDLG.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:34:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:46   FRED
// Initial revision.
// 
//    Rev 2.1   26 Jun 1995 16:58:42   JAY
//  
*/

#include "stdafx.h"
#include "pmw.h"
#include "misc.h"
#include "pmwdlg.h"
#include "prvwdlg.h"
#include "util.h"

#include <afxpriv.h>				// for WM_HELPHITTEST

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL CProgressDialog::m_fUserAbort;

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog dialog

CProgressDialog::CProgressDialog(UINT Id, CWnd* pParent)
{
	//{{AFX_DATA_INIT(CProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(Id, pParent);
	m_fUserAbort = FALSE;
}

void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CProgressDialog)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

BOOL CProgressDialog::abort_check(VOIDPTR)
{
	MSG msg;

	while (!m_fUserAbort &&
			::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetApp()->PumpMessage())
			return TRUE;               // Bag out if WM_QUIT received
	}
	return m_fUserAbort;
}

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog message handlers

BOOL CProgressDialog::OnInitDialog()
{
	Util::CenterWindowInWindow(this);
	
//	standard_dialog_startup(this, CProgressDialog::IDD);
	
	return CDialog::OnInitDialog();
}

#if 0
void CProgressDialog::OnDestroy()
{
//	standard_dialog_shutdown(this, CProgressDialog::IDD);
	
	CDialog::OnDestroy();
}

LRESULT CProgressDialog::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	return standard_dialog_hit_test(this, CProgressDialog::IDD, lParam);
}
#endif

void CProgressDialog::OnCancel()
{
	m_fUserAbort = TRUE;
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CBuildingPreview dialog

CBuildingPreview::CBuildingPreview(CWnd* pParent)
	: CProgressDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CBuildingPreview, CProgressDialog)
	//{{AFX_MSG_MAP(CBuildingPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdatingClipboard dialog

CUpdatingClipboard::CUpdatingClipboard(CWnd* pParent)
	: CProgressDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CUpdatingClipboard, CProgressDialog)
	//{{AFX_MSG_MAP(CUpdatingClipboard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


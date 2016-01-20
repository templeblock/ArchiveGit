// convert.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdlg.h"
#include "convert.h"

#include <afxpriv.h>				// for WM_HELPHITTEST

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConvertingDialog dialog

CConvertingDialog::CConvertingDialog(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CConvertingDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(CConvertingDialog::IDD, pParent);
}

void CConvertingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConvertingDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConvertingDialog, CDialog)
	//{{AFX_MSG_MAP(CConvertingDialog)
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConvertingDialog message handlers

BOOL CConvertingDialog::OnInitDialog()
{
	standard_dialog_startup(this, (UINT)-1);		//CConvertingDialog::IDD);
	return CDialog::OnInitDialog();
}

void CConvertingDialog::OnDestroy()
{
	standard_dialog_shutdown(this, (UINT)-1);		//CConvertingDialog::IDD);
	CDialog::OnDestroy();
}

LRESULT CConvertingDialog::OnHelpHitTest(WPARAM, LPARAM lParam)
{
//	return standard_dialog_hit_test(this, CConvertingDialog::IDD, lParam);
	return 0;
}

void CConvertingDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
//	standard_dialog_activate(CConvertingDialog::IDD, nState, pWndOther, bMinimized);
	standard_dialog_activate((UINT)-1, nState, pWndOther, bMinimized);
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

// progdlg.cpp : implementation file
//

#include "stdafx.h"
#include "Uninstal.h"
#include "progdlg.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUninstallProgressDialog dialog


CUninstallProgressDialog::CUninstallProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CUninstallProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUninstallProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BOOL CUninstallProgressDialog::Create(UINT nIDTemplate, CWnd* pParentWnd /*=NULL*/)
{
	return CDialog::Create(nIDTemplate, pParentWnd);
}

void CUninstallProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUninstallProgressDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUninstallProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CUninstallProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CUninstallProgressDialog::SetProgress(LPCSTR pszAction /*=NULL*/, LPCSTR pszItem /*=NULL*/)
{
	SetAction(pszAction);
	SetItem(pszItem);
}

void CUninstallProgressDialog::SetAction(LPCSTR pszAction /*=NULL*/)
{
	CWnd* pAction = GetDlgItem(IDC_ACTION);
	if (pAction != NULL)
	{
		CString csWndText(pszAction);
		GetConfiguration()->ReplaceText(csWndText);
		pAction->SetWindowText(csWndText.IsEmpty() ? "" : csWndText);
		pAction->UpdateWindow();
	} 
}

void CUninstallProgressDialog::SetItem(LPCSTR pszItem /*=NULL*/)
{
	CWnd* pItem = GetDlgItem(IDC_ITEM);
	if (pItem != NULL)
	{
		CString csWndText(pszItem);
		GetConfiguration()->ReplaceText(csWndText);
		pItem->SetWindowText(csWndText.IsEmpty() ? "" : csWndText);
		pItem->UpdateWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CUninstallProgressDialog message handlers

BOOL CUninstallProgressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetConfiguration()->ReplaceDialogText(this);

	CenterWindow();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

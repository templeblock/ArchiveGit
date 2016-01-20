// ActAcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "ActAcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAccountAccessDlg dialog


CAccountAccessDlg::CAccountAccessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAccountAccessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAccountAccessDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAccountAccessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccountAccessDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAccountAccessDlg, CDialog)
	//{{AFX_MSG_MAP(CAccountAccessDlg)
	ON_BN_CLICKED(IDC_ACCESSEXISTINGACCOUNTRADIO, OnAccessExistingAccount)
	ON_BN_CLICKED(IDC_CREATEACCOUNTRADIO, OnCreateAccount)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccountAccessDlg message handlers

void CAccountAccessDlg::OnAccessExistingAccount() 
{
	m_fNew = FALSE;
	
}

void CAccountAccessDlg::OnCreateAccount() 
{
	m_fNew = TRUE;
	
}

BOOL CAccountAccessDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CButton* pButton = (CButton*)GetDlgItem(IDC_CREATEACCOUNTRADIO);
	
	if(m_fNew)
		pButton->SetCheck(1);
	else
		pButton->SetCheck(0);


	GET_PMWAPP()->WakeWindow();  //Bring us to top

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

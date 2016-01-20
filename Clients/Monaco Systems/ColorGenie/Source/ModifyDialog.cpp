// ModifyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "colorgenie.h"
#include "ModifyDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyDialog dialog


CModifyDialog::CModifyDialog(BOOL	allowmodify, CWnd* pParent /*=NULL*/)
	: CDialog(CModifyDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyDialog)
	//}}AFX_DATA_INIT
	m_allowmodify = allowmodify;
}


void CModifyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyDialog)
	DDX_Control(pDX, IDC_CLEAR, m_bClear);
	DDX_Control(pDX, IDOK, m_bOk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyDialog, CDialog)
	//{{AFX_MSG_MAP(CModifyDialog)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyDialog message handlers

void CModifyDialog::OnClear() 
{
	// TODO: Add your control notification handler code here
	if (!UpdateData(TRUE))
	{
		TRACE0("UpdateData failed during dialog termination.\n");
		// the UpdateData routine will set focus to correct item
		return;
	}
	EndDialog(IDC_CLEAR);	
}

BOOL CModifyDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bOk.EnableWindow(m_allowmodify);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

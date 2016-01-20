// ordfail.cpp : implementation file
//

#include "stdafx.h"
#include "upgrade.h"
#include "ordfail.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COrderFailedDialog dialog


COrderFailedDialog::COrderFailedDialog(LPCSTR pszReason, CWnd* pParent /*=NULL*/)
	: CBaseDialog(COrderFailedDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COrderFailedDialog)
	m_csReason = pszReason;
	//}}AFX_DATA_INIT
}

void COrderFailedDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrderFailedDialog)
	DDX_Text(pDX, IDC_REASON, m_csReason);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COrderFailedDialog, CBaseDialog)
	//{{AFX_MSG_MAP(COrderFailedDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COrderFailedDialog message handlers

BOOL COrderFailedDialog::OnInitDialog()
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	// If there is no help number, get rid of the reference.
	CWnd* pHelp = GetDlgItem(IDC_HELP_INFO);
	if (pHelp != NULL)
	{
		CInternationalData* pInternationalData = GetApp()->GetInternationalData();
		if (pInternationalData != NULL)
		{
			if (!pInternationalData->m_InstantSales.m_Help.Exists())
			{
				pHelp->ShowWindow(SW_HIDE);
			}
		}
	}
	
	CenterWindow();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

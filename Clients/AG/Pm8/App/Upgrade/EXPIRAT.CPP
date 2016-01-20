// expirat.cpp : implementation file
//

#include "stdafx.h"
#include "upgrade.h"
#include "expirat.h"

#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExpirationDateDialog dialog


CExpirationDateDialog::CExpirationDateDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CExpirationDateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExpirationDateDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CExpirationDateDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpirationDateDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExpirationDateDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CExpirationDateDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExpirationDateDialog message handlers

BOOL CExpirationDateDialog::OnInitDialog()
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

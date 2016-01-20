// already.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "already.h"

#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlreadyRegisteredDialog dialog


CAlreadyRegisteredDialog::CAlreadyRegisteredDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CAlreadyRegisteredDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlreadyRegisteredDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAlreadyRegisteredDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlreadyRegisteredDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlreadyRegisteredDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CAlreadyRegisteredDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAlreadyRegisteredDialog message handlers

BOOL CAlreadyRegisteredDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

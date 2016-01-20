// ArtExistsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "ArtExistsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArtExistsDlg dialog


CArtExistsDlg::CArtExistsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CArtExistsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArtExistsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
 

void CArtExistsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArtExistsDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArtExistsDlg, CDialog)
	//{{AFX_MSG_MAP(CArtExistsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArtExistsDlg message handlers


#include "stdafx.h"
#include "MonacoView.h"
#include "CalibrateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CCalibrateDlg, CDialog)
	//{{AFX_MSG_MAP(CCalibrateDlg)
	ON_BN_CLICKED(IDC_OK, OnOK)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalibrateDlg dialog


/////////////////////////////////////////////////////////////////////////////
CCalibrateDlg::CCalibrateDlg(CWnd* pParent /*=NULL*/) : CDialog(CCalibrateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalibrateDlg)
	//}}AFX_DATA_INIT

}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalibrateDlg)
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnCancel() 
{
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnOK() 
{
	CDialog::OnOK();
}

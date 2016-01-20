#include "stdafx.h"
#include "MonacoView.h"
#include "PreferenceDlg.h"
#include "PatchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPreferenceDlg, CDialog)
	//{{AFX_MSG_MAP(CPreferenceDlg)
	ON_BN_CLICKED(IDC_VAL_PATCH, OnValPatch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferenceDlg dialog


/////////////////////////////////////////////////////////////////////////////
CPreferenceDlg::CPreferenceDlg(double* pLab, double* pDeltaE, CWnd* pParent /*=NULL*/) : CDialog(CPreferenceDlg::IDD, pParent)
{
//j	m_has_photo = (short)_data->getphototable();
//j	m_has_icc = (short)_data->geticctable();
	m_pDeltaE = pDeltaE;
	m_pLab = pLab;
	//{{AFX_DATA_INIT(CPreferenceDlg)
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
void CPreferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferenceDlg)
	DDX_Control(pDX, IDC_CHECK_PHOTO, m_check_photo);
	DDX_Control(pDX, IDC_CHECK_ICC, m_check_icc);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPreferenceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_check_icc.SetCheck(1);
	m_check_photo.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CPreferenceDlg::OnValPatch() 
{
	CPatchDlg PatchDlg(m_pLab, m_pDeltaE);
	PatchDlg.DoModal();
}

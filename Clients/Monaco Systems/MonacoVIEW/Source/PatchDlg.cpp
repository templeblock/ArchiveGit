#include "stdafx.h"
#include "MonacoView.h"
#include "PatchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define m_pLab(row, col) *(m_lab + ((row*3) + col))
#define m_pDeltaE(row) *(m_deltaE + row)

BEGIN_MESSAGE_MAP(CPatchDlg, CDialog)
	//{{AFX_MSG_MAP(CPatchDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEditL)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeEditA)
	ON_EN_CHANGE(IDC_EDIT3, OnChangeEditB)
	ON_EN_CHANGE(IDC_EDIT4, OnChangeEditDeltaE)
	ON_BN_CLICKED(IDC_PATCH1, OnPatch1)
	ON_BN_CLICKED(IDC_PATCH2, OnPatch2)
	ON_BN_CLICKED(IDC_PATCH3, OnPatch3)
	ON_BN_CLICKED(IDC_PATCH4, OnPatch4)
	ON_BN_CLICKED(IDC_PATCH5, OnPatch5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchDlg dialog


/////////////////////////////////////////////////////////////////////////////
CPatchDlg::CPatchDlg(double* pLab, double* pDeltaE, CWnd* pParent /*=NULL*/) : CDialog(CPatchDlg::IDD, pParent)
{
	m_iCurrent = 1;
	m_deltaE = pDeltaE;
	m_lab = pLab;

	// This set of data are the actual RGB-intensity curves of
	// the display monitor.  They are necessary for generating
	// the gamma table which enables simulating the target values.

	//xyColorType m_MonitorData.targetWhitePoint;
	//RGBNumber m_MonitorData.targetGamma;
	//IntensityTable m_MonitorData.redCurve[NUM_INTENSITY_POINTS];
	//IntensityTable m_MonitorData.greenCurve[NUM_INTENSITY_POINTS];
	//IntensityTable m_MonitorData.blueCurve[NUM_INTENSITY_POINTS];

//j	m_DataConvert.reset(m_MonitorData);
	//{{AFX_DATA_INIT(CPatchDlg)
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchDlg)
	DDX_Control(pDX, IDC_RADIO1, m_radio1);
	DDX_Control(pDX, IDC_RADIO2, m_radio2);
	DDX_Control(pDX, IDC_RADIO3, m_radio3);
	DDX_Control(pDX, IDC_RADIO4, m_radio4);
	DDX_Control(pDX, IDC_RADIO5, m_radio5);
	DDX_Control(pDX, IDC_EDIT1, m_editL);
	DDX_Control(pDX, IDC_EDIT2, m_editA);
	DDX_Control(pDX, IDC_EDIT3, m_editB);
	DDX_Control(pDX, IDC_EDIT4, m_editDeltaE);
	DDX_Control(pDX, IDC_PATCH_WARN1, m_patch_warn[0]);
	DDX_Control(pDX, IDC_PATCH_WARN2, m_patch_warn[1]);
	DDX_Control(pDX, IDC_PATCH_WARN3, m_patch_warn[2]);
	DDX_Control(pDX, IDC_PATCH_WARN4, m_patch_warn[3]);
	DDX_Control(pDX, IDC_PATCH_WARN5, m_patch_warn[4]);
	DDX_Control(pDX, IDC_PATCH1, m_patch[0]);
	DDX_Control(pDX, IDC_PATCH2, m_patch[1]);
	DDX_Control(pDX, IDC_PATCH3, m_patch[2]);
	DDX_Control(pDX, IDC_PATCH4, m_patch[3]);
	DDX_Control(pDX, IDC_PATCH5, m_patch[4]);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPatchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_radio1.SetCheck(true);
	m_radio2.SetCheck(false);
	m_radio3.SetCheck(false);
	m_radio4.SetCheck(false);
	m_radio5.SetCheck(false);
	
	m_iCurrent = 1;
	int i = m_iCurrent - 1;

	CString szString;
	szString.Format("%.1f", m_pLab(i,0)); m_editL.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,1)); m_editA.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,2)); m_editB.SetWindowText(szString);
	szString.Format("%.1f", m_pDeltaE(i)); m_editDeltaE.SetWindowText(szString);

	for (i=0; i<5; i++)
		m_patch[i].SetLABColor(&m_pLab(i,0));
//j	_data->labtorgb((LabColorType *)_patchlab, patchrgb, outofgamut, 5);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnRadio1() 
{
	m_radio1.SetCheck(true);
	m_radio2.SetCheck(false);
	m_radio3.SetCheck(false);
	m_radio4.SetCheck(false);
	m_radio5.SetCheck(false);

	m_iCurrent = 1;
	int i = m_iCurrent - 1;

	CString szString;
	szString.Format("%.1f", m_pLab(i,0)); m_editL.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,1)); m_editA.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,2)); m_editB.SetWindowText(szString);
	szString.Format("%.1f", m_pDeltaE(i)); m_editDeltaE.SetWindowText(szString);
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnRadio2() 
{
	m_radio1.SetCheck(false);
	m_radio2.SetCheck(true);
	m_radio3.SetCheck(false);
	m_radio4.SetCheck(false);
	m_radio5.SetCheck(false);

	m_iCurrent = 2;
	int i = m_iCurrent - 1;

	CString szString;
	szString.Format("%.1f", m_pLab(i,0)); m_editL.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,1)); m_editA.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,2)); m_editB.SetWindowText(szString);
	szString.Format("%.1f", m_pDeltaE(i)); m_editDeltaE.SetWindowText(szString);
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnRadio3() 
{
	m_radio1.SetCheck(false);
	m_radio2.SetCheck(false);
	m_radio3.SetCheck(true);
	m_radio4.SetCheck(false);
	m_radio5.SetCheck(false);

	m_iCurrent = 3;
	int i = m_iCurrent - 1;

	CString szString;
	szString.Format("%.1f", m_pLab(i,0)); m_editL.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,1)); m_editA.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,2)); m_editB.SetWindowText(szString);
	szString.Format("%.1f", m_pDeltaE(i)); m_editDeltaE.SetWindowText(szString);
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnRadio4() 
{
	m_radio1.SetCheck(false);
	m_radio2.SetCheck(false);
	m_radio3.SetCheck(false);
	m_radio4.SetCheck(true);
	m_radio5.SetCheck(false);

	m_iCurrent = 4;
	int i = m_iCurrent - 1;

	CString szString;
	szString.Format("%.1f", m_pLab(i,0)); m_editL.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,1)); m_editA.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,2)); m_editB.SetWindowText(szString);
	szString.Format("%.1f", m_pDeltaE(i)); m_editDeltaE.SetWindowText(szString);
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnRadio5() 
{
	m_radio1.SetCheck(false);
	m_radio2.SetCheck(false);
	m_radio3.SetCheck(false);
	m_radio4.SetCheck(false);
	m_radio5.SetCheck(true);

	m_iCurrent = 5;
	int i = m_iCurrent - 1;

	CString szString;
	szString.Format("%.1f", m_pLab(i,0)); m_editL.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,1)); m_editA.SetWindowText(szString);
	szString.Format("%.1f", m_pLab(i,2)); m_editB.SetWindowText(szString);
	szString.Format("%.1f", m_pDeltaE(i)); m_editDeltaE.SetWindowText(szString);
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnPatch1() 
{
	OnRadio1();
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnPatch2() 
{
	OnRadio2();
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnPatch3() 
{
	OnRadio3();
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnPatch4() 
{
	OnRadio4();
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnPatch5() 
{
	OnRadio5();
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnChangeEditL() 
{
	if (!m_iCurrent)
		return;
	int i = m_iCurrent - 1;

	CString szString;
	m_editL.GetWindowText(szString);
	m_pLab(i,0) = atof(szString);
	m_patch[i].SetLABColor(&m_pLab(i,0));
//j	_data->labtorgb((LabColorType *)_patchlab, patchrgb, outofgamut, 5);
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnChangeEditA() 
{
	if (!m_iCurrent)
		return;
	int i = m_iCurrent - 1;

	CString szString;
	m_editA.GetWindowText(szString);
	m_pLab(i,1) = atof(szString);
	m_patch[i].SetLABColor(&m_pLab(i,0));
//j	_data->labtorgb((LabColorType *)_patchlab, patchrgb, outofgamut, 5);
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnChangeEditB() 
{
	if (!m_iCurrent)
		return;
	int i = m_iCurrent - 1;

	CString szString;
	m_editB.GetWindowText(szString);
	m_pLab(i,2) = atof(szString);
	m_patch[i].SetLABColor(&m_pLab(i,0));
//j	_data->labtorgb((LabColorType *)_patchlab, patchrgb, outofgamut, 5);
}

/////////////////////////////////////////////////////////////////////////////
void CPatchDlg::OnChangeEditDeltaE() 
{
	if (!m_iCurrent)
		return;
	int i = m_iCurrent - 1;

	CString szString;
	m_editDeltaE.GetWindowText(szString);
	m_pDeltaE(i) = atof(szString);
}

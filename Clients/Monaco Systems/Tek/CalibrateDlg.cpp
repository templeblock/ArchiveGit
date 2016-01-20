// Calibrate.cpp : implementation file
//

#include "stdafx.h"
#include "Tek.h"
#include "CalibrateDlg.h"
#include "CurveWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
class CProfileDoc : public CDocument
{
public:
	int GetCalibrateData( CMYK iCurve, double* x, double* y, int nMaxPoints ) { return 0; };
	BOOL SetCalibrateData( CMYK iCurve, double* x, double* y, int nPoints ) { return false; };
};

/////////////////////////////////////////////////////////////////////////////
static CProfileDoc* GetProfileDoc(CDialog* p)
{
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CCalibrateDlg dialog

BEGIN_MESSAGE_MAP(CCalibrateDlg, CDialog)
	//{{AFX_MSG_MAP(CCalibrateDlg)
	ON_BN_CLICKED(IDC_BLACK, OnBlack)
	ON_BN_CLICKED(IDC_CYAN, OnCyan)
	ON_BN_CLICKED(IDC_MAGENTA, OnMagenta)
	ON_BN_CLICKED(IDC_YELLOW, OnYellow)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_MESSAGE(WM_USER, OnCurveChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CCalibrateDlg::CCalibrateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalibrateDlg::IDD, pParent)
{
	m_bShowCurveCyan    = FALSE;
	m_bShowCurveMagenta = FALSE;
	m_bShowCurveYellow  = FALSE;
	m_bShowCurveBlack   = FALSE;
	m_pCurveWindow = NULL;
	//{{AFX_DATA_INIT(CCalibrateDlg)
	//}}AFX_DATA_INIT

	CCurveWindow::RegisterControlClass();
}

/////////////////////////////////////////////////////////////////////////////
CCalibrateDlg::~CCalibrateDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalibrateDlg)
	DDX_Control(pDX, IDC_LEVELIN, m_editLevelIn);
	DDX_Control(pDX, IDC_LEVELOUT, m_editLevelOut);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCalibrateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Initialize the curve control
	m_pCurveWindow = (CCurveWindow*)GetDlgItem(IDC_CURVE);
	ASSERT(m_pCurveWindow);
	m_pCurveWindow->Init(TRUE/*bColorImage*/, &m_editLevelIn, &m_editLevelOut );
	m_pCurveWindow->SetPercent(FALSE);
	m_pCurveWindow->SetEnableEdits(true);
	
	// Init the curve checkboxes (be sure this is after m_pCurveWindow is set)
	m_bShowCurveCyan    = FALSE; OnCyan();
	m_bShowCurveMagenta = FALSE; OnMagenta();
	m_bShowCurveYellow  = FALSE; OnYellow();
	m_bShowCurveBlack   = FALSE; OnBlack();

	GetDataFromDoc(NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Dlgs should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::GetDataFromDoc(CProfileDoc* pDoc)
{
	if (!pDoc)
		pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return;

	// 1. Get the data from the document
	// 2. Copy the points to the curve control
	for (int i = 0; i < 4; i++)
	{
		double fIndex[MaxNumPoints];
		double fValue[MaxNumPoints];

		int nPoints = pDoc->GetCalibrateData( (CMYK)i, fIndex, fValue, MaxNumPoints );
		m_pCurveWindow->SetPoints( (CMYK)i, fIndex, fValue, nPoints );
	}

	UpdateData(FALSE);	
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnBlack() 
{
	m_bShowCurveBlack = !m_bShowCurveBlack;
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_BLACK);
	pCheckBox->SetCheck(m_bShowCurveBlack);
	m_pCurveWindow->ShowCurve(CI_GAMMA, m_bShowCurveBlack);
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnCyan() 
{
	m_bShowCurveCyan = !m_bShowCurveCyan;
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CYAN);
	pCheckBox->SetCheck(m_bShowCurveCyan);
	m_pCurveWindow->ShowCurve(CI_CYAN, m_bShowCurveCyan);
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnMagenta() 
{
	m_bShowCurveMagenta = !m_bShowCurveMagenta;
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_MAGENTA);
	pCheckBox->SetCheck(m_bShowCurveMagenta);
	m_pCurveWindow->ShowCurve(CI_MAGENTA, m_bShowCurveMagenta);
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnYellow() 
{
	m_bShowCurveYellow = !m_bShowCurveYellow;
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_YELLOW);
	pCheckBox->SetCheck(m_bShowCurveYellow);
	m_pCurveWindow->ShowCurve(CI_YELLOW, m_bShowCurveYellow);
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnReset() 
{
	m_pCurveWindow->SetToDefaults();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCalibrateDlg::OnCurveChange(WPARAM wParam, LPARAM lParam) 
{
	// Stuff the doc with the data passed from the curve control
	// Collect the data from the control
	int iCurve = LOWORD(wParam);
	int nPoints = HIWORD(wParam);

	// Update the document with the new points
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (pDoc)
	{
		double fIndex[MaxNumPoints];
		double fValue[MaxNumPoints];
		int nPoints = m_pCurveWindow->GetPoints( (CMYK)iCurve, fIndex, fValue, MaxNumPoints );
		pDoc->SetCalibrateData( (CMYK)iCurve, fIndex, fValue, nPoints);
		return TRUE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::ClearData()
{
	int i;
	double x[200], y[200];

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	for(i = 0; i < 4; i++)
		pDoc->SetCalibrateData( (CMYK)i, x, y, 0);

	GetDataFromDoc(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCalibrateDlg::HasData()
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return	FALSE;

	int nPoints;
	double fIndex[MaxNumPoints];
	double fValue[MaxNumPoints];

	nPoints = pDoc->GetCalibrateData( (CMYK)0, fIndex, fValue, MaxNumPoints );
	return (nPoints > 0);	
}

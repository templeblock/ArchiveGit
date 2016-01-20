// ViewDataDialog.cpp : implementation file
//

#include "stdafx.h"
#include "colorgenie.h"
#include "ViewDataDialog.h"
#include "ProfileView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Message Map
BEGIN_MESSAGE_MAP(CViewDataDialog, CDialog)
	//{{AFX_MSG_MAP(CViewDataDialog)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Event Map
BEGIN_EVENTSINK_MAP(CViewDataDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CViewDataDialog)
	ON_EVENT(CViewDataDialog, IDC_GRID, -600 /* Click */, OnGridClick, VTS_NONE)
	ON_EVENT(CViewDataDialog, IDC_GRID, -602 /* KeyDown */, OnGridKeyDown, VTS_PI2 VTS_I2)
	ON_EVENT(CViewDataDialog, IDC_GRID, 70 /* RowColChange */, OnGridRowColChange, VTS_NONE)
	ON_EVENT(CViewDataDialog, IDC_GRID, 69 /* SelChange */, OnGridSelChange, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// ViewDataDialog dialog


/////////////////////////////////////////////////////////////////////////////
CViewDataDialog::CViewDataDialog(CProfileDoc* pDoc, int iType, CWnd* pParent /*=NULL*/)
	: CDialog(CViewDataDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewDataDialog)
	m_iType = iType;
	m_pDoc = pDoc;
	m_strPatch = _T("");
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
void CViewDataDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewDataDialog)
	DDX_Control(pDX, IDC_GRID, m_ctlGrid);
	DDX_Text(pDX, IDC_PATCH_NUM, m_strPatch);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CViewDataDialog message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CViewDataDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Initialize the grid control
	//m_ctlGrid.InitLAB(1487);

	BeginWaitCursor();
	GetDataFromDoc(m_pDoc);
	EndWaitCursor();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CViewDataDialog::GetDataFromDoc(CProfileDoc* pDoc) 
{
	if (!pDoc || !IsWindow(this->m_hWnd))
		return;

	// 1. Get the patch data from the document
	// 2. Copy the points to the grid control
	double* pfIndex = NULL;
	double* pfL = NULL;
	double* pfa = NULL;
	double* pfb = NULL;
	int		nLinear;

	// Determine which type of file it is
	CString szTitle;
	int nPoints;
	if (!m_iType)
	{
		nPoints = pDoc->GetColorSourceData( &pfIndex, &pfL, &pfa, &pfb, &nLinear );
		pDoc->GetColorSourceDescription(szTitle);
	}
	else
	{
		nPoints = pDoc->GetColorDryjetData( &pfIndex, &pfL, &pfa, &pfb, &nLinear );
		pDoc->GetColorDryjetDescription(szTitle);
	}
	
	if (!szTitle.IsEmpty())
	{ // Update the window's title bar
		CString szDescription;
		GetWindowText(szDescription);
		SetWindowText(szDescription + " - " + szTitle);
	}

	// Initialize the grid control
	char* rgb;
	if(!m_iType)
		rgb = pDoc->GetRgbs(1);
	else
		rgb = pDoc->GetRgbs(2);

	m_ctlGrid.InitLAB(nPoints, rgb);
	if (pfIndex && pfL && pfa && pfb)
	{
		m_ctlGrid.SetColumn( 0/*iColumn*/, pfIndex, pfL, nPoints, 100.0 );
		m_ctlGrid.SetColumn( 1/*iColumn*/, pfIndex, pfa, nPoints, 100.0 );
		m_ctlGrid.SetColumn( 2/*iColumn*/, pfIndex, pfb, nPoints, 100.0 );
	}
	m_ctlGrid.InitLABRowLabels(0, nPoints - nLinear - 1);

	//convert to rgb values
	char cmyk[800];
	CApp* app = (CApp*)AfxGetApp( );
	if(app && app->mhasCmm && (0 < nLinear) && (nLinear < 200) )	
	{
		int nL = nLinear/4;
		double *density = &pfL[nPoints - nLinear];
		for(int i = 0; i < 4; i++)
			DensityToRgb(cmyk + i*nL*4, density+i*nL, nL, i);

		app->mcmmConvert.ApplyXform(cmyk, cmyk, 8, nLinear);
		m_ctlGrid.InitCMYKRowLabels(1, nPoints - nLinear, nPoints - 1, cmyk);
	} 
	m_ctlGrid.GoHome();

	delete [] pfIndex;
	delete [] pfL;
	delete [] pfa;
	delete [] pfb;

	BOOL bEnable = (nPoints > 0);
	CButton* pButton = (CButton*)GetDlgItem(IDC_EXPORT);
	if (pButton) pButton->EnableWindow(bEnable);
	pButton = (CButton*)GetDlgItem(IDC_VIEW_GAMUT);
	if (pButton) pButton->EnableWindow(bEnable);
}

/////////////////////////////////////////////////////////////////////////////
void CViewDataDialog::DensityToRgb(char *cmyk, double *density, int num, int which)
{
	double ink;
	int i, j, k;

	for(i = 0; i < num; i++)
	{
		j = i*4;
		for(k = 0; k < 4; k++)
			cmyk[j+k] = 0;
		ink = 10000*density[i]/1.5;
		if(ink > 100) ink = 100;
		if(ink < 0)	ink = 0;

		cmyk[j+which] = (char)(ink*2.55);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CViewDataDialog::OnGridKeyDown(short* pKeyCode, short Shift)
{
	// Uncomment this code if you want the grid control to be editable
	// Remember that you must copy any edits the user makes back to print data
	// in order for them to get saved
	#ifdef NOTUSED 
	CRect rect;
	m_ctlGrid.ProcessKey(pKeyCode, Shift, &rect);
	for (int x = rect.left; x <= rect.right; x++ )
	{ // For each of the columns representing a color curve...
		// Get the column values from the grid
		// Values should be doubles less than 100.0%
		double pdXArray[101], pdYArray[101];
		int nPoints = 101;
		nPoints = m_ctlGrid.GetColumn(x/*iColumn*/, pdXArray, pdYArray, nPoints, 100.0);

		// Update the document with the new points
		CProfileDoc* pDoc = GetProfileDoc(this);
		if (pDoc)
			pDoc->SetCalibrateData((CMYK)x, pdXArray, pdYArray, nPoints);
	}

	m_ctlGrid.InitLABRowLabels(rect.top, rect.bottom);
	#endif NOTUSED
}

/////////////////////////////////////////////////////////////////////////////
void CViewDataDialog::OnGridClick()
{
	int selectrow = m_ctlGrid.GetRowSel() - 1;
	char patchstr[10];
	sprintf(patchstr, "%d", selectrow+1);
	m_strPatch = patchstr;
	UpdateData(FALSE);
//	m_ctlGrid.ProcessClick();
}


/////////////////////////////////////////////////////////////////////////////
void CViewDataDialog::OnGridRowColChange() 
{
	int selectrow = m_ctlGrid.GetRowSel() - 1;
	char patchstr[10];
	sprintf(patchstr, "%d", selectrow+1);
	m_strPatch = patchstr;
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CViewDataDialog::OnGridSelChange() 
{
}

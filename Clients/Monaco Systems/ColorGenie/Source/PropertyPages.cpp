// PropertyDlgs.cpp : implementation file
//
#include "stdafx.h"
#include <time.h>

#include "ColorGenie.h"
#include "PropertyDlgs.h"
#include "ProfilerDoc.h"
#include "ProfileView.h"
#include "ProfileSheet.h"
#include "Message.h"
#include "MeasureDialog.h"
#include "ViewDataDialog.h"
#include "BuildProgress.h"
#include "Winmisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
static BOOL IsEmpty( char* p, int iSize )
{
	for ( int i = 0; i < iSize; i++ )
	{
		char c = p[i];
		if ( !c ) // end of string
			break;
		if ( c > ' ' ) // it's not whitespace
			return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
static void ExportCMYKFile( CString& szFileName, CGrid* pGrid )
{
	ofstream out;
	out.open(szFileName);
	if (out.bad())
		return;

	pGrid->OutputCMYK(out);
	out.close();
}

/////////////////////////////////////////////////////////////////////////////
static CProfileView* GetView( CDialog* pDlgDialog )
{
	if (!pDlgDialog)
		return NULL;

	CWnd* pPropertySheet = pDlgDialog->GetParent();
	if (!pPropertySheet)
		return NULL;

	CProfileView* pView = (CProfileView*)pPropertySheet->GetParent();
	if (!pView)
		return NULL;

	return pView;
}

/////////////////////////////////////////////////////////////////////////////
static CProfileDoc* GetProfileDoc( CDialog* pDlgDialog )
{
	if (!pDlgDialog)
		return NULL;

	CWnd* pPropertySheet = pDlgDialog->GetParent();
	if (!pPropertySheet)
		return NULL;

	CProfileView* pView = (CProfileView*)pPropertySheet->GetParent();
	if (!pView)
		return NULL;

	CProfileDoc* pDoc = pView->GetDocument();
	if (!pDoc)
		return NULL;

	return pDoc;
}

/////////////////////////////////////////////////////////////////////////////
static ProfileDocFiles* GetDoc( CDialog* pDlgDialog )
{
	CProfileDoc* pDoc = GetProfileDoc(pDlgDialog);
	if (!pDoc)
		return NULL;
	return &pDoc->_profiledocfiles;
}

/////////////////////////////////////////////////////////////////////////////
void SetTime(CTime *ct, CString& sz)
{
	char *date[] = {"Error", "Sun", "Mon", "Tue", "Wen", "Thu", "Fri", "Sat"};
	char *mon[] = {"Error", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	 
	sz.Format( "%s, %s %d, %d, %d:%d:%d", date[ct->GetDayOfWeek()],
 										  mon[ct->GetMonth()],
										  ct->GetDay(),
										  ct->GetYear(),
										  ct->GetHour(),
										  ct->GetMinute(), 
										  ct->GetSecond() );
}

#define MEASURING			1
#define STOPED				2

#include "Xrite408.h"
#include "GretagScan.h"
#include "GretagScanDensity.h"
#include "TechkonTable.h"
#include "ModifyDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CCalibrateDlg property page

IMPLEMENT_DYNCREATE(CCalibrateDlg, CPropertyDlg)

// Message Map
BEGIN_MESSAGE_MAP(CCalibrateDlg, CPropertyDlg)
	//{{AFX_MSG_MAP(CCalibrateDlg)
	ON_EN_CHANGE(IDC_DESCRIPTION, OnDescriptionChange)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_MEASURE, OnMeasure)
	ON_BN_CLICKED(IDC_BLACK, OnBlack)
	ON_BN_CLICKED(IDC_CYAN, OnCyan)
	ON_BN_CLICKED(IDC_MAGENTA, OnMagenta)
	ON_BN_CLICKED(IDC_YELLOW, OnYellow)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_MESSAGE(WM_USER, OnCurveChange)
	ON_MESSAGE(WM_DATA_READY, OnDataReady)
	ON_MESSAGE(WM_COMM_REQUEST_COMPLETE, OnRequestComplete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Event Map
BEGIN_EVENTSINK_MAP(CCalibrateDlg, CPropertyDlg)
    //{{AFX_EVENTSINK_MAP(CCalibrateDlg)
	ON_EVENT(CCalibrateDlg, IDC_GRID, -600 /* Click */, OnGridClick, VTS_NONE)
	ON_EVENT(CCalibrateDlg, IDC_GRID, -602 /* KeyDown */, OnGridKeyDown, VTS_PI2 VTS_I2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
CCalibrateDlg::CCalibrateDlg() : CPropertyDlg(CCalibrateDlg::IDD)
{
	m_bShowCurveCyan    = FALSE;
	m_bShowCurveMagenta = FALSE;
	m_bShowCurveYellow  = FALSE;
	m_bShowCurveBlack   = FALSE;
	m_pCurveWindow = NULL;
	m_pGrid = NULL;
	//{{AFX_DATA_INIT(CCalibrateDlg)
	m_date = _T("");
	//}}AFX_DATA_INIT

	m_pdevice = NULL;
	m_runningstatus = STOPED;
	m_measureFlag = 0;
	m_currentRow = 0;
	m_currentCol = 0;
	m_currentPatch = 0;

	//paper density
	for(int i = 0; i < 4; i++)
		m_paper[i] = 0.0;
	m_haspaper = FALSE;			

	m_cyanStartPatches = 0;
	m_magentaStartPatches = 26;
	m_yellowStartPatches = 26*2;
	m_blackStartPatches = 26*3;

	m_totalPatches = 26*4;

	CCurveWindow::RegisterControlClass();
}

/////////////////////////////////////////////////////////////////////////////
CCalibrateDlg::~CCalibrateDlg()
{
	CCalibrateDlg::CleanupDevice();
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalibrateDlg)
	DDX_Control(pDX, IDC_MEASURE, m_bMeasure);
	DDX_Control(pDX, IDC_DESCRIPTION, m_editDescription);
	DDX_Control(pDX, IDC_GRID, m_ctlGrid);
	DDX_Control(pDX, IDC_LEVELIN, m_editLevelIn);
	DDX_Control(pDX, IDC_LEVELOUT, m_editLevelOut);
	DDX_Text(pDX, IDC_DATE, m_date);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCalibrateDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CCalibrateDlg::OnInitDialog() 
{
	CPropertyDlg::OnInitDialog();
	
	// Initialize the curve control
	m_pCurveWindow = (CCurveWindow*)GetDlgItem(IDC_CURVE);
	ASSERT(m_pCurveWindow);
	m_pCurveWindow->Init(TRUE/*bColorImage*/, &m_editLevelIn, &m_editLevelOut );
	m_pCurveWindow->SetPercent(FALSE);
	m_pCurveWindow->SetEnableEdits(FALSE);
	
	// Init the curve checkboxes (be sure this is after m_pCurveWindow is set)
	m_bShowCurveCyan    = FALSE; OnCyan();
	m_bShowCurveMagenta = FALSE; OnMagenta();
	m_bShowCurveYellow  = FALSE; OnYellow();
	m_bShowCurveBlack   = FALSE; OnBlack();

	// Initialize the CMYK grid control
    m_pGrid = &m_ctlGrid;
	ASSERT(m_pGrid);
	m_pGrid->InitCMYK(26);

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
	// 2. Copy the points to the grid control
	// 3. Copy the points to the curve control
	int nPoints;
	double fIndex[MaxNumPoints];
	double fValue[MaxNumPoints];

	nPoints = pDoc->GetCalibrateData( (CMYK)0, fIndex, fValue, MaxNumPoints );
	if(nPoints) m_pGrid->InitCMYK(nPoints);

	for (int i = 0; i < 4; i++)
	{
		nPoints = pDoc->GetCalibrateData( (CMYK)i, fIndex, fValue, MaxNumPoints );
		m_pGrid->SetColumn( i/*iColumn*/, fIndex, fValue, nPoints, 2.0 );
		m_pCurveWindow->SetPoints( (CMYK)i, fIndex, fValue, nPoints );
	}

	CString szDescription;
	pDoc->GetCalibrateDescription( szDescription );
	m_editDescription.SetWindowText(szDescription);

	if(nPoints){
		long	date;
		pDoc->GetCalibrateDate(&date);
		if(date < 0)	date = 0;

		CTime	ctime((time_t)date); 
		SetTime(&ctime, m_date);
	}
	else
	{
		m_date = "";
	}

	UpdateData(FALSE);	

	m_currentPatch = 0;
	BOOL bEnable = (nPoints > 0);
	SetCheck(bEnable);
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::SetCheck(BOOL enable)
{
	CButton* pButton;

	pButton = (CButton*)GetDlgItem(IDC_EXPORT);
	if (pButton) pButton->EnableWindow(enable);
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnDescriptionChange() 
{
	if(m_runningstatus != STOPED)	return;

	CWnd* pWnd = GetFocus();
	if (&m_editDescription != pWnd)
		return;

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	char szBuffer[100];
	m_editDescription.GetWindowText(szBuffer, sizeof(szBuffer)-1);
	pDoc->SetCalibrateDescription( szBuffer );
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
void CCalibrateDlg::OnGridKeyDown(short* pKeyCode, short Shift)
{
	if (m_runningstatus != STOPED)
		return;

	CRect rect;
	if (!m_pGrid->ProcessKey(pKeyCode, Shift, &rect))
		return;

	for (int x = rect.left; x <= rect.right; x++ )
	{ // For each of the columns representing a color curve...
		// Get the column values from the grid
		// Values should be doubles less than 2.0 (density)
		double pdXArray[101], pdYArray[101];
		int nPoints = 101;
		nPoints = m_pGrid->GetColumn(x/*iColumn*/, pdXArray, pdYArray, nPoints, 2.0);
		m_pCurveWindow->SetPoints((CMYK)x, pdXArray, pdYArray, nPoints);

		// Update the document with the new points
		CProfileDoc* pDoc = GetProfileDoc(this);
		if (pDoc)
			pDoc->SetCalibrateData((CMYK)x, pdXArray, pdYArray, nPoints);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnGridClick()
{
	if( (m_runningstatus == MEASURING) && (m_measureFlag == 1) )	return;
	 
	m_currentRow = m_pGrid->GetRowSel() - 1;
	m_currentCol = m_pGrid->GetColSel() - 1;
	m_currentPatch = m_currentCol*26 + m_currentRow;

	m_pGrid->ProcessClick();
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCalibrateDlg::OnCurveChange(WPARAM wParam, LPARAM lParam) 
{
	// Stuff the grid and the doc with the data passed from the curve control
	// Collect the data from the control
	int iCurve = LOWORD(wParam);
	int nPoints = HIWORD(wParam);

	int iColumn = iCurve + m_pGrid->GetFixedCols();

	// If the lParam is FALSE, we are being notified of a point selection
	if (!lParam)
	{
		// the nPoints parameter is the current point's X value (row)
		if (nPoints < 0) nPoints = 0;
		if (nPoints > nSCALE) nPoints = nSCALE;

		int iRow = ((nPoints * 100) + nSCALE/2) / nSCALE;
		iRow += m_pGrid->GetFixedRows();
		
		m_pGrid->SetRow(iRow);
		m_pGrid->SetCol(iColumn);

		// Now make sure the row is within view
		#define ROWS_IN_VIEW 13

		if (iRow <  m_pGrid->GetTopRow() ||
			iRow >= m_pGrid->GetTopRow() + ROWS_IN_VIEW )
		{
			int iTopRow = iRow - (ROWS_IN_VIEW/2);
			if (iTopRow < m_pGrid->GetFixedRows() )
				iTopRow = m_pGrid->GetFixedRows();
			m_pGrid->SetTopRow(iTopRow);
		}
		
		return TRUE;
	}

	// Update the document with the new points
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (pDoc)
	{
		double fIndex[MaxNumPoints];
		double fValue[MaxNumPoints];
		int nPoints = m_pCurveWindow->GetPoints( (CMYK)iCurve, fIndex, fValue, MaxNumPoints );
		pDoc->SetCalibrateData( (CMYK)iCurve, fIndex, fValue, nPoints);
		m_pGrid->SetColumn( iCurve/*iColumn*/, fIndex, fValue, nPoints, 2.0 );
		return TRUE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnImport() 
{
	if(m_runningstatus != STOPED)	return;

	CFileDialog FileDialog(
		/*bOpenFileDialog*/	TRUE,
		/*lpszDefExt*/		"cal",
		/*lpszFileName*/	"",
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Calibration Data Files (*.cal;*.txt)|*.cal;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	CString dirname((LPCSTR)IDS_CAL_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	if (FileDialog.DoModal() != IDOK)
		return;

	UpdateWindow(); // clean up screen in case import takes time

	CString szFileName = FileDialog.GetPathName();

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (pDoc)
	{
		BeginWaitCursor();
		if (!pDoc->LoadLinear((char*)LPCTSTR(szFileName)))
		{
			EndWaitCursor();
			Message( "Can't load the selected file." );
			return;
		}

		GetDataFromDoc(pDoc);

		CString& sz = szFileName;
		int c;
		while ((c = sz.Find('\\')) >= 0)
			sz = sz.Mid(c+1);

		pDoc->SetCalibrateDescription(sz);
		m_editDescription.SetWindowText(sz);

		//date
		CFileStatus fstatus;
		if( CFile::GetStatus((LPCSTR)szFileName, fstatus ) )
		{
			time_t long_time;

			long_time = fstatus.m_ctime.GetTime();
			pDoc->SetCalibrateDate(long_time);

			SetTime(&fstatus.m_ctime, m_date);
			UpdateData(FALSE);	
		}

		EndWaitCursor();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnExport() 
{
	if(m_runningstatus != STOPED)	return;

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	CString szDescription;
	pDoc->GetCalibrateDescription( szDescription );	

	CFileDialog FileDialog(
		/*bOpenFileDialog*/	FALSE,
		/*lpszDefExt*/		"cal",
		/*lpszFileName*/	(LPCSTR)szDescription,
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Calibration Data Files (*.cal;*.txt)|*.cal;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	CString dirname((LPCSTR)IDS_CAL_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	if (FileDialog.DoModal() != IDOK)
		return;

	UpdateWindow(); // clean up screen in case import takes time

	CString szFileName = FileDialog.GetPathName();

	BeginWaitCursor();
	if (!pDoc->SaveLinear((char*)LPCTSTR(szFileName)))
	{
		EndWaitCursor();
		Message( "Can't load the selected file." );
		return;
	}

	CString& sz = szFileName;
	int c;
	while ((c = sz.Find('\\')) >= 0)
		sz = sz.Mid(c+1);

	pDoc->SetCalibrateDescription(sz);
	m_editDescription.SetWindowText(sz);

	CFileStatus fstatus;
	if( CFile::GetStatus((LPCSTR)szFileName, fstatus ) )
	{
		time_t long_time;
		pDoc->GetCalibrateDate(&long_time);
		fstatus.m_ctime	= long_time; 
		CFile::SetStatus(LPCTSTR(szFileName), fstatus);
	}

	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCalibrateDlg::OnDataReady(WPARAM wParam, LPARAM lParam)
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return false;

	double fI[256];	double f[256];
	int n = 0;

	//set data
	CGrid *pGrid = (CGrid*)wParam;
	n = pGrid->GetColumn(0, fI, f, 256, 2.0);
	pDoc->SetCalibrateData((CMYK)0, fI, f, n);
	n = pGrid->GetColumn(1, fI, f, 256, 2.0);
	pDoc->SetCalibrateData((CMYK)1, fI, f, n);
	n = pGrid->GetColumn(2, fI, f, 256, 2.0);
	pDoc->SetCalibrateData((CMYK)2, fI, f, n);
	n = pGrid->GetColumn(3, fI, f, 256, 2.0);
	pDoc->SetCalibrateData((CMYK)3, fI, f, n);

	//set description
	CString sz("Untitled.txt");
	pDoc->SetCalibrateDescription(sz);

	//set time
	CTime ctime = CTime::GetCurrentTime();
	time_t long_time = ctime.GetTime();
	pDoc->SetCalibrateDate(long_time);

	return TRUE;
}

/*
/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnMeasure() 
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;
	
	int patches[] = { 100, 0, 0, 0, 0 };	
	// TODO: Add your control notification handler code here
	CMeasureDialog	measuredialog(MEASURE_CURVE_DATA, patches, this);
	memcpy((char*)&(measuredialog.m_deviceSetup), (char*)&(pDoc->m_deviceSetup), sizeof(DeviceSetup));

	if(measuredialog.DoModal() == IDCANCEL)
		return;

	BeginWaitCursor();

	GetDataFromDoc(pDoc);

	EndWaitCursor();
}
*/

/////////////////////////////////////////////////////////////////////////////
BOOL CCalibrateDlg::OnMeasureDone(void)
{
	if(	m_measureFlag == 1 && m_currentPatch < m_totalPatches)
	{
		ClearData();
		return TRUE;
	}

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return false;

	double fI[256];	double f[256];
	int n = 0;

	//set data
	CGrid *pGrid = (CGrid*)&m_ctlGrid;
	n = pGrid->GetColumn(0, fI, f, 256, 2.0);
	pDoc->SetCalibrateData((CMYK)0, fI, f, n);
	n = pGrid->GetColumn(1, fI, f, 256, 2.0);
	pDoc->SetCalibrateData((CMYK)1, fI, f, n);
	n = pGrid->GetColumn(2, fI, f, 256, 2.0);
	pDoc->SetCalibrateData((CMYK)2, fI, f, n);
	n = pGrid->GetColumn(3, fI, f, 256, 2.0);
	pDoc->SetCalibrateData((CMYK)3, fI, f, n);

	if(m_measureFlag == 1)	//clear
	{
		//set description
		CString sz("UntitledCalibration.txt");
		pDoc->SetCalibrateDescription(sz);

		//set time
		CTime ctime = CTime::GetCurrentTime();
		time_t long_time = ctime.GetTime();
		pDoc->SetCalibrateDate(long_time);
	}

	BeginWaitCursor();

	GetDataFromDoc(pDoc);

	EndWaitCursor();

	return TRUE;
}

void CCalibrateDlg::CleanupDevice()
{
	if(m_pdevice){
		delete m_pdevice;
		m_pdevice = 0;
	}

	//paper density
	for(int i = 0; i < 4; i++)
		m_paper[i] = 0.0;
	m_haspaper = FALSE;			

	m_runningstatus = STOPED;
}

void CCalibrateDlg::ReadNextPatch()
{
	//terminate if current is larger than total
	if(m_currentPatch >= m_totalPatches){
		OnMeasureDone();
		CleanupDevice();
		m_bMeasure.SetWindowText("Measure");
		m_currentPatch = 0;
		UpdateData(FALSE);
	}
	else{
		SetParamForGrid();

		m_pdevice->SendRequest(m_currentPatch, (CWnd*)this);
		UpdateData(FALSE);
	}
}

//locate the Grid position for the patch
void CCalibrateDlg::SetParamForGrid()
{
	if(m_currentPatch < m_magentaStartPatches)
	{
		m_currentRow = m_currentPatch;
		m_currentCol = 0;
		m_dataStart = 0;
		m_pdevice->m_type = 2;
	}
	else if(m_currentPatch < m_yellowStartPatches)
	{
		m_currentRow = m_currentPatch - m_magentaStartPatches;
		m_currentCol = 1;
		m_dataStart = 1;
		m_pdevice->m_type = 2;
	}
	else if(m_currentPatch < m_blackStartPatches)
	{
		m_currentRow = m_currentPatch - m_yellowStartPatches;
		m_currentCol = 2;
		m_dataStart = 2;
		m_pdevice->m_type = 2;
	}
	else
	{
		m_currentRow = m_currentPatch - m_blackStartPatches;
		m_currentCol = 3;
		m_dataStart = 3;
		m_pdevice->m_type = 2;
	}

	//grid position
	int currentRow = m_currentRow+1;
	int currentCol = m_currentCol+1;

	// Now make sure the row is within view
	#define ROWS_IN_VIEW 13

	if (currentRow <  m_ctlGrid.GetTopRow() ||
		currentRow >= m_ctlGrid.GetTopRow() + ROWS_IN_VIEW )
	{
		int iTopRow = currentRow - (ROWS_IN_VIEW/2);
		if (iTopRow < m_ctlGrid.GetFixedRows() )
			iTopRow = m_ctlGrid.GetFixedRows();
		m_ctlGrid.SetTopRow(iTopRow);
	}

	m_ctlGrid.SetRow(currentRow);
	m_ctlGrid.SetCol(currentCol);
}

/******************************************************************************
* Name: CMeasureDialog::OnRequestComplete
*
* Purpose: handle the request returned by communication device
******************************************************************************/
afx_msg LRESULT CCalibrateDlg::OnRequestComplete(WPARAM wParam, LPARAM lParam)
{
	double	cmyk[4];

	CCommRequest *pRequest = (CCommRequest*)lParam;

	if(!m_pdevice || !pRequest)	return 0;

	//check error
	switch(pRequest->m_dwRequestError){
	case(ERR_NOERROR):
		break;

	case(ERR_CANCELLED):
		//delete pRequest;
		return 0;

	case(ERR_MAXTRIES):
		//delete pRequest;
		ErrorMessage(ERR_TIMEOUT);
		return 0;

	case(ERR_TERMINATED):
		return 1;

	default:
		//delete pRequest;
		ErrorMessage(ERR_DEVICE);
		return 0;
	}

	int status = 0;
	//no error, then put the data
	if(m_pdevice->ProcessResult(pRequest, cmyk))
	{
		if(	m_haspaper == FALSE )
		{			
			SetPaperDensity(cmyk);
			m_haspaper = TRUE;
			if(	m_deviceSetup.deviceType == XRITE_408 )	//xrite 408
			{
				m_firstdialog.EndDialog(IDOK);
			}
			ReadNextPatch();	
		}
		else
		{
			cmyk[m_dataStart] -= m_paper[m_dataStart];

			status = AdjustDensity(&cmyk[m_dataStart]);
			if(status == 0)
			{
				PutonGrid(m_currentRow+1, m_currentCol+1, m_currentCol+1+1, &cmyk[m_dataStart]);

				m_currentPatch++;

				if( !(m_currentPatch % m_deviceSetup.calFrequency) )
					if(!m_pdevice->Calibrate())	return 0;

				ReadNextPatch();	
			}
			else if(status == 1)
			{
				OnMeasure();
				return 0;
			}
			else
			{
				ReadNextPatch();	
			}
		}

	}

	delete pRequest;

	return 1;
}

void CCalibrateDlg::SetPaperDensity(double* cmyk)
{
	for(int i = 0; i < 4; i++)
		m_paper[i] = cmyk[i];	
}

int CCalibrateDlg::AdjustDensity(double *density)
{
	BOOL	status = 0;		//0 ok, 1 cancel, 2 continue

	//zero density should be zero
	if( m_currentPatch % 26 == 0)
	{
		if(abs(*density) <= 0.035)
			*density = 0;
		else
		{
			int selected = AfxMessageBox( IDS_ERR_DENSITY, MB_YESNO|MB_ICONSTOP);
			if(selected == IDNO) status = 1;
			else status = 2;
		}
	}
	else
	{
		if(*density < 0)
		{
			int selected = AfxMessageBox( IDS_ERR_DENSITY, MB_YESNO|MB_ICONSTOP);
			if(selected == IDNO) status = 1;
			else status = 2;
		}
	}

	return status;
}

void CCalibrateDlg::PutonGrid(int row, int startcol, int endcol, double *data)
{
	
	for (int col=startcol; col < endcol; col++)
	{
		CString sz;
		if(startcol == endcol - 1)
			sz.Format( "%g", data[0]);
		else
			sz.Format( "%g", data[col - startcol] );
		m_ctlGrid.SetTextMatrix(row, col, sz);
	}
}

BOOL CCalibrateDlg::InitReading()
{

	CleanupDevice();

	//check is any port available
	if(m_deviceSetup.numPorts == 0){
		ErrorMessage(ERR_NOPORT);
		return FALSE;
	}

	//create device
	switch(m_deviceSetup.deviceType){
	case(XRITE_408):
		m_pdevice = new CXrite408();
		break;

	case(GRETAG_SCAN):
		m_pdevice = new CGretagScanDensity();
		break;

	case(TECKON_TABLE):
		m_pdevice = new CTechkonTable();
		break;
	}

	if(!m_pdevice){
		ErrorMessage(ERR_MEMORY);
		return FALSE;
	}

	//open port
	if(!m_pdevice->Open(m_deviceSetup.portNames[m_deviceSetup.port], m_deviceSetup.baudRate))
	{
		ErrorMessage(ERR_NOPORT);
		CleanupDevice();
		return FALSE;
	}

	//init the port
	if( !m_pdevice->StartComm()){
		CleanupDevice();
		return FALSE;
	}

	//read the patch;
//	ReadNextPatch();

//	return TRUE;
	
	return GetPaperDensity();	
}

//if ok, return TRUE;
BOOL CCalibrateDlg::GetPaperDensity()
{
	switch(m_deviceSetup.deviceType){
	case(XRITE_408):
		m_pdevice->SendRequest(0, (CWnd*)this);
		if(m_firstdialog.DoModal() == IDCANCEL)
			return FALSE;
		else
			return TRUE;
		break;

	case(GRETAG_SCAN):
		//m_currentRow = m_currentPatch;
		//m_currentCol = 0;
		//m_dataStart = 0;
		m_pdevice->m_type = 2;
		m_pdevice->SendRequest(0, (CWnd*)this);
		return	TRUE;
		break;

	case(TECKON_TABLE):
		return FALSE;
		break;
	}
	
	return FALSE;
}

void CCalibrateDlg::ClearData()
{
	int i;
	double x[200], y[200];

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	m_pGrid->InitCMYK(26);
	for(i = 0; i < 4; i++)
		pDoc->SetCalibrateData( (CMYK)i, x, y, 0);

	char szBuffer[100];
	m_editDescription.GetWindowText(szBuffer, sizeof(szBuffer)-1);
	pDoc->SetCalibrateDescription("");

	GetDataFromDoc(pDoc);
}

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

int CCalibrateDlg::Todo() 
{
	int	todo;
	CModifyDialog	modifydialog(HasData());

	todo = modifydialog.DoModal();
	switch(todo){
		case(IDCANCEL):
		m_measureFlag = 0;
		break;

		case(IDOK):	//modify
		m_measureFlag = 2;
		break;

		case(IDC_CLEAR):	//start from beginning
		m_measureFlag = 1;
		ClearData();
		break;

		default:	//error just return
		m_measureFlag = 0;
		break;
	}

	return todo;
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnMeasure() 
{
	int todo;

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	//check running status
	switch(m_runningstatus){
	case(STOPED):	//start reading
		//setup the device tpye
		pDoc->EditPreferences(FALSE);
		todo = Todo();
		if(todo == IDCANCEL)	return;
		memcpy((char*)&m_deviceSetup, (char*)&(pDoc->m_deviceSetup), sizeof(DeviceSetup));
		if(InitReading()){
			m_runningstatus = MEASURING;
			m_bMeasure.SetWindowText("Stop");
			UpdateData(FALSE);
		}
		else
			CleanupDevice();
		break;

	case(MEASURING):	//stop reading
		OnMeasureDone();
		if(m_pdevice){
			CleanupDevice();
			m_runningstatus = STOPED;
			m_bMeasure.SetWindowText("Measure");
			UpdateData(FALSE);
		}
		break;
	}

}
/////////////////////////////////////////////////////////////////////////////
// CToneAdjustDlg property page

IMPLEMENT_DYNCREATE(CToneAdjustDlg, CPropertyDlg)

// Message Map
BEGIN_MESSAGE_MAP(CToneAdjustDlg, CPropertyDlg)
	//{{AFX_MSG_MAP(CToneAdjustDlg)
	ON_EN_CHANGE(IDC_DESCRIPTION, OnDescriptionChange)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_BLACK, OnBlack)
	ON_BN_CLICKED(IDC_CYAN, OnCyan)
	ON_BN_CLICKED(IDC_MAGENTA, OnMagenta)
	ON_BN_CLICKED(IDC_YELLOW, OnYellow)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_MESSAGE(WM_USER, OnCurveChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Event Map
BEGIN_EVENTSINK_MAP(CToneAdjustDlg, CPropertyDlg)
    //{{AFX_EVENTSINK_MAP(CToneAdjustDlg)
	ON_EVENT(CToneAdjustDlg, IDC_GRID, -600 /* Click */, OnGridClick, VTS_NONE)
	ON_EVENT(CToneAdjustDlg, IDC_GRID, -602 /* KeyDown */, OnGridKeyDown, VTS_PI2 VTS_I2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
CToneAdjustDlg::CToneAdjustDlg() : CPropertyDlg(CToneAdjustDlg::IDD)
{
	m_bShowCurveCyan    = FALSE;
	m_bShowCurveMagenta = FALSE;
	m_bShowCurveYellow  = FALSE;
	m_bShowCurveBlack   = FALSE;
	m_pCurveWindow = NULL;
	m_pGrid = NULL;
	//{{AFX_DATA_INIT(CToneAdjustDlg)
	//}}AFX_DATA_INIT

	CCurveWindow::RegisterControlClass();
}

/////////////////////////////////////////////////////////////////////////////
CToneAdjustDlg::~CToneAdjustDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToneAdjustDlg)
	DDX_Control(pDX, IDC_DESCRIPTION, m_editDescription);
	DDX_Control(pDX, IDC_LEVELIN, m_editLevelIn);
	DDX_Control(pDX, IDC_LEVELOUT, m_editLevelOut);
	DDX_Control(pDX, IDC_GRID, m_ctlGrid);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CToneAdjustDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CToneAdjustDlg::OnInitDialog() 
{
	CPropertyDlg::OnInitDialog();

	// Initialize the curve control
	m_pCurveWindow = (CCurveWindow*)GetDlgItem(IDC_CURVE);
	ASSERT(m_pCurveWindow);
	m_pCurveWindow->Init(TRUE/*bColorImage*/, &m_editLevelIn, &m_editLevelOut );
	m_pCurveWindow->SetPercent(TRUE);
	
	// Init the curve checkboxes (be sure this is after m_pCurveWindow is set)
	m_bShowCurveCyan    = FALSE; OnCyan();
	m_bShowCurveMagenta = FALSE; OnMagenta();
	m_bShowCurveYellow  = FALSE; OnYellow();
	m_bShowCurveBlack   = FALSE; OnBlack();

	// Initialize the CMYK grid control
    m_pGrid = &m_ctlGrid;
	ASSERT(m_pGrid);
	m_pGrid->InitCMYK(101);

	GetDataFromDoc(NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Dlgs should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::GetDataFromDoc(CProfileDoc* pDoc) 
{
	if (!pDoc)
		pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return;

	// 1. Get the data from the document
	// 2. Copy the points to the grid control
	// 3. Copy the points to the curve control
	for (int i = 0; i < 4; i++)
	{
		double fIndex[MaxNumPoints];
		double fValue[MaxNumPoints];

		int nPoints = pDoc->GetToneAdjustData( (CMYK)i, fIndex, fValue, MaxNumPoints );
		m_pGrid->SetColumn( i/*iColumn*/, fIndex, fValue, nPoints, 100.0 );
		m_pCurveWindow->SetPoints( (CMYK)i, fIndex, fValue, nPoints );
	}

	CString szDescription;
	pDoc->GetToneAdjustDescription( szDescription );
	m_editDescription.SetWindowText(szDescription);
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::OnDescriptionChange() 
{
	if (&m_editDescription != GetFocus())
		return;

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	char szBuffer[100];
	m_editDescription.GetWindowText(szBuffer, sizeof(szBuffer)-1);
	pDoc->SetToneAdjustDescription( szBuffer );
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::OnBlack() 
{
	m_bShowCurveBlack = !m_bShowCurveBlack;
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_BLACK);
	pCheckBox->SetCheck(m_bShowCurveBlack);
	m_pCurveWindow->ShowCurve(CI_GAMMA, m_bShowCurveBlack);
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::OnCyan() 
{
	m_bShowCurveCyan = !m_bShowCurveCyan;
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CYAN);
	pCheckBox->SetCheck(m_bShowCurveCyan);
	m_pCurveWindow->ShowCurve(CI_CYAN, m_bShowCurveCyan);
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::OnMagenta() 
{
	m_bShowCurveMagenta = !m_bShowCurveMagenta;
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_MAGENTA);
	pCheckBox->SetCheck(m_bShowCurveMagenta);
	m_pCurveWindow->ShowCurve(CI_MAGENTA, m_bShowCurveMagenta);
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::OnYellow() 
{
	m_bShowCurveYellow = !m_bShowCurveYellow;
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_YELLOW);
	pCheckBox->SetCheck(m_bShowCurveYellow);
	m_pCurveWindow->ShowCurve(CI_YELLOW, m_bShowCurveYellow);
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::OnReset() 
{
	m_pCurveWindow->SetToDefaults();
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::OnGridKeyDown(short* pKeyCode, short Shift)
{
	CRect rect;
	if (!m_pGrid->ProcessKey(pKeyCode, Shift, &rect))
		return;

	for (int x = rect.left; x <= rect.right; x++ )
	{ // For each of the columns representing a color curve...
		// Get the column values from the grid
		// Values should be doubles less than 100.0%
		double pdXArray[101], pdYArray[101];
		int nPoints = 101;
		nPoints = m_pGrid->GetColumn(x/*iColumn*/, pdXArray, pdYArray, nPoints, 100.0);
		m_pCurveWindow->SetPoints((CMYK)x, pdXArray, pdYArray, nPoints);

		// Update the document with the new points
		CProfileDoc* pDoc = GetProfileDoc(this);
		if (pDoc)
			pDoc->SetToneAdjustData((CMYK)x, pdXArray, pdYArray, nPoints);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::OnGridClick()
{
	m_pGrid->ProcessClick();
}


/////////////////////////////////////////////////////////////////////////////
BOOL CToneAdjustDlg::OnCurveChange(WPARAM wParam, LPARAM lParam) 
{ // Stuff the grid and the doc with the data passed from the curve control
	// Collect the data from the control
	int iCurve = LOWORD(wParam);
	int nPoints = HIWORD(wParam);

	int iColumn = iCurve + m_pGrid->GetFixedCols();

	// If the lParam is FALSE, we are being notified of a point selection
	if (!lParam)
	{
		// the nPoints parameter is the current point's X value (row)
		if (nPoints < 0) nPoints = 0;
		if (nPoints > nSCALE) nPoints = nSCALE;

		int iRow = ((nPoints * 100) + nSCALE/2) / nSCALE;
		iRow += m_pGrid->GetFixedRows();
		
		m_pGrid->SetRow(iRow);
		m_pGrid->SetCol(iColumn);

		// Now make sure the row is within view
		#define ROWS_IN_VIEW 13

		if (iRow <  m_pGrid->GetTopRow() ||
			iRow >= m_pGrid->GetTopRow() + ROWS_IN_VIEW )
		{
			int iTopRow = iRow - (ROWS_IN_VIEW/2);
			if (iTopRow < m_pGrid->GetFixedRows() )
				iTopRow = m_pGrid->GetFixedRows();
			m_pGrid->SetTopRow(iTopRow);
		}
		
		return TRUE;
	}

	// Update the document with the new points
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (pDoc)
	{
		double fIndex[MaxNumPoints];
		double fValue[MaxNumPoints];
		int nPoints = m_pCurveWindow->GetPoints( (CMYK)iCurve, fIndex, fValue, MaxNumPoints );
		pDoc->SetToneAdjustData( (CMYK)iCurve, fIndex, fValue, nPoints);
		m_pGrid->SetColumn( iCurve/*iColumn*/, fIndex, fValue, nPoints, 100.0 );
		return TRUE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CToneAdjustDlg::OnImport() 
{
	CFileDialog FileDialog(
		/*bOpenFileDialog*/	TRUE,
		/*lpszDefExt*/		"ton",
		/*lpszFileName*/	"",
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Tone Adjust Data Files (*.ton;*.txt)|*.cal;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	CString dirname((LPCSTR)IDS_TONE_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	if (FileDialog.DoModal() != IDOK)
		return;

	UpdateWindow(); // clean up screen in case import takes time

	CString szFileName = FileDialog.GetPathName();

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (pDoc)
	{
		BeginWaitCursor();
		if (!pDoc->LoadTone((char*)LPCTSTR(szFileName)))
		{
			EndWaitCursor();
			Message( "Can't load the selected file." );
			return;
		}

		GetDataFromDoc(pDoc);

		CString& sz = szFileName;
		int c;
		while ((c = sz.Find('\\')) >= 0)
			sz = sz.Mid(c+1);

		pDoc->SetToneAdjustDescription(sz);
		m_editDescription.SetWindowText(sz);

		EndWaitCursor();
	}
}

/////////////////////////////////////////////////////////////////////////////
//P
void CToneAdjustDlg::OnExport() 
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	CString szDescription;
	pDoc->GetToneAdjustDescription( szDescription );	

	CFileDialog FileDialog(
		/*bOpenFileDialog*/	FALSE,
		/*lpszDefExt*/		"ton",
		/*lpszFileName*/	(LPCSTR)szDescription,
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Tone Adjust Data Files (*.ton;*.txt)|*.cal;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	CString dirname((LPCSTR)IDS_TONE_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	if (FileDialog.DoModal() != IDOK)
		return;

	UpdateWindow(); // clean up screen in case import takes time

	CString szFileName = FileDialog.GetPathName();

	BeginWaitCursor();
	if (!pDoc->SaveTone((char*)LPCTSTR(szFileName)))
	{
		EndWaitCursor();
		Message( "Can't load the selected file." );
		return;
	}

	CString& sz = szFileName;
	int c;
	while ((c = sz.Find('\\')) >= 0)
		sz = sz.Mid(c+1);

	pDoc->SetToneAdjustDescription(sz);
	m_editDescription.SetWindowText(sz);

	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
// CColorDataDlg property page

IMPLEMENT_DYNCREATE(CColorDataDlg, CPropertyDlg)

// Message Map
BEGIN_MESSAGE_MAP(CColorDataDlg, CPropertyDlg)
	//{{AFX_MSG_MAP(CColorDataDlg)
	ON_EN_CHANGE(IDC_DESCRIPTION, OnDescriptionChange)
	ON_EN_CHANGE(IDC_DESCRIPTION_DRY, OnDescriptionDryChange)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_IMPORT_DRY, OnImportDry)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_EXPORT_DRY, OnExportDry)
	ON_BN_CLICKED(IDC_MEASURE, OnMeasure)
	ON_BN_CLICKED(IDC_MEASURE_DRY, OnMeasureDry)
	ON_BN_CLICKED(IDC_VIEW_DATA, OnViewData)
	ON_BN_CLICKED(IDC_VIEW_DATA_DRY, OnViewDataDry)
	ON_BN_CLICKED(IDC_VIEW_GAMUT, OnViewGamut)
	ON_BN_CLICKED(IDC_VIEW_GAMUT_DRY, OnViewGamutDry)
	ON_MESSAGE(WM_DATA_READY, OnDataReady)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Event Map
BEGIN_EVENTSINK_MAP(CColorDataDlg, CPropertyDlg)
    //{{AFX_EVENTSINK_MAP(CColorDataDlg)
	ON_EVENT(CColorDataDlg, IDC_GRID_SRC, -600 /* Click */, OnGridSrcClick, VTS_NONE)
	ON_EVENT(CColorDataDlg, IDC_GRID_SRC, -602 /* KeyDown */, OnGridSrcKeyDown, VTS_PI2 VTS_I2)
	ON_EVENT(CColorDataDlg, IDC_GRID_DST, -600 /* Click */, OnGridDstClick, VTS_NONE)
	ON_EVENT(CColorDataDlg, IDC_GRID_DST, -602 /* KeyDown */, OnGridDstKeyDown, VTS_PI2 VTS_I2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
CColorDataDlg::CColorDataDlg() : CPropertyDlg(CColorDataDlg::IDD)
{
	//{{AFX_DATA_INIT(CColorDataDlg)
	m_pViewGamutDialogSrc = NULL;
	m_pViewGamutDialogDst = NULL;
	m_srcDate = _T("");
	m_dryDate = _T("");
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CColorDataDlg::~CColorDataDlg()
{
	if (m_pViewGamutDialogSrc)
		delete m_pViewGamutDialogSrc;
	if (m_pViewGamutDialogDst)
		delete m_pViewGamutDialogDst;
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorDataDlg)
	DDX_Control(pDX, IDC_DESCRIPTION, m_editDescription);
	DDX_Control(pDX, IDC_DESCRIPTION_DRY, m_editDescriptionDry);
	DDX_Control(pDX, IDC_DATA_OK, m_bmpDataOKSource);
	DDX_Control(pDX, IDC_DATA_OK_DRY, m_bmpDataOKDryjet);
	DDX_Text(pDX, IDC_SRC_DATE, m_srcDate);
	DDX_Text(pDX, IDC_DRY_DATE, m_dryDate);
	//}}AFX_DATA_MAP

	#ifdef NO_COLORDATA_GRIDS
	// To restore the grids, add these lines in above
	DDX_Control(pDX, IDC_GRID_SRC, m_ctlGridSrc);
	DDX_Control(pDX, IDC_GRID_DST, m_ctlGridDst);
	#endif NO_COLORDATA_GRIDS
}

/////////////////////////////////////////////////////////////////////////////
// CColorDataDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CColorDataDlg::OnInitDialog() 
{
	CPropertyDlg::OnInitDialog();
	
	#ifdef NO_COLORDATA_GRIDS
	// Initialize the src grid control
	m_ctlGridSrc.InitLAB(1);

	// Initialize the dst grid control
	m_ctlGridDst.InitLAB(1);
	#endif NO_COLORDATA_GRIDS

	GetDataFromDoc(NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Dlgs should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::GetDataFromDoc(CProfileDoc* pDoc) 
{
	BeginWaitCursor();
	GetSourceDataFromDoc(pDoc);
	GetDryjetDataFromDoc(pDoc);
	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::GetSourceDataFromDoc(CProfileDoc* pDoc) 
{
	if (!pDoc)
		pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return;

	// 1. Get the src patch data from the document
	// 2. Copy the points to the src grid control
	double* pfIndex = NULL;
	double* pfL = NULL;
	double* pfa = NULL;
	double* pfb = NULL;
	int		nLinear;

	int nPoints = pDoc->GetColorSourceData( &pfIndex, &pfL, &pfa, &pfb, &nLinear );
	#ifdef NO_COLORDATA_GRIDS
	m_ctlGridSrc.InitLABRows(nPoints+1);
	if (pfIndex && pfL && pfa && pfb)
	{
		m_ctlGridSrc.SetColumn( 0/*iColumn*/, pfIndex, pfL, nPoints, 100.0 );
		m_ctlGridSrc.SetColumn( 1/*iColumn*/, pfIndex, pfa, nPoints, 100.0 );
		m_ctlGridSrc.SetColumn( 2/*iColumn*/, pfIndex, pfb, nPoints, 100.0 );
	}
	m_ctlGridSrc.InitLABRowLabels(-1, -1);
	#endif NO_COLORDATA_GRIDS

	delete [] pfIndex;
	delete [] pfL;
	delete [] pfa;
	delete [] pfb;

	CString szDescription;
	pDoc->GetColorSourceDescription( szDescription );
	m_editDescription.SetWindowText(szDescription);

	if(nPoints){
		long	date;
		pDoc->GetDataDate(&date, 1);
		if(date < 0)	date = 0;

		CTime	ctime((time_t)date); 
		SetTime(&ctime, m_srcDate);
		UpdateData(FALSE);	
	}

	BOOL bEnable = (nPoints > 0);
	SetCheck(1, bEnable);
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::SetCheck(int which, BOOL enable)
{
	CButton* pButton;
	int idBitmap = (enable ? IDB_CHECK1 : IDB_CHECK2);

	switch(which){
		case(1):  //source
		m_bmpDataOKSource.SetBitmap(::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(idBitmap)));
		pButton = (CButton*)GetDlgItem(IDC_EXPORT);
		if (pButton) pButton->EnableWindow(enable);
		pButton = (CButton*)GetDlgItem(IDC_VIEW_GAMUT);
		if (pButton) pButton->EnableWindow(enable);
		pButton = (CButton*)GetDlgItem(IDC_VIEW_DATA);
		if (pButton) pButton->EnableWindow(enable);
		break;

		default:	//dryjet
		m_bmpDataOKDryjet.SetBitmap(::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(idBitmap)));
		pButton = (CButton*)GetDlgItem(IDC_EXPORT_DRY);
		if (pButton) pButton->EnableWindow(enable);
		pButton = (CButton*)GetDlgItem(IDC_VIEW_GAMUT_DRY);
		if (pButton) pButton->EnableWindow(enable);
		pButton = (CButton*)GetDlgItem(IDC_VIEW_DATA_DRY);
		if (pButton) pButton->EnableWindow(enable);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::GetDryjetDataFromDoc(CProfileDoc* pDoc) 
{
	if (!pDoc)
		pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return;

	// 1. Get the dst patch data from the document
	// 2. Copy the points to the dst grid control
	double* pfIndex = NULL;
	double* pfL = NULL;
	double* pfa = NULL;
	double* pfb = NULL;
	int		nLinear;

	int nPoints = pDoc->GetColorDryjetData( &pfIndex, &pfL, &pfa, &pfb, &nLinear );
	#ifdef NO_COLORDATA_GRIDS
	m_ctlGridDst.InitLABRows(nPoints+1);
	if (pfIndex && pfL && pfa && pfb)
	{
		m_ctlGridDst.SetColumn( 0/*iColumn*/, pfIndex, pfL, nPoints, 100.0 );
		m_ctlGridDst.SetColumn( 1/*iColumn*/, pfIndex, pfa, nPoints, 100.0 );
		m_ctlGridDst.SetColumn( 2/*iColumn*/, pfIndex, pfb, nPoints, 100.0 );
	}
	m_ctlGridDst.InitLABRowLabels(-1, -1);
	#endif NO_COLORDATA_GRIDS

	delete [] pfIndex;
	delete [] pfL;
	delete [] pfa;
	delete [] pfb;

	CString szDescription;
	pDoc->GetColorDryjetDescription( szDescription );
	m_editDescriptionDry.SetWindowText(szDescription);

	if(nPoints){
		long	date;
		pDoc->GetDataDate(&date, 0);
		if(date < 0)	date = 0;

		CTime	ctime((time_t)date); 
		SetTime(&ctime, m_dryDate);
		UpdateData(FALSE);	
	}

	BOOL bEnable = (nPoints > 0);
	SetCheck(0, bEnable);
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnDescriptionChange() 
{
	if (&m_editDescription != GetFocus())
		return;

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	char szBuffer[100];
	m_editDescription.GetWindowText(szBuffer, sizeof(szBuffer)-1);
	pDoc->SetColorSourceDescription( szBuffer );
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnDescriptionDryChange() 
{
	if (&m_editDescriptionDry != GetFocus())
		return;

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	char szBuffer[100];
	m_editDescriptionDry.GetWindowText(szBuffer, sizeof(szBuffer)-1);
	pDoc->SetColorDryjetDescription( szBuffer );
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnGridSrcKeyDown(short* pKeyCode, short Shift)
{
	// Uncomment this code if you want the grid control to be editable
	// Remember that you must copy any edits the user makes back to print data
	// in order for them to get saved
	#ifdef NOTUSED 
	CRect rect;
	if (!m_ctlGridSrc.ProcessKey(pKeyCode, Shift, &rect))
		return;

	for (int x = rect.left; x <= rect.right; x++ )
	{ // For each of the columns representing a color curve...
		// Get the column values from the grid
		// Values should be doubles less than 100.0%
		double pdXArray[101], pdYArray[101];
		int nPoints = 101;
		nPoints = m_ctlGridSrc.GetColumn(x/*iColumn*/, pdXArray, pdYArray, nPoints, 100.0);

		// Update the document with the new points
		CProfileDoc* pDoc = GetProfileDoc(this);
		if (pDoc)
			pDoc->SetCalibrateData((CMYK)x, pdXArray, pdYArray, nPoints);
	}

	m_ctlGridSrc.InitLABRowLabels(rect.top, rect.bottom);
	#endif NOTUSED
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnGridSrcClick()
{
	#ifdef NO_COLORDATA_GRIDS
	m_ctlGridSrc.ProcessClick();
	#endif NO_COLORDATA_GRIDS
}


/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnGridDstKeyDown(short* pKeyCode, short Shift)
{
	// Uncomment this code if you want the grid control to be editable
	// Remember that you must copy any edits the user makes back to print data
	// in order for them to get saved
	#ifdef NOTUSED 
	CRect rect;
	if (!m_ctlGridDst.ProcessKey(pKeyCode, Shift, &rect))
		return;

	for (int x = rect.left; x <= rect.right; x++ )
	{ // For each of the columns representing a color curve...
		// Get the column values from the grid
		// Values should be doubles less than 100.0%
		double pdXArray[101], pdYArray[101];
		int nPoints = 101;
		nPoints = m_ctlGridDst.GetColumn(x/*iColumn*/, pdXArray, pdYArray, nPoints, 100.0);

		// Update the document with the new points
		CProfileDoc* pDoc = GetProfileDoc(this);
		if (pDoc)
			pDoc->SetCalibrateData((CMYK)x, pdXArray, pdYArray, nPoints);
	}

	m_ctlGridDst.InitLABRowLabels(rect.top, rect.bottom);
	#endif NOTUSED
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnGridDstClick()
{
	#ifdef NO_COLORDATA_GRIDS
	m_ctlGridDst.ProcessClick();
	#endif NO_COLORDATA_GRIDS
}


/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnImport() 
{
	CFileDialog FileDialog(
		/*bOpenFileDialog*/	TRUE,
		/*lpszDefExt*/		"cal",
		/*lpszFileName*/	"",
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Color Data Files (*.col;*.txt)|*.col;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	CString dirname((LPCSTR)IDS_DATA_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	if (FileDialog.DoModal() != IDOK)
		return;

	UpdateWindow(); // clean up screen in case import takes time

	CString szFileName = FileDialog.GetPathName();

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (pDoc)
	{
		BeginWaitCursor();
		if (!pDoc->LoadPatch((char*)LPCTSTR(szFileName), 1/*source*/))
		{
			EndWaitCursor();
			Message( "Can't load the selected file." );
			return;
		}

		GetSourceDataFromDoc(NULL);

		CString& sz = szFileName;
		int c;
		while ((c = sz.Find('\\')) >= 0)
			sz = sz.Mid(c+1);

		pDoc->SetColorSourceDescription(sz);
		m_editDescription.SetWindowText(sz);

		//date
		CFileStatus fstatus;
		if( CFile::GetStatus((LPCSTR)szFileName, fstatus ) )
		{
			time_t long_time;

			long_time = fstatus.m_ctime.GetTime();
			pDoc->SetDataDate(long_time, 1);

			SetTime(&fstatus.m_ctime, m_srcDate);
			UpdateData(FALSE);	
		}

		SetCheck(1, TRUE);
		EndWaitCursor();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnImportDry() 
{
	CFileDialog FileDialog(
		/*bOpenFileDialog*/	TRUE,
		/*lpszDefExt*/		"cal",
		/*lpszFileName*/	"",
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Color Data Files (*.col;*.txt)|*.col;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	CString dirname((LPCSTR)IDS_DATA_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	if (FileDialog.DoModal() != IDOK)
		return;

	UpdateWindow(); // clean up screen in case import takes time

	CString szFileName = FileDialog.GetPathName();

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (pDoc)
	{
		BeginWaitCursor();
		if (!pDoc->LoadPatch((char*)LPCTSTR(szFileName), 0/*dest (DryJet)*/))
		{
			EndWaitCursor();
			Message( "Can't load the selected file." );
			return;
		}

		GetDryjetDataFromDoc(NULL);

		CString& sz = szFileName;
		int c;
		while ((c = sz.Find('\\')) >= 0)
			sz = sz.Mid(c+1);

		pDoc->SetColorDryjetDescription(sz);
		m_editDescriptionDry.SetWindowText(sz);

		//date
		CFileStatus fstatus;
		if( CFile::GetStatus((LPCSTR)szFileName, fstatus ) )
		{
			time_t long_time;

			long_time = fstatus.m_ctime.GetTime();
			pDoc->SetDataDate(long_time, 0);

			SetTime(&fstatus.m_ctime, m_dryDate);
			UpdateData(FALSE);	
		}

		SetCheck(0, TRUE);

		//set the linear data
		if( pDoc->LoadLinearFromPatch() )
		{
			CProfileView* pView = GetView( this );
			if(pView)
				((CCalibrateDlg*)(pView->m_pDlgCalibrate))->GetDataFromDoc(pDoc);	
		}

		EndWaitCursor();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnExport() 
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	CString szDescription;
	pDoc->GetColorSourceDescription( szDescription );	

	CFileDialog FileDialog(
		/*bOpenFileDialog*/	FALSE,
		/*lpszDefExt*/		"cal",
		/*lpszFileName*/	(LPCSTR)szDescription,
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Color Data Files (*.col;*.txt)|*.col;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	CString dirname((LPCSTR)IDS_DATA_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	if (FileDialog.DoModal() != IDOK)
		return;

	UpdateWindow(); // clean up screen in case import takes time

	CString szFileName = FileDialog.GetPathName();

	BeginWaitCursor();
	if (!pDoc->SavePatch((char*)LPCTSTR(szFileName), 1/*source*/))
	{
		EndWaitCursor();
		Message( "Can't save the selected file." );
		return;
	}

	CString& sz = szFileName;
	int c;
	while ((c = sz.Find('\\')) >= 0)
		sz = sz.Mid(c+1);

	pDoc->SetColorSourceDescription(sz);
	m_editDescription.SetWindowText(sz);

	CFileStatus fstatus;
	if( CFile::GetStatus((LPCSTR)szFileName, fstatus ) )
	{
		time_t long_time;
		pDoc->GetDataDate(&long_time, 1);
		fstatus.m_ctime	= long_time; 
		CFile::SetStatus(LPCTSTR(szFileName), fstatus);
	}

	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnExportDry() 
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	CString szDescription;
	pDoc->GetColorDryjetDescription( szDescription );	

	CFileDialog FileDialog(
		/*bOpenFileDialog*/	FALSE,
		/*lpszDefExt*/		"cal",
		/*lpszFileName*/	(LPCSTR)szDescription,
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Color Data Files (*.col;*.txt)|*.col;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	CString dirname((LPCSTR)IDS_DATA_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	if (FileDialog.DoModal() != IDOK)
		return;

	UpdateWindow(); // clean up screen in case import takes time

	CString szFileName = FileDialog.GetPathName();

	BeginWaitCursor();
	if (!pDoc->SavePatch((char*)LPCTSTR(szFileName), 0/*dest (DryJet)*/))
	{
		EndWaitCursor();
		Message( "Can't save the selected file." );
		return;
	}

	CString& sz = szFileName;
	int c;
	while ((c = sz.Find('\\')) >= 0)
		sz = sz.Mid(c+1);

	pDoc->SetColorDryjetDescription(sz);
	m_editDescriptionDry.SetWindowText(sz);

	CFileStatus fstatus;
	if( CFile::GetStatus((LPCSTR)szFileName, fstatus ) )
	{
		time_t long_time;
		pDoc->GetDataDate(&long_time, 0);
		fstatus.m_ctime	= long_time; 
		CFile::SetStatus(LPCTSTR(szFileName), fstatus);
	}

	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CColorDataDlg::LoadDataFromGrid(PatchFormat *patchdata, CGrid *pGrid)
{
	long	i, j, m ,totalcmy, labpatches, totallinear;
	long steps[] = {9, 6, 5, 5, 4, 4, 4};
	double blacks[] = {	0.0, 20.0, 40.0, 60.00, 80.00, 90.00, 100.00 }; 
	double cmy[] =
	{ 
		0.0, 10.0, 22.0, 34.00, 47.00, 60.00, 73.00, 86.0, 100.0, 
		0.0, 20.0, 40.0, 60.00, 80.00, 100.00, 
		0.0, 25.0, 50.0, 75.00, 100.0, 
		0.0, 25.0, 50.0, 75.00, 100.0, 
		0.0, 33.0, 66.0, 100.0,
		0.0, 33.0, 66.0, 100.0,
		0.0, 33.0, 66.0, 100.0
	};
	double linear[] =
	{
		0, 4, 8, 12, 16, 20, 
		24, 28, 32, 36, 40,
		44, 48, 52, 56, 60, 
		64, 68, 72, 76, 80,
		84, 88, 92, 96, 100
	};

	patchdata->patchType = 1; 
	patchdata->numComp = 4;
	patchdata->numCube = 7;
	patchdata->numLinear = 26;

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return false;

	Mpfileio mpfileio(pDoc->_profiledocfiles.printData);
	if( mpfileio.createPatchdata(patchdata, steps) != MCO_SUCCESS)
		return FALSE;

	//setup format data
	for(i = 0; i < (long)patchdata->numCube; i++)
		patchdata->steps[i] = steps[i];

	for(i = 0; i < (long)patchdata->numCube; i++)
		patchdata->blacks[i] = blacks[i];

	m = 0;
	for(i = 0; i < (long)patchdata->numCube; i++){
		for(j = 0; j < (long)patchdata->steps[i]; j++)
		{
			patchdata->cmy[m] = cmy[m];
			m++;
		}
	}			

 	labpatches = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		labpatches += patchdata->steps[i]*patchdata->steps[i]*patchdata->steps[i];

	for(i = 0; i < patchdata->numLinear; i++)
		patchdata->linear[i] = linear[i];

	if(patchdata->numLinear > 0)
		totallinear = (patchdata->numLinear-1)*4;	

	//setup real data
	double index[2000], val[2000];

	pGrid->GetColumn(0, index, val, 2000, 1);
	for(i = 0; i < labpatches + totallinear; i++)
	{
		j = i*3;
		if( i < labpatches)
			patchdata->data[j] = val[i];
		else{
			m = (i - labpatches)*3;
			patchdata->ldata[m] = val[i];
			patchdata->ldata[m+1] = 0;
			patchdata->ldata[m+2] = 0;
		}
	}

	pGrid->GetColumn(1, index, val, 2000, 1);
	for(i = 0; i < labpatches; i++){
		j = i*3 + 1;
		patchdata->data[j] = val[i];
	}

	pGrid->GetColumn(2, index, val, 2000, 1);
	for(i = 0; i < labpatches; i++){
		j = i*3 + 2;
		patchdata->data[j] = val[i];
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CColorDataDlg::OnDataReady(WPARAM wParam, LPARAM lParam)
{
	PatchFormat	*patchdata;

	long which = 1;

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return false;

	Mpfileio mpfileio(pDoc->_profiledocfiles.printData);
	mpfileio.cleanup(which);

	if(m_whichData == 1){
		patchdata = &pDoc->_profiledocfiles.printData->srcpatch;
	}
	else{
		patchdata = &pDoc->_profiledocfiles.printData->destpatch;
	}


	CGrid *pGrid = (CGrid*)wParam;

	if(!LoadDataFromGrid(patchdata, pGrid))	return FALSE;

	//set description and time
	CTime ctime = CTime::GetCurrentTime();
	time_t long_time = ctime.GetTime();

	CString sz("Untitled.txt");
	if(m_whichData == 1)
	{
		pDoc->SetColorSourceDescription(sz);
		pDoc->SetDataDate(long_time, 1);
	}
	else
	{
		pDoc->SetColorDryjetDescription(sz);
		pDoc->SetDataDate(long_time, 0);

		//for dryjet also need to copy to the calibration data
	}

	return TRUE;	
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnMeasure() 
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	//setup the device
	pDoc->EditPreferences(TRUE);

	// TODO: Add your control notification handler code here
	//int patches[] = { 1387, 25, 25, 25, 25 };	
	CMeasureDialog	measuredialog(MEASURE_COLOR_DATA, pDoc, this);
	memcpy((char*)&(measuredialog.m_deviceSetup), (char*)&(pDoc->m_deviceSetup), sizeof(DeviceSetup));

	m_whichData = 1;
	if(measuredialog.DoModal() == IDCANCEL)
		return;

	BeginWaitCursor();

	GetDataFromDoc(pDoc);
	
	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnMeasureDry() 
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;

	//setup the device
	pDoc->EditPreferences(TRUE);

	// TODO: Add your control notification handler code here
	//int patches[] = { 1387, 25, 25, 25, 25 };	
	CMeasureDialog	measuredialog(MEASURE_COLOR_DATA, pDoc, this);
	memcpy((char*)&(measuredialog.m_deviceSetup), (char*)&(pDoc->m_deviceSetup), sizeof(DeviceSetup));

	m_whichData = 0;
	if(measuredialog.DoModal() == IDCANCEL)
		return;

	BeginWaitCursor();

	GetDataFromDoc(pDoc);

//set the linear data
	if( pDoc->LoadLinearFromPatch() )
	{
		CProfileView* pView = GetView( this );
		if(pView)
			((CCalibrateDlg*)(pView->m_pDlgCalibrate))->GetDataFromDoc(pDoc);
	}
	
	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnViewData() 
{
	BeginWaitCursor();
	CViewDataDialog	ViewData(GetProfileDoc(this), 0/*source*/, this);
	ViewData.DoModal();
	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnViewDataDry() 
{
	BeginWaitCursor();
	CViewDataDialog	ViewData(GetProfileDoc(this), 1/*dryjet*/, this);
	ViewData.DoModal();
	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnViewGamut() 
{
	if (m_pViewGamutDialogSrc)
	{
		m_pViewGamutDialogSrc->SetActiveWindow();
		if (m_pViewGamutDialogSrc->IsIconic())
			m_pViewGamutDialogSrc->ShowWindow(SW_NORMAL);
	}
	else
	{
		CProfileDoc *pDoc = GetProfileDoc(this);
		if (!pDoc)
			return;
		if( pDoc->_profiledocfiles.simupatchD->copyOutOfPatchData(&pDoc->_profiledocfiles.printData->srcpatch) != MCO_SUCCESS)
		{
			Message("Wrong Source data.");
			return;
		}

		CString szDescription;
		pDoc->GetColorSourceDescription(szDescription);

		McoHandle gamut;
		CBuildProgress buildprogress( (long)pDoc->_profiledocfiles.simupatchD, 
			(long)&gamut, Process_GammutSurface, szDescription);
		if (buildprogress.DoModal() == IDCANCEL)
			return;

		m_pViewGamutDialogSrc = new CViewGamut((long)gamut, 1/*source*/, (LPCTSTR)szDescription, this);
		if (m_pViewGamutDialogSrc)
			m_pViewGamutDialogSrc->DoModeless(this, &m_pViewGamutDialogSrc);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CColorDataDlg::OnViewGamutDry() 
{
	if (m_pViewGamutDialogDst)
	{
		m_pViewGamutDialogDst->SetActiveWindow();
		if (m_pViewGamutDialogDst->IsIconic())
			m_pViewGamutDialogDst->ShowWindow(SW_NORMAL);
	}
	else
	{
		CProfileDoc *pDoc = GetProfileDoc(this);
		if (!pDoc)
			return;
		if( pDoc->_profiledocfiles.patchD->copyOutOfPatchData(&pDoc->_profiledocfiles.printData->destpatch) != MCO_SUCCESS)
		{
			Message("Wrong DryJet data.");
			return;
		}

		CString szDescription;
		pDoc->GetColorDryjetDescription(szDescription);

		McoHandle	gamut;
		CBuildProgress buildprogress( (long)pDoc->_profiledocfiles.patchD, 
			(long)&gamut, Process_GammutSurface, szDescription);
		if (buildprogress.DoModal() == IDCANCEL)
			return;

		m_pViewGamutDialogDst = new CViewGamut((long)gamut, 0/*destination*/, (LPCTSTR)szDescription, this);
		if (m_pViewGamutDialogDst)
			m_pViewGamutDialogDst->DoModeless(this, &m_pViewGamutDialogDst);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg property page

IMPLEMENT_DYNCREATE(CSettingsDlg, CPropertyDlg)

// Message Map
BEGIN_MESSAGE_MAP(CSettingsDlg, CPropertyDlg)
	//{{AFX_MSG_MAP(CSettingsDlg)
	ON_CBN_SELCHANGE(IDC_SETTINGSCOMBO, OnModeSelected)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_OFF, OnTextEnhanceOff)
	ON_BN_CLICKED(IDC_ON, OnTextEnhanceOn)
	ON_BN_CLICKED(IDC_PON, OnCopyPaperOn)
	ON_BN_CLICKED(IDC_POFF, OnCopyPaperOff)
	ON_BN_CLICKED(IDC_HON, OnHighLightOn)
	ON_BN_CLICKED(IDC_HOFF, OnHighLightOff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Event Map
BEGIN_EVENTSINK_MAP(CSettingsDlg, CPropertyDlg)
    //{{AFX_EVENTSINK_MAP(CSettingsDlg)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
CSettingsDlg::CSettingsDlg() : CPropertyDlg(CSettingsDlg::IDD)
{
	//{{AFX_DATA_INIT(CSettingsDlg)
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CSettingsDlg::~CSettingsDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Control(pDX, IDC_ON,				m_radioTextEnhOn);
	DDX_Control(pDX, IDC_OFF,				m_radioTextEnhOff);
	DDX_Control(pDX, IDC_HON,				m_radioHighLightOn);
	DDX_Control(pDX, IDC_HOFF,				m_radioHighLightOff);
	DDX_Control(pDX, IDC_PON,				m_radioCopyPaperOn);
	DDX_Control(pDX, IDC_POFF,				m_radioCopyPaperOff);
	DDX_Control(pDX, IDC_SETTINGSCOMBO,		m_comboMode);
	DDX_Control(pDX, IDC_BALANCE_SLIDER,	m_sldBalanceCutoff);
	DDX_Control(pDX, IDC_BALANCE_VALUE,		m_staBalanceCutoff);
	DDX_Control(pDX, IDC_CONTRAST_SLIDER,	m_sldContrast);
	DDX_Control(pDX, IDC_CONTRAST_VALUE,	m_staContrast);
	DDX_Control(pDX, IDC_SATURATION_SLIDER,	m_sldSaturation);
	DDX_Control(pDX, IDC_SATURATION_VALUE,	m_staSaturation);
	DDX_Control(pDX, IDC_SMOOTHING_SLIDER,	m_sldSmoothing);
	DDX_Control(pDX, IDC_SMOOTHING_VALUE,	m_staSmoothing);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers

/////////////////////////////////////////////////////////////////////////////
//P
BOOL CSettingsDlg::OnInitDialog() 
{
	CPropertyDlg::OnInitDialog();

//	m_comboMode.SetCurSel(0);
//	OnModeSelected(FALSE/*bSave*/);

	m_radioTextEnhOn.SetCheck(TRUE);
	m_radioTextEnhOff.SetCheck(FALSE);

	m_radioHighLightOn.SetCheck(FALSE);
	m_radioHighLightOff.SetCheck(TRUE);

	m_radioCopyPaperOn.SetCheck(FALSE);
	m_radioCopyPaperOff.SetCheck(TRUE);
		
	m_sldBalanceCutoff.SetRange(0, 100, FALSE/*bRedraw*/);
	m_sldBalanceCutoff.SetTicFreq(10);

	m_sldContrast.SetRange(0, 100, FALSE/*bRedraw*/);
	m_sldContrast.SetTicFreq(10);

	m_sldSaturation.SetRange(0, 100, FALSE/*bRedraw*/);
	m_sldSaturation.SetTicFreq(10);

	m_sldSmoothing.SetRange(0, 5, FALSE/*bRedraw*/);
	m_sldSmoothing.SetTicFreq(1);

	GetDataFromDoc(NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Dlgs should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CSettingsDlg::GetDataFromDoc(CProfileDoc* pDoc) 
{
	if (!pDoc)
		pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return;

	BOOL bAutomatic = pDoc->GetSettingAutomatic();

	m_automatic = bAutomatic;
	
	if(m_automatic)
	{			
		m_textEnh = TRUE;
		m_copyPaper = FALSE;
		m_highLightOn = FALSE;
		m_colorBalanceCutoff = 94;
		m_colorBalance = 100;	
		m_inkNeutralize = 1;
		m_contrast = 80;		
 		m_saturation = 80;	
		m_smoothSimu = 0;	
	}
	else
	{
		m_textEnh = pDoc->GetSettingTextEnhance();
		m_colorBalance = pDoc->GetSettingColorBalanceCutoff();
		m_inkNeutralize = pDoc->GetSettingInkNeutralize();
		if(m_colorBalance == 100)
			m_highLightOn = FALSE;
		else
			m_highLightOn = TRUE;	
		if(m_inkNeutralize == 1)
			m_copyPaper = FALSE;
		else
			m_copyPaper = TRUE;
		m_colorBalanceCutoff = pDoc->GetSettingColorBalanceCutoff();
		m_contrast = pDoc->GetSettingContrast();
		m_saturation = pDoc->GetSettingSaturation();
		m_smoothSimu = pDoc->GetSettingSmoothData();
	}

	SetDataToControls(m_automatic);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettingsDlg::SetDataToDoc(CProfileDoc* pDoc) 
{
	if (!pDoc)
		pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return FALSE;

	pDoc->SetSettingAutomatic(m_automatic);

	if(m_automatic){
		pDoc->SetSettingTextEnhance(TRUE);
		pDoc->SetSettingColorBalance( 100 );
		pDoc->SetSettingInkNeutralize(1);
		pDoc->SetSettingContrast( 80 );
		pDoc->SetSettingSaturation( 80 );
		pDoc->SetSettingSmoothData( 0 );
		pDoc->SetSettingColorBalanceCufoff( 94 );
	}
	else{	
		pDoc->SetSettingTextEnhance(m_textEnh);
/*
		if(m_copyPaper)
		{
			pDoc->SetSettingColorBalance( 0 );
			pDoc->SetSettingInkNeutralize(0);
		}
		else
		{
			pDoc->SetSettingColorBalance( 100 );
			pDoc->SetSettingInkNeutralize(1);
		}
*/
		if(m_copyPaper)
		{
			pDoc->SetSettingInkNeutralize(0);
		}
		else
		{
			pDoc->SetSettingInkNeutralize(1);
		}

		if(m_highLightOn)
		{
			pDoc->SetSettingColorBalance( 0 );
		}
		else
		{
			pDoc->SetSettingColorBalance( 100 );
		}
			
		pDoc->SetSettingContrast( m_contrast );
		pDoc->SetSettingSaturation( m_saturation );
		pDoc->SetSettingSmoothData( m_smoothSimu );
		pDoc->SetSettingColorBalanceCufoff( m_colorBalanceCutoff );
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSettingsDlg::SetDataToControls(BOOL bAuto)
{

	if(bAuto){
		m_comboMode.SetCurSel(!bAuto);
		m_radioTextEnhOn.SetCheck(TRUE);
		m_radioTextEnhOff.SetCheck(FALSE);

		m_radioHighLightOn.SetCheck(FALSE);
		m_radioHighLightOff.SetCheck(TRUE);

		m_radioCopyPaperOn.SetCheck(FALSE);
		m_radioCopyPaperOff.SetCheck(TRUE);

		CString sz;
		m_sldBalanceCutoff.SetPos(94);
		sz.Format("%d", 94);
		m_staBalanceCutoff.SetWindowText(sz);

		m_sldContrast.SetPos(80);
		sz.Format("%d", 80);
		m_staContrast.SetWindowText(sz);

		m_sldSaturation.SetPos(80);
		sz.Format("%d", 80);
		m_staSaturation.SetWindowText(sz);

		m_sldSmoothing.SetPos(0);
		sz.Format("%d", 0);
		m_staSmoothing.SetWindowText(sz);
	}
	else{
		m_comboMode.SetCurSel(!bAuto);
		m_radioTextEnhOn.SetCheck(m_textEnh);
		m_radioTextEnhOff.SetCheck(!m_textEnh);

		m_radioHighLightOn.SetCheck(m_highLightOn);
		m_radioHighLightOff.SetCheck(!m_highLightOn);

		m_radioCopyPaperOn.SetCheck(m_copyPaper);
		m_radioCopyPaperOff.SetCheck(!m_copyPaper);

		CString sz;
		m_sldBalanceCutoff.SetPos(m_colorBalanceCutoff);
		sz.Format("%d", m_colorBalanceCutoff);
		m_staBalanceCutoff.SetWindowText(sz);

		m_sldContrast.SetPos(m_contrast);
		sz.Format("%d", m_contrast);
		m_staContrast.SetWindowText(sz);

		m_sldSaturation.SetPos(m_saturation);
		sz.Format("%d", m_saturation);
		m_staSaturation.SetWindowText(sz);

		m_sldSmoothing.SetPos(m_smoothSimu);
		sz.Format("%d", m_smoothSimu);
		m_staSmoothing.SetWindowText(sz);
	}

	m_radioTextEnhOn.EnableWindow(!bAuto);
	m_radioTextEnhOff.EnableWindow(!bAuto);
	m_radioHighLightOn.EnableWindow(!bAuto);
	m_radioHighLightOff.EnableWindow(!bAuto);
	m_radioCopyPaperOn.EnableWindow(!bAuto);
	m_radioCopyPaperOff.EnableWindow(!bAuto);
	m_sldBalanceCutoff.EnableWindow(!bAuto & !m_copyPaper);
	m_staBalanceCutoff.EnableWindow(!bAuto & !m_copyPaper);
	m_sldContrast.EnableWindow(!bAuto);
	m_staContrast.EnableWindow(!bAuto);
	m_sldSaturation.EnableWindow(!bAuto);
	m_staSaturation.EnableWindow(!bAuto);
	m_sldSmoothing.EnableWindow(!bAuto);
	m_staSmoothing.EnableWindow(!bAuto);
}

/////////////////////////////////////////////////////////////////////////////
//P
void CSettingsDlg::OnModeSelected(BOOL bSave) 
{
	m_automatic = (m_comboMode.GetCurSel() == 0);

	SetDataToControls(m_automatic);
	SetDataToDoc(NULL);
}

/////////////////////////////////////////////////////////////////////////////
//P
void CSettingsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	int iValue = pSlider->GetPos();

	CString sz;
	sz.Format("%d", iValue);

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return;

	switch (pSlider->GetDlgCtrlID()) 
	{
		case IDC_BALANCE_SLIDER:
			if (nSBCode == SB_ENDSCROLL)
				m_colorBalanceCutoff = iValue;
			m_staBalanceCutoff.SetWindowText(sz);
			break;
		case IDC_CONTRAST_SLIDER:
			if (nSBCode == SB_ENDSCROLL)
				m_contrast = iValue;
			m_staContrast.SetWindowText(sz);
			break;
		case IDC_SATURATION_SLIDER:
			if (nSBCode == SB_ENDSCROLL)
				m_saturation = iValue;
			m_staSaturation.SetWindowText(sz);
			break;
		case IDC_SMOOTHING_SLIDER:
			if (nSBCode == SB_ENDSCROLL)
				m_smoothSimu = iValue;
			m_staSmoothing.SetWindowText(sz);
			break;
	}	

	SetDataToDoc(NULL);

	CPropertyDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void CSettingsDlg::OnTextEnhanceOff() 
{
	m_textEnh = FALSE;	
	SetDataToDoc(NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CSettingsDlg::OnTextEnhanceOn() 
{
	m_textEnh = TRUE;	
	SetDataToDoc(NULL);
}

void CSettingsDlg::OnHighLightOn() 
{
	// TODO: Add your control notification handler code here
	m_highLightOn = TRUE;
	SetDataToDoc(NULL);
	m_sldBalanceCutoff.EnableWindow(!m_highLightOn);
	m_staBalanceCutoff.EnableWindow(!m_highLightOn);
}

void CSettingsDlg::OnHighLightOff() 
{
	// TODO: Add your control notification handler code here
	m_highLightOn = FALSE;
	SetDataToDoc(NULL);	
	m_sldBalanceCutoff.EnableWindow(!m_highLightOn);
	m_staBalanceCutoff.EnableWindow(!m_highLightOn);
}

void CSettingsDlg::OnCopyPaperOn() 
{
	// TODO: Add your control notification handler code here
	m_copyPaper = TRUE;
	SetDataToDoc(NULL);
//	m_sldBalanceCutoff.EnableWindow(!m_copyPaper);
//	m_staBalanceCutoff.EnableWindow(!m_copyPaper);
}

void CSettingsDlg::OnCopyPaperOff() 
{
	// TODO: Add your control notification handler code here
	m_copyPaper = FALSE;
	SetDataToDoc(NULL);
//	m_sldBalanceCutoff.EnableWindow(!m_copyPaper);
//	m_staBalanceCutoff.EnableWindow(!m_copyPaper);
}

/////////////////////////////////////////////////////////////////////////////
void CSettingsDlg::EnableStatics(BOOL bEnable)
{
	CWnd* pControl = GetTopWindow();
	while (pControl)
	{
		int i = (int)(short)pControl->GetDlgCtrlID();
		if (i == IDC_STATIC)
		{
			if (!pControl->IsWindowEnabled() != !bEnable)
			{
				pControl->EnableWindow(bEnable);
				pControl->Invalidate();
			}
		}

		pControl = pControl->GetNextWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
void ConvertToGamutC(ProfileDocFiles* doc, double* lab, double* labc)
{
	if (!doc) return;
	double* labTable = (double*)McoLockHandle(doc->_labtableH);
	interpolate_33(lab,labc,labTable);
	McoUnlockHandle(doc->_labtableH);
}

/////////////////////////////////////////////////////////////////////////////
void ConvertToPrint(ProfileDocFiles* doc, double* lab, double* labo)
{
	if (!doc) return;
	doc->tweak->eval(lab, labo, 1);
}

/////////////////////////////////////////////////////////////////////////////
void ConvertToGamut(ProfileDocFiles* doc, double* lab, double* labo)
{
	if (!doc) return;
	ConvertToGamutC(doc, lab, labo);
	doc->gtweak->eval(lab, labo, labo, 1);
}

void SetupTweaks(ProfileDocFiles* doc)
{
	if(!doc) return;
	if( doc->copyOutOfPrintData() != MCO_SUCCESS )	return;

	doc->tweak->Init_Table();
	doc->tweak->Modify_Table(doc->printerData->num_tweaks, doc->printerData->tweaks);
	doc->gtweak->Init_Table();
	doc->gtweak->Modify_Table(doc->printerData->num_tweaks, doc->printerData->tweaks);
}

#include "profileview.h"

/////////////////////////////////////////////////////////////////////////////
// CTuningDlg property page

IMPLEMENT_DYNCREATE(CTuningDlg, CPropertyDlg)

// Message Map
BEGIN_MESSAGE_MAP(CTuningDlg, CPropertyDlg)
	//{{AFX_MSG_MAP(CTuningDlg)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_TWEAK_ADD, OnTweakAdd)
	ON_CBN_EDITCHANGE(IDC_TWEAK_SELECT, OnTweakSelectEditchange)
	ON_CBN_SELCHANGE(IDC_TWEAK_SELECT, OnTweakSelectSelchange)
	ON_BN_CLICKED(IDC_TWEAK_DELETE, OnTweakDelete)
	ON_BN_CLICKED(IDC_TWEAK_MOVETOOL, OnTweakMoveTool)
	ON_BN_CLICKED(IDC_TWEAK_SIZETOOL, OnTweakSizeTool)
	ON_BN_CLICKED(IDC_TWEAK_PRINTRADIO, OnTweakPrintRadio)
	ON_BN_CLICKED(IDC_TWEAK_GAMUTRADIO, OnTweakGamutRadio)
	ON_EN_CHANGE(IDC_TWEAK_LDESIRED, OnTweakChangeLdesired)
	ON_EN_CHANGE(IDC_TWEAK_ADESIRED, OnTweakChangeAdesired)
	ON_EN_CHANGE(IDC_TWEAK_BDESIRED, OnTweakChangeBdesired)
	ON_BN_CLICKED(IDC_TWEAK_CDESIRED, OnTweakCdesired)
	ON_EN_CHANGE(IDC_TWEAK_LGAMUT, OnTweakChangeLgamut)
	ON_EN_CHANGE(IDC_TWEAK_AGAMUT, OnTweakChangeAgamut)
	ON_EN_CHANGE(IDC_TWEAK_BGAMUT, OnTweakChangeBgamut)
	ON_BN_CLICKED(IDC_TWEAK_CGAMUT, OnTweakCgamut)
	ON_EN_CHANGE(IDC_TWEAK_LPRINT, OnTweakChangeLprint)
	ON_EN_CHANGE(IDC_TWEAK_APRINT, OnTweakChangeAprint)
	ON_EN_CHANGE(IDC_TWEAK_BPRINT, OnTweakChangeBprint)
	ON_BN_CLICKED(IDC_TWEAK_CPRINT, OnTweakCprint)
	ON_WM_SETCURSOR()
//	ON_MESSAGE(WM_COMM_REQUEST_COMPLETE, OnRequestComplete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Event Map
BEGIN_EVENTSINK_MAP(CTuningDlg, CPropertyDlg)
    //{{AFX_EVENTSINK_MAP(CTuningDlg)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

CTuningDlg::CTuningDlg() : CPropertyDlg(CTuningDlg::IDD)
{

}

/////////////////////////////////////////////////////////////////////////////
CTuningDlg::CTuningDlg(CProfileView *pView) : CPropertyDlg(CTuningDlg::IDD), m_Qd3dGamut()
{
	for (int i = 0; i< MAX_NUM_TWEAKS; i++)
		m_pTweaks[i] = NULL;
	m_pCurrentTweak = NULL;
	m_nCurrentTweak = 0;
	m_nTweaks = 0;

	m_pControl = NULL;

	m_colorDesired	= RGB(255,0,0);
	m_colorDesired2	= RGB(255,0,0);
	m_colorGamut	= RGB(255,0,0);
	m_colorGamut2	= RGB(255,0,0);
	m_colorPrint	= RGB(255,0,0);
	m_colorPrint2	= RGB(255,0,0);
	m_inited = FALSE;
	m_Qd3dGamut.Document.fModel = NULL;
	m_pView = pView;
	//{{AFX_DATA_INIT(CTuningDlg)
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CTuningDlg::~CTuningDlg()
{
	// Delete any existing local tweaks
	for (int i = 0; i< m_nTweaks; i++)
	{
		delete m_pTweaks[i];
		m_pTweaks[i] = NULL;
	}

	if (m_pControl)
		delete m_pControl;
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTuningDlg)
	DDX_Control(pDX, IDC_TWEAK_GAMUT, m_wndGamut);
	DDX_Control(pDX, IDC_TWEAK_LRSLIDER, m_sldLightRange);
	DDX_Control(pDX, IDC_TWEAK_LRVALUE, m_staLightRange);
	DDX_Control(pDX, IDC_TWEAK_CRSLIDER, m_sldColorRange);
	DDX_Control(pDX, IDC_TWEAK_CRVALUE, m_staColorRange);
	DDX_Control(pDX, IDC_TWEAK_LSCROLL, m_scrLightness);
	DDX_Control(pDX, IDC_TWEAK_LVALUE, m_staLightness);
	DDX_Control(pDX, IDC_TWEAK_SELECT, m_comboTweaks);
	DDX_Control(pDX, IDC_TWEAK_MOVETOOL, m_radioMoveTool);
	DDX_Control(pDX, IDC_TWEAK_SIZETOOL, m_radioSizeTool);
	DDX_Control(pDX, IDC_TWEAK_GAMUTRADIO, m_radioGamut);
	DDX_Control(pDX, IDC_TWEAK_PRINTRADIO, m_radioPrint);
	DDX_Control(pDX, IDC_TWEAK_LDESIRED, m_editLDesired);
	DDX_Control(pDX, IDC_TWEAK_ADESIRED, m_editADesired);
	DDX_Control(pDX, IDC_TWEAK_BDESIRED, m_editBDesired);
	DDX_Control(pDX, IDC_TWEAK_CDESIRED, m_colrCDesired);
	DDX_Control(pDX, IDC_TWEAK_LDESIRED2, m_staLDesired2);
	DDX_Control(pDX, IDC_TWEAK_ADESIRED2, m_staADesired2);
	DDX_Control(pDX, IDC_TWEAK_BDESIRED2, m_staBDesired2);
	DDX_Control(pDX, IDC_TWEAK_CDESIRED2, m_colrCDesired2);
	DDX_Control(pDX, IDC_TWEAK_LGAMUT, m_editLGamut);
	DDX_Control(pDX, IDC_TWEAK_AGAMUT, m_editAGamut);
	DDX_Control(pDX, IDC_TWEAK_BGAMUT, m_editBGamut);
	DDX_Control(pDX, IDC_TWEAK_CGAMUT, m_colrCGamut);
	DDX_Control(pDX, IDC_TWEAK_LGAMUT2, m_staLGamut2);
	DDX_Control(pDX, IDC_TWEAK_AGAMUT2, m_staAGamut2);
	DDX_Control(pDX, IDC_TWEAK_BGAMUT2, m_staBGamut2);
	DDX_Control(pDX, IDC_TWEAK_CGAMUT2, m_colrCGamut2);
	DDX_Control(pDX, IDC_TWEAK_LPRINT, m_editLPrint);
	DDX_Control(pDX, IDC_TWEAK_APRINT, m_editAPrint);
	DDX_Control(pDX, IDC_TWEAK_BPRINT, m_editBPrint);
	DDX_Control(pDX, IDC_TWEAK_CPRINT, m_colrCPrint);
	DDX_Control(pDX, IDC_TWEAK_LPRINT2, m_staLPrint2);
	DDX_Control(pDX, IDC_TWEAK_APRINT2, m_staAPrint2);
	DDX_Control(pDX, IDC_TWEAK_BPRINT2, m_staBPrint2);
	DDX_Control(pDX, IDC_TWEAK_CPRINT2, m_colrCPrint2);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTuningDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CTuningDlg::OnInitDialog() 
{
	CPropertyDlg::OnInitDialog();

	
	m_bMouseDown = FALSE;
	m_ingamut = TRUE;

	SetQd3dErrorHandler();

	m_sldLightRange.SetRange(0, 100, FALSE/*bRedraw*/);
	m_sldLightRange.SetTicFreq(10);

	m_sldColorRange.SetRange(0, 100, FALSE/*bRedraw*/);
	m_sldColorRange.SetTicFreq(10);

	m_scrLightness.SetScrollRange(0, 100, FALSE/*bRedraw*/);

	m_comboTweaks.ResetContent();
	m_comboTweaks.SetCurSel(0);

	m_colrCDesired.SetRGBColor(RGB(255,0,0));
	m_colrCDesired2.SetRGBColor(RGB(255,0,0));
	m_colrCGamut.SetRGBColor(RGB(255,0,0));
	m_colrCGamut2.SetRGBColor(RGB(255,0,0));
	m_colrCPrint.SetRGBColor(RGB(255,0,0));
	m_colrCPrint2.SetRGBColor(RGB(255,0,0));

	m_radioMoveTool.SetBitmap( ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MOVEPLANE)) );
	m_radioSizeTool.SetBitmap( ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MOVEDEPTH)) );

	DoTweakSetTool(kColorDesired);
		
	SetTweakRadio(FALSE/*bGamutMapping*/);
	Update3dGamut(50); // Default value for gamut view (slice)

	Init3D();
	GetDataFromDoc(NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Dlgs should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::GetDataFromDoc(CProfileDoc* pDoc) 
{
	if (!pDoc)
		pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return;

	m_nTweaks = pDoc->GetTweaks(m_pTweaks, m_nTweaks);
	m_nCurrentTweak = 0;
	m_pCurrentTweak = m_pTweaks[m_nCurrentTweak];

	EnableAllButAdd(NULL, m_nTweaks > 0 /*bEnable*/);

	ReloadTweakList();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::PutTweaksIntoDoc(void) 
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (pDoc)
		pDoc->PutTweaks(m_pTweaks, m_nTweaks);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::ReloadTweakList(void) 
{
	m_comboTweaks.ResetContent();
	for (int i = 0; i < m_nTweaks; i++)
		m_comboTweaks.AddString(m_pTweaks[i]->name);
	m_comboTweaks.SetCurSel(m_nCurrentTweak);

	// Update all of the other controls
	SetControlsForCurrentTweak();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::SetControlsForCurrentTweak(void) 
{
	Tweak_Element* pTweak = m_pCurrentTweak;

	Tweak_Element EmptyTweak("");
	memset( &EmptyTweak, 0, sizeof(Tweak_Element));
	if (!pTweak)
		pTweak = &EmptyTweak;

	CString sz;
	sz.Format("%.0f", pTweak->lab_d[0]);	m_editLDesired.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_d[1]);	m_editADesired.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_d[2]);	m_editBDesired.SetWindowText(sz);
	m_colrCDesired.SetLABColor(pTweak->lab_d);
	m_colrCDesired.UpdateWindow();
	m_editLDesired.UpdateWindow();
	m_editADesired.UpdateWindow();
	m_editBDesired.UpdateWindow();
	
	sz.Format("%.0f", pTweak->lab_p[0]);	m_editLPrint.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_p[1]);	m_editAPrint.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_p[2]);	m_editBPrint.SetWindowText(sz);
	m_colrCPrint.SetLABColor(pTweak->lab_p);
	m_colrCPrint.UpdateWindow();
	m_editLPrint.UpdateWindow();
	m_editAPrint.UpdateWindow();
	m_editBPrint.UpdateWindow();

	sz.Format("%.0f", pTweak->lab_g[0]);	m_editLGamut.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_g[1]);	m_editAGamut.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_g[2]);	m_editBGamut.SetWindowText(sz);
	m_colrCGamut.SetLABColor(pTweak->lab_g);
	m_colrCGamut.UpdateWindow();
	m_editLGamut.UpdateWindow();
	m_editAGamut.UpdateWindow();
	m_editBGamut.UpdateWindow();

	m_sldLightRange.SetPos(pTweak->Lr);
	sz.Format("%d", (int)pTweak->Lr);	m_staLightRange.SetWindowText(sz);

	m_sldColorRange.SetPos(pTweak->Cr);
	sz.Format("%d", (int)pTweak->Cr);	m_staColorRange.SetWindowText(sz);

	SetTweakRadio(pTweak->type == GamutComp_Tweak);
	switch(m_kTool)
	{
		case(kColorGamut):
			Update3dGamut(pTweak->lab_g[0]);
			break;

		case(kColorPrint):
			Update3dGamut(pTweak->lab_p[0]);
			break;

		default:
			Update3dGamut(pTweak->lab_d[0]);
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::SetTweakRadio(BOOL bGamutMapping) 
{
	if (m_pCurrentTweak)
		m_pCurrentTweak->type = (bGamutMapping ? GamutComp_Tweak : Calibrate_Tweak);

	m_radioGamut.SetCheck(bGamutMapping);
	m_radioPrint.SetCheck(!bGamutMapping);

	BOOL bEnablePrint = (!bGamutMapping && m_nTweaks>0);
	BOOL bEnableGamut = (bGamutMapping && m_nTweaks>0);
	int nCmdShowPrint = (bEnablePrint ? SW_SHOW : SW_HIDE);
	int nCmdShowGamut = (bEnablePrint ? SW_HIDE : SW_SHOW);

	m_editLPrint.ShowWindow(nCmdShowPrint);
	m_editAPrint.ShowWindow(nCmdShowPrint);
	m_editBPrint.ShowWindow(nCmdShowPrint);
	m_colrCPrint.ShowWindow(nCmdShowPrint);
	m_colrCPrint.Invalidate();

	m_staLPrint2.ShowWindow(nCmdShowPrint);
	m_staAPrint2.ShowWindow(nCmdShowPrint);
	m_staBPrint2.ShowWindow(nCmdShowPrint);
	m_colrCPrint2.ShowWindow(nCmdShowPrint);
	m_colrCPrint2.Invalidate();

/*
	m_editLGamut.EnableWindow(bEnableGamut);
	m_editAGamut.EnableWindow(bEnableGamut);
	m_editBGamut.EnableWindow(bEnableGamut);
	m_colrCGamut.EnableWindow(bEnableGamut);
	m_colrCGamut.Invalidate();
*/

	m_staLGamut2.ShowWindow(nCmdShowGamut);
	m_staAGamut2.ShowWindow(nCmdShowGamut);
	m_staBGamut2.ShowWindow(nCmdShowGamut);
	m_colrCGamut2.ShowWindow(nCmdShowGamut);
	m_colrCGamut2.Invalidate();

	// Change to color tool if we are hiding or showing the matching color patches
	if (m_kTool == kColorGamut && nCmdShowGamut == SW_HIDE)
		DoTweakSetTool(kColorPrint);
	else
	if (m_kTool == kColorPrint && nCmdShowPrint == SW_HIDE)
		DoTweakSetTool(kColorGamut);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::Update3dGamut(int iLvalue) 
{
	// Set the slider
	if (iLvalue >= 0)
	{
		m_scrLightness.SetScrollPos(100 - iLvalue, TRUE/*bRedraw*/);
		// Set the readout
		CString sz;
		sz.Format("%d", iLvalue);
		m_staLightness.SetWindowText(sz);
	}
	else
		iLvalue = 100 - m_scrLightness.GetScrollPos();

	// Set the Gamut window, if we have been initialized
	if (m_Qd3dGamut.Document.fModel)
		m_Qd3dGamut.ChangeTweakData(m_pCurrentTweak, iLvalue);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	int iValue = pSlider->GetPos();

	CString sz;
	sz.Format("%d", iValue);

	switch (pSlider->GetDlgCtrlID()) 
	{
		case IDC_TWEAK_LRSLIDER:
			m_staLightRange.SetWindowText(sz);
			if (m_pCurrentTweak)
				m_pCurrentTweak->Lr = iValue;
			Update3dGamut(-1);
			PutTweaksIntoDoc(); // Keep the doc up to date
			SetupTweaks(GetDoc(this));
			break;
		case IDC_TWEAK_CRSLIDER:
			m_staColorRange.SetWindowText(sz);
			if (m_pCurrentTweak)
				m_pCurrentTweak->Cr = iValue;
			Update3dGamut(-1);
			PutTweaksIntoDoc(); // Keep the doc up to date
			SetupTweaks(GetDoc(this));
			break;
	}	

	CPropertyDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetFocus();

	int iValue = 100 - pScrollBar->GetScrollPos();
	if (nSBCode == SB_LINEUP)			iValue++;
	if (nSBCode == SB_LINEDOWN)			iValue--;
	if (nSBCode == SB_PAGEUP)			iValue += 10;
	if (nSBCode == SB_PAGEDOWN)			iValue -= 10;
	if (nSBCode == SB_THUMBPOSITION)	iValue = 100 - nPos;
	if (nSBCode == SB_THUMBTRACK)		iValue = 100 - nPos;
	if (nSBCode == SB_TOP)				iValue = 100;
	if (nSBCode == SB_BOTTOM)			iValue = 0;
	if (nSBCode == SB_ENDSCROLL)		return;

	int iMinPos, iMaxPos;
	pScrollBar->GetScrollRange(&iMinPos, &iMaxPos);
	if (iValue < iMinPos) iValue = iMinPos;
	if (iValue > iMaxPos) iValue = iMaxPos;

	switch (pScrollBar->GetDlgCtrlID()) 
	{
		case IDC_TWEAK_LSCROLL:
			Update3dGamut(iValue);
			break;
	}	

	CPropertyDlg::OnVScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTuningDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	BOOL bStatus = CPropertyDlg::OnSetCursor(pWnd, nHitTest, message);

	CWnd* pWndGamut = &m_wndGamut;
	if (pWndGamut && pWndGamut->GetTopWindow())
		pWndGamut = pWndGamut->GetTopWindow();

	if (pWndGamut != pWnd)
		return bStatus;

	static POINT ptLast;
	POINT pt;
	GetCursorPos(&pt);
	GetDesktopWindow()->MapWindowPoints(&m_wndGamut, &pt, 1);

	if (message == WM_MOUSEMOVE)
	{
		if (m_bMouseDown)
			OnTweakMouseDown(pt, ptLast);
		else
			OnTweakMouseMove(pt);
	}
	else
	if (message == WM_LBUTTONDOWN)
	{
		m_bMouseDown = TRUE;
		OnTweakMouseDown(pt, ptLast = pt);
	}
	else
	if (message == WM_LBUTTONUP)
		m_bMouseDown = FALSE;

	ptLast = pt;
	
	return bStatus;
}

//#ifdef NOTUSED
/////////////////////////////////////////////////////////////////////////////
BOOL CTuningDlg::IsColorInGamut(double* lab)
{
	ProfileDocFiles* doc = GetDoc(this);
	if (!doc) return 0;

	if (lab[1] < -128) lab[1] = -128;
	if (lab[1] > 127) lab[1] = 127;

	if (lab[2] < -128) lab[2] = -128;
	if (lab[2] > 127) lab[2] = 127;

	int j = 33*lab[0]/100;
	int k = 33*(lab[1]+128)/255;
	int l = 33*(lab[2]+128)/255;

	unsigned char* out_gamut = (unsigned char*)McoLockHandle(doc->_out_of_gamutH);
	BOOL bInGamut;
	if (out_gamut[l+k*33+j*33*33] == 255)
		bInGamut = 0;
	else
		bInGamut = 1;

	McoUnlockHandle(doc->_out_of_gamutH);
	return bInGamut;
}
//#endif NOTUSED


/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakMouseMove(POINT pt) 
{
	if (!m_pCurrentTweak)
		return;

	double lab_d[3] = {100.0 - m_scrLightness.GetScrollPos(), 0.0, 0.0};
	McoStatus status;
//	BOOL ingamut = 0;
	if (m_pCurrentTweak->type == GamutComp_Tweak)
	{
		status = m_Qd3dGamut.Get3Dinfo(pt, lab_d, 0/*GamutCheck*/, &m_ingamut);
//		ingamut = 1 - ingamut;
	}
	else
	if (m_pCurrentTweak->type == Calibrate_Tweak)
	{
		status = m_Qd3dGamut.Get3Dinfo(pt, lab_d, 1/*GamutCheck*/, &m_ingamut);
	}

	if (status == MCO_FAILURE)
		return;

	double lab_p[3];
	double lab_g[3];

	//j Display the color regardless of whether it was in the gamut or not.
	//j This allows Get3Dinfo() to return black outside the gamut

	ProfileDocFiles* doc = GetDoc(this);
	if (doc)
	{
		ConvertToGamut(doc, lab_d, lab_g);
		ConvertToGamutC(doc, lab_d, lab_p);
		ConvertToPrint(doc, lab_p, lab_p);
	}
	else
	{
		lab_p[0] = lab_d[0];
		lab_p[1] = lab_d[1];
		lab_p[2] = lab_d[2];

		lab_g[0] = lab_d[0];
		lab_g[1] = lab_d[1];
		lab_g[2] = lab_d[2];
	}

	CString sz;
	sz.Format("%.0f", lab_d[0]); m_staLDesired2.SetWindowText(sz); m_staLDesired2.UpdateWindow();
	sz.Format("%.0f", lab_d[1]); m_staADesired2.SetWindowText(sz); m_staADesired2.UpdateWindow();
	sz.Format("%.0f", lab_d[2]); m_staBDesired2.SetWindowText(sz); m_staBDesired2.UpdateWindow();
	m_colrCDesired2.SetLABColor(lab_d); m_colrCDesired2.UpdateWindow();

	sz.Format("%.0f", lab_p[0]); m_staLPrint2.SetWindowText(sz); m_staLPrint2.UpdateWindow();
	sz.Format("%.0f", lab_p[1]); m_staAPrint2.SetWindowText(sz); m_staAPrint2.UpdateWindow();
	sz.Format("%.0f", lab_p[2]); m_staBPrint2.SetWindowText(sz); m_staBPrint2.UpdateWindow();
	m_colrCPrint2.SetLABColor(lab_p); m_colrCPrint2.UpdateWindow();

	sz.Format("%.0f", lab_g[0]); m_staLGamut2.SetWindowText(sz); m_staLGamut2.UpdateWindow();
	sz.Format("%.0f", lab_g[1]); m_staAGamut2.SetWindowText(sz); m_staAGamut2.UpdateWindow();
	sz.Format("%.0f", lab_g[2]); m_staBGamut2.SetWindowText(sz); m_staBGamut2.UpdateWindow();
	m_colrCGamut2.SetLABColor(lab_g); m_colrCGamut2.UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakMouseDown(POINT pt, POINT ptLast)
{
	if (m_kTool == kMove)
		DoTweakMoveTool(pt, ptLast);
	else
	if (m_kTool == kSize)
		DoTweakSizeTool(pt, ptLast);
	else
		DoTweakColorPickTool(pt, m_kTool);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakColorPickTool(POINT pt, kTOOL kTool)
{
	// Copy the color values into the tweak
	Tweak_Element* pTweak = m_pCurrentTweak;
	if (!pTweak)
		return;

	CString sz;
	if (kTool == kColorDesired)
	{
		m_staLDesired2.GetWindowText(sz); pTweak->lab_d[0] = atof(sz);
		m_staADesired2.GetWindowText(sz); pTweak->lab_d[1] = atof(sz);
		m_staBDesired2.GetWindowText(sz); pTweak->lab_d[2] = atof(sz);

		ProfileDocFiles* doc = GetDoc(this);
		if (doc)
		{
			ConvertToGamutC(doc, pTweak->lab_d, pTweak->lab_g);
		}
		else
		{
			pTweak->lab_g[0] = pTweak->lab_d[0];
			pTweak->lab_g[1] = pTweak->lab_d[1];
			pTweak->lab_g[2] = pTweak->lab_d[2];
		}
		pTweak->lab_p[0] = pTweak->lab_g[0];
		pTweak->lab_p[1] = pTweak->lab_g[1];
		pTweak->lab_p[2] = pTweak->lab_g[2];

		
		//should check which Radio button is selected
		//if inside the gamut, the printing button should be selected
		//if outside the gamut, the mapping button should be selected
		if (m_pCurrentTweak)
		{
			if(m_ingamut && (m_pCurrentTweak->type == GamutComp_Tweak) )
			{
				SetTweakRadio(FALSE);	
			}
			else if(!m_ingamut && (m_pCurrentTweak->type == Calibrate_Tweak) )
			{
				SetTweakRadio(TRUE);
			}			
		}
	}

//	if (kTool == kColorGamut || kTool == kColorDesired)
	if (kTool == kColorGamut && m_ingamut)
	{
		m_staLDesired2.GetWindowText(sz); pTweak->lab_g[0] = atof(sz);
		m_staADesired2.GetWindowText(sz); pTweak->lab_g[1] = atof(sz);
		m_staBDesired2.GetWindowText(sz); pTweak->lab_g[2] = atof(sz);

		//m_staLGamut2.GetWindowText(sz); pTweak->lab_g[0] = atof(sz);
		//m_staAGamut2.GetWindowText(sz); pTweak->lab_g[1] = atof(sz);
		//m_staBGamut2.GetWindowText(sz); pTweak->lab_g[2] = atof(sz);
	}

//	if (kTool == kColorPrint || kTool == kColorDesired)
	if (kTool == kColorPrint && m_ingamut)
	{
		m_staLDesired2.GetWindowText(sz); pTweak->lab_p[0] = atof(sz);
		m_staADesired2.GetWindowText(sz); pTweak->lab_p[1] = atof(sz);
		m_staBDesired2.GetWindowText(sz); pTweak->lab_p[2] = atof(sz);

		//m_staLPrint2.GetWindowText(sz); pTweak->lab_p[0] = atof(sz);
		//m_staAPrint2.GetWindowText(sz); pTweak->lab_p[1] = atof(sz);
		//m_staBPrint2.GetWindowText(sz); pTweak->lab_p[2] = atof(sz);
	}

	// Update all of the other controls
	SetControlsForCurrentTweak();

	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakRotateTool(POINT pt, POINT ptLast)
{
	// Rotate the 3d model
	double dx = (pt.x - ptLast.x);
	double dy = (pt.y - ptLast.y);
	m_Qd3dGamut.DoTransform(WE_Rotate, dy/100, dx/100, 0);
	m_Qd3dGamut.MakeCross(FALSE);
	m_Qd3dGamut.DocumentDraw3DData();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakMoveTool(POINT pt, POINT ptLast)
{
	// Translate the 3d model
	double dx = (pt.x - ptLast.x);
	double dy = (pt.y - ptLast.y);
	m_Qd3dGamut.DoTransform(WE_Translate, dx/175, dy/175, 0);
	m_Qd3dGamut.MakeCross(FALSE);
	m_Qd3dGamut.DocumentDraw3DData();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakSizeTool(POINT pt, POINT ptLast)
{
	// Scale the 3d model
	double dy = (pt.y - ptLast.y);
	m_Qd3dGamut.DoTransform(WE_Scale, dy/200, 0, 0);
	m_Qd3dGamut.MakeCross(FALSE);
	m_Qd3dGamut.DocumentDraw3DData();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakMoveTool() 
{
	DoTweakSetTool(m_kTool == kMove ? kColorDesired : kMove);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakSizeTool() 
{
	DoTweakSetTool(m_kTool == kSize ? kColorDesired : kSize);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakSetTool(kTOOL kTool)
/////////////////////////////////////////////////////////////////////////////
{
	m_kTool = kTool;
	m_radioMoveTool.SetCheck(m_kTool == kMove);
	m_radioSizeTool.SetCheck(m_kTool == kSize);
	m_colrCDesired.SetCheck(m_kTool == kColorDesired);
	m_colrCGamut.SetCheck(m_kTool == kColorGamut);
	m_colrCPrint.SetCheck(m_kTool == kColorPrint);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakPrintRadio() 
{
	ProfileDocFiles* doc = GetDoc(this);
	if(doc && m_pCurrentTweak)
	{
		ConvertToGamutC(doc, m_pCurrentTweak->lab_d, m_pCurrentTweak->lab_g);
	}

	SetTweakRadio(FALSE/*bGamutMapping*/);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakGamutRadio() 
{
	SetTweakRadio(TRUE/*bGamutMapping*/);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakSelectEditchange() 
{
	DWORD dw = m_comboTweaks.GetEditSel();

	CString szName;
	m_comboTweaks.GetWindowText(szName);
	memcpy(m_pCurrentTweak->name, szName, sizeof(m_pCurrentTweak->name));
	m_comboTweaks.DeleteString(m_nCurrentTweak);
	m_comboTweaks.InsertString(m_nCurrentTweak, szName);
	m_comboTweaks.SetCurSel(m_nCurrentTweak);
	m_comboTweaks.SetEditSel(LOWORD(dw), HIWORD(dw));
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakSelectSelchange() 
{
	m_nCurrentTweak = m_comboTweaks.GetCurSel();
	m_pCurrentTweak = m_pTweaks[m_nCurrentTweak];

	// Update all of the other controls
	SetControlsForCurrentTweak();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::SetupTweak(Tweak_Element* tweak) 
{	
	ProfileDocFiles* pDoc = GetDoc(this);
	if(!pDoc || !tweak)	return; 

	if(tweak->type == Calibrate_Tweak)
	{
		ConvertToGamutC(pDoc, tweak->lab_d, tweak->lab_g);	
	}

}

BOOL CTuningDlg::IsDlgOkToShow(void)
{
	if(m_inited)	return TRUE;
	else
	{
		m_inited = BuildGamut();
		return m_inited;
	}
}

BOOL CTuningDlg::BuildGamut()
{
//	if (!m_Qd3dGamut.Document.fModel)
//	{
//	}

	CProfileDoc* pDoc = m_pView->GetDocument();
	if (!pDoc)
		return FALSE;

	if( pDoc->_profiledocfiles.copyOutOfPrintData() != MCO_SUCCESS)
	{
		Message("Wrong DryJet data.");
		return FALSE;
	}
			
	CBuildProgress	buildprogress( (long)&pDoc->_profiledocfiles, 
		(long)&m_gamut, Process_GammutCompress, pDoc->GetTitle());
	if(buildprogress.DoModal() == IDCANCEL)
	{
		return FALSE;
	}

	m_ingamut = TRUE;

	return TRUE;
}

BOOL CTuningDlg::Init3D()
{
	RECT rect;
	m_wndGamut.GetClientRect(&rect);

	m_pControl = new CQd3dControl(rect, &m_wndGamut, &m_Qd3dGamut, -1/*id*/);
	if (!m_pControl)	return FALSE;

	BeginWaitCursor();

	McoStatus status = m_Qd3dGamut.InitFromDoc(m_gamut, m_pControl->m_hWnd);
	if(status != MCO_SUCCESS)
	{
		Message("Can't load QuickDraw3D.");
		return FALSE;
	}

	Update3dGamut(-1);
	EndWaitCursor();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakAdd() 
{
	// Create a gamut control window to display the 3d data, and to catch all mouse moves and button clicks
	// Initialize the control and the attached 3d data, the first time a tweak is added
	// This process can be moved to another convenient location, but was done here so
	// the Tuning tab can be displayed initially without a wait

//	Init3D();

	if (m_nTweaks >= MAX_NUM_TWEAKS)
		return; 

	// Create a new tweak
	CString szTweakName;
	szTweakName.Format("Tweak Number %d", m_nTweaks);
	Tweak_Element* pTweak = new Tweak_Element((char*)(LPCTSTR)szTweakName);
	if (!pTweak)
		return;

	// Set in the default values
	pTweak->lab_d[0] = 50;	//(((long)rand() * 100) / RAND_MAX);
	pTweak->lab_d[1] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	pTweak->lab_d[2] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	
	pTweak->lab_p[0] = 50;	//(((long)rand() * 100) / RAND_MAX);
	pTweak->lab_p[1] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	pTweak->lab_p[2] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	
	pTweak->lab_g[0] = 50;	//(((long)rand() * 100) / RAND_MAX);
	pTweak->lab_g[1] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	pTweak->lab_g[2] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;

	pTweak->Lr = 10;		//(((long)rand() * 100) / RAND_MAX); //50;
	pTweak->Cr = 10;		//(((long)rand() * 100) / RAND_MAX); //50;
	pTweak->type = Calibrate_Tweak;

	// Add it to the list
	m_pTweaks[m_nTweaks] = pTweak;
	m_pCurrentTweak = pTweak;
	m_nCurrentTweak = m_nTweaks;

	// Increment the number of tweaks
	m_nTweaks++;
	EnableAllButAdd(NULL, m_nTweaks > 0 /*bEnable*/);

	// Update the combobox list
	m_comboTweaks.AddString(m_pCurrentTweak->name);
	m_comboTweaks.SetCurSel(m_nCurrentTweak);
	
	// Update all of the other controls
	SetControlsForCurrentTweak();

	SetButtonStyle(1);
	PutTweaksIntoDoc(); // Keep the doc up to date

	//setup the compressed lab
	SetupTweak(m_pCurrentTweak);
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakDelete() 
{
	if (!m_pCurrentTweak)
		return;

	// Delete the current tweak
	delete m_pCurrentTweak;
	int nDeletedTweak = m_nCurrentTweak;

	// Remove it from the list
	for (int j = m_nCurrentTweak+1; j < m_nTweaks; j++)
	{
		m_pTweaks[j-1] = m_pTweaks[j];
		m_pTweaks[j] = NULL;
	}

	// Decrement the number of tweaks
	m_nTweaks--;
	EnableAllButAdd(NULL, m_nTweaks > 0 /*bEnable*/);

	// Make sure the current tweak is still valid
	if (m_nCurrentTweak >= m_nTweaks)
		m_nCurrentTweak = m_nTweaks - 1;

	// Reset the current tweak pointer
 	m_pCurrentTweak = m_pTweaks[m_nCurrentTweak];

	// Update the combobox list
	m_comboTweaks.DeleteString(nDeletedTweak);
	m_comboTweaks.SetCurSel(m_nCurrentTweak);

	// Update all of the other controls
	SetControlsForCurrentTweak();

	PutTweaksIntoDoc(); // Keep the doc up to date

	//setup the compressed lab
	SetupTweak(m_pCurrentTweak);
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeLdesired() 
{
	if (GetFocus() != &m_editLDesired) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editLDesired.GetWindowText(szValue);
	m_pCurrentTweak->lab_d[0] = atof(szValue);
	m_colrCDesired.SetLABColor(m_pCurrentTweak->lab_d);
	Update3dGamut(m_pCurrentTweak->lab_d[0]);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeAdesired() 
{
	if (GetFocus() != &m_editADesired) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editADesired.GetWindowText(szValue);
	m_pCurrentTweak->lab_d[1] = atof(szValue);
	m_colrCDesired.SetLABColor(m_pCurrentTweak->lab_d);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeBdesired() 
{
	if (GetFocus() != &m_editBDesired) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editBDesired.GetWindowText(szValue);
	m_pCurrentTweak->lab_d[2] = atof(szValue);
	m_colrCDesired.SetLABColor(m_pCurrentTweak->lab_d);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeLgamut() 
{
	if (GetFocus() != &m_editLGamut) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editLGamut.GetWindowText(szValue);
	m_pCurrentTweak->lab_g[0] = atof(szValue);
	m_colrCGamut.SetLABColor(m_pCurrentTweak->lab_g);
	Update3dGamut(m_pCurrentTweak->lab_g[0]);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeAgamut() 
{
	if (GetFocus() != &m_editAGamut) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editAGamut.GetWindowText(szValue);
	m_pCurrentTweak->lab_g[1] = atof(szValue);
	m_colrCGamut.SetLABColor(m_pCurrentTweak->lab_g);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeBgamut() 
{
	if (GetFocus() != &m_editBGamut) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editBGamut.GetWindowText(szValue);
	m_pCurrentTweak->lab_g[2] = atof(szValue);
	m_colrCGamut.SetLABColor(m_pCurrentTweak->lab_g);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeLprint() 
{
	if (GetFocus() != &m_editLPrint) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editLPrint.GetWindowText(szValue);
	m_pCurrentTweak->lab_p[0] = atof(szValue);
	m_colrCPrint.SetLABColor(m_pCurrentTweak->lab_p);
	Update3dGamut(m_pCurrentTweak->lab_p[0]);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeAprint() 
{
	if (GetFocus() != &m_editAPrint) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editAPrint.GetWindowText(szValue);
	m_pCurrentTweak->lab_p[1] = atof(szValue);
	m_colrCPrint.SetLABColor(m_pCurrentTweak->lab_p);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeBprint() 
{
	if (GetFocus() != &m_editBPrint) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editBPrint.GetWindowText(szValue);
	m_pCurrentTweak->lab_p[2] = atof(szValue);
	m_colrCPrint.SetLABColor(m_pCurrentTweak->lab_p);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakCdesired()
{
	SetButtonStyle(1);
	DoTweakSetTool(kColorDesired);
	Update3dGamut(m_pCurrentTweak->lab_d[0]);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakCgamut() 
{
	if (!m_pCurrentTweak)	return;

	if (m_pCurrentTweak->type == Calibrate_Tweak)	return;

	SetButtonStyle(2);
	DoTweakSetTool(kColorGamut);
	Update3dGamut(m_pCurrentTweak->lab_g[0]);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakCprint() 
{
	//BOOL test = m_colrCPrint.ModifyStyleEx(WS_EX_DLGMODALFRAME, WS_EX_CLIENTEDGE);
	SetButtonStyle(3);
	DoTweakSetTool(kColorPrint);
	Update3dGamut(m_pCurrentTweak->lab_p[0]);
}

void CTuningDlg::SetButtonStyle(int which)
{
	BOOL	test;
	switch(which)
	{
		case(1):
		test = m_colrCDesired.ModifyStyleEx(1, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
		test = m_colrCGamut.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		test = m_colrCPrint.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		break;

		case(2):
		m_colrCDesired.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		m_colrCGamut.ModifyStyleEx(1, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
		m_colrCPrint.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		break;

		case(3):
		m_colrCDesired.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		m_colrCGamut.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		m_colrCPrint.ModifyStyleEx(1, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
		break;
	}

	CRect rect;

	m_colrCDesired.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);
	m_colrCGamut.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);
	m_colrCPrint.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);

}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::EnableAllButAdd(CWnd* pControl, BOOL bEnable)
{
	if (!pControl)
		pControl = GetTopWindow();

	while (pControl)
	{
		int i = (int)(short)pControl->GetDlgCtrlID();
		if (i != IDC_TWEAK_ADD)
		{
			if (!pControl->IsWindowEnabled() != !bEnable)
			{
				pControl->EnableWindow(bEnable);
				pControl->Invalidate();
			}
		}

		pControl = pControl->GetNextWindow();
	}
}


/******************************************************************************
* Name: CMeasureDialog::OnRequestComplete
*
* Purpose: handle the request returned by communication device
******************************************************************************/
/*
afx_msg LRESULT CTuningDlg::OnRequestComplete(WPARAM wParam, LPARAM lParam)
{
	double	lab[3];

	CCommRequest *pRequest = (CCommRequest*)lParam;

	if(!m_pdevice || !pRequest)	return 0;

	//check error
	switch(pRequest->m_dwRequestError){
	case(ERR_NOERROR):
		break;

	case(ERR_CANCELLED):
		//delete pRequest;
		return 0;

	case(ERR_MAXTRIES):
		//delete pRequest;
		ErrorMessage(ERR_TIMEOUT);
		return 0;

	case(ERR_TERMINATED):
		return 1;

	default:
		//delete pRequest;
		ErrorMessage(ERR_DEVICE);
		return 0;
	}

	int status = 0;
	//no error, then put the data
	if(m_pdevice->ProcessResult(pRequest, lab))
	{
		if(	m_whiteCalibrated == FALSE )
		{			
		//	SetPaperDensity(cmyk);
			m_whiteCalibrated = TRUE;
			if(	m_deviceSetup.deviceType == XRITE_408 )	//xrite 408
			{
				m_firstdialog.EndDialog(IDOK);
			}
			ReadNextPatch();	
		}
		else
		{
//			cmyk[m_dataStart] -= m_paper[m_dataStart];

			status = AdjustLab(&lab[m_dataStart]);
			if(status == 0)
			{
				PutonEdit(&lab[m_dataStart]);

				ReadNextPatch();	
			}
			else if(status == 1)
			{
				OnMeasure();
				return 0;
			}
			else
			{
				ReadNextPatch();	
			}
		}

	}

	delete pRequest;

	return 1;
}

void CTuningDialog::ReadNextPatch()
{
	//terminate if current is larger than total
	if(m_whitecalibrated == FALSE)
	{
		m_dataStart = 0;
		m_pdevice->m_type = 2;
		m_pdevice->SendRequest(m_currentPatch, (CWnd*)this);
	}
	else{
		m_pdevice->SendRequest(m_currentPatch, (CWnd*)this);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCalibrateDlg::OnMeasure() 
{
	int todo;

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return;


	memcpy((char*)&m_deviceSetup, (char*)&(pDoc->m_deviceSetup), sizeof(DeviceSetup));
	if(InitReading()){
		m_runningstatus = MEASURING;
		m_bMeasure.SetWindowText("Stop");
		UpdateData(FALSE);
	}
	else
		CleanupDevice();
	break;

	case(MEASURING):	//stop reading
		OnMeasureDone();
		if(m_pdevice){
			CleanupDevice();
			m_runningstatus = STOPED;
			m_bMeasure.SetWindowText("Measure");
			UpdateData(FALSE);
		}
		break;
	}

}
*/
// MeasureDialog.cpp : implementation file
//

#include "stdafx.h"
#include "colorgenie.h"
#include "MeasureDialog.h"

#include "Xrite408.h"
#include "GretagScan.h"
#include "GretagScanDensity.h"
#include "TechkonTable.h"
#include "ErrorMessage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define START_READING				1
#define CONT_READING				2
#define STOP_READING				3

/////////////////////////////////////////////////////////////////////////////
// CMeasureDialog dialog


CMeasureDialog::CMeasureDialog(int measuretype, CProfileDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CMeasureDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMeasureDialog)
	m_strPatch = _T("");
	//}}AFX_DATA_INIT

	m_phWnd = pParent->m_hWnd;
	m_pDoc = pDoc;

	int *patches = m_pDoc->m_patches;
	m_refRgb = m_pDoc->GetRgbs(3);

	//set the current patch and total patch numbers
	m_currentPatch = 0;

	m_labPatches = patches[0];
	m_cyanStartPatches = m_labPatches;
	m_magentaStartPatches = patches[0] + patches[1];
	m_yellowStartPatches = m_magentaStartPatches + patches[2];
	m_blackStartPatches = m_yellowStartPatches + patches[3];

	m_totalPatches = m_blackStartPatches + patches[4];
	m_numCols = 3;

	m_pdevice = 0;

	//default device setup
	m_deviceSetup.numPorts = 0;
	m_deviceSetup.deviceType = XRITE_408;
	m_deviceSetup.port	= 0;
	m_deviceSetup.baudRate = 9600;
	m_deviceSetup.calFrequency = 50;

	//set running status
	m_runningstatus = START_READING;
	
	//paper density
	for(int i = 0; i < 4; i++)
		m_paper[i] = 0.0;
	m_haspaper = FALSE;			
}

CMeasureDialog::~CMeasureDialog()
{
	CleanupDevice();
}

BOOL CMeasureDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here	
	m_ctlGrid.InitLAB(m_totalPatches, m_refRgb);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMeasureDialog::CleanupDevice()
{
	if(m_pdevice){
		delete m_pdevice;
		m_pdevice = 0;
	}

	m_currentPatch = 0;
	m_runningstatus = START_READING;
	for(int i = 0; i < 4; i++)
		m_paper[i] = 0.0;	
	m_haspaper = FALSE;			
}

void CMeasureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeasureDialog)
	DDX_Control(pDX, IDOK, m_bOk);
	DDX_Control(pDX, IDC_START_BUT, m_bStart);
	DDX_Control(pDX, IDC_SETUP_BUTTON, m_bSetup);
	DDX_Text(pDX, IDC_STATUS_EDIT, m_strStatus);
	DDX_Control(pDX, IDC_GRID, m_ctlGrid);
	DDX_Text(pDX, IDC_PATCH_NUM, m_strPatch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMeasureDialog, CDialog)
	//{{AFX_MSG_MAP(CMeasureDialog)
	ON_BN_CLICKED(IDC_START_BUT, OnStartBut)
	ON_BN_CLICKED(IDC_SETUP_BUTTON, OnSetupButton)
	ON_MESSAGE(WM_COMM_REQUEST_COMPLETE, OnRequestComplete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasureDialog message handlers

BOOL CMeasureDialog::InitReading()
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
		m_pdevice = new CGretagScan();
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
	ReadNextPatch();

	return TRUE;	
}

void CMeasureDialog::OnStartBut() 
{
	//check running status
	switch(m_runningstatus){
	case(START_READING):
		if(InitReading()){
			m_runningstatus = CONT_READING;
			m_bStart.SetWindowText("Stop");
			m_bSetup.EnableWindow(FALSE);
			UpdateData(FALSE);
		}
		else
			CleanupDevice();
		break;

	case(STOP_READING):
		if(m_pdevice){
			m_runningstatus = CONT_READING;
			m_bStart.SetWindowText("Stop");
			UpdateData(FALSE);			

			//resume reading
			ReadNextPatch();		
		}
		break;

	case(CONT_READING):
		if(m_pdevice){
			m_pdevice->CancelOperation();
			m_runningstatus = STOP_READING;
			m_bStart.SetWindowText("Start");
			UpdateData(FALSE);
		}
		break;
	}
}

void CMeasureDialog::ReadNextPatch()
{
	//terminate if current is larger than total
	if(m_haspaper == FALSE)
	{
		m_numCols = 4;
		m_dataStart = 0;
		m_pdevice->m_type = 2;
		m_pdevice->SendRequest(m_currentPatch, (CWnd*)this);
	}
	else if(m_currentPatch >= m_totalPatches){
		CleanupDevice();
		m_ctlGrid.GoHome();
		m_bOk.EnableWindow(TRUE);
		m_bStart.EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	else{
		SetGridPosition();

		SetParamForGrid();

		char patchstr[10];
		sprintf(patchstr, "%d", m_currentPatch+1);
		m_strPatch = patchstr;
		UpdateData(FALSE);

		m_pdevice->SendRequest(m_currentPatch, (CWnd*)this);
	}
}

void CMeasureDialog::SetParamForGrid()
{
	if( m_currentPatch < m_labPatches)
	{
		m_numCols = 3;
		m_dataStart = 0;
		m_pdevice->m_type = 1;
	}
	else if(m_currentPatch < m_magentaStartPatches)
	{
		m_numCols = 1;
		m_dataStart = 0;
		m_pdevice->m_type = 2;
	}
	else if(m_currentPatch < m_yellowStartPatches)
	{
		m_numCols = 1;
		m_dataStart = 1;
		m_pdevice->m_type = 2;
	}
	else if(m_currentPatch < m_blackStartPatches)
	{
		m_numCols = 1;
		m_dataStart = 2;
		m_pdevice->m_type = 2;
	}
	else
	{
		m_numCols = 1;
		m_dataStart = 3;
		m_pdevice->m_type = 2;
	}
}

void CMeasureDialog::SetGridPosition()
{
	//compute the grid position
	int iTopRow;

	m_currentRow = m_currentPatch + 1;
	m_currentCol = 2;

	// Now make sure the row is within view
	#define ROWS_IN_VIEW 13

	if (m_currentRow <  m_ctlGrid.GetTopRow() ||
		m_currentRow >= m_ctlGrid.GetTopRow() + ROWS_IN_VIEW )
	{
		int iTopRow = m_currentRow - (ROWS_IN_VIEW/2);
		if (iTopRow < m_ctlGrid.GetFixedRows() )
			iTopRow = m_ctlGrid.GetFixedRows();
		m_ctlGrid.SetTopRow(iTopRow);
	}

	m_ctlGrid.SetRow(m_currentRow);
	m_ctlGrid.SetCol(m_currentCol);
}

void CMeasureDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CMeasureDialog::OnOK() 
{
	// TODO: Add extra validation here

	//send a message
	WPARAM wParam = (WPARAM)&m_ctlGrid;
	LPARAM lParam = (LPARAM)FALSE;

	LRESULT lresult = ::SendMessage(m_phWnd, WM_DATA_READY, wParam, lParam);
	CDialog::OnOK();
}

BEGIN_EVENTSINK_MAP(CMeasureDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CMeasureDialog)
	ON_EVENT(CMeasureDialog, IDC_GRID, -600 /* Click */, OnClickGrid, VTS_NONE)
	ON_EVENT(CMeasureDialog, IDC_GRID, -603 /* KeyPress */, OnKeyPressGrid, VTS_PI2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMeasureDialog::OnClickGrid() 
{
	// TODO: Add your control notification handler code here
//	if( m_runningstatus == CONT_READING )	return;

	int selectrow = m_ctlGrid.GetRowSel() - 1;
	char patchstr[10];
	sprintf(patchstr, "%d", selectrow+1);
	m_strPatch = patchstr;
	UpdateData(FALSE);
}

void CMeasureDialog::OnKeyPressGrid(short FAR* KeyAscii) 
{
	// TODO: Add your control notification handler code here
	
}

void CMeasureDialog::OnSetupButton() 
{
	int	result;
	int	baudrate[] = { 4800, 9600 };
	int	frequency[] = { 50, 100 };
	int i;

	// TODO: Add your control notification handler code here
	CDeviceSheet	devicesheet(IDS_DEVICE, this);

	devicesheet.m_deviceSetting.m_port  = m_deviceSetup.port;
	for(i = 0; i < 2; i++){
		if(baudrate[i] == m_deviceSetup.baudRate)
			break;
	}
	devicesheet.m_deviceSetting.m_baudRate = i;
	devicesheet.m_deviceSetting.m_deviceType = m_deviceSetup.deviceType;
	for(i = 0; i < 2; i++){
		if(frequency[i] == m_deviceSetup.calFrequency)
			break;
	}
	devicesheet.m_patchFormat.m_calFrequency = i;

	if( devicesheet.DoModal() == IDOK){
		m_deviceSetup.port = devicesheet.m_deviceSetting.m_port;
		m_deviceSetup.baudRate = baudrate[devicesheet.m_deviceSetting.m_baudRate];
		m_deviceSetup.deviceType = devicesheet.m_deviceSetting.m_deviceType;
		m_deviceSetup.calFrequency = frequency[devicesheet.m_patchFormat.m_calFrequency];
	}

}

/******************************************************************************
* Name: CMeasureDialog::OnRequestComplete
*
* Purpose: handle the request returned by communication device
******************************************************************************/
afx_msg LRESULT CMeasureDialog::OnRequestComplete(WPARAM wParam, LPARAM lParam)
{
	double	cmyk[4];

	CCommRequest *pRequest = (CCommRequest*)lParam;

	if(!pRequest)	return 0;

	//check error
	switch(pRequest->m_dwRequestError){
	case(ERR_NOERROR):
		break;

	case(ERR_CANCELLED):
		//delete pRequest;
		return 0;

	case(ERR_MAXTRIES):
		//delete pRequest;
		OnStartBut();
		ErrorMessage(ERR_TIMEOUT);
		return 0;

	default:
		//delete pRequest;
		ErrorMessage(ERR_DEVICE);
		return 0;
	}

	//no error, then put the data
	if(m_pdevice->ProcessResult( pRequest, cmyk)){

		if(	m_haspaper == FALSE )
		{			
			SetPaperDensity(cmyk);
			m_haspaper = TRUE;
		}
		else
		{
			if( m_numCols == 1 && m_haspaper == TRUE )
				cmyk[m_dataStart] -= m_paper[m_dataStart];

			PutonGrid(m_currentRow, m_currentCol, m_currentCol+m_numCols, &cmyk[m_dataStart]);

			m_currentPatch++;

			if( !(m_currentPatch % m_deviceSetup.calFrequency) )
				if(!m_pdevice->Calibrate())	return 0;
		}

		//request more data
		ReadNextPatch();	
	}

	delete pRequest;

	return 1;
}

void CMeasureDialog::SetPaperDensity(double* cmyk)
{
	for(int i = 0; i < 4; i++)
		m_paper[i] = cmyk[i];	
}

void CMeasureDialog::PutonGrid(int row, int startcol, int endcol, double *data)
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

	if(m_currentPatch < m_labPatches)
		m_ctlGrid.InitLABRowLabels(row-1, row-1);
	else{
		char cmyk[4];
		if(GetRgb(data[0], cmyk))
			m_ctlGrid.InitCMYKRowLabels(1, row-1, row-1, cmyk);
		else
			m_ctlGrid.InitCMYKRowLabels(1, row-1, row-1, NULL);
	}
}

BOOL CMeasureDialog::GetRgb(double density, char* cmyk)
{
	double ink;

	CApp* app = (CApp*)AfxGetApp( );
	if(app && app->mhasCmm)	
	{
		for(int i = 0; i < 4; i++)
			cmyk[i] = 0;

		ink = 100*density/1.5;
		if(ink > 100) ink = 100;
		if(ink < 0)	ink = 0;

		cmyk[m_dataStart] = (char)(ink*2.55 + 0.5);

		app->mcmmConvert.ApplyXform(cmyk, cmyk, 8, 1);
		return TRUE;
	}
	return FALSE;		 
}


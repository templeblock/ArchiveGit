// BuildProgress.cpp : implementation file
//

#include "stdafx.h"
#include "colorgenie.h"
#include "BuildProgress.h"
#include "Gamutdata.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short UpdateBuildStatus(short state, long scale, unsigned long data);

/////////////////////////////////////////////////////////////////////////////
// CBuildProgress dialog


CBuildProgress::CBuildProgress(long input, long data, ProcessingStates type, 
				const char* name, CWnd* pParent /*=NULL*/)
	:m_title(name), CDialog(CBuildProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBuildProgress)
	m_strMessage = _T("");
	//}}AFX_DATA_INIT

	switch(type)
	{
		case(Process_BuildingTable):
			m_pProfile = (ProfileDocFiles*)input;
			m_range = 10000;
			break;

		case(Process_GammutCompress):
			m_pProfile = (ProfileDocFiles*)input;
			m_range = 2000;
			break;

		case(Process_GammutSurface):
			m_rawdata = (RawData*)input;
			m_range = 900;
			break;

		default:
			m_pProfile = NULL;
			m_rawdata = NULL;
			break;
	}


	m_data = (McoHandle*)data;
	m_type = type;
	m_pThread = NULL;
	m_hThread = INVALID_HANDLE_VALUE;
	m_bTerminate = FALSE;
	m_therm = 0;
}

CBuildProgress::~CBuildProgress(void)
{
	if(m_therm)
		delete m_therm;
}

void CBuildProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBuildProgress)
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_ctlProgress);
	DDX_Text(pDX, IDC_PROGRESS_EDIT, m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBuildProgress, CDialog)
	//{{AFX_MSG_MAP(CBuildProgress)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PROFILEFINISH, OnFinish)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuildProgress message handlers

void CBuildProgress::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_bTerminate = TRUE;

	//kill the timer event
	KillTimer(m_nTimer);

	// wait for the thread to terminate
	if( m_hThread != INVALID_HANDLE_VALUE)	{
		// wait for thread to terminate
		DWORD dwExitCode;	
		while( ::GetExitCodeThread( m_hThread, &dwExitCode) &&
					 (dwExitCode == STILL_ACTIVE)
				 ) 
		{
			Sleep(40); // turn processing time over to thread
		}
		m_pThread = NULL;

		CloseHandle( m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
	
	CDialog::OnCancel();
}

BOOL CBuildProgress::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_title);

	// TODO: Add extra initialization here

	// initialize timer:
	m_scale = 0; 
	m_nTimer = SetTimer(1, 100, NULL); // 1/10 second
	ASSERT(m_nTimer != 0);

	m_therm = new ThermObject(UpdateBuildStatus, (unsigned long)this);

	switch(m_type)
	{
		case(Process_BuildingTable):
			m_pProfile->thermd = m_therm;
			m_therm->SetUpTherm(Process_Start, m_range);
			m_pThread = AfxBeginThread( BuildProfile, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			break;

		case(Process_GammutCompress):
			m_pProfile->thermd = m_therm;
			m_therm->SetUpTherm(Process_Start, m_range);
			m_pThread = AfxBeginThread( BuildGamut, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			break;

		case(Process_GammutSurface):
			m_therm->SetUpTherm(Process_Start, m_range);
			m_pThread = AfxBeginThread( BuildGamutSurface, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			break;

		default:
			m_pThread = NULL;
			break;
	}

	if( !m_pThread) {
		AfxMessageBox( IDS_CANTSTARTBUILDPROFILE, MB_OK|MB_ICONSTOP);
		EndDialog(IDCANCEL);
	}
	if(!::DuplicateHandle(::GetCurrentProcess(), m_pThread->m_hThread,
				::GetCurrentProcess(), &m_hThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		m_bTerminate = TRUE;
		m_pThread->ResumeThread();
		Sleep(40); // turn processor over to thread for termination
		return FALSE;
	}
	m_pThread->ResumeThread();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

afx_msg LONG CBuildProgress::OnFinish( UINT, LONG )
{ 
	m_bTerminate = TRUE;

	//kill the timer event
	KillTimer(m_nTimer);

	// wait for the thread to terminate
	if( m_hThread != INVALID_HANDLE_VALUE)	
	{
		// wait for thread to terminate
		DWORD dwExitCode;	
		while( ::GetExitCodeThread( m_hThread, &dwExitCode) &&
					 (dwExitCode == STILL_ACTIVE)
				 ) 
		{
			Sleep(40); // turn processing time over to thread
		}
		m_pThread = NULL;

		CloseHandle( m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}


	CDialog::OnOK();
 
	return 1;	
}

short UpdateBuildStatus(short state, long scale, unsigned long data)
{
	if( !data) return 1;

	CBuildProgress* pWnd = (CBuildProgress*)data;
	if( !pWnd || pWnd->m_bTerminate) return 0;

	pWnd->m_scale = scale;

	switch(state) 
	{
		case Process_Start:
		pWnd->m_state = BUILD_STATUS_INIT;
		break;

		case Process_GammutSurface:
		case Process_GammutCompress:
		case Process_BuildingTable:
		pWnd->m_state = BUILD_STATUS_UPDATE;
		break;

		case Process_End:
		if(scale < 0)
		{
			pWnd->m_state = BUILD_STATUS_ERROR;
			return 0;
		}
		else
			pWnd->m_state = BUILD_STATUS_FINISHED;
		break;
	}

	return 1;
}

/******************************************************************************
* Name: BuildProfile
*
* Purpose: worker thread function that actually builds the profile
******************************************************************************/
UINT AFX_CDECL BuildProfile( LPVOID lParam)
{
	if( !lParam) return 2;

	CBuildProgress* pWnd = (CBuildProgress*)lParam;

	if( pWnd->m_bTerminate)
		return 0;

	if( pWnd->m_pProfile->_handle_calibrate() != MCO_SUCCESS)
		pWnd->m_state = BUILD_STATUS_ERROR;
	else
		pWnd->PostMessage(WM_PROFILEFINISH);

	return 1;
}

/******************************************************************************
* Name: ComputeGamutSurface
*
* Purpose: worker thread function that actually builds the gamut surface
******************************************************************************/
UINT AFX_CDECL BuildGamutSurface( LPVOID lParam)
{
	if( !lParam) return 2;

	CBuildProgress* pWnd = (CBuildProgress*)lParam;

	if( pWnd->m_bTerminate)
		return 0;

	CGamutData gamutdata;
	McoStatus status;
	if ((status = gamutdata.Init(pWnd->m_rawdata, pWnd->m_therm)) != MCO_SUCCESS)
		pWnd->m_state = BUILD_STATUS_ERROR;
	else
	{
		*(pWnd->m_data) = gamutdata.gdata_gH;
		pWnd->PostMessage(WM_PROFILEFINISH);
	}

	return 1;
}

/******************************************************************************
* Name: ComputeGamut
*
* Purpose: worker thread function that actually builds the gamut table
******************************************************************************/
UINT AFX_CDECL BuildGamut( LPVOID lParam)
{
	if( !lParam) return 2;

	CBuildProgress* pWnd = (CBuildProgress*)lParam;

	if( pWnd->m_bTerminate)
		return 0;

	McoStatus status;
	if ((status = pWnd->m_pProfile->buildgamut()) != MCO_SUCCESS)
		pWnd->m_state = BUILD_STATUS_ERROR;
	else
	{
		*(pWnd->m_data) = pWnd->m_pProfile->_gammutSurfaceBlackH;
		pWnd->PostMessage(WM_PROFILEFINISH);
	}

	return 1;
}

//timer event
void CBuildProgress::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch( m_state) 
	{
		case BUILD_STATUS_INIT:
		{
			//m_strMessage.Format("Building Profile: %s", m_pProfile->m_szProfileName);
			if(m_type == Process_BuildingTable)
				m_strMessage.Format("Processing data.");
			else
				m_strMessage.Format("Calculating Gamut.");
			m_ctlProgress.SetRange(0, m_range);
			m_ctlProgress.SetPos(0);
			break;
		}
		case BUILD_STATUS_ERROR:
		{
			m_strMessage.Format("Unable to process data.");
			break;
		}
		case BUILD_STATUS_FINISHED:
		{
			m_strMessage.Format("processing successful.");
			m_ctlProgress.SetPos(m_scale);
			break;
		}
		case BUILD_STATUS_UPDATE:
		{
			m_ctlProgress.SetPos(m_scale);
			break;
		}
	}

	UpdateData(FALSE);

	//default timer event	
	CDialog::OnTimer(nIDEvent);
}

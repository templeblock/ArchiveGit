// LumDemoW32DLLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LumDemoW32DLL.h"
#include "LumDemoW32DLLDlg.h"
#include "lumkit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
// Globals
//
unsigned short	gCalState, gUnits;
char			lptmpstr[1024];

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLumDemoW32DLLDlg dialog

CLumDemoW32DLLDlg::CLumDemoW32DLLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLumDemoW32DLLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLumDemoW32DLLDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLumDemoW32DLLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLumDemoW32DLLDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLumDemoW32DLLDlg, CDialog)
	//{{AFX_MSG_MAP(CLumDemoW32DLLDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN_POD, OnOpenPod)
	ON_BN_CLICKED(IDC_MEAS_FREQ, OnMeasFreq)
	ON_BN_CLICKED(IDC_UNITS, OnUnits)
	ON_BN_CLICKED(IDC_MEASURE, OnMeasure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLumDemoW32DLLDlg message handlers

BOOL CLumDemoW32DLLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CheckRadioButton(IDC_COM1, IDC_COM2, IDC_COM1);
	gCalState=0;
	gUnits = FOOT_LAMBERTS;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLumDemoW32DLLDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLumDemoW32DLLDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLumDemoW32DLLDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLumDemoW32DLLDlg::OnOpenPod() 
{
	char	sztemp[64];
	long	err;
	double	dIntTime;
	HCURSOR	hSaveCursor;

	hSaveCursor=SetCursor(LoadCursor(NULL,IDC_WAIT));
	err=sipOpenCalibrator((IsDlgButtonChecked(IDC_COM1) ? 1 : 2), sztemp, NULL);
	SetCursor(hSaveCursor);
	if(err!=SUCCESS)
	{
		sprintf(lptmpstr, "Error %ld\nCould not find calibrator",err);
		MessageBox(lptmpstr, (LPSTR)"Initializing Error", MB_OK | MB_ICONSTOP);
		gCalState=0;
	}
	else
	{
		SetDlgItemText(IDC_CAL_VERSION, sztemp);
		if((err=sipGetIntegrationTime(&dIntTime)) != SUCCESS)
		{
			sprintf(lptmpstr, "Comm Error %ld",err);
			MessageBox(lptmpstr, (LPSTR)"ERROR", MB_OK | MB_ICONSTOP);
		}

		sprintf(sztemp, "%6.3lf", dIntTime);
		SetDlgItemText(IDC_INTTIME, sztemp);
		SetDlgItemText(IDC_NUM_FIELDS, "100");
		gCalState=1;
	}

}

void CLumDemoW32DLLDlg::OnOK() 
{
	sipCloseCalibrator();
	
	CDialog::OnOK();
}

void CLumDemoW32DLLDlg::OnMeasFreq() 
{
	double	dMonFreq, dIntTime;
	long		err;
	short		sNumFields;

	if(!gCalState)
	{
		MessageBeep((UINT)-1);
		return;
	}
	GetDlgItemText(IDC_NUM_FIELDS, lptmpstr, 10);
	sNumFields = atoi(lptmpstr);
	err=sipMeasMonFreqAndSetIntTime(sNumFields, &dMonFreq);
	if(err == ERR_FF_COULD_NOT_SYNC)
	{
		MessageBox((LPSTR)"Could not sync to monitor.", (LPSTR)"ERROR", MB_OK);
	}
	else if(err != SUCCESS)
	{
		sprintf(lptmpstr, "Comm Error %ld",err);
		MessageBox(lptmpstr, (LPSTR)"ERROR", MB_OK | MB_ICONSTOP);
	}

	sprintf(lptmpstr, "%6.2f", dMonFreq);
	SetDlgItemText(IDC_MON_FREQ, lptmpstr);
	if((err=sipGetIntegrationTime(&dIntTime)) != SUCCESS)
	{
		sprintf(lptmpstr, "Comm Error %ld",err);
		MessageBox(lptmpstr, (LPSTR)"ERROR", MB_OK | MB_ICONSTOP);
	}
	sprintf(lptmpstr, "%6.3f", dIntTime);
	SetDlgItemText(IDC_INTTIME, lptmpstr);

}

void CLumDemoW32DLLDlg::OnUnits() 
{
	if(gUnits == FOOT_LAMBERTS)
	{
		SetDlgItemText(IDC_UNITS, "Nits");
		gUnits = CANDELAS;
	}
	else
	{
		SetDlgItemText(IDC_UNITS, "Ft-Lts");
		gUnits = FOOT_LAMBERTS;
	}
}

void CLumDemoW32DLLDlg::OnMeasure() 
{
	double	dMeas;
	HCURSOR	hSaveCursor;
	long		err;

	if(!gCalState)
	{
		MessageBeep((UINT)-1);
		return;
	}

	hSaveCursor=SetCursor(LoadCursor(NULL,IDC_WAIT));
	if((err=sipMeasureLuminance(&dMeas, gUnits)) != SUCCESS)
	{
		sprintf(lptmpstr, "Comm Error %ld",err);
		MessageBox(lptmpstr, (LPSTR)"ERROR", MB_OK | MB_ICONSTOP);
	}
	SetCursor(hSaveCursor);
	sprintf(lptmpstr, "%6.2f", dMeas);
	SetDlgItemText(IDC_MEAS_DISPLAY, lptmpstr);
}

#include "stdafx.h"
#include "CTPUtil.h"
#include "MainFrm.h"
#include "scappint.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
//

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString m_csVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_csVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_VERSION, m_csVersion);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCTPUtilApp

BEGIN_MESSAGE_MAP(CCTPUtilApp, CWinApp)
	//{{AFX_MSG_MAP(CCTPUtilApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCTPUtilApp construction
//
CCTPUtilApp::CCTPUtilApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCTPUtilApp object
//
CCTPUtilApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CCTPUtilApp initialization
//
BOOL CCTPUtilApp::InitInstance()
{
	SCENG_Init();

	m_pMainWnd = new CMainFrame();
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCTPUtilApp commands

/////////////////////////////////////////////////////////////////////////////
BOOL CAboutDlg::OnInitDialog() 
{
	char	szAppPath[MAX_PATH];
	DWORD	dwVerInfoSize;
	DWORD	dwVerHnd;
	UINT	uVersionLen;
	LPSTR	lpVersion;
	CString csVersion;

	csVersion.Empty();

	::GetModuleFileName(AfxGetInstanceHandle(), szAppPath, sizeof(szAppPath));
	dwVerInfoSize = ::GetFileVersionInfoSize(szAppPath, &dwVerHnd);
	if (dwVerInfoSize)
	{
		LPSTR lpVffInfo = (LPSTR)malloc((int)dwVerInfoSize);
		if (lpVffInfo)
		{
			::GetFileVersionInfo(szAppPath, dwVerHnd, dwVerInfoSize, lpVffInfo);
			::VerQueryValue(lpVffInfo, "\\StringFileInfo\\040904B0\\ProductVersion", (LPVOID*)&lpVersion, &uVersionLen);

			if (uVersionLen)
				csVersion = lpVersion;
			free(lpVffInfo);
		}
	}

	UpdateData(TRUE);
	m_csVersion += csVersion;

	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
int CCTPUtilApp::ExitInstance() 
{
	SCENG_Fini();
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// App command to run the dialog
void CCTPUtilApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Uninsdlg.cpp : implementation file
//

#include "stdafx.h"
#include "Uninstal.h"
#include "Uninsdlg.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUninstallDlg dialog

CUninstallDlg::CUninstallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUninstallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUninstallDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUninstallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUninstallDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUninstallDlg, CDialog)
	//{{AFX_MSG_MAP(CUninstallDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUninstallDlg message handlers

BOOL CUninstallDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CenterWindow();
	GetConfiguration()->ReplaceDialogText(this);
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUninstallDlg::OnPaint() 
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
HCURSOR CUninstallDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CUninstallDlg::OnContinue() 
{
/*
// Hide us!
*/ 

	ShowWindow(SW_HIDE);

/*
// The user has requested that we uninstall.
// Do a double confirmation.
*/
	CString csMessage;
	csMessage.LoadString(IDS_CONFIRM_UNINSTALL);
	GetConfiguration()->ReplaceText(csMessage);
	CString csTitle;
	csTitle.LoadString(IDS_MB_TITLE);
	GetConfiguration()->ReplaceText(csTitle);
	
	if (UninstMessageBox(IDS_CONFIRM_UNINSTALL, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
	{
		((CUninstallApp*)AfxGetApp())->Uninstall();
	}

	EndDialog(IDOK);
}

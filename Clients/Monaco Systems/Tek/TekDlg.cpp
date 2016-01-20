// TekDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tek.h"
#include "TekDlg.h"
#include "EditImageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTekDlg dialog

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTekDlg, CDialog)
	//{{AFX_MSG_MAP(CTekDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnEditProfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CTekDlg::CTekDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTekDlg::IDD, pParent)
{
	m_pEditProfileDlg = NULL;
	//{{AFX_DATA_INIT(CTekDlg)
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/////////////////////////////////////////////////////////////////////////////
CTekDlg::~CTekDlg()
{
	if (m_pEditProfileDlg)
		delete m_pEditProfileDlg;
}

/////////////////////////////////////////////////////////////////////////////
void CTekDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTekDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTekDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CTekDlg::OnPaint() 
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

/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CTekDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
void CTekDlg::OnEditProfile() 
{
	if (m_pEditProfileDlg && ::IsWindow(m_pEditProfileDlg->m_hWnd))
	{
		if (m_pEditProfileDlg->IsWindowVisible())
			m_pEditProfileDlg->ShowWindow(SW_HIDE);
		else
		{
			m_pEditProfileDlg->SetActiveWindow();
			m_pEditProfileDlg->ShowWindow(SW_NORMAL);
		}
		return;
	}

	CString szImageFile;
	szImageFile = _T("c:\\program files\\imagn\\pics\\ellen.pic");
	m_pEditProfileDlg = new CEditImageDlg(szImageFile, this);
	if (!m_pEditProfileDlg)
		return;

	if (!m_pEditProfileDlg->Create(CEditImageDlg::IDD, this/*pParent*/))
		AfxMessageBox("Can't create the dialog");
}

// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MonacoScan.h"
#include "MainDlg.h"
#include "Crop.h"
#include "Target.h"
#include "Dibapi.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

/////////////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
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
// CMainDlg dialog
CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainDlg)
	m_szReferenceFile = _T("");
	m_szImageFile = _T("");
	m_ptCornerUR = -1;
	m_ptCornerUL = -1;
	m_ptCornerLR = -1;
	m_ptCornerLL = -1;
	m_ptOutputUR = -1;
	m_ptOutputUL = -1;
	m_ptOutputLR = -1;
	m_ptOutputLL = -1;
	m_hDib = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bImageDone = FALSE;
	m_bReferenceDone = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

//	AfxGetApp()->WriteProfileInt( "Settings", "KeyValue", 123 );
//	AfxGetApp()->WriteProfileString( "Settings", "KeyString", "123" );
//	int i = AfxGetApp()->GetProfileInt( "Settings", "KeyValue", 999/*nDefault*/ );
//	CString sz;
//	sz = AfxGetApp()->GetProfileString( "Settings", "KeyString", "Default" );
}

/////////////////////////////////////////////////////////////////////////////
CMainDlg::~CMainDlg() // standard destructor
{
	// Free any previously allocated DIB
	if (m_hDib)
	{
		GlobalFree(m_hDib);
		m_hDib = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainDlg)
	DDX_CBString(pDX, IDC_REFERENCE_FILE, m_szReferenceFile);
	DDX_CBString(pDX, IDC_IMAGE_FILE, m_szImageFile);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	//{{AFX_MSG_MAP(CMainDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_REFERENCE, OnReference)
	ON_BN_CLICKED(IDC_IMAGE, OnImage)
	ON_BN_CLICKED(IDC_CROP, OnCrop)
	ON_BN_CLICKED(IDC_CALIBRATE, OnCalibrate)
	ON_BN_CLICKED(IDOK, OnProfileSave)
	ON_CBN_SELCHANGE(IDC_REFERENCE_FILE, OnReferenceFileSelected)
	ON_CBN_SELCHANGE(IDC_IMAGE_FILE, OnImageFileSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CMainDlg::OnInitDialog()
{
	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this, 5000);

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
	CComboBox* pCombo;
	if (pCombo = (CComboBox*)GetDlgItem(IDC_REFERENCE_FILE))
		pCombo->SetCurSel(0);

	if (pCombo = (CComboBox*)GetDlgItem(IDC_IMAGE_FILE))
		pCombo->SetCurSel(0);

	EnableControl(IDC_CROP, FALSE);
	SetControlCompletion(IDC_CROP_COMPLETE, FALSE);

	EnableControl(IDC_CALIBRATE, FALSE);
	SetControlCompletion(IDC_CALIBRATE_COMPLETE, FALSE);

	EnableControl(IDOK, FALSE);
	SetDone(IDCANCEL, FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		// CG: The following line was added by the Splash Screen component.
		CSplashWnd::ShowSplashScreen(this);

		// Use the splash screen instead of the about dialog
		//CAboutDlg dlgAbout;
		//dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CMainDlg::OnPaint() 
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
//  the minimized window.
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


/////////////////////////////////////////////////////////////////////////////
void CMainDlg::EnableControl(int id, BOOL bEnable)
{
	CWnd* pControl = GetDlgItem(id);
	if (!pControl)
		return;
	if (bEnable)
		pControl->ModifyStyle(WS_DISABLED/*dwRemove*/, NULL/*dwAdd*/, NULL);
	else
		pControl->ModifyStyle(NULL/*dwRemove*/, WS_DISABLED/*dwAdd*/, NULL);
	pControl->Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::SetControlCompletion(int id, BOOL bComplete, CString* pszExtraText)
{
	CWnd* pControl = GetDlgItem(id);
	if (!pControl)
		return;

	CString szText;
	szText = (bComplete ? "Completed" : "Not completed");

	if (pszExtraText)
	{
		szText += "            ";
		szText += *pszExtraText;
	}

	pControl->SetWindowText(szText);
	pControl->Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::SetDone(int id, BOOL bDone)
{
	CWnd* pControl = GetDlgItem(id);
	if (!pControl)
		return;

	CString szText;
	szText = (bDone ? "Done" : "Quit");

	pControl->SetWindowText(szText);
	pControl->Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnReferenceFileSelected() 
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_REFERENCE_FILE);
	if (!pCombo)
		return;

	int i = pCombo->GetCurSel();
	if (i == 0) // The default filename; has no effect
	{
		pCombo->SetCurSel(0);
		return;
	}

	if (i == 1) // A separator; has no effect
	{
		pCombo->SetCurSel(0);
		return;
	}

	int nLast = pCombo->GetCount() - 1;

	if (i < nLast) // A filename from the list
	{
		CString szFile;
		pCombo->GetLBText(0, szFile);
		GetReference(&szFile);
	}
	else
	if (i == nLast) // The open dialog is the last item in the list
		GetReference(NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnImageFileSelected() 
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_IMAGE_FILE);
	if (!pCombo)
		return;

	int i = pCombo->GetCurSel();
	if (i == 0) // The default filename; has no effect
	{
		pCombo->SetCurSel(0);
		return;
	}

	if (i == 1) // A separator; has no effect
	{
		pCombo->SetCurSel(0);
		return;
	}

	int nLast = pCombo->GetCount() - 1;

	if (i < nLast) // A filename from the list
	{
		CString szFile;
		pCombo->GetLBText(0, szFile);
		GetImage(&szFile);
	}
	else
	if (i == nLast) // The open dialog is the last item in the list
		GetImage(NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnReference()
{
	GetReference(NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnImage()
{
	GetImage(NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::GetReference(CString* pszFileName)
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_REFERENCE_FILE);
	if (!pCombo)
		return;

	CString szFileName;

	if (pszFileName)
		szFileName = *pszFileName;
	else
	{
		if (!GetReferenceFileName(szFileName))
		{
			pCombo->SetCurSel(0);
			return;
		}
	}

	// Validate that file can be read before dropping through
	if (!m_Target.ReadTargetDataFile(szFileName))
	{
		MessageBox("Bad reference data file", AfxGetApp()->m_pszAppName);
		pCombo->SetCurSel(0);
		return;
	}

	m_szReferenceFile = szFileName;

	// Strip off the path
	int i;
	while ((i = szFileName.FindOneOf( ":\\" )) >= 0)
		szFileName = szFileName.Mid(i+1);

	pCombo->DeleteString(0);
	pCombo->InsertString(0, szFileName);
	pCombo->SetCurSel(0);

	m_bReferenceDone = TRUE;

	EnableControl(IDC_CALIBRATE, m_bImageDone && m_bReferenceDone);
	SetControlCompletion(IDC_CALIBRATE_COMPLETE, FALSE);

	EnableControl(IDOK, FALSE);
	SetDone(IDCANCEL, FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::GetImage(CString* pszFileName)
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_IMAGE_FILE);
	if (!pCombo)
		return;

	CString szFileName;

	if (pszFileName)
		szFileName = *pszFileName;
	else
	{
		if (!GetImageFileName(szFileName))
		{
			pCombo->SetCurSel(0);
			return;
		}
	}

	m_szImageFile = szFileName;

	m_ptCornerUR = -1;
	m_ptCornerUL = -1;
	m_ptCornerLR = -1;
	m_ptCornerLL = -1;
	m_ptOutputUR = -1;
	m_ptOutputUL = -1;
	m_ptOutputLR = -1;
	m_ptOutputLL = -1;

	// Strip off the path
	int i;
	while ((i = szFileName.FindOneOf( ":\\" )) >= 0)
		szFileName = szFileName.Mid(i+1);

	pCombo->DeleteString(0);
	pCombo->InsertString(0, szFileName);
	pCombo->SetCurSel(0);

	m_bImageDone = FALSE;

	EnableControl(IDC_CROP, TRUE);
	SetControlCompletion(IDC_CROP_COMPLETE, FALSE);

	EnableControl(IDC_CALIBRATE, FALSE);
	SetControlCompletion(IDC_CALIBRATE_COMPLETE, FALSE);

	EnableControl(IDOK, FALSE);
	SetDone(IDCANCEL, FALSE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainDlg::GetReferenceFileName( CString& szFileName )
{
	CFileDialog FileDialog(
		/*bOpenFileDialog*/	TRUE,
		/*lpszDefExt*/		"tif",
		/*lpszFileName*/	"",
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Reference Target Data Files (*.q60;*.txt)|*.q60;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	if (FileDialog.DoModal() != IDOK)
		return FALSE;

	szFileName = FileDialog.GetPathName();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainDlg::GetImageFileName( CString& szFileName )
{
	CFileDialog FileDialog(
		/*bOpenFileDialog*/	TRUE,
		/*lpszDefExt*/		"tif",
		/*lpszFileName*/	"",
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Image Files (*.tif;*.bmp)|*.tif;*.bmp|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	if (FileDialog.DoModal() != IDOK)
		return FALSE;

	szFileName = FileDialog.GetPathName();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnCrop() 
{
	CCrop Crop( m_szImageFile, this );
	Crop.m_ptCornerUR = m_ptCornerUR;
	Crop.m_ptCornerUL = m_ptCornerUL;
	Crop.m_ptCornerLR = m_ptCornerLR;
	Crop.m_ptCornerLL = m_ptCornerLL;
	Crop.m_hDib = m_hDib;

	int idReturn = Crop.DoModal();
	if (idReturn != IDOK)
		return;

	// Get the new DIB from the crop dialog
	if (!(m_hDib = Crop.m_hDib))
		return;

	// Get the crop points; the crop dialog does all error checking
	m_ptOutputUR = Crop.m_ptOutputUR;
	m_ptOutputUL = Crop.m_ptOutputUL;
	m_ptOutputLR = Crop.m_ptOutputLR;
	m_ptOutputLL = Crop.m_ptOutputLL;
	m_ptCornerUR = Crop.m_ptCornerUR;
	m_ptCornerUL = Crop.m_ptCornerUL;
	m_ptCornerLR = Crop.m_ptCornerLR;
	m_ptCornerLL = Crop.m_ptCornerLL;

	SetControlCompletion(IDC_CROP_COMPLETE, TRUE);

	m_bImageDone = TRUE;

	EnableControl(IDC_CALIBRATE, m_bImageDone && m_bReferenceDone);
	SetControlCompletion(IDC_CALIBRATE_COMPLETE, FALSE);

	EnableControl(IDOK, FALSE);
	SetDone(IDCANCEL, FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnCalibrate() 
{
	if (!m_Target.ReadTargetDataFile( m_szReferenceFile ))
		return;

	if (!m_hDib)
		return;

	if (!m_Target.ReadPatchData( m_hDib, m_ptOutputUL, m_ptOutputUR, m_ptOutputLL, m_ptOutputLR ))
		return;

	BeginWaitCursor();
	double fAvgDeltaE, fMaxDeltaE;
	BOOL bOK = m_Target.Calibrate( &fAvgDeltaE, &fMaxDeltaE );
	EndWaitCursor();
	if (!bOK)
		return;

	CString szDeltaE;
	szDeltaE.Format("(DeltaE is %.2f)", fAvgDeltaE);

	SetControlCompletion(IDC_CALIBRATE_COMPLETE, TRUE, &szDeltaE);

	EnableControl(IDOK, TRUE);
	SetDone(IDCANCEL, FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnProfileSave() 
{
	CFileDialog FileDialog(
		/*bOpenFileDialog*/	FALSE,
		/*lpszDefExt*/		"icm",
		/*lpszFileName*/	"",
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Color Profiles (*.icm)|*.icm|All Files (*.*)|*.*||",
		/*pParentWnd*/		this
		);

	if (FileDialog.DoModal() != IDOK)
		return;

	CString szProfileName;
	szProfileName = FileDialog.GetPathName();

	if (!m_Target.SaveProfile( szProfileName ))
		return;

	SetDone(IDCANCEL, TRUE);
	// Don't exit when we save a profile
	//CDialog::OnOK();
}

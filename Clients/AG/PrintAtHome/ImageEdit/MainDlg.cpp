#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainDlg, CResizeDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTOVER, OnStartOver)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_BN_CLICKED(IDC_REDO, OnRedo)
	ON_BN_CLICKED(IDC_ZOOMIN, OnZoomIn)
	ON_BN_CLICKED(IDC_ZOOMOUT, OnZoomOut)
	ON_BN_CLICKED(IDC_ZOOMFULL, OnZoomFull)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CResizeDlg(CMainDlg::IDD, pParent)
{
	CoInitialize(NULL);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	bInit = false;
}

/////////////////////////////////////////////////////////////////////////////
CMainDlg::~CMainDlg()
{
	CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::PreSubclassWindow()
{
	ModifyStyle(0/*dwRemove*/, WS_CLIPCHILDREN/*dwAdd*/);

	CResizeDlg::PreSubclassWindow();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainDlg::OnInitDialog()
{
	CResizeDlg::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, true);			// Set big icon
	SetIcon(m_hIcon, false);		// Set small icon


	HICON hIcon = AfxGetApp()->LoadIcon(IDI_ZOOMFULL);
	m_btnZoomFull.SetIcon(hIcon);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
bool CMainDlg::DoCommand(LPCTSTR strCommand, LPCTSTR strValue)
{
	try {
		if (m_ImageControl.m_hWnd)
			return !!m_ImageControl.DoCommand(strCommand, strValue);
	} catch(...) {}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
long CMainDlg::GetCommand(LPCTSTR strCommand)
{
	try {
		if (m_ImageControl.m_hWnd)
			return m_ImageControl.GetCommand(strCommand);
	} catch(...) {}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CImageControl& CMainDlg::GetImageControl()
{
	return m_ImageControl;
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizeDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabControl);
	DDX_Control(pDX, IDC_IMAGE_CONTROL, m_ImageControl);
	DDX_Control(pDX, IDC_ZOOMFULL, m_btnZoomFull);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnPaint()
{
	if (!bInit)
	{
		AddControl(IDC_IMAGE_CONTROL,	CST_RESIZE,	CST_RESIZE,	CST_RESIZE,	CST_RESIZE);
		AddControl(IDC_TAB_CONTROL,		CST_NONE,	CST_NONE,	CST_RESIZE,	CST_RESIZE);
		AddControl(IDC_STARTOVER,		CST_NONE,	CST_NONE,	CST_REPOS,	CST_REPOS);
		AddControl(IDC_UNDO,			CST_NONE,	CST_NONE,	CST_REPOS,	CST_REPOS);
		AddControl(IDC_REDO,			CST_NONE,	CST_NONE,	CST_REPOS,	CST_REPOS);
		AddControl(IDC_ZOOMIN,			CST_NONE,	CST_NONE,	CST_REPOS,	CST_REPOS);
		AddControl(IDC_ZOOMOUT,			CST_NONE,	CST_NONE,	CST_REPOS,	CST_REPOS);
		AddControl(IDC_ZOOMFULL,		CST_NONE,	CST_NONE,	CST_REPOS,	CST_REPOS);
		AddControl(IDC_APPLY,			CST_REPOS,	CST_REPOS,	CST_REPOS,	CST_REPOS);
		AddControl(IDOK,				CST_REPOS,	CST_REPOS,	CST_REPOS,	CST_REPOS);
		AddControl(IDCANCEL,			CST_REPOS,	CST_REPOS,	CST_REPOS,	CST_REPOS);
		bInit = true;
	}

	CResizeDlg::OnPaint();
}

/////////////////////////////////////////////////////////////////////////////
// The system calls this function to obtain the cursor to display while the user drags the minimized window.
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnStartOver()
{
	DoCommand(_T("StartOver"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnUndo()
{
	DoCommand(_T("Undo"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnRedo()
{
	DoCommand(_T("Redo"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnZoomIn()
{
	DoCommand(_T("ZoomIn"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnZoomOut()
{
	DoCommand(_T("ZoomOut"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnZoomFull()
{
	DoCommand(_T("ZoomFull"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CMainDlg::OnApply()
{
	DoCommand(_T("Apply"), _T(""));
}

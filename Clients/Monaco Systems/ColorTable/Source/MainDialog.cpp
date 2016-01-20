// MainDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ColorTable.h"
#include "MainDialog.h"
#include "AboutDlg.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainDialog dialog

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	//{{AFX_MSG_MAP(CMainDialog)
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_RESTORE, OnRestore)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#include <fStream.h>

/////////////////////////////////////////////////////////////////////////////
CMainDialog::CMainDialog(CWnd* pParent /*=NULL*/) : CDialog(CMainDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainDialog)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Read in the color table file
	ifstream file;
	file.open("c:\\ColorTable.txt", ios::in | ios::nocreate);
	bool bError = (file.bad() || !file.is_open());

	for (int i=0 ; i<256; i++)
	{
		if (bError)
			break;

		for (int j=0; j<3; j++)
		{ // Get 3 RGB components per line
			int iValue = -1;
			file >> iValue;
			if (iValue < 0)
			{
				bError = true;
				break;
			}

			m_ColorTable[i + (256*j)] = iValue;

			char cComma;
			file >> cComma;
			if (cComma != ',')
				file.putback(cComma);
			file.eatwhite();
		}
	}

	file.close();

	if (i != 256)
		bError = true;

	if (bError)
	{
		if (!i)
			Message("Bad color table file");
		else
			Message("Bad color table entry on line %d", i);

		// Initialize the color table with a ramp
		WORD wValue = 0;
		for (int i=0; i<256*3; i++)
		{
			if (i == 0 || i == 256 || i == 512)
				wValue = 0;
			m_ColorTable[i] = wValue;
			wValue += 256;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
CMainDialog::~CMainDialog()
{
}

/////////////////////////////////////////////////////////////////////////////
void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainDialog)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu)
	{
		pSysMenu->RemoveMenu(61488, MF_BYCOMMAND); // Maximize position 4
		pSysMenu->RemoveMenu(61440, MF_BYCOMMAND); // Size position 2
		pSysMenu->RemoveMenu(61456, MF_BYCOMMAND); // Move position 1

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
	
	// Do the real work
	bool bSupported = SetColorTable();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CMainDialog::OnDestroy() 
{
	bool bSupported = RestoreColorTable();
	CDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
void CMainDialog::OnClose() 
{
//	ShowWindow(SW_MINIMIZE);
	CDialog::OnClose();
}

/////////////////////////////////////////////////////////////////////////////
void CMainDialog::OnOK() 
{
	bool bSupported = RestoreColorTable();
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CMainDialog::OnCancel() 
{
	bool bSupported = RestoreColorTable();
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
void CMainDialog::OnSet() 
{
	bool bSupported = SetColorTable();
	ShowWindow(SW_MINIMIZE);
}

/////////////////////////////////////////////////////////////////////////////
void CMainDialog::OnRestore() 
{
	bool bSupported = RestoreColorTable();
	ShowWindow(SW_MINIMIZE);
}

/////////////////////////////////////////////////////////////////////////////
bool CMainDialog::SetColorTable(void)
{
	CDC* pDC = GetDC();
	if (!pDC)
		return false;

	BOOL bSupported = SetDeviceGammaRamp(pDC->GetSafeHdc(), m_ColorTable);

	ReleaseDC(pDC);

	return bSupported != 0;
}

/////////////////////////////////////////////////////////////////////////////
bool CMainDialog::RestoreColorTable(void)
{
	CDC* pDC = GetDC();
	if (!pDC)
		return false;

	WORD ColorTableNew[256*3];
	WORD wValue = 0;
	for (int i=0; i<256*3; i++)
	{
		if (i == 0 || i == 256 || i == 512)
			wValue = 0;
		ColorTableNew[i] = wValue;
		wValue += 256;
	}

	BOOL bSupported = SetDeviceGammaRamp(pDC->GetSafeHdc(), ColorTableNew);

	ReleaseDC(pDC);

	return bSupported != 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainDialog::OnSysCommand(UINT nID, LPARAM lParam)
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

/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CMainDialog::OnPaint() 
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
		return;
	}

	CDialog::OnPaint();
}


/////////////////////////////////////////////////////////////////////////////
void CMainDialog::OnNcPaint() 
{
	static bool bInit;
	if (!bInit)
	{
		bInit = true;
		ShowWindow(SW_MINIMIZE);
		return;
	}
	
	CDialog::OnNcPaint();
}

/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags the minimized window.
HCURSOR CMainDialog::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

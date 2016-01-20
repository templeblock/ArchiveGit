// FontsInUseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontsInUse.h"
#include "FontsInUseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
void CFontsInUseDlg::FontsInUse()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST);
	if (!pListBox)
		return;

	pListBox->ResetContent();

	char strPath[_MAX_PATH];
	::GetWindowsDirectory(strPath, sizeof(strPath));
	if (strPath[lstrlen(strPath) - 1] != '\\')
		lstrcat(strPath, "\\");

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS || (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion >= 4))
		lstrcat(strPath, "Fonts\\");
	else
		lstrcat(strPath, "System\\");

	CString strWildcard("*.ttf");

	WIN32_FIND_DATA FileInfo;
	HANDLE hFileFind = ::FindFirstFile((strPath + strWildcard), &FileInfo);
	if (hFileFind == INVALID_HANDLE_VALUE)
		return;

	while (1)
	{
		CString strFile = FileInfo.cFileName;
		CString strFileName = strPath + strFile;

		// See if we can open the file for write access
		HANDLE hf = ::CreateFile(strFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hf != INVALID_HANDLE_VALUE)
			::CloseHandle(hf);
		else
		{
			CString strItem;
			strItem.Format("%s", strFile);
			pListBox->AddString(strItem);
		}

		if (!::FindNextFile(hFileFind, &FileInfo))
			break;
	}

	::FindClose(hFileFind);
}

/////////////////////////////////////////////////////////////////////////////
// CFontsInUseDlg dialog

CFontsInUseDlg::CFontsInUseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontsInUseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFontsInUseDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFontsInUseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontsInUseDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFontsInUseDlg, CDialog)
	//{{AFX_MSG_MAP(CFontsInUseDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontsInUseDlg message handlers

BOOL CFontsInUseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CWnd* pListBox = GetDlgItem(IDC_LIST);
	if (pListBox)
	{
		CRect rect;
		GetClientRect(&rect);
		CRect rectListBox;
		pListBox->GetWindowRect(&rectListBox);
		m_dx = rect.Width() - rectListBox.Width();
		m_dy = rect.Height() - rectListBox.Height();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFontsInUseDlg::OnPaint() 
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

HCURSOR CFontsInUseDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFontsInUseDlg::OnOK() 
{
	FontsInUse();
}

void CFontsInUseDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	CWnd* pListBox = GetDlgItem(IDC_LIST);
	if (pListBox)
		pListBox->SetWindowPos(NULL, 0, 0, cx - m_dx, cy - m_dy, SWP_NOZORDER | SWP_NOMOVE);
}

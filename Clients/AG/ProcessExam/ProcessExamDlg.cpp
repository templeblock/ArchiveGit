#include "stdafx.h"
#include "ProcessExam.h"
#include "ProcessExamDlg.h"
#include "ProcessApi.h"
#include "ModuleVersion.h"

#define UWM_SHUTDOWNALL_MSG _T("UWM_SHUTDOWNALL_MSG-DE8B2FF0-BD06-4e5b-8325-52DEA2E6DE7E")
static const UINT UWM_SHUTDOWNALL	= ::RegisterWindowMessage(UWM_SHUTDOWNALL_MSG);

#ifdef FULL
	#define SHOW_HIDDEN_WINDOWS true
	#define SHOW_PROCESS_FILENAMES true
	#define SHOW_OPEN_MODULES true
	#define SHOW_OPEN_MODULE_DESCRIPTIONS true
	#define ALL_PROCESSES_MATCH true
#else
	#define SHOW_HIDDEN_WINDOWS false
	#define SHOW_PROCESS_FILENAMES false
	#define SHOW_OPEN_MODULES false
	#define SHOW_OPEN_MODULE_DESCRIPTIONS false
	#define ALL_PROCESSES_MATCH false
#endif FULL

#define REFRESH_TIME 2000L // 2 second

/////////////////////////////////////////////////////////////////////////////
LPCSTR FindList[] = {
	"Outlook.exe",
	"msimn.exe",
	"iexplore.exe"/*,
	"waol.exe"*/ // Temp, Until AOL supported - JHC
};

/////////////////////////////////////////////////////////////////////////////
CProcessExamDlg::CProcessExamDlg(CWnd* pParent)
	: CDialog(CProcessExamDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_idTimer = NULL;
	m_iTimerCount = 0;
	m_bCloseAllStarted = false;
}

/////////////////////////////////////////////////////////////////////////////
CProcessExamDlg::~CProcessExamDlg()
{
	if (m_idTimer)
	{
		KillTimer(m_idTimer);
		m_idTimer = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_EDIT1, m_Search);
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CProcessExamDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_REFRESH, OnRefreshClicked)
	ON_BN_CLICKED(IDC_CLOSEALL, OnCloseAllClicked)
	ON_BN_CLICKED(IDC_KILLALL, OnKillAllClicked)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDOK, OnCancel)
	ON_EN_CHANGE(IDC_EDIT1, OnSearchChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CProcessExamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, true);			// Set big icon
	SetIcon(m_hIcon, false);		// Set small icon

	CRect rect;
	GetClientRect(&rect);
	CRect rectListBox;
	m_ListBox.GetWindowRect(&rectListBox);
	m_dx = rect.Width() - rectListBox.Width();
	m_dy = rect.Height() - rectListBox.Height();

	OnRefreshClicked();
//j	m_idTimer = SetTimer(TA_NONE/**nIDEvent*/, REFRESH_TIME, NULL);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::OnSize(UINT nType, int cx, int cy) 
{
	CRect rectListBox;
	if (::IsWindow(m_ListBox.m_hWnd))
		m_ListBox.GetWindowRect(&rectListBox);

	CDialog::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_ListBox.m_hWnd))
		m_ListBox.SetWindowPos(NULL, 0, 0, cx - m_dx, rectListBox.Height(), SWP_NOZORDER | SWP_NOMOVE);
}

/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		if (m_bCloseAllStarted)
			RestoreWaitCursor();
	}
}

/////////////////////////////////////////////////////////////////////////////
// The system calls this function to obtain the cursor to display while the user drags the minimized window.
HCURSOR CProcessExamDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////////
void TerminateApp(DWORD dwPID, DWORD dwTimeout);
void GetProcessWindowText(DWORD dwPID, CString& strText);
bool CALLBACK CountOverlappedProcessWindows(HWND hWnd, LPARAM lParam);
bool CALLBACK HandleEachProcessWindow(HWND hWnd, LPARAM lParam);

static CString g_strWindowText;
static int g_nOverlapped;
static bool g_bClose;

/////////////////////////////////////////////////////////////////////////////
int CountOverlappedWindows(DWORD dwPID)
{
	g_nOverlapped = 0;
	::EnumWindows((WNDENUMPROC)CountOverlappedProcessWindows, (LPARAM)dwPID);
	return g_nOverlapped;
}

/////////////////////////////////////////////////////////////////////////////
void GetProcessWindowText(DWORD dwPID, CString& strText)
{
	int nOverlapped = CountOverlappedWindows(dwPID);
	g_strWindowText.Empty();
	g_bClose = false;
	::EnumWindows((WNDENUMPROC)HandleEachProcessWindow, (LPARAM)dwPID);
	if (!g_strWindowText.IsEmpty())
		strText = g_strWindowText;
}

/////////////////////////////////////////////////////////////////////////////
void TerminateApp(DWORD dwPID, int iTerminateCode, DWORD dwTimeout)
{
	if (iTerminateCode & TA_CLOSE)
	{
		// Post a WM_CLOSE to all windows whose PID matches the target process
		int nOverlapped = CountOverlappedWindows(dwPID);
		g_strWindowText.Empty();
		g_bClose = true;
		::EnumWindows((WNDENUMPROC)HandleEachProcessWindow, (LPARAM)dwPID);
	}

	if (iTerminateCode & TA_WAIT)
	{
		// If we can't open the process with PROCESS_TERMINATE rights, then we give up immediately
		HANDLE hProcess = ::OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, dwPID);
		if (hProcess)
		{
			// Wait on the process handle
			bool bSuccess = (::WaitForSingleObject(hProcess, dwTimeout) == WAIT_OBJECT_0);
			::CloseHandle(hProcess);
		}
	}

	if (iTerminateCode & TA_KILL)
	{
		// If we can't open the process with PROCESS_TERMINATE rights, then we give up immediately
		HANDLE hProcess = ::OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, dwPID);
		if (hProcess)
		{
			// Try to kill the process
			bool bSuccess = !!::TerminateProcess(hProcess, 0);
			::CloseHandle(hProcess);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CountOverlappedProcessWindows(HWND hWnd, LPARAM lParam)
{
	DWORD dwID = 0;
	::GetWindowThreadProcessId(hWnd, &dwID);
	if (dwID != (DWORD)lParam)
		return true;

	char szWindowText[MAX_PATH];
	::GetWindowText(hWnd, szWindowText, sizeof(szWindowText));
	DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
	bool bOverlapped = ((dwStyle & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW);
	if (bOverlapped)
		g_nOverlapped++;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK HandleEachProcessWindow(HWND hWnd, LPARAM lParam)
{
	DWORD dwID = 0;
	::GetWindowThreadProcessId(hWnd, &dwID);
	if (dwID != (DWORD)lParam)
		return true;

	char szWindowText[MAX_PATH];
	::GetWindowText(hWnd, szWindowText, sizeof(szWindowText));
	DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
	bool bOverlapped = ((dwStyle & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW);
	bool bCaption = ((dwStyle & WS_CAPTION) == WS_CAPTION);
	bool bVisible = !!(dwStyle & WS_VISIBLE);
	bool bHasParent = !!::GetParent(hWnd);
	bool bHasTitle = !CString(szWindowText).IsEmpty();
	bool bMatch = bOverlapped || (!g_nOverlapped && !bHasParent && bHasTitle);
	if (bMatch)
	{
		if (g_bClose)
		{
			if (::IsWindow(hWnd))
				::PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		else
		if (SHOW_HIDDEN_WINDOWS || bVisible)
		{
			g_strWindowText += szWindowText;
			g_strWindowText += "\n";
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CProcessExamDlg::IsMatch(LPCSTR strFind)
{
	CString strFindLowerCase = strFind;
	strFindLowerCase.MakeLower();

	CString strSearch = m_strSearch;
	if (ALL_PROCESSES_MATCH && strSearch.IsEmpty())
		strSearch = ".";

	if (!strSearch.IsEmpty())
	{
		strSearch.MakeLower();
		return (strFindLowerCase.Find(strSearch) >= 0);
	}

	// Empty search string - use the FileList
	int iCount = sizeof(FindList)/sizeof(LPCSTR);

	for (int i=0; i< iCount; i++)
	{
		CString strSearch = FindList[i];
		strSearch.MakeLower();
		if (strFindLowerCase.Find(strSearch) >= 0)
			return true;
	}
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////
int CProcessExamDlg::ProcessWalk(int iTerminateCode, bool bSilent)
{
	CProcessApi papi;
	if (!papi.Init(false))
	{
#ifdef _DEBUG
		MessageBox("Failed to load either of process api libraries!");
#endif _DEBUG
		return 0;
	}

	if (!bSilent)
	{
		m_ListBox.ResetContent();
		RestoreWaitCursor();
	}

	DWORD pl = papi.ProcessesGetList();
	if (!pl)
	{
#ifdef _DEBUG
		MessageBox("Failed to get the list of processes!");
#endif _DEBUG
		return 0;
	}
	
	int nMatches = 0;
	CString strMsg;
	CProcessApi::tProcessInfo pi;
	while (papi.ProcessesWalk(pl, &pi))
	{
		CString strProcessDesc;
		CModuleVersion ver;
		if (ver.GetFileVersionInfo(pi.FileName))
			strProcessDesc = ver.GetValue(_T("FileDescription"));
		bool bProcessMatch = (ALL_PROCESSES_MATCH && IsMatch(pi.FileName));
		bool bProcessDisplayed = false;

		CProcessApi::tModuleInfo mi = {0};
		DWORD ml = papi.ModulesGetList(pi.pid);

		while (papi.ModulesWalk(ml, &mi))
		{
			bool bModuleMatch = IsMatch(mi.FileName);
			if (!bProcessMatch && !bModuleMatch)
				continue;
			
			nMatches++;
			if (!bProcessDisplayed)
			{
				bProcessDisplayed = true;
				if (iTerminateCode & (TA_CLOSE|TA_KILL))
					TerminateApp(pi.pid, iTerminateCode, 5000/*dwTimeout*/);
				else
				if (!bSilent)
				{
					if (strProcessDesc.IsEmpty())
					{ // See if the pi filename is contained if the mi filename - if so, use it
						CString strMiFileName = mi.FileName;
						if (strMiFileName.Find(pi.FileName) >= 0)
						{
							if (ver.GetFileVersionInfo(mi.FileName))
								strProcessDesc = ver.GetValue(_T("FileDescription"));
						}
					}
					if (strProcessDesc.IsEmpty())
						strProcessDesc = pi.FileName;
					if ((SHOW_PROCESS_FILENAMES || !m_strSearch.IsEmpty()))
						strMsg.Format("%s (%s)", strProcessDesc, pi.FileName);
					else
						strMsg.Format("%s", strProcessDesc);

					m_ListBox.AddString(strMsg);

					CString strWindowText;
					GetProcessWindowText(pi.pid, strWindowText);
					int index1 = 0;
					int index2 = 0;
					while ((index2 = strWindowText.Find("\n", index1)) >= 0)
					{
						strMsg.Format("        %s", strWindowText.Mid(index1, index2-index1));
						m_ListBox.AddString(strMsg);
						index1 = index2 + 1;
					}
				}
			}

			if ((SHOW_OPEN_MODULES || !m_strSearch.IsEmpty()) && !bSilent)
			{
				if (SHOW_OPEN_MODULE_DESCRIPTIONS)
				{
					CString strModuleDesc;
					CModuleVersion ver;
					if (ver.GetFileVersionInfo(mi.FileName))
						strModuleDesc = ver.GetValue(_T("FileDescription"));
					strMsg.Format("                %s (%s)", strModuleDesc, mi.FileName);
				}
				else
					strMsg.Format("                %s", mi.FileName);

				m_ListBox.AddString(strMsg);
			}
		}

		papi.ModulesFreeList(ml);
	}

	papi.ProcessesFreeList(pl);

	if (::IsWindow(m_ListBox.m_hWnd))
	{
		// Select the listbox font, save the old font
		CDC& dc = *m_ListBox.GetDC();
		CFont& Font = *m_ListBox.GetFont();
		CFont* pOldFont = dc.SelectObject(&Font);

		// Get the text metrics for avg char width
		TEXTMETRIC tm;
		dc.GetTextMetrics(&tm); 

		int dx = 0;
		for (int i = 0; i < m_ListBox.GetCount(); i++)
		{
			CString str;
			m_ListBox.GetText(i, str);
			CSize size = dc.GetTextExtent(str);

			// Add the avg width to prevent clipping
			size.cx += tm.tmAveCharWidth;

			if (size.cx > dx)
				dx = size.cx;
		}

		dc.SelectObject(pOldFont);
		m_ListBox.ReleaseDC(&dc);
		m_ListBox.SetHorizontalExtent(dx);
	}

	return nMatches;
}
/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::ShutdownAll()
{
	RestoreWaitCursor();
	::PostMessage(HWND_BROADCAST, UWM_SHUTDOWNALL, 0, 0);		
}
/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::OnTimer(UINT_PTR nIDEvent)
{
	RestoreWaitCursor();

	if (nIDEvent == TA_NONE)
	{
		OnRefreshClicked();
		return;
	}
	
	int iTerminateCode = nIDEvent;
	//if (++m_iTimerCount > 5)
	//{
	//j	This causes a problem if the user is being prompted to save something
	//j	iTerminateCode = nIDEvent/*TA_CLOSE or TA_KILL*/;
	//	m_iTimerCount = 0;
	//}
	int nMatches = ProcessWalk(iTerminateCode, false/*bSilent*/);
	if (nMatches)
		return;

	if (m_idTimer)
	{
		KillTimer(m_idTimer);
		m_idTimer = NULL;
		EndWaitCursor(); 
	}

	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::OnRefreshClicked()
{
	ProcessWalk(TA_NONE, false/*bSilent*/);
}

/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::OnCloseAllClicked()
{	
	CString szMsg;
	szMsg.LoadString(IDS_WARN_TERMINATE);
	if (::MessageBox(m_hWnd, szMsg, "Creata Mail Setup", MB_ICONWARNING | MB_YESNO) != IDYES)
		return;
	
	m_bCloseAllStarted = true;
	BeginWaitCursor();


	// Allow Jazzymail components to cleanup.
	ShutdownAll();

	if (m_idTimer)
	{
		KillTimer(m_idTimer);
		m_idTimer = NULL;
	}

	//int nMatches = ProcessWalk(TA_KILL, true/*bSilent*/);
	//if (nMatches)
	{
		RestoreWaitCursor();	
		m_idTimer = SetTimer(TA_KILL/**nIDEvent*/, REFRESH_TIME, NULL);
		m_iTimerCount = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::OnKillAllClicked()
{
	if (m_idTimer)
	{
		KillTimer(m_idTimer);
		m_idTimer = NULL;
	}

	int nMatches = ProcessWalk(TA_KILL/*|TA_CLOSE|TA_WAIT*/, true/*bSilent*/);
	if (nMatches)
	{
		m_idTimer = SetTimer(TA_KILL/**nIDEvent*/, REFRESH_TIME, NULL);
		m_iTimerCount = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::OnSearchChanged()
{
	m_Search.GetWindowText(m_strSearch);
}

/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::OnOK()
{
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CProcessExamDlg::OnCancel()
{
	CDialog::OnCancel();
}

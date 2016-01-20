// PrintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrintDLL.h"
#include "PrintDlg.h"
#include <dde.h>
#include "util.h"
#include <regstr.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UM_CONNECT_COMPLETE WM_USER + 1
#define UM_PRINT_COMPLETE WM_USER + 2
#define UM_START_PRINT WM_USER + 3

HWND FindMainWindow(void);

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg dialog


CPrintDlg::CPrintDlg(CString csFilePath, CString csProductCode, CWnd* pParent /*=NULL*/)
	: CDialog(CPrintDlg::IDD, pParent),
	m_hPrintAppWnd(NULL),
	m_bConnectionMade(FALSE),
	m_nLastDDEMessage(0),
	m_csFilePath(csFilePath),
	m_csProductCode(csProductCode),
	m_hStartupEvent(NULL),
	m_hGlobal(NULL)
{
	//{{AFX_DATA_INIT(CPrintDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPrintDlg::~CPrintDlg()
{
	GlobalFree(m_hGlobal);
	if(m_hStartupEvent)
		CloseHandle(m_hStartupEvent);
}

void CPrintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintDlg)
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintDlg)
	ON_MESSAGE(WM_DDE_ACK, OnDDEAck)
	ON_MESSAGE(WM_DDE_TERMINATE, OnDDETerminate)
	ON_COMMAND(UM_CONNECT_COMPLETE, OnConnectComplete)
	ON_COMMAND(UM_PRINT_COMPLETE, OnPrintComplete)
	ON_COMMAND(UM_START_PRINT, OnStartPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg message handlers

BOOL CPrintDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char szIniEntry[MAX_PATH];
	GetModuleFileName(NULL, szIniEntry, MAX_PATH);

	CString csIniFile(szIniEntry);
	csIniFile = csIniFile.Left(csIniFile.ReverseFind('\\') + 1) + "PrintInfo.ini";
   GetPrivateProfileString("Configuration", "ExeName", "agpr.exe", szIniEntry, MAX_PATH, csIniFile);
	m_csExeName = szIniEntry;

   CString csKey = REGSTR_PATH_APPPATHS ;
	csKey += "\\" + m_csExeName;

    GetRegistryString(
        HKEY_LOCAL_MACHINE,
        csKey,
        NULL,
        m_csAppPath);

	//write the product code to the ini file

	CString csIniPath = m_csAppPath.Left(m_csAppPath.ReverseFind('\\') + 1) +="AgRemote.ini";

	if(WritePrivateProfileString("Bar Code", "bc_text", m_csProductCode, csIniPath))
	{
		PostMessage(WM_COMMAND, UM_START_PRINT);
	}
	else
	{
		EndDialog(ERR_PRODUCT_CODE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintDlg::OnStartPrint()
{
	m_cMessage.SetWindowText("Initializing connection with print app");
	UpdateWindow();
	DdeInit();
}

void CPrintDlg::DdeInit() 
{
	if(m_hStartupEvent == NULL)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		m_hStartupEvent = CreateEvent(&sa, FALSE, FALSE, "AG_DDE_Event");
	}
	if(!m_bConnectionMade)
	{
		m_hPrintAppWnd = FindMainWindow();
		if(m_hPrintAppWnd == NULL)
			StartProcess();

		
		CString csAppAtom = m_csExeName.SpanExcluding(".");
		csAppAtom.MakeUpper();
		ATOM aApp = GlobalAddAtom(csAppAtom);
		ATOM aTopic = GlobalAddAtom("System");
		m_nLastDDEMessage = WM_DDE_INITIATE;

		::SendMessage(HWND_BROADCAST, WM_DDE_INITIATE, (WPARAM)m_hWnd, MAKELPARAM(aApp, aTopic));
		GlobalDeleteAtom(aApp);
		GlobalDeleteAtom(aTopic);
	}

} 

void CPrintDlg::StartProcess() 
{	
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if(CreateProcess(m_csAppPath, 
		"",
		NULL,
		NULL,
		TRUE,
		NORMAL_PRIORITY_CLASS, 
		NULL,
		NULL,
		&si,
		&m_pi))
	{
		m_cMessage.SetWindowText("Starting Print App");
		UpdateWindow();
		WaitForSingleObject(m_hStartupEvent, 10000);
	}
}

void CPrintDlg::DoPrintJob() 
{
	if(::IsWindow(m_hPrintAppWnd))
	{
		if(m_hGlobal == NULL)
		{
			CString csCommand;
			csCommand.Format("%s%s%s", "[print(\"", m_csFilePath,"\")]");

			m_hGlobal = GlobalAlloc(GMEM_DDESHARE, csCommand.GetLength() + 1);
			char* pData = (char*)GlobalLock(m_hGlobal);
			strcpy(pData, csCommand);
			GlobalUnlock(m_hGlobal);
		}

		CString csMessage;
		csMessage.Format("Printing %s", m_csFilePath);
		m_cMessage.SetWindowText(csMessage);
		UpdateWindow();

		m_nLastDDEMessage = WM_DDE_EXECUTE;
		::PostMessage(m_hPrintAppWnd , WM_DDE_EXECUTE, (WPARAM)m_hWnd, (LPARAM)m_hGlobal);
	
	}
	else
	{
		m_bConnectionMade = FALSE;
	}
	
}


void CPrintDlg::OnConnectComplete()
{
	if(m_hPrintAppWnd == NULL)
	{
		EndDialog(ERR_APP_STARTUP);
		return;
	}

	m_cMessage.SetWindowText("Connection Made");
	UpdateWindow();
	DoPrintJob();
}

void CPrintDlg::OnPrintComplete()
{
	char szIniEntry[MAX_PATH];
	GetModuleFileName(NULL, szIniEntry, MAX_PATH);

	//get timeout value from ini file in minutes
	CString csIniFile(szIniEntry);
	csIniFile = csIniFile.Left(csIniFile.ReverseFind('\\') + 1) + "PrintInfo.ini";
   DWORD nTimeout = GetPrivateProfileInt("Configuration", "Timeout", 5, csIniFile);
	//convert timeout to milliseconds
	nTimeout = nTimeout*60*1000;
	if(WaitForSingleObject(m_hStartupEvent, 300000) == WAIT_TIMEOUT)
	{
		AfxMessageBox("Timed out");
		EndDialog(ERR_TIME_OUT);
	}
	if(::IsWindow(m_hPrintAppWnd))
	{
		m_cMessage.SetWindowText("Printing complete");
		UpdateWindow();
		::PostMessage(m_hPrintAppWnd, WM_DDE_TERMINATE, (WPARAM)m_hWnd, 0);
	}

}

LRESULT CPrintDlg::OnDDEAck(WPARAM wParam, LPARAM lParam)
{
	switch(m_nLastDDEMessage)
	{
		case WM_DDE_INITIATE:
		{
			ATOM aApp = LOWORD(lParam);;
			ATOM aTopic = HIWORD(lParam);
			GlobalDeleteAtom(aApp);
			GlobalDeleteAtom(aTopic);
			m_hPrintAppWnd = (HWND)wParam;
			m_bConnectionMade = TRUE;
			if(m_hPrintAppWnd != NULL)
				PostMessage(WM_COMMAND, UM_CONNECT_COMPLETE);
			break;
		}
		case WM_DDE_EXECUTE:
		{
			DDEACK wStatus;
			HGLOBAL hData;
			VERIFY(UnpackDDElParam(WM_DDE_EXECUTE, lParam, (UINT*)&wStatus, (UINT*)&hData));
			FreeDDElParam(WM_DDE_EXECUTE, lParam);
			PostMessage(WM_COMMAND, UM_PRINT_COMPLETE);
			break;
		}
	}
	m_nLastDDEMessage = 0;
	return 0L;
}

LRESULT CPrintDlg::OnDDETerminate(WPARAM wParam, LPARAM lParam)
{
	m_hPrintAppWnd = NULL;
	m_bConnectionMade = FALSE;
	
	EndDialog(SUCCESS);

	return 0L; 
}

void CPrintDlg::OnOK()
{
	if(m_bConnectionMade)
	{
		::PostMessage(m_hPrintAppWnd, WM_DDE_TERMINATE, (WPARAM)m_hWnd, 0);	
	}
	CDialog::OnOK();
}

void CPrintDlg::OnCancel()
{
	if(m_bConnectionMade)
	{
		::PostMessage(m_hPrintAppWnd, WM_DDE_TERMINATE, (WPARAM)m_hWnd, 0);	
	}
	CDialog::OnCancel();
}

BOOL CPrintDlg::GetRegistryString(HKEY hRootKey, LPCTSTR pszSubkey, LPCTSTR pszValue, CString& csValue)
{
   csValue.Empty();

   HKEY hKey = NULL;

   TRY
   {
      if (RegOpenKeyEx(
               hRootKey,
               pszSubkey,
               0,
               KEY_QUERY_VALUE,
               &hKey) == ERROR_SUCCESS)
      {
         DWORD dwLength = 0;
         if (RegQueryValueEx(
                  hKey,
                  pszValue,
                  NULL,
                  NULL,
                  NULL,
                  &dwLength) == ERROR_SUCCESS)
         {
            if (dwLength > 0)
            {
               long lResult = RegQueryValueEx(
                        hKey,
                        pszValue,
                        NULL,
                        NULL,
                        (LPBYTE)(csValue.GetBuffer(dwLength)),
                        &dwLength);

               csValue.ReleaseBuffer();

               if (lResult != ERROR_SUCCESS)
               {
                  csValue.Empty();
               }
            }
         }
      }

      RegCloseKey(hKey);
   }
   CATCH_ALL(e)
   {
      if (hKey != NULL)
      {
         RegCloseKey(hKey);
      }
      csValue.Empty();
   }
   END_CATCH_ALL

   return !csValue.IsEmpty();
}

static HWND FindMainWindow(void)
{
   HWND hWndDesktop = ::GetDesktopWindow();
   HWND hWndChild;

	char szMainWndProp[MAX_PATH];
	GetModuleFileName(NULL, szMainWndProp, MAX_PATH);

	CString csIniFile(szMainWndProp);
	csIniFile = csIniFile.Left(csIniFile.ReverseFind('\\') + 1) + "PrintInfo.ini";
   GetPrivateProfileString("Configuration", "MainWndProp", "AGPrint", szMainWndProp, MAX_PATH, csIniFile);

   for (hWndChild = ::GetWindow(hWndDesktop, GW_CHILD);
                  hWndChild != NULL;
                  hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT))
   {
      HANDLE hData = ::GetProp(hWndChild, szMainWndProp);
      if (hData == (HANDLE)1)
      {
         break;
      }
   }
   return hWndChild;
}


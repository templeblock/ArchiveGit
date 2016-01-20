#include "stdafx.h"
#include "ProcessExam.h"
#include "ProcessExamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CProcessExamApp theApp;
static int iExitCode;

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CProcessExamApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CProcessExamApp::CProcessExamApp()
{
	// Place all significant initialization in InitInstance
}

// For checking to see if Word is the email editor
#define REGKEY_OUTLOOK08_USING_WORD	"Software\\Microsoft\\Office\\8.0\\Outlook\\Options\\Mail"
#define REGKEY_OUTLOOK09_USING_WORD	"Software\\Microsoft\\Office\\9.0\\Outlook\\Options\\Mail"
#define REGKEY_OUTLOOK10_USING_WORD	"Software\\Microsoft\\Office\\10.0\\Outlook\\Options\\Mail"
#define REGKEY_OUTLOOK11_USING_WORD	"Software\\Microsoft\\Office\\11.0\\Outlook\\Options\\Mail"
#define REGVAL_OUTLOOK_USING_WORD	"EditorPreference" // DWORD bit 0 is on if using Word

/////////////////////////////////////////////////////////////////////////////
bool RegKeyValueExists(LPCTSTR pKeyName, LPCTSTR pValueName)
{
	CString strMsg;
	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, pKeyName) != ERROR_SUCCESS)
	{
		strMsg.Format("Error opening key %s", pKeyName);
		::MessageBox(NULL, strMsg, "JazzyMail", MB_OK);
		return false;
	}

	DWORD dwValue = 0;
	if (regkey.QueryDWORDValue(pValueName, dwValue) != ERROR_SUCCESS)
	{
//j		strMsg.Format("Error getting value %s", pValueName);
//j		::MessageBox(NULL, strMsg, "JazzyMail", MB_OK);
		return false;
	}
		
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool WordIsEmailEditor()
{
	CString strMsg;

	LPCTSTR pKeyName = REGKEY_OUTLOOK11_USING_WORD;
	LPCTSTR pValueName = REGVAL_OUTLOOK_USING_WORD;
	bool bFound = RegKeyValueExists(pKeyName, pValueName);
	strMsg.Format("Outlook11 %sFound", (bFound ? "" : "NOT "));
	::MessageBox(NULL, strMsg, "JazzyMail", MB_OK);

	if (!bFound)
	{
		pKeyName = REGKEY_OUTLOOK10_USING_WORD;
		bFound = RegKeyValueExists(pKeyName, pValueName);
		strMsg.Format("Outlook10 %sFound", (bFound ? "" : "NOT "));
		::MessageBox(NULL, strMsg, "JazzyMail", MB_OK);

		if (!bFound)
		{
			pKeyName = REGKEY_OUTLOOK09_USING_WORD;
			bFound = RegKeyValueExists(pKeyName, pValueName);
			strMsg.Format("Outlook9 %sFound", (bFound ? "" : "NOT "));
			::MessageBox(NULL, strMsg, "JazzyMail", MB_OK);
		}
	}

	if (!bFound)
	{
		::MessageBox(NULL, "No version of Outlook found", "JazzyMail", MB_OK);
		return false;
	}

	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, pKeyName) != ERROR_SUCCESS)
	{
		strMsg.Format("Error opening key %s", pKeyName);
		::MessageBox(NULL, strMsg, "JazzyMail", MB_OK);
		return false;
	}
	
	DWORD dwValue = 0;
	if (regkey.QueryDWORDValue(pValueName, dwValue) != ERROR_SUCCESS)
	{
		strMsg.Format("Error getting value %s", pValueName);
		::MessageBox(NULL, strMsg, "JazzyMail", MB_OK);
		return false;
	}
			
	// dwValue format:
	// 0x00010001 = PlainText & WordMail
	// 0x00020001 = HTML & WordMail
	// 0x00030001 = RTF & WordMail
	// 0x00010000 = PlainText
	// 0x00020000 = HTML
	// 0x00030002 = RTF

	bool bWordMail  = !!(dwValue & 0x00000001);
	bool bRTF       = !!(dwValue & 0x00000002);
	bool bPlainText = !!(dwValue & 0x00010000);
//j	if (!bWordMail && !bRTF && !bPlainText)
	if (!bWordMail && !bRTF) // Allow plain text for now since we can convert it
	{
		strMsg.Format("DWORD is 0x%lx", dwValue);
		::MessageBox(NULL, strMsg, "JazzyMail", MB_OK);
		return false;
	}

	// Ask the user if we can make the change from Word to HTML mail
	bool bOK = (::MessageBox(NULL, "JazzyMail requires that you use HTML as your default email format.\n\nIs it OK for us to make that change for you?", "JazzyMail", MB_YESNO) == IDYES);
	if (!bOK)
		return true;

	dwValue &= ~0x00000001;
	dwValue &= ~0x00000002;
	dwValue &= ~0x00010000;
	if (regkey.SetDWORDValue(pValueName, dwValue) != ERROR_SUCCESS)
		return true;
	
	::MessageBox(NULL, "HTML is now your default email format.\n\nPlease restart Outlook in order to activate this change.", "JazzyMail", MB_OK);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CProcessExamApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("ProcessExam"));

	WordIsEmailEditor();
	return false;
	
	CProcessExamDlg dlg;

	int nMatches = dlg.ProcessWalk(TA_NONE, true/*bSilent*/);
	if (!nMatches)
	{
		iExitCode = 0;
		return false;
	}

	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK || nResponse == IDCANCEL)
	{
		int nMatches = dlg.ProcessWalk(TA_NONE, true/*bSilent*/);
		if (nMatches)
			iExitCode = -1;
		return false;
	}

	// Since the dialog has been closed, return false so that we exit the
	//  application, rather than start the application's message pump.
	return false;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProcessExamApp::ExitInstance()
{
	return iExitCode;
}

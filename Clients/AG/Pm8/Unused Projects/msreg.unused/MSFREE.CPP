// msfree.cpp : Defines the class behaviors for the application.
//

#include "msfrafx.h"
#include "msfree.h"
#include "cmfaddon.h"

extern char const far _afxWnd[];

#include "cctl3d.h"
CControl3D Control3D;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef PMW_CLASSIC
#include "pmwmm\enabled.h"
#endif

char m_cbWorkingDirectory[_MAX_PATH];

/////////////////////////////////////////////////////////////////////////////
// CMsfreeApp

BEGIN_MESSAGE_MAP(CMsfreeApp, CWinApp)
	//{{AFX_MSG_MAP(CMsfreeApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsfreeApp construction

CMsfreeApp::CMsfreeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMsfreeApp object

CMsfreeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// The message sent to signal our completion

static UINT m_msgInstallFinished = 0;
static DWORD m_dwInstallFinished = 0;
static char szTitle[] = "Free Fonts";

/////////////////////////////////////////////////////////////////////////////
// CMsfreeApp initialization

BOOL CMsfreeApp::InitInstance()
{
	// Initialize.
	
	if (SetWorkingVariables())
	{
		// No multiple instances.

		HWND hWnd = FindWindow(NULL, szTitle);

//		if (m_hPrevInstance == NULL)
		if (hWnd == NULL)
		{
			SetDialogBkColor();        // set dialog background color to gray
				
			Control3D.Startup(AfxGetInstanceHandle());

			// Create a main window to use as a parent and to display the icon.
			
         // Use desktop window so any children can properly center themselves on us.
			CWnd* pDesktopWnd = CWnd::GetDesktopWindow();
			CRect crRect;
			pDesktopWnd->GetWindowRect(crRect);
						
			m_pMainWnd = (CWnd*)(new CFrameWnd);
			((CFrameWnd*)m_pMainWnd)->Create(AfxRegisterWndClass(0, 0, 0, LoadIcon("IDI_ICON1")), szTitle, WS_OVERLAPPEDWINDOW, crRect);

			// Install the bonus.
			
			InstallFreeBonus();
		}
		else
		{
			HWND hWndLastActive;

			hWndLastActive = GetLastActivePopup(hWnd);

			if ( IsIconic(hWnd) )
			{
				ShowWindow(hWnd, SW_SHOWNORMAL);
			}
			else
			{
#ifdef WIN32
				SetForegroundWindow(hWnd);
#endif
				BringWindowToTop(hWnd);
				if ( hWndLastActive && (hWndLastActive != hWnd) )
				{
#ifdef WIN32
					SetForegroundWindow(hWndLastActive);
#endif
					BringWindowToTop( hWndLastActive );
				}
			}
		}
	}
	
	// Tell whoever is listening that we're done.

	if ((m_msgInstallFinished != 0) && (m_dwInstallFinished != 0))
	{
		PostMessage(HWND_BROADCAST, m_msgInstallFinished, 0, m_dwInstallFinished);
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CMsfreeApp free bonus installation

void CMsfreeApp::InstallFreeBonus(void)
{
	// PrintMaster classic tells the user that their free graphics
	// are already enabled and then lets them install fonts.
	
#ifdef PMW_CLASSIC
	CFreeFontsAndGraphicsEnabledDialog Dialog(m_pMainWnd);
	Dialog.DoModal();
#endif

	// Use MFADDON.DLL to allow the user to install free fonts.
	
	char cbLibraryName[_MAX_PATH];

	lstrcpy(cbLibraryName, m_cbWorkingDirectory);
	lstrcat(cbLibraryName, "MFADDON.DLL");
	
	CMFAddonDll Dll(cbLibraryName);

	if (!Dll.Valid())
	{
		// Can't do this.
		AfxMessageBox("MFADDON.DLL is missing or bad.");
		return;
	}

	rAddonProcessFontsData data;

	// Do this first so we have it.
	strcpy(data.ApplicationDirectory, m_cbWorkingDirectory);

	data.RecordLength = sizeof(data);

	data.Flags = AddonFlagAlwaysDeleteOtlFonts|AddonFlagOnlyRegistration;
	data.AskForSource = AddonAskIfNotValid;

	strcpy(data.TrueTypeDll, data.ApplicationDirectory);
	strcat(data.TrueTypeDll, "MFOTL2TT.DLL");

	data.PostScriptDll[0] = '\0';

	strcpy(data.SymbolOtl, data.ApplicationDirectory);
	strcat(data.SymbolOtl, "SYMBOL.OTL");

	strcpy(data.SymbolSet, data.ApplicationDirectory);
	strcat(data.SymbolSet, "SYMBOL.SET");

	GetSystemDirectory(data.TempDirectory, sizeof(data.TempDirectory));
	
	data.ConversionType = AddonTrueTypeConversion;
	strcpy(data.Source, data.ApplicationDirectory);

	GetSystemDirectory(data.TTFDirectory, sizeof(data.TTFDirectory));
	data.PFBDirectory[0] = '\0';
	data.PFMDirectory[0] = '\0';

	Dll.ProcessFonts(m_pMainWnd->GetSafeHwnd(), &data);
	
	// Report what happened using correct english.
	if (data.TrueTypeFontsInstalled != 0)
	{
		char szMessage[256];
		wsprintf(
			szMessage,
			"%d TrueType font%s %s installed.",
			data.TrueTypeFontsInstalled,
			(LPCSTR)(data.TrueTypeFontsInstalled == 1 ? "" : "s"),
			(LPCSTR)(data.TrueTypeFontsInstalled == 1 ? "was" : "were"));
		::MessageBox(m_pMainWnd->GetSafeHwnd(), szMessage, "Congratulations!", MB_OK|MB_ICONINFORMATION);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMsfreeApp main initialization

BOOL CMsfreeApp::SetWorkingVariables(void)
{
	BOOL fResult = FALSE;
	
	CString csValue;
	
	TRY
	{
		// Setup the working directory.
	
		::GetModuleFileName(AfxGetInstanceHandle(), m_cbWorkingDirectory, sizeof(m_cbWorkingDirectory)-1);
	
		// Skip the drive.
	
		char* ptr;
		if ((ptr = strchr(m_cbWorkingDirectory, ':')) == NULL)
		{
			ptr = m_cbWorkingDirectory;
		}
		else
		{
			ptr++; 
		}
	
		// Skip all directories.
	
		char *ptr2;
		while ((ptr2 = strchr(ptr, '\\')) != NULL)
		{
			ptr = ptr2+1;
		}
		*ptr = '\0';	// Knock off the final name.
	
		// Make sure this is terminated correctly.
	
		int length = lstrlen(m_cbWorkingDirectory);
		if (length != 0 && m_cbWorkingDirectory[length-1] != '\\' && m_cbWorkingDirectory[length-1] != ':')
		{
			m_cbWorkingDirectory[length++] = '\\';
			m_cbWorkingDirectory[length] = '\0';
		}
		
		// Get the name of the message to send when we terminate.
		
		if (LocateParameter("/M", &csValue))
		{
			m_msgInstallFinished = RegisterWindowMessage(csValue);
		
			if (m_msgInstallFinished == 0)
			{
				AfxThrowUserException();
			}
	
			// Get the id number to use when we terminate.
		
			if (LocateParameter("/D", &csValue))
			{
				m_dwInstallFinished = atol(csValue);
				
				if (m_dwInstallFinished == 0)
				{
					AfxThrowUserException();
				}
			}
		}
		
		fResult = TRUE;
	}
	CATCH_ALL(e)
	{
		fResult = FALSE;
	}
	END_CATCH_ALL
	
	return fResult;
}

/*
// Parse a parameter in the command line. A Parameter always begins
// at the start of the command line or after a space. No spaces
// are allowed within parameters, but quoted strings are handled
// properly and may contain spaces. All comparisons are case-insensitive.
//
// The command line is what is stored in the app.
*/

LPSTR CMsfreeApp::LocateParameter(LPCSTR pszParamName, CString* pcsValue)
{
	LPSTR pszResult = NULL;
	
	LPSTR pszCommandLine = m_lpCmdLine;		// <- the MFC command line.
	int nCommandLineLength = lstrlen(pszCommandLine);
	int nParamNameLength = lstrlen(pszParamName);
	BOOL fInQuote = FALSE;
	BOOL fStartOfParameter = TRUE;
	BOOL fFoundParameter = FALSE;
	
	TRY
	{
		// Empty the result string.
		
		if (pcsValue != NULL)
		{
			pcsValue->Empty();
		}
		
		// Search for the parameter name.
		
		while ((!fFoundParameter) && (nCommandLineLength >= nParamNameLength))
		{
			if (fStartOfParameter)
			{
				// A parameter can start here, check for a match.
				
				int i = 0;
				
				for (;;)
				{
					char ch1 = pszCommandLine[i];
					if ((ch1 > 'a') && (ch1 < 'z')) ch1 = (char)(ch1+'A'-'a');
					
					char ch2 = pszParamName[i];
					if ((ch2 > 'a') && (ch2 < 'z')) ch2 = (char)(ch2+'A'-'a');
					
					if (ch2 == 0)
					{
						fFoundParameter = TRUE;
						break;
					}
					
					if (ch1 != ch2)
					{
						fStartOfParameter = FALSE;
						break;
					}
					
					i++;
				}
			}
			
			else
			{
				if (*pszCommandLine == ' ')
				{
					fStartOfParameter = !fInQuote;
				}
				
				else if (*pszCommandLine == '"')
				{
					fInQuote = !fInQuote;
				}
				
				pszCommandLine++;
				nCommandLineLength--;
			}
		}
		
		// If we found the parameter, collect its value into csValue.
		
		if (fFoundParameter)
		{
			pszResult = pszCommandLine;
			
			if (pcsValue != NULL)
			{
				pszCommandLine += nParamNameLength;
	
				if (*pszCommandLine == '=')
				{
					pszCommandLine++;
				}
				
				fInQuote = FALSE;
				
				for (;;)
				{
					if (*pszCommandLine == 0)
					{
						break;
					}
					
					if ((!fInQuote) && (*pszCommandLine == ' '))
					{
						break;
					}
					
					if (*pszCommandLine == '"')
					{
						fInQuote = !fInQuote;
					}
					
					else
					{
						(*pcsValue) += *pszCommandLine;
					}
					
					pszCommandLine++;
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		if (pcsValue != NULL)
		{
			pcsValue->Empty();
		}
		
		pszResult = NULL;
	}
	END_CATCH_ALL
	
	return pszResult;
}

BOOL CMsfreeApp::GetParameter(LPCSTR pszParamName, CString* pcsValue /*=NULL*/)
{
	return LocateParameter(pszParamName, pcsValue) != NULL;
}


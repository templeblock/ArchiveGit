// Uninstall.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Uninstal.h"
#include "Uninsdlg.h"
#include "util.h"
#ifdef WIN32
#include <regstr.h>
#else
#include "regstr16.h"			// Our version of this for 16-bit
#include <stdlib.h>
#include <direct.h>
#endif
#include "deldlg.h"
#include "progdlg.h"
#include "rfor.h"
#include "pmwcfg.h"
#include "heap.h"

#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BOOL ddeInit(HINSTANCE hInst, HINSTANCE hPrevInst);
extern HWND pmOpen(LPCSTR szName, BOOL fDelete);
extern BOOL pmClose(LPCSTR szName);
extern BOOL pmDeleteGroup(LPCSTR lpszName);
extern BOOL pmReplaceItem(LPCSTR lpszName);
extern BOOL pmAddItem(LPCSTR szName, LPCSTR szCmd, LPCSTR szIconName, WORD wIconNumber);
extern BOOL pmDeleteItem(LPCSTR lpszName);
extern BOOL pmGetGroupData(CString& csResult, LPCSTR pszGroupName = NULL);
extern void ddeShutdown(void);

// The system heap.
HeapPtr system_heap;

// The following files are REQUIRED to run the uninstall program.
// They are deleted LAST, so that the user can restart the uninstall
// program is something goes wrong.
LPCSTR pszUninstallProgramFiles[] =
{
	"MSREG.INI",
	"MSRUN.EXE"
};

#define NUMBER_OF_UNINSTALL_PROGRAM_FILES (sizeof(pszUninstallProgramFiles)/sizeof(pszUninstallProgramFiles[0]))

/*
// Registry configuration keys.
*/

char BASED_CODE SECTION_Registry[] = "Registry";
char BASED_CODE ENTRY_Name[] = "Name";
char BASED_CODE ENTRY_Version[] = "Version";
char BASED_CODE ENTRY_Company[] = "Company";

int UninstMessageBox(UINT nID, UINT nType /*=MB_OK*/)
{
	CString csMessage;
	csMessage.LoadString(nID);
	return UninstMessageBox(csMessage, nType);
}

int UninstMessageBox( CString csMessage, UINT nType /*=MB_OK*/)
{
	GetConfiguration()->ReplaceText(csMessage);
	CString csTitle;
	csTitle.LoadString(IDS_MB_TITLE);
	GetConfiguration()->ReplaceText(csTitle);

	return AfxGetMainWnd()->MessageBox(csMessage, csTitle, nType);

}
/////////////////////////////////////////////////////////////////////////////
// Some little static functions.

static BOOL GetNextLine(CString& csLines, CString& csLine)
{
	BOOL fResult = FALSE;
	TRY
	{
		csLine.Empty();

		// Find end of current line.
		int nIndex = csLines.Find('\n');

		if (nIndex == -1)
		{
			// Handle last line without carriage return.
			if (!csLines.IsEmpty())
			{
				csLine = csLines;
				csLines.Empty();
				fResult = TRUE;
			}
		}
		else
		{
			// Got a carriage return.
			fResult = TRUE;

			// Strip of trailing /n and /r
			int nLastChar = nIndex;
			while ((nLastChar >= 0)
				  && ((csLines[nLastChar] == '\n') || (csLines[nLastChar] == '\r')))
			{
				nLastChar--;
			}
			csLine = csLines.Left(nLastChar+1);
			csLines = csLines.Mid(nIndex+1);
		}
	}
	END_TRY

	return fResult;
}

static BOOL GetNextField(CString& csFields, CString& csField, char cDelimiter)
{
	BOOL fResult = FALSE;
	TRY
	{
		csField.Empty();

		// Find end of current field.
		int nIndex = csFields.Find(cDelimiter);

		if (nIndex == -1)
		{
			// Handle last field without delimiter.
			if (!csFields.IsEmpty())
			{
				csField = csFields;
				csFields.Empty();
				fResult = TRUE;
			}
		}
		else
		{
			// Got a field.
			fResult = TRUE;
			csField = csFields.Left(nIndex);
			csFields = csFields.Mid(nIndex+1);

			// Strip off leading an trailing quotes.
			if (!csField.IsEmpty() && (csField[0] == '"'))
			{
				csField = csField.Mid(1);
			}
			if (!csField.IsEmpty() && (csField[csField.GetLength()-1] == '"'))
			{
				csField = csField.Left(csField.GetLength()-1);
			}
		}
	}
	END_TRY

	return fResult;
}

static BOOL GetField(const CString& csFields, CString& csField, char cDelimiter, int nField)
{
	CString csTemp;
	BOOL fResult = FALSE;
	TRY
	{
		csField.Empty();
		csTemp = csFields;

		fResult = TRUE;
		for (int i = 0; i <= nField; i++)
		{
			if (!GetNextField(csTemp, csField, cDelimiter))
			{
				fResult = FALSE;
				break;
			}
		}
	}
	END_TRY

	return fResult;
}

CString& GetShortName(const CString& csLongName, CString& csShortName)
{
#ifdef WIN32
	GetShortPathName(csLongName, csShortName.GetBuffer(_MAX_PATH+1), _MAX_PATH);
	csShortName.ReleaseBuffer();
#else
	csShortName = csLongName;
#endif
	return csShortName;
}

/////////////////////////////////////////////////////////////////////////////
// CUninstallApp

BEGIN_MESSAGE_MAP(CUninstallApp, CWinApp)
	//{{AFX_MSG_MAP(CUninstallApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUninstallApp construction

CUninstallApp::CUninstallApp()
{
	// Place all significant initialization in InitInstance
	system_heap = new StandardHeap;
	m_fDemoVersion = FALSE;
	m_fWorkstationUninstall = FALSE;
	m_pProgressDialog = NULL;
}

CUninstallApp::~CUninstallApp()
{
	if (m_pProgressDialog != NULL)
	{
		m_pProgressDialog->DestroyWindow();
		delete m_pProgressDialog;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUninstallApp object

CUninstallApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUninstallApp initialization

BOOL CUninstallApp::InitInstance()
{
	BOOL fGoDeleteYourself = FALSE;
	if (system_heap == NULL)
	{
		// We really need a heap...
		return FALSE;
	}
	m_ResourceString.SetInstanceHandle (m_hInstance);

	//
	// Compute the home directory.
	//

	ComputeHomeDirectory();

	/*
	// Make sure we have a working directory parameter.
	*/

	//
	// Compute the working directory.
	//

	char convert_buffer[32];
	char buffer[_MAX_PATH+1];
	m_lpCmdLine = GetCommandLine();	
	if (GetParameter("/W", buffer) == NULL)
	{
		// Check if it's first incarnation
		if (GetParameter("/I", buffer))
		{
			CString csModuleName;
			GetModuleFileName(AfxGetInstanceHandle(), csModuleName.GetBuffer(256), 256-1);
			csModuleName.ReleaseBuffer();

			// Find the temp dir
			char temp_dir[_MAX_PATH];
			GetTempPath(_MAX_PATH, temp_dir);
			CString csClone = CString(temp_dir) + "~tmp.EXE";
			
			// Copy the uninstall app to temp
			if (!CopyFile((LPCTSTR)csModuleName, (LPCTSTR)csClone, FALSE))
				return FALSE;

			CString csInstdatName;
			csInstdatName.LoadString(IDS_INSTDAT_NAME);
			CString csInstDatClone = CString(temp_dir) + csInstdatName;
			CString csInstDatPath = csModuleName.Left(csModuleName.ReverseFind('\\') + 1) + csInstdatName;
			// Copy the uninstall app to temp
			if (!CopyFile((LPCTSTR)csInstDatPath, (LPCTSTR)csInstDatClone, FALSE))
				return FALSE;

			// Delete the clone upon reboot
			ReplaceFileOnReboot((LPCTSTR)csClone, NULL);

			// Launch the clone and return
			HANDLE hProcessOrig = OpenProcess(SYNCHRONIZE, TRUE, GetCurrentProcessId());

			PROCESS_INFORMATION ProcessInformation;

			STARTUPINFO StartupInfo;
			ZeroMemory(&StartupInfo, sizeof(StartupInfo));
			StartupInfo.cb = sizeof(StartupInfo);

			CString csCmdLine = 
				csClone +
				" /H" +
				CString(itoa((int)hProcessOrig, convert_buffer, 10)) +
				" /W\"" + 
				m_csHomeDirectory +
				"\"";

			if (!CreateProcess(
								 NULL,
								 csCmdLine.GetBuffer(256),
								 NULL,
								 NULL,
								 TRUE,
								 NORMAL_PRIORITY_CLASS,
								 NULL,
								 NULL,
								 &StartupInfo,
								 &ProcessInformation))
			{
				LPVOID lpMsgBuf;
				FormatMessage(     
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,    
					NULL,
					GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,    0,    NULL );		// Process any inserts in lpMsgBuf.
					// ...
					// Display the string.
					UninstMessageBox((LPCTSTR)lpMsgBuf);
					// Free the buffer.
					LocalFree( lpMsgBuf );
			}
			CloseHandle(hProcessOrig);
		}
		return FALSE;
	}
	else
	{
	
		// Wait for original process to terminate, then we're all set.
		GetParameter("/H", convert_buffer);
		HANDLE hProcessOrig = (HANDLE) atoi(convert_buffer);

		// Wait for it to finish...
		MsgWaitForMultipleObjects(
							1,											// nCount
							&hProcessOrig,							// pHandles
							FALSE,									// bWaitAll
							INFINITE,								// dwMilliseconds
							QS_ALLINPUT);							// dwWakeMask
		CloseHandle(hProcessOrig);

	}
	/*
	// Read the installation data.
	*/
	if (CPrintMasterConfiguration::IsAddOn () == TRUE)
	{
		m_installation_info.nInstallConfiguration = CPrintMasterConfiguration::AddOn;
		m_installation_info.nCurrentConfiguration = CPrintMasterConfiguration::AddOn;   
	}
	else
	{
		if (!ReadInstDat())
		{
			return FALSE;
		}
	}
	/*
	// Validate that we have some kind of configuration.
	*/

	if (m_installation_info.nInstallConfiguration == 0
		 || m_installation_info.nCurrentConfiguration == 0)
	{
		return ConfigurationError();
	}
	m_signature_file = CPrintMasterConfiguration::GetSignatureFile(m_installation_info.nInstallConfiguration);

	if (m_signature_file == NULL)
	{
		  return ConfigurationError();
	}
	/*
	// Create our product configuration.
	*/

	if (!CPrintMasterConfiguration::Create(m_installation_info.nCurrentConfiguration))
	{
		return ConfigurationError();
	}

	m_csWorkingDirectory = buffer;

	//
	// Build the path to the MSREG.INI file.
	//

	CString csTemp;
	Util::ConstructPath(csTemp, m_csWorkingDirectory, "MSREG.INI");
	m_MsregIniFile.Name(csTemp);

	/*
	// Get the registry keys needed for installation, etc.
	*/

	m_csRegistryName = m_MsregIniFile.GetString(SECTION_Registry,
															  ENTRY_Name);
	m_csRegistryVersion = m_MsregIniFile.GetString(SECTION_Registry,
																  ENTRY_Version);
	m_csRegistryCompany = m_MsregIniFile.GetString(SECTION_Registry,
																  ENTRY_Company);
	if (m_csRegistryName.IsEmpty()
		 || m_csRegistryVersion.IsEmpty()
		 || m_csRegistryCompany.IsEmpty())
	{
	/* Some registry info is missing. */
		UninstMessageBox(IDS_ErrBadRegistryInfo);
		return FALSE;
	}

	//
	// See if this is a demo version.
	//

	if (GetParameter("/D", buffer) != NULL)
	{
		m_fDemoVersion = TRUE;
	}

	//
	// See if this is a network version.
	//

	if (GetParameter("/N", buffer) != NULL)
	{
		m_fWorkstationUninstall = TRUE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//	Enable3dControls();
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Let the user know what's going to happen and give them a chance
	// to change their mind.
	CUninstallDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	/*
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	*/

	
	return FALSE;
}

BOOL CUninstallApp::ConfigurationError(void)
{
	UninstMessageBox(IDS_CONFIG_NO_SEE, MB_OK | MB_ICONEXCLAMATION);
	return FALSE;
}

BOOL CUninstallApp::ReadInstDat(void)
{
   CString csInstdatName;

   TRY
      LoadConfigurationString(IDS_INSTDAT_NAME, csInstdatName);
   END_TRY

	char path[MAX_PATH];
   GetModuleFileName(NULL, path, MAX_PATH);
   CString csPath(path);
   csPath = csPath.Left(csPath.ReverseFind('\\') + 1) + csInstdatName;

   if (!m_installation_info.Get(csPath, FALSE))
   {
      CString message;

      TRY
      {
         char buffer[_MAX_PATH];
         LoadConfigurationString(IDS_INSTDAT_MISSING, message);
         sprintf(buffer, message, csPath);
         UninstMessageBox(buffer, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
      }
      CATCH_ALL(e)
      {
      }
      END_CATCH_ALL
   /* Regardless. */
      return FALSE;
   }

   return TRUE;
}

void CUninstallApp::ComputeHomeDirectory(void)
{
	/*
	// Get the source directory for the install. We assume that UNINST*.EXE
	// is being run from the MSREG directory under the root of the product
	// directory tree as shipped by MLS.
	*/

	CString csPath;
	CString csFile;

	TRY
	{
		int nLength = ::GetModuleFileName(AfxGetInstanceHandle(), m_csHomeDirectory.GetBuffer(256), 256-1);
		m_csHomeDirectory.ReleaseBuffer();

		if (nLength == 0)
		{
			// ::GetModuleFileName failed, force string to empty
			m_csHomeDirectory.Empty();
		}

		else
		{
			nLength = m_csHomeDirectory.GetLength();	// reget value to be DBCS safe

			// strip off module name to get directory
			while (--nLength >= 0)
			{
				if (m_csHomeDirectory[nLength] == '\\')
				{
					break;
				}
			}

			if (nLength < 0)
			{
				m_csHomeDirectory.Empty();
			}

			else
			{
				m_csHomeDirectory = m_csHomeDirectory.Left(nLength+1);
			}
		}
	}
	END_TRY
}

/*
// Do the actual uninstallation.
*/

void CUninstallApp::Uninstall(void)
{

	// Create the progress dialog.
	m_pProgressDialog = NULL;

	TRY
	{
		m_pProgressDialog = new CUninstallProgressDialog;
		if (m_pProgressDialog != NULL)
		{
			if (!m_pProgressDialog->Create(IDD_PROGRESS, m_pMainWnd))
			{
				delete m_pProgressDialog;
				m_pProgressDialog = NULL;
			}
		}
	}
	CATCH_ALL(e)
	{
		delete m_pProgressDialog;
		m_pProgressDialog = NULL;
	}
	END_CATCH_ALL

	ShowProgress();

	// First, try to delete the files in the PMG program directory which
	// can go away, but still let the user run the uninstall program. This
	// is useful, because PMG may have crashed and some of the PMG DLL's may
	// still be in memory and cannot be deleted. The user should be allowed
	// to retry the uninstall after restarting Windows.
	BOOL fSuccess = FALSE;
	if (DeleteProgramDirectory(TRUE))
	{
		// All "non-essential" files have been removed. Not that this breaks
		// many of the icons in the program manager group. The uninstall icon
		// should still be functional.
		if (UpdateRegistry())
		{
			// Remove any shared DLL's
			DeleteSharedDlls();

			// Remove the uninstall item from the registry.
			RemoveUninstall();

			// Attempt to delete the PrintMaster program manager group.
			// If this is successful, then the the user no longer has a
			// way to launch the uninstall even though the required files
			// still exist. So, the next step better succeed!
			if (DeleteGroup())
			{
				if (DeleteProgramDirectory(FALSE))
				{
					fSuccess = TRUE;
				}
			}
		}
	}

	HideProgress();
	UninstMessageBox(fSuccess ? IDS_UNINSTALL_SUCCESS : IDS_UNINSTALL_FAILURE);
}

void CUninstallApp::HandleOldGroup(LPCSTR pszGroupName, BOOL fDeleteIfEmpty /*=TRUE*/)
{
	TRACE("Handle old Group |%s|\n", pszGroupName);
	CString csItems;
	CString csItem;
	CString csName;
	CString csFileName;
	CString csPath;
	CString csMatchPath;
	TRY
	{
		csPath = m_csWorkingDirectory;
		Util::RemoveBackslashFromPath(csPath);
		GetShortName(csPath, csMatchPath);

		if (pmGetGroupData(csItems, pszGroupName))
		{
			// Check if there are some items to delete. We check this
			// without deleting anything so we don't bring groups to
			// the front which are not going to be affected. We also check
			// if all the items are going to be deleted.

			BOOL fDeletedAllItems = TRUE;
			BOOL fDeletedSomeItems = FALSE;

			// Skip past the first line... it's the group itself.
			if (GetNextLine(csItems, csItem))
			{
				while (GetNextLine(csItems, csItem))
				{
					// Got a line, get the program path - field 1 (zero based).
					if (GetField(csItem, csFileName, ',', 1))
					{
						TRY
						{
							Util::SplitPath(csFileName, &csPath, NULL);
							Util::RemoveBackslashFromPath(csPath);
							TRACE("Path for |%s| is |%s|\n", (LPCSTR)csFileName, (LPCSTR)csPath);
							if (csPath.CompareNoCase(csMatchPath) == 0)
							{
								fDeletedSomeItems = TRUE;
							}
							else
							{
								fDeletedAllItems = FALSE;
							}
						}
						CATCH_ALL(e)
						{
							fDeletedAllItems = FALSE;
						}
						END_CATCH_ALL
					}
				}

				if (fDeletedSomeItems)
				{
					if (fDeletedAllItems && fDeleteIfEmpty)
					{
						// Everything goes, delete the entire enchilada.
						TRACE("Delete the entire group |%s|\n", pszGroupName);
						pmDeleteGroup(pszGroupName);
					}
					else
					{
						// Trudge through the names and delete individual items.
						// Note that this will bring the group to the top.
						if (pmGetGroupData(csItems, pszGroupName))
						{
							if (pmOpen(pszGroupName, FALSE))
							{
								if (GetNextLine(csItems, csItem))
								{
									while (GetNextLine(csItems, csItem))
									{
										// Got a line, get the program path - field 1 (zero based).
										if (GetField(csItem, csFileName, ',', 1))
										{
											TRY
											{
												Util::SplitPath(csFileName, &csPath, NULL);
												Util::RemoveBackslashFromPath(csPath);
												TRACE("Path for |%s| is |%s|\n", (LPCSTR)csFileName, (LPCSTR)csPath);
												if (csPath.CompareNoCase(csMatchPath) == 0)
												{
													if (GetField(csItem, csName, ',', 0))
													{
														TRACE("DELETE |%s|!\n", (LPCSTR)csName);
														pmDeleteItem(csName);
													}
												}
											}
											END_TRY
										}
									}
								}

								pmClose(pszGroupName);
							}
						}
					}
				}
			}
		}
	}
	END_TRY
}

void CUninstallApp::GetStartupGroupName(CString& csName)
{
	TRY
	{
		csName.LoadString(IDS_STARTUP);
		// check if thisa group exists, if not, use the generic default
		// startup group name
		CString csItems;
		if (!pmGetGroupData(csItems, csName))
			csName = "Startup";
	}
	END_TRY
}

/*
// Delete the Program Manager.
// This removes us from the Start menu as well.
*/

BOOL CUninstallApp::DeleteGroup(void)
{
	CString csGroupName;
	CString csMessage;


	TRY
	{
		LoadConfigurationString(m_fDemoVersion ? IDS_PMG_GROUP_TRIAL_VERSION : IDS_PMG_GROUP, csGroupName);
		csMessage.LoadString(IDS_UNINSTALLING);
	}
	END_TRY

	SetProgress(csMessage, csGroupName);

	/*
	// Set up for doing DDE communications with PROGMAN for creating the program manager group.
	*/

	if (!ddeInit(AfxGetInstanceHandle(), m_hPrevInstance))
	{
		UninstMessageBox(IDS_CANT_INITIALIZE_DDE);
		return FALSE;
	}

	TRACE("Handle old Groups\n");
	CString csGroups;
	CString csGroup;
	CString csStartupGroup;


	TRY
	{
		// Get the name of the startup group.
		GetStartupGroupName(csStartupGroup);

		// Get the group names.
		if (pmGetGroupData(csGroups))
		{
			while (GetNextLine(csGroups, csGroup))
			{
				if (csGroup.Find(csGroupName) >= 0)
				{
					HandleOldGroup(csGroup);
				}
				if (csGroup.CompareNoCase(csStartupGroup) == 0)
				{
					HandleOldGroup(csGroup, FALSE);
				}
			}
		}
	}
	END_TRY

	ddeShutdown();

	return TRUE;
}

/*
// Update the registry to remove all traces of us.
*/

struct CProjectInfo
{
	LPCSTR m_pObjectType;
	LPCSTR m_pLongName;
	LPCSTR m_pExtension;					// Including the dot.
	LPCSTR m_pCLSID;
};

static CProjectInfo ProjectInfo[] =
{
	{"PMWPosterType",		 "PMW Poster Type",			".sig",	"{76F54460-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWCardType",		 "PMW Card Type",				".car",	"{76F54461-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWBannerType",		 "PMW Banner Type",			".ban",	"{76F54462-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWCalendarType",	 "PMW Calendar Type",		".cal",	"{76F54463-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWLabelType",		 "PMW Label Type",			".lbl",	"{76F54464-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWEnvelopeType",	 "PMW Envelope Type",		".env",	"{76F54465-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWBizCardType",	 "PMW Business Card Type",	".biz",	"{76F54466-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWCertificateType","PMW Certificate Type",	".cer",	"{76F54467-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWNoteCardType",	 "PMW Note Card Type",		".not",	"{76F54468-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWFaxCoverType",	 "PMW Fax Cover Type",		".fax",	"{76F54469-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWLetterheadType", "PMW Letterhead Type",		".let",	"{76F5446A-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWNewsletterType", "PMW Newsletter Type",		".nws",	"{76F5446B-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWBrochureType",	 "PMW Brochure Type",		".bro",	"{76F5446C-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWHalfCardType",	 "PMW Half Card Type",		".hcr",	"{76F5446D-046F-11cf-B79A-0000C0E9C528}"},
	{"PMWWebPubType",		 "PMW Web Pub Type",			".web",	"{76F5446E-046F-11cf-B79A-0000C0E9C528}"},
	{ NULL, NULL, NULL, NULL }
};

BOOL CUninstallApp::UpdateRegistry(void)
{
#ifdef WIN32
	CString csMessage;
	TRY
	{
		csMessage.LoadString(IDS_UPDATING_REGISTRY);
	}
	END_TRY

	SetProgress(csMessage, "");

/*
// Remove the project keys.
*/
	CProjectInfo* pInfo = ProjectInfo;
	while (pInfo->m_pObjectType != NULL)
	{
	// Remove the CLSID for this object.

		CString csKey = "CLSID\\";
		csKey += pInfo->m_pCLSID;
		DeleteKey(HKEY_CLASSES_ROOT, csKey);

	// Remove the object type for this object.

		DeleteKey(HKEY_CLASSES_ROOT, pInfo->m_pObjectType);

	// Remove the extension for this object if the object referred to is this.

		char cbBuffer[_MAX_PATH];
		long lSize = sizeof(cbBuffer);

		if (::RegQueryValue(HKEY_CLASSES_ROOT,
								  pInfo->m_pExtension,
								  cbBuffer,
								  &lSize) == ERROR_SUCCESS)
		{
		// We got the value associated with this extension.
		// If the value is our object, remove it.
			if (stricmp(cbBuffer, pInfo->m_pObjectType) == 0)
			{
			/* The value is one we put in. Delete it. */
				DeleteKey(HKEY_CLASSES_ROOT, pInfo->m_pExtension);
			}
		}

		pInfo++;
	}

// Remove the software entry for PMW 2.00 under "company name".

// "Software\\MicroLogic Software\\PrintMaster Gold Publishing Suite\\2.00",
	CString csKey = "SOFTWARE\\";
	csKey += m_csRegistryCompany;
	csKey += '\\';
	csKey += m_csRegistryName;
	csKey += '\\';
	csKey += m_csRegistryVersion;

	DeleteKey(HKEY_LOCAL_MACHINE,
				 csKey,
				 2);			// nDeleteUp

// Remove the app path for PMW.

	csKey = REGSTR_PATH_APPPATHS "\\";
	csKey += m_MsregIniFile.GetString("Configuration", "exe", "pmw.exe");
	DeleteKey(HKEY_LOCAL_MACHINE,	 csKey);

#endif

	return TRUE;
}

/*
// Delete the shared dlls (if the references so warrant).
*/

static LPCSTR pSharedDlls[] =
{
	"MFCANS32.DLL",
	"MFCUIA32.DLL",
	NULL
};

BOOL CUninstallApp::DeleteSharedDlls(void)
{
#ifdef WIN32
	CString csMessage;
	TRY
	{
		csMessage.LoadString(IDS_UPDATING_SHARED_FILES);
	}
	END_TRY

	SetProgress(csMessage, "");

	char cbBuffer[_MAX_PATH];
	GetSystemDirectory(cbBuffer, sizeof(cbBuffer));

	LPCSTR* ppName = pSharedDlls;
	while (*ppName != NULL)
	{
	/*
	// Our shared files are in the system directory.
	*/
		CString csPath;

		Util::ConstructPath(csPath, cbBuffer, *ppName);
		SetItem(*ppName);
		DeleteSharedDll(csPath);
		ppName++;
	}
#endif
	return TRUE;
}

/*
// Delete the main program directory.
// This will include deleting the EXE file that we belong to.
*/

BOOL CUninstallApp::DeleteProgramDirectory(BOOL fCheckForProgramFiles)
{
	if (m_fWorkstationUninstall)
	{
		// Workstation uninstall does not the program.
		return TRUE;
	}

/*
// Now delete the directory.
*/

	CString csMessage;
	TRY
	{
		csMessage.LoadString(IDS_REMOVING_PRINTMASTER_DIRECTORIES);
		GetConfiguration()->ReplaceText(csMessage);
	}
	END_TRY

	SetProgress(csMessage, "");

	return DeleteDirectory(m_csWorkingDirectory, fCheckForProgramFiles);
}

/*
// Remove us from the registry's uninstall menu.
*/

BOOL CUninstallApp::RemoveUninstall(void)
{
#ifdef WIN32
	CString csMessage;
	TRY
	{
		csMessage.LoadString(IDS_REMOVING_UNINSTALL_FROM_REGISTRY);
	}
	END_TRY

	SetProgress(csMessage, "");

/*
// Remove the registry uninstall key pointing to us.
*/
	CString csKey = REGSTR_PATH_UNINSTALL "\\";
	csKey += m_csRegistryName;
	csKey += ' ';
	csKey += m_csRegistryVersion;
	DeleteKey(HKEY_LOCAL_MACHINE, csKey);
#endif
	return TRUE;
}

/*
// Delete a directory, including all files and subdirectories.
*/

BOOL CUninstallApp::DeleteDirectory(LPCSTR pDirectory, BOOL fCheckForProgramFiles)
{
	SetItem(pDirectory);

	CString csProgramFile;
	CStringArray csaSkipFiles;

	csaSkipFiles.RemoveAll();

	if (fCheckForProgramFiles)
	{
		TRY
		{
			for (int nProgramFile = 0; nProgramFile < NUMBER_OF_UNINSTALL_PROGRAM_FILES; nProgramFile++)
			{
				csProgramFile = m_csWorkingDirectory;
				Util::AppendBackslashToPath(csProgramFile);
				csProgramFile += pszUninstallProgramFiles[nProgramFile];
				csaSkipFiles.Add(csProgramFile);
			}
		}
		END_TRY
	}

	CString csMessage;
	CString csDrive;

	if ((!Util::DirectoryDrive(pDirectory, csDrive))
	 || (csDrive == pDirectory)
	 || ((csDrive + '\\') == pDirectory))
	{
		TRY
		{
			csMessage.LoadString(IDS_BAD_DIRECTORY);
		}
		END_TRY

		Util::MessageBox(MB_OK | MB_ICONSTOP, (UINT)-1, csMessage, pDirectory);
		return FALSE;
	}

	CFileIterator It;

	CString csSearchPath;
	// Do we use "*.*" under NT or "*"?

	Util::ConstructPath(csSearchPath, pDirectory, "*.*");

	for (BOOL fRet = It.FindFirst(csSearchPath, _A_NORMAL | _A_SUBDIR | _A_HIDDEN); fRet; fRet = It.FindNext())
	{
		CString csPath;
		Util::ConstructPath(csPath, pDirectory, It.Name());
//		TRACE("Found file %s (%08lx)\n", (LPCSTR)It.Name(), It.Attributes());

		if (It.Attributes() & _A_SUBDIR)
		{
			if ((strcmp(It.Name(), ".") != 0)
			 && (strcmp(It.Name(), "..") != 0))
			{
				// This is a subdirectory.
				if (!DeleteDirectory(csPath, fCheckForProgramFiles))
				{
					return FALSE;
				}
			}
		}
		else
		{
			// This is a file.
			BOOL fSkipFile = FALSE;

			for (int nSkipFile = 0; nSkipFile < csaSkipFiles.GetSize(); nSkipFile++)
			{
				if (csaSkipFiles[nSkipFile].CompareNoCase(csPath) == 0)
				{
					fSkipFile = TRUE;
					break;
				}
			}

			if (!fSkipFile)
			{
				TRY
				{
					CFileStatus FileStatus;
					if (CFile::GetStatus(csPath, FileStatus))
					{
						if ((FileStatus.m_attribute & CFile::readOnly) != 0)
						{
							TRACE("Make file %s writable\n", (LPCSTR)csPath);
							FileStatus.m_attribute &= ~CFile::readOnly;
							CFile::SetStatus(csPath, FileStatus);
						}
					}
				}
				END_TRY

				TRY
				{
					TRACE("Delete file %s\n", (LPCSTR)csPath);
					CFile::Remove(csPath);
				}
				CATCH_ALL(e)
				{
					TRY
					{
						csMessage.LoadString(IDS_CANT_REMOVE_FILE);
					}
					END_TRY

					Util::MessageBox(MB_OK | MB_ICONEXCLAMATION, (UINT)-1, csMessage, (LPCSTR)csPath);
					return FALSE;
				}
				END_CATCH_ALL
			}
		}
	}

	It.Close();

// NT won't delete a directory if someone's using it.
	CString csRootDirectory = csDrive;
	Util::AppendBackslashToPath(csRootDirectory);
#ifdef WIN32
	SetCurrentDirectory(csRootDirectory);
#else
	chdir(csRootDirectory);
#endif

// And finally, remove the directory itself.
	CString csDirectory = pDirectory;
	Util::RemoveBackslashFromPath(csDirectory);

	if (fCheckForProgramFiles)
	{
		return TRUE;
	}

	TRACE("Delete directory %s\n", (LPCSTR)csDirectory);
 #ifdef WIN32
	if (RemoveDirectory(csDirectory))
 #else
	if (rmdir(csDirectory) == 0)
 #endif
	{
		return TRUE;
	}

	TRY
	{
		csMessage.LoadString(IDS_CANT_REMOVE_DIRECTORY);
	}
	END_TRY
	Util::MessageBox(MB_OK | MB_ICONEXCLAMATION, (UINT)-1, csMessage, (LPCSTR)csDirectory,
#ifdef WIN32
					  GetLastError());
#else
					  errno);
#endif

	return FALSE;
}

#ifdef WIN32
/*
// Can a key be deleted?
*/

BOOL KeyCanBeDeleted(HKEY hRoot, LPCSTR pSubKey)
{
/*
// Try to open the key.
// If this fails, we assume the key doesn't exist.
*/

	HKEY hKey;

	if (::RegOpenKeyEx(hRoot, pSubKey, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
	{
	/* We won't do anything with this. */
		return FALSE;
	}

	BOOL fDelete = FALSE;
	char ClassName[_MAX_PATH] = "";	// Buffer for class name.
	DWORD dwcClassLen = _MAX_PATH;		// Length of class string.
	DWORD dwcSubKeys;						// Number of sub keys.
	DWORD dwcMaxSubKey;					// Longest sub key size.
	DWORD dwcMaxClass;					// Longest class string.
	DWORD dwcValues;						// Number of values for this key.
	DWORD dwcMaxValueName;				// Longest Value name.
	DWORD dwcMaxValueData;				// Longest Value data.
	DWORD dwcSecDesc;						// Security descriptor.
	FILETIME ftLastWriteTime;			// Last write time.

	if (::RegQueryInfoKey(hKey,
								 ClassName,
								 &dwcClassLen,
								 NULL,
								 &dwcSubKeys,
								 &dwcMaxSubKey,
								 &dwcMaxClass,
								 &dwcValues,
								 &dwcMaxValueName,
								 &dwcMaxValueData,
								 &dwcSecDesc,
								 &ftLastWriteTime) == ERROR_SUCCESS)
	{
		fDelete = (dwcSubKeys == 0);
	}

	::RegCloseKey(hKey);
	return fDelete;
}

/*
// Delete a key from the registry.
// This will delete all subkeys of the key. It will also "delete upwards"
// (pruning empty key trees) if desired.
*/

BOOL CUninstallApp::DeleteKey(HKEY hRoot, LPCSTR pSubKey, int nDeleteUp /*=0*/)
{
/*
// Try to open the key.
// If this fails, we assume the key doesn't exist.
*/

	HKEY hKey;

	if (::RegOpenKeyEx(hRoot, pSubKey, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
	{
		/* Doesn't exist? */
		return TRUE;
	}

	DWORD dwSubKey = 0;			/* Always delete 0'th item. */
	char cbName[_MAX_PATH+1];

	while (::RegEnumKey(hKey, dwSubKey, cbName, sizeof(cbName)) == ERROR_SUCCESS)
	{
		CString csSubPath = pSubKey;
		csSubPath += '\\';
		csSubPath += cbName;

		if (!DeleteKey(hRoot, csSubPath))
		{
			::RegCloseKey(hKey);
			return FALSE;
		}
	}

	::RegCloseKey(hKey);

/*
// Now delete the key in question.
*/

	if (::RegDeleteKey(hRoot, pSubKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

/*
// See if we need to delete upwards (to remove empty subtrees).
*/

	if (nDeleteUp > 0)
	{
		CString csPath = pSubKey;
		int nIndex;
		while (nDeleteUp-- > 0 && (nIndex = csPath.ReverseFind('\\')) > 0)
		{
			csPath = csPath.Left(nIndex);

			if (!KeyCanBeDeleted(hRoot, csPath))
			{
				break;
			}

			if (::RegDeleteKey(hRoot, csPath) != ERROR_SUCCESS)
			{
			/* Could not delete the key. It probably has sub keys. Stop now. */
				break;
			}
		}
	}
	return TRUE;
}

DWORD CUninstallApp::DeleteSharedDll(LPCSTR pszName)
{
	TRACE("Delete shared DLL '%s'\n", pszName);
	CString csKey;
	DWORD dwCount = 0;
	HKEY hKey = NULL;
	DWORD dwDisposition = 0;

	TRY
	{
		csKey = REGSTR_PATH_SETUP "\\SharedDLLs";

		if (RegOpenKeyEx(
					HKEY_LOCAL_MACHINE,
					csKey,
					0,
					KEY_ALL_ACCESS,
					&hKey) == ERROR_SUCCESS)
		{
			// The key has been opened.
			DWORD dwType;
			DWORD dwSize;

			dwCount = 0;
			dwSize = sizeof(dwCount);

		/*
		// Read the current value for this key.
		*/

			LONG lResult = RegQueryValueEx(
										hKey,
										pszName,
										0,
										&dwType,
										(BYTE*)&dwCount,
										&dwSize);

			BOOL fWriteNewCount = FALSE;

			if (lResult == ERROR_SUCCESS)
			{
			/*
			// We got a value. See if it is one we can decrement.
			*/

				fWriteNewCount = FALSE;

				if ((dwType == REG_BINARY) || (dwType == REG_DWORD))
				{
					if (dwSize == sizeof(dwCount))
					{
					/* We got a count to play with. */
						if (dwCount <= 1)			/* In case it's 0. */
						{
						/*
						// Ask the user if deletion is desired.
						*/
							CSharedDllDialog Dialog(pszName);

							if (Dialog.DoModal() == IDOK)
							{
								TRY
								{
								/* Delete the file from the hard disk. */
									TRACE("Removed shared DLL %s\n", pszName);
//									CFile::Remove(pszName);

								/* Delete the key from the registry. */

									::RegDeleteValue(hKey, pszName);
								}
								CATCH_ALL(e)
								{
									UninstMessageBox(IDS_CANT_REMOVE_FILE);
								}
								END_CATCH_ALL
							}
						}
						else
						{
						/*
						// Decrement the count and write it back to the registry.
						*/
							dwCount--;
							RegSetValueEx(
											  hKey,
											  pszName,
											  0,
											  dwType,
											  (const BYTE *)&dwCount,
											  dwSize);
						}
					}
				}
			}

		/*
		// We're now done with the key.
		*/

			RegCloseKey(hKey);
			hKey = NULL;
		}
	}
	END_TRY

	TRACE("Final count is %ld\n", dwCount);

	return dwCount;
}
#endif

/*
// Parse a parameter in a command line. A Parameter always begins
// at the start of the command line or after a space. No spaces
// are allowed within parameters, but quoted strings are handled
// properly and may contain spaces. All comparisons are case-insensitive.
*/

LPSTR CUninstallApp::LocateParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
	LPSTR lpszResult = NULL;
	
	LPSTR lpszCommandLine = m_lpCmdLine;
	int nParamNameLength = lstrlen(lpszParamName);
	int nCommandLineLength = lstrlen(lpszCommandLine);
	BOOL fInQuote = FALSE;
	BOOL fStartOfParameter = TRUE;
	BOOL fFoundParameter = FALSE;
	
	while ((!fFoundParameter) && (nCommandLineLength >= nParamNameLength))
	{
		if (fStartOfParameter)
		{
			// A parameter can start here, check for a match.
			
			int i = 0;
			
			for (;;)
			{
				char ch1 = lpszCommandLine[i];
				if ((ch1 > 'a') && (ch1 < 'z')) ch1 = (char)(ch1+'A'-'a');
				
				char ch2 = lpszParamName[i];
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
			if (*lpszCommandLine == ' ')
			{
				fStartOfParameter = !fInQuote;
			}
			
			else if (*lpszCommandLine == '"')
			{
				fInQuote = !fInQuote;
			}
			
			lpszCommandLine++;
			nCommandLineLength--;
		}
	}
	
	if (fFoundParameter)
	{
		lpszResult = lpszCommandLine;
		
		if (lpszValue != NULL)
		{
			*lpszValue = 0;
			
			lpszCommandLine += nParamNameLength;

			if (*lpszCommandLine == '=')
			{
				lpszCommandLine++;
			}
			
			fInQuote = FALSE;
			
			for (;;)
			{
				if (*lpszCommandLine == 0)
				{
					break;
				}
				
				if ((!fInQuote) && (*lpszCommandLine == ' '))
				{
					break;
				}
				
				if (*lpszCommandLine == '"')
				{
					fInQuote = !fInQuote;
				}
				
				else
				{
					*(lpszValue++) = *lpszCommandLine;
				}
				
				lpszCommandLine++;
			}
			
			*lpszValue = 0;
		}
	}
	
	return lpszResult;
}

BOOL CUninstallApp::FindParameter(LPSTR lpszParamName)
{
	return LocateParameter(lpszParamName, NULL) != NULL;
}

LPSTR CUninstallApp::GetParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
	if (LocateParameter(lpszParamName, lpszValue) == NULL)
	{
		return NULL;
	}
	
	else
	{
		return lpszValue;
	}
}

void CUninstallApp::SetProgress(LPCSTR pszAction /*=NULL*/, LPCSTR pszItem /*=NULL*/)
{
	if (m_pProgressDialog != NULL)
	{
		m_pProgressDialog->SetProgress(pszAction, pszItem);
	}
}

void CUninstallApp::SetAction(LPCSTR pszAction /*=NULL*/)
{
	if (m_pProgressDialog != NULL)
	{
		m_pProgressDialog->SetAction(pszAction);
	}
}

void CUninstallApp::SetItem(LPCSTR pszItem /*=NULL*/)
{
	if (m_pProgressDialog != NULL)
	{
		m_pProgressDialog->SetItem(pszItem);
	}
}

void CUninstallApp::ShowProgress(void)
{
	if (m_pProgressDialog != NULL)
	{
		m_pProgressDialog->ShowWindow(SW_SHOW);
	}
}

void CUninstallApp::HideProgress(void)
{
	if (m_pProgressDialog != NULL)
	{
		m_pProgressDialog->ShowWindow(SW_HIDE);
	}
}


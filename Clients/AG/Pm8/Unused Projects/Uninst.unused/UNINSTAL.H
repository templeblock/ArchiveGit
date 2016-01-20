// Uninstall.h : main header file for the UNINSTALL application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "inifile.h"
#include "strclass.h" 
#include "instdat.h"

class CUninstallProgressDialog;

/////////////////////////////////////////////////////////////////////////////
// CUninstallApp:
// See Uninstall.cpp for the implementation of this class
//

int UninstMessageBox( UINT nID, UINT nType = MB_OK);
int UninstMessageBox( CString csMessage, UINT nType = MB_OK);

class CUninstallApp : public CWinApp
{
public:
	CUninstallApp();
	virtual ~CUninstallApp();

	void Uninstall(void);

   LPCSTR
   GetResourceStringPointer (UINT id)
   {
      return m_ResourceString.GetPointer (id);
   }
	InstallationInfo m_installation_info;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUninstallApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUninstallApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CString m_csWorkingDirectory;		// Installed directory
	CString m_csHomeDirectory;			// On CD.
	BOOL m_fDemoVersion;
	BOOL m_fWorkstationUninstall;

	LPCSTR m_signature_file;
	BOOL ReadInstDat(void);
	BOOL ConfigurationError(void);
	CResourceString   m_ResourceString;
	CIniFile m_MsregIniFile;
	CString m_csRegistryName;
	CString m_csRegistryVersion;
	CString m_csRegistryCompany;

	void ComputeHomeDirectory(void);

	BOOL DeleteGroup(void);
	void HandleOldGroup(LPCSTR pszGroupName, BOOL fDeleteIfEmpty = TRUE);
	BOOL UpdateRegistry(void);
	BOOL DeleteIdapiDirectories(void);
	BOOL DeleteSharedDlls(void);
	BOOL DeleteProgramDirectory(BOOL fCheckForProgramFiles);
	BOOL RemoveUninstall(void);

	void GetStartupGroupName(CString& csName);

	BOOL DeleteDirectory(LPCSTR pDirectory, BOOL fCheckForProgramFiles);
	BOOL DeleteKey(HKEY hRoot, LPCSTR pSubKey, int nDeleteUp = 0);
	DWORD DeleteSharedDll(LPCSTR pszName);

	LPSTR LocateParameter(LPSTR lpszParamName, LPSTR lpszValue);
	BOOL FindParameter(LPSTR lpszParamName);
	LPSTR GetParameter(LPSTR lpszParamName, LPSTR lpszValue);

protected:
	CUninstallProgressDialog* m_pProgressDialog;
	void SetProgress(LPCSTR pszAction = NULL, LPCSTR pszItem = NULL);
	void SetAction(LPCSTR pszAction = NULL);
	void SetItem(LPCSTR pszItem = NULL);
	void ShowProgress(void);
	void HideProgress(void);
};


/////////////////////////////////////////////////////////////////////////////

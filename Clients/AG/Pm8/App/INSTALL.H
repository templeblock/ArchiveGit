/*
// $Workfile: INSTALL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/INSTALL.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 10    7/07/98 11:07a Rlovejoy
// Added DeleteOldContent() to remove v4.0 content files.
// 
// 9     7/06/98 2:03p Mwilson
// changed to read ini file for exe and icon file name.  Defaults to
// pmw.exe and pmg.ico
// 
// 8     6/24/98 8:26a Jayn
// 
// 7     6/10/98 4:45p Jayn
// AT&T install
// 
// 6     6/06/98 3:20p Fredf
// 
// 5     4/13/98 10:33a Jayn
// Changes for DrawPlus installation.
// 
// 4     4/08/98 12:34p Fredf
// New explorer progress indicator.
// 
// 3     3/26/98 10:29a Fredf
// Changes to install program to install Internet Explorer 4.0 and the Web
// Publishing Wizard.
// 
// 2     1/21/98 4:50p Fredf
// Year 2000 compliance.
// 
//    Rev 1.1   17 Sep 1997 09:12:48   Jay
// No more build.id
// 
//    Rev 1.0   14 Aug 1997 15:21:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:16   Fred
// Initial revision.
// 
//    Rev 1.8   02 Jun 1997 10:44:40   Fred
// Changes for 4.0 install
// 
//    Rev 1.7   21 Apr 1997 16:01:50   Jay
// 4.0 changes
// 
//    Rev 1.6   12 Dec 1996 13:16:36   Fred
// Quick hacks to install 4.0
// 
//    Rev 1.5   01 Nov 1996 09:28:02   Jay
// Moved over from 3.01.17
// 
//    Rev 1.6   29 Oct 1996 16:35:16   Fred
// Twain Tweaks
// 
//    Rev 1.5   29 Oct 1996 15:25:50   Fred
// Twain install
// 
//    Rev 1.4   20 Aug 1996 18:21:02   Fred
// AT&T Installation
// 
//    Rev 1.3   18 Jul 1996 17:34:12   Fred
// Some improvements for PMW 3.0
// 
//    Rev 1.2   18 Jul 1996 14:48:20   Fred
// PMW 3 changes
// 
//    Rev 1.1   07 May 1996 12:43:54   Jay
// From PMW2
// 
//    Rev 2.22   26 Apr 1996 18:09:42   FRED
// More demo stuff
// 
//    Rev 2.21   25 Apr 1996 14:47:10   JAY
// Now handles sound state correctly.
// 
//    Rev 2.20   24 Apr 1996 16:43:46   JAY
// New upgradable demo support.
// 
//    Rev 2.19   19 Apr 1996 15:49:58   FRED
// Removes old group
// 
//    Rev 2.18   19 Apr 1996 10:05:22   JAY
// Now suuports previous installation directory.
// 
//    Rev 2.17   19 Apr 1996 08:53:42   JAY
// New old/new version/level logic for autorun installations.
// 
//    Rev 2.16   03 Apr 1996 12:50:44   JAY
// Now touches a directory before going past it.
// 
//    Rev 2.15   12 Feb 1996 14:17:22   JAY
// Unregisters use if previous installation was of lesser level.
// 
//    Rev 2.14   12 Feb 1996 10:36:30   JAY
// Upgrade Icons
// 
//    Rev 2.13   31 Jan 1996 10:29:50   FRED
// New workstation install
// 
//    Rev 2.12   29 Jan 1996 16:53:28   JAY
// Variable configuration in INSTALL and PMW.
// 
//    Rev 2.11   26 Jan 1996 15:09:32   JAY
//  
// 
//    Rev 2.10   19 Jan 1996 09:37:38   JAY
// Changes for the demo version.
// 
//    Rev 2.9   05 Dec 1995 09:40:56   JAY
// Added sound command lines arguments.
// 
//    Rev 2.8   04 Dec 1995 17:09:26   JAY
// Now checks to see if any OLE (group) files are in use and won't install any
// unless all are not in use.
// 
//    Rev 2.7   20 Nov 1995 11:19:02   JAY
// Now deletes VFLs before installing.
// 
//    Rev 2.6   15 Nov 1995 14:40:18   FRED
//  
// 
//    Rev 2.5   27 Aug 1995 18:30:50   FRED
// Changes to make install program not reference OLE
// 
//    Rev 2.4   09 Aug 1995 08:35:10   FRED
// Hookup with INSTICFG.DLL
// 
//    Rev 2.3   08 Aug 1995 09:00:56   FRED
// Publishing Suite Installation
// 
//    Rev 2.2   07 Aug 1995 17:11:30   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:39:36   JAY
// Reverted. New series.
// 
//    Rev 1.11   18 Jul 1994 16:08:06   FRED
// Added "Audio Guide" dialog.
// 
// 
//    Rev 1.10   15 Jul 1994 10:39:56   JAY
// Support for monochrome sound buttons.
// 
//    Rev 1.9   24 Jun 1994 14:14:46   JAY
// Added define for MUTIMEDIA.
// 
//    Rev 1.8   23 Jun 1994 13:26:44   FRED
// Main application is started after CTL3D is shut down.
// If not, Windows reports lost bitmaps and brushes.
// 
// 
//    Rev 1.7   23 Jun 1994 11:59:28   FRED
// Build ID is now enumerated in CInstallApp class.
// 
// 
//    Rev 1.6   22 Jun 1994 13:45:46   FRED
// Fixed problem with CheckForPreviousInstance() trying
// to load undefined string resource. The program title
// is now passed in.
// 
// InstallProgram() now disables the main (gradient) window
// when the progress dialog is showing.
// 
//    Rev 1.5   22 Jun 1994 08:54:46   FRED
// Uses CControl3d.
// Better error trapping.
// 
// 
//    Rev 1.4   21 Jun 1994 14:42:06   FRED
// Does version checking.
// 
// 
//    Rev 1.3   17 Jun 1994 16:59:50   FRED
// Writes installation data.
// Other goodies.
// 
// 
//    Rev 1.2   16 Jun 1994 18:13:32   FRED
// Version that copies files.
// 
// 
//    Rev 1.1   15 Jun 1994 18:58:32   FRED
// No change.
// 
//    Rev 1.0   15 Jun 1994 17:13:24   FRED
// Initial revision.
*/ 

#ifndef _INSTALL_H_
#define _INSTALL_H_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "cpfl.h"
#include "instdat.h"
#include "iprogres.h"
#include "inifile.h"
#include "strclass.h"
#include "gradient.h"

#define	MINIMUM_WORKSPACE_SIZE	( 1000000L)
#define	MAXIMUM_WORKSPACE_SIZE	(99000000L)
#define	DEFAULT_WORKSPACE_SIZE	( 5000000L)

#define	MAKE_DOUBLE(l) (((double)(((l)+5000L)/10000L))/100.0)

class CInstallApp;

#define	GetInstallApp()	((CInstallApp*)AfxGetApp())

struct CProductInfo
{
	// Product level.
	int m_nInstallConfiguration;
	int m_nCurrentConfiguration;

	// Product version.
	int m_nVersionMajor;
	int m_nVersionMinor;
	int m_nRevision;

	BOOL operator > (const CProductInfo& OtherInfo);
	BOOL operator == (const CProductInfo& OtherInfo);
	BOOL operator < (const CProductInfo& OtherInfo);
	BOOL operator != (const CProductInfo& OtherInfo)
		{ return !(*this == OtherInfo); }
};

/////////////////////////////////////////////////////////////////////////////
// CInstallMainFrame

class CInstallMainFrame : public CGradientWindow
{
// Construction
public:

// Attributes
public:

// Operations
public:

// Implementation
public:

protected:
	// Generated message map functions
	//{{AFX_MSG(CInstallMainFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CInstallApp:
// See install.cpp for the implementation of this class
//

class CInstallApp : public CWinApp
{
public:
	CInstallApp();

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance(void);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

// Implementation

	//{{AFX_MSG(CInstallApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	afx_msg BOOL OnQueryEndSession();
	DECLARE_MESSAGE_MAP()

public:	
	CProductInfo* GetInstallInfo(void)
		{ return &m_InstallInfo; }
	CProductInfo* GetPreviousInfo(void)
		{ return &m_PreviousInfo; }
	BOOL DemoCanBeInstalledIntoDirectory(LPCSTR pszDirectory);

	// Make sure a CD is inserted.
	char InsertCD(LPCSTR pszVolumeLabel, LPCSTR pszCDName);

protected:

	char FindCD(LPCSTR pszVolumeLabel);
	BOOL PromptForCD(LPCSTR pszCDName);

	BOOL DetermineConfiguration(void);
	BOOL CheckForPreviousInstance(LPCSTR lpzsTitle);
	BOOL GetInstallationParameters(CWnd *pcwndParent);
	void DeleteOldVFLs(void);
	void DeleteOldContent(void);
	void InstallProgram(CWnd *pcwndParent);
	void WriteInstallationInformation(CWnd *pcwndParent, CInstallProgressDialog *pProgressDlg);
	void CopyFiles(CWnd *pcwndParent, CInstallProgressDialog *pProgressDlg);
	void CopyFile(const CProgramFile *pProgramFile, CWnd *pcwndParent, CInstallProgressDialog *pProgressDlg);
	void WriteIniEntries(CWnd *pcwndParent, CInstallProgressDialog *pProgressDlg);
	void MakeProgramManagerGroup(CInstallProgressDialog *pProgressDlg = NULL);
	void UpdateRegistry(void);
	void ConvertOldAddressBooks(CInstallProgressDialog *pProgressDlg);
	BOOL OldAddressBooksExist(void);
	void HandleOldGroups(void);
	void HandleOldGroup(LPCSTR pszGroupName);
	BOOL FileSharingAvailable(CWnd *pcwndParent);
	void InstallIsp(CWnd *pcwndParent);
	void SetOleInUse(void);
	BOOL FileInUse(const CProgramFile *pProgramFile);
	CString GetAppPathKey(void);
	CString GetExeName(void);
	CString GetUninstallKey(void);
	CString GetMsregUsrIniFile(void);
	void RegisterPrintMasterAutomation(void);
	void InstallDrawPlus(CWnd* pParent);
	void InstallArtStore(CWnd* pParent);

	void GetVersion(CProductInfo* pInfo, CIniFile& IniFile);
	void GetVersion(CProductInfo* pInfo, LPCSTR pFileName);

	LPSTR GetParameter(LPSTR lpszParamName, LPSTR lpszValue);
	BOOL FindParameter(LPSTR lpszParamName);
	LPSTR LocateParameter(LPSTR lpszParamName, LPSTR lpszValue);

	void UpgradeIcons(LPCSTR pszUpgradeDirectory);

	void TouchSourceDirectory(const CString& csFileName);
	void TouchDirectory(const CString& csDirectory);

private:
	CString m_csInstallDirectory;
	DWORD m_dwWorkspaceSize;
	BOOL m_fInstallIsp;
	BOOL m_fTrapHelpMessages;
	BOOL m_fMonochrome;
	BOOL m_fCallRegEdit;
	BOOL m_fOleInUse;
	CIniFile m_MsregIniFile;
	BOOL m_fForceSound8;
	BOOL m_fForceSoundOff;
	BOOL m_fMustReboot;

	CString m_csApplicationName;
	CString m_csRegistryName;
	CString m_csRegistryVersion;
	CString m_csRegistryCompany;

	CString m_csLastSourceDirectory;


	// Previous installation information.
	CProductInfo m_PreviousInfo;
	CString m_csPreviousInstallDirectory;

	// Current installation information.
	CProductInfo m_InstallInfo;

	BOOL m_fWorkstationInstall;			// TRUE if workstation installation parameter (/N) was passed
	BOOL m_fTwainInstall;					// TRUE if Twain installation parameter (/T) was passed

	HINSTANCE         m_InstDLL;
   CResourceString   m_ResourceString;

	CString m_csSourceDirectory;
	CString m_csProgramName;
	CProgramFileList m_ProgramFileList;

public:
	// Return a pointer to a resource string.
	// The return type is LPCSTR so that the data can not be modified
	// through the returned pointer.
   LPCSTR
   GetResourceStringPointer (UINT id)
   {
      LPCSTR pszString = m_ResourceString.GetPointer (id);
		return pszString;
   }

public:
	const CString &GetSourceDirectory(void);
	const CProgramFileList &GetProgramFileList(void);
	DWORD GetProgramSize(const CString &csDirectory);
	DWORD GetRecommendedWorkspaceSize(const CString &csDirectory);
};


/////////////////////////////////////////////////////////////////////////////

UINT GetFileExceptionCauseStringID(CFileException *e);
struct InstallationInfo &GetUserInformation(CWnd *pcwndParent);

#endif		// ifndef _INSTALL_H_

/*
// $Workfile: MSREGDLL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSREGDLL.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   05 Dec 1995 12:51:20   JAY
// 
//    Rev 1.0   22 Nov 1995 08:51:56   FRED
// Initial revision.
// 
//    Rev 1.4   31 Oct 1995 12:01:48   FRED
//  
// 
//    Rev 1.3   26 Oct 1995 10:27:52   FRED
//  
*/ 

// MSREGDLL.h : main header file for the MSREGDLL DLL
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class CMsregDllApp;

#ifndef __MSREGDLL_H__
#define __MSREGDLL_H__

#include "resource.h"
#include "inifile.h"

#ifdef WIN32
#define REGEXPORT
#else
#define REGEXPORT _export
#endif

/*
// Define alternate entry point protocol for dll itself.
*/

#define REGENTRYPOINT	REGEXPORT PASCAL FAR

#include "msreg.h"

/////////////////////////////////////////////////////////////////////////////
// CMsregDllApp
// See MSREGDLL.cpp for the implementation of this class
//

class CMsregDllApp : public CWinApp
{
public:
	CMsregDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsregDllApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMsregDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CIniFile m_IniFile;
	CIniFile m_UserIniFile;
	CString m_csWorkingDirectory;
	CString m_csApplicationName;

	BOOL SetupApplicationName(void);
	BOOL SetupWorkingDirectory(void);
	BOOL SetupIniFile(void);
public:
	/*
	// Get the IniFile.
	*/

	CIniFile& IniFile(void)
		{ return m_IniFile; }

	/*
	// Get the user data IniFile.
	*/

	CIniFile& UserIniFile(void)
		{ return m_UserIniFile; }

	/*
	// Get the working directory.
	*/

	const CString& WorkingDirectory(void) const
		{ return m_csWorkingDirectory; }

	/*
	// Get the application name.
	*/

	const CString& ApplicationName(void) const
		{ return m_csApplicationName; }
};

/*
// A little helper function to get the app.
*/

#define GetApp()		((CMsregDllApp*)AfxGetApp())

/////////////////////////////////////////////////////////////////////////////

#endif

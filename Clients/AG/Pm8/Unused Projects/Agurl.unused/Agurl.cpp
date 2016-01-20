/*
// $Workfile: Agurl.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/Agurl/Agurl.cpp $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 1     7/06/98 8:55a Mwilson
// 
//    Rev 1.0   14 Aug 1997 15:39:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:28:36   Fred
// Initial revision.
// 
//    Rev 1.0   20 Aug 1996 18:06:28   Fred
// Initial revision.
*/

 
#include "stdafx.h"
#include "Agurl.h"
#include "urlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgurlApp

BEGIN_MESSAGE_MAP(CAgurlApp, CWinApp)
	//{{AFX_MSG_MAP(CAgurlApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgurlApp construction

CAgurlApp::CAgurlApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAgurlApp object

CAgurlApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAgurlApp initialization

BOOL CAgurlApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef WIN32
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#else
	SetDialogBkColor();
#endif

	char szPath[_MAX_PATH];
	if (::GetModuleFileName(AfxGetInstanceHandle(), szPath, sizeof(szPath)-1) > 0)
	{
		char *pEnd = strrchr(szPath, '\\');
		if (pEnd != NULL)
		{
			strcpy(pEnd, "\\AG.URL");
			TRY
			{
				m_csUrlFile = szPath;
			}
			END_TRY
		}
	}

	CAgurlDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

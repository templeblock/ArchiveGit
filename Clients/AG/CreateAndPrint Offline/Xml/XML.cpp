// XML.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XML.h"
#include "XMLDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CXMLApp

BEGIN_MESSAGE_MAP(CXMLApp, CWinApp)
	//{{AFX_MSG_MAP(CXMLApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXMLApp construction

CXMLApp::CXMLApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXMLApp object

CXMLApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXMLApp initialization

BOOL CXMLApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

	CXMLDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else
	if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

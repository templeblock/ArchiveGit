// PrintDLL.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PrintDLL.h"
#include "PrintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintDLL

BEGIN_MESSAGE_MAP(CPrintDLL, CWinApp)
	//{{AFX_MSG_MAP(CPrintDLL)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintDLL construction

CPrintDLL::CPrintDLL()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPrintDLL object

CPrintDLL theApp;

extern "C" __declspec(dllexport) int WINAPI PrintFile (LPCSTR pFilePath, LPCSTR pProductCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPrintDlg dlg(pFilePath, pProductCode);

	return dlg.DoModal();
}
 

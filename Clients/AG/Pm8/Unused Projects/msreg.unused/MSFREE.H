// msfree.h : main header file for the MSFREE application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMsfreeApp:
// See msfree.cpp for the implementation of this class
//

class CMsfreeApp : public CWinApp
{
public:
	CMsfreeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsfreeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMsfreeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	BOOL SetWorkingVariables(void);
	void InstallFreeBonus(void);
	LPSTR LocateParameter(LPCSTR lpszParamName, CString* pcsValue);
	BOOL GetParameter(LPCSTR lpszParamName, CString* pcsValue = NULL);
};


/////////////////////////////////////////////////////////////////////////////

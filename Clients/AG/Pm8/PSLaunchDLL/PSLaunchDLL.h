// PSLaunchDLL.h : main header file for the PSLAUNCHDLL DLL
//

#if !defined(AFX_PSLAUNCHDLL_H__583611E1_5FA6_11D2_9B90_00A0C99F6B3C__INCLUDED_)
#define AFX_PSLAUNCHDLL_H__583611E1_5FA6_11D2_9B90_00A0C99F6B3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <ddeml.h>

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchDLLApp
// See PSLaunchDLL.cpp for the implementation of this class
//

class CPSLaunchDLLApp : public CWinApp
{
public:
	CPSLaunchDLLApp();

// IClassFactory
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSLaunchDLLApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	CString FindModulePath();
	DWORD	GetDDEInstance() { return m_dwDDEInstance; }
	void	SetPrintFormat( short sPaper, short sOrient );

	//{{AFX_MSG(CPSLaunchDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	static HDDEDATA CALLBACK DdeCallback( UINT uType, UINT uFmt, HCONV hconv, 
										  HSZ hsz1, HSZ hsz2, HDDEDATA hdata, 
										  DWORD dwData1, DWORD dwData2 );

private:

	DWORD	m_dwDDEInstance;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSLAUNCHDLL_H__583611E1_5FA6_11D2_9B90_00A0C99F6B3C__INCLUDED_)

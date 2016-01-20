// PrintDLL.h : main header file for the PRINTDLL DLL
//

#if !defined(AFX_PRINTDLL_H__1BC12965_FEB3_11D1_B06E_00A024EA69C0__INCLUDED_)
#define AFX_PRINTDLL_H__1BC12965_FEB3_11D1_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CPrintDLL
// See Temp.cpp for the implementation of this class
//

class CPrintDLL : public CWinApp
{
public:
	CPrintDLL();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintDLL)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPrintDLL)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#ifdef EXPORT
extern "C" __declspec(dllexport) int WINAPI PrintFile (LPCSTR pFilePath, LPCSTR pProductCode);
#else
extern "C" __declspec(dllimport) int WINAPI PrintFile (LPCSTR pFilePath, LPCSTR pProductCode);

#endif



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTDLL_H__1BC12965_FEB3_11D1_B06E_00A024EA69C0__INCLUDED_)

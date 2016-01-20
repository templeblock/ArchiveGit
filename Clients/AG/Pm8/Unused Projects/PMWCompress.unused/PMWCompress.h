// PMWCompress.h : main header file for the PMWCOMPRESS application
//

#if !defined(AFX_PMWCOMPRESS_H__2A823AE5_C7DE_11D1_8680_0060089672BE__INCLUDED_)
#define AFX_PMWCOMPRESS_H__2A823AE5_C7DE_11D1_8680_0060089672BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define DEFAULT_W 450
#define DEFAULT_H 300

/////////////////////////////////////////////////////////////////////////////
// CPMWCompressApp:
// See PMWCompress.cpp for the implementation of this class
//

class CPMWCompressApp : public CWinApp
{
public:
	CPMWCompressApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPMWCompressApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPMWCompressApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PMWCOMPRESS_H__2A823AE5_C7DE_11D1_8680_0060089672BE__INCLUDED_)

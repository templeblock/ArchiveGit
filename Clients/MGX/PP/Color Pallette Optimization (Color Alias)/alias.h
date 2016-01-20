// alias.h : main header file for the ALIAS application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "quantize.h"
#include "dib.h"

/////////////////////////////////////////////////////////////////////////////
// CAliasApp:
// See alias.cpp for the implementation of this class
//

class CMyStatusDlg : public CDialog
{
public:
	CMyStatusDlg();

	void OnCancel();
	BOOL AbortTest();

public:
	BOOL m_fAbort;
};

class CAliasApp : public CWinApp
{
public:
	CAliasApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAliasApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAliasApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL ProcessFiles(LPSTR lpGrayDir, LPSTR lpColorDir, LPSTR lpCompDir, BOOL fCountFiles = FALSE);
	BOOL CreateAntiAliasedFile(LPCSTR lpForeFile, LPCSTR lpCompFile, LPCSTR lpDstFile, CWnd *pStatusCtrl);
	BOOL AntiAliasDib(PDIB pForeDib, PDIB pCompDib);
private:
	CQuantize 		m_Quantize;
	int				m_nTotal;
	int				m_nFiles;
	CMyStatusDlg 	m_statusDlg;
	RGBS			m_rgbTrans;
};


/////////////////////////////////////////////////////////////////////////////

// border.h : main header file for the BORDER DLL
//

#if !defined(AFX_BORDER_H__D4E817C6_85DA_11D1_8680_006008661BA9__INCLUDED_)
#define AFX_BORDER_H__D4E817C6_85DA_11D1_8680_006008661BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// border editor specific
#include "borddef.h"
#include "ifaced.h"
#include "Borddoc.h"

#define BE_VERSION 3

#define WM_DOREALIZE   (WM_USER + 0)

/////////////////////////////////////////////////////////////////////////////
// CBorderApp
// See border.cpp for the implementation of this class
//

class CBorderApp : public CWinApp
{
	CBECallbackInterface* m_pBECallbackInterface;
	HWND m_hParent;

public:
	CBorderApp();

	int InitApp(CBEObjectD* pBEObject, HWND hParent);
	int ExitApp();

// Attributes
	CBorddoc* GetDocument();
	void ShowWindow(int nCmdShow);
	void SetCallbacks(CBECallbackInterface* pBECallbackInterface);
	void ResizeBEWindow(double dXSize, double dYSize);
	int CreateBEInterface(CBEInterface*& pBEInterface);
	HPALETTE GetPalette(void);
	int DoModal();
	HWND GetParent()
	{
		return (m_hParent);
	}
	CBECallbackInterface* GetCallbacks()
	{
		return (m_pBECallbackInterface);
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBorderApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CBorderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// The one and only CBorderApp object

extern CBorderApp theBorderApp;

/////////////////////////////////////////////////////////////////////////////
// exported functions

extern "C" __declspec(dllexport)
int Initialize(CBECallbackInterface* pBECallbackInterface, CBEInterface*& pBEInterface);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BORDER_H__D4E817C6_85DA_11D1_8680_006008661BA9__INCLUDED_)
